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

	class GpuShader
	{
	public:
		GpuShader() = default;
		GpuShader(std::shared_ptr<SDL_GPUDevice> device, const std::filesystem::path& path, int sampler_count, int uniform_count, int storage_buffer_count, int storage_texture_count);
		~GpuShader();

		template<typename Self>
		auto&& get(this Self&& self);
	private:
		SDL_GPUShader* shader = nullptr;
		std::shared_ptr<SDL_GPUDevice> device = nullptr;
	};

	template <typename Self>
	auto&& GpuShader::get(this Self&& self)
	{
		return self.shader;
	}
}
