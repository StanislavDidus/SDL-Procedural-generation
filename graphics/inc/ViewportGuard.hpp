#pragma once

#include "Renderer.hpp"

namespace graphics
{

	/// <summary>
	/// RAII class that sets the viewport of the given renderer and automatically resets it at the end of the lifetime.
	/// </summary>
	class ViewportGuard
	{
	public:
		ViewportGuard(graphics::Renderer& renderer, const SDL_Rect& viewport_rect);
		~ViewportGuard();

		ViewportGuard(const ViewportGuard& other) = delete;
		ViewportGuard(ViewportGuard&& other) noexcept = delete;
		ViewportGuard& operator=(const ViewportGuard& other) = delete;
		ViewportGuard& operator=(ViewportGuard&& other) noexcept = delete;

	private:
		graphics::Renderer& renderer;
	};

	/*Usage example
	Some scope
	{
		//Create an object of a class ViewportGuard
		auto viewport = ViewportGuard(screen{ 100,100,100,100 });

		//Draw
		//...

		//Viewport will be automatically reset when it goes out of scope
	}*/

} // namespace graphics