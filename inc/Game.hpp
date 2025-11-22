#pragma once

#include <SDL3/SDL.h>

#include "Renderer.hpp"
#include "Sprite.hpp"

class Game
{
public:
	Game(Renderer& screen);
	virtual ~Game();

	void update(float dt);
private:
	Renderer& screen;
	Sprite background;
	Sprite player;
};