#pragma once
#include "Walnut/Input/Input.h"
#include "Walnut/Layer.h"
#include "imgui.h"
#include "Engine.h"
#include <glm/gtc/type_ptr.hpp>

class EditorApp : public Walnut::Layer
{

public:
	EditorApp(std::shared_ptr<Engine> engine)
		:m_Engine(engine)
	{

	}

	virtual void OnUpdate(float ts) override
	{

	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_Engine->m_LastRenderTime);
		/*if (ImGui::Button("Render"))
		{
			m_Engine->m_Renderer->Render(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
		}*/

		ImGui::Checkbox("Accumulate", &m_Engine->m_Renderer->GetSettings().Accumulate);

		if (ImGui::Button("Reset"))
			m_Engine->m_Renderer->ResetFrameIndex();

		ImGui::End();

		ImGui::Begin("Scene");
		for (size_t i = 0; i < m_Engine->m_Scene->Spheres.size(); i++)
		{
			ImGui::PushID(i);

			Sphere& sphere = m_Engine->m_Scene->Spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
			ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Engine->m_Scene->Materials.size() - 1);

			ImGui::Separator();

			ImGui::PopID();
		}

		for (size_t i = 0; i < m_Engine->m_Scene->Materials.size(); i++)
		{
			ImGui::PushID(i);

			Material& material = m_Engine->m_Scene->Materials[i];
			ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			ImGui::Separator();

			ImGui::PopID();
		}

		ImGui::End();
	}
private:
	std::shared_ptr<Engine> m_Engine;
};