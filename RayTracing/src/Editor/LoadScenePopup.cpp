#include "LoadScenePopup.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include "Engine.h"
#include <string>

namespace fs = std::filesystem;

std::vector<std::string> GetJsonFiles(const std::string& directoryPath) {
	std::vector<std::string> jsonFiles;

	// Check if the directory exists
	if (fs::exists(directoryPath) && fs::is_directory(directoryPath)) {
		// Iterate through the directory
		for (const auto& entry : fs::directory_iterator(directoryPath)) {
			// Check if it's a regular file
			if (fs::is_regular_file(entry)) {
				// Get the file path
				const auto& filePath = entry.path();

				// Check if the file has a .json extension
				if (filePath.extension() == ".json") {
					// Store the file name in the vector
					jsonFiles.push_back(filePath.stem().string());
				}
			}
		}
	}
	else {
		std::cerr << "Directory does not exist or is not a directory: " << directoryPath << std::endl;
	}

	return jsonFiles;
}

LoadScenePopup::LoadScenePopup(const std::string& title, const std::string& approve, const std::string& cancel, std::shared_ptr<Engine> engine)
	:ApproveCancelPopup(title, approve, cancel),
	m_engine(engine),
	m_locatedFiles({}),
	m_selectedFileIndex(0)
{
	m_locatedFiles = GetJsonFiles("Content\\Scenes\\");
}

void LoadScenePopup::OnApprove()
{
	m_engine->m_AssetManager->LoadJSON(m_engine->m_Scene, std::string("Content\\Scenes\\") + m_locatedFiles[m_selectedFileIndex] + std::string(".json"));
	m_engine->m_Renderer->ResetFrameIndex();
}

void LoadScenePopup::OnCancel()
{

}

bool ItemGetter(void* data, int index, const char** out_text) {
	const std::vector<std::string>* items = static_cast<const std::vector<std::string>*>(data);
	if (index < 0 || index >= items->size()) {
		return false; // Index out of range
	}
	*out_text = (*items)[index].c_str(); // Assign the item text
	return true;
}


void LoadScenePopup::DrawContent()
{
	ImGui::Combo("File", &m_selectedFileIndex, ItemGetter, &m_locatedFiles, m_locatedFiles.size());
}
