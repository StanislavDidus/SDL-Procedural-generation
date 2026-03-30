#pragma once
#include <memory>
#include <optional>

#include "SDL3/SDL_gpu.h"

namespace graphics
{
	class GpuSampler
	{
	public:
		GpuSampler() = default;
		GpuSampler
		(
			std::shared_ptr<SDL_GPUDevice> device,
			SDL_GPUFilter gpu_filter,
			SDL_GPUSamplerMipmapMode mipmap_mode,
			SDL_GPUSamplerAddressMode address_mode,
			std::optional<unsigned int> anisotropy = std::nullopt
		);
		~GpuSampler();

		template<typename Self>
		auto&& get(this Self&& self);
	private:
		std::shared_ptr<SDL_GPUDevice> device;
		SDL_GPUSampler* sampler = nullptr;
	};

	template <typename Self>
	auto&& GpuSampler::get(this Self&& self)
	{
		return self.sampler;
	}
};
