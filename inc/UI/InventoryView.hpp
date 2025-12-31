#pragma once

#include "UIElement.hpp"
#include "Inventory.hpp"
#include "SpriteSheet.hpp"

class InventoryView : public UIElement
{
public:
	InventoryView(const SpriteSheet& item_sprites, Inventory* inventory, int rows, int columns, float slot_size, const glm::vec2& position)
		: UIElement(position, {}), item_sprites(item_sprites), inventory(inventory), rows(rows), columns(columns), slot_size(slot_size)
	{

	}

	void update() override
	{

	}

	void render(Renderer& screen) override
	{
		for (int i = 0; i < rows * columns; i++)
		{
			int x = i % columns;
			int y = i / columns;

			glm::vec2 pos = {position.x + x * slot_size, position.y + y * slot_size};

			screen.drawRectangle(pos.x, pos.y, slot_size, slot_size, RenderType::NONE, Color::YELLOW, IGNORE_VIEW_ZOOM);

			const auto& item = inventory->getItems()[i];

			if (item)
			{
				const auto& item_sprite = item_sprites[item.value().properties.sprite_index];

				screen.drawScaledSprite(item_sprite, pos.x, pos.y, slot_size, slot_size, IGNORE_VIEW_ZOOM);
			}
		}
	}
private:
	Inventory* inventory;

	int rows;
	int columns;

	float slot_size;

	SpriteSheet item_sprites;
};