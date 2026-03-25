#include "GpuRenderer.hpp"
#include "Window.hpp"

#include <iostream>

#include "SDL3/SDL_gpu.h"
	
static SDL_GPUSampler* Samplers[std::size(graphics::SamplerNames)];

graphics::GpuRenderer::GpuRenderer(Window& window)
	: window{window}
{
	//Create GPU

	SDL_GPUShaderFormat shader_formats
	{
		SDL_GPU_SHADERFORMAT_SPIRV |
		SDL_GPU_SHADERFORMAT_DXIL |
		SDL_GPU_SHADERFORMAT_MSL
	};

	device = std::shared_ptr<SDL_GPUDevice>{ SDL_CreateGPUDevice(shader_formats, false, nullptr), SDL_DestroyGPUDevice };

	if (!device)
	{
		throw std::runtime_error{ std::format("Could not create GPU device: {}", SDL_GetError()) };
	}

	std::cout << "GPU device created." << std::endl;
	std::cout << std::format("Using {} GPU implementation.", SDL_GetGPUDeviceDriver(device.get())) << std::endl;

	if (!SDL_ClaimWindowForGPUDevice(device.get(), window.get()))
	{
		throw std::runtime_error{ std::format("SDL_ClaimWindowForGPUDevice failed: {}", SDL_GetError()) };
	}
	else
	{
		window_claimer = std::make_unique<WindowClaimer>(device.get(), window.get());
	}

	std::cout << "Window was successfully claimed for current GPU device." << std::endl;

	// Disable VSync
	//SDL_SetGPUSwapchainParameters(device.get(), window.get(), SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_IMMEDIATE);

	// Init vertex shader
	vertex_shader = std::make_unique<GpuShader>(device, "shaders/compiled/TextureQuad.vert.spv", 0, 0);
	fragment_shader = std::make_unique<GpuShader>(device, "shaders/compiled/TextureQuad.frag.spv", 1, 0);

	std::cout << "Shaders initialized." << std::endl;

	// PointClamp
	SDL_GPUSamplerCreateInfo pointClamp{};
	pointClamp.min_filter = SDL_GPU_FILTER_NEAREST;
	pointClamp.mag_filter = SDL_GPU_FILTER_NEAREST;
	pointClamp.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
	pointClamp.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	pointClamp.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	pointClamp.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

	Samplers[0] = SDL_CreateGPUSampler(device.get(), &pointClamp);

	// PointWrap
	SDL_GPUSamplerCreateInfo pointWrap{};
	pointWrap.min_filter = SDL_GPU_FILTER_NEAREST;
	pointWrap.mag_filter = SDL_GPU_FILTER_NEAREST;
	pointWrap.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
	pointWrap.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	pointWrap.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	pointWrap.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;

	Samplers[1] = SDL_CreateGPUSampler(device.get(), &pointWrap);

	// LinearClamp
	SDL_GPUSamplerCreateInfo linearClamp{};
	linearClamp.min_filter = SDL_GPU_FILTER_LINEAR;
	linearClamp.mag_filter = SDL_GPU_FILTER_LINEAR;
	linearClamp.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
	linearClamp.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	linearClamp.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	linearClamp.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

	Samplers[2] = SDL_CreateGPUSampler(device.get(), &linearClamp);

	// LinearWrap
	SDL_GPUSamplerCreateInfo linearWrap{};
	linearWrap.min_filter = SDL_GPU_FILTER_LINEAR;
	linearWrap.mag_filter = SDL_GPU_FILTER_LINEAR;
	linearWrap.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
	linearWrap.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	linearWrap.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	linearWrap.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;

	Samplers[3] = SDL_CreateGPUSampler(device.get(), &linearWrap);

	// AnisotropicClamp
	SDL_GPUSamplerCreateInfo anisotropicClamp{};
	anisotropicClamp.min_filter = SDL_GPU_FILTER_LINEAR;
	anisotropicClamp.mag_filter = SDL_GPU_FILTER_LINEAR;
	anisotropicClamp.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
	anisotropicClamp.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	anisotropicClamp.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	anisotropicClamp.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	anisotropicClamp.enable_anisotropy = true;
	anisotropicClamp.max_anisotropy = 4;

	Samplers[4] = SDL_CreateGPUSampler(device.get(), &anisotropicClamp);

	// AnisotropicWrap
	SDL_GPUSamplerCreateInfo anisotropicWrap{};
	anisotropicWrap.min_filter = SDL_GPU_FILTER_LINEAR;
	anisotropicWrap.mag_filter = SDL_GPU_FILTER_LINEAR;
	anisotropicWrap.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
	anisotropicWrap.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	anisotropicWrap.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	anisotropicWrap.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	anisotropicWrap.enable_anisotropy = true;
	anisotropicWrap.max_anisotropy = 4;

	Samplers[5] = SDL_CreateGPUSampler(device.get(), &anisotropicWrap);

	std::cout << "Samplers initialized." << std::endl;
	
	graphics_pipeline = std::make_unique<GpuGraphicsPipeline>(device, window.get(), *vertex_shader, *fragment_shader);

	texture_vertex_buffer = std::make_unique<GpuVertexBuffer>(device, MAX_OBJECTS * 4 * sizeof(TextureVertex), SDL_GPU_BUFFERUSAGE_VERTEX);
	texture_index_buffer = std::make_unique<GpuVertexBuffer>(device, MAX_OBJECTS * 6 * sizeof(Uint16), SDL_GPU_BUFFERUSAGE_INDEX);
	transfer_buffer = std::make_unique<GpuTransferBuffer<TextureVertex>>(
		device, static_cast<Uint32>(MAX_OBJECTS * 4 * sizeof(TextureVertex) + MAX_OBJECTS * 6 * sizeof(Uint16)), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD);
}

