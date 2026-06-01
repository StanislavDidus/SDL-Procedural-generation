#pragma once
#include <graphics/Renderer.hpp>

struct Background
{
	glm::vec2 start_pos{};
	float parallax_effect = 0.0f;
	glm::vec2 size{};
	graphics::Sprite sprite;
	bool night = false;
};

class DynamicBackground
{
public:
	DynamicBackground();
	~DynamicBackground() = default;

	void update(const glm::vec2& camera_position);
	void render(graphics::Renderer& screen, const glm::vec2& camera_position) const;

	void setGlobalTime(float time);
private:
	glm::vec2 offset{0.0f, -80.0f};
	std::vector<Background> backgrounds;
	float global_time = 0.0f; // 0.0 - day, 1.0f - night
};
