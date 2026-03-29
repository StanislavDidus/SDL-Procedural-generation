#pragma once
#include <memory>
#include <vector>

#include "SDL3/SDL_gpu.h"

namespace graphics
{
	class GpuShader;

	class GpuGraphicsPipeline
	{
	public:
		GpuGraphicsPipeline() = default;
		GpuGraphicsPipeline
		(
			std::shared_ptr<SDL_GPUDevice> device,
			SDL_Window* window,
			GpuShader& vertex_shader,
			GpuShader& fragment_shader,
			const std::vector<SDL_GPUVertexBufferDescription>& vertex_buffer_descriptions,
			const std::vector<SDL_GPUVertexAttribute>& vertex_attributes
		);
		~GpuGraphicsPipeline();

		template<typename Self>
		auto&& get(this Self&& self);
	private:
		SDL_GPUGraphicsPipeline* graphics_pipeline = nullptr;
		SDL_Window* window = nullptr;
		std::shared_ptr<SDL_GPUDevice> device;
	};

	template <typename Self>
	auto&& GpuGraphicsPipeline::get(this Self&& self)
	{
		return self.graphics_pipeline;
	}
}
