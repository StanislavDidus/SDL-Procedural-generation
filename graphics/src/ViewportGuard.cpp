#include "ViewportGuard.hpp"

ViewportGuard::ViewportGuard(Renderer& renderer, const SDL_Rect& viewport_rect) : renderer(renderer)
{
	SDL_SetRenderViewport(renderer.getRenderer(), &viewport_rect);
}

ViewportGuard::~ViewportGuard()
{
	SDL_SetRenderViewport(renderer.getRenderer(), nullptr);
}
