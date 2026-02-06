#include "Renderer.hpp"
#include "Sprite.hpp"
#include "Color.hpp"
#include "Window.hpp"
#include "Text.hpp"

#include <iostream>

namespace graphics
{
	Renderer::Renderer(Window& window) : window(window)
	{
		renderer = SDL_CreateRenderer(window.get(), nullptr);

		std::cout << "Renderer was created" << std::endl;

		const auto& window_size = this->window.getWindowSize();

		SDL_SetRenderVSync(renderer, 0);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderLogicalPresentation(renderer, window_size.x, window_size.y, SDL_LOGICAL_PRESENTATION_LETTERBOX);
	}

	Renderer::~Renderer()
	{
		SDL_DestroyRenderer(renderer);

		std::cout << "Renderer was deleted" << std::endl;
	}

	void Renderer::setView(const glm::vec2& view_position)
	{
		this->view_position = view_position - static_cast<glm::vec2>(getWindowSize()) * 0.5f;
	}

	void Renderer::setZoom(float zoom)
	{
		this->zoom = zoom;
	}


	const glm::vec2& Renderer::getView() const
	{
		return view_position;
	}

	float Renderer::getZoom() const
	{
		return zoom;
	}

	glm::ivec2 Renderer::getWindowSize() const
	{
		return window.getWindowSize();
	}
} //namespace graphics