#pragma once

#include <memory>
#include "AABB.h"
#include <string>
#include <iostream>
#include "Interval.h"


struct HitDetection
{
	Interval CurrentHitInterval;
	int HitObject = -1;
};


struct Sphere;
struct Scene;

struct BVH_Node
{
public:
	inline static int Count1 = 0;
	inline static int Count2 = 0;

	static bool box_compare(const Sphere& a, const Sphere& b, int axis_index);

	static bool box_x_compare(const std::tuple<const Sphere&, int>& a, const std::tuple<const Sphere&, int>& b) {
		return box_compare(std::get<0>(a), std::get<0>(b), 0);
	}

	static bool box_y_compare(const std::tuple<const Sphere&, int>& a, const std::tuple<const Sphere&, int>& b) {
		return box_compare(std::get<0>(a), std::get<0>(b), 1);
	}

	static bool box_z_compare(const std::tuple<const Sphere&, int>& a, const std::tuple<const Sphere&, int>& b) {
		return box_compare(std::get<0>(a), std::get<0>(b), 2);
	}


	BVH_Node(int16_t left, int16_t right, const aabb& bbox)
		: m_left(left),
		m_right(right),
		m_bbox(bbox),
		m_boundingObjectIndex(-1)

	{
		Count1++;
	}

	BVH_Node(int boundingObjectIndex, const aabb& bbox)
		: m_left(std::numeric_limits<int16_t>::max()),
		m_right(std::numeric_limits<int16_t>::max()),
		m_bbox(bbox),
		m_boundingObjectIndex(boundingObjectIndex)

	{
		Count2++;
	}

	void Display(int parentCount) 
	{

	}

	void PrintChildAddress() {

	}
	bool RayCast(const Ray& ray, HitDetection& transientHit, const Scene* scene) const;

	const aabb& GetBoundingBox() const { return m_bbox; }

	aabb m_bbox;
	int16_t m_left;
	int16_t m_right;
	int m_boundingObjectIndex;
};
