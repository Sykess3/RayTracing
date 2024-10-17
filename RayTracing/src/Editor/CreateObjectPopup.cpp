#include "CreateObjectPopup.h"
#include "EditorApp.h"

CreateObjectPopup::CreateObjectPopup(const std::string& title, const std::string& approve, const std::string& cancel, std::shared_ptr<Engine> Engine)
	: ApproveCancelPopup(title, approve, cancel),
	m_engine(Engine),
	m_editingObject({})
{

}

void CreateObjectPopup::OnApprove()
{
	m_engine->m_Scene->Spheres.push_back(m_editingObject);
}

void CreateObjectPopup::OnCancel()
{
	
}

void CreateObjectPopup::DrawContent()
{
	bool bDirty;
	EditorApp::DrawDetailsOfObject(m_editingObject, bDirty, m_engine->m_Scene);
}
