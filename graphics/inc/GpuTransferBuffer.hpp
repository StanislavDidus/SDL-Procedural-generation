#pragma once
#include <format>
#include <memory>
#include <stdexcept>

#include "SDL3/SDL_gpu.h"

namespace graphics
{
	class GpuTransferBuffer
	{
	public:
		GpuTransferBuffer() = default;
		GpuTransferBuffer(std::shared_ptr<SDL_GPUDevice> device, Uint32 size, SDL_GPUTransferBufferUsage flags);
		~GpuTransferBuffer() noexcept;

		template<typename MapType>
		MapType* map(bool cycle);
		void unmap();
		
		/// Function will take a dynamic array pointer and copy its contents to the transfer tile_buffer.
		/// It will automatically map and unmap itself.
		/// You should not try to call it yourself unless you intend to copy it manually.
		/// @param arr Array pointer.
		/// @param size Array size;
		/// @param offset = 0
		/// @param cycle
		template<typename MapType>
		void putAutomatically(MapType* arr, size_t size, size_t offset, bool cycle = false);

		template<typename Self>
		auto&& get(this Self&& self);
	private:
		std::shared_ptr<SDL_GPUDevice> device;
		SDL_GPUTransferBuffer* transfer_buffer;
	};

	inline GpuTransferBuffer::GpuTransferBuffer(std::shared_ptr<SDL_GPUDevice> device, Uint32 size, SDL_GPUTransferBufferUsage flags)
		: device{device}
	{
		// Create transfer tile_buffer
		SDL_GPUTransferBufferCreateInfo transfer_info;
		transfer_info.size = size;
		transfer_info.usage = flags;
		transfer_buffer = SDL_CreateGPUTransferBuffer(device.get(), &transfer_info);

		if (!transfer_buffer)
		{
			throw std::runtime_error{ std::format("Could not create GPU transfer tile_buffer: {}", SDL_GetError()) };
		}
	}

	inline GpuTransferBuffer::~GpuTransferBuffer() noexcept
	{
		if (transfer_buffer)
		{
			SDL_ReleaseGPUTransferBuffer(device.get(), transfer_buffer);
		}
	}

	template<typename MapType>
	inline MapType* GpuTransferBuffer::map(bool cycle) 
	{
		MapType* data = static_cast<MapType*>(SDL_MapGPUTransferBuffer(device.get(), transfer_buffer, cycle));

		if (!data)
		{
			throw std::runtime_error{ std::format("Could not map GPU transfer tile_buffer: {}", SDL_GetError()) };
		}

		return data;
	}

	inline void GpuTransferBuffer::unmap()
	{
		SDL_UnmapGPUTransferBuffer(device.get(), transfer_buffer);
	}

	template<typename MapType>
	inline void GpuTransferBuffer::putAutomatically(MapType* arr, size_t size, size_t offset, bool cycle)
	{
		MapType* data = map<MapType>(cycle);

		data += offset;

		SDL_memcpy(data, arr, size);

		unmap();
	}

	template <typename Self>
	auto&& GpuTransferBuffer::get(this Self&& self)
	{
		return self.transfer_buffer;
	}
}
