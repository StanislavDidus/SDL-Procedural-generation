#pragma once

#include <vector>

#include "Object.hpp"
#include "XMLParsing.hpp"
#include <filesystem>

/// <summary>
/// Manager that holds <b>ObjectProperties</b> and <b>ObjectSpawnInfo</b> and gives access to them by a special ID that is assigned to all objects
/// </summary>
class ObjectManager
{
public:
	ObjectManager() = default;

	//Load data from XML file
	void loadXML(const std::filesystem::path& path); ///< Takes a <b>path</b> to the <b>XML</b> file and loads the contents of it

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