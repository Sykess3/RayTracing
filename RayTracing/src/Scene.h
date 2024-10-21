#pragma once

#include <glm/glm.hpp>
#include "Ray.h"
#include <vector>
#include <array>
#include "AABB.h"
#include <algorithm>
#include "BHV_Node.h"
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

struct RayCastHit
{
	int ObjectIndex;
	float HitDistance;
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
	glm::vec3 Position{ 0.0f };
	float Radius = 0.5f;

	int MaterialIndex = 0;
	aabb BoundingBox;

	void CalcBoundingBox()
	{
		auto rvec = glm::vec3(Radius, Radius, Radius);
		BoundingBox = aabb(Position - rvec, Position + rvec);
	}

	YAS_DEFINE_STRUCT_SERIALIZE_NVP("Sphere",
		("Position", Position),
		("Radius", Radius),
		("MaterialIndex", MaterialIndex))
};

struct Scene
{
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;

	std::unique_ptr<BVH_Node> RootBVH_Node;

	YAS_DEFINE_STRUCT_SERIALIZE_NVP("Scene", ("Spheres", Spheres), ("Materials", Materials));

	void ConstructBVH() 
	{
		auto copy = Spheres;
		RootBVH_Node = ConstructBVH(copy, 0, copy.size());
		RootBVH_Node->Display(0);
	}

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

	RayCastHit RayCast(const Ray& ray) const
	{
		HitDetection transieHit;
		transieHit.CurrentHitInterval.Min = 0.01f;
		transieHit.HitObject = -1;
		RootBVH_Node->RayCast(ray, transieHit, this);

		RayCastHit hit;
		hit.HitDistance = transieHit.CurrentHitInterval.Max;
		hit.ObjectIndex = transieHit.HitObject;
		return hit;
	}

	RayCastHit TestHitSphere(const Ray& ray, int objectIndex, Interval& foundObjects)  const
	{
		const Sphere& sphere = Spheres[objectIndex];
		glm::vec3 origin = ray.Origin - sphere.Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

		// Quadratic forumula discriminant:
		// b^2 - 4ac

		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0f)
		{
			RayCastHit failure;
			failure.ObjectIndex = -1;
			return failure;
		}

		// Quadratic formula:
		// (-b +- sqrt(discriminant)) / 2a

		if (b > 0)
		{
			int a = 5;
		}

		// float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a); // Second hit distance (currently unused)
		float root = glm::abs((-b - glm::sqrt(discriminant)) / (2.0f * a));
		//float root = (-b + glm::sqrt(discriminant)) / (2.0f * a);

		if (root < 0.0f)
		{
			int a = 5;
		}
		if (!foundObjects.Surrounds(root))
		{
			RayCastHit failure;
			failure.ObjectIndex = -1;
			return failure;
		}
		RayCastHit hit;
		hit.ObjectIndex = objectIndex;
		hit.HitDistance = root;
		return hit;
	}
private:

	/*
	RayCastHit RayCastSphere(const Ray& ray) const {
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
	}*/

private:
	std::unique_ptr<BVH_Node> ConstructBVH(std::vector<Sphere>& nodes, int start, int end) {
		if (start >= end) {
			return nullptr; 
		}

		aabb bbox;
		for (int i = start; i < end; ++i) {
			bbox = aabb(bbox, nodes[i].BoundingBox);
		}

		int object_span = end - start;

		// If only one object, it's a leaf node
		if (object_span == 1) {
			return std::make_unique<BVH_Node>(start, bbox);
		}

		// Choose the axis to sort on, based on the longest axis of the bounding box
		int axis = bbox.longest_axis();
		auto comparator = (axis == 0) ? BVH_Node::box_x_compare
			: (axis == 1) ? BVH_Node::box_y_compare
			: BVH_Node::box_z_compare;

		// Sort objects along the chosen axis
		std::sort(nodes.begin() + start, nodes.begin() + end, comparator);

		// Find the midpoint to split the objects
		int mid = start + object_span / 2;

		// Recursively build the left and right subtrees
		std::unique_ptr<BVH_Node> left = ConstructBVH(nodes, start, mid);
		std::unique_ptr<BVH_Node> right = ConstructBVH(nodes, mid, end);

		// Return a new BVH node that combines the left and right subtrees
		auto node = std::make_unique<BVH_Node>(std::move(left), std::move(right), bbox);

		return node;
	}
};