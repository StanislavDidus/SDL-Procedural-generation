#pragma once
#include <cmath>
#include <vector>

#include "Chunk.hpp"
#include "ResourceManager.hpp"
#include "WorldHelper.hpp"
#include "WorldOutput.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ApplyArmorEffects.hpp"

struct WorldOutput;

template<int WorldWidthTiles, int WorldHeightTiles, int TileWidthWorld, int TileHeightWorld>
class WorldRenderer
{
public:
	WorldRenderer() = default;
	WorldRenderer(entt::registry& registry, const WorldOutput& output);
	~WorldRenderer() = default;
	
	void update();
	void render(graphics::Renderer& screen, const glm::vec2& target);

	void spawnObjects();
private:
	/// Const member function that takes <b>WorldOutput</b> as a param
	/// and removes all objects from it that do not fit inside a chunk
	/// completely.
	/// @param output modifies the output by reference
	//void adjustOutput(WorldOutput& output) const;

	static constexpr int ScreenWidth = 960;
	static constexpr int ScreenHeight = 540;

	static constexpr int ChunkWidthTiles = ScreenWidth / TileWidthWorld / 2;
	static constexpr int ChunkHeightTiles = ScreenHeight / TileHeightWorld;

	static constexpr float ChunkWidthWorld = ChunkWidthTiles * TileWidthWorld;
	static constexpr float ChunkHeightWorld = ChunkHeightTiles * TileHeightWorld;

	static constexpr int WorldWidthChunks = WorldWidthTiles / ChunkWidthTiles;
	static constexpr int WorldHeightChunks = WorldHeightTiles / ChunkHeightTiles;
	static constexpr int World_Chunks = WorldWidthChunks * WorldHeightChunks;

	const WorldOutput& output;
	entt::registry& registry;

	std::array<Chunk<ChunkWidthTiles, ChunkHeightTiles>, World_Chunks> chunks;
	size_t chunk_count = 0;

	//std::array<SDL_Vertex, WorldWidthTiles * WorldHeightTiles * 4> vertices;
	//std::array<int, WorldWidthTiles* WorldHeightTiles * 6> indices;
	std::vector<SDL_Vertex> vertices;
	std::vector<int> indices;
};

template <int WorldWidthTiles, int WorldHeightTiles, int TileWidthWorld, int TileHeightWorld>
WorldRenderer<WorldWidthTiles, WorldHeightTiles, TileWidthWorld, TileHeightWorld>::WorldRenderer(
	entt::registry& registry,
	const WorldOutput& output)
	: output{ output }
	, registry(registry)
{
	for (int i = 0; i < World_Chunks; ++i)
	{
		int x = i % WorldWidthChunks;
		int y = i / WorldWidthChunks;

		glm::ivec2 grid_position = glm::ivec2{ x, y };
		SDL_FRect rect =
		{
			static_cast<float>(x) * ChunkWidthWorld,
			static_cast<float>(y) * ChunkHeightWorld,
			ChunkWidthWorld,
			ChunkHeightWorld
		};

		chunks[chunk_count++] = Chunk<ChunkWidthTiles, ChunkHeightTiles>{ rect, grid_position };
	}
}

template <int WorldWidthTiles, int WorldHeightTiles, int TileWidthWorld, int TileHeightWorld>
void WorldRenderer<WorldWidthTiles, WorldHeightTiles, TileWidthWorld, TileHeightWorld>::update()
{
	for (auto& chunk : chunks)
	{
		auto new_end = std::ranges::remove_if(chunk.objects, [this](const Entity& entity)
		{
			if (registry.all_of<Components::Object>(entity))
			{
				if (registry.get<Components::Object>(entity).is_destroyed)
				{
					registry.destroy(entity);
					return true;
				}
			}
			return false;
		}).begin();

		chunk.objects.erase(new_end, chunk.objects.end());
	}
}

