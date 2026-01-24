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

	static ObjectManager& get()
	{
		static ObjectManager s;
		return s;
	}

	//Load data from XML file
	void loadXml(const std::filesystem::path& path); ///< Takes a <b>path</b> to the <b>XML</b> file and loads the contents of it

	//ObjectProperties
	const ObjectProperties& getProperties(int ID) const;

	//ObjectSpawnInfo
	std::vector<ObjectSpawnInfo>& getAllObjectSpawnInfos();
private:
	ObjectManager() = default;

	ObjectManager(const ObjectManager& other) = delete;
	ObjectManager(ObjectManager&& other) noexcept = delete;
	ObjectManager& operator=(const ObjectManager& other) = delete;
	ObjectManager& operator=(ObjectManager&& other) noexcept = delete;

	size_t registerObjectProperties(const ObjectProperties& properties); ///< Adds new <b>ObjectProperties</b> to the <b>ObjectManager</b> and returns an ID to them.
	void addObjectSpawnInfo(const ObjectSpawnInfo& spawn_info);

	std::vector<ObjectProperties> object_properties;
	std::vector<ObjectSpawnInfo> object_spawn_infos;

	size_t objects_count = 0;
};