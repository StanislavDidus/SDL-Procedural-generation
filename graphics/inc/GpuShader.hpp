#pragma once

#include "SDL3/SDL_gpu.h"
#include <filesystem>

namespace graphics
{
	struct VertexCodeDeleter
	{
		constexpr VertexCodeDeleter() = default;

		void operator()(void* vertex_code) const
		{
			if (vertex_code)
				SDL_free(vertex_code);
		}
	};

	struct GpuShaderDeleter
	{
		SDL_GPUDevice* device = nullptr;

		GpuShaderDeleter() = default;
		GpuShaderDeleter(SDL_GPUDevice* device) : device{device} {}

		void operator()(SDL_GPUShader* gpu_shader) const
		{
			if (gpu_shader && device)
				SDL_ReleaseGPUShader(device, gpu_shader);
		}
	};

	class GpuShader
	{
	public:
		GpuShader(std::shared_ptr<SDL_GPUDevice> device, const std::filesystem::path& path, int num_uniform_buffers);
		~GpuShader() = default;

		template<typename Self>
		auto&& get(this Self&& self);
	private:
		std::unique_ptr<SDL_GPUShader, GpuShaderDeleter> shader = nullptr;
		std::shared_ptr<SDL_GPUDevice> device = nullptr;
	};

	template <typename Self>
	auto&& GpuShader::get(this Self&& self)
	{
		return self.shader;
	}
}
