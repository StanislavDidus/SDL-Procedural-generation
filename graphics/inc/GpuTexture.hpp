#pragma once
#include <filesystem>
#include <memory>

#include "SDL3/SDL_gpu.h"

namespace graphics
{
	class GpuTexture
	{
	public:
		GpuTexture() = default;
		GpuTexture(std::shared_ptr<SDL_GPUDevice> device, const std::filesystem::path& path);
		~GpuTexture();

		void setName(const std::string& name);

	private:
		std::shared_ptr<SDL_GPUDevice> device = nullptr;
		SDL_GPUTexture* texture = nullptr;
		SDL_Surface* image_data = nullptr;
	};

	inline GpuTexture::GpuTexture(std::shared_ptr<SDL_GPUDevice> device, const std::filesystem::path& path)
		: device{device}
	{
		image_data = SDL_LoadPNG(path.string().c_str());
		SDL_PixelFormat format = SDL_PIXELFORMAT_ARGB8888;

		if (!image_data)
		{
			throw std::runtime_error{ std::format("Could not load png image: {}", SDL_GetError()) };
		}

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

		std::cout << std::format("Texture path:{} was successfully loaded.", path.string());
	}

	inline GpuTexture::~GpuTexture()
	{
		if (image_data) SDL_DestroySurface(image_data);
		if (texture) SDL_ReleaseGPUTexture(device.get(), texture);
	}

	inline void GpuTexture::setName(const std::string& name)
	{
		if (texture)
			SDL_SetGPUTextureName(device.get(), texture, name.c_str());
	}
}
