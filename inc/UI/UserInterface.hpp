#pragma once

#include "Renderer.hpp"
#include "memory"

#include "UIElement.hpp"

class SpriteSheet;
class Inventory;

class UserInterface
{
public:
	UserInterface() = default;

	void addFillBar(const glm::vec2& position, const glm::vec2& size, const float& value, float max_value, Color color);
	void addInventoryView(const SpriteSheet& item_sprites, Inventory* inventory, int rows, int columns, float slot_size, const glm::vec2& position);

	void update();
	void render(Renderer& screen);
private:
	std::vector<std::unique_ptr<UIElement>> elements;
};