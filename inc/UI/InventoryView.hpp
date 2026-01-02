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
	InventoryView(const Font& font, const SpriteSheet& item_sprites, Inventory* inventory, int rows, int columns, float slot_size, const glm::vec2& position)
		: UIElement(position, { slot_size * columns , slot_size * rows }), font(font), item_sprites(item_sprites), inventory(inventory), rows(rows), columns(columns), slot_size(slot_size)
	{
		if (inventory) {
			slot_text.resize(inventory->getItems().size());
		}
	}

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
	Font font;
	std::vector<std::optional<Text>> slot_text;
	SpriteSheet item_sprites;
};