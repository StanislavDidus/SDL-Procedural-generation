#pragma once

#include "Components.hpp"
#include "ResourceManager.hpp"

class OpenChestSystem
{
public:
	OpenChestSystem(entt::registry& registry) : registry{registry} {}

	void update()
	{
		auto view = registry.view<Components::Chest, Components::Renderable, Components::UI::OpenChest>();
		for (auto [entity, chest_component, renderable_component, open_chest_component] : view.each())
		{
			renderable_component.sprite = ResourceManager::get().getSpriteSheet("objects")->getSprite("Opened_Chest");
			registry.emplace<Components::Closed_Chest>(entity);
			registry.erase<Components::UI::OpenChest>(entity);

			/*
			auto drop_item = registry.create();
			auto item_to_drop = ItemManager::get().createItem(registry, chest_component.base_item);
			registry.emplace<Components::DropItem>(drop_item, entity, item_to_drop);
			*/

			Entity item_to_drop = entt::null;
			if(open_chest_component.number == 0)
				item_to_drop = ItemManager::get().createItem(registry, chest_component.base_item);
			else if(open_chest_component.type == EssenceType::COMMON)
				item_to_drop = ItemManager::get().createItem(registry, chest_component.common_item);
			else if(open_chest_component.type == EssenceType::SNOW)
				item_to_drop = ItemManager::get().createItem(registry, chest_component.snow_item);
			else if(open_chest_component.type == EssenceType::SAND)
				item_to_drop = ItemManager::get().createItem(registry, chest_component.sand_item);
			
			if(item_to_drop != entt::null)
				registry.emplace<Components::DropItemChest>(entity, item_to_drop);
		}
	}

private:
	entt::registry& registry;
};
