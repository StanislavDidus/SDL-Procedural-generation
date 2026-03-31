#include "GpuRenderer.hpp"
#include "Window.hpp"

#include <iostream>

#include "glm/mat4x4.hpp"
#include <SDL3_shadercross/SDL_shadercross.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "GpuSampler.hpp"
#include "Surface.hpp"
#include "glm/gtx/transform.hpp"
#include "SDL3/SDL_gpu.h"

graphics::GpuRenderer::GpuRenderer(Window& window)
	: window{ window }
{
	//Create GPU

	bool debug = false;
#if _DEBUG
	debug = true;
#endif
	device = std::shared_ptr<SDL_GPUDevice>{ SDL_CreateGPUDevice(SDL_ShaderCross_GetHLSLShaderFormats(), debug, nullptr), SDL_DestroyGPUDevice };

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
	SDL_SetGPUSwapchainParameters(device.get(), window.get(), SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_IMMEDIATE);

	// Init vertex shader
	vertex_shader = std::make_unique<GpuShader>(device, "shaders/OnlyPosition.vert.hlsl", 0, 1, 0, 0);
	fragment_shader = std::make_unique<GpuShader>(device, "shaders/SolidColor.frag.hlsl", 0, 0, 0, 0);
	texture_vertex_shader = std::make_unique<GpuShader>(device, "shaders/PullSpriteBatch.vert.hlsl", 0, 1, 1, 0);
	texture_fragment_shader = std::make_unique<GpuShader>(device, "shaders/TextureQuadColor.frag.hlsl", 1, 0, 0, 0);

	std::cout << "Shaders initialized." << std::endl;

	initSamplers();

	// Vertex graphics pipeline
	{
		std::vector<SDL_GPUVertexBufferDescription> vertex_buffer_descriptions;
		vertex_buffer_descriptions.emplace_back(0, static_cast<Uint32>(sizeof(Vertex)), SDL_GPU_VERTEXINPUTRATE_VERTEX, 0);

		std::vector<SDL_GPUVertexAttribute> vertex_attributes;
		vertex_attributes.emplace_back(0u, 0u, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, 0u);
		vertex_attributes.emplace_back(1u, 0u, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, static_cast<Uint32>(3 * sizeof(float)));

		vertex_graphics_pipeline = std::make_unique<GpuGraphicsPipeline>(device, window.get(), *vertex_shader, *fragment_shader, vertex_buffer_descriptions, vertex_attributes, false);
	}

	// TextureVertex graphics pipeline
	{
		std::vector<SDL_GPUVertexBufferDescription> vertex_buffer_descriptions;
		//vertex_buffer_descriptions.emplace_back(0, sizeof(TextureVertex), SDL_GPU_VERTEXINPUTRATE_VERTEX, 0);

		std::vector<SDL_GPUVertexAttribute> vertex_attributes;
		//vertex_attributes.emplace_back(0, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, 0);
		//vertex_attributes.emplace_back(1, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, 3 * sizeof(float));

		texture_graphics_pipeline = std::make_unique<GpuGraphicsPipeline>(device, window.get(), *texture_vertex_shader, *texture_fragment_shader, vertex_buffer_descriptions, vertex_attributes, true);
	}

	Uint32 texture_buffer_size = static_cast<Uint32>(MAX_NUMBER_OBJECTS * sizeof(SpriteData));
	texture_vertex_buffer = std::make_unique<GpuVertexBuffer>(device, texture_buffer_size, SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ);

	transfer_buffer = std::make_unique<GpuTransferBuffer<SpriteData>>(
		device, static_cast<Uint32>(texture_buffer_size), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD);
}


void graphics::GpuRenderer::initSamplers()
{
	// PointClamp
	samplers[0] = std::make_unique<GpuSampler>(device, SDL_GPU_FILTER_NEAREST, SDL_GPU_SAMPLERMIPMAPMODE_NEAREST, SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE);
	// PointWrap
	samplers[1] = std::make_unique<GpuSampler>(device, SDL_GPU_FILTER_NEAREST, SDL_GPU_SAMPLERMIPMAPMODE_NEAREST, SDL_GPU_SAMPLERADDRESSMODE_REPEAT);
	// LinearClamp
	samplers[2] = std::make_unique<GpuSampler>(device, SDL_GPU_FILTER_LINEAR, SDL_GPU_SAMPLERMIPMAPMODE_LINEAR, SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE);
	// LinearWrap
	samplers[3] = std::make_unique<GpuSampler>(device, SDL_GPU_FILTER_LINEAR, SDL_GPU_SAMPLERMIPMAPMODE_LINEAR, SDL_GPU_SAMPLERADDRESSMODE_REPEAT);
	// AnisotropicClamp
	samplers[4] = std::make_unique<GpuSampler>(device, SDL_GPU_FILTER_LINEAR, SDL_GPU_SAMPLERMIPMAPMODE_LINEAR, SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE, 4);
	// AnisotropicWrap
	samplers[5] = std::make_unique<GpuSampler>(device, SDL_GPU_FILTER_LINEAR, SDL_GPU_SAMPLERMIPMAPMODE_LINEAR, SDL_GPU_SAMPLERADDRESSMODE_REPEAT, 4);

	std::cout << "Samplers initialized." << std::endl;
}

