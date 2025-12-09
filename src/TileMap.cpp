#include "TileMap.hpp"
#include "Sprite.hpp"

TileMap::TileMap(World& world, SpriteSheet& tileset, float width, float height, float rows, float columns)
	: world(world), tileset(tileset), tile_width(width / columns), tile_height(height / rows)
{
	float max_tiles = rows * columns;
	//tiles.reserve(static_cast<size_t>(max_tiles));
}

TileMap::~TileMap()
{
}

void TileMap::render(Renderer& screen)
{
	const auto& window_size = screen.getWindowSize();
	const auto& view_position = screen.getView();
	auto zoom = screen.getZoom();
	zoom = 1.f;

	float tile_width_world = tile_width;
	float left_world = view_position.x;
	float right_world = view_position.x + window_size.x / zoom;
	int begin_x = static_cast<int>(std::floor(left_world / tile_width_world));
	int end_x = static_cast<int>(std::ceil(right_world / tile_width_world));

	float tile_height_world = tile_height;
	float up_world = view_position.y;
	float down_world = view_position.y + window_size.y / zoom;
	int begin_y = static_cast<int>(std::floor(up_world / tile_height_world));
	int end_y = static_cast<int>(std::ceil(down_world / tile_height_world));

	for (int x = begin_x; x < end_x; x++)
	{
		for (int y = begin_y; y < end_y; y++)
		{
			int tile_id = world.getTile(x,y);

			screen.drawTile(tileset[tile_id], x * tile_width, y * tile_height, tile_width, tile_height);
		}
	}
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
