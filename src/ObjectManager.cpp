#include "ObjectManager.hpp"

const ObjectProperties& ObjectManager::getProperties(int ID) const
{
	return object_properties.at(ID);
}

int ObjectManager::registerObjectProperties(const ObjectProperties& properties)
{
	object_properties.push_back(properties);
	return static_cast<int>(object_properties.size() - 1ULL);
}

const std::vector<ObjectSpawnInfo>& ObjectManager::getAllObjectSpawnInfos() const
{
	return object_spawn_infos;
}

void ObjectManager::addObjectSpawnInfo(const ObjectSpawnInfo& spawn_info)
{
	object_spawn_infos.push_back(spawn_info);
}
