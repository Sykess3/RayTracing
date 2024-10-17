#pragma once
#include "Walnut/Layer.h"
#include <string>
#include "imgui.h"
#include "UIManager.h"


class ApproveCancelPopup : public IPopup
{
public:
	ApproveCancelPopup(const std::string& title, const std::string& approve, const std::string& cancel);
	virtual void OnUIRender();
protected:
	virtual void OnApprove() {}
	virtual void OnCancel() {}
	virtual void DrawContent() {}
private:

	std::string m_approve;
	std::string m_cancel;
	std::string m_title;
};

