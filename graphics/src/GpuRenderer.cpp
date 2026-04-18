#include "GpuRenderer.hpp"
#include "Window.hpp"

#include <iostream>

#include "glm/mat4x4.hpp"
#include <SDL3_shadercross/SDL_shadercross.h>

#include "GpuSampler.hpp"
#include "Surface.hpp"
#include "TileMap.hpp"
#include "SDL3/SDL_gpu.h"
#include "CommandBuffer.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"

graphics::GpuRenderer::GpuRenderer(Window& window)
	: window{ window }
{
	//Create GPU

	bool debug = false;
#if _DEBUG
	debug = true;
#endif
	device = std::shared_ptr<SDL_GPUDevice>{ SDL_CreateGPUDevice(SDL_ShaderCross_GetHLSLShaderFormats(), debug, nullptr), SDL_DestroyGPUDevice};

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
	tilemap_vertex_shader = std::make_unique<GpuShader>(device, "shaders/TileMap.vert.hlsl", 0, 2, 2, 0);
	vertex_shader = std::make_unique<GpuShader>(device, "shaders/VertexPositionMatrix.vert.hlsl", 0, 1, 0, 0);
	fragment_shader = std::make_unique<GpuShader>(device, "shaders/SolidColor.frag.hlsl", 0, 0, 0, 0);
	texture_vertex_shader = std::make_unique<GpuShader>(device, "shaders/PullSpriteBatch.vert.hlsl", 0, 2, 1, 0);
	texture_fragment_shader = std::make_unique<GpuShader>(device, "shaders/TextureQuadColor.frag.hlsl", 1, 0, 0, 0);

	std::cout << "Shaders initialized." << std::endl;

	initSamplers();

	// TileMap graphics pipeline
	{
		tilemap_graphics_pipeline = std::make_unique<GpuGraphicsPipeline>(device, window, *tilemap_vertex_shader, *texture_fragment_shader, SDL_GPU_PRIMITIVETYPE_TRIANGLELIST);
	}

	// Line graphics pipeline
	{
		std::vector<SDL_GPUVertexBufferDescription> vertex_buffer_descriptions;
		vertex_buffer_descriptions.emplace_back(0, static_cast<Uint32>(sizeof(Vertex)), SDL_GPU_VERTEXINPUTRATE_VERTEX, 0);

		std::vector<SDL_GPUVertexAttribute> vertex_attributes;
		vertex_attributes.emplace_back(0u, 0u, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, 0u);
		vertex_attributes.emplace_back(1u, 0u, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, static_cast<Uint32>(3 * sizeof(float)));

		line_graphics_pipeline = std::make_unique<GpuGraphicsPipeline>(device, window, *vertex_shader, *fragment_shader, SDL_GPU_PRIMITIVETYPE_LINELIST, vertex_buffer_descriptions, vertex_attributes);
	}

	// Vertex graphics pipeline
	{
		std::vector<SDL_GPUVertexBufferDescription> vertex_buffer_descriptions;
		vertex_buffer_descriptions.emplace_back(0, static_cast<Uint32>(sizeof(Vertex)), SDL_GPU_VERTEXINPUTRATE_VERTEX, 0);

		std::vector<SDL_GPUVertexAttribute> vertex_attributes;
		vertex_attributes.emplace_back(0u, 0u, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, 0u);
		vertex_attributes.emplace_back(1u, 0u, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, static_cast<Uint32>(3 * sizeof(float)));

		vertex_graphics_pipeline = std::make_shared<GpuGraphicsPipeline>(device, window, *vertex_shader, *fragment_shader, SDL_GPU_PRIMITIVETYPE_TRIANGLELIST, vertex_buffer_descriptions, vertex_attributes);

	}

	// TextureVertex graphics pipeline
	{
		texture_graphics_pipeline = std::make_shared<GpuGraphicsPipeline>(device, window, *texture_vertex_shader, *texture_fragment_shader, SDL_GPU_PRIMITIVETYPE_TRIANGLELIST);
	}

	// Allocate enough memory in vertex buffers
	Uint32 line_buffer_size = static_cast<Uint32>(TOTAL_NUMBER_OBJECTS * 8 * sizeof(Vertex));
	line_buffer = std::make_unique<GpuBuffer>(device, line_buffer_size, SDL_GPU_BUFFERUSAGE_VERTEX);

	line_transfer_buffer = std::make_unique<GpuTransferBuffer>(
		device, static_cast<Uint32>(line_buffer_size), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD);

	Uint32 sprite_buffer_size = static_cast<Uint32>(TOTAL_NUMBER_OBJECTS * sizeof(SpriteData));
	sprite_buffer = std::make_unique<GpuBuffer>(device, sprite_buffer_size, SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ);

	sprite_transfer_buffer = std::make_unique<GpuTransferBuffer>(
		device, static_cast<Uint32>(sprite_buffer_size), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD);

	Uint32 vertex_buffer_size = static_cast<Uint32>(TOTAL_NUMBER_OBJECTS * 6 * sizeof(Vertex));
	vertex_buffer = std::make_unique<GpuBuffer>(device, vertex_buffer_size, SDL_GPU_BUFFERUSAGE_VERTEX);


	vertex_transfer_buffer = std::make_unique<GpuTransferBuffer>(
		device, static_cast<Uint32>(vertex_buffer_size), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD);

	sprite_batch = std::make_unique<SpriteBatch>(device, texture_graphics_pipeline);
	ui_sprite_batch = std::make_unique<SpriteBatch>(device, texture_graphics_pipeline);
	rectangle_batch = std::make_unique<RectangleBatch>(device, vertex_graphics_pipeline);
	ui_rectangle_batch = std::make_unique<RectangleBatch>(device, vertex_graphics_pipeline);
}


