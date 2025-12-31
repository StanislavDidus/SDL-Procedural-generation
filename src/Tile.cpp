#include "Tile.hpp"

void Tile::dealDamage(float damage)
{
    current_durability -= damage;
    received_damage_last_frame = true;

    if (current_durability <= 0.f) is_destroyed = true;
}
