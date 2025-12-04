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
			float x = (tile.column + chunk.x) * tile_width;
			float y = (tile.row + chunk.y) * tile_height;

			screen.drawTile(tileset[tile.index], x, y, tile_width, tile_height);
		}
		chunk_counter++;
	}

	//std::cout << "Number of chunks rendered: " << chunk_counter << std::endl;
	//std::cout << "Total number of chunks: " << chunks.size() << "\n";
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

void TileMap::clear()
{
	chunks.clear();
}
