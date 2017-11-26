#include "Resolver.h"

using namespace SourceEngine;

std::map<int, ResolveData> Resolver::PlayerData;
extern int GlobalCmdCounter;

// Literal garbage, don't use.

int GetLowsestDenominator(int a, int b)
{
	for (int i = 2; i <= min(a, b); i++)
		if ((a % i == 0) && (b % i == 0)) { return i; }
	return 1;
}

int GetServerSideID(int EntityIndex)
{
	player_info_t pInfo;
	Interfaces::Engine()->GetPlayerInfo(EntityIndex, &pInfo);
	return pInfo.userId;
}

void YawStep(QAngle& StepAngle)
{
	if (Config.g_iResolver_AngleStep == 0)
	{
		if (fmod(StepAngle.y, 30))
			StepAngle.y = 0;
		if (Config.g_iResolver_YawMode == 0)
			StepAngle.y -= 30.0f;
		else
		{
			if (GlobalCmdCounter % 2)
				StepAngle.y += 180.0f;
			else
				StepAngle.y -= 30.0f;
		}
	}
	else if (Config.g_iResolver_AngleStep == 1)
	{
		if (fmod(StepAngle.y, 45))
			StepAngle.y = 0;
		if (Config.g_iResolver_YawMode == 0)
			StepAngle.y -= 45.0f;
		else
		{
			if (GlobalCmdCounter % 2)
				StepAngle.y += 180.0f;
			else
				StepAngle.y -= 45.0f;
		}
	}
	else if (Config.g_iResolver_AngleStep == 2)
	{
		if (fmod(StepAngle.y, 90))
			StepAngle.y = 0;
		if (Config.g_iResolver_YawMode == 0)
			StepAngle.y -= 90.0f;
		else
		{
			if (GlobalCmdCounter % 2)
				StepAngle.y += 180.0f;
			else
				StepAngle.y -= 90.0f;
		}
	}
}

void PitchStep(QAngle& StepAngle)
{
	if (Config.g_iResolver_PitchMode == 0)
	{
		if (StepAngle.x < 0)
		{
			YawStep(StepAngle);
			StepAngle.x = 89.0f;
		}
		else
			StepAngle.x = -45.0f;
	}
	else if (Config.g_iResolver_PitchMode == 1)
	{
		if (StepAngle.x < 0)
		{
			YawStep(StepAngle);
			StepAngle.x = 89.0f;
		}
		else if (StepAngle.x == 0)
			StepAngle.x = -89.0f;
		else if (StepAngle.x > 0)
			StepAngle.x = 0.0f;
	}
	else if (Config.g_iResolver_PitchMode == 2)
	{
		YawStep(StepAngle);
		StepAngle.x = 89.0f;
	}
}

void SingleStep(QAngle& StepAngle)
{
	PitchStep(StepAngle);
	Utils::Clamp(StepAngle);
}

void Resolver::AddShot(int EntityIndex)
{
	int ID = GetServerSideID(EntityIndex);
	PlayerData[ID].MissedShots++;
	SingleStep(PlayerData[ID].LastAttemptedAngles);
}

void Resolver::DamageCallback(IGameEvent* pEvent)
{
	if (!Config.g_bResolve) return;
	if (!pEvent) return;

	bool IsHeadshot = (pEvent->GetInt(XorStr("hitgroup")) == 1);

	int EntityServerID = pEvent->GetInt(XorStr("userID"));
	int AttackerServerID = pEvent->GetInt(XorStr("attacker"));
	int AttackerIndex = Interfaces::Engine()->GetPlayerForUserID(AttackerServerID);
	if (AttackerIndex != Interfaces::Engine()->GetLocalPlayer()) return;
	int EntityIndex = Interfaces::Engine()->GetPlayerForUserID(EntityServerID);
	if (EntityIndex == Interfaces::Engine()->GetLocalPlayer()) return;

	auto CurrentPlayerData = PlayerData.find(EntityServerID);
	if (CurrentPlayerData == PlayerData.end() && EntityIndex != Interfaces::Engine()->GetLocalPlayer()) { PlayerData[EntityServerID] = ResolveData(); }

	if (IsHeadshot)
	{
		PlayerData[EntityServerID].LastAnglesHit = PlayerData[EntityServerID].LastAttemptedAngles;
		PlayerData[EntityServerID].KilledByCount = 0;
		PlayerData[EntityServerID].MissedShots = 0;
	}
}