void graphics::GpuRenderer::updateBuffers()
{
	/*
	if (!vertices.empty())
	{

		// Create vertex buffer
		vertex_buffer = std::make_unique<GpuVertexBuffer>(device, static_cast<Uint32>(vertices.size()) * sizeof(Vertex), SDL_GPU_BUFFERUSAGE_VERTEX);

		// Create transfer buffer
		GpuTransferBuffer<Vertex> transfer_buffer = GpuTransferBuffer<Vertex>{ device, static_cast<Uint32>(static_cast<Uint32>(vertices.size()) * sizeof(Vertex)), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD };

		//std::cout << "Transfer buffer created." << std::endl;

		transfer_buffer.putAutomatically(vertices.data(), vertices.size() * sizeof(Vertex));

		std::unique_ptr<SDL_GPUCommandBuffer, GPUCommandBufferDeleter> command_buffer{ SDL_AcquireGPUCommandBuffer(device.get()) };

		if (!command_buffer)
		{
			throw std::runtime_error{ std::format("SDL_AcquireGPUCommandBuffer failed: {}", SDL_GetError()) };
		}

		//std::cout << "Command buffer acquired." << std::endl;

		SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

		// Where is
		SDL_GPUTransferBufferLocation location;
		location.transfer_buffer = transfer_buffer.get();
		location.offset = 0;

		// Where to
		SDL_GPUBufferRegion region{};
		region.buffer = vertex_buffer->get();
		region.size = vertices.size() * sizeof(Vertex);
		region.offset = 0;

		// Upload
		SDL_UploadToGPUBuffer(copy_pass, &location, &region, true);

		SDL_EndGPUCopyPass(copy_pass);

		vertices.clear();
	}
	*/

	if (!texture_objects.empty())
	{
		size_t objects_size = texture_objects.size();
		//texture_vertex_buffer = std::make_unique<GpuVertexBuffer>(device, objects_size * 4 * sizeof(TextureVertex), SDL_GPU_BUFFERUSAGE_VERTEX);
		//texture_index_buffer = std::make_unique<GpuVertexBuffer>(device, objects_size * 6 * sizeof(Uint16), SDL_GPU_BUFFERUSAGE_INDEX);

		TextureVertex* data = transfer_buffer->map();

		//SDL_memcpy can be used instead
		for (int i = 0; i < objects_size; ++i)
		{
			auto& texture_vertices = texture_objects[i].vertices;
			data[i * 4 + 0] = texture_vertices[0];
			data[i * 4 + 1] = texture_vertices[1];
			data[i * 4 + 2] = texture_vertices[2];
			data[i * 4 + 3] = texture_vertices[3];
		}
		size_t end = texture_objects.size() * 4;
		Uint16* index_data = reinterpret_cast<Uint16*>(&data[end]);
		for (int i = 0; i < objects_size; ++i)
		{
			index_data[i * 6 + 0] = static_cast<Uint16>(i * 4 + 0);
			index_data[i * 6 + 1] = static_cast<Uint16>(i * 4 + 1);
			index_data[i * 6 + 2] = static_cast<Uint16>(i * 4 + 2);
			index_data[i * 6 + 3] = static_cast<Uint16>(i * 4 + 2);
			index_data[i * 6 + 4] = static_cast<Uint16>(i * 4 + 3);
			index_data[i * 6 + 5] = static_cast<Uint16>(i * 4 + 0);
		}

		transfer_buffer->unmap();

		std::unique_ptr<SDL_GPUCommandBuffer, GPUCommandBufferDeleter> command_buffer{ SDL_AcquireGPUCommandBuffer(device.get()) };
		SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());
		
		// Upload vertices
		SDL_GPUTransferBufferLocation vertices_transfer_info = {};
		vertices_transfer_info.transfer_buffer = transfer_buffer->get();
		vertices_transfer_info.offset = 0;
		SDL_GPUBufferRegion vertices_buffer_region = {};
		vertices_buffer_region.buffer = texture_vertex_buffer->get();
		vertices_buffer_region.offset = 0;
		vertices_buffer_region.size = objects_size * sizeof(TextureVertex) * 4;

		SDL_UploadToGPUBuffer(copy_pass, &vertices_transfer_info, &vertices_buffer_region, false);

		// Upload indices
		SDL_GPUTransferBufferLocation indices_transfer_info = {};
		indices_transfer_info.transfer_buffer = transfer_buffer->get();
		indices_transfer_info.offset = objects_size * sizeof(TextureVertex) * 4;
		SDL_GPUBufferRegion indices_buffer_region = {};
		indices_buffer_region.buffer = texture_index_buffer->get();
		indices_buffer_region.offset = 0;
		indices_buffer_region.size = objects_size * sizeof(Uint16) * 6;

		SDL_UploadToGPUBuffer(copy_pass, &indices_transfer_info, &indices_buffer_region, false);

		SDL_EndGPUCopyPass(copy_pass);
	}
}

