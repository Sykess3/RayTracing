#include "Editor/SaveScenePopup.h"
#include "Engine.h"

SaveScenePopup::SaveScenePopup(const std::string& title, const std::string& approve, const std::string& cancel, std::shared_ptr<Engine> engine)
	: ApproveCancelPopup(title, approve, cancel),
	m_engine(engine),
	m_buf("")
{

}

void SaveScenePopup::OnApprove()
{
	std::string path = std::string("Content\\Scenes\\") + std::string(m_buf) + std::string(".json");
	m_engine->m_AssetManager->SaveJSON(m_engine->m_Scene, path);

}

void SaveScenePopup::OnCancel()
{

}

void SaveScenePopup::DrawContent()
{
	ImGui::Text("Enter file name:");

	ImGui::InputText("##filename", m_buf, sizeof(m_buf));

	const char* items[] = { "Text", "Binary" };
	static int currentItem = 0; // This will store the current selection

	ImGui::Combo("Select Format", &currentItem, items, IM_ARRAYSIZE(items));
}