void graphics::GpuRenderer::initSamplers()
{
	createNewSampler("PointClamp", SDL_GPU_FILTER_NEAREST, SDL_GPU_SAMPLERMIPMAPMODE_NEAREST, SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE);
	createNewSampler("PointWrap", SDL_GPU_FILTER_NEAREST, SDL_GPU_SAMPLERMIPMAPMODE_NEAREST, SDL_GPU_SAMPLERADDRESSMODE_REPEAT);
	createNewSampler("LinearClamp", SDL_GPU_FILTER_LINEAR, SDL_GPU_SAMPLERMIPMAPMODE_LINEAR, SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE);
	createNewSampler("LinearWrap", SDL_GPU_FILTER_LINEAR, SDL_GPU_SAMPLERMIPMAPMODE_LINEAR, SDL_GPU_SAMPLERADDRESSMODE_REPEAT);
	createNewSampler("AnisotropicClamp", SDL_GPU_FILTER_LINEAR, SDL_GPU_SAMPLERMIPMAPMODE_LINEAR, SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE, 4);
	createNewSampler("AnisotropicWrap", SDL_GPU_FILTER_LINEAR, SDL_GPU_SAMPLERMIPMAPMODE_LINEAR, SDL_GPU_SAMPLERADDRESSMODE_REPEAT, 4);

	std::cout << "Samplers initialized." << std::endl;
}

void graphics::GpuRenderer::render
(
	CommandBuffer& command_buffer,
	SDL_GPUColorTargetInfo& target_info,
	glm::mat4& matrix,
	const std::vector<DrawData>& draw_buffer_,
	SpriteBatch& sprite_batch_,
	RectangleBatch& rectangle_batch_, bool& first_render
)
{
	sprite_batch_.setMatrix(matrix);
	rectangle_batch_.setMatrix(matrix);
	for (const auto& draw_data : draw_buffer_)
	{
		std::visit([&](auto&& data)
		{
			using T = std::decay_t<decltype(data)>;
			if constexpr (std::is_same_v<T, GpuSprite>)
			{
				rectangle_batch_.flushBatch(command_buffer, target_info, first_render);

				if (!sprite_batch_.canBatch(data))
				{
					sprite_batch_.flushBatch(command_buffer, target_info, first_render);
				}
				sprite_batch_.addToBatch(data.data, data.texture);
			}
			else if constexpr (std::is_same_v<T, RectangleData>)
			{
				sprite_batch_.flushBatch(command_buffer, target_info, first_render);
				if (!rectangle_batch_.canBatch(data))
				{
					rectangle_batch_.flushBatch(command_buffer, target_info, first_render);
				}
				rectangle_batch_.addToBatch(data);
			}
			else if constexpr (std::is_same_v<T, TileMapData>)
			{
				sprite_batch_.flushBatch(command_buffer, target_info, first_render);
				rectangle_batch_.flushBatch(command_buffer, target_info, first_render);
				renderTileMap(data, command_buffer, target_info, matrix, first_render);
			}
		}, draw_data);
	}
	sprite_batch_.flushBatch(command_buffer, target_info, first_render);
	sprite_batch_.reset();
	rectangle_batch_.flushBatch(command_buffer, target_info, first_render);
	rectangle_batch_.reset();

}

