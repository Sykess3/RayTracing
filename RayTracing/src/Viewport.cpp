#include "Viewport.h"
#include "glm/gtx/string_cast.hpp"
#include "Walnut/Input/Input.h"
#include "imgui_internal.h"
#include "Walnut/Random.h"
#include <cassert>

static Viewport* s_Instance;

Viewport& Viewport::Get()
{
	return *s_Instance;
}

Viewport::Viewport(std::shared_ptr<Engine> inEngine)
	: m_Engine(inEngine)
{
	s_Instance = this;
	m_Engine->m_Scene->Materials.reserve(20);

	Material& pinkSphere = m_Engine->m_Scene->Materials.emplace_back();
	pinkSphere.Albedo = { 1.0f, 0.0f, 1.0f };
	pinkSphere.Fuzzy = 0.8f;
	pinkSphere.Type = Material::EType::Metalic;

	Material& blueSphere = m_Engine->m_Scene->Materials.emplace_back();
	blueSphere.Albedo = { 0.2f, 0.3f, 1.0f };
	pinkSphere.Fuzzy = 0.9f;
	pinkSphere.Type = Material::EType::Metalic;

	Material& blueSphere2 = m_Engine->m_Scene->Materials.emplace_back();
	blueSphere2.Albedo = { 0.f, 1.0f, 1.0f };
	blueSphere2.Fuzzy = 0.2f;
	blueSphere2.Type = Material::EType::Metalic;

	Material& blueSphere3 = m_Engine->m_Scene->Materials.emplace_back();
	blueSphere3.Albedo = { 1.f, 0.3f, 0.0f };
	blueSphere3.Fuzzy = 0.6f;
	blueSphere3.Type = Material::EType::Metalic;

	Material& blueSphere4 = m_Engine->m_Scene->Materials.emplace_back();
	blueSphere4.Albedo = { 1.f, 0.0f, 0.3f };
	blueSphere4.Fuzzy = 0.1f;
	blueSphere4.Type = Material::EType::Metalic;

	/*Material& TestSphere = m_Engine->m_Scene->Materials.emplace_back();
	TestSphere.Albedo = { 1.0f, 1.0f, 1.0f };
	TestSphere.RefactionIndex = 0.99f;
	TestSphere.Type = Material::EType::Dielectric;

	Material& TestSphere2 = m_Engine->m_Scene->Materials.emplace_back();
	TestSphere2.Albedo = { 1.0f, 1.0f, 1.0f };
	TestSphere2.RefactionIndex = 1.5f;
	TestSphere2.Type = Material::EType::Dielectric;

	Material& TestSphere3 = m_Engine->m_Scene->Materials.emplace_back();
	TestSphere3.Albedo = { 1.0f, 0.0f, 1.0f };
	TestSphere3.RefactionIndex = 0.7f;
	TestSphere3.Type = Material::EType::Dielectric;*/

	Material& orangeSphere = m_Engine->m_Scene->Materials.emplace_back();
	orangeSphere.Albedo = { 0.8f, 0.5f, 0.2f };
	pinkSphere.Type = Material::EType::Lambertian;

	Material& sphere2 = m_Engine->m_Scene->Materials.emplace_back();
	sphere2.Albedo = { 0.2f, 0.6f, 0.2f };
	sphere2.Type = Material::EType::Lambertian;

	Material& sphere3 = m_Engine->m_Scene->Materials.emplace_back();
	sphere3.Albedo = { 0.7f, 0.1f, 0.7f };
	sphere3.Type = Material::EType::Lambertian;

	/* {
		Sphere sphere;
		sphere.Position = { 0.0f, 0.0f, 0.0f };
		sphere.Radius = 1.0f;
		sphere.MaterialIndex = 0;
		m_Engine->m_Scene->Spheres.push_back(sphere);
	}*/

	/*{
		Sphere sphere;
		sphere.Position = { 2.0f, 0.0f, 0.0f };
		sphere.Radius = 1.0f;
		sphere.MaterialIndex = 2;
		m_Engine->m_Scene->Spheres.push_back(sphere);

		{
			Sphere sphere;
			sphere.Position = { 2.0f, 0.0f, 2.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 2;
			m_Engine->m_Scene->Spheres.push_back(sphere);
		}



		{
			Sphere sphere;
			sphere.Position = { -1.0f, 0.0f, -3.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 2;
			m_Engine->m_Scene->Spheres.push_back(sphere);
		}
	}*/

	{
	}

	Sphere sphere;
	sphere.Position = { 0.0f, -100.450f, 0.0f };
	sphere.Radius = 100.0f;
	auto it = std::find(m_Engine->m_Scene->Materials.begin(), m_Engine->m_Scene->Materials.end(), orangeSphere);
	sphere.MaterialIndex = std::distance(m_Engine->m_Scene->Materials.begin(), it);
	m_Engine->m_Scene->Spheres.push_back(sphere);

	for (int a = -3; a < 3; a++) {
		for (int b = -3; b < 3; b++) {
			glm::vec3 center(1.4f * (float)a + Walnut::Random::Float(), 0.f,  1.4f * (float)b + Walnut::Random::Float());
			if ((center - glm::vec3(4.f, 0.2f, 0.f)).length() > 0.9f)
			{
				Scene& scene = *m_Engine->m_Scene;
				Sphere& sphere = m_Engine->m_Scene->Spheres.emplace_back();
				sphere.Radius = Walnut::Random::Float() * 0.1f + 0.5f;
				sphere.MaterialIndex = Walnut::Random::UInt(0, m_Engine->m_Scene->Materials.size() - 1);
				sphere.Position = center;
			}
		}
	}
	for (Sphere& sphere : m_Engine->m_Scene->Spheres)
	{
		sphere.CalcBoundingBox();
		if (sphere.MaterialIndex < 0 || sphere.MaterialIndex >= m_Engine->m_Scene->Materials.size())
		{
			assert(0);
		}
	}

	m_Engine->m_Scene->ConstructBVH();
}