void Resolver::LowerBodyYawFix(int CurrentTarget)
{
	IClientEntity* pEntity = Interfaces::EntityList()->GetClientEntity(CurrentTarget);
	if (!pEntity) return;
	if (pEntity->GetClientClass()->m_ClassID != SourceEngine::EClassIds::CCSPlayer) return;
	auto pCSPlayer = static_cast<C_CSPlayer*>(pEntity);
	if (!pCSPlayer->IsAlive() || !pCSPlayer->GetActiveWeapon()) return;
	if (pCSPlayer == C_CSPlayer::GetLocalPlayer()) return;

	auto EyeAngles = pCSPlayer->EyeAngles();
	if (pCSPlayer->GetVelocity().Length() > 1 && (pCSPlayer->GetFlags() & (int)SourceEngine::EntityFlags::FL_ONGROUND))
		EyeAngles->y = pCSPlayer->GetLowerBodyYawTarget();

	// PoseParam when speed > 80.0f
}

void Resolver::Resolve(int CurrentTarget)
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	auto pCSPlayer = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(CurrentTarget));
	if (!pCSPlayer) return;
	if (!pCSPlayer->IsAlive()) return;
	if (!pCSPlayer->GetActiveWeapon()) return;
	if (pCSPlayer->GetTeamNum() == pLocal->GetTeamNum()) return;

	int EntityServerID = GetServerSideID(CurrentTarget);
	if (!CurrentTarget) return;

	auto CurrentPlayerData = PlayerData.find(EntityServerID);
	if (CurrentPlayerData == PlayerData.end()) { PlayerData[EntityServerID] = ResolveData(); }

	auto EyeAngles = pCSPlayer->EyeAngles();
	QAngle OriginalAngle = *EyeAngles;

	Vector VelocityDir = pCSPlayer->GetVelocity();
	if (VelocityDir.IsZero()) VelocityDir = Vector(0, 0, 0.5);
	QAngle Velocity = VelocityDir.Normalized().Angle();
	Velocity.x = 0;
	Velocity.z = 0;

	QAngle AtTarget = (pLocal->GetOrigin() - pCSPlayer->GetOrigin()).Normalized().Angle();

	if (PlayerData[EntityServerID].MissedShots > 9)
		PlayerData[EntityServerID] = ResolveData();
	if (PlayerData[EntityServerID].MissedShots > 5)
		PlayerData[EntityServerID].LastAnglesHit.Invalidate();

	if (PlayerData[EntityServerID].LastAnglesHit.IsValid())
	{
		if (Config.g_iResolver_AngleSnapping == 0)
			*EyeAngles = PlayerData[EntityServerID].LastAnglesHit + QAngle(0, OriginalAngle.y, 0);
		else if (Config.g_iResolver_AngleSnapping == 1)
			*EyeAngles = PlayerData[EntityServerID].LastAnglesHit;
		else if (Config.g_iResolver_AngleSnapping == 2)
			*EyeAngles = PlayerData[EntityServerID].LastAnglesHit + QAngle(0, Velocity.y, 0);
		else if (Config.g_iResolver_AngleSnapping == 3)
			*EyeAngles = PlayerData[EntityServerID].LastAnglesHit + QAngle(0, AtTarget.y, 0);
	}
	else
	{
		bool Rotate = !(GlobalCmdCounter % 16);
		if (Rotate)
			SingleStep(PlayerData[EntityServerID].LastAttemptedAngles);

		if (Config.g_iResolver_AngleSnapping == 0)
			*EyeAngles = PlayerData[EntityServerID].LastAttemptedAngles + QAngle(0, OriginalAngle.y, 0);
		else if (Config.g_iResolver_AngleSnapping == 1)
			*EyeAngles = PlayerData[EntityServerID].LastAttemptedAngles;
		else if (Config.g_iResolver_AngleSnapping == 2)
			*EyeAngles = PlayerData[EntityServerID].LastAttemptedAngles + QAngle(0, Velocity.y, 0);
		else if (Config.g_iResolver_AngleSnapping == 3)
			*EyeAngles = PlayerData[EntityServerID].LastAttemptedAngles + QAngle(0, AtTarget.y, 0);
	}

	Utils::Clamp(*EyeAngles);
	PlayerData[EntityServerID].LastSetAngles = QAngle(EyeAngles->x, EyeAngles->y, 0);
}