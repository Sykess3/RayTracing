#pragma once

#include "Walnut/Image.h"

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"
#include "Utils.h"

#include <memory>
#include <glm/glm.hpp>

class Renderer
{
public:


	enum class Algo : int
	{
		CPUSingleThreaded,
		CPUMultiThreaded,
		MAX
	};
	constexpr static int AlgoCout = static_cast<int>(Algo::MAX);


	struct Settings
	{

		bool Accumulate = true;
		Algo AlgoType = Algo::CPUMultiThreaded;
		int Bounces = 50.0f;
	};
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& camera);


	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

	void ResetFrameIndex() { m_FrameIndex = 1; }
	Settings& GetSettings() { return m_Settings; }
	
private:
	void ClearAccumulationData();
private:

	struct HitPayload
	{
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;

		int ObjectIndex;
	};

	void Metal(const Scene& activeScene, Ray& inOutRay, const Renderer::HitPayload& payload, glm::vec3& contribution, uint32_t& seed);
	void Lambertian(const Scene& activeScene, Ray& inOutRay, const Renderer::HitPayload& payload, glm::vec3& contribution, uint32_t& seed);
	void Dielectric(const Scene& activeScene, Ray& inOutRay, const Renderer::HitPayload& payload, glm::vec3& contribution, uint32_t& seed);

	glm::vec4 PerPixel(uint32_t x, uint32_t y); // RayGen

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
	HitPayload Miss(const Ray& ray);
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	Settings m_Settings;

	std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

	uint32_t m_FrameIndex = 1;
	uint32_t m_frameRandomNumber;
};


template <>
inline const std::array<const char*, Renderer::AlgoCout>& GetAllArrChars<Renderer::Algo, Renderer::AlgoCout>() {
	static const std::array<const char*, Renderer::AlgoCout> typeNames = { "CPU Single Threaded", "CPU Multi Threaded" };
	return typeNames;
}

inline const char* EnumToChar(Renderer::Algo t) {
	switch (t)
	{
	case Renderer::Algo::CPUSingleThreaded:
		return "CPU Single Threaded";
	case Renderer::Algo::CPUMultiThreaded:
		return "CPU Multi Threaded";
	}
	assert(false);
}