#include "Item.hpp"

ItemProperties::ItemProperties(bool can_stack, int sprite_index, const std::string& name,
	std::vector<std::unique_ptr<ItemComponent>> components)
		: can_stack(can_stack)
		, sprite_index(sprite_index)
		, name(name)
		, components(std::move(components))
{

}
