#include "GpuGraphicsPipeline.hpp"

#include <iostream>

#include "GpuShader.hpp"
#include "Vertex.hpp"

graphics::GpuGraphicsPipeline::GpuGraphicsPipeline
(
	std::shared_ptr<SDL_GPUDevice> device,
	SDL_Window* window,
	GpuShader& vertex_shader,
	GpuShader& fragment_shader,
	const std::vector<SDL_GPUVertexBufferDescription>& vertex_buffer_descriptions,
	const std::vector<SDL_GPUVertexAttribute>& vertex_attributes
)
	: device{device}
{
	// Graphics pipeline
	SDL_GPUGraphicsPipelineCreateInfo pipeline_info = {};
	pipeline_info.vertex_shader = vertex_shader.get();
	pipeline_info.fragment_shader = fragment_shader.get();
	pipeline_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
	pipeline_info.target_info = {};
	pipeline_info.vertex_input_state = {};
	pipeline_info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_BACK;
	pipeline_info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;

	// Describe the vertex buffers
	//SDL_GPUVertexBufferDescription vertex_buffer_descriptions[1] = {};
	/*
	vertex_buffer_descriptions[0].slot = 0;
	vertex_buffer_descriptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
	vertex_buffer_descriptions[0].instance_step_rate = 0;
	vertex_buffer_descriptions[0].pitch = sizeof(Vertex);
	*/

	/*vertex_buffer_descriptions[0].slot = 0;
	vertex_buffer_descriptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
	vertex_buffer_descriptions[0].instance_step_rate = 0;
	vertex_buffer_descriptions[0].pitch = sizeof(TextureVertex);*/

	pipeline_info.vertex_input_state.num_vertex_buffers = vertex_buffer_descriptions.size();
	pipeline_info.vertex_input_state.vertex_buffer_descriptions = vertex_buffer_descriptions.data();

	//SDL_GPUVertexAttribute vertex_attributes[2] = {};

	// float3 Position
	/*
	vertex_attributes[0].buffer_slot = 0;
	vertex_attributes[0].location = 0;
	vertex_attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
	vertex_attributes[0].offset = 0;

	// float4 Color
	vertex_attributes[1].buffer_slot = 0;
	vertex_attributes[1].location = 1;
	vertex_attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
	vertex_attributes[1].offset = 3 * sizeof(float);
	*/

	pipeline_info.vertex_input_state.num_vertex_attributes = vertex_attributes.size();
	pipeline_info.vertex_input_state.vertex_attributes = vertex_attributes.data();

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
	color_target_descriptions[0].format = SDL_GetGPUSwapchainTextureFormat(device.get(), window);

	if (color_target_descriptions[0].format == SDL_GPU_TEXTUREFORMAT_INVALID)
	{
		throw std::runtime_error{ std::format("Could not get swapchain texture format: {}", SDL_GetError()) };
	}

	pipeline_info.target_info.num_color_targets = 1;
	pipeline_info.target_info.color_target_descriptions = color_target_descriptions;

	// Create pipeline
	graphics_pipeline = SDL_CreateGPUGraphicsPipeline(device.get(), &pipeline_info);

	if (!graphics_pipeline)
	{
		throw std::runtime_error{ std::format("Could not create GPU graphics pipeline: {}", SDL_GetError()) };
	}

	std::cout << "GPU graphics pipeline was created." << std::endl;
}

graphics::GpuGraphicsPipeline::~GpuGraphicsPipeline()
{
	if (graphics_pipeline)
	{
		SDL_ReleaseGPUGraphicsPipeline(device.get(), graphics_pipeline);
		std::cout << "GPU graphics pipeline deleted." << std::endl;
	}
}
