#pragma once
#include "GameEventListener.h"
#include "PushNotifications.h"
#include "CSGOStructs.hpp"
#include <vector>

typedef void(*EventCallback)(IGameEvent* pEvent);

class DeathEvent : CGameEventListener
{
protected:
	std::vector<EventCallback> Callbacks;
public:
	DeathEvent()
	{
		szEventName = XorStr("player_death");
		RegListener();
	}
	virtual void HandleGameEvent(IGameEvent *pEvent)
	{
		for (auto && fn : Callbacks)
			fn(pEvent);

		if (Interfaces::Engine()->GetPlayerForUserID(pEvent->GetInt(XorStr("userid"))) == Interfaces::Engine()->GetLocalPlayer())
		{
			if (Config.g_bAutoResetScore)
			{
				auto pLocal = C_CSPlayer::GetLocalPlayer();
				int Kills = pLocal->GetScoreboardKills();
				int Deaths = pLocal->GetScoreboardDeaths();

				if (Deaths > Kills)
					Interfaces::Engine()->ExecuteClientCmd(XorStr("say /rs"));
			}
		}

		player_info_t AttackerInfo;
		player_info_t VictimInfo;
		auto pVictim = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetPlayerForUserID(pEvent->GetInt(XorStr("userid")))));
		if (!pVictim) return;
		Interfaces::Engine()->GetPlayerInfo(Interfaces::Engine()->GetPlayerForUserID(pEvent->GetInt(XorStr("attacker"))), &AttackerInfo);
		Interfaces::Engine()->GetPlayerInfo(Interfaces::Engine()->GetPlayerForUserID(pEvent->GetInt(XorStr("userid"))), &VictimInfo);

		std::string WeaponName = pEvent->GetString(XorStr("weapon"));
		std::string Location(pVictim->GetLastPlaceName());

		std::string AttackerName = AttackerInfo.szName;
		std::string VictimName = VictimInfo.szName;

		std::string Message;

		if (!WeaponName.length() || WeaponName == "world") { Message = VictimName + XorStr(" killed himself");}
		else
		{
			if (Location.length())
				Message = AttackerName + XorStr(" killed ") + VictimName + XorStr(" using a ") + WeaponName + XorStr(" at ") + Location;
			else
				Message = AttackerName + XorStr(" killed ") + VictimName + XorStr(" using a ") + WeaponName;
		}
		PushNotifications::Push(Message);
	}
	virtual void RegisterHook(EventCallback NewFunc)
	{
		Callbacks.push_back(NewFunc);
	}
	~DeathEvent()
	{
		RemoveListener();
	}
};