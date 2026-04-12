#pragma once
#include "GpuRenderer.hpp"

struct Background
{
	glm::vec2 start_pos{};
	float parallax_effect = 0.0f;
	glm::vec2 size{};
	graphics::Sprite sprite;
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

	glm::vec2 offset{0.0f, -100.0f};
	std::vector<Background> backgrounds;
	std::vector<Background> cave_backgrounds;
	float start_pos_y = 0.0f;
};
