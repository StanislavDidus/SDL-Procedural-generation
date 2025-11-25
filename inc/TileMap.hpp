#pragma once

#include <iostream>
#include <vector>

#include "Renderer.hpp"
#include "Tile.hpp"

class TileMap
{
public:
	TileMap(Sprite& tileset, float width, float height, float raws, float columns);
	~TileMap();

	void render(Renderer& screen);

	void setTile(int index, int raw, int column);
private:
	float tile_width = 0.f;
	float tile_height = 0.f;

	Sprite& tileset;
	std::vector<Tile> tiles;
};