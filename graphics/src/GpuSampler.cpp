#include "GpuSampler.hpp"

#include <format>
#include <optional>
#include <stdexcept>

using namespace graphics;

GpuSampler::GpuSampler(std::shared_ptr<SDL_GPUDevice> device, SDL_GPUFilter gpu_filter,
                       SDL_GPUSamplerMipmapMode mipmap_mode, SDL_GPUSamplerAddressMode address_mode, std::optional<unsigned int> anisotropy)
		: device{device}
{
	SDL_GPUSamplerCreateInfo create_info = {};
	create_info.min_filter = gpu_filter;
	create_info.mag_filter = gpu_filter;
	create_info.mipmap_mode = mipmap_mode;
	create_info.address_mode_u = address_mode;
	create_info.address_mode_v = address_mode;
	create_info.address_mode_w = address_mode;

	if (anisotropy.has_value())
	{
		create_info.enable_anisotropy = true;
		create_info.max_anisotropy = anisotropy.value();
	}

	sampler = SDL_CreateGPUSampler(device.get(), &create_info);

	if (!sampler)
	{
		throw std::runtime_error{ std::format("Could not create GPU sampler: {}", SDL_GetError()) };
	}
}

GpuSampler::~GpuSampler()
{
	SDL_ReleaseGPUSampler(device.get(), sampler);
}
