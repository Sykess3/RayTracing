#include "Renderer.h"

#include "Walnut/Random.h"
#include <glm/gtx/norm.hpp >
#include <execution>

namespace Utils {

	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}

}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		// No resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];
	ClearAccumulationData();

	m_ImageHorizontalIter.resize(width);
	m_ImageVerticalIter.resize(height);
	for (uint32_t i = 0; i < width; i++)
		m_ImageHorizontalIter[i] = i;
	for (uint32_t i = 0; i < height; i++)
		m_ImageVerticalIter[i] = i;
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;
	
	if (m_FrameIndex == 1)
		ClearAccumulationData();


#define MT 1
#if MT
	std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
		[this](uint32_t y)
		{
			std::for_each(m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
				[this, y](uint32_t x)
				{
					glm::vec4 color = PerPixel(x, y);
					if (color.x < 0)
					{
						int a = 6;
					}
					m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

					glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
					if (accumulatedColor.x < 0)
					{
						int a = 5;
					}
					accumulatedColor /= (float)m_FrameIndex;

					accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
					m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
				});
		});

#else

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec4 color = PerPixel(x, y);
			m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

			glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
			accumulatedColor /= (float)m_FrameIndex;

			accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
		}
	}
#endif

	m_FinalImage->SetData(m_ImageData);

	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}

void Renderer::ClearAccumulationData()
{
	memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));
}

void Renderer::Metal(const Scene& activeScene, Ray& inOutRay,  const Renderer::HitPayload& payload, glm::vec3& contribution, uint32_t& seed)
{
	glm::vec3 reflected = glm::reflect(inOutRay.Direction, payload.WorldNormal);
	const Material& hitMaterial = activeScene.GetMaterial(payload.ObjectIndex);
	reflected = glm::normalize(reflected) + (hitMaterial.Fuzzy * Utils::Random::Fast::InUnitSphereRef(seed));
	inOutRay.Direction = glm::normalize(reflected);
	inOutRay.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
	contribution = hitMaterial.Albedo;
}

void Renderer::Lambertian(const Scene& activeScene, Ray& inOutRay, const Renderer::HitPayload& payload, glm::vec3& contribution, uint32_t& seed)
{
	auto direction = glm::normalize(payload.WorldNormal + Utils::Random::Fast::InUnitSphereRef(seed));
	if (Utils::Math::IsNearZero(direction))
	{
		direction = payload.WorldNormal;
	}

	const Material& hitMaterial = activeScene.GetMaterial(payload.ObjectIndex);

	inOutRay.Direction = direction;
	inOutRay.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
	contribution = hitMaterial.Albedo;
}

static float Reflectance(float cosine, float refraction_index) {
	// Use Schlick's approximation for reflectance.
	auto r0 = (1 - refraction_index) / (1 + refraction_index);
	r0 = r0 * r0;
	return r0 + (1 - r0) * glm::pow((1 - cosine), 5);
}

void Renderer::Dielectric(const Scene& activeScene, Ray& inOutRay, const HitPayload& payload, glm::vec3& contribution, uint32_t& seed)
{
	const Material& hitMaterial = activeScene.GetMaterial(payload.ObjectIndex);

	bool front_face = dot(inOutRay.Direction, payload.WorldNormal) < 0;
	glm::vec3 correctNormal = front_face ? payload.WorldNormal : -payload.WorldNormal;

	contribution = glm::vec3(1.0f);
	float refactionIndex = front_face ? 1 / hitMaterial.RefactionIndex : hitMaterial.RefactionIndex;

	auto normalizedHitRayDirection = glm::normalize(inOutRay.Direction);
	float cos_theta = glm::min(dot(-normalizedHitRayDirection, correctNormal), 1.0f);
	float sin_theta = glm::sqrt(1.0 - cos_theta * cos_theta);

	bool cannot_refract = refactionIndex * sin_theta > 1.0;

	glm::vec3 direction;
	
	if (cannot_refract || Reflectance(cos_theta, refactionIndex) > Utils::Random::Fast::FloatRef(seed))
	{
		direction = reflect(normalizedHitRayDirection, correctNormal);
	}
	else
	{
		direction = glm::refract(normalizedHitRayDirection, correctNormal, refactionIndex);
	}

	inOutRay.Origin = payload.WorldPosition + correctNormal * -0.0001f;

	inOutRay.Direction = direction;
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	int PixedId = x + y * m_FinalImage->GetWidth();
	ray.Direction = m_ActiveCamera->GetRayDirections()[PixedId];
	
	glm::vec3 contribution(1.0f);

	int bounces = 50;
	uint32_t traceSeed = PixedId * m_FrameIndex;
	float attenuationPerBounce = 0.7;
	for (int i = 0; i < bounces; i++)
	{
		traceSeed += i;
		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f)
		{
			contribution *= glm::vec3(0.6f, 0.7f, 0.9f);
			break;
		}
		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];

		glm::vec3 contributionScatteredFromTrace(1.0f);
		switch (material.Type)
		{
		case Material::Type::Lambertian:
			Lambertian(*m_ActiveScene, ray, payload, contributionScatteredFromTrace, traceSeed);
			break;
		case Material::Type::Metalic:
			Metal(*m_ActiveScene, ray, payload, contributionScatteredFromTrace, traceSeed);
			break;
		case Material::Type::Dielectric:
			Dielectric(*m_ActiveScene, ray, payload, contributionScatteredFromTrace, traceSeed);
			break;
		}
		float attenuation = attenuationPerBounce;
		if (Material::Type::Dielectric == material.Type)
		{
			attenuation = 1.0f;
		}

		//contribution *= contributionScatteredFromTrace * attenuationPerBounce

		contribution *= contributionScatteredFromTrace * attenuation;
	}

	return glm::vec4(contribution, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{

	Scene::RayCastHit Hit = m_ActiveScene->RayCast(ray);

	if (Hit.ObjectIndex < 0)
		return Miss(ray);

	return ClosestHit(ray, Hit.HitDistance, Hit.ObjectIndex);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	Renderer::HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];

	glm::vec3 origin = ray.Origin - closestSphere.Position;
	payload.WorldPosition = origin + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);

	payload.WorldPosition += closestSphere.Position;

	return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
}
