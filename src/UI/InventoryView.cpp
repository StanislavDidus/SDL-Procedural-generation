#include "UI/InventoryView.hpp"
#include <sstream>

void InventoryView::update()
{
	isCoveringInventory();
	checkUse();
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

void InventoryView::checkUse()
{
	const auto& mouse = InputManager::getMouseState();
	if (covered_slot && !dragged_slot && mouse.right == MouseButtonState::RELEASED)
	{
		if (inventory)
		{
			inventory->useItem(*covered_slot);
		}
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

	if (covered_slot && dragged_slot && mouse.right == MouseButtonState::DOWN)
	{
		if (inventory)
		{
			inventory->splitItemTo(*dragged_slot, *covered_slot);
		}
	}
}

void InventoryView::isMovingItems()
{
	const auto& mouse = InputManager::getMouseState();
	const auto& mouse_position = mouse.position;

	if (dragged_slot && covered_slot && mouse.left == MouseButtonState::RELEASED)
	{
		if (inventory)
		{
			auto& dragged_item = inventory->getItems()[*dragged_slot];
			auto& covered_item = inventory->getItems()[*covered_slot];

			if (dragged_item == covered_item && *dragged_slot != *covered_slot)
			{
				inventory->stackItems(*dragged_slot, *covered_slot);
			}
			else
			{
				inventory->moveItem(*dragged_slot, *covered_slot);
				dragged_slot = std::nullopt;
			}
		}
	}
	else if (dragged_slot && mouse.left == MouseButtonState::RELEASED)
	{
		dragged_slot = std::nullopt;
	}
}

void InventoryView::render(Renderer& screen)
{
	for (int i = 0; i < rows * columns; i++)
	{
		int x = i % columns;
		int y = i / columns;

		glm::vec2 pos = { position.x + x * slot_size, position.y + y * slot_size };

		screen.drawRectangle(pos.x, pos.y, slot_size, slot_size, RenderType::NONE, Color::YELLOW, IGNORE_VIEW_ZOOM);

		/*if(std::find(inventory->free_slots.begin(), inventory->free_slots.end(), x + y * columns) !=  inventory->free_slots.end())
			screen.drawRectangle(pos.x, pos.y, slot_size, slot_size, RenderType::FILL, Color::GREEN, IGNORE_VIEW_ZOOM);
		else
			screen.drawRectangle(pos.x, pos.y, slot_size, slot_size, RenderType::FILL, Color::RED, IGNORE_VIEW_ZOOM);*/

		const auto& item = inventory->getItems()[i];

		int index = x + y * columns;

		//Draw static items in an inventory slot
		if (item && index != dragged_slot)
		{
			const auto& item_sprite = item_sprites[item.value().properties.sprite_index];

			drawItem(screen, *item, { pos.x, pos.y }, index);
		}

		//Draw item while dragging
		if (item && index == dragged_slot)
		{
			const auto& item_sprite = item_sprites[item.value().properties.sprite_index];
			const auto& mouse_position = InputManager::getMouseState().position;


			drawItem(screen, *item, { mouse_position.x - dragged_position.x, mouse_position.y - dragged_position.y }, index);
		}
	}

	if (covered_slot)
	{
		int index = *covered_slot;
		int x = index % columns;
		int y = index / columns;

		screen.drawRectangle(x * slot_size, y * slot_size, slot_size, slot_size, RenderType::FILL, Color::TRANSPARENT_BLUE, IGNORE_VIEW_ZOOM);
	}

	//drawStackNumbers(screen);
}

void InventoryView::drawStackNumbers(Renderer& screen)
{
	if (inventory)
	{
		int i = 0;
		for (const auto& item : inventory->getItems())
		{
			if (item)
			{
				const auto& item_ = *item;
				int stack_number = item_.properties.stack_number;
				std::stringstream ss;
				ss << stack_number;
				
				auto& text = slot_text[i];

				updateText(screen, text, ss.str());

				int x = i % columns;
				int y = i / columns;

				screen.printText(text->texture, x * slot_size + slot_size * 0.5f, y * slot_size + slot_size * 0.5f, slot_size * 0.5f, slot_size * 0.5f, IGNORE_VIEW_ZOOM);
			}

			++i;
		}
	}
}

void InventoryView::updateText(const Renderer& screen, std::optional<Text>& text_, const std::string& new_text)
{
	if (text_)
	{
		auto& text = *text_;
		if (text.text == new_text) return;

		if (text.texture) SDL_DestroyTexture(text.texture);
	}

	Surface surface{ font.getFont(), new_text, {0,0,0,255} };
	SDL_Texture* texture = SDL_CreateTextureFromSurface(screen.getRenderer(), surface.getSurface());
	if (!texture) std::cerr << "ERROR: Texture was not load succesufully: " << SDL_GetError() << std::endl;
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR);
	text_ = Text{ new_text, texture };
}

void InventoryView::drawItem(Renderer& screen, const Item& item, const glm::vec2& position, int index)
{
	const auto& item_sprite = item_sprites[item.properties.sprite_index];

	screen.drawScaledSprite(item_sprite, position.x, position.y, slot_size, slot_size, IGNORE_VIEW_ZOOM);

	int stack_number = item.properties.stack_number;
	std::stringstream ss;
	ss << stack_number;

	auto& text = slot_text[index];

	updateText(screen, text, ss.str());

	int x = index % columns;
	int y = index / columns;

	screen.printText(text->texture, position.x + slot_size * 0.5f, position.y + slot_size * 0.5f, slot_size * 0.5f, slot_size * 0.5f, IGNORE_VIEW_ZOOM);
}
