#pragma once

#include <iostream>
#include <vector>

#include "SpriteSheet.hpp"
#include "glm/vec2.hpp"
#include "Chunk.hpp"
#include "World.hpp"
#include "ECS/Systems.hpp"
#include "Debug.hpp"

class TileMap
{
public:
#ifdef DEBUG_TILES
	void drawDebugInfo(Renderer& screen, float value, float x, float y);
#endif

	TileMap(World& world, SpriteSheet& tileset, std::shared_ptr<CollisionSystem> collision_system, float tile_width_world, float tile_height_world);
	~TileMap();

	//Setters
	void setTileSize(float w, float h);
	void setTarget(const glm::vec2& target);

	void render(Renderer& screen);
	void clear();

	int render_mode = 0;
private:
	glm::vec2 target;

	float tile_width_world = 0.f;
	float tile_height_world = 0.f;

	SpriteSheet& tileset;
	std::vector<Chunk> chunks;
	World& world;
	std::weak_ptr<CollisionSystem> collision_system;
};