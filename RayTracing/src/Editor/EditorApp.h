#pragma once
#include "Walnut/Input/Input.h"
#include "Walnut/Layer.h"
#include "imgui.h"
#include "Engine.h"
#include "eventpp/callbacklist.h"
#include "Walnut/Input/KeyCodes.h"
#include "glm/gtx/string_cast.hpp"
#include "Viewport.h"

#include <glm/gtc/type_ptr.hpp>
#include <memory>

class EditorApp : public Walnut::Layer
{

public:
	EditorApp(std::shared_ptr<Engine> engine)
		:m_Engine(engine)
	{
		OnViewport_LeftMouseClick.append(std::bind(&EditorApp::ViewportLeftMouseClick, this, std::placeholders::_1));
	}

	virtual void OnUpdate(float ts) override
	{
		if (Walnut::Input::IsMouseButtonDown(Walnut::MouseButton::Left))
		{
			ImGuiIO& IO = ImGui::GetIO();
			glm::vec2 input = { IO.MousePos.x, IO.MousePos.y };

			Viewport& viewport = Viewport::Get();
			if (viewport.IsWithin(input)) 
			{
				OnViewport_LeftMouseClick(input);
			}
		}
	}

	virtual void OnUIRender() override
	{
		ImGui::ShowDemoWindow();
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_Engine->m_LastRenderTime);

		ImGui::Checkbox("Accumulate", &m_Engine->m_Renderer->GetSettings().Accumulate);

		if (ImGui::Button("Reset"))
			m_Engine->m_Renderer->ResetFrameIndex();

		ImGui::End();

		ImGui::Begin("Scene");
		bool bDirty = false;
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
		if (m_SelectedObjectIndex >= 0)
		{
			auto& scene = m_Engine->m_Scene;
			Sphere& selectedObject = scene->GetObject_Mutable(m_SelectedObjectIndex);
			Material& selectedMaterial = scene->GetMaterial_Mutable(m_SelectedObjectIndex);
			if (ImGui::TreeNodeEx("Sphere", flags)) 
			{
				bDirty |= ImGui::DragFloat3("Position", glm::value_ptr(selectedObject.Position), 0.1f);
				bDirty |= ImGui::DragFloat("Radius", &selectedObject.Radius, 0.1f);
				bDirty |= ImGui::DragInt("Material", &selectedObject.MaterialIndex, 1.0f, 0, (int)m_Engine->m_Scene->Materials.size() - 1);
				ImGui::TreePop();
			}
			if (ImGui::TreeNodeEx("Material", flags))
			{
				{
					const std::array<const char*, Material::ETypeCount>& enumChars = GetAllArrChars<Material::EType, Material::ETypeCount>();
					int currentType = static_cast<int>(selectedMaterial.Type);
					if (ImGui::Combo("Material Type", &currentType, enumChars.data(), enumChars.size()))
					{
						bDirty |= true;
						selectedMaterial.Type = static_cast<Material::EType>(currentType);
					}

				}
				bDirty |= ImGui::ColorEdit3("Albedo", glm::value_ptr(selectedMaterial.Albedo));
				switch (selectedMaterial.Type)
				{
					case Material::EType::Dielectric:
					{
						bDirty |= ImGui::DragFloat("Refraction Index", &selectedMaterial.RefactionIndex, 0.001f);
						break;
					}
					case Material::EType::Lambertian:
					{
						break;
					}
					case Material::EType::Metalic:
					{
						bDirty |= ImGui::DragFloat("Fuzzy", &selectedMaterial.Fuzzy, 0.001f);

						break;
					}
				}
				ImGui::TreePop();
			}
		}
		if (bDirty)
		{
			m_Engine->m_Renderer->ResetFrameIndex();
		}

		ImGui::End();
	}

	void ViewportLeftMouseClick(const glm::vec2& input)
	{
		Viewport& viewport = Viewport::Get();
		glm::vec4 mouseClippedPos = viewport.GetClippedPosition(input);

		glm::vec4 target = m_Engine->m_Camera->GetInverseProjection() * mouseClippedPos;
		glm::vec3 rayDirection = glm::vec3(m_Engine->m_Camera->GetInverseView() * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));
		Ray ray;
		ray.Direction = rayDirection;
		ray.Origin = m_Engine->m_Camera->GetPosition();
		auto hit = m_Engine->m_Scene->RayCast(ray);
		m_SelectedObjectIndex = hit.ObjectIndex;
	}
public:
	eventpp::CallbackList<void(const glm::vec2&)> OnViewport_LeftMouseClick;

private:
	std::shared_ptr<Engine> m_Engine;
	int m_SelectedObjectIndex = -1;
};