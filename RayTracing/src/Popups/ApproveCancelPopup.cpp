#include "ApproveCancelPopup.h"
#include "UIManager.h"

ApproveCancelPopup::ApproveCancelPopup(const std::string& title, const std::string& approve, const std::string& cancel)
	: m_approve(approve),
	m_cancel(cancel),
	m_title(title)
{
}

void ApproveCancelPopup::OnUIRender()
{
	ImGui::OpenPopup(m_title.data());

	if (ImGui::BeginPopupModal(m_title.data(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		DrawContent();

		if (ImGui::Button(m_approve.data())) {
			OnApprove();
			ImGui::CloseCurrentPopup();
			UIManager& Manager = UIManager::Get();
			Manager.HidePopup(m_key);
		}

		ImGui::SameLine();
		if (ImGui::Button(m_cancel.data())) {
			OnCancel();
			ImGui::CloseCurrentPopup();
			UIManager& Manager = UIManager::Get();
			Manager.HidePopup(m_key);
		}

		ImGui::EndPopup();
	}
}
