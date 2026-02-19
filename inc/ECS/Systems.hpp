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

//TODO: Change AddItem in ComponentManager to store vector of components in order to add multiple items in one frame
/// <summary>
/// Static function that takes an item and randomly decides whether add the item to the given entity.
/// Quantities are also generated randomly based on the variables defined in <b>RandomizedItem</b> class.
/// </summary>
/// <param name="entity">Takes an entity that will get an item.</param>
/// <param name="item">Item that is being added to the inventory</param>
static void addRandomizedItem(Entity entity, const RandomizedItem& item)
{
	if (item.drop_chance == 0.0f) return;

	float drop_rand = glm::linearRand(0.0f, 1.0f);
	int quantities_rand = glm::linearRand(item.drop_quantity_min, item.drop_quantity_max);

	if (drop_rand <= item.drop_chance)
	{
		/*
		inventory.addItem(item.item_id, quantities_rand);
	*/
		ComponentManager::get().add_item[entity] = AddItem{ Item{item.item_id, quantities_rand} };
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
static bool AABB(const SDL_FRect& a, const SDL_FRect& b)
{
	return a.x <= b.x + b.w &&
		a.x + a.w >= b.x &&
		a.y <= b.y + b.h &&
		a.y + a.h >= b.y;
}

struct PhysicsSystem
{
	PhysicsSystem() = default;

	void update(float dt)
	{
		for (auto& entity : EntityManager::get().getEntities())
		{
			if (ComponentManager::get().transform.contains(entity) && ComponentManager::get().physics.contains(entity))
			{
				auto& ts = ComponentManager::get().transform.at(entity);
				auto& ph = ComponentManager::get().physics.at(entity);

				//Add gravity
				float gravity = 1400.f;
				ph.velocity.y += gravity * dt;

				ts.position.y += ph.velocity.y * dt;

				if (ph.can_move_horizontal) ts.position.x += ph.velocity.x * dt;

				if (ph.is_ground)
				{
					ph.velocity.x -= ph.velocity.x * ph.decelaration * dt;
				}

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

class TileCollisionSystem
{
public:
	TileCollisionSystem(std::shared_ptr<World> world) : world(std::move(world)) {}

	void update(float dt)
	{
		const auto& grid = world->getGrid();
		for (auto& entity : EntityManager::get().getEntities())
		{
			if (ComponentManager::get().transform.contains(entity) && ComponentManager::get().physics.contains(entity))
			{
				auto& ts = ComponentManager::get().transform.at(entity);
				auto& ph = ComponentManager::get().physics.at(entity);

				ph.is_ground = false;

				int grid_local_x = static_cast<int>(ts.position.x / 20.0f);
				int grid_local_y = static_cast<int>(ts.position.y / 20.0f);

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
	}

	//std::vector<Transform> collisions;
private:
	int load_size = 5;
	std::shared_ptr<World> world;

	void resolveCollision(Entity entity, Transform& ts, Physics& ph, const SDL_FRect& collider_rect) const
	{
		auto& component_manager = ComponentManager::get();
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
					if (component_manager.physic_step.contains(entity))
					{
						const auto& step_component = component_manager.physic_step.at(entity);
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
					if (component_manager.physic_step.contains(entity))
					{
						const auto& step_component = component_manager.physic_step.at(entity);
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
		const auto& grid = world->getGrid();
		Transform above = { {step.x + 1.f, step.y - height + 1.f}, {step.w - 2.f, height - 2.f} };
		SDL_FRect above_rect = { step.x + 1.f, step.y - height + 1.f, step.w - 2.f, height - 2.f };

		int grid_local_x = static_cast<int>(above.position.x / 20.0f);
		int grid_local_y = static_cast<int>(above.position.y / 20.0f);

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
	InputSystem() = default;

	void update(const graphics::Renderer& screen, float dt)
	{
		//Calculate global mouse position
		glm::vec2 mouse_global_position = getMouseGlobalPosition(screen);

		for (auto& entity : EntityManager::get().getEntities())
		{
			if (ComponentManager::get().player.contains(entity))
			{
				auto& p = ComponentManager::get().player.at(entity);

				// Jump
				if (ComponentManager::get().jump.contains(entity))
				{
					auto& j = ComponentManager::get().jump.at(entity);
					j.jump_ready = InputManager::isKey(SDLK_U);
				}

				//Mining
				if (ComponentManager::get().mine_intent.contains(entity))
				{
					auto& mi = ComponentManager::get().mine_intent.at(entity);
					//mi.active = InputManager::getMouseState().left == MouseButtonState::HELD;

					if (InputManager::getMouseState().left == MouseButtonState::DOWN)
					{
						mi.active = true;
						mi.start_mouse_position = mouse_global_position;
					}
					if (InputManager::getMouseState().left == MouseButtonState::RELEASED || ComponentManager::get().mine_objects_finished.contains(entity))
					{
						mi.active = false;
					}

					mi.current_mouse_position = mouse_global_position;
				}

				//Block placement
				if (ComponentManager::get().place_intent.contains(entity))
				{
					auto& pi = ComponentManager::get().place_intent.at(entity);
					pi.active = InputManager::getMouseState().right == MouseButtonState::HELD;
					pi.target_global_position = mouse_global_position;
				}

				//Horizontal movement
				if (ComponentManager::get().physics.contains(entity))
				{
					auto& ph = ComponentManager::get().physics.at(entity);
					if (InputManager::isKey(SDLK_H))
					{
						ph.velocity.x -= ph.acceleration.x * dt;
						ph.velocity.x = std::clamp(ph.velocity.x, -ph.max_velocity.x, ph.max_velocity.x);

						//Flip
						if (ComponentManager::get().renderable.contains(entity)) ComponentManager::get().renderable.at(entity).flip_mode = SDL_FLIP_HORIZONTAL;
					}
					if (InputManager::isKey(SDLK_K))
					{
						ph.velocity.x += ph.acceleration.x * dt;
						ph.velocity.x = std::clamp(ph.velocity.x, -ph.max_velocity.x, ph.max_velocity.x);

						//Flip
						if (ComponentManager::get().renderable.contains(entity))ComponentManager::get().renderable.at(entity).flip_mode = SDL_FLIP_NONE;
					}
					if (!InputManager::isKey(SDLK_K) && !InputManager::isKey(SDLK_H))
					{
						/*ph.velocity.x -= ph.velocity.x * ph.decelaration * dt;*/
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
	JumpSystem() = default;

	void update(float dt)
	{
		for (auto& entity : EntityManager::get().getEntities())
		{
			if (ComponentManager::get().physics.contains(entity) && ComponentManager::get().jump.contains(entity))
			{
				auto& ph = ComponentManager::get().physics.at(entity);
				auto& j = ComponentManager::get().jump.at(entity);

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
	MiningTilesSystem(World& world, float tile_width, float tile_height)
		: tile_width(tile_width)
		, tile_height(tile_height)
		, world(world)
	{

	}

	void update(float dt)
	{
		for (const auto& entity : EntityManager::get().getEntities())
		{
			if (!ComponentManager::get().transform.contains(entity) ||
				!ComponentManager::get().mine_intent.contains(entity) ||
				ComponentManager::get().mine_objects_state.contains(entity) ||
				!ComponentManager::get().mining_ability.contains(entity))
				continue;

			tiles_covered.clear();

			auto& mi = ComponentManager::get().mine_intent.at(entity);
			auto& ts = ComponentManager::get().transform.at(entity);
			const auto& mining_ability_component = ComponentManager::get().mining_ability.at(entity);

			float mining_speed = mining_ability_component.speed;
			float mining_radius = mining_ability_component.radius;
			float mining_size = mining_ability_component.size;

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
		for (const auto& entity : EntityManager::get().getEntities())
		{
			if (!ComponentManager::get().transform.contains(entity) ||
				ComponentManager::get().mine_objects_state.contains(entity) ||
				!ComponentManager::get().mine_intent.contains(entity) ||
				!ComponentManager::get().mining_ability.contains(entity))
				continue;

			auto& ts = ComponentManager::get().transform.at(entity);
			const auto& mining_ability_component = ComponentManager::get().mining_ability.at(entity);

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
	World& world;

	float tile_width = 1.f;
	float tile_height = 1.f;

	std::vector<glm::ivec2> tiles_covered;
};

class PlaceSystem
{
public:
	PlaceSystem(World& world, float tile_width, float tile_height)
		: tile_width(tile_width)
		, tile_height(tile_height)
		, world(world)
	{
	}

	void update(float dt)
	{
		auto& component_manager = ComponentManager::get();
		for (const auto& entity : EntityManager::get().getEntities())
		{
			if (component_manager.transform.contains(entity) && 
				component_manager.place_ability.contains(entity) &&
				component_manager.place_intent.contains(entity))
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
	World& world;
	float tile_width = 1.f;
	float tile_height = 1.f;
};

class MiningObjectsSystem
{
public:
	MiningObjectsSystem(World& world, float tile_width, float tile_height)
		: tile_width(tile_width)
		, tile_height(tile_height)
		, world(world)
	{

	}

	void update(float dt)
	{
		auto& component_manager = ComponentManager::get();
		for (auto& entity : EntityManager::get().getEntities())
		{
			//Check if required components exist
			if (!component_manager.transform.contains(entity) ||
				!component_manager.mine_intent.contains(entity) ||
				!component_manager.mining_ability.contains(entity))
				continue;

			auto& ts = component_manager.transform.at(entity);
			auto& mi = component_manager.mine_intent.at(entity);
			const auto& mining_ability_component = component_manager.mining_ability.at(entity);

			float mining_speed = mining_ability_component.speed;
			float mining_radius = mining_ability_component.radius;
			float mining_size = mining_ability_component.size;

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
							addRandomizedItem(entity, item);
						}

					}
					// std::cout << "Deal damage\n";
				}
			}
		}
	}

	void render(graphics::Renderer& screen)
	{
		auto& component_manager = ComponentManager::get();
		for (auto& entity : EntityManager::get().getEntities())
		{
			if (component_manager.mine_intent.contains(entity) && component_manager.transform.contains(entity) &&  component_manager.mine_intent.contains(entity))
			{
				auto& ts = component_manager.transform.at(entity);
				auto& mi = component_manager.mine_intent.at(entity);

				float mining_speed;
				float mining_radius;
				float mining_size;
				if (ComponentManager::get().equipment.contains(entity) && ComponentManager::get().equipment.at(entity).pickaxe)
				{
					const auto& pickaxe_component = ComponentManager::get().equipment.at(entity).pickaxe;
					const auto& item_properties = ItemManager::get().getProperties(pickaxe_component->id);
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
	World& world;

	float tile_width = 1.f;
	float tile_height = 1.f;
};

//Do it before this system, so that EquipItem Component is right of other systems as well
//Note: I just need to send another event ex. ItemEquipped(Item) to notify my WeaponCircleSystem works correctly
class ItemUsageSystem
{
public:
	ItemUsageSystem(Entity& target_entity)
		: target_entity(target_entity)
	{
	}

	void update()
	{
		auto& component_manager = ComponentManager::get();

		//Remove Item Equip/Unequip components after one frame
		if (component_manager.item_equipped.contains(target_entity))
		{
			component_manager.item_equipped.erase(target_entity);
		}
		if (component_manager.item_unequipped.contains(target_entity))
		{
			component_manager.item_unequipped.erase(target_entity);
		}

		if (component_manager.equipment.contains(target_entity))
		{
			auto& equipment_component = component_manager.equipment.at(target_entity);

			if (component_manager.use_item.contains(target_entity))
			{
				const auto& use_item_component = component_manager.use_item.at(target_entity);
				const auto& item_properties = ItemManager::get().getProperties(use_item_component.item_id);

				if (item_properties.heal_data)
				{
					auto& health_component = ComponentManager::get().health[target_entity];
					health_component.current_health = std::min(health_component.max_health, health_component.current_health + item_properties.heal_data->amount);
				}

				component_manager.use_item.erase(target_entity);
			}
			if (component_manager.equip_item.contains(target_entity))
			{
				const auto& equip_item_component = component_manager.equip_item.at(target_entity);
				const auto& item_properties = ItemManager::get().getProperties(equip_item_component.item->id);

				if (item_properties.pickaxe_data)
				{
					if (equipment_component.pickaxe)
						component_manager.equipment.at(target_entity).pickaxe->equipped = false;

					equipment_component.pickaxe = equip_item_component.item;
					equip_item_component.item->equipped = true;	
					component_manager.item_equipped[target_entity] = ItemEquipped{ equip_item_component.item };

					// Set mining properties (speed, radius, size)
					if (component_manager.mining_ability.contains(target_entity))
					{
						auto& mining_ability = component_manager.mining_ability.at(target_entity);
						mining_ability.speed = item_properties.pickaxe_data->speed;
						mining_ability.radius = item_properties.pickaxe_data->radius;
						mining_ability.size = item_properties.pickaxe_data->size;
					}

				}

				// Melee weapon
				if (item_properties.melee_weapon_data)
				{
					if (equipment_component.weapons.size() < equipment_component.max_weapon)
					{
						component_manager.equipment.at(target_entity).weapons.push_back(equip_item_component.item);
						equip_item_component.item->equipped = true;
						component_manager.item_equipped[target_entity] = ItemEquipped{ equip_item_component.item };
					}
				}


				component_manager.equip_item.erase(target_entity);
			}	

			if (component_manager.unequip_item.contains(target_entity))
			{
				auto& unequip_item_component = component_manager.unequip_item.at(target_entity);
				const auto& item_properties = ItemManager::get().getProperties(unequip_item_component.item->id);
				auto* item = unequip_item_component.item;
				
				if (item_properties.pickaxe_data)
				{
					equipment_component.pickaxe = nullptr;
					item->equipped = false;

					// Set mining properties (speed, radius, size)
					if (component_manager.mining_ability.contains(target_entity))
					{
						auto& mining_ability = component_manager.mining_ability.at(target_entity);
						mining_ability.speed = BASE_MINING_SPEED;
						mining_ability.radius = BASE_MINING_RADIUS;
						mining_ability.size = BASE_MINING_SIZE;
					}
				}

				// Melee weapon
				if (item_properties.melee_weapon_data)
				{
					auto& weapon_array = component_manager.equipment.at(target_entity).weapons;
					weapon_array.erase(std::ranges::remove(weapon_array, item).begin(), weapon_array.end());
					item->equipped = false;
				}

				component_manager.item_unequipped[target_entity] = ItemUnequipped{ unequip_item_component.item };
				component_manager.unequip_item.erase(target_entity);
			}
		}
	}

	void useItem(const ItemProperties& item_properties)
	{
		if (item_properties.heal_data)
		{
			auto& health_component = ComponentManager::get().health[target_entity];
			health_component.current_health = std::min(health_component.max_health, health_component.current_health + item_properties.heal_data->amount);
		}
	}

	void equipItem(Item* item)
	{
		auto& component_manager = ComponentManager::get();
		const auto& item_properties = ItemManager::get().getProperties(item->id);

		if (component_manager.equipment.contains(target_entity))
		{
			auto& equipment_component = component_manager.equipment.at(target_entity);

			// Pickaxe
			if (item_properties.pickaxe_data)
			{
				if (equipment_component.pickaxe)
					component_manager.equipment.at(target_entity).pickaxe->equipped = false;

				equipment_component.pickaxe = item;
				item->equipped = true;

				// Set mining properties (speed, radius, size)
				if (component_manager.mining_ability.contains(target_entity))
				{
					auto& mining_ability = component_manager.mining_ability.at(target_entity);
					mining_ability.speed = item_properties.pickaxe_data->speed;
					mining_ability.radius = item_properties.pickaxe_data->radius;
					mining_ability.size = item_properties.pickaxe_data->size;
				}

				return;
			}

			// Melee weapon
			if (item_properties.melee_weapon_data)
			{
				if (equipment_component.weapons.size() < equipment_component.max_weapon)
				{
					component_manager.equipment.at(target_entity).weapons.push_back(item);
					item->equipped = true;
					return;
				}
			}
		}
	}

	void unequip(Item* item)
	{
		const auto& item_properties = ItemManager::get().getProperties(item->id);
		auto& component_manager = ComponentManager::get();

		if (component_manager.equipment.contains(target_entity))
		{
			auto& equipment_component = component_manager.equipment.at(target_entity);

			// Pickaxe
			if (item_properties.pickaxe_data)
			{
				equipment_component.pickaxe = nullptr;
				item->equipped = false;

				// Set mining properties (speed, radius, size)
				if (component_manager.mining_ability.contains(target_entity))
				{
					auto& mining_ability = component_manager.mining_ability.at(target_entity);
					mining_ability.speed = BASE_MINING_SPEED;
					mining_ability.radius = BASE_MINING_RADIUS;
					mining_ability.size = BASE_MINING_SIZE;
				}

				return;
			}

			// Melee weapon
			if (item_properties.melee_weapon_data)
			{
				auto& weapon_array = component_manager.equipment.at(target_entity).weapons;
				weapon_array.erase(std::ranges::remove(weapon_array, item).begin(), weapon_array.end());
				item->equipped = false;
				return;
			}
		}
	}

private:
	Entity& target_entity;
};


class ButtonSystem
{
public:
	ButtonSystem() = default;

	void update()
	{
		auto& component_manager = ComponentManager::get();
		for (const auto& entity : EntityManager::get().getEntities())
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
		auto& component_manager = ComponentManager::get();
		if (component_manager.craft_button.contains(button))
		{
			const auto& craft_component = component_manager.craft_button.at(button);

			if (craft_component.is_available)
			{
				component_manager.craft_item[button] = CraftItem{ craft_component.recipe_id };
			}
		}
	}
};


class CraftSystem
{
public:
	CraftSystem() = default;

	void update(Entity target_entity)
	{
		auto& component_manager = ComponentManager::get();
		for (const auto& entity : EntityManager::get().getEntities())
		{
			//NOTE: CraftItem is a component that is attached to a CraftButton when user tries to craft an item.
			//And the entity we have to give an item is our target_entity
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
				/*
				inventory->addItem(recipe.item_id, 1);
				*/

				component_manager.add_item[target_entity] = AddItem{ Item{recipe.item_id, 1} };
				
				for (const auto& required_craft_item : recipe.required_items)
				{
					inventory->removeItem(required_craft_item);
				}
			}
		}
	}
};