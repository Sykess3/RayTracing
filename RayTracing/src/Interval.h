#pragma once
#include <limits>
class Interval {
public:
	float Min, Max;

	Interval() : Min(std::numeric_limits<float>::max()), Max(std::numeric_limits<float>::min()) {} // Default interval is empty

	Interval(float min, float max) : Min(min), Max(max) {}

	Interval(const Interval& a, const Interval& b) {
		// Create the interval tightly enclosing the two input intervals.
		Min = a.Min <= b.Min ? a.Min : b.Min;
		Max = a.Max >= b.Max ? a.Max : b.Max;
	}

	float size() const {
		return Max - Min;
	}

	bool Contains(float x) const {
		return Min <= x && x <= Max;
	}

	bool Surrounds(float x) const {
		return Min < x && x < Max;
	}

	float Clamp(float x) const {
		if (x < Min) return Min;
		if (x > Max) return Max;
		return x;
	}

	Interval Expand(float delta) const {
		auto padding = delta / 2;
		return Interval(Min - padding, Max + padding);
	}

	static const Interval Empty;
};

inline const Interval Interval::Empty = Interval();

inline Interval operator+(const Interval& ival, float displacement) {
	return Interval(ival.Min + displacement, ival.Max + displacement);
}

inline Interval operator+(float displacement, const Interval& ival) {
	return ival + displacement;
}

