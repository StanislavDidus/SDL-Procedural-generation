
#include "TileMap.hpp"

#include <utility>

#include "GpuTransferBuffer.hpp"
#include "GpuRenderer.hpp"

graphics::TileMap::TileMap(std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuTexture> texture, int world_width_tiles, int world_height_tiles,
                           int tile_width_world, int tile_height_world, int chunk_width_tiles, int chunk_height_tiles)
{
	int world_width_chunks = world_width_tiles / chunk_width_tiles;
	int world_height_chunks = world_height_tiles / chunk_height_tiles;
	
	int index = 0;
	chunks.reserve(world_width_chunks * world_height_chunks);
	for (int y = 0; y < world_height_chunks; ++y)
	{
		float offset_y = y * chunk_height_tiles * tile_height_world;
		for (int x = 0; x < world_width_chunks; ++x)
		{
			float offset_x = x * chunk_width_tiles * tile_width_world;
			chunks.emplace_back(std::make_unique<Chunk>(
				device, texture,
				chunk_width_tiles, chunk_height_tiles,
				tile_width_world, tile_height_world,
				offset_x, offset_y,
				index++
				));
		}
	}
}

std::shared_ptr<graphics::GpuTexture> graphics::TileMap::getTexture() const
{
	return texture;
}

const std::vector<std::shared_ptr<graphics::Chunk>>& graphics::TileMap::getChunks() const
{
	return chunks;
}