void graphics::GpuRenderer::renderTileMap(const TileMapData& tile_map_data, CommandBuffer& command_buffer,
                                          SDL_GPUColorTargetInfo& target_info, const glm::mat4& matrix, bool& first_render) const
{
	if (first_render)
	{
		target_info.load_op = SDL_GPU_LOADOP_CLEAR;
		first_render = false;
	}
	else
	{
		target_info.load_op = SDL_GPU_LOADOP_LOAD;
	}

	SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer.get(), &target_info, 1, nullptr);
	//Draw tilemap
	const auto& tilemap = tile_map_data.tile_map;
	SDL_BindGPUGraphicsPipeline(render_pass, tilemap_graphics_pipeline->get());

	SDL_GPUTextureSamplerBinding texture_sampler_binding = {};
	texture_sampler_binding.texture = tilemap->getTexture()->get();
	texture_sampler_binding.sampler = tilemap->getTexture()->getSampler()->get();
	SDL_BindGPUFragmentSamplers(render_pass, 0, &texture_sampler_binding, 1);

	SDL_GPUBuffer* buffer = tilemap->getTileBuffer();
	SDL_BindGPUVertexStorageBuffers(render_pass, 0, &buffer, 1);
	SDL_GPUBuffer* sprite_buffer = tilemap->getSpriteBuffer();
	SDL_BindGPUVertexStorageBuffers(render_pass, 1, &sprite_buffer, 1);
	SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &matrix, sizeof(glm::mat4));
	SDL_DrawGPUPrimitives(render_pass, 6, tilemap->getSize(), 0, 0);

	SDL_EndGPURenderPass(render_pass);
}

