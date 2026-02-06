#pragma once

#include "Craft.hpp"
#include "UI/UIElement.hpp"
#include "glm/vec2.hpp"
#include "Font.hpp"
#include "Inventory.hpp"
#include "SpriteSheet.hpp"
#include "CraftingManager.hpp"
#include "ECS/EntityManager.hpp"
#include "ECS/ComponentManager.hpp"

class CraftView : public UIElement
{
public:
	CraftView(Entity target_entity, EntityManager& entity_manager, ComponentManager& component_manager, int rows, int columns, float slot_size, const glm::vec2& position);

	void update() override;
	void render(graphics::Renderer& screen) override;
private:
	int rows;
	int columns;

	float slot_size;

	EntityManager& entity_manager;
	ComponentManager& component_manager;

	Entity target_entity;
};	