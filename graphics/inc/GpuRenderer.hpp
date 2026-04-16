#pragma once

#include <iostream>
#include <unordered_map>

#include "SDL3/SDL_gpu.h"

#include "Window.hpp"
#include "GpuShader.hpp"
#include "GpuGraphicsPipeline.hpp"
#include "GpuBuffer.hpp"
#include "GpuTexture.hpp"
#include "GpuTransferBuffer.hpp"
#include "GpuSampler.hpp"
#include "Sprite.hpp"
#include "TileMap.hpp"

#include "Vertex.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "RenderBatch.hpp"
#include "glm/gtx/transform.hpp"

namespace graphics
{
	class TileMap;
	constexpr int ALLOCATED_NUMBER_OBJECTS = 1'000;
	constexpr int ALLOCATED_NUMBER_UI_OBJECTS = ALLOCATED_NUMBER_OBJECTS;
	constexpr int TOTAL_NUMBER_OBJECTS = ALLOCATED_NUMBER_OBJECTS + ALLOCATED_NUMBER_UI_OBJECTS;

	enum class RenderType
	{
		NONE,
		FILL,
	};

	constexpr bool IGNORE_VIEW_ZOOM = true;

	struct GPUCommandBufferDeleter
	{
		constexpr GPUCommandBufferDeleter() = default;

		void operator()(SDL_GPUCommandBuffer* command_buffer) const
		{
			SDL_SubmitGPUCommandBuffer(command_buffer);
		}
	};

	struct WindowClaimer
	{
		SDL_GPUDevice* device;
		SDL_Window* window;

		WindowClaimer() = default;
		WindowClaimer(SDL_GPUDevice* device, SDL_Window* window) : device{device}, window{window} {}
		~WindowClaimer()
		{
			if (device && window)
				SDL_ReleaseWindowFromGPUDevice(device, window);
		}
	};

	/*struct UniformBuffer
	{
		float time = 0.0f;
	};

	static UniformBuffer time_uniform;*/

	struct ScreenSize
	{
		int width = 960;
		int height = 540;
	};

	struct Uniform
	{
		float time;
	};
	
	static ScreenSize screen_size_uniform;
	static Uniform time_uniform;

	class TextEngine;
	
	class GpuRenderer
	{
	public:
		explicit GpuRenderer(Window& window);
		~GpuRenderer() = default;

		void updateBuffers();
		void update();

		std::shared_ptr<GpuTexture> loadTexture(const Surface& surface);
		void createNewSampler(const std::string& name, SDL_GPUFilter filter, SDL_GPUSamplerMipmapMode mipmap, SDL_GPUSamplerAddressMode address_mode, std::optional<int> anisotropy = std::nullopt);

		//Getters
		glm::vec2 getView() const;
		float getZoom() const;
		float getAngle() const;
		glm::ivec2 getWindowSize() const;
		std::vector<Vertex>& getVertices();
		const glm::mat4& getWorldMatrix() const;

		//Setters
		void setView(glm::vec2 view);
		void setZoom(float zoom);
		void setAngle(float angle);

		template<typename Self>
		auto&& getDevice(this Self&& self);

		//void renderTriangle(float x1, float y1, float x2, float y2, float x3, float y3, SDL_FColor color);
		void renderRectangle(float x, float y, float w, float h, RenderType render_type, glm::vec4 color, bool ignore_view_zoom = false);
		void renderSprite(const Sprite& sprite, float x, float y, float w, float h, float angle, SDL_FlipMode flip = SDL_FLIP_NONE, bool ignore_view_zoom = false);
		void renderTexture(std::shared_ptr<GpuTexture> texture, std::optional<SDL_FRect> source, std::optional<SDL_FRect> destination, float angle, SDL_FlipMode flip = SDL_FLIP_NONE, bool ignore_view_zoom = false);
		void renderTileMap(std::shared_ptr<TileMap> tilemap, float x, float y);
	private:
		void initSamplers();

		Window& window;
		std::shared_ptr<SDL_GPUDevice> device = nullptr;

		std::vector<DrawData> draw_buffer;
		std::vector<DrawData> ui_draw_buffer;

		// <Render Parameters> //
		glm::vec2 view = {0.0f, 0.0f};
		float zoom = 1.0f;
		float angle = 0.0f; ///< Degrees.
		glm::mat4 world_matrix;

		std::unique_ptr<SpriteBatch> sprite_batch;
		std::unique_ptr<SpriteBatch> ui_sprite_batch;

		std::unique_ptr<WindowClaimer> window_claimer;
		std::unique_ptr<GpuGraphicsPipeline> tilemap_graphics_pipeline;
		std::unique_ptr<GpuGraphicsPipeline> line_graphics_pipeline;
		std::unique_ptr<GpuGraphicsPipeline> vertex_graphics_pipeline;
		std::shared_ptr<GpuGraphicsPipeline> texture_graphics_pipeline;

		//Buffers
		//Vertex buffers
		std::unique_ptr<GpuBuffer> line_buffer;
		std::unique_ptr<GpuBuffer> vertex_buffer;
		std::unique_ptr<GpuBuffer> sprite_buffer;
		//Transfer tile_buffer
		std::unique_ptr<GpuTransferBuffer> line_transfer_buffer;
		std::unique_ptr<GpuTransferBuffer> vertex_transfer_buffer;
		std::unique_ptr<GpuTransferBuffer> sprite_transfer_buffer;

		std::unique_ptr<GpuShader> tilemap_vertex_shader;
		std::unique_ptr<GpuShader> vertex_shader;
		std::unique_ptr<GpuShader> fragment_shader;
		std::unique_ptr<GpuShader> texture_vertex_shader;
		std::unique_ptr<GpuShader> texture_fragment_shader;

		std::vector<Vertex> lines;
		std::vector<Vertex> ui_lines;
		std::vector<Vertex> vertices;
		std::vector<Vertex> ui_vertices;
		std::vector<GpuSprite> sprites;
		std::vector<GpuSprite> ui_sprites;
		std::vector<std::shared_ptr<TileMap>> tilemaps;

		std::unordered_map<std::string, std::shared_ptr<GpuSampler>> samplers;
	};

	template <typename Self>
	auto&& graphics::GpuRenderer::getDevice(this Self&& self)
	{
		return self.device;
	}
}