void graphics::GpuRenderer::updateBuffers()
{
/*	if (!lines.empty())
	{
		line_transfer_buffer->putAutomatically<Vertex>(lines.data(), lines.size() * sizeof(Vertex), 0, true);
		
		CommandBuffer command_buffer{ device };

		SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

		// Where is
		SDL_GPUTransferBufferLocation location;
		location.transfer_buffer = line_transfer_buffer->get();
		location.offset = 0;

		// Where to
		SDL_GPUBufferRegion region{};
		region.buffer = line_buffer->get();
		region.size = lines.size() * sizeof(Vertex);
		region.offset = 0;

		// Upload
		SDL_UploadToGPUBuffer(copy_pass, &location, &region, true);

		SDL_EndGPUCopyPass(copy_pass);

		command_buffer.submit();
	}
	
	if (!ui_lines.empty())
	{
		line_transfer_buffer->putAutomatically<Vertex>(ui_lines.data(), ui_lines.size() * sizeof(Vertex), ALLOCATED_NUMBER_OBJECTS * 8, false);

		CommandBuffer command_buffer{ device };

		//std::cout << "Command tile_buffer acquired." << std::endl;

		SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

		// Where is
		SDL_GPUTransferBufferLocation location;
		location.transfer_buffer = line_transfer_buffer->get();
		location.offset = ALLOCATED_NUMBER_OBJECTS * 8 * sizeof(Vertex);

		// Where to
		SDL_GPUBufferRegion region{};
		region.buffer = line_buffer->get();
		region.size = ui_lines.size() * sizeof(Vertex);
		region.offset = ALLOCATED_NUMBER_OBJECTS * 8 * sizeof(Vertex);

		// Upload
		SDL_UploadToGPUBuffer(copy_pass, &location, &region, false);

		SDL_EndGPUCopyPass(copy_pass);

		command_buffer.submit();
	}

	if (!vertices.empty())
	{
		vertex_transfer_buffer->putAutomatically(vertices.data(), vertices.size() * sizeof(Vertex), 0, true);

		std::unique_ptr<SDL_GPUCommandBuffer, GPUCommandBufferDeleter> command_buffer{ SDL_AcquireGPUCommandBuffer(device.get()) };

		if (!command_buffer)
		{
			throw std::runtime_error{ std::format("SDL_AcquireGPUCommandBuffer failed: {}", SDL_GetError()) };
		}

		//std::cout << "Command tile_buffer acquired." << std::endl;

		SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

		// Where is
		SDL_GPUTransferBufferLocation location;
		location.transfer_buffer = vertex_transfer_buffer->get();
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

	if (!ui_vertices.empty())
	{
		vertex_transfer_buffer->putAutomatically(ui_vertices.data(), ui_vertices.size() * sizeof(Vertex), ALLOCATED_NUMBER_OBJECTS * 4, false);

		std::unique_ptr<SDL_GPUCommandBuffer, GPUCommandBufferDeleter> command_buffer{ SDL_AcquireGPUCommandBuffer(device.get()) };

		if (!command_buffer)
		{
			throw std::runtime_error{ std::format("SDL_AcquireGPUCommandBuffer failed: {}", SDL_GetError()) };
		}

		//std::cout << "Command tile_buffer acquired." << std::endl;

		SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

		// Where is
		SDL_GPUTransferBufferLocation location;
		location.transfer_buffer = vertex_transfer_buffer->get();
		location.offset = ALLOCATED_NUMBER_OBJECTS * 4 * sizeof(Vertex);

		// Where to
		SDL_GPUBufferRegion region{};
		region.buffer = vertex_buffer->get();
		region.size = ui_vertices.size() * sizeof(Vertex);
		region.offset = ALLOCATED_NUMBER_OBJECTS * 4 * sizeof(Vertex);

		// Upload
		SDL_UploadToGPUBuffer(copy_pass, &location, &region, false);

		SDL_EndGPUCopyPass(copy_pass);
	}

	if (!sprites.empty())
	{
		size_t objects_size = sprites.size();

		SpriteData* data = sprite_transfer_buffer->map<SpriteData>(true);

		size_t i = 0;
		for (const auto& sprite : sprites)
		{
			data[i++] = sprite.data;
		}

		sprite_transfer_buffer->unmap();

		std::unique_ptr<SDL_GPUCommandBuffer, GPUCommandBufferDeleter> command_buffer{ SDL_AcquireGPUCommandBuffer(device.get()) };
		SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

		// Upload vertices
		SDL_GPUTransferBufferLocation vertices_transfer_info = {};
		vertices_transfer_info.transfer_buffer = sprite_transfer_buffer->get();
		vertices_transfer_info.offset = 0;
		SDL_GPUBufferRegion vertices_buffer_region = {};
		vertices_buffer_region.buffer = sprite_buffer->get();
		vertices_buffer_region.offset = 0;
		vertices_buffer_region.size = objects_size * sizeof(SpriteData);

		SDL_UploadToGPUBuffer(copy_pass, &vertices_transfer_info, &vertices_buffer_region, true);

		SDL_EndGPUCopyPass(copy_pass);
	}

	if (!ui_sprites.empty())
	{
		size_t objects_size = ui_sprites.size();

		SpriteData* data = sprite_transfer_buffer->map<SpriteData>(false);

		data += ALLOCATED_NUMBER_OBJECTS;

		size_t i = 0;
		for (const auto& sprite : ui_sprites)
		{
			data[i++] = sprite.data;
		}

		sprite_transfer_buffer->unmap();


		std::unique_ptr<SDL_GPUCommandBuffer, GPUCommandBufferDeleter> command_buffer{ SDL_AcquireGPUCommandBuffer(device.get()) };
		SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

		// Upload vertices
		SDL_GPUTransferBufferLocation vertices_transfer_info = {};
		vertices_transfer_info.transfer_buffer = sprite_transfer_buffer->get();
		vertices_transfer_info.offset = ALLOCATED_NUMBER_OBJECTS * sizeof(SpriteData);
		SDL_GPUBufferRegion vertices_buffer_region = {};
		vertices_buffer_region.buffer = sprite_buffer->get();
		vertices_buffer_region.offset = ALLOCATED_NUMBER_OBJECTS * sizeof(SpriteData);
		vertices_buffer_region.size = objects_size * sizeof(SpriteData);

		SDL_UploadToGPUBuffer(copy_pass, &vertices_transfer_info, &vertices_buffer_region, false);

		SDL_EndGPUCopyPass(copy_pass);
	}*/
}