void graphics::GpuRenderer::update()
{
	updateBuffers();

	std::unique_ptr<SDL_GPUCommandBuffer, GPUCommandBufferDeleter> command_buffer{ SDL_AcquireGPUCommandBuffer(device.get()) };

	if (!command_buffer)
	{
		throw std::runtime_error{ std::format("SDL_AcquireGPUCommandBuffer failed: {}", SDL_GetError()) };
	}

	SDL_GPUTexture* swapchain_texture;
	uint32_t width, height;
	
	if (!SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer.get(), window.get(), &swapchain_texture, &width, &height))
	{
		throw std::runtime_error{ std::format("SDL_WaitAndAcquireGPUSwapchainTexture failed: {}", SDL_GetError()) };
	}

	if (swapchain_texture)
	{
		SDL_GPUColorTargetInfo target_info
		{
			.texture = swapchain_texture,
			.clear_color = {0.0f, 0.0f, 0.0f, 1.0f},
			.load_op = SDL_GPU_LOADOP_CLEAR,
			.store_op = SDL_GPU_STOREOP_STORE,
			.cycle = true
		};

		SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer.get(), &target_info, 1, nullptr);

		if (!texture_objects.empty())
		{
			SDL_BindGPUGraphicsPipeline(render_pass, graphics_pipeline->get());

			// Bind the vertex buffer
			SDL_GPUBufferBinding buffer_bindings[2];
			/*
			buffer_bindings[0].buffer = vertex_buffer->get();
			buffer_bindings[0].offset = 0;
			*/

			// Bind texture vertex buffer
			buffer_bindings[0].buffer = texture_vertex_buffer->get();
			buffer_bindings[0].offset = 0;
			
			// Bind texture indices buffer
			buffer_bindings[1].buffer = texture_index_buffer->get();
			buffer_bindings[1].offset = 0;

			SDL_BindGPUVertexBuffers(render_pass, 0, &buffer_bindings[0], 1);
			SDL_BindGPUIndexBuffer(render_pass, &buffer_bindings[1], SDL_GPU_INDEXELEMENTSIZE_16BIT);

			// Bind texture samplers
			std::vector<SDL_GPUTextureSamplerBinding> texture_sampler_bindings;
			for (const auto& texture_object : texture_objects)
			{
				const auto& texture = textures.at(texture_object.texture_name);

				SDL_GPUTextureSamplerBinding texture_sampler_binding = {};
				texture_sampler_binding.sampler = Samplers[0];
				texture_sampler_binding.texture = texture->get();

				texture_sampler_bindings.push_back(texture_sampler_binding);
			}
			SDL_BindGPUFragmentSamplers(render_pass, 0, texture_sampler_bindings.data(), 1);

			//SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &screen_size_uniform, sizeof(ScreenSize));

			//time_uniform.time = SDL_GetTicksNS() / 1e9f;
			//SDL_PushGPUFragmentUniformData(command_buffer.get(), 0, &time_uniform, sizeof(Uniform));

			SDL_DrawGPUIndexedPrimitives(render_pass, texture_objects.size() * 6, 1, 0, 0, 0);
			//SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);
		}

		SDL_EndGPURenderPass(render_pass);
	}
	
	texture_objects.clear();
}

