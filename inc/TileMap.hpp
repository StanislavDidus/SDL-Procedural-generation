#pragma once

#include <iostream>
#include <vector>

#include "Renderer.hpp"
#include "glm/glm.hpp"
#include "Tile.hpp"

class TileMap
{
public:
	TileMap(Sprite& tileset, float width, float height, float rows, float columns);
	~TileMap();

	void render(Renderer& screen);

	//Setters
	void setTile(int index, int row, int column);

	//Getters
	glm::vec2 getTileSize() const;

	void clear();
private:
	float tile_width = 0.f;
	float tile_height = 0.f;

	Sprite& tileset;
	std::vector<Tile> tiles;
};