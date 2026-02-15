#pragma once
#include "ComponentManager.hpp"
#include "Entity.hpp"

class RenderWeaponCircle
{
public:
	RenderWeaponCircle() = default;

	void update(Entity target_entity)
	{
		const auto& component_manager = ComponentManager::get();
		if (!component_manager.transform.contains(target_entity) || !component_manager.equipment.contains(target_entity)) return;

		const auto& player_transform_component = component_manager.transform.at(target_entity);
		const auto& player_equipment_component = component_manager.equipment.at(target_entity);

	
	}
};
