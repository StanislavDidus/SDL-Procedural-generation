#pragma once

#include <vector>
#include <cstdint>
#include <optional>

#include "ECS/Entity.hpp"

constexpr uint32_t MAXENTITIES = 10000;

class EntityManager
{
public:

	static EntityManager& get()
	{
		static EntityManager s;
		return s;
	}

	std::optional<Entity> createEntity();
	void destroyEntity(Entity id);

	const std::vector<Entity>& getEntities() const;
private:
	EntityManager();

	std::vector<uint32_t> ids;
	std::vector<Entity> entities;
};