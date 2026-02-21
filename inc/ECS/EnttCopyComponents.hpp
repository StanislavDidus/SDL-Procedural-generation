#pragma once
#include "Entity.hpp"

inline static void copyComponents(entt::registry& registry, Entity src, Entity dst)
{
	for(auto [id, storage]: registry.storage()) {
    if(storage.contains(src)) {
        storage.push(dst, storage.value(src));
    }
	}
}
