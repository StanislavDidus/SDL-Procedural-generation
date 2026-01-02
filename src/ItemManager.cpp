#include "ItemManager.hpp"

ItemManager::ItemManager()
{
}

ItemManager::~ItemManager()
{
}

const ItemProperties& ItemManager::getProperties(int index) const
{
	return items[index];
}

void ItemManager::addItem(const ItemProperties& properties)
{
	items.push_back(properties);
}
