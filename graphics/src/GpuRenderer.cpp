#include "GpuRenderer.hpp"
#include "Window.hpp"

#include <iostream>
#include "GpuTransferBuffer.hpp"

#include "SDL3/SDL_gpu.h"

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

	device = std::shared_ptr<SDL_GPUDevice>{ SDL_CreateGPUDevice(shader_formats, true, nullptr), SDL_DestroyGPUDevice };

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
	//	SDL_SetGPUSwapchainParameters(device.get(), window.get(), SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_IMMEDIATE);

	// Init vertex shader
	vertex_shader = std::make_unique<GpuShader>(device, "shaders/compiled/OnlyPosition.vert.spv", 1);
	fragment_shader = std::make_unique<GpuShader>(device, "shaders/compiled/Gradient.frag.spv", 1);

	std::cout << "Shaders initialized." << std::endl;
	
	graphics_pipeline = std::make_unique<GpuGraphicsPipeline>(device, window.get(), *vertex_shader, *fragment_shader);
}

void graphics::GpuRenderer::updateBuffers()
{
	if (vertices.empty()) return;

	// Create vertex buffer
	vertex_buffer = std::make_unique<GpuVertexBuffer>(device, static_cast<Uint32>(vertices.size()) * sizeof(Vertex), SDL_GPU_BUFFERUSAGE_VERTEX);

	//std::cout << "Vertex buffer created." << std::endl;

	// Create transfer buffer
	GpuTransferBuffer transfer_buffer = GpuTransferBuffer{ device, static_cast<Uint32>(static_cast<Uint32>(vertices.size()) * sizeof(Vertex)), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD };

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
			.clear_color = {0.16f, 0.47f, 0.34f, 1.0f},
			.load_op = SDL_GPU_LOADOP_CLEAR,
			.store_op = SDL_GPU_STOREOP_STORE,
			.cycle = true
		};

		SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer.get(), &target_info, 1, nullptr);

		if (vertex_buffer)
		{
			SDL_BindGPUGraphicsPipeline(render_pass, graphics_pipeline->get());

			// Bind the vertex buffer
			SDL_GPUBufferBinding buffer_bindings[1];
			buffer_bindings[0].buffer = vertex_buffer->get();
			buffer_bindings[0].offset = 0;

			SDL_BindGPUVertexBuffers(render_pass, 0, buffer_bindings, 1);

			SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &screen_size_uniform, sizeof(ScreenSize));

			time_uniform.time = SDL_GetTicksNS() / 1e9f;
			SDL_PushGPUFragmentUniformData(command_buffer.get(), 0, &time_uniform, sizeof(Uniform));

			SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);
		}

		SDL_EndGPURenderPass(render_pass);
	}
}

void graphics::GpuRenderer::loadTexture(const std::filesystem::path& path, const std::string& name)
{
	auto texture = std::make_shared<GpuTexture>(device, path);
	texture->setName(name);
	textures[name] = texture;
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