void graphics::GpuRenderer::updateBuffers()
{
	if (!vertices.empty())
	{
		// Create vertex buffer
		vertex_buffer = std::make_unique<GpuVertexBuffer>(device, static_cast<Uint32>(vertices.size() * sizeof(Vertex)), SDL_GPU_BUFFERUSAGE_VERTEX);

		// Create transfer buffer
		GpuTransferBuffer<Vertex> transfer_buffer = GpuTransferBuffer<Vertex>{ device, static_cast<Uint32>(static_cast<Uint32>(vertices.size()) * sizeof(Vertex)), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD };

		//std::cout << "Transfer buffer created." << std::endl;

		transfer_buffer.putAutomatically(vertices.data(), vertices.size() * sizeof(Vertex), 0);

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
	}

	if (!texture_objects.empty())
	{
		size_t objects_size = texture_objects.size();
		//texture_vertex_buffer = std::make_unique<GpuVertexBuffer>(device, objects_size * 4 * sizeof(TextureVertex), SDL_GPU_BUFFERUSAGE_VERTEX);
		//texture_index_buffer = std::make_unique<GpuVertexBuffer>(device, objects_size * 6 * sizeof(Uint16), SDL_GPU_BUFFERUSAGE_INDEX);

		SpriteData* data = transfer_buffer->map();

		SDL_memcpy(data, texture_objects.data(), texture_objects.size() * sizeof(SpriteData));

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
		vertices_buffer_region.size = objects_size * sizeof(SpriteData);

		SDL_UploadToGPUBuffer(copy_pass, &vertices_transfer_info, &vertices_buffer_region, false);

		SDL_EndGPUCopyPass(copy_pass);
	}

	if (!ui_texture_objects.empty())
	{
		size_t objects_size = ui_texture_objects.size();
		//texture_vertex_buffer = std::make_unique<GpuVertexBuffer>(device, objects_size * 4 * sizeof(TextureVertex), SDL_GPU_BUFFERUSAGE_VERTEX);
		//texture_index_buffer = std::make_unique<GpuVertexBuffer>(device, objects_size * 6 * sizeof(Uint16), SDL_GPU_BUFFERUSAGE_INDEX);

		transfer_buffer->putAutomatically(ui_texture_objects.data(), ui_texture_objects.size(), MAX_NUMBER_TEXTURE_OBJECTS);

		std::unique_ptr<SDL_GPUCommandBuffer, GPUCommandBufferDeleter> command_buffer{ SDL_AcquireGPUCommandBuffer(device.get()) };
		SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

		// Upload vertices
		SDL_GPUTransferBufferLocation vertices_transfer_info = {};
		vertices_transfer_info.transfer_buffer = transfer_buffer->get();
		vertices_transfer_info.offset = MAX_NUMBER_TEXTURE_OBJECTS * sizeof(SpriteData);
		SDL_GPUBufferRegion vertices_buffer_region = {};
		vertices_buffer_region.buffer = texture_vertex_buffer->get();
		vertices_buffer_region.offset = MAX_NUMBER_TEXTURE_OBJECTS * sizeof(SpriteData);
		vertices_buffer_region.size = objects_size * sizeof(SpriteData);

		SDL_UploadToGPUBuffer(copy_pass, &vertices_transfer_info, &vertices_buffer_region, false);

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
	
	//.........................//
	//<Calculate object matrix>//
	//.........................//
	const auto& window_size = window.getWindowSize();
	const auto& view_position = view;
	glm::vec2 mid_screen = { window_size.x * 0.5f, window_size.y * 0.5f };

	glm::mat4 view_matrix = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ -view_position, 0.0f });
	glm::mat4 to_origin = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ -mid_screen, 0.0f });
	glm::mat4 from_origin = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ mid_screen, 0.0f });

	glm::mat4 scale = glm::scale(glm::mat4{ 1.0f }, glm::vec3{zoom});
	glm::mat4 rotation = glm::rotate(glm::mat4{ 1.0f }, glm::degrees(angle), glm::vec3{0.0f, 0.0f, 1.0f});

	glm::mat4 projection = glm::ortho(
		0.0f, static_cast<float>(window_size.x),
		static_cast<float>(window_size.y), 0.0f
	);
	//.........................//
	//<_______________________>//
	//.........................//

	glm::mat4 base_matrix = glm::transpose(projection);
	glm::mat4 final_matrix = glm::transpose(projection * view_matrix * from_origin * rotation * scale * to_origin);

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

		if (!vertices.empty())
		{
			SDL_BindGPUGraphicsPipeline(render_pass, vertex_graphics_pipeline->get());

			SDL_GPUBufferBinding buffer_bindings[1];

			buffer_bindings[0].buffer = vertex_buffer->get();
			buffer_bindings[0].offset = 0;

			SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &screen_size_uniform, sizeof(ScreenSize));

			SDL_BindGPUVertexBuffers(render_pass, 0, buffer_bindings, 1);
			SDL_DrawGPUPrimitives(render_pass, vertices.size(), 1, 0, 0);
		}

		if (!texture_objects.empty())
		{
			SDL_BindGPUGraphicsPipeline(render_pass, texture_graphics_pipeline->get());

			SDL_BindGPUVertexStorageBuffers(render_pass, 0, &texture_vertex_buffer->get(), 1);

			SDL_BindGPUFragmentSamplers(render_pass, 0, texture_sampler_bindings.data(), texture_sampler_bindings.size());

			//SDL_BindGPUFragmentSamplers(render_pass, 1, texture_sampler_bindings.data() + 1, 1);
			
			SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &final_matrix, sizeof(glm::mat4));
			SDL_DrawGPUPrimitives(render_pass, texture_objects.size() * 6, 1, 0, 0);
		}
		
		if (!ui_texture_objects.empty())
		{
			SDL_BindGPUGraphicsPipeline(render_pass, texture_graphics_pipeline->get());

			SDL_BindGPUVertexStorageBuffers(render_pass, 0, &texture_vertex_buffer->get(), 1);
		
			SDL_BindGPUFragmentSamplers(render_pass, 0, texture_sampler_bindings.data(), 1);
			
			SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &base_matrix, sizeof(glm::mat4));
			SDL_DrawGPUPrimitives(render_pass, ui_texture_objects.size() * 6, 1,0, 0);

		}
	
		SDL_EndGPURenderPass(render_pass);
	}

	texture_objects.clear();
	ui_texture_objects.clear();
	vertices.clear();
}

