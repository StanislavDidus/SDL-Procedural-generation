#include "UI/UserInterface.hpp"
#include "UI/FillBar.hpp"
#include "UI/InventoryView.hpp"
#include <utility>

void UserInterface::addFillBar(const glm::vec2& position, const glm::vec2& size, const float& value, float max_value, Color color)
{
	auto fill_bar = std::make_unique<FillBar>(position, size, value, max_value, color);
	elements.push_back(std::move(fill_bar));
}

void UserInterface::addInventoryView(const SpriteSheet& item_sprites, Inventory* inventory, int rows, int columns, float slot_size, const glm::vec2& position)
{
	auto inventory_view = std::make_unique<InventoryView>(item_sprites, inventory, rows, columns, slot_size, position);
	elements.push_back(std::move(inventory_view));
}

void UserInterface::update()
{
	for (const auto& element : elements)
	{
		element->update();
	}
}

void UserInterface::render(Renderer& screen)
{
	for (const auto& element : elements)
	{
		element->render(screen);
	}
}