#pragma once

#include "glm/vec2.hpp"
#include "SDL3/SDL_render.h"

/*constexpr float BaseWidthScreen = 960.f;
constexpr float BaseHeightScreen = 540.f;

constexpr float TileWidth = 25.f;
constexpr float TileHeight = 25.f;

constexpr float PlayerWidth = 25.f;
constexpr float PlayerHeight = 50.f;*/

namespace graphics
{
	struct Color;

	class Text;
	class Sprite;
	class Window;

	constexpr bool IGNORE_VIEW_ZOOM = true;

	enum class RenderType
	{
		NONE,
		FILL,
	};

	class Renderer
	{
	public:
		explicit Renderer(Window& window);
		~Renderer();

		Renderer(const Renderer& other) = delete;
		Renderer(Renderer&& other) noexcept = delete;
		Renderer& operator=(const Renderer& other) = delete;
		Renderer& operator=(Renderer&& other) noexcept = delete;

		//Setters
		void setView(const glm::vec2& view_position);
		void setZoom(float zoom);

		//Getters
		const glm::vec2& getView() const;
		float getZoom() const;
		glm::ivec2 getWindowSize() const;

		template<typename Self>
		auto&& get(this Self&& self);
	private:
		SDL_Renderer* renderer = nullptr;
		Window& window;

		glm::vec2 view_position{ 0.f, 0.f };
		float zoom = 1.5f;
	};

	template <typename Self>
	auto&& Renderer::get(this Self&& self)
	{
		return self.renderer;
	}
} //namespace graphics