#pragma once

#include "GpuBuffer.hpp"
#include "GpuTexture.hpp"
#include <glm/vec4.hpp>

namespace graphics
{
	struct TileData
	{
		glm::vec4 position; // Position3 Padding1
		glm::vec4 size_uv; // Size2 UV2
		glm::vec4 color; // Color4
	};

	class Chunk
	{
	public:
		Chunk(std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuTexture> texture,
			int width_tiles, int height_tiles,
			float tile_width_world, float tile_height_world,
			float offset_x, float offset_y,
			int index);
			
		~Chunk() = default;

		// Setters
		/// @param sprite_map A vector of Uint32 where each integer is an index of a sprite
		/// in a tileset.
		/// @return Void.
		void setSpriteMap(const std::vector<Uint32>& sprite_map);
		
		// Getters
		const SDL_FRect& getRect() const;
		std::shared_ptr<GpuTexture> getTexture() const;
		int getIndex() const;
		int getSize() const;
		int getWidth() const;
		int getHeight() const;
		SDL_GPUBuffer* getTileBuffer();
		SDL_GPUBuffer* getSpriteBuffer();
	private:
		GpuBuffer tile_buffer;
		GpuBuffer sprite_buffer;
		
		SDL_FRect rect;
		
		std::shared_ptr<SDL_GPUDevice> device;
		std::shared_ptr<GpuTexture> texture;
		int width_tiles, height_tiles;
		int size;
		int index;
	};
}
