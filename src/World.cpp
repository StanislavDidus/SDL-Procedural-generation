#include "World.hpp"

#include <UI/Button.hpp>

#include "ObjectManager.hpp"
#include "ResourceManager.hpp"
#include "TileManager.hpp"
#include "ECS/Components.hpp"
#include "glm/gtx/io.hpp"

World::World(const Grid<Tile>& grid, const std::vector<PortalData>& portals, const std::vector<ObjectData>& objects, const std::vector<ChestData>& chests)
	: grid{grid}
	, portals{portals}
	, objects{objects}
	, chests{chests}
{
	sprite_map.resize(grid.getColumns() * grid.getRows());
	chunks.resize((grid.getColumns() / chunk_width_tiles) * (grid.getRows() / chunk_height_tiles));
}

void World::initWorld(entt::registry& registry, float tile_width, float tile_height)
{
	spawnPortals(registry, tile_width, tile_height);
	spawnObjects(registry, tile_width, tile_height);
	spawnChests(registry, tile_width, tile_height);
}

void World::placeTile(int x, int y, int tile_id)
{
	float world_width_tiles = grid.getColumns();
	float world_height_tiles = grid.getRows();

	if (x < 0 || x > world_width_tiles - 1 || y < 0 || y > world_height_tiles - 1) return;

	auto& tile = grid(x, y);
	auto& tile_manager = TileManager::get();

	if (auto& tile_properties = tile_manager.getProperties(tile.id); !tile_properties.is_solid)
	{
		//Check adjacent tiles
		bool is_placement_allowed = false;
		for (int i = -1; i < 2; ++i) for (int j = -1; j < 2; ++j)
		{
			//Don't check the middle tile (it is where we want to place a new tile)
			if (i == 0 && j == 0) continue;

			int new_x = x + i;
			int new_y = y + j;

			if (new_x < 0 || new_x >= world_width_tiles || new_y < 0 || new_y >= world_height_tiles) continue;

			auto& tile_temp = grid(new_x, new_y);
			auto& properties_temp = tile_manager.getProperties(tile_temp.id);
			if (properties_temp.is_solid)
			{
				is_placement_allowed = true;
				break;
			}
		}

		if (is_placement_allowed)
		{
			tile.id = tile_id;
			tile.current_durability = TileManager::get().getProperties(tile.id).max_durability;
			chunks[getChunkIndexByTilePosition(x, y)].is_dirty = true;
			
			ResourceManager::get().getSound("Place Tile")->play();
		}
	}

}

void World::damageTile(int x, int y, float damage)
{
	float world_width_tiles = grid.getColumns();
	float world_height_tiles = grid.getRows();

	if (x < 0 || x > world_width_tiles - 1 || y < 0 || y > world_height_tiles - 1) return;

	auto& tile = grid(x, y);

	if (auto& tile_properties = TileManager::get().getProperties(tile.id); tile_properties.is_solid && !tile.attached)
	{
		tile.dealDamage(damage);
		
		chunks[getChunkIndexByTilePosition(x, y)].damaged_tile = true;
		
		// Render break animations
		
		float current_durability = tile.current_durability;
		float max_durability = tile_properties.max_durability;
		float percentage = 1.0f - current_durability / max_durability;
		
		damaged_tiles.emplace_back(percentage, glm::ivec2{x, y});

		if (tile.is_destroyed)
		{
			tile.id = TileManager::get().getTileID("Sky");
			tile.is_destroyed = false;
			chunks[getChunkIndexByTilePosition(x, y)].is_dirty = true;
		}
	}
}

const std::vector<Uint32>& World::getSpriteMap() const
{
	return sprite_map;
}

void World::update(entt::registry& registry)
{
	updateObjectsDurability(registry);
	updateTilesDurability();
	
	damaged_tiles.clear();
}

