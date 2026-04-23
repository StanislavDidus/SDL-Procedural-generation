#pragma once

#include <memory>

#include "GpuBuffer.hpp"
#include "GpuTexture.hpp"
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

namespace graphics
{
	struct TileData
	{
		glm::vec4 position; // Position3 Padding1
		glm::vec4 size_uv; // Size2 UV2
		glm::vec4 color; // Color4
	};

	class TileMap
	{
	public:
		TileMap(std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuTexture> texture, int world_width_tiles, int world_height_tiles,
			int tile_width_world, int tile_height_world,
			int chunk_width_tile, int chunk_height_tile,
			float offset_x, float offset_y);
		~TileMap() = default;

		// Getters
		int getSize() const;
		std::shared_ptr<GpuTexture> getTexture() const;

		// Setters
		void setSpriteMap(const std::vector<Uint32>& sprite_map);

		SDL_GPUBuffer* getTileBuffer();
		SDL_GPUBuffer* getSpriteBuffer();
	private:
		GpuBuffer tile_buffer;
		GpuBuffer sprite_buffer;
		std::shared_ptr<SDL_GPUDevice> device;
		std::shared_ptr<GpuTexture> texture;
		int size;
	};
}
