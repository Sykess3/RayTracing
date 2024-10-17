#pragma once

#include "Popups/ApproveCancelPopup.h"
#include "Scene.h"

class Engine;

class CreateObjectPopup : public ApproveCancelPopup 
{
public:
	CreateObjectPopup(const std::string& title, const std::string& approve, const std::string& cancel, std::shared_ptr<Engine> Engine);

protected:


	void OnApprove() override;


	void OnCancel() override;


	void DrawContent() override;

private:
	std::shared_ptr<Engine> m_engine;
	Sphere m_editingObject;
};

