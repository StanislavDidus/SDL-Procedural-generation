#include "TileMap.hpp"
#include "Sprite.hpp"

TileMap::TileMap(SpriteSheet& tileset, float width, float height, float rows, float columns) : tileset(tileset), tile_width(width / columns), tile_height(height / rows)
{
	float max_tiles = rows * columns;
	//tiles.reserve(static_cast<size_t>(max_tiles));
}

TileMap::~TileMap()
{
}

void TileMap::render(Renderer& screen)
{
	int chunk_counter = 0;
	for (const auto& chunk : chunks)
	{
		for (const auto& tile : chunk.tiles)
		{
			float x = tile.column * tile_width + chunk.x;
			float y = tile.row * tile_height + chunk.y;

			screen.drawScaledSprite(tileset[tile.index], x, y, tile_width, tile_height);
		}

		chunk_counter++;
	}

	std::cout << "Number of chunks rendered: " << chunk_counter << std::endl;
}

void TileMap::setDirtTiles()
{
	/*for (const auto& tile : tiles)
	{
		tiles.emplace_back(14, tile.row + 1, tile.column);
	}*/
}

void TileMap::setTile(int index, int row, int column)
{
	//tiles.emplace_back(index, row, column);
}

void TileMap::setChunks(const std::vector<Chunk>& chunks)
{
	this->chunks = chunks;
}

glm::vec2 TileMap::getTileSize() const
{
	return { tile_width, tile_height };
}

Chunk& TileMap::getChunk(int index) const
{
	return Chunk();
}

void TileMap::clear()
{
	chunks.clear();
}
