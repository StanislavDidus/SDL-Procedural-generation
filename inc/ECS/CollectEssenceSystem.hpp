#pragma once

#include "Components.hpp"
#include "glm/gtc/random.hpp"

class CollectEssenceSystem
{
public:
	CollectEssenceSystem(entt::registry& registry) : registry{registry} {}

	void update(Entity target_entity)
	{
		if (registry.all_of<Components::EquipmentEssence>(target_entity))
		{
			auto& equipment = registry.get<Components::EquipmentEssence>(target_entity);
			
			std::vector<Entity> to_destroy;

			auto view = registry.view<Components::GiveEssence>();
			for (const auto& [entity, give_essence_component] : view.each())
			{
				float rng = glm::linearRand(0.0f, 1.0f);
				if (rng <= give_essence_component.chance)
				{
					switch (give_essence_component.type)
					{
						case EssenceType::COMMON:
							equipment.common_essence += give_essence_component.number;
							break;
						case EssenceType::SNOW:
							equipment.snow_essence += give_essence_component.number;
							break;
						case EssenceType::SAND:
							equipment.sand_essence += give_essence_component.number;
							break;
						case EssenceType::NONE:	
							break;
					}
				}

				to_destroy.push_back(entity);
			}

			for (const auto& entity : to_destroy)
			{
				registry.destroy(entity);
			}
		}
	}

private:
	entt::registry& registry;
};
