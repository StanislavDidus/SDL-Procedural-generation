#pragma once

#include <algorithm>

#include "Color.hpp"
#include "CraftingManager.hpp"
#include "ECS/ComponentManager.hpp"
#include "ECS/EntityManager.hpp"
#include "RenderFunctions.hpp"
#include "World.hpp"
#include "Item.hpp"
#include "RandomizedItem.hpp"

#include "InputManager.hpp"
#include "Inventory.hpp"
#include "glm/gtc/random.hpp"

constexpr float BASE_MINING_SPEED = 100.0f;
constexpr float BASE_MINING_RADIUS = 100.0f;
constexpr int BASE_MINING_SIZE = 2;

/// <summary>
/// Static function that takes an item and randomly decides whether add the item to the given inventory.
/// Quantities are also generated randomly based on the variables defined in <b>RandomizedItem</b> class.
/// </summary>
/// <param name="inventory">Takes an inventory where an item will be added.</param>
/// <param name="item">Item that is being added to the inventory</param>
static void addRandomizedItem(Inventory& inventory, const RandomizedItem& item)
{
	if (item.drop_chance == 0.0f) return;

	float drop_rand = glm::linearRand(0.0f, 1.0f);
	int quantities_rand = glm::linearRand(item.drop_quantity_min, item.drop_quantity_max);

	if (drop_rand <= item.drop_chance)
	{
		inventory.addItem(item.item_id, quantities_rand);
	}
}

/// <summary>
/// Checks if a mouse cursor is inside a bounding box.
/// </summary>
/// <param name="mouse_position">Position of a cursor,</param>
/// <param name="rect">Bounding box that we are checking.</param>
/// <returns>True if mouse is inside a bounding box.</returns>
static bool isMouseIntersection(const glm::vec2& mouse_position, const SDL_FRect& rect)
{
	return (mouse_position.x >= rect.x &&
		mouse_position.x < rect.x + rect.w &&
		mouse_position.y >= rect.y &&
		mouse_position.y < rect.y + rect.h);
}

/// <summary>
/// Checks intersection between two bounding boxes.
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
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
				float gravity = 1400.f;
				ph.velocity.y += gravity * dt;

				ts.position.y += ph.velocity.y * dt;

				if (ph.can_move_horizontal) ts.position.x += ph.velocity.x * dt;
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

	void update(const graphics::Renderer& screen, float dt)
	{
		//Calculate global mouse position
		glm::vec2 mouse_global_position = getMouseGlobalPosition(screen);

		for (auto& entity : entity_manager.getEntities())
		{
			if (component_manager.player.contains(entity))
			{
				auto& p = component_manager.player.at(entity);

				// Jump
				if (component_manager.jump.contains(entity))
				{
					auto& j = component_manager.jump.at(entity);
					j.jump_ready = InputManager::isKey(SDLK_U);
				}

				//Mining
				if (component_manager.mine_intent.contains(entity))
				{
					auto& mi = component_manager.mine_intent.at(entity);
					//mi.active = InputManager::getMouseState().left == MouseButtonState::HELD;

					if (InputManager::getMouseState().left == MouseButtonState::DOWN)
					{
						mi.active = true;
						mi.start_mouse_position = mouse_global_position;
					}
					if (InputManager::getMouseState().left == MouseButtonState::RELEASED || component_manager.mine_objects_finished.contains(entity))
					{
						mi.active = false;
					}

					mi.current_mouse_position = mouse_global_position;
				}

				//Block placement
				if (component_manager.place_intent.contains(entity))
				{
					auto& pi = component_manager.place_intent.at(entity);
					pi.active = InputManager::getMouseState().right == MouseButtonState::HELD;
					pi.target_global_position = mouse_global_position;
				}

				//Horizontal movement
				if (component_manager.physics.contains(entity))
				{
					auto& ph = component_manager.physics.at(entity);
					if (InputManager::isKey(SDLK_H))
					{
						ph.velocity.x -= ph.acceleration.x * dt;
						ph.velocity.x = std::clamp(ph.velocity.x, -ph.max_velocity.x, ph.max_velocity.x);

						//Flip
						if (component_manager.renderable.contains(entity)) component_manager.renderable.at(entity).flip_mode = SDL_FLIP_HORIZONTAL;
					}
					if (InputManager::isKey(SDLK_K))
					{
						ph.velocity.x += ph.acceleration.x * dt;
						ph.velocity.x = std::clamp(ph.velocity.x, -ph.max_velocity.x, ph.max_velocity.x);

						//Flip
						if (component_manager.renderable.contains(entity)) component_manager.renderable.at(entity).flip_mode = SDL_FLIP_NONE;
					}
					if (!InputManager::isKey(SDLK_K) && !InputManager::isKey(SDLK_H))
					{
						ph.velocity.x -= ph.velocity.x * ph.decelaration * dt;
					}

				}
			}
		}
	}

