#include "TileMap.hpp"
#include "Sprite.hpp"

TileMap::TileMap(SpriteSheet& tileset, float width, float height, float rows, float columns) : tileset(tileset), tile_width(width / columns), tile_height(height / rows)
{
	float max_tiles = rows * columns;
	tiles.reserve(static_cast<size_t>(max_tiles));
}

TileMap::~TileMap()
{
}

void TileMap::render(Renderer& screen)
{
	for (const auto& tile : tiles)
	{
		float x = tile.column * tile_width;
		float y = tile.row * tile_height;

		screen.drawScaledSprite(tileset[tile.index], x, y, tile_width, tile_height);
	}
}

void TileMap::setTile(int index, int row, int column)
{
	tiles.emplace_back(index, row, column);
}

glm::vec2 TileMap::getTileSize() const
{
	return { tile_width, tile_height };
}

void TileMap::clear()
{
	tiles.clear();
}
