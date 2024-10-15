#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Editor/EditorApp.h"
#include "Engine.h"
#include "Viewport.h"
#include "Scene.h"
#include <memory>



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
	app->PushLayer(std::make_shared<Viewport>(engine));

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