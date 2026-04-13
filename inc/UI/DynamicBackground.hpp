#pragma once
#include "GpuRenderer.hpp"

enum class BackgroundMoveType
{
	NONE,
	HORIZONTAL,
	ABOVE,
	VERTICAL,
	BOTH,
};

enum class BackgroundRenderType
{
	NONE,
	HORIZONTAL,
	ABOVE,
	VERTICAL,
	BOTH,
};

struct Background
{
	glm::vec2 start_pos{};
	float parallax_effect = 0.0f;
	glm::vec2 size{};
	graphics::Sprite sprite;
	BackgroundMoveType move_type = BackgroundMoveType::NONE;
	BackgroundRenderType render_type = BackgroundRenderType::NONE;

	std::optional<float> min_x = std::nullopt;
	std::optional<float> max_x = std::nullopt;
	std::optional<float> max_height = std::nullopt;
	std::optional<float> min_height = std::nullopt;
};

class DynamicBackground
{
public:
	DynamicBackground();
	~DynamicBackground() = default;

	void update(const glm::vec2& camera_position);
	void render(graphics::GpuRenderer& screen, const glm::vec2& camera_position) const;

private:
	static glm::vec2 getZoomedCoordinates(const graphics::GpuRenderer& screen, const glm::vec2& position);

	glm::vec2 offset{0.0f, -80.0f};
	std::vector<Background> backgrounds;
	std::vector<Background> cave_backgrounds;
	float start_pos_y = 0.0f;
};