void graphics::GpuRenderer::loadTexture(const std::filesystem::path& path, const std::string& name)
{
	auto texture = std::make_shared<GpuTexture>(device, path);
	texture->setName(name);
	textures[name] = texture;

	// Upload texture on the GPU
	GpuTransferBuffer<Uint8> texture_transfer_buffer {device, static_cast<Uint32>(texture->w() * texture->h() * 4), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD};
	
	std::unique_ptr<SDL_GPUCommandBuffer, GPUCommandBufferDeleter> command_buffer{ SDL_AcquireGPUCommandBuffer(device.get()) };
	SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

	Uint8* texture_data = texture_transfer_buffer.map();

	SDL_memcpy(texture_data, texture->pixels(), texture->w() * texture->h() * 4);

	texture_transfer_buffer.unmap();

	// Source
	SDL_GPUTextureTransferInfo gpu_texture_transfer_info = {};
	gpu_texture_transfer_info.transfer_buffer = texture_transfer_buffer.get();
	gpu_texture_transfer_info.offset = 0;

	// Destination
	SDL_GPUTextureRegion gpu_texture_region = {};
	gpu_texture_region.texture = texture->get();
	gpu_texture_region.w = texture->w();
	gpu_texture_region.h = texture->h();
	gpu_texture_region.d = 1;

	SDL_UploadToGPUTexture(copy_pass, &gpu_texture_transfer_info, &gpu_texture_region, false);
	SDL_EndGPUCopyPass(copy_pass);
}

void graphics::GpuRenderer::renderTriangle(float x1, float y1, float x2, float y2, float x3, float y3, SDL_FColor color)
{
	vertices.emplace_back(x1, y1, 0.0f, color.r, color.g, color.b, color.a);
	vertices.emplace_back(x2, y2, 0.0f, color.r, color.g, color.b, color.a);
	vertices.emplace_back(x3, y3, 0.0f, color.r, color.g, color.b, color.a);
}

void graphics::GpuRenderer::renderRectangle1(float x1, float y1, float x2, float y2, SDL_FColor color)
{
	/*vertices.emplace_back(x1, y1, 0.0f, color.r, color.g, color.b, color.a);
	vertices.emplace_back(x1, y2, 0.0f, color.r, color.g, color.b, color.a);
	vertices.emplace_back(x2, y2, 0.0f, color.r, color.g, color.b, color.a);

	vertices.emplace_back(x2, y2, 0.0f, color.r, color.g, color.b, color.a);
	vertices.emplace_back(x2, y1, 0.0f, color.r, color.g, color.b, color.a);
	vertices.emplace_back(x1, y1, 0.0f, color.r, color.g, color.b, color.a);*/
}

void graphics::GpuRenderer::renderRectangle2(float x, float y, float w, float h, SDL_FColor color)
{
	renderRectangle1(x, y, x + w, y + h, color);
}

void graphics::GpuRenderer::renderSprite(const std::string& texture_name, float x, float y, float w, float h)
{
	texture_objects.emplace_back(TextureObject
	{
		.texture_name = texture_name,
		.vertices = 
		{
			TextureVertex{x, y, 0.f, 0.0f, 0.0f},
			TextureVertex{x, y + h, 0.0f, 0.0f, 1.0f},
			TextureVertex{x + w, y + h, 0.0f, 1.0f, 1.0f},
			TextureVertex{x + w, y, 0.0f, 1.0f, 0.0f}
		}
	});
}
