#include "GpuRenderer.hpp"
#include "Window.hpp"

#include <iostream>

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
	
	// Graphics pipeline
	SDL_GPUGraphicsPipelineCreateInfo pipeline_info = {};
	pipeline_info.vertex_shader = vertex_shader->get().get();
	pipeline_info.fragment_shader = fragment_shader->get().get();
	pipeline_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
	pipeline_info.target_info = {};
	pipeline_info.vertex_input_state = {};
	pipeline_info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_BACK;
	pipeline_info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;

	// Describe the vertex buffers
	SDL_GPUVertexBufferDescription vertex_buffer_descriptions[1] = {};
	vertex_buffer_descriptions[0].slot = 0;
	vertex_buffer_descriptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
	vertex_buffer_descriptions[0].instance_step_rate = 0;
	vertex_buffer_descriptions[0].pitch = sizeof(Vertex);

	pipeline_info.vertex_input_state.num_vertex_buffers = 1;
	pipeline_info.vertex_input_state.vertex_buffer_descriptions = vertex_buffer_descriptions;

	SDL_GPUVertexAttribute vertex_attributes[2] = {};

	// float3 Position
	vertex_attributes[0].buffer_slot = 0;
	vertex_attributes[0].location = 0;
	vertex_attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
	vertex_attributes[0].offset = 0;

	// float4 Color
	vertex_attributes[1].buffer_slot = 0;
	vertex_attributes[1].location = 1;
	vertex_attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
	vertex_attributes[1].offset = 3 * sizeof(float);

	pipeline_info.vertex_input_state.num_vertex_attributes = 2;
	pipeline_info.vertex_input_state.vertex_attributes = vertex_attributes;

	// Describe the color target
	SDL_GPUColorTargetDescription color_target_descriptions[1] = {};
	color_target_descriptions[0] = {};
	color_target_descriptions[0].blend_state.enable_blend = true;
	color_target_descriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
	color_target_descriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
	color_target_descriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
	color_target_descriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	color_target_descriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
	color_target_descriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	color_target_descriptions[0].format = SDL_GetGPUSwapchainTextureFormat(device.get(), window.get());

	if (color_target_descriptions[0].format == SDL_GPU_TEXTUREFORMAT_INVALID)
	{
		throw std::runtime_error{ std::format("Could not get swapchain texture format: {}", SDL_GetError()) };
	}

	pipeline_info.target_info.num_color_targets = 1;
	pipeline_info.target_info.color_target_descriptions = color_target_descriptions;

	// Create pipeline
	graphics_pipeline = { SDL_CreateGPUGraphicsPipeline(device.get(), &pipeline_info), GPUGraphicsPipelineDeleter{device.get()} };

	if (!graphics_pipeline)
	{
		throw std::runtime_error{ std::format("Could not create GPU graphics pipeline: {}", SDL_GetError()) };
	}

	std::cout << "GPU graphics pipeline was created." << std::endl;

}

void graphics::GpuRenderer::updateBuffers()
{
	if (vertices.empty()) return;

	// Create vertex buffer
	SDL_GPUBufferCreateInfo buffer_info;
	buffer_info.size = vertices.size() * sizeof(Vertex);
	buffer_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
	vertex_buffer = { SDL_CreateGPUBuffer(device.get(), &buffer_info), GPUBufferDeleter{device.get()} };

	if (!vertex_buffer)
	{
		throw std::runtime_error{ std::format("Could not create GPU vertex buffer: {}", SDL_GetError()) };
	}

	//std::cout << "Vertex buffer created." << std::endl;

	// Create transfer buffer
	SDL_GPUTransferBufferCreateInfo transfer_info;
	transfer_info.size = vertices.size() * sizeof(Vertex);
	transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	std::unique_ptr<SDL_GPUTransferBuffer, GPUTransferBufferDeleter> transfer_buffer = { SDL_CreateGPUTransferBuffer(device.get(), &transfer_info), GPUTransferBufferDeleter{device.get()} };

	if (!transfer_buffer)
	{
		throw std::runtime_error{ std::format("Could not create GPU transfer buffer: {}", SDL_GetError()) };
	}

	//std::cout << "Transfer buffer created." << std::endl;

	// Map transfer buffer to a pointer
	Vertex* data = static_cast<Vertex*>(SDL_MapGPUTransferBuffer(device.get(), transfer_buffer.get(), false));

	if (!data)
	{
		throw std::runtime_error{ std::format("Could not map GPU transfer buffer: {}", SDL_GetError()) };
	}

	/*data[0] = vertices[0];
	data[1] = vertices[1];
	data[2] = vertices[2];*/

	SDL_memcpy(data, vertices.data(), vertices.size() * sizeof(Vertex));

	SDL_UnmapGPUTransferBuffer(device.get(), transfer_buffer.get());

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
	region.buffer = vertex_buffer.get();
	region.size = vertices.size() * sizeof(Vertex);
	region.offset = 0;

	// Upload
	SDL_UploadToGPUBuffer(copy_pass, &location, &region, true);
	
	SDL_EndGPUCopyPass(copy_pass);
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
		SDL_BindGPUGraphicsPipeline(render_pass, graphics_pipeline.get());

		// Bind the vertex buffer
		SDL_GPUBufferBinding buffer_bindings[1];
		buffer_bindings[0].buffer = vertex_buffer.get();
		buffer_bindings[0].offset = 0;

		SDL_BindGPUVertexBuffers(render_pass, 0, buffer_bindings, 1);

		SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &screen_size_uniform, sizeof(ScreenSize));

		time_uniform.time = SDL_GetTicksNS() / 1e9f;
		SDL_PushGPUFragmentUniformData(command_buffer.get(), 0, &time_uniform, sizeof(Uniform));

		SDL_DrawGPUPrimitives(render_pass, 6, 1, 0, 0);
	}

	SDL_EndGPURenderPass(render_pass);
}

void graphics::GpuRenderer::renderTriangle(float x1, float y1, float x2, float y2, float x3, float y3, SDL_FColor color)
{
	/*
	vertices.emplace_back(x1, y1, 0.0f, color.r, color.g, color.b, color.a);
	vertices.emplace_back(x2, y2, 0.0f, color.r, color.g, color.b, color.a);
	vertices.emplace_back(x3, y3, 0.0f, color.r, color.g, color.b, color.a);
*/
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
