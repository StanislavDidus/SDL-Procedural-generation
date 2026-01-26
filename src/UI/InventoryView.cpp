#include "UI/InventoryView.hpp"

#include <iostream>

#include "Color.hpp"
#include "Surface.hpp"

InventoryView::InventoryView(const Font* font, const SpriteSheet& item_sprites, Inventory* inventory, int rows, int columns, float slot_size, const glm::vec2& position)
	: UIElement(position, { slot_size * columns , slot_size * rows }), font(font), item_sprites(item_sprites), inventory(inventory), rows(rows), columns(columns), slot_size(slot_size)
{
	if (inventory) {
		slot_text.resize(inventory->getItems().size());
	}
}

void InventoryView::update()
{
	isCoveringInventory();
	isUsing();
	isDragging();
	isSplitting();
	isMovingItems();
}

void InventoryView::isCoveringInventory()
{
	const auto& mouse = InputManager::getMouseState();
	const auto& mouse_position = mouse.position;

	bool is_covered = false;
	if (mouse_position.x >= position.x &&
		mouse_position.x < position.x + size.x &&
		mouse_position.y >= position.y &&
		mouse_position.y < position.y + size.y)
	{
		int x = static_cast<int>(std::floor(mouse_position.x / slot_size));
		int y = static_cast<int>(std::floor(mouse_position.y / slot_size));

		covered_slot = x + y * columns;
		is_covered = true;
	}

	if (!is_covered) covered_slot = std::nullopt;
}

void InventoryView::isUsing()
{
	const auto& mouse = InputManager::getMouseState();
	if (covered_slot && !dragged_slot && mouse.right == MouseButtonState::RELEASED && inventory)
	{
		inventory->useItem(*covered_slot);
	}
}

void InventoryView::isDragging()
{
	const auto& mouse = InputManager::getMouseState();
	const auto& mouse_position = mouse.position;

	if (covered_slot && mouse.left == MouseButtonState::DOWN)
	{
		if (!inventory) return;

		const auto& item = inventory->getItems()[*covered_slot];

		//Drag if the slot is occupied with item
		if (!item) return;
		
		dragged_slot = covered_slot;
		//Offset for mouse dragging
		dragged_position = glm::vec2{ std::fmodf(mouse_position.x, slot_size), std::fmodf(mouse_position.y, slot_size) };
	}
}

void InventoryView::isSplitting()
{
	const auto& mouse = InputManager::getMouseState();
	const auto& mouse_position = mouse.position;

	if (covered_slot && dragged_slot && mouse.right == MouseButtonState::DOWN && inventory)
	{
		inventory->splitItemTo(*dragged_slot, *covered_slot);
	}
}

void InventoryView::isMovingItems()
{
	const auto& mouse = InputManager::getMouseState();
	const auto& mouse_position = mouse.position;

	if (dragged_slot && covered_slot && mouse.left == MouseButtonState::RELEASED && inventory)
	{
		auto& dragged_item = inventory->getItems()[*dragged_slot];
		auto& covered_item = inventory->getItems()[*covered_slot];

		if (dragged_item == covered_item && *dragged_slot != *covered_slot)
		{
			inventory->stackItems(*dragged_slot, *covered_slot);
			dragged_slot = std::nullopt;
		}
		else
		{
			inventory->moveItem(*dragged_slot, *covered_slot);
			dragged_slot = std::nullopt;
		}
	}
	else if (dragged_slot && mouse.left == MouseButtonState::RELEASED)
	{
		dragged_slot = std::nullopt;
	}
}

bool InventoryView::isMouseCoveringInventory() const
{
	return covered_slot.has_value();
}

void InventoryView::render(Renderer& screen)
{
	for (int i = 0; i < rows * columns; i++)
	{
		int x = i % columns;
		int y = i / columns;

		glm::vec2 pos = { position.x + x * slot_size, position.y + y * slot_size };

		//Draw slot outline
		screen.drawRectangle(pos.x, pos.y, slot_size, slot_size, RenderType::NONE, Color::YELLOW, IGNORE_VIEW_ZOOM);


		const auto& item = inventory->getItems()[i];

		if (!item) continue;
		
		const auto& item_manager = ItemManager::get();

		const auto& item_properties = item_manager.getProperties(item->id);

		//Draw static items in an inventory slot
		if (item && i != dragged_slot)
		{
			const auto& item_sprite = item_sprites[item_properties.sprite_index];

			drawItem(screen, *item, { pos.x, pos.y }, i);
		}

		//Draw item while dragging
		if (item && i == dragged_slot)
		{
			const auto& item_sprite = item_sprites[item_properties.sprite_index];
			const auto& mouse_position = InputManager::getMouseState().position;


			drawItem(screen, *item, { mouse_position.x - dragged_position.x, mouse_position.y - dragged_position.y }, i);
		}
	}

	if (covered_slot)
	{
		int index = *covered_slot;
		int x = index % columns;
		int y = index / columns;

		screen.drawRectangle(x * slot_size, y * slot_size, slot_size, slot_size, RenderType::FILL, Color::TRANSPARENT_BLUE, IGNORE_VIEW_ZOOM);
	}
}


void InventoryView::drawItem(Renderer& screen, const Item& item, const glm::vec2& position, int index)
{
	if (!inventory) return;

	const auto& item_manager = ItemManager::get();
	const auto& item_properties = item_manager.getProperties(item.id);

	const auto& item_sprite = item_sprites[item_properties.sprite_index];

	screen.drawScaledSprite(item_sprite, position.x, position.y, slot_size, slot_size, IGNORE_VIEW_ZOOM);

	auto& text = slot_text[index];

	if (text)
	{
		text->setText(std::to_string(item.stack_number));
		text->updateText(screen);
	}
	else
	{
		text = std::make_unique<Text>( font, screen, std::to_string(item.stack_number) );
	}

	int x = index % columns;
	int y = index / columns;

	//Don't draw stack number if it is 1
	if (item.stack_number > 1)
		screen.printText(*text, position.x + slot_size * 0.5f, position.y + slot_size * 0.5f, slot_size * 0.5f, slot_size * 0.5f, IGNORE_VIEW_ZOOM);
}
