#pragma once

#include <vector>
#include <cstdint>
#include <optional>

#include "ECS/Entity.hpp"

constexpr uint32_t MAXENTITIES = 10000;

class EntityManager
{
public:
	EntityManager();

	std::optional<Entity> createEntity();
	void destroyEntity(Entity entity);

	const std::vector<Entity>& getEntities() const;
private:
	std::vector<uint32_t> ids;
	std::vector<Entity> entities;
};