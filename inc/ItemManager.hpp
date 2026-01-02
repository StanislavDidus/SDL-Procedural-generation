#include <vector>

#include "Item.hpp"

class ItemManager
{
public:
	ItemManager();
	~ItemManager();

	const ItemProperties& getProperties(int index) const;
	void addItem(const ItemProperties& properties);
private:
	std::vector<ItemProperties> items;
};