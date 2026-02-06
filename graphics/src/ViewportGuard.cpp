#include "ViewportGuard.hpp"

namespace graphics
{

	ViewportGuard::ViewportGuard(graphics::Renderer& renderer, const SDL_Rect& viewport_rect) : renderer(renderer)
	{
		SDL_SetRenderViewport(renderer.get(), &viewport_rect);
	}

	ViewportGuard::~ViewportGuard()
	{
		SDL_SetRenderViewport(renderer.get(), nullptr);
	}

} // namespace graphics