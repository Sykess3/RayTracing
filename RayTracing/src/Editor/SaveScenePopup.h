#pragma once
#include "Popups/ApproveCancelPopup.h"

class Engine;

class SaveScenePopup : public ApproveCancelPopup
{
public:
	SaveScenePopup(const std::string& title, const std::string& approve, const std::string& cancel, std::shared_ptr<Engine> engine);

	virtual void OnApprove() override;
	virtual void OnCancel() override;
	virtual void DrawContent() override;

private:
	std::shared_ptr<Engine> m_engine;
	char m_buf[256];
};

