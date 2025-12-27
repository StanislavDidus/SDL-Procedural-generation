#pragma once

#include "ECS/ComponentManager.hpp"
#include "ECS/EntityManager.hpp"
#include "Renderer.hpp"
#include "World.hpp"

#include "InputManager.hpp"

static bool AABB(const Transform& a, const Transform& b)
{
	return a.position.x <= b.position.x + b.size.x &&
		a.position.x + a.size.x >= b.position.x && 
		a.position.y <= b.position.y + b.size.y &&
		a.position.y + a.size.y >= b.position.y;
}

struct PhysicsSystem 
{
	PhysicsSystem(ComponentManager& component_manager, const EntityManager& entity_manager)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
	{

	}

	ComponentManager& component_manager;
	const EntityManager& entity_manager;

	void update(float dt) 
	{
		for (auto& entity : entity_manager.getEntities())
		{
			if (component_manager.transform.contains(entity) && component_manager.physics.contains(entity))
			{
				auto& ts = component_manager.transform.at(entity);
				auto& ph = component_manager.physics.at(entity);

				//Add gravity
				float gravity = 800.f;
				ph.velocity.y += gravity * dt;
				
				ts.position += ph.velocity * dt;
			}
		}
	}
};

enum class ColliderType
{
	ENTITY,
	TILE,
};

struct Collider
{
	ColliderType type;
	glm::vec2 position;
	glm::vec2 size;
};

class CollisionSystem
{
public:
	CollisionSystem(ComponentManager& component_manager, const EntityManager& entity_manager)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
	{

	}

	void addCollider(const glm::vec2& position, const glm::vec2& size, ColliderType type)
	{
		//collisions.emplace_back(type, position, size);
	}

	void update(float dt)
	{
		for (auto& entity : entity_manager.getEntities())
		{
			if (component_manager.transform.contains(entity) && component_manager.physics.contains(entity))
			{
				auto& ts = component_manager.transform.at(entity);
				auto& ph = component_manager.physics.at(entity);

				ph.is_ground = false;

				checkCollisions(ts, ph);
			}
		}
	}

	std::vector<Transform> collisions;
private:
	void checkCollisions(Transform& ts, Physics& ph)
	{
		for (const auto& collision : collisions)
		{
			if (AABB(ts, collision))
			{
				glm::vec2 p_min = { ts.position.x, ts.position.y };
				glm::vec2 p_max = { ts.position.x + ts.size.x, ts.position.y + ts.size.y };

				glm::vec2 c_min = { collision.position.x, collision.position.y };
				glm::vec2 c_max = { collision.position.x + collision.size.x, collision.position.y + collision.size.y };

				float x_overlap = glm::min(p_max.x, c_max.x) - glm::max(p_min.x, c_min.x);
				float y_overlap = glm::min(p_max.y, c_max.y) - glm::max(p_min.y, c_min.y);

				glm::vec2 normal = { 0.f, 0.f };

				//Chec x collision first
				if (x_overlap < y_overlap)
				{
					//If collider is on the right side
					if (p_min.x < c_min.x)
					{
						Transform step = { {ts.position.x + ts.size.x, ts.position.y + ts.size.y - ph.step}, {ph.step, ph.step} };

						if (isSpaceAbove(step, ts.size.y) && ph.velocity.x > 0.f)
						{
							ts.position.y -= ph.step;
							ts.position.x += 1.f;
						}
						else
						{
							normal.x = -1.f;
							ph.velocity.x = 0.f;
						}
					}
					//If collider is on the left side
					else
					{
						Transform step = { {ts.position.x - ph.step, ts.position.y + ts.size.y - ph.step}, {ph.step, ph.step} };

						if (isSpaceAbove(step, ts.size.y) && ph.velocity.x < 0.f)
						{
							ts.position.y -= ph.step;
							ts.position.x -= 1.f;
						}
						else
						{
							normal.x = 1.f;
							ph.velocity.x = 0.f;
						}
					}
				}
				//Check y collision
				else
				{
					if (p_min.y < c_min.y)
					{
						normal.y = -1.f;
						ph.is_ground = true;
					}
					else
					{
						normal.y = 1.f;
					}
					ph.velocity.y = 0.f;
				}

				//Resolve
				ts.position += glm::vec2{ x_overlap * normal.x, y_overlap * normal.y };
			}
		}
	}

	bool isSpaceAbove(const Transform& step, float height)
	{
		Transform above = { {step.position.x + 1.f, step.position.y - height + 1.f}, {step.size.x - 2.f, height - 2.f} };

		for (auto& collision : collisions)
		{
			if (AABB(above, collision))
			{
				return false;
			}
		}

		return true;
	}

	ComponentManager& component_manager;
	const EntityManager& entity_manager;

	
};

struct InputSystem
{
	InputSystem(ComponentManager& component_manager, const EntityManager& entity_manager)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
	{

	}

	ComponentManager& component_manager;
	const EntityManager& entity_manager;