std::shared_ptr<graphics::GpuTexture> graphics::GpuRenderer::loadTexture(const std::filesystem::path& path, const std::string& name)
{
	auto texture = std::make_shared<GpuTexture>(device, path);
	texture->setName(name);
	textures[name] = texture;

	// Upload texture on the GPU
	GpuTransferBuffer<Uint8> texture_transfer_buffer{ device, static_cast<Uint32>(texture->w() * texture->h() * 4), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD };

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

	//Bind Sampler to a texture
	texture_sampler_bindings.emplace_back(texture->get(), samplers[5]->get());

	return texture;
}

glm::vec2 graphics::GpuRenderer::getView() const
{
	return view;
}

float graphics::GpuRenderer::getZoom() const
{
	return zoom;
}

float graphics::GpuRenderer::getAngle() const
{
	return angle;
}

glm::ivec2 graphics::GpuRenderer::getWindowSize() const
{
	return window.getWindowSize();
}

std::vector<graphics::Vertex>& graphics::GpuRenderer::getVertices()
{
	return vertices;
}

void graphics::GpuRenderer::setView(glm::vec2 view)
{
	this->view = view;
}

void graphics::GpuRenderer::setZoom(float zoom)
{
	this->zoom = zoom;
}

void graphics::GpuRenderer::setAngle(float angle)
{
	this->angle = angle;
}

/*void graphics::GpuRenderer::renderTriangle(float x1, float y1, float x2, float y2, float x3, float y3, SDL_FColor color)
{
	vertices.emplace_back(x1, y1, 0.0f, color.r, color.g, color.b, color.a);
	vertices.emplace_back(x2, y2, 0.0f, color.r, color.g, color.b, color.a);
	vertices.emplace_back(x3, y3, 0.0f, color.r, color.g, color.b, color.a);
}

void graphics::GpuRenderer::renderRectangle1(float x1, float y1, float x2, float y2, SDL_FColor color)
{
	vertices.emplace_back(x1, y1, 0.0f, color.r, color.g, color.b, color.a);
	vertices.emplace_back(x1, y2, 0.0f, color.r, color.g, color.b, color.a);
	vertices.emplace_back(x2, y2, 0.0f, color.r, color.g, color.b, color.a);

	vertices.emplace_back(x2, y2, 0.0f, color.r, color.g, color.b, color.a);
	vertices.emplace_back(x2, y1, 0.0f, color.r, color.g, color.b, color.a);
	vertices.emplace_back(x1, y1, 0.0f, color.r, color.g, color.b, color.a);
}

void graphics::GpuRenderer::renderRectangle2(float x, float y, float w, float h, SDL_FColor color)
{
}*/

void graphics::GpuRenderer::renderSprite(const std::string& texture_name, float x, float y, float w, float h, float angle, bool ignore_view_zoom)
{
	texture_objects.push_back(
		SpriteData{
			.x = x,
			.y = y,
			.z = 0.0f,
			.rotation = angle,
			.w = w,
			.h = h,
			.tex_u = 0.0f,
			.tex_v = 0.0f,
			.tex_w = 1.0f,
			.tex_h = 1.0f,
			.r = 1.0f,
			.g = 1.0f,
			.b = 1.0f,
			.a = 1.0f,
		}
	);
}

