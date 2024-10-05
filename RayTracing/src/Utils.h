#pragma once

#include "glm/glm.hpp"

namespace Utils 
{
	namespace Math 
	{
		inline bool IsNearZero(const glm::vec3& v, float epsilon = 1e-6f) {
			return std::fabs(v.x) < epsilon && std::fabs(v.y) < epsilon && std::fabs(v.z) < epsilon;
		}
	}
}