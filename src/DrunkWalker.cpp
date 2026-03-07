#include "DrunkWalker.hpp"

#include <cmath>
#include <random>

DrunkWalker::DrunkWalker(const glm::ivec2 position, int max_size)
	: position(position)
	, max_size(max_size)
{
}

void DrunkWalker::setPosition(const glm::ivec2& position)
{
	this->position = position;
}

glm::ivec2 DrunkWalker::getPosition() const
{
	return position;
}

bool DrunkWalker::isFinished() const
{
	return finished;
}

void DrunkWalker::move(int direction)
{
	glm::ivec2 random_direction = directions[direction];
	position += random_direction;
	++current_size;

	if (current_size >= max_size)
	{
		finished = true;
	}
}

