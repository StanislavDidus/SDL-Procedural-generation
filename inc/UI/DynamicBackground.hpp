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
	glm::vec2 offset{0.0f, -80.0f};
	std::vector<Background> backgrounds;
};
