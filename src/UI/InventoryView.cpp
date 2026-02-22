#include "UI/InventoryView.hpp"

#include <iostream>

#include "Color.hpp"
#include "ResourceManager.hpp"
#include "Surface.hpp"
#include "RenderFunctions.hpp"
#include "ECS/ComponentManager.hpp"
#include "ItemManager.hpp"

constexpr float LABEL_WIDTH = 250.f;
constexpr float LABEL_HEIGHT = 25.f;

constexpr float RESOURCE_ICON_WIDTH = 50.f;
constexpr float RESOURCE_ICON_HEIGHT = 50.f;

using namespace graphics;
using namespace Components::InventoryItems;

InventoryView::InventoryView(entt::registry& registry, const Font* font, const SpriteSheet& item_sprites, int rows, int columns, const glm::vec2& position, const UISettings& ui_settings, Entity target_entity)
	: UIElement(position, { ui_settings.inventory_slot_width * columns
		, ui_settings.inventory_slot_height * rows })
		, font(font)
		, item_sprites(item_sprites)
		, rows(rows)	
		, columns(columns)
		, ui_settings(ui_settings)
		, target_entity(target_entity)
		, registry{registry}
{
}

void InventoryView::update()
{
	//TODO: remove this hack
	size = {ui_settings.inventory_slot_width * columns, ui_settings.inventory_slot_height * rows };

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
		int x = static_cast<int>(std::floor(mouse_position.x / ui_settings.inventory_slot_width));
		int y = static_cast<int>(std::floor(mouse_position.y / ui_settings.inventory_slot_height));

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
		inventory->useItem(*covered_slot, target_entity, registry);
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
		dragged_position = glm::vec2{ std::fmodf(mouse_position.x, ui_settings.inventory_slot_width), std::fmodf(mouse_position.y, ui_settings.inventory_slot_height) };
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

		if (!dragged_item || !covered_item)
		{
			inventory->moveItem(*dragged_slot, *covered_slot);
			dragged_slot = std::nullopt;
		}
		else
		{
			const auto& dragged_item_info = registry.get<Item>(*dragged_item);
			const auto& covered_item_info = registry.get<Item>(*covered_item);

			if (dragged_item_info == covered_item_info && *dragged_slot != *covered_slot)
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
	}
	else if (dragged_slot && !covered_slot && mouse.left == MouseButtonState::RELEASED && inventory)
	{
		//Drop item
		const auto& item = *getItem(*dragged_slot);
		const auto& item_info = registry.get<Item>(item);
		if (!item_info.equipped)
		{
			// Drop item if it is moved outside the inventory area
			auto drop_item = registry.create();
			registry.emplace<Components::DropItem>(drop_item, target_entity, item);
			inventory->removeItemAtSlot(*dragged_slot);
			dragged_slot = std::nullopt;
		}
		else
		{
			dragged_slot = std::nullopt;
		}
	}
}

bool InventoryView::isMouseCoveringInventory() const
{
	return covered_slot.has_value();
}

std::optional<int> InventoryView::getCoveredSlotIndex() const
{
	if (!isMouseCoveringInventory()) return std::nullopt;

	const auto& mouse_position = InputManager::getMouseState().position;

	glm::ivec2 mouse_grid_position = static_cast<glm::ivec2>(mouse_position / glm::vec2{ui_settings.inventory_slot_width, ui_settings.inventory_slot_height});

	int index = mouse_grid_position.x + mouse_grid_position.y * columns;

	return index;
}

glm::vec2 InventoryView::getSlotGlobalCoords(int slot) const
{
	int x = slot % columns;
	int y = slot / columns;

	return glm::vec2{ position.x + x * ui_settings.inventory_slot_width, position.y + y * ui_settings.inventory_slot_height};
}

glm::vec2 InventoryView::getSlotSize() const
{
	return glm::vec2{ ui_settings.inventory_slot_width, ui_settings.inventory_slot_height };
}

std::optional<Entity> InventoryView::getItem(int slot) const
{
	if (const auto& val = inventory->getItems()[slot]) return *val;
	return std::nullopt;
}

void InventoryView::setTargetEntity(Entity entity)
{
	target_entity = entity;

	if (registry.all_of<Components::HasInventory>(target_entity))
	{
		inventory = registry.get<Components::HasInventory>(target_entity).inventory.get();
		slot_text.resize(inventory->getItems().size());
	}
}

void InventoryView::render(graphics::Renderer& screen)
{
	//Render inventory ui slots
	for (int i = 0; i < rows * columns; i++)
	{
		int x = i % columns;
		int y = i / columns;

		glm::vec2 pos = { position.x + x * ui_settings.inventory_slot_width, position.y + y * ui_settings.inventory_slot_height };

		//Draw slot outline
		drawRectangle(screen, pos.x, pos.y, ui_settings.inventory_slot_width, ui_settings.inventory_slot_height, RenderType::NONE, Color::YELLOW, IGNORE_VIEW_ZOOM);


		const auto& item = inventory->getItems()[i];

		if (!item) continue;
		
		const auto& item_manager = ItemManager::get();

		const auto& item_properties = item_manager.getProperties(registry, *item);

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
	
	//Render item that is being dragged by the user
	if (covered_slot)
	{
		int index = *covered_slot;
		int x = index % columns;
		int y = index / columns;

		drawRectangle
		(
			screen,
			x * ui_settings.inventory_slot_width,
			y * ui_settings.inventory_slot_height,
			ui_settings.inventory_slot_width,
			ui_settings.inventory_slot_height,
			RenderType::FILL,
			Color::TRANSPARENT_BLUE, IGNORE_VIEW_ZOOM
		);
	}


}


void InventoryView::drawItem(graphics::Renderer& screen, Entity item, const glm::vec2& position, int index)
{
	if (!inventory) return;

	const auto& item_manager = ItemManager::get();
	const auto& item_properties = item_manager.getProperties(registry, item);
	const auto& item_info = registry.get<Item>(item);

	const auto& item_sprite = item_sprites[item_properties.sprite_index];

	drawScaledSprite(screen, item_sprite, position.x, position.y, ui_settings.inventory_slot_width, ui_settings.inventory_slot_height, IGNORE_VIEW_ZOOM);

	if (item_info.equipped)
	{
		//screen.drawRectangle(position.x, position.y, ui_settings.inventory_slot_width, ui_settings.inventory_slot_height, RenderType::FILL, Color::TRANSPARENT_GREEN, IGNORE_VIEW_ZOOM);
		drawScaledSprite(screen, (*ResourceManager::get().getSpriteSheet("icons"))[0], position.x, position.y, ui_settings.inventory_slot_width, ui_settings.inventory_slot_height, IGNORE_VIEW_ZOOM);
	}

	auto& text = slot_text[index];

	if (text)
	{
		std::string new_text = std::to_string(item_info.stack_number);
		if (text->getText() != new_text)
		{
			text->setText(new_text);
			text->updateText(screen);
		}
	}
	else
	{
		text = std::make_unique<Text>( font, screen, std::to_string(item_info.stack_number) );
	}

	int x = index % columns;
	int y = index / columns;

	//Don't draw stack number if it is 1
	if (item_info.stack_number > 1)
	{
		printText
		(
			screen,
			*text,
			position.x + ui_settings.inventory_slot_width * 0.5f,
			position.y + ui_settings.inventory_slot_height * 0.5f,
			ui_settings.inventory_slot_width * 0.5f,
			ui_settings.inventory_slot_height * 0.5f,
			IGNORE_VIEW_ZOOM
		);
	}
}
