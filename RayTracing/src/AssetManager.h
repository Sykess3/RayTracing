#pragma once

#include <string>
#include <glm/glm.hpp>
#include <cassert>

#include "yas/object.hpp"
#include "yas/std_types.hpp"
#include <filesystem>  
#include <yas/serialize.hpp>
#include "yas/object.hpp"
#include <memory>

namespace yas {
	YAS_DEFINE_INTRUSIVE_SERIALIZE_NVP("glm::vec3", glm::vec3, ("x", x), ("y", y), ("z", z));
}

class AssetManager
{
public:

	template <typename T>
	void SaveJSON(const T& object, const std::string& path)
	{
		std::filesystem::path dir = std::filesystem::path(path).parent_path(); 
		if (!dir.empty()) {
			std::filesystem::create_directories(dir);
		}
		if (std::filesystem::exists(path)) {
			std::filesystem::remove(path);
			std::cout << "Existing file deleted: " << path << std::endl;
		}

		constexpr std::size_t flags = yas::file | yas::json;

		yas::save<flags>(path.data(), object);
	}

	template <typename T>
	void SaveJSON(const std::shared_ptr<typename T> object, const std::string& path)
	{
		SaveJSON<T>(*object, path);
	}

	template <typename T>
	void LoadJSON(T& object, const std::string& path)
	{
		if (!std::filesystem::exists(path)) {
			std::cout << "File not exist: " << path << std::endl;
			return;
		}
		T temp;
		yas::load<yas::file | yas::json>(path.data(), temp);
		object = std::move(temp);
	}

	template <typename T>
	void LoadJSON(std::shared_ptr<typename T> object, const std::string& path)
	{
		LoadJSON(*object, path);
	}
};

