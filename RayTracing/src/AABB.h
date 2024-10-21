#pragma once

#include "glm/glm.hpp"
#include "Ray.h"
#include "Interval.h"

class aabb {
public:
	Interval x, y, z;

	aabb()
		: x(Interval::Empty),
		y(Interval::Empty),
		z(Interval::Empty)
	{
	}

	aabb(const Interval& x, const Interval& y, const Interval& z)
		: x(x), y(y), z(z)
	{
		pad_to_minimums();
	}

	aabb(const glm::vec3& a, const glm::vec3& b) {
		// Treat the two points a and b as extrema for the bounding box, so we don't require a
		// particular minimum/maximum coordinate order.

		x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
		y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
		z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);

		pad_to_minimums();
	}

	aabb(const aabb& box0, const aabb& box1) {
		x = Interval(box0.x, box1.x);
		y = Interval(box0.y, box1.y);
		z = Interval(box0.z, box1.z);
	}

	const Interval& axis_interval(int n) const {
		if (n == 1) return y;
		if (n == 2) return z;
		return x;
	}

	bool hit(const Ray& r, Interval ray_t) const {
		const glm::vec3& ray_orig = r.Origin;
		const glm::vec3& ray_dir = r.Direction;

		for (int axis = 0; axis < 3; axis++) {
			const Interval& ax = axis_interval(axis);
			const float adinv = 1.0f / ray_dir[axis];

			float t0 = (ax.Min - ray_orig[axis]) * adinv;
			float t1 = (ax.Max - ray_orig[axis]) * adinv;

			if (t0 < t1) {
				if (t0 > ray_t.Min) ray_t.Min = t0;
				if (t1 < ray_t.Max) ray_t.Max = t1;
			}
			else {
				if (t1 > ray_t.Min) ray_t.Min = t1;
				if (t0 < ray_t.Max) ray_t.Max = t0;
			}

			if (ray_t.Max <= ray_t.Min)
				return false;
		}
		return true;
	}

	int longest_axis() const {
		// Returns the index of the longest axis of the bounding box.

		if (x.size() > y.size())
			return x.size() > z.size() ? 0 : 2;
		else
			return y.size() > z.size() ? 1 : 2;
	}

private:

	void pad_to_minimums() {
		// Adjust the AABB so that no side is narrower than some delta, padding if necessary.

		float delta = 0.0001;
		if (x.size() < delta) x = x.Expand(delta);
		if (y.size() < delta) y = y.Expand(delta);
		if (z.size() < delta) z = z.Expand(delta);
	}
};

inline aabb operator+(const aabb& bbox, const glm::vec3& offset) {
	return aabb(bbox.x + offset.x, bbox.y + offset.y, bbox.z + offset.z);
}

inline aabb operator+(const glm::vec3& offset, const aabb& bbox) {
	return bbox + offset;
}
