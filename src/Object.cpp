#include "Object.hpp"

ObjectProperties::ObjectProperties(float durability, int sprite_index, const std::string& name,
	const std::vector<RandomizedItem>& drop, const glm::ivec2& size_tiles) 
	: durability(durability)
	, sprite_index(sprite_index)
	, name(name)
	, drop(drop)
	, size_tiles(size_tiles)
{
}

ObjectSpawnInfo::ObjectSpawnInfo(const std::vector<int>& spawn_tile_ids, float spawn_weight, const glm::ivec2& size_tiles, int object_properties_id) 
	: spawn_tile_ids(spawn_tile_ids)
	, spawn_weight(spawn_weight)
	, size_tiles(size_tiles)
	, object_properties_id(object_properties_id)
{
}

Object::Object(int object_id, int properties_id, const SDL_FRect& rect) 
	: object_id(object_id)
	, properties_id(properties_id)
	, rect(rect)
{
}
