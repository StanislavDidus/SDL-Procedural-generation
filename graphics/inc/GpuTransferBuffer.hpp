#pragma once
#include <format>
#include <memory>
#include <stdexcept>

#include "SDL3/SDL_gpu.h"

namespace graphics
{
	template<typename MapType>
	class GpuTransferBuffer
	{
	public:
		GpuTransferBuffer() = default;
		GpuTransferBuffer(std::shared_ptr<SDL_GPUDevice> device, Uint32 size, SDL_GPUTransferBufferUsage flags);
		~GpuTransferBuffer() noexcept;

		MapType* map();
		void unmap();
		
		/// Function will take a dynamic array pointer and copy its contents to the transfer buffer.
		/// It will automatically map and unmap itself.
		/// You should not try to call it yourself unless you intend to copy it manually.
		/// @param arr Array pointer.
		/// @param size Array size;
		void putAutomatically(MapType* arr, size_t size);

		template<typename Self>
		auto&& get(this Self&& self);
	private:
		std::shared_ptr<SDL_GPUDevice> device;
		SDL_GPUTransferBuffer* transfer_buffer;
	};

	template<typename MapType>
	template <typename Self>
	auto&& GpuTransferBuffer<MapType>::get(this Self&& self)
	{
		return self.transfer_buffer;
	}

	template<typename MapType>
	inline GpuTransferBuffer<MapType>::GpuTransferBuffer(std::shared_ptr<SDL_GPUDevice> device, Uint32 size, SDL_GPUTransferBufferUsage flags)
		: device{device}
	{
		// Create transfer buffer
		SDL_GPUTransferBufferCreateInfo transfer_info;
		transfer_info.size = size;
		transfer_info.usage = flags;
		transfer_buffer = SDL_CreateGPUTransferBuffer(device.get(), &transfer_info);

		if (!transfer_buffer)
		{
			throw std::runtime_error{ std::format("Could not create GPU transfer buffer: {}", SDL_GetError()) };
		}
	}

	template<typename MapType>
	inline GpuTransferBuffer<MapType>::~GpuTransferBuffer() noexcept
	{
		if (transfer_buffer)
		{
			SDL_ReleaseGPUTransferBuffer(device.get(), transfer_buffer);
		}
	}

	template<typename MapType>
	inline MapType* GpuTransferBuffer<MapType>::map() 
	{
		MapType* data = static_cast<MapType*>(SDL_MapGPUTransferBuffer(device.get(), transfer_buffer, false));

		if (!data)
		{
			throw std::runtime_error{ std::format("Could not map GPU transfer buffer: {}", SDL_GetError()) };
		}

		return data;
	}

	template<typename MapType>
	inline void GpuTransferBuffer<MapType>::unmap()
	{
		SDL_UnmapGPUTransferBuffer(device.get(), transfer_buffer);
	}

	template<typename MapType>
	inline void GpuTransferBuffer<MapType>::putAutomatically(MapType* arr, size_t size)
	{
		auto* data = map();

		/*data[0] = vertices[0];
		data[1] = vertices[1];
		data[2] = vertices[2];*/

		SDL_memcpy(data, arr, size);

		unmap();
	}
}