void graphics::GpuRenderer::update()
{
	updateBuffers();

	CommandBuffer command_buffer{ device };

	SDL_GPUTexture* swapchain_texture;
	uint32_t width, height;

	if (!SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer.get(), window.get(), &swapchain_texture, &width, &height))
	{
		throw std::runtime_error{ std::format("SDL_WaitAndAcquireGPUSwapchainTexture failed: {}", SDL_GetError()) };
	}

	ImGui::Render();

	ImDrawData* draw_data = ImGui::GetDrawData();
	ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, command_buffer.get());

	//.........................//
	//<Calculate object matrix>//
	//.........................//
	const auto& window_size = window.getWindowSize();
	const auto& view_position = view;
	glm::vec2 mid_screen = { window_size.x * 0.5f, window_size.y * 0.5f };

	glm::mat4 view_matrix = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ -view_position, 0.0f });
	glm::mat4 to_origin = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ -mid_screen, 0.0f });
	glm::mat4 from_origin = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ mid_screen, 0.0f });

	glm::mat4 scale = glm::scale(glm::mat4{ 1.0f }, glm::vec3{ zoom });
	glm::mat4 rotation = glm::rotate(glm::mat4{ 1.0f }, glm::radians(angle), glm::vec3{ 0.0f, 0.0f, 1.0f });

	glm::mat4 projection = glm::ortho(
		0.0f, static_cast<float>(window_size.x),
		static_cast<float>(window_size.y), 0.0f
	);
	//.........................//
	//<_______________________>//
	//.........................//

	glm::mat4 base_matrix = glm::transpose(projection);
	world_matrix = glm::transpose(projection * from_origin * rotation * scale * to_origin * view_matrix);

	if (swapchain_texture)
	{
		SDL_GPUColorTargetInfo target_info
		{
			.texture = swapchain_texture,
			.clear_color = {0.0f, 0.0f, 0.0f, 1.0f},
			.load_op = SDL_GPU_LOADOP_CLEAR,
			.store_op = SDL_GPU_STOREOP_STORE,
			.cycle = false
		};

		bool first_render = true;
		render(command_buffer, target_info, world_matrix, draw_buffer, *sprite_batch, *rectangle_batch, first_render);
		render(command_buffer, target_info, base_matrix, ui_draw_buffer, *ui_sprite_batch, *ui_rectangle_batch, first_render);

		/*if (!lines.empty())
		{
			SDL_BindGPUGraphicsPipeline(render_pass, line_graphics_pipeline->get());

			SDL_GPUBufferBinding buffer_binding[1];
			buffer_binding[0].buffer = line_buffer->get();
			buffer_binding[0].offset = 0;

			SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &world_matrix, sizeof(glm::mat4));

			SDL_BindGPUVertexBuffers(render_pass, 0, buffer_binding, 1);
			SDL_DrawGPUPrimitives(render_pass, lines.size(), 1, 0, 0);
		}

		SDL_BindGPUGraphicsPipeline(render_pass, texture_graphics_pipeline->get());
		SDL_BindGPUVertexStorageBuffers(render_pass, 0, &sprite_buffer->get(), 1);
		SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &world_matrix, sizeof(glm::mat4));
		SpriteUniform sprite_uniform{};
		int i = 0;
		for (const auto& sprite : sprites)
		{
			sprite_uniform.index = i;
			SDL_PushGPUVertexUniformData(command_buffer.get(), 1, &sprite_uniform, sizeof(SpriteUniform));

			SDL_GPUTextureSamplerBinding texture_sampler_binding = {};
			texture_sampler_binding.texture = sprite.texture->get();
			texture_sampler_binding.sampler = sprite.texture->getSampler()->get();
			SDL_BindGPUFragmentSamplers(render_pass, 0, &texture_sampler_binding, 1);

			SDL_DrawGPUPrimitives(render_pass, 6, 1, 0, 0);
			++i;
		}

		//Draw tilemap
		for (auto& tilemap : tilemaps)
		{
			SDL_BindGPUGraphicsPipeline(render_pass, tilemap_graphics_pipeline->get());

			SDL_GPUTextureSamplerBinding texture_sampler_binding = {};
			texture_sampler_binding.texture = tilemap->getTexture()->get();
			texture_sampler_binding.sampler = tilemap->getTexture()->getSampler()->get();
			SDL_BindGPUFragmentSamplers(render_pass, 0, &texture_sampler_binding, 1);

			SDL_GPUBuffer* buffer = tilemap->getTileBuffer();
			SDL_BindGPUVertexStorageBuffers(render_pass, 0, &buffer, 1);
			SDL_GPUBuffer* sprite_buffer = tilemap->getSpriteBuffer();
			SDL_BindGPUVertexStorageBuffers(render_pass, 1, &sprite_buffer, 1);
			SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &world_matrix, sizeof(glm::mat4));
			SDL_DrawGPUPrimitives(render_pass, 6, tilemap->getSize(), 0, 0);
			//std::cout << "Tilemap drawn" << std::endl;
		}
		
		if (!vertices.empty())
		{
			SDL_BindGPUGraphicsPipeline(render_pass, vertex_graphics_pipeline->get());

			SDL_GPUBufferBinding buffer_bindings[1];

			buffer_bindings[0].buffer = vertex_buffer->get();
			buffer_bindings[0].offset = 0;

			SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &world_matrix, sizeof(glm::mat4));

			SDL_BindGPUVertexBuffers(render_pass, 0, buffer_bindings, 1);
			SDL_DrawGPUPrimitives(render_pass, vertices.size(), 1, 0, 0);
		}

		
		if (!ui_lines.empty())
		{
			SDL_BindGPUGraphicsPipeline(render_pass, line_graphics_pipeline->get());

			SDL_GPUBufferBinding buffer_binding[1];
			buffer_binding[0].buffer = line_buffer->get();
			buffer_binding[0].offset = ALLOCATED_NUMBER_OBJECTS * 8 * sizeof(Vertex);

			SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &base_matrix, sizeof(glm::mat4));

			SDL_BindGPUVertexBuffers(render_pass, 0, buffer_binding, 1);
			SDL_DrawGPUPrimitives(render_pass, ui_lines.size(), 1, 0, 0);
		}


		SDL_BindGPUGraphicsPipeline(render_pass, texture_graphics_pipeline->get());
		SDL_BindGPUVertexStorageBuffers(render_pass, 0, &sprite_buffer->get(), 1);
		i = 0;
		for (const auto& sprite : ui_sprites)
		{
			sprite_uniform.index = ALLOCATED_NUMBER_OBJECTS + i;
			SDL_PushGPUVertexUniformData(command_buffer.get(), 1, &sprite_uniform, sizeof(SpriteUniform));

			SDL_GPUTextureSamplerBinding texture_sampler_binding = {};
			texture_sampler_binding.texture = sprite.texture->get();
			texture_sampler_binding.sampler = sprite.texture->getSampler()->get();

			SDL_BindGPUFragmentSamplers(render_pass, 0, &texture_sampler_binding, 1);
			SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &base_matrix, sizeof(glm::mat4));
			SDL_DrawGPUPrimitives(render_pass, 6, 1, 0, 0);
			++i;
		}

		if (!ui_vertices.empty())
		{
			SDL_BindGPUGraphicsPipeline(render_pass, vertex_graphics_pipeline->get());

			SDL_GPUBufferBinding buffer_bindings[1];

			buffer_bindings[0].buffer = vertex_buffer->get();
			buffer_bindings[0].offset = ALLOCATED_NUMBER_OBJECTS * 4 * sizeof(Vertex);

			SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &base_matrix, sizeof(glm::mat4));

			SDL_BindGPUVertexBuffers(render_pass, 0, buffer_bindings, 1);
			SDL_DrawGPUPrimitives(render_pass, ui_vertices.size(), 1, 0, 0);
		}*/


		// Render ImGui
		//ImGui_ImplSDLGPU3_RenderDrawData(draw_data, command_buffer.get(), render_pass);

		//SDL_EndGPURenderPass(render_pass);
	}

	draw_buffer.clear();
	ui_draw_buffer.clear();
	lines.clear();
	ui_lines.clear();
	sprites.clear();
	ui_sprites.clear();
	vertices.clear();
	ui_vertices.clear();
	tilemaps.clear();
}

