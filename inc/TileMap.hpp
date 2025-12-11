#pragma once

#include <iostream>
#include <vector>

#include "SpriteSheet.hpp"
#include "glm/glm.hpp"
#include "Chunk.hpp"
#include "World.hpp"

class TileMap
{
public:
	TileMap(World& world, SpriteSheet& tileset, float tile_width_world, float tile_height_world);
	~TileMap();

	void setTileSize(float w, float h);

	void render(Renderer& screen);
	void clear();
private:
	float tile_width_world = 0.f;
	float tile_height_world = 0.f;

	SpriteSheet& tileset;
	std::vector<Chunk> chunks;
	World& world;
};