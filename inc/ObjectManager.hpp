#pragma once

#include <vector>

#include "Object.hpp"

class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

	const ObjectProperties& getProperties(int index) const;
	void addObject(const ObjectProperties& properties);
private:
	std::vector<ObjectProperties> objects;
};