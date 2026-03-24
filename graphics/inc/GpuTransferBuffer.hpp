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

		Vertex* map();
		void unmap();
		
		/// Function will take a dynamic array pointer and copy its contents to the transfer buffer.
		/// It will automatically map and unmap itself.
		/// You should not try to call it yourself unless you intend to copy it manually.
		/// @param arr Array pointer.
		/// @param size Array size;
		void putAutomatically(Vertex* arr, size_t size);

		template<typename Self>
		auto&& get(this Self&& self);
	private:
		std::shared_ptr<SDL_GPUDevice> device;
		SDL_GPUTransferBuffer* transfer_buffer;
	};

	template <typename Self>
	auto&& GpuTransferBuffer::get(this Self&& self)
	{
		return self.transfer_buffer;
	}

	inline GpuTransferBuffer::GpuTransferBuffer(std::shared_ptr<SDL_GPUDevice> device, Uint32 size, SDL_GPUTransferBufferUsage flags)
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

	inline GpuTransferBuffer::~GpuTransferBuffer() noexcept
	{
		if (transfer_buffer)
		{
			SDL_ReleaseGPUTransferBuffer(device.get(), transfer_buffer);
		}
	}

	inline Vertex* GpuTransferBuffer::map() 
	{
		Vertex* data = static_cast<Vertex*>(SDL_MapGPUTransferBuffer(device.get(), transfer_buffer, false));

		if (!data)
		{
			throw std::runtime_error{ std::format("Could not map GPU transfer buffer: {}", SDL_GetError()) };
		}

		return data;
	}

	inline void GpuTransferBuffer::unmap()
	{
		SDL_UnmapGPUTransferBuffer(device.get(), transfer_buffer);
	}

	inline void GpuTransferBuffer::putAutomatically(Vertex* arr, size_t size)
	{
		auto* data = map();

		/*data[0] = vertices[0];
		data[1] = vertices[1];
		data[2] = vertices[2];*/

		SDL_memcpy(data, arr, size);

		unmap();
	}
}
