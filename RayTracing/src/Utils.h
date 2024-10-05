#pragma once

#include "glm/glm.hpp"

namespace Utils 
{
	inline uint32_t PCG_Hash(uint32_t seed)
	{
		uint32_t state = seed * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	namespace Random 
	{
		namespace Fast 
		{
			inline float FloatRef(uint32_t& seed) 
			{
				seed = PCG_Hash(seed);
				return (float)seed / UINT32_MAX;
			}

			inline glm::vec3 InUnitSphereRef(uint32_t& seed)
			{
				return glm::normalize(glm::vec3(FloatRef(seed) * 2 - 1, FloatRef(seed) * 2 - 1, FloatRef(seed) * 2 - 1));
			}
		}
	}
	namespace Math 
	{
		inline bool IsNearZero(const glm::vec3& v, float epsilon = 1e-6f) {
			return std::fabs(v.x) < epsilon && std::fabs(v.y) < epsilon && std::fabs(v.z) < epsilon;
		}
	}
}