void World::render(graphics::GpuRenderer& screen, float tile_width_world, float tile_height_world) const
{
	for (const auto& tile : damaged_tiles)
	{
		if (tile.durability_percentage <= 0.25f)	
			graphics::drawScaledSprite(
				screen,
				ResourceManager::get().getSpriteSheet("tile_break_anim")->getSprite("TileBreak0"),
				tile.grid_position.x * tile_width_world,
				tile.grid_position.y * tile_height_world,
				tile_width_world,
				tile_height_world
				);
		else if (tile.durability_percentage <= 0.50f)	
			graphics::drawScaledSprite(
				screen,
				ResourceManager::get().getSpriteSheet("tile_break_anim")->getSprite("TileBreak1"),
				tile.grid_position.x * tile_width_world,
				tile.grid_position.y * tile_height_world,
				tile_width_world,
				tile_height_world
				);
		else if (tile.durability_percentage <= 0.75f)	
			graphics::drawScaledSprite(
				screen,
				ResourceManager::get().getSpriteSheet("tile_break_anim")->getSprite("TileBreak2"),
				tile.grid_position.x * tile_width_world,
				tile.grid_position.y * tile_height_world,
				tile_width_world,
				tile_height_world
				);
		else if (tile.durability_percentage <= 1.0f)	
			graphics::drawScaledSprite(
				screen,
				ResourceManager::get().getSpriteSheet("tile_break_anim")->getSprite("TileBreak3"),
				tile.grid_position.x * tile_width_world,
				tile.grid_position.y * tile_height_world,
				tile_width_world,
				tile_height_world
				);
	}	
}

void World::setSpriteMap(graphics::TileMap& tilemap)
{
	for (const auto& chunk : tilemap.getChunks())
	{
		int world_height_chunks = grid.getColumns() / chunk_width_tiles;
		int index = chunk->getIndex();
		int index_x = index % world_height_chunks;
		int index_y = index / world_height_chunks;
		
		if (chunks[index].is_dirty)
		{
			std::vector<Uint32> sprite_map;
			sprite_map.resize(chunk->getSize());
			for (int y = 0; y < chunk->getHeight(); ++y)
			{
				for (int x = 0; x < chunk->getWidth(); ++x)
				{
					int sprite_index = TileManager::get().getProperties(grid(index_x * chunk->getWidth() + x, index_y * chunk->getHeight() + y).id).sprite_index;
					sprite_map[x + y * chunk->getWidth()] = sprite_index;
				}
			}
			chunk->setSpriteMap(sprite_map);
			chunks[index].is_dirty = false;
		}
	}
}

void World::spawnPortals(entt::registry& registry, float tile_width, float tile_height)
{
	for (const auto& portal : portals)
	{
		Entity entity = registry.create();
		auto& ts = registry.emplace<Components::Transform>(entity);
		ts.position.x = portal.grid_rect.x * tile_width;
		ts.position.y = portal.grid_rect.y * tile_height;
		ts.size.x = portal.grid_rect.w * tile_width;
		ts.size.y = portal.grid_rect.h * tile_height;
		
		auto& renderable = registry.emplace<Components::Renderable>(entity);
		renderable.sprite = ResourceManager::get().getSpriteSheet("objects")->getSprite("Portal");
		registry.emplace<Components::AlwaysRender>(entity);
		
		auto& button = registry.emplace<Components::Button>(entity);
		button.global = true;
		
		auto& button_entered = registry.emplace<Components::ButtonEnteredSprite>(entity);
		button_entered.sprite = ResourceManager::get().getSpriteSheet("objects")->getSprite("Portal_Outlined");
		
		auto& button_exit = registry.emplace<Components::ButtonExitSprite>(entity);
		button_exit.sprite = ResourceManager::get().getSpriteSheet("objects")->getSprite("Portal");
		
		registry.emplace<Components::Portal>(entity);
	}
}

void World::spawnObjects(entt::registry& registry, float tile_width, float tile_height)
{	
	const auto& tile_set = ResourceManager::get().getSpriteSheet("objects");

	for (const auto& object_data : objects)
	{
		auto entity = registry.create();
		const auto& rect = object_data.grid_rect;
		auto& ts = registry.emplace<Components::Transform>(entity);
		ts.position.x = rect.x * tile_width;
		ts.position.y = rect.y * tile_height;
		ts.size.x = rect.w * tile_width;
		ts.size.y = rect.h * tile_height;

		const auto& sprite = tile_set->getSprite(ObjectManager::get().getProperties(object_data.properties_id).sprite_index);
		auto& renderable = registry.emplace<Components::Renderable>(entity);
		renderable.sprite = sprite;

		registry.emplace<Components::AlwaysRender>(entity);

		size_t object_id = object_data.properties_id;
		float maximum_durability = ObjectManager::get().getProperties(object_id).durability;
		auto& object_component = registry.emplace<Components::Object>(entity, object_id);
		object_component.current_durability = maximum_durability;

		object_entities.push_back(entity);
	}

}