std::shared_ptr<graphics::GpuTexture> graphics::GpuRenderer::loadTexture(const Surface& surface)
{
	auto texture = std::make_shared<GpuTexture>(device, surface, samplers["PointClamp"]);

	// Upload texture on the GPU
	GpuTransferBuffer texture_transfer_buffer{ device, static_cast<Uint32>(texture->w() * texture->h() * 4), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD };

	std::unique_ptr<SDL_GPUCommandBuffer, GPUCommandBufferDeleter> command_buffer{ SDL_AcquireGPUCommandBuffer(device.get()) };
	SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer.get());

	Uint8* texture_data = texture_transfer_buffer.map<Uint8>(false);

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

	return texture;
}

void graphics::GpuRenderer::createNewSampler(const std::string& name, SDL_GPUFilter filter,
                                             SDL_GPUSamplerMipmapMode mipmap, SDL_GPUSamplerAddressMode address_mode, std::optional<int> anisotropy)
{
	auto it = samplers.find(name);
	if (it != samplers.end())
	{
		throw std::runtime_error{ std::format("Sampler with this name already exists.") };
	}
	else
	{
		samplers[name] = std::make_shared<GpuSampler>(device, filter, mipmap, address_mode, anisotropy);
	}
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

const glm::mat4& graphics::GpuRenderer::getWorldMatrix() const
{
	return world_matrix;
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

void graphics::GpuRenderer::renderRectangle(float x, float y, float w, float h, RenderType render_type, glm::vec4 color, bool ignore_view_zoom)
{
	if (!ignore_view_zoom)
	{
		if (render_type == RenderType::FILL)
		{
			draw_buffer.push_back(RectangleData{ glm::vec2{x,y},glm::vec2{w,h}, color });
			/*
			vertices.emplace_back(x, y, 0.0f, color.r, color.g, color.b, color.a);
			vertices.emplace_back(x, y + h, 0.0f, color.r, color.g, color.b, color.a);
			vertices.emplace_back(x + w, y + h, 0.0f, color.r, color.g, color.b, color.a);

			vertices.emplace_back(x + w, y + h, 0.0f, color.r, color.g, color.b, color.a);
			vertices.emplace_back(x + w, y, 0.0f, color.r, color.g, color.b, color.a);
			vertices.emplace_back(x, y, 0.0f, color.r, color.g, color.b, color.a);
		*/
		}
		else if (render_type == RenderType::NONE)
		{
			draw_buffer.push_back(LineData{ x,y,x + w,y + h, color });

			/*lines.emplace_back(x, y, 0.0f, color.r, color.g, color.b, color.a);
			lines.emplace_back(x, y + h, 0.0f, color.r, color.g, color.b, color.a);
			
			lines.emplace_back(x, y + h, 0.0f, color.r, color.g, color.b, color.a);
			lines.emplace_back(x + w, y + h, 0.0f, color.r, color.g, color.b, color.a);

			lines.emplace_back(x + w, y + h, 0.0f, color.r, color.g, color.b, color.a);
			lines.emplace_back(x + w, y, 0.0f, color.r, color.g, color.b, color.a);

			lines.emplace_back(x + w, y, 0.0f, color.r, color.g, color.b, color.a);
			lines.emplace_back(x, y, 0.0f, color.r, color.g, color.b, color.a);*/
		}

		if (vertices.size() > ALLOCATED_NUMBER_OBJECTS)
			throw std::runtime_error{ std::format("Reached the maximum available number of object being renderer at the same time.\n"
																"Increase the limit inside GpuRenderer.hpp file.") };
	}
	else
	{
		if (render_type == RenderType::FILL)
		{	
			ui_draw_buffer.push_back(RectangleData{ glm::vec2{x,y},glm::vec2{w,h}, color });

			/*ui_vertices.emplace_back(x, y, 0.0f, color.r, color.g, color.b, color.a);
			ui_vertices.emplace_back(x, y + h, 0.0f, color.r, color.g, color.b, color.a);
			ui_vertices.emplace_back(x + w, y + h, 0.0f, color.r, color.g, color.b, color.a);

			ui_vertices.emplace_back(x + w, y + h, 0.0f, color.r, color.g, color.b, color.a);
			ui_vertices.emplace_back(x + w, y, 0.0f, color.r, color.g, color.b, color.a);
			ui_vertices.emplace_back(x, y, 0.0f, color.r, color.g, color.b, color.a);*/
		}
		else if (render_type == RenderType::NONE)
		{
			ui_draw_buffer.emplace_back(LineData{ x,y,x + w,y + h, color });
			/*
			ui_lines.emplace_back(x, y, 0.0f, color.r, color.g, color.b, color.a);
			ui_lines.emplace_back(x, y + h, 0.0f, color.r, color.g, color.b, color.a);
			
			ui_lines.emplace_back(x, y + h, 0.0f, color.r, color.g, color.b, color.a);
			ui_lines.emplace_back(x + w, y + h, 0.0f, color.r, color.g, color.b, color.a);

			ui_lines.emplace_back(x + w, y + h, 0.0f, color.r, color.g, color.b, color.a);
			ui_lines.emplace_back(x + w, y, 0.0f, color.r, color.g, color.b, color.a);

			ui_lines.emplace_back(x + w, y, 0.0f, color.r, color.g, color.b, color.a);
			ui_lines.emplace_back(x, y, 0.0f, color.r, color.g, color.b, color.a);
		*/
		}

		if (ui_vertices.size() > ALLOCATED_NUMBER_UI_OBJECTS)
			throw std::runtime_error{ std::format("Reached the maximum available number of object being renderer at the same time.\n"
																"Increase the limit inside GpuRenderer.hpp file.") };
	}
}

void graphics::GpuRenderer::renderSprite(const Sprite& sprite, float x, float y, float w, float h, float angle, SDL_FlipMode flip, bool ignore_view_zoom)
{
	renderTexture(sprite.getTexture(), sprite.getRect(), SDL_FRect{ x,y,w,h }, angle, flip, ignore_view_zoom);
}

void graphics::GpuRenderer::renderTexture(std::shared_ptr<GpuTexture> texture, std::optional<SDL_FRect> source,
                                          std::optional<SDL_FRect> destination, float angle, SDL_FlipMode flip, bool ignore_view_zoom)
{
	//flip = SDL_FLIP_HORIZONTAL;
	SDL_FRect src = source.value_or(SDL_FRect{ 0.0f, 0.0f, static_cast<float>(texture->w()), static_cast<float>(texture->h()) });
	SDL_FRect dst = destination.value_or(SDL_FRect{ 0.0f, 0.0f, static_cast<float>(getWindowSize().x), static_cast<float>(getWindowSize().y) });

	if (!ignore_view_zoom)
	{
		draw_buffer.emplace_back(GpuSprite
		{
			texture,
			SpriteData
			{
			.pos_rot{dst.x, dst.y, 0.0f, angle},
			.size{dst.w, dst.h, 0.0f, 0.0f},
			.uv{src.x, src.y, src.w, src.h},
			.color{1.0f, 1.0f, 1.0f, 1.0f},
			.flip{static_cast<float>(static_cast<unsigned int>(flip)), 0.0f, 0.0f, 0.0f}
			}
		});

		if (sprites.size() > ALLOCATED_NUMBER_OBJECTS)
		{
			throw std::runtime_error{ std::format("Reached the maximum available number of object being renderer at the same time.\n"
																"Increase the limit inside GpuRenderer.hpp file.") };
		}
	}
	else
	{
		ui_draw_buffer.emplace_back(GpuSprite
		{
			texture,
			SpriteData
			{
			.pos_rot{dst.x, dst.y, 0.0f, angle},
			.size{dst.w, dst.h, 0.0f, 0.0f},
			.uv{src.x, src.y, src.w, src.h},
			.color{1.0f, 1.0f, 1.0f, 1.0f},
			.flip{static_cast<float>(static_cast<unsigned int>(flip)), 0.0f, 0.0f, 0.0f}
			}
		});

		if (ui_sprites.size() > ALLOCATED_NUMBER_UI_OBJECTS)
		{
			throw std::runtime_error{ std::format("Reached the maximum available number of object being renderer at the same time.\n"
																"Increase the limit inside GpuRenderer.hpp file.") };
		}
	}
}

void graphics::GpuRenderer::renderTileMap(std::shared_ptr<TileMap> tilemap, float x, float y)
{
	draw_buffer.emplace_back(TileMapData{tilemap});
}

