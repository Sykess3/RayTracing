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

class EditorApp : public Walnut::Layer
{

public:
	EditorApp(std::shared_ptr<Engine> engine)
		:m_Engine(engine)
	{
		OnViewport_LeftMouseClick.append(std::bind(&EditorApp::ViewportLeftMouseClick, this));
	}

	virtual void OnUpdate(float ts) override
	{
		if (Walnut::Input::IsMouseButtonDown(Walnut::MouseButton::Left))
		{
			OnViewport_LeftMouseClick();
		}
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_Engine->m_LastRenderTime);

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

	void ViewportLeftMouseClick() 
	{
		ImGuiIO& IO = ImGui::GetIO();
		glm::vec2 input = { IO.MousePos.x, IO.MousePos.y };
	
		Viewport& viewport = Viewport::Get();
		if (viewport.IsWithin(input))
		{
			 glm::vec4 mouseClippedPos = viewport.GetClippedPosition(input);
			
			 glm::vec4 target = m_Engine->m_Camera->GetInverseProjection() * mouseClippedPos;
			 glm::vec3 rayDirection = glm::vec3(m_Engine->m_Camera->GetInverseView() * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));
			 Ray ray;
			 ray.Direction = rayDirection;
			 ray.Origin = m_Engine->m_Camera->GetPosition();
			 auto hit = m_Engine->m_Scene->RayCast(ray);
			 std::cout << hit.ObjectIndex << std::endl;
		}
	}
public:
	eventpp::CallbackList<void()> OnViewport_LeftMouseClick;

private:
	std::shared_ptr<Engine> m_Engine;
};