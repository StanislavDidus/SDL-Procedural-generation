#pragma once
#include "ComponentManager.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"
#include "ResourceManager.hpp"

class DropItemSystem
{
public:
	DropItemSystem() = default;

	void update()
	{
		auto& component_manager = ComponentManager::get();
	
		for (const auto& entity : EntityManager::get().getEntities())
		{
			if (component_manager.transform.contains(entity) && component_manager.drop_item.contains(entity))
			{
				const auto& transform_component = component_manager.transform.at(entity);
				auto& drop_item_component = component_manager.drop_item.at(entity);
				const auto& item_properties = ItemManager::get().getProperties(drop_item_component.item.id);

				component_manager.drop_item.erase(entity);

				//Create dropped item entity
				auto drop_item = EntityManager::get().createEntity();
				if (!entity) continue;

				component_manager.transform[*drop_item] = Transform
				{
					transform_component.position,
					{35.0f, 35.0f}
				};

				component_manager.renderable[*drop_item] = Renderable
				{
					(*ResourceManager::get().getSpriteSheet("items"))[item_properties.sprite_index]
				};

				component_manager.physics[*drop_item] = Physics
				{
					
				};
			}
		}
	}
};