Viewport::~Viewport()
{
	s_Instance = nullptr;
}

void Viewport::OnUpdate(float ts)
{
	if (m_Engine->m_Camera->OnUpdate(ts))
		m_Engine->m_Renderer->ResetFrameIndex();
}

void Viewport::OnUIRender()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport");

	Width = ImGui::GetContentRegionAvail().x;
	Height = ImGui::GetContentRegionAvail().y;
 
	Min = glm::vec2(ImGui::GetWindowContentRegionMin().x, ImGui::GetWindowContentRegionMin().y) - glm::vec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY());
	Max = glm::vec2(ImGui::GetWindowContentRegionMax().x, ImGui::GetWindowContentRegionMax().y) - glm::vec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY());
	ContentPosition = glm::vec2(ImGui::GetWindowPos().x,ImGui::GetWindowPos().y ) + glm::vec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY());

	auto image = m_Engine->m_Renderer->GetFinalImage();
	if (image)
		ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
			ImVec2(0, 1), ImVec2(1, 0));

	Min += ContentPosition;
	Max += ContentPosition;

	ImGui::End();
	ImGui::PopStyleVar();

	m_Engine->Render(Height, Width);
}

glm::vec4 Viewport::GetClippedPosition(const glm::vec2& pos)
{
	glm::vec2 relativePos = pos - ContentPosition;
	auto size = Size();
	glm::vec2 result = { (relativePos.x / size.x) * 2.0f - 1.0f, 1.0f - (relativePos.y / size.y) * 2.0f };
	return { result.x, result.y, 1, 1 };
}

bool Viewport::IsWithin(const glm::vec2& pos)
{
	Interval xInterval(Min.x, Max.x);
	Interval yInterval(Min.y, Max.y);

	return xInterval.Contains(pos.x) && yInterval.Contains(pos.y);
}
