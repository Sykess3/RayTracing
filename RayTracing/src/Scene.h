#pragma once

#include <glm/glm.hpp>

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
	float Fuzzy;
	float RefactionIndex;

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
};