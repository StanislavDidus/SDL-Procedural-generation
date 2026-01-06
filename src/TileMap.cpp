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

TileMap::TileMap(World& world, SpriteSheet& tileset, std::shared_ptr<CollisionSystem> collision_system, float tile_width_world, float tile_height_world)
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
	auto cs = collision_system.lock();
	if (!cs) return;

	cs->collisions.clear();

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

	const auto& chunks = world.getChunks();

	//float position_x = 0.f;
	//float position_y = 0.f;

	SDL_FRect camera_rect;
	camera_rect.x = left_world;
	camera_rect.y = up_world;
	camera_rect.w = right_world - left_world;
	camera_rect.h = down_world - up_world;

	float chunk_width_tiles = 25.f;
	float chunk_height_tiles = 25.f;


	int active_chunks = 0;
	for (int i = 0; const auto& chunk : world.getChunks())
	{
		int chunk_x = i / (world.getSize().y / static_cast<int>(chunk_height_tiles));
		int chunk_y = i % (world.getSize().y / static_cast<int>(chunk_height_tiles));

		SDL_FRect chunk_rect;
		chunk_rect.x = chunk_x * chunk_width_tiles * tile_width_world;
		chunk_rect.y = chunk_y * chunk_height_tiles * tile_height_world;
		chunk_rect.w = chunk_width_tiles * tile_width_world;
		chunk_rect.h = chunk_height_tiles * tile_height_world;

		if (SDL_HasRectIntersectionFloat(&camera_rect, &chunk_rect))
		{
			active_chunks++;

			for (const auto& tile : chunk.tiles)
			{
				float tile_x = chunk_rect.x + tile.local_x * tile_width_world;
				float tile_y = chunk_rect.y + tile.local_y * tile_height_world;
				int sprite_index = tile.sprite_index;
				screen.drawScaledSprite(tileset[sprite_index], tile_x, tile_y, tile_width_world, tile_height_world);

				if (tile.solid) cs->collisions.emplace_back(glm::ivec2{ static_cast<int>(tile_x), static_cast<int>(tile_y) }, glm::ivec2{ tile_width_world, tile_height_world });
			}
		}

		++i;
	}

	//std::cout << active_chunks << std::endl;
	
	/*for (int x = begin_x; x < end_x; ++x)
	{
		for (int y = begin_y; y < end_y; ++y)
		{

		}
	}*/

	

	/*SDL_HasRectIntersection(nullptr, nullptr);

	for (int x = 0; x < world.getSize().x; x++)
	{
		for (int y = 0; y < world.getSize().y; y++)
		{
			auto& tile = world.getTiles()[y + x * world.getSize().y];
			int sprite_index = tile.sprite_index;
			screen.drawScaledSprite(tileset[sprite_index], position_x, position_y, tile_width_world, tile_height_world);

			if (tile.solid) cs->collisions.emplace_back(glm::ivec2{ position_x, position_y }, glm::ivec2{ tile_width_world, tile_height_world });

			position_y += tile_height_world;
		}

		position_x += tile_width_world;
		position_y = 0.f;
	}*/
	

//	for (int x = begin_x; x < end_x; x++)
//	{
//		for (int y = begin_y; y < end_y; y++)
//		{
//			const auto& chunk = world.getChunks();
//			const auto& tile = world.getTile(x,y);
//			int index = tile.sprite_index;
//			
//			glm::vec2 position = { x * tile_width_world, y * tile_height_world };
//			glm::vec2 size = { tile_width_world, tile_height_world };
//
//			if(tile.solid) cs->collisions.emplace_back(position, size);
//
//#ifdef DEBUG_TILES
//			switch (render_mode)
//			{
//			case 0:
//				screen.drawScaledSprite(tileset[index], x * tile_width_world, y * tile_height_world, tile_width_world, tile_height_world);
//				break;
//			case 1:
//				//PV
//				drawDebugInfo(screen, tile.pv, x, y);
//				break;
//			case 2:
//				//Temperature
//				drawDebugInfo(screen, tile.temperature, x, y);
//				break;
//			case 3:
//				//Moisture
//				drawDebugInfo(screen, tile.moisture, x, y);
//				break;
//			case 4:
//				//Durability
//				drawDebugInfo(screen, tile.current_durability / tile.max_durability, x, y);
//				break;
//			}
//#else
//			screen.drawScaledSprite(tileset[index], x * tile_width_world, y * tile_height_world, tile_width_world, tile_height_world);
//#endif
//		}
//		
//	}
//
//	world.resetChunks();
}

void TileMap::clear()
{
	chunks.clear();
}
