#pragma once

#include <glm/glm.hpp>
#include "Ray.h"
#include <vector>
#include <array>
#include "AABB.h"
#include <algorithm>
#include "BHV_Node.h"
#include <tuple>
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
	std::vector<BVH_Node> m_nodes;

	YAS_DEFINE_STRUCT_SERIALIZE_NVP("Scene", ("Spheres", Spheres), ("Materials", Materials));

	void ConstructBVH() 
	{
		std::vector<std::tuple<Sphere, int>> sphereToIndex;
		m_nodes.reserve(1000);
		for (int i = 0; i < Spheres.size(); ++i)
		{
			sphereToIndex.push_back({Spheres[i], i });
		}

		ConstructBVH(sphereToIndex, m_nodes, 0, sphereToIndex.size());

		std::reverse(m_nodes.begin(), m_nodes.end());
		for(BVH_Node& node : m_nodes)
		{
			node.m_left = m_nodes.size() - 1 - node.m_left;
			node.m_right = m_nodes.size() - 1 - node.m_right;
		}
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

	void RayCastRec(const Ray& ray, const BVH_Node& root, HitDetection& detected) const
	{
		if (root.RayCast(ray, detected, this))
		{
			if (root.m_right != std::numeric_limits<int16_t>::max() && m_nodes[root.m_right].RayCast(ray, detected, this))
			{
				RayCastRec(ray, m_nodes[root.m_right], detected);
			}
			if (root.m_left != std::numeric_limits<int16_t>::max() && m_nodes[root.m_left].RayCast(ray, detected, this))
			{
				RayCastRec(ray, m_nodes[root.m_left], detected);
			}
		}
	}

	RayCastHit RayCast(const Ray& ray) const
	{
#if 0

		HitDetection transieHit;
		transieHit.CurrentHitInterval.Min = 0.01f;
		transieHit.CurrentHitInterval.Max = std::numeric_limits<float>::max();
		transieHit.HitObject = -1;
		const BVH_Node& root = m_nodes[0];
		RayCastRec(ray, root, transieHit);

		RayCastHit hit;
		hit.HitDistance = transieHit.CurrentHitInterval.Max;
		hit.ObjectIndex = transieHit.HitObject;
		return hit;
#else
		return RayCastSphere(ray);
#endif
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
		float root = (-b - glm::sqrt(discriminant)) / (2.0f * a);

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
	}

private:
	int ConstructBVH(std::vector<std::tuple<Sphere, int>>& sphereToIndex, std::vector<BVH_Node>& nodes, int start, int end) {
		if (start >= end) {
			return std::numeric_limits<int16_t>::max();
		}

		aabb bbox;
		for (int i = start; i < end; ++i) {
			bbox = aabb(bbox, std::get<0>(sphereToIndex[i]).BoundingBox);
		}

		int object_span = end - start;

		if (object_span == 1) {
			int index = std::get<1>(sphereToIndex[start]);
			nodes.emplace_back(index, bbox);
			return nodes.size() - 1;
		}

		int axis = bbox.longest_axis();
		auto comparator = (axis == 0) ? BVH_Node::box_x_compare
			: (axis == 1) ? BVH_Node::box_y_compare
			: BVH_Node::box_z_compare;

		std::sort(sphereToIndex.begin() + start, sphereToIndex.begin() + end, comparator);

		int mid = start + object_span / 2;

		int left = ConstructBVH(sphereToIndex, nodes, start, mid);
		int right = ConstructBVH(sphereToIndex, nodes, mid, end);

		nodes.emplace_back(left, right, bbox);

		return nodes.size() - 1;
	}
};