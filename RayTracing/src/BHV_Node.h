#pragma once

#include <memory>
#include "AABB.h"
#include <string>
#include <iostream>
#include "Interval.h"


struct HitDetection
{
	Interval CurrentHitInterval;
	int HitObject;
};


struct Sphere;
struct Scene;

struct BVH_Node
{
public:

	inline static int Count1 = 0;
	inline static int Count2 = 0;
	std::string Debug;

	static bool box_compare(const Sphere& a, const Sphere& b, int axis_index);

	static bool box_x_compare(const Sphere& a, const Sphere& b) {
		return box_compare(a, b, 0);
	}

	static bool box_y_compare(const Sphere& a, const Sphere& b) {
		return box_compare(a, b, 1);
	}

	static bool box_z_compare(const Sphere& a, const Sphere& b) {
		return box_compare(a, b, 2);
	}


	BVH_Node(std::unique_ptr<BVH_Node> left, std::unique_ptr<BVH_Node> right, const aabb& bbox)
		: m_left(std::move(left)),
		m_right(std::move(right)),
		m_bbox(bbox),
		m_boundingObjectIndex(-1)

	{
		Count1++;
		Debug = std::string("bbox:") + std::to_string(Count1);
	}

	BVH_Node(int boundingObjectIndex, const aabb& bbox)
		: m_left(nullptr),
		m_right(nullptr),
		m_bbox(bbox),
		m_boundingObjectIndex(boundingObjectIndex)

	{
		Count2++;
		Debug = std::string("object:") + std::to_string(boundingObjectIndex);
	}

	void Display(int parentCount) 
	{
		std::string result;
		for (int i = 0; i <= parentCount; ++i)
		{
			result += "   ";
		}

		result += Debug;
		std::cout << result << std::endl;
		if (m_left)
		{
			m_left->Display(parentCount + 1);
		}
		if (m_right)
		{
			m_right->Display(parentCount + 1);
		}
	}
	void RayCast(const Ray& ray, HitDetection& transientHit, const Scene* scene);

	const aabb& GetBoundingBox() const { return m_bbox; }

private:
	std::unique_ptr<BVH_Node> m_left;
	std::unique_ptr<BVH_Node> m_right;

	aabb m_bbox;
	int m_boundingObjectIndex;
};
