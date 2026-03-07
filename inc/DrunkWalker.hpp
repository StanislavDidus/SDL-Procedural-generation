#pragma once
#include "glm/vec2.hpp"

constexpr glm::ivec2 directions[4] = 
{
	{0,-1}, // UP
	{1, 0}, // RIGHT
	{0,1}, // DOWN
	{-1, 0} // LEFT
};

class DrunkWalker
{
public:
	DrunkWalker(const glm::ivec2 position, int max_size);

	//Setters
	void setPosition(const glm::ivec2& position);

	//Getters
	glm::ivec2 getPosition() const;
	bool isFinished() const;

	void move(int direction);

private:
	glm::ivec2 position; ///< Position on a grid.
	int max_size;
	
	int current_size = 0;
	bool finished = false;
};

