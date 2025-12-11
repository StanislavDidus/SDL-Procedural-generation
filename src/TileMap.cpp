#include "TileMap.hpp"
#include "Sprite.hpp"

TileMap::TileMap(World& world, SpriteSheet& tileset, float tile_width_world, float tile_height_world)
	: world(world), tileset(tileset), tile_width_world(tile_width_world), tile_height_world(tile_height_world)
{
}

TileMap::~TileMap()
{
}

void TileMap::setTileSize(float w, float h)
{
	tile_width_world = w;
	tile_height_world = h;
	world.clear();
}

void TileMap::render(Renderer& screen)
{
	const auto& window_size = screen.getWindowSize();
	const auto& view_position = screen.getView();
	auto zoom = screen.getZoom();

	float halfW = (window_size.x * 0.5f) / zoom;
	float halfH = (window_size.y * 0.5f) / zoom;

	glm::vec2 view_center = {view_position.x + window_size.x / 2.f, view_position.y + window_size.y / 2.f };

	float left_world = view_center.x - halfW;
	float right_world = view_center.x + halfW;
	int begin_x = static_cast<int>(std::floor(left_world / tile_width_world));
	int end_x = static_cast<int>(std::ceil(right_world / tile_width_world));

	float up_world = view_center.y - halfH;
	float down_world = view_center.y + halfH;
	int begin_y = static_cast<int>(std::floor(up_world / tile_height_world));
	int end_y = static_cast<int>(std::ceil(down_world / tile_height_world));

	for (int x = begin_x; x < end_x; x++)
	{
		for (int y = begin_y; y < end_y; y++)
		{
			int tile_id = world.getTile(x,y);

			screen.drawTile(tileset[tile_id], x * tile_width_world, y * tile_height_world, tile_width_world, tile_height_world);
		}
	}

	world.resetChunks();
}

void TileMap::clear()
{
	chunks.clear();
}
