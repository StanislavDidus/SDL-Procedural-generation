#pragma once
#include <vector>

#include "CommandBuffer.hpp"
#include "Vertex.hpp"
#include "GpuBuffer.hpp"
#include "GpuGraphicsPipeline.hpp"
#include "GpuTransferBuffer.hpp"
#include "glm/mat4x4.hpp"

namespace graphics
{
	class RenderBatch
	{
	public:
		RenderBatch() = default;
		~RenderBatch() = default;

	private:

	};

	class SpriteBatch
	{
	public:
		SpriteBatch() = default;
		SpriteBatch(std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline);
		~SpriteBatch() = default;

		void setMatrix(const glm::mat4& matrix);

		void addToBatch(const SpriteData& sprite_data, std::shared_ptr<GpuTexture> texture);
		void flushBatch(CommandBuffer& command_buffer, SDL_GPUColorTargetInfo& target_info);
		bool canBatch(const GpuSprite& gpu_sprite) const;

		void reset();

	private:
		std::vector<SpriteData> sprites;
		std::shared_ptr<SDL_GPUDevice> device;
		std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline;
		std::shared_ptr<GpuTexture> texture;

		GpuBuffer storage_buffer;
		GpuTransferBuffer transfer_buffer;

		SpriteUniform sprite_uniform{};
		size_t sprite_offset = 0;
		bool first_draw = true;

		glm::mat4 world_matrix;
	};
	
	class RectangleBatch
	{
	public:
		RectangleBatch() = default;
		RectangleBatch(std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline);
		~RectangleBatch() = default;

		void setMatrix(const glm::mat4& matrix);

		void addToBatch(const RectangleData& rectangle_data);
		void flushBatch(CommandBuffer& command_buffer, SDL_GPUColorTargetInfo& target_info);
		bool canBatch(const RectangleData& rectangle_data) const;

		void reset();

	private:
		std::vector<RectangleData> rectangles;
		std::shared_ptr<SDL_GPUDevice> device;
		std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline;

		GpuBuffer vertices_buffer;
		GpuBuffer indices_buffer;
		GpuTransferBuffer transfer_buffer;

		size_t offset;
		bool first_draw = true;

		glm::mat4 world_matrix;
	};

}
