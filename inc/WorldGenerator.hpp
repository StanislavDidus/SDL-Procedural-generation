#pragma once

#include "glm/vec2.hpp"

#include <memory>
#include <array>
#include <optional>
#include <UI/Button.hpp>
#include <UI/Button.hpp>
#include <UI/Button.hpp>
#include <UI/Button.hpp>

#include "Biomes.hpp"
#include "ValueNoise.hpp"
#include "PerlynNoise.hpp"
#include "MapRange.hpp"
#include "Chunk.hpp"
#include "ObjectManager.hpp"
#include "TileManager.hpp"
#include "GenerationData.hpp"
#include "RenderFunctions.hpp"
#include "ECS/Entity.hpp"
#include "DrunkWalker.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "ECS/ApplyArmorEffects.hpp"
#include "World.hpp"

class TileCollisionSystem;

struct TileChange
{
	size_t id;
	glm::ivec2 position;
};

	
class WorldGenerator
{
public:
	WorldGenerator(
		GenerationData& generation_data,
		entt::registry& registry,
		int world_width_tiles,
		int world_height_tiles
	);
	~WorldGenerator() = default;

	[[nodiscard]] std::shared_ptr<World> generateWorld(std::optional<int> seed);
private:
	void initSeeds(std::optional<int> seed_opt);
	GenerationData& generation_data;
	entt::registry& registry;

	uint32_t master_seed;

	void generateBase(Grid<Tile>& grid) const;
	void addGrass(Grid<Tile>& grid) const;
	void addDirt(Grid<Tile>& grid) const;
	void addCaves(Grid<Tile>& grid) const;
	void applyCellularAutomata(Grid<Tile>& grid);
	void addWater(Grid<Tile>& grid) const;
	void addBiomes(Grid<Tile>& grid) const;
	void addObjects(std::vector<ObjectData>& objects, Grid<Tile>& grid);
	void addChests(std::vector<ChestData>& chests, std::vector<ObjectData>& objects, Grid<Tile>& grid);
	void setTileDurability(Grid<Tile>& grid) const;

	void removeTileCave(const glm::ivec2& position, Grid<Tile>& grid) const;

	int world_width_tiles;
	int world_height_tiles;
};