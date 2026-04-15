#pragma once
#include <filesystem>
#include <memory>
#include <iostream>

#include "GpuSampler.hpp"
#include "Surface.hpp"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_log.h"

namespace graphics
{
	class GpuTexture
	{
	public:
		GpuTexture() = default;
		GpuTexture(std::shared_ptr<SDL_GPUDevice> device, const Surface& surface, std::shared_ptr<GpuSampler> sampler);
		~GpuTexture();

		void setName(const std::string& name);

		// Getters
		void* pixels();
		int w() const;
		int h() const;
		int pitch() const;
		std::shared_ptr<GpuSampler> getSampler() const;

		template<typename Self>
		auto&& get(this Self&& self);

	private:
		std::shared_ptr<SDL_GPUDevice> device = nullptr;
		std::shared_ptr<GpuSampler> sampler = nullptr;
		SDL_GPUTexture* texture = nullptr;
		
		void* m_pixels = nullptr;
		int m_width = 0;
		int m_height = 0;
		int m_pitch = 0;
	};

	template <typename Self>
	auto&& GpuTexture::get(this Self&& self)
	{
		return self.texture;
	}

	inline GpuTexture::GpuTexture(std::shared_ptr<SDL_GPUDevice> device, const Surface& surface, std::shared_ptr<GpuSampler> sampler)
		: device{device}
		, sampler{sampler}
	{
		SDL_Surface* image_data = surface.getSurface();

		SDL_GPUTextureCreateInfo texture_create_info = {};
		texture_create_info.type = SDL_GPU_TEXTURETYPE_2D;
		texture_create_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
		texture_create_info.width = image_data->w;
		texture_create_info.height = image_data->h;
		texture_create_info.layer_count_or_depth = 1;
		texture_create_info.num_levels = 1;
		texture_create_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
		
		texture = SDL_CreateGPUTexture(device.get(), &texture_create_info);

		if (!texture)
		{
			throw std::runtime_error{ std::format("Could not create GPU texture: {}", SDL_GetError()) };
		}

		m_pixels = image_data->pixels;
		m_width = image_data->w;
		m_height = image_data->h;
		m_pitch = image_data->pitch;

		//std::cout << std::format("Texture was successfully created.") << std::endl;
	}

	inline GpuTexture::~GpuTexture()
	{
		if (texture) SDL_ReleaseGPUTexture(device.get(), texture);
	}

	inline void GpuTexture::setName(const std::string& name)
	{
		if (texture)
			SDL_SetGPUTextureName(device.get(), texture, name.c_str());
	}

	inline void* GpuTexture::pixels()
	{
		return m_pixels;
	}

	inline int GpuTexture::w() const
	{
		return m_width;
	}

	inline int GpuTexture::h() const
	{
		return m_height;
	}

	inline int GpuTexture::pitch() const
	{
		return m_pitch;
	}

	inline std::shared_ptr<GpuSampler> GpuTexture::getSampler() const
	{
		return sampler;
	}
}
