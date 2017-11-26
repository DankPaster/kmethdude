#include "NoRecoil.h"

SourceEngine::Vector LegitPunchAngles = SourceEngine::Vector(0, 0, 0);
SourceEngine::Vector DeltaLegitPunchAngles = SourceEngine::Vector(0, 0, 0);
SourceEngine::Vector CurrentPunchAngles = SourceEngine::Vector(0, 0, 0);

SourceEngine::Vector RCS::GetLegitPunchAngles() { return LegitPunchAngles; }
SourceEngine::Vector RCS::GetDeltaLegitPunchAngles() { return DeltaLegitPunchAngles; }
SourceEngine::Vector RCS::GetCurrentPunchAngles() { return CurrentPunchAngles; }

// Fail attempt at making legit norecoil, do not use

void RCS::UpdatePunchAngles()
{	
	static auto weapon_recoil_scale = SourceEngine::Interfaces::CVar()->FindVar(XorStr("weapon_recoil_scale"));
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	CurrentPunchAngles = *(pLocal->AimPunch()) * weapon_recoil_scale->GetFloat();
	float AccuracyFactor = Config.g_iRCSAccuracy / 100.0f;
	DeltaLegitPunchAngles = LegitPunchAngles;
	LegitPunchAngles = (CurrentPunchAngles * AccuracyFactor + Vector(-1, 0, 0) * CurrentPunchAngles.Length() * (1.0f - AccuracyFactor) * 0.84f) * pow(AccuracyFactor, 0.5);
	DeltaLegitPunchAngles = LegitPunchAngles - DeltaLegitPunchAngles;
	DeltaLegitPunchAngles *= weapon_recoil_scale->GetFloat();
}