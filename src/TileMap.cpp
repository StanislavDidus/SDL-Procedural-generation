#include "TileMap.hpp"
#include "Sprite.hpp"

TileMap::TileMap(Sprite& tileset, float width, float height, float raws, float columns) : tileset(tileset), tile_width(width / columns), tile_height(height / raws)
{
	float max_tiles = raws * columns;
	tiles.reserve(max_tiles);
}

TileMap::~TileMap()
{
}

void TileMap::render(Renderer& screen)
{
	for (const auto& tile : tiles)
	{
		tileset.setFrame(tile.index);

		float x = tile.column * tile_width;
		float y = tile.raw * tile_height;

		screen.drawScaledSprite(tileset, x, y, tile_width, tile_height);
	}
}

void TileMap::setTile(int index, int raw, int column)
{
	tiles.emplace_back(index, raw, column);
}
