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

#include "Vertex.hpp"


namespace graphics
{
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

		std::shared_ptr<GpuTexture> loadTexture(const std::filesystem::path& path, const std::string& name);

		//Getters
		glm::vec2 getView() const;
		float getZoom() const;
		float getAngle() const;
		glm::ivec2 getWindowSize() const;
		std::vector<Vertex>& getVertices();

		//Setters
		void setView(glm::vec2 view);
		void setZoom(float zoom);
		void setAngle(float angle);

		template<typename Self>
		auto&& getDevice(this Self&& self);


		//void renderTriangle(float x1, float y1, float x2, float y2, float x3, float y3, SDL_FColor color);
		void renderRectangle(float x, float y, float w, float h, RenderType render_type, Color color, bool ignore_view_zoom = false);
		void renderSprite(const Sprite& sprite, float x, float y, float w, float h, float angle, SDL_FlipMode flip = SDL_FLIP_NONE, bool ignore_view_zoom = false);
	private:
		void initSamplers();

		Window& window;
		std::shared_ptr<SDL_GPUDevice> device = nullptr;

		// <Render Parameters> //
		glm::vec2 view = {0.0f, 0.0f};
		float zoom = 1.0f;
		float angle = 0.0f; ///< Degrees.

		std::unique_ptr<WindowClaimer> window_claimer;
		std::unique_ptr<GpuGraphicsPipeline> line_graphics_pipeline;
		std::unique_ptr<GpuGraphicsPipeline> vertex_graphics_pipeline;
		std::unique_ptr<GpuGraphicsPipeline> texture_graphics_pipeline;

		//Buffers
		//Vertex buffers
		std::unique_ptr<GpuBuffer> line_buffer;
		std::unique_ptr<GpuBuffer> vertex_buffer;
		std::unique_ptr<GpuBuffer> sprite_buffer;
		//Transfer buffer
		std::unique_ptr<GpuTransferBuffer> line_transfer_buffer;
		std::unique_ptr<GpuTransferBuffer> vertex_transfer_buffer;
		std::unique_ptr<GpuTransferBuffer> sprite_transfer_buffer;

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

		std::array<std::unique_ptr<GpuSampler>, 6> samplers;
	};

	template <typename Self>
	auto&& graphics::GpuRenderer::getDevice(this Self&& self)
	{
		return self.device.get();
	}
}
