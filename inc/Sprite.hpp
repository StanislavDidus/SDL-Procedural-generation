#pragma once

#include <SDL3/SDL.h>
#include "Surface.hpp"
#include "Renderer.hpp"
#include "glm/glm.hpp"

class Sprite
{
public:
	Sprite(Renderer& renderer, const Surface& surface, const glm::vec2& frame_rect = {0.f, 0.f}, SDL_ScaleMode scale_mode = SDL_SCALEMODE_LINEAR);
	virtual ~Sprite();

	//Setters
	void setFrame(int frame);

	//Getters
	const SDL_FRect& getSpriteRect() const;
	SDL_Texture* getTexture() const;
	const glm::vec2& getTextureSize() const;

private:
	void setSpriteRect();

	Surface surface;
	SDL_Texture* texture = nullptr;

	//Frames
	glm::vec2 frame_size {0.f, 0.f};
	SDL_FRect sprite_rect{ 0.f, 0.f, 0.f, 0.f };
	int columns = 0;
	int frames = 0;
	int current_frame = 0;

	glm::vec2 texture_size {0.f, 0.f};
};
