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
#include "WorldHelper.hpp"
#include "glm/gtc/random.hpp"

constexpr float BASE_MINING_SPEED = 100.0f;
constexpr float BASE_MINING_RADIUS = 100.0f;
constexpr int BASE_MINING_SIZE = 2;

static Entity getRandomizedItem(entt::registry& registry, const RandomizedItem& random_item)
{
	if (random_item.drop_chance == 0.0f) return entt::null;

	float drop_rand = glm::linearRand(0.0f, 1.0f);
	int quantities_rand = glm::linearRand(random_item.drop_quantity_min, random_item.drop_quantity_max);
	
	if (drop_rand <= random_item.drop_chance)
	{
		auto item = ItemManager::get().createItem(registry, random_item.item_id, quantities_rand);
		return item;
	}
	else
	{
		return entt::null;
	}
}

template<typename T>
std::vector<Entity> getEffects(entt::registry& registry, Entity target)
{
	std::vector<Entity> effects;
	std::set<Entity> found_entities;
	auto view = registry.view<Components::WeaponEffects::Effect, T>();
	for (auto [entity, effect_component, weapon_effect_component] : view.each())
	{
		if (effect_component.target == target && !found_entities.contains(effect_component.source))
		{
			effects.push_back(entity);
			found_entities.emplace(effect_component.source);
		}
	}
	return effects;
}

template<typename T>
bool isEffectApplied(entt::registry& registry, Entity target)
{
	int i = 0;
	auto view = registry.view<Components::WeaponEffects::Effect, T>();
	for (auto [entity, effect] : view.each())
	{
		if (effect.target == target) ++i;
	}
	return i != 0;
}

// Get stacked WeaponEffect value
// Specified effect has to have a variable called "value"
/*
template<typename T>
T getEffectValue(entt::registry& registry, Entity target)
{
	T total;
	std::set<Entity> found_sources;

	auto view = registry.view<Components::WeaponEffects::Effect, T>();
	for (auto [entity, effect_component, weapon_effect_component] : view.each())
	{
		if (effect_component.target == target)
			if (!found_sources.contains(effect_component.source))
			{
				total += weapon_effect_component;
				found_sources.emplace(effect_component.source);
			}
	}
	return total;
}
*/

/// <summary>
/// Static function that takes an item and randomly decides whether add the item to the given target.
/// Quantities are also generated randomly based on the variables defined in <b>RandomizedItem</b> class.
/// </summary>
/// <param name="target">Takes an target that will get an item.</param>
/// <param name="item">Item that is being added to the inventory</param>
static void addRandomizedItem(Entity entity, const RandomizedItem& item, entt::registry& registry)
{
	auto crafted_item = getRandomizedItem(registry, item);
	if (crafted_item != entt::null)
	{
		auto item_entity = registry.create();
		registry.emplace_or_replace<Components::AddItem>(item_entity, entity, crafted_item);
	}
}

/// <summary>
/// Checks if a mouse cursor is inside a bounding box.
/// </summary>
/// <param name="mouse_position">Position of a cursor,</param>
/// <param name="grid_rect">Bounding box that we are checking.</param>
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
static bool AABB(const Components::Transform& a, const Components::Transform& b)
{
	return a.position.x <= b.position.x + b.size.x &&
		a.position.x + a.size.x >= b.position.x &&
		a.position.y <= b.position.y + b.size.y &&
		a.position.y + a.size.y >= b.position.y;
}
static bool AABB(const SDL_FRect& a, const SDL_FRect& b)
{
	return a.x <= b.x + b.w &&
		a.x + a.w >= b.x &&
		a.y <= b.y + b.h &&
		a.y + a.h >= b.y;
}

struct PhysicsSystem
{
	PhysicsSystem(entt::registry& registry) : registry{registry} {}

	void update(float dt)
	{
		auto view = registry.view<Components::Transform, Components::Physics>();
		for (auto [entity, ts, ph] : view.each())
		{
			//Add gravity
			float gravity = ph.gravity;
			ph.velocity.y += gravity * dt;

			ts.position.y += ph.velocity.y * dt;

			if (ph.can_move_horizontal)
			{
				ts.position.x += ph.velocity.x * dt;
			}

			ph.velocity.x -= ph.velocity.x * ph.decelaration * dt;

		}
	}

private:
	entt::registry& registry;
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

class TileCollisionSystem
{
public:
	TileCollisionSystem(entt::registry& registry, Grid<Tile>& grid) : registry{ registry }, grid{ grid } {}


