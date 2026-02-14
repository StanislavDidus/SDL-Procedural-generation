#include "Tile.hpp"

TileProperties::TileProperties(int sprite_index, TileType type, bool is_solid, float max_durability)
	: sprite_index(sprite_index)
	, type(type)
	, is_solid(is_solid)
	, max_durability(max_durability)
{
}

void Tile::dealDamage(float damage)
{
    current_durability -= damage;
    received_damage_last_frame = true;

    if (current_durability <= 0.f) is_destroyed = true;
}
