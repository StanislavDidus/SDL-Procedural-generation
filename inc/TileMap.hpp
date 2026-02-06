#pragma once

#include "SpriteSheet.hpp"
#include "Tile.hpp"
#include "Grid.hpp"

class TileMap
{
public:
	TileMap(const graphics::SpriteSheet& tileset, int rows, int columns);
	~TileMap();

	//Getters
	int getRows() const;
	int getColumns() const;
	const graphics::SpriteSheet& getSpriteSheet() const;

	const Tile& operator()(int x, int y) const
	{
		return grid(x, y);
	}

	Tile& operator()(int x, int y)
	{
		return grid(x, y);
	}
private:
	Grid<Tile> grid;
	const graphics::SpriteSheet& tileset;
};