	void update(float dt)
	{
		auto view = registry.view<Components::Transform, Components::Physics>();
		for (auto [entity, ts, ph] : view.each())
		{
			ph.is_ground = false;

			glm::vec2 mid_position = ts.position + ts.size * 0.5f;
			int grid_local_x = static_cast<int>(mid_position.x / 20.0f);
			int grid_local_y = static_cast<int>(mid_position.y / 20.0f);

			for (int i = -load_size; i < load_size; ++i)
			{
				for (int j = -load_size; j < load_size; ++j)
				{
					int new_x = grid_local_x + i;
					int new_y = grid_local_y + j;
					
					if (new_x < 0 || new_x >= grid.getColumns() || new_y < 0 || new_y >= grid.getRows()) continue;

					const auto& tile = grid(new_x, new_y);
					const auto& properties = TileManager::get().getProperties(tile.id);

					if (properties.is_solid)
					{
						SDL_FRect rect;
						rect.x = new_x * 20.0f;
						rect.y = new_y * 20.0f;
						rect.w = 20.0f;
						rect.h = 20.0f;

						resolveCollision(entity, ts, ph, rect);
					}
					
				}
			}
		}
	}

	//std::vector<Transform> collisions;
private:
	entt::registry& registry;
	int load_size = 3;
	Grid<Tile>& grid;

