#pragma once

#include "BaseState.hpp"

struct GameState : public BaseState
{
	GameState() = default;

	void enter(Game& game) override
	{
		
	}

	void update(Game& game, float dt) override
	{
		
	}
	 
	void render(Game& game, graphics::Renderer& screen) override
	{
		
	}

	void exit(Game& game) override
	{
		
	}
};