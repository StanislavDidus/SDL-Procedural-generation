#include "ObjectManager.hpp"

ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
}

const ObjectProperties& ObjectManager::getProperties(int id) const
{
	return objects[id];
}

void ObjectManager::addObject(const ObjectProperties& properties)
{
	objects.push_back(properties);
}
