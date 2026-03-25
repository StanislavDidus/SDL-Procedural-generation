#pragma once

#include <iostream>
#include <unordered_map>

#include "SDL3/SDL_gpu.h"

#include "Window.hpp"
#include "GpuShader.hpp"
#include "GpuGraphicsPipeline.hpp"
#include "GpuVertexBuffer.hpp"
#include "GpuTexture.hpp"

#include "Vertex.hpp"

namespace graphics
{
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
	
	class GpuRenderer
	{
	public:
		explicit GpuRenderer(Window& window);
		~GpuRenderer() = default;

		void updateBuffers();
		void update();

		void loadTexture(const std::filesystem::path& path, const std::string& name);

		void renderTriangle(float x1, float y1, float x2, float y2, float x3, float y3, SDL_FColor color);
		void renderRectangle1(float x1, float y1, float x2, float y2, SDL_FColor color);
		void renderRectangle2(float x, float y, float w, float h, SDL_FColor color);
		void renderSprite(const std::string& texture_name, float x, float y, float w, float h);
	private:
		Window& window;
		std::shared_ptr<SDL_GPUDevice> device = nullptr;

		std::unique_ptr<WindowClaimer> window_claimer;
		std::unique_ptr<GpuGraphicsPipeline> graphics_pipeline;

		std::unique_ptr<GpuVertexBuffer> vertex_buffer;
		std::unique_ptr<GpuVertexBuffer> texture_vertex_buffer;
		std::unique_ptr<GpuVertexBuffer> texture_index_buffer;

		std::unique_ptr<GpuShader> vertex_shader;
		std::unique_ptr<GpuShader> fragment_shader;

		std::vector<Vertex> vertices;
		//std::vector<TextureVertex> texture_vertices;
		std::vector<TextureObject> texture_objects;

		std::unordered_map<std::string, std::shared_ptr<GpuTexture>> textures;
	};
}
