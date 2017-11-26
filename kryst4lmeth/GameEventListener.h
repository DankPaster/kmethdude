#pragma once

#include "SourceEngine\SDK.hpp"
#include "XorStr.hpp"
#include <iostream>

using namespace SourceEngine;

class CGameEventListener : public IGameEventListener2 {
protected:
	std::string szEventName;
public:
	CGameEventListener() : szEventName("") {}
	CGameEventListener(const std::string& szGameEventName) : szEventName(szGameEventName)
	{
		if (!this->RegListener())
			std::cout << XorStr("CGameEventListener::RegListener() failed");
	}

	~CGameEventListener()
	{
		if (szEventName.length()) RemoveListener();
	}

	virtual void HandleGameEvent(IGameEvent *pEvent) {}

	virtual int IndicateEventHandling(void)
	{
		return CEL_PROCEED_EVENT_HANDLING;
	}

	virtual bool RegListener(void)
	{
		if (!SourceEngine::Interfaces::GameEventManager())
			return false;

		if (!this->szEventName.length())
			return false;

		return SourceEngine::Interfaces::GameEventManager()->AddListener(this, this->szEventName.c_str(), false);
	}
	virtual void RemoveListener()
	{
		if (!SourceEngine::Interfaces::GameEventManager())
			return;

		SourceEngine::Interfaces::GameEventManager()->RemoveListener(this);
	}
};

