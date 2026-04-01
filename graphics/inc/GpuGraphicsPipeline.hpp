#pragma once
#include <memory>
#include <optional>
#include <vector>

#include "Window.hpp"
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
			const Window& window,
			GpuShader& vertex_shader,
			GpuShader& fragment_shader,
			SDL_GPUPrimitiveType primitive_type,
			std::optional<std::vector<SDL_GPUVertexBufferDescription>> vertex_buffer_descriptions = std::nullopt,
			std::optional<const std::vector<SDL_GPUVertexAttribute>> vertex_attributes = std::nullopt
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
