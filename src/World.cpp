#include "World.hpp"

#include "ObjectManager.hpp"
#include "ResourceManager.hpp"
#include "TileManager.hpp"
#include "ECS/Components.hpp"

World::World(const Grid<Tile>& grid, const std::vector<ObjectData>& objects, const std::vector<ChestData>& chests)
	: grid{grid}
	, objects{objects}
	, chests{chests}
{
	sprite_map.resize(grid.getColumns() * grid.getRows());

	updateSpriteMap();
}

void World::initWorld(entt::registry& registry, float tile_width, float tile_height)
{
	spawnObjects(registry, tile_width, tile_height);
	spawnChests(registry, tile_width, tile_height);
}

const std::vector<Uint32>& World::getSpriteMap() const
{
	return sprite_map;
}

void World::update(entt::registry& registry)
{
	std::vector<Entity> to_destroy;
	auto view = registry.view<Components::Object>();
	for (auto [entity, object_component] : view.each())
	{
		if (object_component.current_durability <= 0.0f)
		{
			to_destroy.push_back(entity);
		}
	}

	for (const auto& entity : to_destroy)
	{
		registry.destroy(entity);
	}
}

void World::updateSpriteMap()
{
	for (int y = 0; y < grid.getRows(); ++y)
	{
		for (int x = 0; x < grid.getColumns(); ++x)
		{
			int sprite_index = TileManager::get().getProperties(grid(x, y).id).sprite_index;
			sprite_map[x + y * grid.getColumns()] = sprite_index;
		}
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
