#pragma once
#include "Popups/ApproveCancelPopup.h"
#include <vector>
#include <string>

class Engine;

class LoadScenePopup : public ApproveCancelPopup
{
public:
	LoadScenePopup(const std::string& title, const std::string& approve, const std::string& cancel, std::shared_ptr<Engine> engine);
protected:
	virtual void OnApprove() override;
	virtual void OnCancel() override;
	virtual void DrawContent() override;
private:
	std::vector<std::string> m_locatedFiles;
	std::shared_ptr<Engine> m_engine;
	int m_selectedFileIndex;
};