	void update(float dt)                                              
	{
		for (auto& entity : entity_manager.getEntities())
		{
			if (component_manager.player.contains(entity) && component_manager.physics.contains(entity))
			{
				auto& p = component_manager.player.at(entity);
				auto& ph = component_manager.physics.at(entity);

				if (component_manager.jump.contains(entity))
				{
					auto& j = component_manager.jump.at(entity);
					j.jump_ready = InputManager::isKeyDown(SDLK_U);
				}

				if (component_manager.mine_intent.contains(entity))
				{
					auto& mi = component_manager.mine_intent.at(entity);
					mi.active = InputManager::getMouseState().left;
				}

				if (component_manager.place_intent.contains(entity))
				{
					auto& pi = component_manager.place_intent.at(entity);
					pi.active = InputManager::getMouseState().right;
				}

				if (InputManager::isKey(SDLK_H))
				{
					ph.velocity.x -= ph.acceleration.x * dt;
					ph.velocity.x = std::clamp(ph.velocity.x, -ph.max_velocity.x, ph.max_velocity.x);
				}
				if (InputManager::isKey(SDLK_K))
				{
					ph.velocity.x += ph.acceleration.x * dt;
					ph.velocity.x = std::clamp(ph.velocity.x, -ph.max_velocity.x, ph.max_velocity.x);
				}
				if(!InputManager::isKey(SDLK_K) && !InputManager::isKey(SDLK_H))
				{
					ph.velocity.x -= ph.velocity.x * ph.decelaration * dt;
				}
			}
		}
	}
};

struct JumpSystem
{
	JumpSystem(ComponentManager& component_manager, const EntityManager& entity_manager)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
	{

	}

	ComponentManager& component_manager;
	const EntityManager& entity_manager;

	void update(float dt)
	{
		for (auto& entity : entity_manager.getEntities())
		{
			if (component_manager.physics.contains(entity) && component_manager.jump.contains(entity))
			{
				auto& ph = component_manager.physics[entity];
				auto& j = component_manager.jump[entity];

				if (j.jump_ready && ph.is_ground)
				{
					ph.velocity.y -= j.jump_force;
					j.jump_ready = false;
					ph.is_ground = false;
				}
			}
		}
	}
};

class MiningSystem
{
public:
	MiningSystem(ComponentManager& component_manager, const EntityManager& entity_manager, World& world, float tile_width, float tile_height)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
		, tile_width(tile_width)
		, tile_height(tile_height)
		, world(world)
	{

	}

	void update(float dt, const MouseState& mouse_state, Renderer& screen)
	{
		for (const auto& entity : entity_manager.getEntities())
		{
			if (component_manager.transform.contains(entity) && component_manager.mine_ability.contains(entity) && component_manager.mine_intent.contains(entity))
			{
				auto& mi = component_manager.mine_intent.at(entity);

				if (!mi.active) continue;

				auto& ts = component_manager.transform.at(entity);
				auto& mn = component_manager.mine_ability.at(entity);
					

				const auto& view_position = screen.getView();
				const auto& zoom = screen.getZoom();
				const auto& window_size = screen.getWindowSize();

				glm::vec2 mid_screen = { window_size.x / 2.f, window_size.y / 2.f };
				glm::vec2 view_centered = view_position + mid_screen;
				glm::vec2 mid_position = ts.position + ts.size * 0.5f;

				const auto& mouse_position = view_centered + (mouse_state.position - mid_screen) / zoom;

				float distance = glm::distance(mid_position, mouse_position);

				if (distance > mn.radius) continue;

				int tile_x = static_cast<int>(std::floor((mouse_position.x) / tile_width));
				int tile_y = static_cast<int>(std::floor((mouse_position.y) / tile_height));

				world.damageTile(tile_x, tile_y, mn.speed * dt);
			}
		}
	}
	
private:
	ComponentManager& component_manager;
	const EntityManager& entity_manager;

	World& world;

	float tile_width = 1.f;
	float tile_height = 1.f;
};

class PlaceSystem
{
public:
	PlaceSystem(ComponentManager& component_manager, const EntityManager& entity_manager, World& world, float tile_width, float tile_height)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
		, tile_width(tile_width)
		, tile_height(tile_height)
		, world(world)
	{
	}

	void update(float dt, const MouseState& mouse_state, Renderer& screen)
	{
		for (const auto& entity : entity_manager.getEntities())
		{
			if (component_manager.transform.contains(entity) && component_manager.place_ability.contains(entity) && component_manager.place_intent.contains(entity))
			{
				auto& pi = component_manager.place_intent.at(entity);

				if (!pi.active) continue;

				auto& ts = component_manager.transform.at(entity);
				auto& pl = component_manager.place_ability.at(entity);


				const auto& view_position = screen.getView();
				const auto& zoom = screen.getZoom();
				const auto& window_size = screen.getWindowSize();

				glm::vec2 mid_screen = { window_size.x / 2.f, window_size.y / 2.f };
				glm::vec2 view_centered = view_position + mid_screen;
				glm::vec2 mid_position = ts.position + ts.size * 0.5f;

				const auto& mouse_position = view_centered + (mouse_state.position - mid_screen) / zoom;

				float distance = glm::distance(mid_position, mouse_position);

				if (distance > pl.radius) continue;

				int tile_x = static_cast<int>(std::floor((mouse_position.x) / tile_width));
				int tile_y = static_cast<int>(std::floor((mouse_position.y) / tile_height));

				world.placeTile(tile_x, tile_y, BlockType::GRASS);
			}
		}
	}

private:
	ComponentManager& component_manager;
	const EntityManager& entity_manager;
	World& world;
	float tile_width = 1.f;
	float tile_height = 1.f;
};