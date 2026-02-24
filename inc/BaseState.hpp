#pragma once

#include "Game.hpp"
#include "Renderer.hpp"

struct BaseState
{
	BaseState() = default;
	virtual ~BaseState() = default;

	virtual void enter(Game& game);
	virtual void update(Game& game, float dt);
	virtual void render(Game& game, graphics::Renderer& screen);
	virtual void exit(Game& game);
};