private:
	glm::vec2 getMouseGlobalPosition(const graphics::Renderer& screen)
	{
		const auto& view_position = screen.getView();
		const auto& zoom = screen.getZoom();
		const auto& window_size = screen.getWindowSize();

		glm::vec2 mid_screen = { window_size.x / 2.f, window_size.y / 2.f };
		glm::vec2 view_centered = view_position + mid_screen;
		return view_centered + (InputManager::getMouseState().position - mid_screen) / zoom;
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
				auto& ph = component_manager.physics.at(entity);
				auto& j = component_manager.jump.at(entity);

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

class MiningTilesSystem
{
public:
	MiningTilesSystem(ComponentManager& component_manager, const EntityManager& entity_manager, World& world, float tile_width, float tile_height)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
		, tile_width(tile_width)
		, tile_height(tile_height)
		, world(world)
	{

	}

	void update(float dt)
	{
		for (const auto& entity : entity_manager.getEntities())
		{
			if (!component_manager.transform.contains(entity) ||
				!component_manager.mine_intent.contains(entity) ||
				component_manager.mine_objects_state.contains(entity))
				continue;

			tiles_covered.clear();

			auto& mi = component_manager.mine_intent.at(entity);
			auto& ts = component_manager.transform.at(entity);

			float mining_speed;
			float mining_radius;
			float mining_size;
			if (component_manager.pickaxe.contains(entity))
			{
				const auto& pickaxe_component = component_manager.pickaxe.at(entity);
				const auto& item_properties = ItemManager::get().getProperties(pickaxe_component.item->id);
				const auto& pickaxe_data = item_properties.pickaxe_data;

				mining_speed = pickaxe_data->speed;
				mining_radius = pickaxe_data->radius;
				mining_size = pickaxe_data->size;
			}
			else
			{
				mining_speed = BASE_MINING_SPEED;
				mining_radius = BASE_MINING_RADIUS;
				mining_size = BASE_MINING_SIZE;
			}

			const auto& mid_position = ts.position + ts.size * 0.5f;

			int tile_x = static_cast<int>(std::floor((mi.current_mouse_position.x) / tile_width));
			int tile_y = static_cast<int>(std::floor((mi.current_mouse_position.y) / tile_height));

			//Mine in 3x3 radius
			int mine_size = mining_size;
			float mid_radius_f = std::floor(mine_size / 2.f);
			float mid_radius_c = std::ceil(mine_size / 2.f);
			for (int i = -mid_radius_f; i < mid_radius_c; i++)
			{
				for (int j = -mid_radius_f; j < mid_radius_c; j++)
				{
					tiles_covered.emplace_back(tile_x + i, tile_y + j);
				}
			}

			if (!mi.active) continue;

			for (const auto& tile_covered : tiles_covered)
			{
				glm::vec2 tile_position_global = { tile_covered.x * tile_width, tile_covered.y * tile_height };
				float distance = glm::distance(tile_position_global, mid_position);

					if (distance <= mining_radius)
						world.damageTile(tile_covered.x, tile_covered.y, mining_speed * dt); 
			}
		}
	}

	void renderOutline(graphics::Renderer& screen)
	{
		for (const auto& entity : entity_manager.getEntities())
		{
			if (!component_manager.transform.contains(entity) ||
				component_manager.mine_objects_state.contains(entity) ||
				!component_manager.mine_intent.contains(entity))
				continue;

			auto& ts = component_manager.transform.at(entity);

			float mining_radius;
			if (component_manager.pickaxe.contains(entity))
			{
				const auto& pickaxe_component = component_manager.pickaxe.at(entity);
				const auto& item_properties = ItemManager::get().getProperties(pickaxe_component.item->id);
				const auto& pickaxe_data = item_properties.pickaxe_data;

				mining_radius = pickaxe_data->radius;
			}
			else
			{
				mining_radius = BASE_MINING_RADIUS;
			}

			const auto& player_mid_position = ts.position + ts.size * 0.5f;

			for (const auto& tile_covered : tiles_covered)
			{
				glm::vec2 tile_position_global = { tile_covered.x * tile_width, tile_covered.y * tile_height };
				float distance = glm::distance(tile_position_global, player_mid_position);

				if (distance > mining_radius)
				{
					graphics::drawRectangle(screen, tile_position_global.x, tile_position_global.y, tile_width, tile_height, graphics::RenderType::FILL, graphics::Color::TRANSPARENT_RED);
				}
				else
				{
					graphics::drawRectangle(screen, tile_position_global.x, tile_position_global.y, tile_width, tile_height, graphics::RenderType::FILL, graphics::Color::TRANSPARENT_BLUE);
				}
			}
		}
	}

private:
	ComponentManager& component_manager;
	const EntityManager& entity_manager;

	World& world;

	float tile_width = 1.f;
	float tile_height = 1.f;

	std::vector<glm::ivec2> tiles_covered;
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

	void update(float dt)
	{
		for (const auto& entity : entity_manager.getEntities())
		{
			if (component_manager.transform.contains(entity) && component_manager.place_ability.contains(entity) && component_manager.place_intent.contains(entity))
			{
				auto& pi = component_manager.place_intent.at(entity);
				auto& pl = component_manager.place_ability.at(entity);

				pl.placing_timer += dt;

				if (!pi.active || pl.placing_timer < pl.placing_time) continue;

				pl.placing_timer = 0.f;

				auto& ts = component_manager.transform.at(entity);

				glm::vec2 mid_position = ts.position + ts.size * 0.5f;
				const auto& mouse_global_position = pi.target_global_position;

				float distance = glm::distance(mid_position, mouse_global_position);

				if (distance > pl.radius) continue;
				if (world.getObjectOnPosition(mouse_global_position)) continue;

				//Prevents from placing blocks on the player 
				float min_distance = glm::distance(mid_position, ts.position);
				if (distance < min_distance) continue;

				int tile_x = static_cast<int>(std::floor((mouse_global_position.x) / tile_width));
				int tile_y = static_cast<int>(std::floor((mouse_global_position.y) / tile_height));

				//TODO Temporary placed 0 instead of a tile id
				world.placeTile(tile_x, tile_y, 0);

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

class MiningObjectsSystem
{
public:
	MiningObjectsSystem(ComponentManager& component_manager, const EntityManager& entity_manager, World& world, float tile_width, float tile_height)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
		, tile_width(tile_width)
		, tile_height(tile_height)
		, world(world)
	{

	}

	void update(float dt)
	{
		for (auto& entity : entity_manager.getEntities())
		{
			//Check if required components exist
			if (!component_manager.transform.contains(entity) ||
				!component_manager.mine_intent.contains(entity))
				continue;

			auto& ts = component_manager.transform.at(entity);
			auto& mi = component_manager.mine_intent.at(entity);

			float mining_speed;
			float mining_radius;
			float mining_size;
			if (component_manager.pickaxe.contains(entity))
			{
				const auto& pickaxe_component = component_manager.pickaxe.at(entity);
				const auto& item_properties = ItemManager::get().getProperties(pickaxe_component.item->id);
				const auto& pickaxe_data = item_properties.pickaxe_data;

				mining_speed = pickaxe_data->speed;
				mining_radius = pickaxe_data->radius;
				mining_size = pickaxe_data->size;
			}
			else
			{
				mining_speed = BASE_MINING_SPEED;
				mining_radius = BASE_MINING_RADIUS;
				mining_size = BASE_MINING_SIZE;
			}
			

			const auto& mid_position = ts.position + ts.size * 0.5f;
			float distance = glm::distance(mid_position, mi.start_mouse_position);
			bool is_mining = world.getObjectOnPosition(mi.start_mouse_position) && distance < mining_radius && mi.active;

			//Remove start-finish marks
			if (component_manager.mine_objects_started.contains(entity))
			{
				component_manager.mine_objects_started.erase(entity);
			}
			if (component_manager.mine_objects_finished.contains(entity))
			{
				component_manager.mine_objects_finished.erase(entity);
			}

			//Entity was not mining 
			if (!component_manager.mine_objects_state.contains(entity))
			{
				if (is_mining)
				{
					//Do something at the start
					component_manager.mine_objects_state[entity] = MineObjectsState{};
					component_manager.mine_objects_started[entity] = MineObjectsStarted{};
					//std::cout << "Mining_Objects_Started" << std::endl;

					//If entity has physics component - limit horizontal movement
					if (component_manager.physics.contains(entity))
					{
						component_manager.physics.at(entity).can_move_horizontal = false;
					}

				}
			}
			//Entity was mining
			else
			{
				if (!is_mining)
				{
					component_manager.mine_objects_state.erase(entity);
					component_manager.mine_objects_finished[entity] = MineObjectsFinished{};

					//Do something at the end
					//std::cout << "Mining_Objects_Ended" << std::endl;

					//If entity has physics component - return horizontal movement
					if (component_manager.physics.contains(entity))
					{
						component_manager.physics.at(entity).can_move_horizontal = true;
					}
				}
				else
				{
					auto destroyed_object_id = world.damageObject(mi.start_mouse_position, mining_speed * dt);

					//If object was successfully destroyed - add items to the entity's inventory
					if (destroyed_object_id && component_manager.has_inventory.contains(entity))
					{
						const auto& items = ObjectManager::get().getProperties(*destroyed_object_id).drop;
						for (const auto& item : items)
						{
							addRandomizedItem(*component_manager.has_inventory.at(entity).inventory, item);
						}

					}
					// std::cout << "Deal damage\n";
				}
			}
		}
	}

	void render(graphics::Renderer& screen)
	{
		for (auto& entity : entity_manager.getEntities())
		{
			if (component_manager.mine_intent.contains(entity) && component_manager.transform.contains(entity) &&  component_manager.mine_intent.contains(entity))
			{
				auto& ts = component_manager.transform.at(entity);
				auto& mi = component_manager.mine_intent.at(entity);

				float mining_speed;
				float mining_radius;
				float mining_size;
				if (component_manager.pickaxe.contains(entity))
				{
					const auto& pickaxe_component = component_manager.pickaxe.at(entity);
					const auto& item_properties = ItemManager::get().getProperties(pickaxe_component.item->id);
					const auto& pickaxe_data = item_properties.pickaxe_data;

					mining_speed = pickaxe_data->speed;
					mining_radius = pickaxe_data->radius;
					mining_size = pickaxe_data->size;
				}
				else
				{
					mining_speed = BASE_MINING_SPEED;
					mining_radius = BASE_MINING_RADIUS;
					mining_size = BASE_MINING_SIZE;
				}

				if (component_manager.mine_objects_state.contains(entity))
				{
					auto object = world.getObjectOnPosition(mi.start_mouse_position);

					if (object)
					{
						const auto& object_rect = object->rect;
						graphics::drawRectangle(screen, object_rect.x, object_rect.y, object_rect.w, object_rect.h, graphics::RenderType::NONE, graphics::Color::YELLOW);
					}
				}
				else
				{
					auto object = world.getObjectOnPosition(mi.current_mouse_position);

					if (object)
					{
						const auto& mid_position = ts.position + ts.size * 0.5f;
						float distance = glm::distance(mid_position, mi.current_mouse_position);
						const auto& object_rect = object->rect;

						if (distance < mining_radius)
						{

							graphics::drawRectangle(screen, object_rect.x, object_rect.y, object_rect.w, object_rect.h, graphics::RenderType::NONE, graphics::Color::BLUE);
						}
						else
						{
							graphics::drawRectangle(screen, object_rect.x, object_rect.y, object_rect.w, object_rect.h, graphics::RenderType::NONE, graphics::Color::RED);
						}
					}
				}
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

class ItemUsageSystem
{
public:
	ItemUsageSystem(ComponentManager& component_manager, Entity& target_entity)
		: component_manager(component_manager)
		, target_entity(target_entity)
	{
	}

	void useItem(const ItemProperties& item_properties)
	{
		if (item_properties.heal_data)
		{
			auto& health_component = component_manager.health[target_entity];
			health_component.current_health = std::min(health_component.max_health, health_component.current_health + item_properties.heal_data->amount);
		}
	}

	void equipItem(Item* item)
	{
		const auto& item_properties = ItemManager::get().getProperties(item->id);

		if (component_manager.pickaxe.contains(target_entity))
		{
			component_manager.pickaxe.at(target_entity).item->equipped = false;
		}

		if (item_properties.pickaxe_data)
		{
			component_manager.pickaxe[target_entity] = Pickaxe{ item };
		}

		item->equipped = true;
	}

	void unequip(Item* item)
	{
		const auto& item_properties = ItemManager::get().getProperties(item->id);

		if (item_properties.pickaxe_data)
		{
			component_manager.pickaxe.erase(target_entity);
		}

		item->equipped = false;
	}

private:
	ComponentManager& component_manager;
	Entity& target_entity;
};


class ButtonSystem
{
public:
	ButtonSystem(ComponentManager& component_manager, const EntityManager& entity_manager)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
	{
	}

	void update()
	{
		for (const auto& entity : entity_manager.getEntities())
		{
			if (component_manager.transform.contains(entity) && component_manager.button.contains(entity))
			{
				//Remove CraftItem components from the last frame
				if (component_manager.craft_item.contains(entity))
				{
					component_manager.craft_item.erase(entity);
				}

				const auto& ts = component_manager.transform.at(entity);
				auto& bt = component_manager.button.at(entity);

				const auto& mouse_state = InputManager::getMouseState();
				const auto& mouse_position = mouse_state.position;
				const auto& mouse_left_state = mouse_state.left;

				bool is_covered = isMouseIntersection(mouse_position, SDL_FRect{ ts.position.x, ts.position.y, ts.size.x, ts.size.y });
				bool was_covered = component_manager.button_covered.contains(entity);

				//If button is not being held right now
				if (!component_manager.button_held.contains(entity))
				{
					//If cursor is on the button now but wasn't last frame - ENTER
					if (is_covered && !was_covered)
					{
						component_manager.button_entered[entity] = ButtonEntered{};
					}
					//If cursor is on the button and was last frame - STAY/COVERED
					else if (is_covered && was_covered)
					{
						component_manager.button_covered[entity] = ButtonCovered{};

						if (mouse_left_state == MouseButtonState::DOWN)
						{
							component_manager.button_held[entity] = ButtonHeld{};
						}
					}
					//If cursor is not on the mouse but was last frame - EXIT
					else if (!is_covered && was_covered)
					{
						component_manager.button_exit[entity] = ButtonExit{};
					}
				}
				//If button is being held
				else
				{
					//If cursor is no longer on the button - Remove ButtonHeld component
					if (!is_covered)
					{
						component_manager.button_held.erase(entity);
					}
					//Or if left mouse button was released - Remove ButtonHeld component and call a function assigned to the button
					else if (mouse_left_state == MouseButtonState::RELEASED)
					{
						component_manager.button_held.erase(entity);

						//Do func()
						press(entity);
					}
				}

				if (is_covered && !was_covered)
				{
					component_manager.button_covered[entity] = ButtonCovered{};
				}
				else if (!is_covered && was_covered)
				{
					component_manager.button_covered.erase(entity);
				}
			}
		}
	}

private:
	void press(Entity button)
	{
		if (component_manager.craft_button.contains(button))
		{
			const auto& craft_component = component_manager.craft_button.at(button);

			if (craft_component.is_available)
			{
				component_manager.craft_item[button] = CraftItem{ craft_component.recipe_id };
			}
		}
	}

	ComponentManager& component_manager;
	const EntityManager& entity_manager;
};


class CraftSystem
{
public:
	CraftSystem(ComponentManager& component_manager, const EntityManager& entity_manager)
		: component_manager(component_manager)
		, entity_manager(entity_manager)
	{
	}

	void update(Entity target_entity)
	{
		for (const auto& entity : entity_manager.getEntities())
		{
			if (!component_manager.craft_item.contains(entity) || !component_manager.has_inventory.contains(target_entity)) continue;

			auto& inventory_component = component_manager.has_inventory.at(target_entity);
			auto& craft_item_component = component_manager.craft_item.at(entity);

			auto& inventory = inventory_component.inventory;

			const auto& recipe = CraftingManager::get().getRecipe(craft_item_component.recipe_id);

			bool can_craft = true;
			for (const auto& required_craft_item : recipe.required_items)
			{
				if (!inventory->hasItem(required_craft_item))
				{
					can_craft = false;
					break;
				}
			}

			if (can_craft)
			{
				inventory->addItem(recipe.item_id, 1);
				
				for (const auto& required_craft_item : recipe.required_items)
				{
					inventory->removeItem(required_craft_item);
				}
			}
		}
	}

private:
	ComponentManager& component_manager;
	const EntityManager& entity_manager;
};