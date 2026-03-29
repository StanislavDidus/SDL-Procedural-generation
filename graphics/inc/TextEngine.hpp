#pragma once
#include "GpuRenderer.hpp"
#include "SDL3_ttf/SDL_ttf.h"

namespace graphics
{
	class TextEngine
	{
	public:
		TextEngine() = default;
		TextEngine(GpuRenderer& renderer);

		template<typename Self>
		auto&& get(this Self&& self);
	private:
		TTF_TextEngine* text_engine;
	};

	template <typename Self>
	auto&& TextEngine::get(this Self&& self)
	{
		return self.text_engine;
	}

	inline TextEngine::TextEngine(GpuRenderer& renderer)
	{
		text_engine = TTF_CreateGPUTextEngine(renderer.getDevice());

		if (!text_engine)
		{
			throw std::runtime_error{ std::format("Could not create GPU text engine: {}", SDL_GetError()) };
		}
	}
}
