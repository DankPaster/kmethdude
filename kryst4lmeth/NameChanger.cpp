#include "NameChanger.h"
#include <iostream>

bool Unlocked = false;
bool flip = true;
bool temp = false;
int EntIndex = 0;

void NullCallback(SourceEngine::IConVar *var, const char *pOldValue, float flOldValue) { return; }
SourceEngine::ConVar* cName;
std::string NameChanger::OriginalName;

// Great fucking meme, gg

void NameChanger::UnlockCVar()
{
	if (!cName)
		cName = SourceEngine::Interfaces::CVar()->FindVar(XorStr("name"));
	OriginalName = cName->GetString();
	for (int i = 0; i < cName->m_pParent->m_fnChangeCallbacks.Count(); i++)
		cName->m_pParent->m_fnChangeCallbacks[i] = (SourceEngine::FnChangeCallback_t)NullCallback;
	cName->SetValue(XorStr("\n­­­"));
	Unlocked = true;
}

void NameChanger::AimwareNameSpam()
{
	if (!(SourceEngine::Interfaces::Engine()->IsConnected() && SourceEngine::Interfaces::Engine()->IsInGame())) return;
	if (!Unlocked) { UnlockCVar(); }

	if (flip)
		cName->SetValue(XorStr(u8"�AIMWARE.net"));
	else
		cName->SetValue(XorStr(u8"឴AIMWARE.net"));

	flip = !flip;
}

void NameChanger::InterwebzNameSpam()
{
	if (!(SourceEngine::Interfaces::Engine()->IsConnected() && SourceEngine::Interfaces::Engine()->IsInGame())) return;
	if (!Unlocked) { UnlockCVar(); }

	if (flip)
		cName->SetValue(XorStr("-INTERWEBZ"));
	else
		cName->SetValue(XorStr("INTERWEBZ-"));

	flip = !flip;
}

void NameChanger::NoNameSpam()
{
	if (!(SourceEngine::Interfaces::Engine()->IsConnected() && SourceEngine::Interfaces::Engine()->IsInGame())) return;
	if (!Unlocked) { UnlockCVar(); }

	cName->SetValue(XorStr("\n­­­"));
}

void NameChanger::RestoreName()
{
	if (!(SourceEngine::Interfaces::Engine()->IsConnected() && SourceEngine::Interfaces::Engine()->IsInGame())) return;
	if (!Unlocked) { UnlockCVar(); }

	cName->SetValue(OriginalName.c_str());
}

void NameChanger::RandomName()
{
	if (!(SourceEngine::Interfaces::Engine()->IsConnected() && SourceEngine::Interfaces::Engine()->IsInGame())) return;
	if (!Unlocked) { UnlockCVar(); return; }

	int NumPlayers = 0;
	std::vector<int> Indexes;
	for (int i = 1; i < SourceEngine::Interfaces::Engine()->GetMaxClients(); i++)
	{
		if (!C_CSPlayer::GetLocalPlayer()) break;
		if (i == SourceEngine::Interfaces::Engine()->GetLocalPlayer()) continue;
		auto pEntity = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(i));
		if (!pEntity) continue;
		if (pEntity->GetClientClass()->m_ClassID == SourceEngine::EClassIds::CCSPlayer)
			Indexes.push_back(i);
	}
	EntIndex = Indexes[rand() % Indexes.size()];
	player_info_t pInfo;
	Interfaces::Engine()->GetPlayerInfo(EntIndex, &pInfo);
	std::string NewName = pInfo.szName;
	NewName += XorStr("\t");
	cName->SetValue(NewName.c_str());
}

void NameChanger::AutoNameStealer()
{
	if (!(SourceEngine::Interfaces::Engine()->IsConnected() && SourceEngine::Interfaces::Engine()->IsInGame())) return;
	if (!Unlocked) { UnlockCVar(); return; }
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	if (!pLocal || (pLocal->GetTeamNum() != 1 && pLocal->GetTeamNum() != 2))
	{
		temp = false;
		return;
	}
	if (!temp)
	{
		cName->SetValue(XorStr("\n­­­"));
		temp = true;
		return;
	}
	else
		RandomName();
}
