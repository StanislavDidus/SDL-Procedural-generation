#include "GpuRenderer.hpp"
#include "Window.hpp"
#include "GpuRenderFunctions.hpp"

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
	SDL_SetGPUSwapchainParameters(device.get(), window.get(), SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_MAILBOX);

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

		line_graphics_pipeline = std::make_shared<GpuGraphicsPipeline>(device, window, *vertex_shader, *fragment_shader, SDL_GPU_PRIMITIVETYPE_LINELIST, vertex_buffer_descriptions, vertex_attributes);
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

	sprite_batch = std::make_unique<SpriteBatch>(device, texture_graphics_pipeline);
	ui_sprite_batch = std::make_unique<SpriteBatch>(device, texture_graphics_pipeline);
	rectangle_batch = std::make_unique<RectangleBatch>(device, vertex_graphics_pipeline);
	ui_rectangle_batch = std::make_unique<RectangleBatch>(device, vertex_graphics_pipeline);
	line_batch = std::make_unique<LineBatch>(device, line_graphics_pipeline);
	ui_line_batch = std::make_unique<LineBatch>(device, line_graphics_pipeline);
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
	RectangleBatch& rectangle_batch_,
	LineBatch& line_batch_
)
{
	sprite_batch_.setMatrix(matrix);
	rectangle_batch_.setMatrix(matrix);
	line_batch_.setMatrix(matrix);
	for (const auto& draw_data : draw_buffer_)
	{
		std::visit([&](auto&& data)
		{
			using T = std::decay_t<decltype(data)>;
			if constexpr (std::is_same_v<T, GpuSprite>)
			{
				//Flush the rest of the batches
				rectangle_batch_.flushBatch(command_buffer, target_info);
				line_batch_.flushBatch(command_buffer, target_info);
				
				// If objects cannot be put in a batch we flush it
				if (!sprite_batch_.canBatch(data))
				{
					sprite_batch_.flushBatch(command_buffer, target_info);
				}
				// Add to batch in the end
				sprite_batch_.addToBatch(data);
			}
			else if constexpr (std::is_same_v<T, RectangleData>)
			{
				sprite_batch_.flushBatch(command_buffer, target_info);
				line_batch_.flushBatch(command_buffer, target_info);
				
				if (!rectangle_batch_.canBatch(data))
				{
					rectangle_batch_.flushBatch(command_buffer, target_info);
				}
				rectangle_batch_.addToBatch(data);
			}
			else if constexpr (std::is_same_v<T, LineData>)
			{
				rectangle_batch_.flushBatch(command_buffer, target_info)	;
				sprite_batch_.flushBatch(command_buffer, target_info);
				
				if (!line_batch_.canBatch(data))
				{
					line_batch_.flushBatch(command_buffer, target_info);
				}
				line_batch_.addToBatch(data);
			}
			else if constexpr (std::is_same_v<T, ChunkData>)
			{
				sprite_batch_.flushBatch(command_buffer, target_info);
				rectangle_batch_.flushBatch(command_buffer, target_info);
				line_batch_.flushBatch(command_buffer, target_info);
				
				renderChunk(data, command_buffer, target_info, matrix);
			}
		}, draw_data);
	}
	sprite_batch_.flushBatch(command_buffer, target_info);
	sprite_batch_.reset();
	rectangle_batch_.flushBatch(command_buffer, target_info);
	rectangle_batch_.reset();
	line_batch_.flushBatch(command_buffer, target_info);
	line_batch_.reset();
}

void graphics::GpuRenderer::renderChunk(const ChunkData& chunk_data, CommandBuffer& command_buffer,
                                          SDL_GPUColorTargetInfo& target_info, const glm::mat4& matrix) const
{
	SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer.get(), &target_info, 1, nullptr);
	target_info.load_op = SDL_GPU_LOADOP_LOAD;
	const auto& chunk = chunk_data.chunk;
	SDL_BindGPUGraphicsPipeline(render_pass, tilemap_graphics_pipeline->get());

	SDL_GPUTextureSamplerBinding texture_sampler_binding = {};
	texture_sampler_binding.texture = chunk->getTexture()->get();
	texture_sampler_binding.sampler = chunk->getTexture()->getSampler()->get();
	SDL_BindGPUFragmentSamplers(render_pass, 0, &texture_sampler_binding, 1);

	SDL_GPUBuffer* buffer = chunk->getTileBuffer();
	SDL_BindGPUVertexStorageBuffers(render_pass, 0, &buffer, 1);
	SDL_GPUBuffer* sprite_buffer = chunk->getSpriteBuffer();
	SDL_BindGPUVertexStorageBuffers(render_pass, 1, &sprite_buffer, 1);
	SDL_PushGPUVertexUniformData(command_buffer.get(), 0, &matrix, sizeof(glm::mat4));
	SDL_DrawGPUPrimitives(render_pass, 6, chunk->getSize(), 0, 0);

	SDL_EndGPURenderPass(render_pass);
}