void World::spawnChests(entt::registry& registry, float tile_width, float tile_height)
{
	for (const auto& chest_data : chests)
	{
		auto chest = registry.create();
		auto& ts = registry.emplace<Components::Transform>(chest);
		ts.position.x = chest_data.grid_rect.x * tile_width;
		ts.position.y = chest_data.grid_rect.y * tile_height;
		ts.size.x = chest_data.grid_rect.w * tile_width;
		ts.size.y = chest_data.grid_rect.h * tile_height;

		const auto& sprite = ResourceManager::get().getSpriteSheet("objects")->getSprite("Chest");
		auto& renderable = registry.emplace<Components::Renderable>(chest);
		renderable.sprite = sprite;
		registry.emplace<Components::AlwaysRender>(chest);

		auto& chest_component = registry.emplace<Components::Chest>(chest);
		chest_component.base_item = chest_data.base_item;
		chest_component.common_item = chest_data.common_item;
		chest_component.sand_item = chest_data.sand_item;
		chest_component.snow_item = chest_data.snow_item;

		registry.emplace<Components::Button>(chest, true);

		chest_entities.push_back(chest);
	}
}

int World::getChunkIndexByTilePosition(int x, int y) const
{
	int chunk_x = x / chunk_width_tiles;
	int chunk_y = y / chunk_height_tiles;
	int world_width_chunks = grid.getColumns() / chunk_width_tiles;
	int world_height_chunks = grid.getRows() / chunk_height_tiles;
	return chunk_x + chunk_y * world_width_chunks;
}

void World::updateObjectsDurability(entt::registry& registry)
{
	std::vector<Entity> to_destroy;
	auto view = registry.view<Components::Object>();
	for (auto [entity, object_component] : view.each())
	{
		if (object_component.current_durability <= 0.0f)
		{
			to_destroy.push_back(entity);
		}
		
		if (!object_component.received_damage_last_frame)
		{
			object_component.current_durability = ObjectManager::get().getProperties(object_component.id).durability;
		}
		
		object_component.received_damage_last_frame = false;
	}

	for (const auto& entity : to_destroy)
	{
		registry.destroy(entity);
	}
}

void World::updateTilesDurability()
{
	for (int i = 0; auto& chunk : chunks)
	{
		if (chunk.damaged_tile)
		{
			int world_width_chunks = grid.getColumns() / chunk_width_tiles;
			int world_height_chunks = grid.getRows() / chunk_height_tiles;
			
			int chunk_x = i % world_height_chunks;
			int chunk_y = i / world_height_chunks;
			
			for (int x = 0; x < chunk_width_tiles; ++x)
			{
				for (int y = 0; y < chunk_height_tiles; ++y)
				{
					auto& tile = grid(chunk_x * chunk_width_tiles + x, chunk_y * chunk_height_tiles + y);
					
					if (!tile.received_damage_last_frame)
					{
						tile.current_durability = TileManager::get().getProperties(tile.id).max_durability;
					}
				}
			}
			
			std::cout << "Chunk recovered." << std::endl;
			chunk.damaged_tile = false;
		}
		++i;
	}
	
	/*float world_width_tiles = grid.getColumns();
	float world_height_tiles = grid.getRows();

	const auto& tile_manager = TileManager::get();
	for (int x = 0; x < world_width_tiles; ++x)
	{
		for (int y = 0; y < world_height_tiles; ++y)
		{
			auto& tile = grid(x, y);

			if (tile.received_damage_last_frame == false)
			{
				tile.current_durability = tile_manager.getProperties(tile.id).max_durability;
			}
			tile.received_damage_last_frame = false;
		}
	}*/

}
