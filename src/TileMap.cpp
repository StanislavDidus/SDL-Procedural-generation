#include "TileMap.hpp"
#include "Sprite.hpp"

#ifdef DEBUG_TILES
void TileMap::drawDebugInfo(Renderer& screen, float value, float x, float y)
{
	uint8_t c = static_cast<uint8_t>(value * 255.f + 0.5f);
	Color color{ c,c,c };
	screen.drawRectangle(x * tile_width_world, y * tile_height_world, tile_width_world, tile_height_world, RenderType::FILL, color);
}
#endif

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

void TileMap::setTarget(const glm::vec2& target)
{
	this->target = target;
}


void TileMap::render(Renderer& screen)
{
	collision_system.collisions.clear();

	const auto& window_size = screen.getWindowSize();
	const auto& view_position = screen.getView();
	auto zoom = screen.getZoom();

	float halfW = (window_size.x * 0.5f) / zoom;
	float halfH = (window_size.y * 0.5f) / zoom;

	//glm::vec2 view_center = {view_position.x + window_size.x / 2.f, view_position.y + window_size.y / 2.f };

	float left_world = target.x - halfW;
	float right_world = target.x + halfW;
	int begin_x = static_cast<int>(std::floor(left_world / tile_width_world));
	int end_x = static_cast<int>(std::ceil(right_world / tile_width_world));

	float up_world = target.y - halfH;
	float down_world = target.y + halfH;
	int begin_y = static_cast<int>(std::floor(up_world / tile_height_world));
	int end_y = static_cast<int>(std::ceil(down_world / tile_height_world));

	for (int x = begin_x; x < end_x; x++)
	{
		for (int y = begin_y; y < end_y; y++)
		{
			const auto& tile = world.getTile(x,y);
			int index = tile.sprite_index;
			
			glm::vec2 position = { x * tile_width_world, y * tile_height_world };
			glm::vec2 size = { tile_width_world, tile_height_world };

			if(tile.solid) collision_system.collisions.emplace_back(position, size);

#ifdef DEBUG_TILES
			switch (render_mode)
			{
			case 0:
				screen.drawScaledSprite(tileset[index], x * tile_width_world, y * tile_height_world, tile_width_world, tile_height_world);
				break;
			case 1:
				//PV
				drawDebugInfo(screen, tile.pv, x, y);
				break;
			case 2:
				//Temperature
				drawDebugInfo(screen, tile.temperature, x, y);
				break;
			case 3:
				//Moisture
				drawDebugInfo(screen, tile.moisture, x, y);
			case 4:
				//Durability
				drawDebugInfo(screen, tile.current_durability / tile.max_durability, x, y);
			}
#else
			screen.drawScaledSprite(tileset[index], x * tile_width_world, y * tile_height_world, tile_width_world, tile_height_world);
#endif
		}
		
	}

	world.resetChunks();
}

void TileMap::clear()
{
	chunks.clear();
}