void graphics::GpuRenderer::update()
{
	CommandBuffer command_buffer{ device };

	SDL_GPUTexture* swapchain_texture;

	if (!SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer.get(), window.get(), &swapchain_texture, &render_resolution_width, &render_resolution_height))
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
		
		//std::cout << std::format("Width: {}, Height: {}", width, height) << std::endl;

		render(command_buffer, target_info, world_matrix, draw_buffer, *sprite_batch, *rectangle_batch, *line_batch);
		render(command_buffer, target_info, base_matrix, ui_draw_buffer, *ui_sprite_batch, *ui_rectangle_batch, *ui_line_batch);

		// Render ImGui
		SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer.get(), &target_info, 1, nullptr);
		ImGui_ImplSDLGPU3_RenderDrawData(draw_data, command_buffer.get(), render_pass);

		SDL_EndGPURenderPass(render_pass);
	}

	draw_buffer.clear();
	ui_draw_buffer.clear();
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

glm::ivec2 graphics::GpuRenderer::getStandardWindowSize() const
{
	return window.getWindowSize();
}

const glm::mat4& graphics::GpuRenderer::getWorldMatrix() const
{
	return world_matrix;
}

glm::vec2 graphics::GpuRenderer::getRenderResolution() const
{
	return glm::vec2{static_cast<float>(render_resolution_width), static_cast<float>(render_resolution_height)};
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


void graphics::GpuRenderer::renderRectangle(float x, float y, float w, float h, RenderType render_type, glm::vec4 color, bool ignore_view_zoom)
{
	if (!ignore_view_zoom)
	{
		if (render_type == RenderType::FILL)
		{
			draw_buffer.push_back(RectangleData{ glm::vec2{x,y},glm::vec2{w,h}, color });
		}
		else if (render_type == RenderType::NONE)
		{
			draw_buffer.push_back(LineData{ x,y,x + w,y, color });
			draw_buffer.push_back(LineData{ x + w,y,x + w,y + h, color });
			draw_buffer.push_back(LineData{ x + w,y + h,x,y + h, color });
			draw_buffer.push_back(LineData{ x,y + h,x,y, color });
		}
	}
	else
	{
		if (render_type == RenderType::FILL)
		{	
			ui_draw_buffer.push_back(RectangleData{ glm::vec2{x,y},glm::vec2{w,h}, color });
		}
		else if (render_type == RenderType::NONE)
		{
			ui_draw_buffer.push_back(LineData{ x,y,x + w,y, color });
			ui_draw_buffer.push_back(LineData{ x + w,y,x + w,y + h, color });
			ui_draw_buffer.push_back(LineData{ x + w,y + h,x,y + h, color });
			ui_draw_buffer.push_back(LineData{ x,y + h,x,y, color });
		
		}
	}
}

void graphics::GpuRenderer::renderSprite(const Sprite& sprite, float x, float y, float w, float h, float angle, SDL_FlipMode flip, bool ignore_view_zoom, graphics::Color color)
{
	renderTexture(sprite.getTexture(), sprite.getRect(), SDL_FRect{ x,y,w,h }, angle, flip, ignore_view_zoom, color);
}

void graphics::GpuRenderer::renderTexture(std::shared_ptr<GpuTexture> texture, std::optional<SDL_FRect> source,
                                          std::optional<SDL_FRect> destination, float angle, SDL_FlipMode flip, bool ignore_view_zoom, graphics::Color color)
{
	//flip = SDL_FLIP_HORIZONTAL;
	SDL_FRect src = source.value_or(SDL_FRect{ 0.0f, 0.0f, static_cast<float>(texture->w()), static_cast<float>(texture->h()) });
	SDL_FRect dst = destination.value_or(SDL_FRect{ 0.0f, 0.0f, static_cast<float>(getStandardWindowSize().x), static_cast<float>(getStandardWindowSize().y) });

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
			.color{color.r, color.g, color.b, color.a},
			.flip{static_cast<float>(static_cast<unsigned int>(flip)), 0.0f, 0.0f, 0.0f}
			}
		});
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
			.color{color.r, color.g, color.b, color.a},
			.flip{static_cast<float>(static_cast<unsigned int>(flip)), 0.0f, 0.0f, 0.0f}
			}
		});
	}
}

void graphics::GpuRenderer::renderTileMap(std::shared_ptr<TileMap> tilemap, float x, float y)
{
	SDL_FRect camera_rect = graphics::getCameraRect(*this);
	
	for (const auto& chunk : tilemap->getChunks())
	{
		if (SDL_HasRectIntersectionFloat(&chunk->getRect(), &camera_rect))
		{
			draw_buffer.emplace_back(ChunkData{chunk});
		}
	}
}