	void resolveCollision(Entity entity, Components::Transform& ts, Components::Physics& ph, const SDL_FRect& collider_rect) const
	{
		SDL_FRect rect = {ts.position.x, ts.position.y, ts.size.x, ts.size.y};
		if (AABB(rect, collider_rect))
		{
			glm::vec2 p_min = { ts.position.x, ts.position.y };
			glm::vec2 p_max = { ts.position.x + ts.size.x, ts.position.y + ts.size.y };

			glm::vec2 c_min = { collider_rect.x, collider_rect.y };
			glm::vec2 c_max = { collider_rect.x + collider_rect.w, collider_rect.y + collider_rect.h };

			float x_overlap = glm::min(p_max.x, c_max.x) - glm::max(p_min.x, c_min.x);
			float y_overlap = glm::min(p_max.y, c_max.y) - glm::max(p_min.y, c_min.y);

			glm::vec2 normal = { 0.f, 0.f };

			//Chec x collision first
			if (x_overlap < y_overlap)
			{
				//If collider is on the right side
				if (p_min.x < c_min.x)
				{
					if (registry.all_of<Components::PhysicStep>(entity))
					{
						const auto& step_component = registry.get<Components::PhysicStep>(entity);
						SDL_FRect step = { ts.position.x + ts.size.x, ts.position.y + ts.size.y - step_component.max_step_height, step_component.max_step_height, step_component.max_step_height };

						if (isSpaceAbove(step, ts.size.y) && ph.velocity.x > 0.f)
						{
							ts.position.y -= step_component.max_step_height;
							ts.position.x += 1.f;
						}
						else
						{
							normal.x = -1.f;
							ph.velocity.x = 0.f;
						}
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
					if (registry.all_of<Components::PhysicStep>(entity))
					{
						const auto& step_component = registry.get<Components::PhysicStep>(entity);
						SDL_FRect step = { ts.position.x - step_component.max_step_height, ts.position.y + ts.size.y - step_component.max_step_height, step_component.max_step_height, step_component.max_step_height};
						if (isSpaceAbove(step, ts.size.y) && ph.velocity.x < 0.f)
						{
							ts.position.y -= step_component.max_step_height;
							ts.position.x -= 1.f;
						}
						else
						{
							normal.x = 1.0f;
							ph.velocity.x = 0.0f;
						}
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

	bool isSpaceAbove(const SDL_FRect& step, float height) const
	{
		Components::Transform above = { {step.x + 1.f, step.y - height + 1.f}, {step.w - 2.f, height - 2.f} };
		SDL_FRect above_rect = { step.x + 1.f, step.y - height + 1.f, step.w - 2.f, height - 2.f };

		glm::vec2 mid_position = glm::vec2{ above_rect.x, above_rect.y } + glm::vec2{ above_rect.w, above_rect.h } * 0.5f;
		int grid_local_x = static_cast<int>(mid_position.x / 20.0f);
		int grid_local_y = static_cast<int>(mid_position.y / 20.0f);

		for (int i = -load_size; i < load_size; ++i)
		{
			for (int j = -load_size; j < load_size; ++j)
			{
				int new_x = grid_local_x + i;
				int new_y = grid_local_y + j;

				if (new_x < 0 || new_x >= grid.getColumns() || new_y < 0 || new_y >= grid.getRows()) continue;

				const auto& tile = grid(new_x, new_y);
				const auto& properties = TileManager::get().getProperties(tile.id);

				if (properties.is_solid)
				{
					SDL_FRect rect;
					rect.x = new_x * 20.0f;
					rect.y = new_y * 20.0f;
					rect.w = 20.0f;
					rect.h = 20.0f;

					if (AABB(rect, above_rect))
					{
						return false;
					}
				}

			}
		}
		return true;
	}
};

struct InputSystem
{
	InputSystem(entt::registry& registry) : registry{registry} {}

	void update(const graphics::Renderer& screen, float dt)
	{
		//Calculate global mouse position
		glm::vec2 mouse_global_position = graphics::getMouseGlobalPosition(screen, InputManager::getMouseState().position);
		
		auto view = registry.view<Components::Player>();

		for (auto [entity] : view.each())	
		{
			// Jump
			if (registry.all_of<Components::Jump>(entity))
			{
				auto& j = registry.get<Components::Jump>(entity);
				j.jump_pressed_this_frame = InputManager::isKeyDown(SDL_SCANCODE_U);
				j.jump_held = InputManager::isKey(SDL_SCANCODE_U);
			}

			//Mining
			if (registry.all_of<Components::MineIntent>(entity))
			{
				auto& mi = registry.get<Components::MineIntent>(entity);
				//mi.active = InputManager::getMouseState().left == MouseButtonState::HELD;

				if (InputManager::getMouseState().left == MouseButtonState::DOWN)
				{
					mi.active = true;
					mi.start_mouse_position = mouse_global_position;
				}
				if (InputManager::getMouseState().left == MouseButtonState::RELEASED || registry.all_of<Components::MineObjectsFinished>(entity))
				{
					mi.active = false;
				}

				mi.current_mouse_position = mouse_global_position;
			}

			//Block placement
			if (registry.all_of<Components::PlaceIntent>(entity))
			{
				auto& pi = registry.get<Components::PlaceIntent>(entity);
				pi.active = InputManager::getMouseState().right == MouseButtonState::HELD;
				pi.target_global_position = mouse_global_position;
			}

			//Horizontal movement
			if (registry.all_of<Components::Physics>(entity))
			{
				auto& ph = registry.get<Components::Physics>(entity);
				if (InputManager::isKey(SDL_SCANCODE_H))
				{
					ph.velocity.x -= ph.acceleration.x * dt;
					ph.velocity.x = std::clamp(ph.velocity.x, -ph.max_velocity.x, ph.max_velocity.x);

					//Flip
					if (registry.all_of<Components::Renderable>(entity))
						registry.get<Components::Renderable>(entity).flip_mode = SDL_FLIP_HORIZONTAL;
				}
				if (InputManager::isKey(SDL_SCANCODE_K))
				{
					ph.velocity.x += ph.acceleration.x * dt;
					ph.velocity.x = std::clamp(ph.velocity.x, -ph.max_velocity.x, ph.max_velocity.x);

					//Flip
					if (registry.all_of<Components::Renderable>(entity))
						registry.get<Components::Renderable>(entity).flip_mode = SDL_FLIP_NONE;
				}
				if (!InputManager::isKey(SDL_SCANCODE_K) && !InputManager::isKey(SDL_SCANCODE_H))
				{
					/*ph.velocity.x -= ph.velocity.x * ph.decelaration * dt;*/
				}

			}
		}
	}

private:
	entt::registry& registry;
};

struct JumpSystem
{
	JumpSystem(entt::registry& registry) : registry(registry) {}

	void update(float dt)
	{
		//Jump for common entities
		auto view = registry.view<Components::Physics, Components::Jump>();
		for (auto [entity, ph, j] : view.each())
		{
			if (ph.is_ground)
			{
				if (j.jump_held)
				{
					//Jump
					ph.velocity.y = -j.jump_force;
					j.jump_held = false;
					ph.is_ground = false;
				}
			}
			else if (j.jump_count - 1 >= 1)
			{
				if (j.jump_pressed_this_frame)
				{
					//Jump
					ph.velocity.y = -j.jump_force;
					j.jump_pressed_this_frame = false;
					ph.is_ground = false;
					j.jump_count--;
				}
			}
			  
			if (ph.is_ground) j.jump_count = j.max_jump_count;
		}
	}

private:
	entt::registry& registry;
};

class MiningTilesSystem
{
public:
	MiningTilesSystem(entt::registry& registry, Grid<Tile>& grid, float tile_width, float tile_height)
		: tile_width(tile_width)
		, tile_height(tile_height)
		, grid{grid}
		, registry(registry)
	{

	}

	void update(float dt)
	{
		auto view = registry.view<Components::Transform, Components::MineIntent, Components::MiningAbility>();
		for (auto [entity, ts, mi, mining_ability_component] : view.each())
		{
			if (registry.all_of<Components::MineObjectsState>(entity)) continue;

			tiles_covered.clear();

			float mining_speed = mining_ability_component.speed;
			float mining_radius = mining_ability_component.radius;
			float mining_size = mining_ability_component.current_size;
			//std::cout << mining_size << std::endl;

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
						damageTile(grid, tile_covered.x, tile_covered.y, mining_speed * dt); 
			}
		}
	}

	void renderOutline(graphics::Renderer& screen) const
	{
		auto view = registry.view<Components::Transform, Components::MineIntent, Components::MiningAbility>();
		for (auto [entity, ts, mi, mining_ability_component] : view.each())
		{
			if (registry.all_of<Components::MineObjectsState>(entity)) continue;
			float mining_radius = mining_ability_component.radius;

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
	entt::registry& registry;

	Grid<Tile>& grid;

	float tile_width = 1.f;
	float tile_height = 1.f;

	std::vector<glm::ivec2> tiles_covered;
};

class PlaceSystem
{
public:
	PlaceSystem(entt::registry& registry, Grid<Tile>& grid, float tile_width, float tile_height)
		: tile_width(tile_width)
		, tile_height(tile_height)
		, grid(grid)
		, registry(registry)
	{
	}

	void update(float dt)
	{
		auto view = registry.view<Components::Transform, Components::PlaceAbility, Components::PlaceIntent>();
		for (auto [entity, ts, pl, pi] : view.each())
		{
			pl.placing_timer += dt;

			if (!pi.active || pl.placing_timer < pl.placing_time) continue;

			pl.placing_timer = 0.f;

			glm::vec2 mid_position = ts.position + ts.size * 0.5f;
			const auto& mouse_global_position = pi.target_global_position;

			float distance = glm::distance(mid_position, mouse_global_position);

			if (distance > pl.radius) continue;
			if (getObjectOnPosition(registry, mouse_global_position) != entt::null) continue;

			//Prevents from placing blocks on the player 
			float min_distance = glm::distance(mid_position, ts.position);
			if (distance < min_distance) continue;

			int tile_x = static_cast<int>(std::floor((mouse_global_position.x) / tile_width));
			int tile_y = static_cast<int>(std::floor((mouse_global_position.y) / tile_height));

			//TODO Temporary placed 0 instead of a tile id
			placeTile(grid, tile_x, tile_y, 0);
		}
	}

private:
	entt::registry& registry;
	Grid<Tile>& grid;
	float tile_width = 1.f;
	float tile_height = 1.f;
};

class MiningObjectsSystem
{
public:
	MiningObjectsSystem(entt::registry& registry, Grid<Tile>& grid, float tile_width, float tile_height)
		: tile_width(tile_width)
		, tile_height(tile_height)
		, grid(grid)
		, registry(registry)
	{

	}

	void update(float dt)
	{
		auto view = registry.view<Components::Transform, Components::MineIntent, Components::MiningAbility>();
		for (auto [entity, ts, mi, mining_ability_component] : view.each())
		{
			float mining_speed = mining_ability_component.speed;
			float mining_radius = mining_ability_component.radius;
			float mining_size = mining_ability_component.current_size;

			const auto& mid_position = ts.position + ts.size * 0.5f;
			float distance = glm::distance(mid_position, mi.start_mouse_position);
			bool is_mining = getObjectOnPosition(registry, mi.start_mouse_position) != entt::null && distance < mining_radius && mi.active;

			//Remove start-finish marks
			registry.remove<Components::MineObjectsStarted>(entity);
			registry.remove<Components::MineObjectsFinished>(entity);

			//Entity was not mining 
			if (!registry.all_of<Components::MineObjectsState>(entity))
			{
				if (is_mining)
				{
					//Do something at the start
					registry.emplace<Components::MineObjectsState>(entity);
					registry.emplace<Components::MineObjectsStarted>(entity);
					//std::cout << "Mining_Objects_Started" << std::endl;

					//If target has physics component - limit horizontal movement
					if (registry.all_of<Components::Physics>(entity))
						registry.get<Components::Physics>(entity).can_move_horizontal = false;

				}
			}
			//Entity was mining
			else
			{
				if (!is_mining)
				{
					registry.erase<Components::MineObjectsState>(entity);
					registry.emplace<Components::MineObjectsFinished>(entity);

					//Do something at the end

					//If target has physics component - return horizontal movement
					if (registry.all_of<Components::Physics>(entity))
						registry.get<Components::Physics>(entity).can_move_horizontal = true;
				}
				else
				{
					auto destroyed_object = damageObject(registry, mi.start_mouse_position, mining_speed * dt);

					//If object was successfully destroyed - add items to the target's inventory
					if (destroyed_object && registry.all_of<Components::HasInventory>(entity))
					{
						const auto& items = ObjectManager::get().getProperties(destroyed_object->id).drop;
						for (const auto& item : items)
						{
							addRandomizedItem(entity, item, registry);
						}

					}
				}
			}
		}
	}

	void render(graphics::Renderer& screen)
	{
		auto view = registry.view<Components::Transform, Components::MineIntent>();
		for (auto [entity, ts, mi] : view.each())
		{
			float mining_speed;
			float mining_radius;
			float mining_size;
			if (registry.all_of<Components::Equipment>(entity) && registry.get<Components::Equipment>(entity).pickaxe != entt::null)
			{
				auto& equipment_component = registry.get<Components::Equipment>(entity);
				const auto& pickaxe_component = registry.get<Components::InventoryItems::PickaxeComponent>(equipment_component.pickaxe);
		
				mining_speed = pickaxe_component.speed;
				mining_radius = pickaxe_component.radius;
				mining_size = pickaxe_component.size;
			}
			else
			{
				mining_speed = BASE_MINING_SPEED;
				mining_radius = BASE_MINING_RADIUS;
				mining_size = BASE_MINING_SIZE;
			}

			if (registry.all_of<Components::MineObjectsState>(entity))
			{
				Entity object = getObjectOnPosition(registry, mi.start_mouse_position);

				if (object != entt::null)
				{
					const auto& object_transform = registry.get<Components::Transform>(object);
					graphics::drawRectangle(screen, object_transform.position.x, object_transform.position.y, object_transform.size.x, object_transform.size.y, graphics::RenderType::NONE, graphics::Color::YELLOW);
				}
			}
			else
			{
				Entity object = getObjectOnPosition(registry, mi.current_mouse_position);

				if (object != entt::null)
				{
					const auto& mid_position = ts.position + ts.size * 0.5f;
					float distance = glm::distance(mid_position, mi.current_mouse_position);
					const auto& object_transform = registry.get<Components::Transform>(object);

					if (distance < mining_radius)
					{
						graphics::drawRectangle(screen, object_transform.position.x, object_transform.position.y, object_transform.size.x, object_transform.size.y, graphics::RenderType::NONE, graphics::Color::BLUE);
					}
					else
					{
						graphics::drawRectangle(screen, object_transform.position.x, object_transform.position.y, object_transform.size.x, object_transform.size.y, graphics::RenderType::NONE, graphics::Color::RED);
					}
				}
			}
		}
	}
private:
	entt::registry& registry;
	Grid<Tile>& grid;

	float tile_width = 1.f;
	float tile_height = 1.f;
};

//Do it before this system, so that EquipItem Component is right of other systems as well
//Note: I just need to send another event ex. ItemEquipped(Item) to notify my WeaponCircleSystem works correctly
class ItemUsageSystem
{
public:
	ItemUsageSystem(entt::registry& registry)
		: registry(registry)
	{
	}

	void update()
	{
		//Remove Item Equip/Unequip components after one frame
		std::vector<Entity> to_destroy;

		// Delete all tag components after one frame
		{
			for (auto [entity, item_equipped_component] : registry.view<Components::ItemEquipped>().each()) to_destroy.push_back(entity);
			for (auto [entity, item_equipped_component] : registry.view<Components::ItemUnequipped>().each()) to_destroy.push_back(entity);
		}

		//--------------------------------//
		//......EQUIP............ITEM.....//
		//--------------------------------//
		auto view = registry.view<Components::EquipItem>();
		for (auto [entity, equip_item_component] : view.each())
		{
			Entity target = equip_item_component.target;

			if (!registry.all_of<Components::Equipment>(target)) continue;

			auto& equipment_component = registry.get<Components::Equipment>(target);
			bool was_equipped = false;

			// Pickaxe 
			if (registry.all_of<Components::InventoryItems::PickaxeComponent>(equip_item_component.item))
			{
				if (equipment_component.pickaxe != entt::null)
				{
					registry.get<Components::InventoryItems::Item>(equipment_component.pickaxe).equipped = false;
				}

				equipment_component.pickaxe = equip_item_component.item;
				auto& item_info = registry.get<Components::InventoryItems::Item>(equip_item_component.item);
				item_info.equipped = true;

				// Set mining properties (speed, radius, size)
				if (registry.all_of<Components::MiningAbility>(target))
				{
					const auto& pickaxe_component = registry.get<Components::InventoryItems::PickaxeComponent>(equip_item_component.item);
					auto& mining_ability = registry.get<Components::MiningAbility>(target);
					mining_ability.speed = pickaxe_component.speed;
					mining_ability.radius = pickaxe_component.radius;
					mining_ability.max_size = pickaxe_component.size;
					mining_ability.current_size = std::clamp(mining_ability.current_size, 1, mining_ability.max_size);
				}
				was_equipped = true;
			}

			// Melee weapon
			if (registry.all_of<Components::InventoryItems::WeaponComponent>(equip_item_component.item))
			{
				if (equipment_component.weapons.size() < equipment_component.max_weapon)
				{
					registry.get<Components::Equipment>(target).weapons.emplace_back(equip_item_component.item);
					registry.get<Components::InventoryItems::Item>(equip_item_component.item).equipped = true;
					was_equipped = true;
				}
			}

			//Helmet
			if (registry.all_of<Components::InventoryItems::Helmet>(equip_item_component.item))
			{
				if (equipment_component.helmet != entt::null)
				{
					registry.get<Components::InventoryItems::Item>(equipment_component.helmet).equipped = false;
					unequipItem(equipment_component.helmet, target);
				}

				equipment_component.helmet = equip_item_component.item;
				auto& item_info = registry.get<Components::InventoryItems::Item>(equip_item_component.item);
				item_info.equipped = true;
				was_equipped = true;
			}

			//Armor
			if (registry.all_of<Components::InventoryItems::Armor>(equip_item_component.item))
			{
				if (equipment_component.armor != entt::null)
				{
					registry.get<Components::InventoryItems::Item>(equipment_component.armor).equipped = false;
					unequipItem(equipment_component.armor, target);
				}

				equipment_component.armor = equip_item_component.item;
				auto& item_info = registry.get<Components::InventoryItems::Item>(equip_item_component.item);
				item_info.equipped = true;
				was_equipped = true;
			}

			//Boots
			if (registry.all_of<Components::InventoryItems::Boots>(equip_item_component.item))
			{
				if (equipment_component.boots != entt::null)
				{
					registry.get<Components::InventoryItems::Item>(equipment_component.boots).equipped = false;
					unequipItem(equipment_component.boots, target);

				}

				equipment_component.boots = equip_item_component.item;
				auto& item_info = registry.get<Components::InventoryItems::Item>(equip_item_component.item);
				item_info.equipped = true;
				was_equipped = true;
			}

			//Accessory
			if (registry.all_of<Components::InventoryItems::Accessory>(equip_item_component.item))
			{
				if (equipment_component.accessories.size() < equipment_component.max_accessories)
				{
					equipment_component.accessories.push_back(equip_item_component.item);
					was_equipped = true;
				}
			}

			if (was_equipped)
			{
				auto item_equipped_entity = registry.create();
				registry.emplace<Components::ItemEquipped>(item_equipped_entity, equip_item_component.item, target);
			}
			to_destroy.push_back(entity);
		
		}
		//--------------------------------//
		//....UNEQUIP............ITEM.....//
		//--------------------------------//
		auto view1 = registry.view<Components::UnequipItem>();
		for (auto [entity, unequip_item_component] : view1.each())
		{
			Entity target = unequip_item_component.target;

			if (!registry.all_of<Components::Equipment>(target)) continue;

			auto& equipment_component = registry.get<Components::Equipment>(target);
			bool was_unequipped = false;
			
			if (registry.all_of<Components::InventoryItems::PickaxeComponent>(unequip_item_component.item))
			{
				equipment_component.pickaxe = entt::null;
				registry.get<Components::InventoryItems::Item>(unequip_item_component.item).equipped = false;
				was_unequipped = true;

				// Set mining properties (speed, radius, size)
				if (registry.all_of<Components::MiningAbility>(target))
				{
					auto& mining_ability = registry.get<Components::MiningAbility>(target);
					mining_ability.speed = BASE_MINING_SPEED;
					mining_ability.radius = BASE_MINING_RADIUS;
					mining_ability.max_size = BASE_MINING_SIZE;
					mining_ability.current_size = std::clamp(mining_ability.current_size, 1, mining_ability.max_size);
				}
			}

			// Melee weapon
			if (registry.all_of<Components::InventoryItems::WeaponComponent>(unequip_item_component.item))
			{
				auto& weapon_array = registry.get<Components::Equipment>(target).weapons;
				weapon_array.erase(std::ranges::remove(weapon_array, unequip_item_component.item).begin(), weapon_array.end());
				registry.get<Components::InventoryItems::Item>(unequip_item_component.item).equipped = false;
				was_unequipped = true;
			}


			//Helmet
			if (registry.all_of<Components::InventoryItems::Helmet>(unequip_item_component.item))
			{
				equipment_component.helmet = entt::null;
				registry.get<Components::InventoryItems::Item>(unequip_item_component.item).equipped = false;
				was_unequipped = true;
			}

			//Armor
			if (registry.all_of<Components::InventoryItems::Armor>(unequip_item_component.item))
			{
				equipment_component.armor = entt::null;
				registry.get<Components::InventoryItems::Item>(unequip_item_component.item).equipped = false;
				was_unequipped = true;
			}

			//Boots
			if (registry.all_of<Components::InventoryItems::Boots>(unequip_item_component.item))
			{
				equipment_component.boots = entt::null;
				registry.get<Components::InventoryItems::Item>(unequip_item_component.item).equipped = false;
				was_unequipped = true;
			}

			if (was_unequipped)
			{
				auto item_unequipped = registry.create();
				registry.emplace<Components::ItemUnequipped>(item_unequipped, unequip_item_component.item, target);
			}
			to_destroy.push_back(entity);
		}
		//--------------------------------//
		//......USE..............ITEM.....//
		//--------------------------------//
		auto view2 = registry.view<Components::UseItem>();
		for (auto [entity, use_item_component] : view2.each())
		{
			Entity target = use_item_component.target;

			const auto& item_properties = ItemManager::get().getProperties(use_item_component.item_id);
			const auto& item = ItemManager::get().getItem(use_item_component.item_id);

			if (registry.all_of<Components::InventoryItems::HealComponent>(item))
			{
				const auto& heal_component = registry.get<Components::InventoryItems::HealComponent>(item);
				auto& health_component = registry.get<Components::Health>(target);
				health_component.current_health = std::min(health_component.max_health, health_component.current_health + heal_component.value);
			}

			to_destroy.push_back(entity);
		}

		for (const auto& entity : to_destroy)
		{
			registry.destroy(entity);
		}
	}


private:
	void unequipItem(Entity item, Entity target)
	{
		auto entity = registry.create();
		registry.emplace<Components::UnequipItem>(entity, item, target);
	}
	entt::registry& registry;
};


class ButtonSystem
{
public:
	ButtonSystem(entt::registry& registry) : registry{registry} {}

	void update(const graphics::Renderer& screen)
	{
		auto view = registry.view<Components::Transform, Components::Button>();
		for (auto [entity, ts, button] : view.each())
		{
			//Remove CraftItem components from the last frame
			registry.remove<Components::CraftItem>(entity);
			registry.remove<Components::ButtonReleased>(entity);

			const auto& mouse_state = InputManager::getMouseState();
			const auto& mouse_position = button.global ? graphics::getMouseGlobalPosition(screen, mouse_state.position) : mouse_state.position;
			const auto& mouse_left_state = mouse_state.left;

			bool is_covered = isMouseIntersection(mouse_position, SDL_FRect{ ts.position.x, ts.position.y, ts.size.x, ts.size.y });
			bool was_covered = registry.all_of<Components::ButtonCovered>(entity);

			//If button is not being held right now
			if (!registry.all_of<Components::ButtonHeld>(entity))
			{
				//If cursor is on the button now but wasn't last frame - ENTER
				if (is_covered && !was_covered)
				{
					registry.emplace_or_replace<Components::ButtonEntered>(entity);
				}
				//If cursor is on the button and was last frame - STAY/COVERED
				else if (is_covered && was_covered)
				{
					registry.emplace_or_replace<Components::ButtonCovered>(entity);

					if (mouse_left_state == MouseButtonState::DOWN)
					{
						registry.emplace_or_replace<Components::ButtonHeld>(entity);
					}
				}
				//If cursor is not on the mouse but was last frame - EXIT
				else if (!is_covered && was_covered)
				{
					registry.emplace_or_replace<Components::ButtonExit>(entity);
				}
			}
			//If button is being held
			else
			{
				//If cursor is no longer on the button - Remove ButtonHeld component
				if (!is_covered)
				{
					registry.erase<Components::ButtonHeld>(entity);
				}
				//Or if left mouse button was released - Remove ButtonHeld component and call a function assigned to the button
				else if (mouse_left_state == MouseButtonState::RELEASED)
				{
					registry.erase<Components::ButtonHeld>(entity);

					registry.emplace<Components::ButtonReleased>(entity);
				}
			}

			if (is_covered && !was_covered)
			{
				registry.emplace_or_replace<Components::ButtonCovered>(entity);
			}
			else if (!is_covered && was_covered)
			{
				registry.erase<Components::ButtonCovered>(entity);
			}
		}
	}

private:
	entt::registry& registry;
};


class CraftSystem
{
public:
	CraftSystem(entt::registry& registry) : registry{registry} {}

	void update(Entity target_entity)
	{
		if (!registry.all_of<Components::HasInventory>(target_entity)) return;

		auto& inventory_component = registry.get<Components::HasInventory>(target_entity);
		auto view = registry.view<Components::CraftItem>();
		for (auto [entity, craft_item_component] : view.each())
		{
			//NOTE: CraftItem is a component that is attached to a CraftButton when user tries to craft an item.
			//And the target we have to give an item is our target_entity

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
				/*
				inventory->addItem(recipe.item_id, 1);
				*/

				auto item_entity = registry.create();
				registry.emplace<Components::AddItem>(item_entity, target_entity, ItemManager::get().createItem( registry,recipe.item_id, 1 ));
				
				for (const auto& required_craft_item : recipe.required_items)
				{
					inventory->removeItem(required_craft_item);
				}
			}
		}
	}
	
private:
	entt::registry& registry;
};