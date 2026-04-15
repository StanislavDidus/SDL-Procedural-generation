#pragma once

#include <format>
#include <memory>
#include <stdexcept>

#include "SDL3/SDL_gpu.h"

namespace graphics
{
	class CommandBuffer
	{
	public:
		CommandBuffer() = default;
		CommandBuffer(std::shared_ptr<SDL_GPUDevice> device);
		~CommandBuffer();

		void submit();

		template<typename Self>
		auto&& get(this Self&& self);
	private:
		SDL_GPUCommandBuffer* command_buffer;
	};

	inline CommandBuffer::CommandBuffer(std::shared_ptr<SDL_GPUDevice> device)
	{
		command_buffer = SDL_AcquireGPUCommandBuffer(device.get());

		if (!command_buffer)
		{
			throw std::runtime_error{ std::format("Could not acquire command buffer: {}", SDL_GetError()) };
		}
	}

	inline CommandBuffer::~CommandBuffer()
	{
		submit();
	}

	inline void CommandBuffer::submit()
	{
		if (command_buffer)
		{
			if (!SDL_SubmitGPUCommandBuffer(command_buffer))
			{
				throw std::runtime_error{ std::format("Could not submit a command buffer: {}", SDL_GetError()) };
			}
			command_buffer = nullptr;
		}
	}

	template <typename Self>
	auto&& CommandBuffer::get(this Self&& self)
	{
		return self.command_buffer;
	}
}
