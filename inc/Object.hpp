#pragma once

#include "glm/glm.hpp"

class Object
{
public:
	Object() = default;
	virtual ~Object() = default;
private:
	glm::vec2 position;
	glm::vec2 size;
};