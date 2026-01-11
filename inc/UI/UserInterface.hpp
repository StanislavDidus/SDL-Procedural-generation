#pragma once

#include "Renderer.hpp"
#include "memory"

#include "UIElement.hpp"
#include "InventoryView.hpp"

class SpriteSheet;
class Inventory;
class Font;

class UserInterface
{
public:
	UserInterface() = default;

	void addFillBar(const glm::vec2& position, const glm::vec2& size, const float& value, float max_value, Color color);
	void addInventoryView(const Font& font, const SpriteSheet& item_sprites, Inventory* inventory, int rows, int columns, float slot_size, const glm::vec2& position);

	bool isMouseCoveringInventory() const;

	void update();
	void render(Renderer& screen);
private:
	std::vector<std::unique_ptr<UIElement>> elements;

	std::unique_ptr<InventoryView> inventory_view;
};