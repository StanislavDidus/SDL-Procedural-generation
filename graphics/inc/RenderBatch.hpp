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
	extern int MAX_SPRITES_IN_BATCH;
	extern int MAX_RECTANGLES_IN_BATCH;
	extern int MAX_LINES_IN_BATCH;
	extern int MAX_TILEMAPS_IN_BATCH;
	
	class Batch
	{
	public:
		Batch() = default;
		Batch(std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline);
		virtual ~Batch() = default;

		void setMatrix(const glm::mat4& matrix);
		virtual void flushBatch(CommandBuffer& command_buffer, SDL_GPUColorTargetInfo& target_info, bool& first_render) = 0;
		virtual void reset() = 0;
	protected:
		std::shared_ptr<SDL_GPUDevice> device;
		std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline;

		size_t offset = 0;
		bool first_draw = true;
		glm::mat4 matrix;
	};
	class SpriteBatch : public Batch
	{
	public:
		SpriteBatch() = default;
		SpriteBatch(std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline);
		~SpriteBatch() override = default;


		void addToBatch(const GpuSprite& sprite);
		void flushBatch(CommandBuffer& command_buffer, SDL_GPUColorTargetInfo& target_info, bool& first_render) override;
		bool canBatch(const GpuSprite& gpu_sprite) const;

		void reset() override;
	private:
		std::vector<SpriteData> sprites;
		std::shared_ptr<GpuTexture> texture;

		GpuBuffer storage_buffer;
		GpuTransferBuffer transfer_buffer;

		SpriteUniform sprite_uniform{};
	};
	
	class RectangleBatch : public Batch
	{
	public:
		RectangleBatch() = default;
		RectangleBatch(std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline);
		~RectangleBatch() override = default;

		void addToBatch(const RectangleData& rectangle_data);
		void flushBatch(CommandBuffer& command_buffer, SDL_GPUColorTargetInfo& target_info, bool& first_render) override;
		bool canBatch(const RectangleData& rectangle_data) const;

		void reset() override;
	private:
		std::vector<Vertex> vertices;

		GpuBuffer vertices_buffer;
		GpuTransferBuffer transfer_buffer;
	};

	class LineBatch : public Batch
	{
	public:
		LineBatch() = default;
		LineBatch(std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline);
		~LineBatch() override = default;

		void addToBatch(const LineData& line_data);
		void flushBatch(CommandBuffer& command_buffer, SDL_GPUColorTargetInfo& target_info, bool& first_render) override;
		bool canBatch(const LineData& line_data) const;

		void reset() override;
	private:
		std::vector<Vertex> vertices;

		GpuBuffer line_buffer;
		GpuTransferBuffer transfer_buffer;
	};

	class TileMapBatch
	{
	public:
		TileMapBatch() = default;
		TileMapBatch(std::shared_ptr<SDL_GPUDevice> device, std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline);
		~TileMapBatch() = default;

		void setMatrix(const glm::mat4& matrix);

		void addToBatch(const RectangleData& rectangle_data);
		void flushBatch(CommandBuffer& command_buffer, SDL_GPUColorTargetInfo& target_info, bool& first_render);
		bool canBatch(const RectangleData& rectangle_data) const;

		void reset();

	private:
		std::vector<TileMapData> vertices;
		std::shared_ptr<SDL_GPUDevice> device;
		std::shared_ptr<GpuGraphicsPipeline> graphics_pipeline;

		GpuBuffer vertices_buffer;
		GpuTransferBuffer transfer_buffer;

		size_t offset;
		bool first_draw = true;

		glm::mat4 world_matrix;
	};

}
