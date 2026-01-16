#pragma once

#include <vector>

#include "Object.hpp"

/// <summary>
/// Manager that holds <b>ObjectProperties</b> and <b>ObjectSpawnInfo</b> and gives access to them by a special ID that is assigned to all objects
/// </summary>
class ObjectManager
{
public:
	ObjectManager() = default;

	//ObjectProperties
	const ObjectProperties& getProperties(int ID) const;
	int registerObjectProperties(const ObjectProperties& properties); ///< Adds new <b>ObjectProperties</b> to the <b>ObjectManager</b> and returns an ID to them.

	//ObjectSpawnInfo
	const std::vector<ObjectSpawnInfo>& getAllObjectSpawnInfos() const;
	void addObjectSpawnInfo(const ObjectSpawnInfo& spawn_info);
private:
	std::vector<ObjectProperties> object_properties;
	std::vector<ObjectSpawnInfo> object_spawn_infos;
};