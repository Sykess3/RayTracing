#pragma once

#include <glm/glm.hpp>
#include "Ray.h"
#include <vector>


struct Material
{
	enum class Type
	{
		Lambertian,
		Metalic,
		Dielectric
	};

	glm::vec3 Albedo{ 1.0f };
	// Metalic only
	float Fuzzy = 0.0f;
	// Dielectrict only
	float RefactionIndex = 0.0f;

	Type Type = Type::Lambertian;
};

struct Sphere
{
	glm::vec3 Position{0.0f};
	float Radius = 0.5f;

	int MaterialIndex = 0;
};

struct Scene
{
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;

	inline const Material& GetMaterial(int objectIndex) const 
	{
		int materialIndex = Spheres[objectIndex].MaterialIndex;
		return Materials[materialIndex];
	}

	inline const Sphere& GetObject(int objectIndex) const 
	{
		return Spheres[objectIndex];
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