#pragma once

#include <string>
#include "RandomizedItem.hpp"
#include "glm/vec2.hpp"
#include "Noise.hpp"
#include "SDL3/SDL_rect.h"

/// <summary>
/// Properties of an object that <b>remain unchanged</b> for all instances.
/// </summary>
struct ObjectProperties
{
	ObjectProperties(float durability, int sprite_index, const std::string& name,
		const std::vector<RandomizedItem>& drop, const glm::ivec2& size_tiles);

	float durability;
	int sprite_index;
	std::string name;
	std::vector<RandomizedItem> drop;
	glm::ivec2 size_tiles;
};

/// <summary>
/// Information about the conditions that the <b>world_generator</b> must pass in order spawn the object.
/// </summary>
struct ObjectSpawnInfo
{
	ObjectSpawnInfo(const std::vector<int>& spawn_tile_ids, float spawn_weight, const glm::ivec2& size_tiles, int object_properties_id);

	std::vector<int> spawn_tile_ids; ///< Ids of tiles that the object can spawn on.
	float spawn_weight; ///< The chance of object spawning relative to all other objects.
	glm::ivec2 size_tiles; ///< The area in tiles that must be clear for the object to spawn there.
	int object_properties_id; ///< ID used to get object's properties in <b>Object Manager</b>.
};

/// <summary>
/// Struct that represents a destroyable object in the world_generator
/// </summary>
struct Object
{
	Object(int object_id, int properties_id, const SDL_Rect& rect);

	size_t object_id = 0; ///< Object's unique id.
	size_t properties_id = 0; ///< ID used to get object's properties in <b>Object Manager</b>.
	SDL_Rect grid_rect; ///< Object's grid grid_rect.

	float current_durability = 0.0f;
	bool is_destroyed = false;
	bool received_damage_last_frame = false;

	void dealDamage(float damage)
	{
		current_durability -= damage;
		received_damage_last_frame = true;

		if (current_durability <= 0.f) is_destroyed = true;
	}
};

inline bool operator==(const Object& lhs, const Object& rhs)
{
	return lhs.object_id == rhs.object_id;
}