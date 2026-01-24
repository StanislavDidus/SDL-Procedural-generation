#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>


template<typename T>
class Manager
{
public:
	const T& getData(size_t ID) const;
	const std::vector<T>& getAllData() const;
protected:
	size_t add(const T& data);
	std::unordered_map<std::string, size_t> nameToID;
	std::vector<T> data;
	size_t data_size = 0;
};
