#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Renderer.h"
#include "Camera.h"
#include <memory>
#include "Engine.h"
#include "Editor/EditorApp.h"

class ViewportLayer : public Walnut::Layer
{
public:

	ViewportLayer(std::shared_ptr<Engine> inEngine)
		: m_Engine(inEngine)
	{
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
	virtual void OnUpdate(float ts) override
	{
		if (m_Engine->m_Camera->OnUpdate(ts))
			m_Engine->m_Renderer->ResetFrameIndex();
	}

	virtual void OnUIRender() override 
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Engine->m_Renderer->GetFinalImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();

		m_Engine->Render(m_ViewportHeight, m_ViewportWidth);
	}

	std::shared_ptr<Engine> m_Engine;

	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	auto camera = std::make_shared<Camera>(45.0f, 0.1f, 100.0f);
	auto scene = std::make_shared<Scene>();
	auto renderer = std::make_shared<Renderer>();
	auto engine = std::make_shared<Engine>(camera, scene, renderer);

	app->PushLayer(std::make_shared<EditorApp>(engine));
	app->PushLayer(std::make_shared<ViewportLayer>(engine));

	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}