#pragma once
#include "GameEventListener.h"
#include "PushNotifications.h"
#include "Resolver.h"
#include <vector>

typedef void(*EventCallback)(IGameEvent* pEvent);

class DamageEvent : CGameEventListener
{
protected:
	std::vector<EventCallback> Callbacks;
public:
	DamageEvent()
	{
		szEventName = XorStr("player_hurt");
		RegListener();
	}
	virtual void HandleGameEvent(IGameEvent *pEvent)
	{
		Resolver::DamageCallback(pEvent);
	}
	virtual void RegisterHook(EventCallback NewFunc)
	{
		Callbacks.push_back(NewFunc);
	}
	~DamageEvent()
	{
		RemoveListener();
	}
};

