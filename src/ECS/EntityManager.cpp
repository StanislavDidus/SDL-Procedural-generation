#include "ECS/EntityManager.hpp"

EntityManager::EntityManager()
{
	for (int i = 0; i < MAXENTITIES; i++)
	{
		ids.push_back(i);
	}
}

std::optional<Entity> EntityManager::createEntity()
{
	if (ids.empty())
		return std::nullopt;

	uint32_t id = ids.back();
	ids.pop_back();

	entities.emplace_back(id);

	return entities.back();
}

void EntityManager::destroyEntity(Entity id)
{
	ids.push_back(id);

	std::remove(entities.begin(), entities.end(), id);
}

const std::vector<Entity>& EntityManager::getEntities() const
{
	return entities;
}
