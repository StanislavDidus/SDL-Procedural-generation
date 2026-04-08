#pragma once
#include <cmath>
#include <vector>

#include "Chunk.hpp"
#include "ResourceManager.hpp"
#include "WorldHelper.hpp"
#include "World.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ApplyArmorEffects.hpp"

struct World;

template<int WorldWidthTiles, int WorldHeightTiles, int TileWidthWorld, int TileHeightWorld>
class WorldRenderer
{
public:
	WorldRenderer() = default;
	WorldRenderer(entt::registry& registry, const World& output);
	~WorldRenderer() = default;
	
	void update();
	void render(graphics::GpuRenderer& screen, const glm::vec2& target);

	void spawnObjects();
private:
	/// Const member function that takes <b>World</b> as a param
	/// and removes all objects from it that do not fit inside a chunk
	/// completely.
	/// @param output modifies the output by reference
	//void adjustOutput(World& output) const;

	static constexpr int ScreenWidth = 960;
	static constexpr int ScreenHeight = 540;

	static constexpr int ChunkWidthTiles = ScreenWidth / TileWidthWorld / 2;
	static constexpr int ChunkHeightTiles = ScreenHeight / TileHeightWorld;

	static constexpr float ChunkWidthWorld = ChunkWidthTiles * TileWidthWorld;
	static constexpr float ChunkHeightWorld = ChunkHeightTiles * TileHeightWorld;

	static constexpr int WorldWidthChunks = WorldWidthTiles / ChunkWidthTiles;
	static constexpr int WorldHeightChunks = WorldHeightTiles / ChunkHeightTiles;
	static constexpr int World_Chunks = WorldWidthChunks * WorldHeightChunks;

	const World& output;
	entt::registry& registry;

	//std::array<SDL_Vertex, WorldWidthTiles * WorldHeightTiles * 4> vertices;
	//std::array<int, WorldWidthTiles* WorldHeightTiles * 6> indices;
	std::vector<SDL_Vertex> vertices;
	std::vector<int> indices;
};

template <int WorldWidthTiles, int WorldHeightTiles, int TileWidthWorld, int TileHeightWorld>
WorldRenderer<WorldWidthTiles, WorldHeightTiles, TileWidthWorld, TileHeightWorld>::WorldRenderer(
	entt::registry& registry,
	const World& output)
	: output{ output }
	, registry(registry)
{
	}

template <int WorldWidthTiles, int WorldHeightTiles, int TileWidthWorld, int TileHeightWorld>
void WorldRenderer<WorldWidthTiles, WorldHeightTiles, TileWidthWorld, TileHeightWorld>::update()
{
}

template <int WorldWidthTiles, int WorldHeightTiles, int TileWidthWorld, int TileHeightWorld>
void WorldRenderer<WorldWidthTiles, WorldHeightTiles, TileWidthWorld, TileHeightWorld>::render(
	graphics::GpuRenderer& screen, const glm::vec2& target)
{
}

template <int WorldWidthTiles, int WorldHeightTiles, int TileWidthWorld, int TileHeightWorld>
void WorldRenderer<WorldWidthTiles, WorldHeightTiles, TileWidthWorld, TileHeightWorld>::spawnObjects(
)
{
}
