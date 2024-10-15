#include "Viewport.h"
#include "glm/gtx/string_cast.hpp"
#include "Walnut/Input/Input.h"
#include "imgui_internal.h"

static Viewport* s_Instance;

Viewport& Viewport::Get()
{
	return *s_Instance;
}

Viewport::Viewport(std::shared_ptr<Engine> inEngine)
	: m_Engine(inEngine)
{

	s_Instance = this;
	Material& pinkSphere = m_Engine->m_Scene->Materials.emplace_back();
	pinkSphere.Albedo = { 1.0f, 0.0f, 1.0f };
	pinkSphere.Fuzzy = 0.1f;
	pinkSphere.Type = Material::Type::Metalic;

	Material& blueSphere = m_Engine->m_Scene->Materials.emplace_back();
	blueSphere.Albedo = { 0.2f, 0.3f, 1.0f };
	pinkSphere.Fuzzy = 0.1f;
	pinkSphere.Type = Material::Type::Metalic;

	Material& TestSphere = m_Engine->m_Scene->Materials.emplace_back();
	TestSphere.Albedo = { 1.0f, 1.0f, 1.0f };
	TestSphere.RefactionIndex = 0.99f;
	TestSphere.Type = Material::Type::Dielectric;

	Material& orangeSphere = m_Engine->m_Scene->Materials.emplace_back();
	orangeSphere.Albedo = { 0.8f, 0.5f, 0.2f };
	pinkSphere.Type = Material::Type::Lambertian;

	{
		Sphere sphere;
		sphere.Position = { 0.0f, 0.0f, 0.0f };
		sphere.Radius = 1.0f;
		sphere.MaterialIndex = 0;
		m_Engine->m_Scene->Spheres.push_back(sphere);
	}

	{
		Sphere sphere;
		sphere.Position = { 2.0f, 0.0f, 0.0f };
		sphere.Radius = 1.0f;
		sphere.MaterialIndex = 2;
		m_Engine->m_Scene->Spheres.push_back(sphere);
	}

	{
		Sphere sphere;
		sphere.Position = { 0.0f, -101.0f, 0.0f };
		sphere.Radius = 100.0f;
		sphere.MaterialIndex = 3;
		m_Engine->m_Scene->Spheres.push_back(sphere);
	}

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
