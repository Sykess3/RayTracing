#pragma once

#include <glm/glm.hpp>
#include "Ray.h"
#include <vector>
#include <array>
#include <yas/serialize.hpp>

template <typename T, int Count>
inline const std::array<const char*, Count>& GetAllArrChars() {
	static_assert(false);

	static const std::array<const char*, Count> empty{};
	return empty;
}

struct Material
{
	enum class EType
	{
		Lambertian,
		Metalic,
		Dielectric,
		MAX
	};

	bool operator==(const Material& other) const
	{
		return Albedo == other.Albedo &&
			Fuzzy == other.Fuzzy &&
			RefactionIndex == other.RefactionIndex &&
			Type == other.Type;
	}

	YAS_DEFINE_STRUCT_SERIALIZE_NVP("Material",
		("Albedo", Albedo),
		("Fuzzy", Fuzzy),
		("RefractionIndex", RefactionIndex),
		("Type", Type))


	static constexpr int ETypeCount = static_cast<int>(Material::EType::MAX);

	glm::vec3 Albedo{ 1.0f };
	// Metalic only
	float Fuzzy = 0.0f;
	// Dielectrict only
	float RefactionIndex = 0.0f;

	EType Type = EType::Lambertian;
};

template <>
inline const std::array<const char*, Material::ETypeCount>& GetAllArrChars<Material::EType, Material::ETypeCount>() {
    static const std::array<const char*, Material::ETypeCount> typeNames = { "Lambertian", "Metalic", "Dielectric" };
	return typeNames;
}

inline const char* EnumToChar(Material::EType t) {
	switch (t)
	{
	default: assert(false);
		return "InvalidEnumStr";
	case Material::EType::Lambertian:
		return "Lambertian";
	case Material::EType::Metalic:
		return "Metalic";
	case Material::EType::Dielectric:
		return "Dielectric";
	}
}

struct Sphere
{
	glm::vec3 Position{0.0f};
	float Radius = 0.5f;

	int MaterialIndex = 0;

	YAS_DEFINE_STRUCT_SERIALIZE_NVP("Sphere",
		("Position", Position),
		("Radius", Radius),
		("MaterialIndex", MaterialIndex))
};

struct Scene
{
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;

	YAS_DEFINE_STRUCT_SERIALIZE_NVP("Scene", ("Spheres", Spheres), ("Materials", Materials));


	inline const Material& GetMaterial(int objectIndex) const 
	{
		int materialIndex = Spheres[objectIndex].MaterialIndex;
		return Materials[materialIndex];
	}

	inline Material& GetMaterial_Mutable(int objectIndex) 
	{
		return const_cast<Material&>(GetMaterial(objectIndex));
	}


	inline const Sphere& GetObject(int objectIndex) const 
	{
		return Spheres[objectIndex];
	}
	inline Sphere& GetObject_Mutable(int objectIndex)
	{
		return const_cast<Sphere&>(GetObject(objectIndex));
	}

	inline void Destroy(int objectIndex) 
	{
		if (objectIndex >= 0 && Spheres.size() > objectIndex)
		{
			Spheres.erase(Spheres.begin() + objectIndex);
		}
	}

	struct RayCastHit 
	{
		int ObjectIndex;
		float HitDistance;
	};

	RayCastHit RayCast(const Ray& ray) const {
		int closestSphere = -1;
		float hitDistance = std::numeric_limits<float>::max();
		for (size_t i = 0; i < Spheres.size(); i++)
		{
			const Sphere& sphere = Spheres[i];
			glm::vec3 origin = ray.Origin - sphere.Position;

			float a = glm::dot(ray.Direction, ray.Direction);
			float b = 2.0f * glm::dot(origin, ray.Direction);
			float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

			// Quadratic forumula discriminant:
			// b^2 - 4ac

			float discriminant = b * b - 4.0f * a * c;
			if (discriminant < 0.0f)
				continue;

			// Quadratic formula:
			// (-b +- sqrt(discriminant)) / 2a

			// float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a); // Second hit distance (currently unused)
			float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
			if (closestT > 0.0f && closestT < hitDistance)
			{
				hitDistance = closestT;
				closestSphere = (int)i;
			}
		}

		RayCastHit hit;
		hit.HitDistance = hitDistance;
		hit.ObjectIndex = closestSphere;
		return hit;
	}
};