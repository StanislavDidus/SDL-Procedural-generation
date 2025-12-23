#include "TileMap.hpp"
#include "Sprite.hpp"

TileMap::TileMap(World& world, SpriteSheet& tileset, CollisionSystem& collision_system, float tile_width_world, float tile_height_world)
	: world(world)
	, tileset(tileset)
	, collision_system(collision_system)
	, tile_width_world(tile_width_world)
	, tile_height_world(tile_height_world)
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
	collision_system.collisions.clear();

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
			const auto& tile = world.getTile(x,y);
			int index = tile.index;

			if (tile.solid)
			{
				glm::vec2 position = { x * tile_width_world, y * tile_height_world };
				glm::vec2 size = { tile_width_world, tile_height_world };

				collision_system.collisions.emplace_back(position, size);


				switch (render_mode)
				{
				case 0:
					screen.drawScaledSprite(tileset[index], x * tile_width_world, y * tile_height_world, tile_width_world, tile_height_world);
					break;
				case 1:
					//PV
				{
					uint8_t c = static_cast<uint8_t>(tile.debug_info.pv * 255.f + 0.5f);
					Color color{ c,c,c };
					screen.drawRectangle(x * tile_width_world, y * tile_height_world, tile_width_world, tile_height_world, RenderType::FILL, color);
					break;
				}
				case 2:
					//Temperature
				{
					uint8_t c = static_cast<uint8_t>(tile.debug_info.temperature * 255.f + 0.5f);
					Color color{ c,c,c };
					screen.drawRectangle(x * tile_width_world, y * tile_height_world, tile_width_world, tile_height_world, RenderType::FILL, color);
					break;
				}
				case 3:
					//Moisture
				{
					uint8_t c = static_cast<uint8_t>(tile.debug_info.moisture * 255.f + 0.5f);
					Color color{ c,c,c };
					screen.drawRectangle(x * tile_width_world, y * tile_height_world, tile_width_world, tile_height_world, RenderType::FILL, color);
					break;
				}
				}
			}
			else
			{
				screen.drawScaledSprite(tileset[index], x * tile_width_world, y * tile_height_world, tile_width_world, tile_height_world);
			}
		}
	}

	world.resetChunks();
}

void TileMap::clear()
{
	chunks.clear();
}