template <int WorldWidthTiles, int WorldHeightTiles, int TileWidthWorld, int TileHeightWorld>
void WorldRenderer<WorldWidthTiles, WorldHeightTiles, TileWidthWorld, TileHeightWorld>::render(
	graphics::Renderer& screen, const glm::vec2& target)
{
	vertices.clear();
	indices.clear();
	vertices.reserve(WorldWidthTiles * WorldHeightTiles * 4);
	indices.reserve(WorldWidthTiles * WorldHeightTiles * 6);

	const auto& view_position = screen.getView();

	SDL_FRect camera_rect = graphics::getCameraRectFromTarget(screen, target);

	for (int i = 0; i < chunk_count; ++i)
	{
		auto& chunk = chunks[i];
		if (SDL_HasRectIntersectionFloat(&camera_rect, &chunk.rect))
		{
			for (int j = 0; j < ChunkWidthTiles * ChunkHeightTiles; ++j)
			{
				int x = j % ChunkWidthTiles;
				int y = j / ChunkWidthTiles;

				int tile_x = chunk.grid_position.x * ChunkWidthTiles + x;
				int tile_y = chunk.grid_position.y * ChunkHeightTiles + y;
				const auto& tile = output.grid(tile_x, tile_y);

				//If tile is not solid(Sky tile) do not render it
				if (TileManager::get().getProperties(tile.id).is_solid == false) continue;

				const auto& sprite = ResourceManager::get().getSpriteSheet("tiles")->getSprite(TileManager::get().getProperties(tile.id).sprite_index);
				const auto& sprite_rect = sprite.getRect();

				const auto& chunk_vertices = chunk.vertices;
				SDL_Vertex v = chunk_vertices[j * 4 + 0];
				SDL_Vertex v1 = chunk_vertices[j * 4 + 1];
				SDL_Vertex v2 = chunk_vertices[j * 4 + 2];
				SDL_Vertex v3 = chunk_vertices[j * 4 + 3];

				v.position -= view_position;
				v1.position -= view_position;
				v2.position -= view_position;
				v3.position -= view_position;

				graphics::zoomPoint(screen, v.position.x, v.position.y);
				graphics::zoomPoint(screen, v1.position.x, v1.position.y);
				graphics::zoomPoint(screen, v2.position.x, v2.position.y);
				graphics::zoomPoint(screen, v3.position.x, v3.position.y);

				const auto& texture_size = SDL_FPoint{ static_cast<float>(sprite.getTexture()->w), static_cast<float>(sprite.getTexture()->h) };
				SDL_FPoint t(sprite_rect.x / texture_size.x, sprite_rect.y / texture_size.y);
				SDL_FPoint t1((sprite_rect.x + sprite_rect.w) / texture_size.x, sprite_rect.y / texture_size.y);
				SDL_FPoint t2((sprite_rect.x + sprite_rect.w) / texture_size.x, (sprite_rect.y + sprite_rect.h) / texture_size.y);
				SDL_FPoint t3(sprite_rect.x / texture_size.x, (sprite_rect.y + sprite_rect.h) / texture_size.y);

				v.tex_coord = t;
				v1.tex_coord = t1;
				v2.tex_coord = t2;
				v3.tex_coord = t3;

				int base = vertices.size();

				vertices.push_back(v);
				vertices.push_back(v1);
				vertices.push_back(v2);
				vertices.push_back(v3);

				indices.push_back(base + 0);
				indices.push_back(base + 1);
				indices.push_back(base + 2);
				indices.push_back(base + 2);
				indices.push_back(base + 3);
				indices.push_back(base + 0);
			}

			// Render objects
			for (const auto& object : chunk.objects)
			{
				const auto& transform = registry.get<Components::Transform>(object);
				const auto& renderable = registry.get<Components::Renderable>(object);

				graphics::drawRotatedSprite(
					screen,
					renderable.sprite,
					transform.position.x,
					transform.position.y,
					transform.size.x,
					transform.size.y,
					0.0f,
					renderable.flip_mode,
					renderable.ignore_view_zoom
				);
			}
		}
	}

	const auto& tile_set = ResourceManager::get().getSpriteSheet("tiles")->getTexture();
	SDL_RenderGeometry(screen.get(), tile_set.get(), vertices.data(), vertices.size(), indices.data(), indices.size());
}

template <int WorldWidthTiles, int WorldHeightTiles, int TileWidthWorld, int TileHeightWorld>
void WorldRenderer<WorldWidthTiles, WorldHeightTiles, TileWidthWorld, TileHeightWorld>::spawnObjects(
)
{
	for (const auto& object : output.objects)
	{
		const auto object_grid_rect = object.grid_rect;
		SDL_FRect object_rect
		{
			static_cast<float>(object_grid_rect.x) * TileWidthWorld,
			static_cast<float>(object_grid_rect.y) * TileHeightWorld,
			TileWidthWorld,
			TileHeightWorld
		};

		for (int i = 0; i < chunk_count; ++i)
		{
			auto& chunk = chunks[i];
			if (isRectInsideFloat(object_rect, chunk.rect))
			{
				//If object completely fits inside a chunk we spawn it by using a registry	
				Entity object_entity = spawnObject(registry, object, TileWidthWorld, TileHeightWorld);
				chunk.objects.push_back(object_entity);
			}
		}
	}

	for (const auto& object : output.chests)
	{
		const auto object_grid_rect = object.grid_rect;
		SDL_FRect object_rect
		{
			static_cast<float>(object_grid_rect.x) * TileWidthWorld,
			static_cast<float>(object_grid_rect.y) * TileHeightWorld,
			TileWidthWorld,
			TileHeightWorld
		};

		for (int i = 0; i < chunk_count; ++i)
		{
			auto& chunk = chunks[i];
			if (isRectInsideFloat(object_rect, chunk.rect))
			{
				//If object completely fits inside a chunk we spawn it by using a registry	
				Entity object_entity = spawnChest(registry, object, TileWidthWorld, TileHeightWorld);
				chunk.objects.push_back(object_entity);
			}
		}
	}
}
