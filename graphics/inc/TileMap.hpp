#pragma once

#include <memory>

#include "GpuTexture.hpp"
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"
#include "Chunk.hpp"

namespace graphics
{
	class TileMap
	{
	public:
		TileMap(std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuTexture> texture,
			int world_width_tiles, int world_height_tiles,
			int tile_width_world, int tile_height_world,
			int chunk_width_tile, int chunk_height_tile);
		~TileMap() = default;

		// Getters
		std::shared_ptr<GpuTexture> getTexture() const;
		const std::vector<std::shared_ptr<Chunk>>& getChunks() const;
	private:
		std::shared_ptr<SDL_GPUDevice> device;
		std::shared_ptr<GpuTexture> texture;
		
		std::vector<std::shared_ptr<Chunk>> chunks;
	};
}
