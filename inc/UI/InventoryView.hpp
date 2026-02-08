#pragma once

#include "UIElement.hpp"
#include "Inventory.hpp"
#include "SpriteSheet.hpp"
#include "InputManager.hpp"
#include "Font.hpp"
#include <vector>

#include "Button.hpp"
#include "Button.hpp"
#include "Text.hpp"
#include "UISettings.hpp"
#include "ECS/Entity.hpp"

namespace tinyxml2
{
	struct Entity;
}

/// <summary>
/// Class that draws the inventory contents and lets the user manipulate it by using or dragging items.
/// </summary>
class InventoryView : public UIElement
{
public:
	InventoryView(const graphics::Font* font, const graphics::SpriteSheet& item_sprites, int rows, int columns, const glm::vec2& position, const UISettings& ui_settings);
	~InventoryView() override = default;

	/// <summary>
	/// Check if a mouse is on the inventory.
	/// </summary>
	/// <returns>Returns true if the mouse intersects with the inventory bounds.</returns>
	inline bool isMouseCoveringInventory() const;

	/// <summary>
	/// Returns the index of the inventory slot that is being covered by a mouse.
	/// </summary>
	/// <returns>Returns <b>std::nulloptr</b> if mouse is oustside of inventory bounds.</returns>
	std::optional<int> getCoveredSlotIndex() const;

	/// <summary>
	/// Converts slot index to its centre global position in the world according to inventory position and slot width and height.
	/// </summary>
	/// <param name="slot">Slot index.</param>
	/// <returns>Centre global position in the world space.</returns>
	glm::vec2 getSlotGlobalCoords(int slot) const;

	/// <summary>
	/// Get slot world size.
	/// </summary>
	/// <returns>World slot size.</returns>
	glm::vec2 getSlotSize() const;

	std::optional<Item>getItem(int slot) const;

	void setTargetEntity(Entity entity);

	void update() override;

	/// <summary>
	/// Renders inventory items on screen.
	/// </summary>
	/// <param name="screen">Takes renderer as a parameter to render things on the screen.</param>
	void render(graphics::Renderer& screen) override;
private:
	/// <summary>
	/// Finds the inventory slot that is being covered by a mouse(if mouse is on the inventory UI).
	/// </summary>
	void isCoveringInventory();

	/// <summary>
	/// If right mouse button is released uses an item at the covered slot.
	/// </summary>
	void isUsing();

	/// <summary>
	/// If left mouse button is pressed and the slot that mouse is covering contains an item than that item is being dragged.
	/// </summary>
	void isDragging();

	/// <summary>
	/// If while dragging an item user presses right mouse button than item is split to the covered slot.
	/// </summary>
	void isSplitting();

	/// <summary>
	/// If while dragging an item user releases the left mouse button that item is being moved to a new slot (possibly swapping two items if a new slot already contains an item).
	/// </summary>
	void isMovingItems();

	void drawItem(graphics::Renderer& screen, const Item& item, const glm::vec2& position, int index);

	int rows;
	int columns;

	std::optional<int> covered_slot;

	std::optional<int> dragged_slot;
	glm::vec2 dragged_position{};


	const UISettings& ui_settings;

	//Items
	const graphics::Font* font;
	std::vector<std::unique_ptr<graphics::Text>> slot_text;
	const graphics::SpriteSheet& item_sprites;

	Inventory* inventory;
	Entity target_entity;
};