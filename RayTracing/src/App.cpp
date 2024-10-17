#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Editor/EditorApp.h"
#include "Engine.h"
#include "Viewport.h"
#include "Scene.h"
#include <memory>
#include "Camera.h"
#include "AssetManager.h"
#include "Editor/SaveScenePopup.h"
#include "Editor/LoadScenePopup.h"
#include "UIManager.h"


Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	auto camera = std::make_shared<Camera>(45.0f, 0.1f, 100.0f);
	auto scene = std::make_shared<Scene>();
	auto renderer = std::make_shared<Renderer>();
	auto assetManager = std::make_shared<AssetManager>();
	auto engine = std::make_shared<Engine>(camera, scene, renderer, assetManager);

	app->PushLayer<UIManager>();
	app->PushLayer(std::make_shared<EditorApp>(engine));
	app->PushLayer(std::make_shared<Viewport>(engine));

	app->SetMenubarCallback([app, engine]()
		{
			if (ImGui::BeginMenu("File"))
			{
				UIManager& uiManager = UIManager::Get();
				if (ImGui::MenuItem("Save scene")) 
				{
					uiManager.ShowPopup("SaveScene", std::make_unique<SaveScenePopup>("Save scene", "Save", "Cancel", engine));
				}
				else if (ImGui::MenuItem("Load scene")) 
				{
					uiManager.ShowPopup("LoadScene", std::make_unique<LoadScenePopup>("Load scene", "Load", "Cancel", engine));
					//engine->m_AssetManager->LoadJSON(engine->m_Scene, "Content\\Scenes\\testfile.json");

					//engine->m_Renderer->ResetFrameIndex();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}