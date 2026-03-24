#pragma once
#include <memory>

#include "SDL3/SDL_gpu.h"

namespace graphics
{
	class GpuVertexBuffer
	{
	public:
		GpuVertexBuffer() = default;
		GpuVertexBuffer(std::shared_ptr<SDL_GPUDevice> device, Uint32 size, SDL_GPUBufferUsageFlags flags);
		~GpuVertexBuffer() noexcept;

		template<typename Self>
		auto&& get(this Self&& self);
	private:
		std::shared_ptr<SDL_GPUDevice> device;
		SDL_GPUBuffer* vertex_buffer;
	};

	template <typename Self>
	auto&& GpuVertexBuffer::get(this Self&& self)
	{
		return self.vertex_buffer;
	}

	inline GpuVertexBuffer::GpuVertexBuffer(std::shared_ptr<SDL_GPUDevice> device, Uint32 size, SDL_GPUBufferUsageFlags flags)
		: device{device}
	{
		SDL_GPUBufferCreateInfo buffer_info;
		buffer_info.size = size;
		buffer_info.usage = flags;
		vertex_buffer = SDL_CreateGPUBuffer(device.get(), &buffer_info);
		
		if (!vertex_buffer)
		{
			throw std::runtime_error{ std::format("Could not create GPU vertex buffer: {}", SDL_GetError()) };
		}

	}

	inline GpuVertexBuffer::~GpuVertexBuffer() noexcept
	{
		if (vertex_buffer)
		{
			SDL_ReleaseGPUBuffer(device.get(), vertex_buffer);
		}
	}
}
