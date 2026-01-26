#pragma once

#include "UIElement.hpp"
#include "Inventory.hpp"
#include "SpriteSheet.hpp"
#include "InputManager.hpp"
#include "Font.hpp"
#include <vector>
#include "Text.hpp"

/// <summary>
/// Class that draws the inventory contents and lets the user manipulate it by using or dragging items.
/// </summary>
class InventoryView : public UIElement
{
public:
	InventoryView(const Font* font, const SpriteSheet& item_sprites, Inventory* inventory, int rows, int columns, float slot_size, const glm::vec2& position);

	/// <summary>
	/// Check if a mouse is on the inventory.
	/// </summary>
	/// <returns>Returns true if the mouse intersects with the inventory bounds.</returns>
	bool isMouseCoveringInventory() const;

	void update() override;

	/// <summary>
	/// Renders inventory items on screen.
	/// </summary>
	/// <param name="screen">Takes renderer as a parameter to render things on the screen.</param>
	void render(Renderer& screen) override;
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

	void drawItem(Renderer& screen, const Item& item, const glm::vec2& position, int index);

	Inventory* inventory;

	int rows;
	int columns;

	std::optional<int> covered_slot;

	std::optional<int> dragged_slot;
	glm::vec2 dragged_position{};


	float slot_size;

	//Items
	const Font* font;
	std::vector<std::unique_ptr<Text>> slot_text;
	const SpriteSheet& item_sprites;
};