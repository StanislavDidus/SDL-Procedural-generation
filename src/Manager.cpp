#include "Manager.hpp"

template<typename T>
const T& Manager<T>::getData(size_t ID) const
{
	return data[ID];
}

template <typename T>
const std::vector<T>& Manager<T>::getAllData() const
{
	return data;
}

template <typename T>
size_t Manager<T>::add(const T& data)
{
	this->data.push_back(data);
	return data_size++;
}
