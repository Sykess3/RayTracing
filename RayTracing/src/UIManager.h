#pragma once
#include <string>
#include <map>
#include <memory>
#include <cassert>
#include "Walnut/Layer.h"

class IPopup
{
public:
	virtual void OnUIRender() {};
protected:
	std::string m_key;

	friend class UIManager;
};
class UIManager : public Walnut::Layer
{
private:
	static inline UIManager* s_Instance = nullptr;
public:
	static UIManager& Get() { return *s_Instance; }
	UIManager()
	{
		assert(s_Instance == nullptr);
		s_Instance = this;
	}
	void ShowPopup(const std::string& key, std::unique_ptr<IPopup> popup) 
	{
		popup->m_key = key;
		m_keyToPopup.emplace(key, std::move(popup));
	}
	virtual void OnUIRender()
	{
		for each (auto& KVp in m_keyToPopup)
		{
			KVp.second->OnUIRender();
		}
	}
	void HidePopup(const std::string& key) 
	{
		m_keyToPopup.erase(key);
	}
protected:
	std::map<std::string, std::unique_ptr<IPopup>> m_keyToPopup;
};

