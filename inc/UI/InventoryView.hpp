#pragma once

#include "UIElement.hpp"
#include "Inventory.hpp"
#include "SpriteSheet.hpp"
#include "InputManager.hpp"
#include "Font.hpp"
#include <vector>
#include "Text.hpp"

class InventoryView : public UIElement
{
public:
	InventoryView(const Font& font, const SpriteSheet& item_sprites, Inventory* inventory, int rows, int columns, float slot_size, const glm::vec2& position);

	bool isMouseCoveringInventory() const;

	void update() override;
	void render(Renderer& screen) override;
private:
	void isCoveringInventory();
	void isUsing();
	void isDragging();
	void isSplitting();
	void isMovingItems();

	void drawStackNumbers(Renderer& screen);

	void updateText(const Renderer& screen, std::optional<Text>& text_, const std::string& new_text);

	void drawItem(Renderer& screen, const Item& item, const glm::vec2& position, int index);

	Inventory* inventory;

	int rows;
	int columns;

	std::optional<int> covered_slot;

	std::optional<int> dragged_slot;
	glm::vec2 dragged_position{};


	float slot_size;

	//Items
	const Font& font;
	std::vector<std::optional<Text>> slot_text;
	const SpriteSheet& item_sprites;
};