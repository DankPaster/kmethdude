#include "AimAssist.h"
#include "Triggerbot.h"

int Aimbot::CurrentSpeed = 0;
int Aimbot::CurrentTarget = 0;
bool AimLock::WasOnEnemy = false;
QAngle LastAngle = QAngle(0, 0, 0);

float GetAngleDelta()
{
	QAngle ViewAngles;
	Interfaces::Engine()->GetViewAngles(ViewAngles);
	ViewAngles -= LastAngle;
	Utils::Clamp(ViewAngles);
	return Vector(ViewAngles.x, ViewAngles.y, ViewAngles.z).Length();
}

float GetRelativeFOVAngle(int PlayerID)
{
	if (!PlayerID) return 180.0f;
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	auto Target = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(PlayerID));
	QAngle TargetAngles = (Target->GetOrigin() - pLocal->GetOrigin()).Angle();
	QAngle ViewAngles;
	Interfaces::Engine()->GetViewAngles(ViewAngles);
	TargetAngles -= ViewAngles;
	Utils::Clamp(TargetAngles);
	return Vector(TargetAngles.x, TargetAngles.y, TargetAngles.z).Length();
}

void AimLock::LookAtPlayer(int PlayerID, SourceEngine::CUserCmd* pCmd)
{
	if (!PlayerID) return;
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	auto ActiveWeapon = pLocal->GetActiveWeapon();

	auto Target = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(PlayerID));
	SourceEngine::Vector AimPos = SourceEngine::Vector(0, 0, 0);
	if (Config.g_iAimbotTarget == 0)
		AimPos = Target->GetHeadCenterPos();
	else if (Config.g_iAimbotTarget == 1)
		AimPos = Target->GetBoneByName(XorStr("spine_3"));
	else if (Config.g_iAimbotTarget == 2)
		AimPos = Target->GetBoneByName(XorStr("spine_0"));

	pCmd->viewangles = (AimPos - pLocal->GetEyePos()).Angle();
	pCmd->viewangles -= RCS::GetCurrentPunchAngles();

	if (!Config.g_iViewMode)
		Interfaces::Engine()->SetViewAngles(pCmd->viewangles);
}

void AimLock::LookAtPoint(SourceEngine::Vector Point, SourceEngine::CUserCmd* pCmd)
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	auto ActiveWeapon = pLocal->GetActiveWeapon();

	SourceEngine::QAngle Angle = (Point - pLocal->GetEyePos()).Angle();
	Utils::Clamp(Angle);

	pCmd->viewangles = Angle;
	pCmd->viewangles -= RCS::GetCurrentPunchAngles();
}

void AimLock::LockOnTarget(int PlayerID, SourceEngine::CUserCmd* pCmd)
{
	auto ActiveWeapon = C_CSPlayer::GetLocalPlayer()->GetActiveWeapon();
	if (ActiveWeapon->IsKnife()) return;
	if (ActiveWeapon->IsSniper() && !(C_CSPlayer::GetLocalPlayer()->IsScoped())) return;
	LookAtPlayer(PlayerID, pCmd);
	SourceEngine::Interfaces::Engine()->SetViewAngles(pCmd->viewangles);
}

void AimLock::Think(SourceEngine::CUserCmd* pCmd)
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	int PlayerID = pLocal->InCross();
	if (PlayerID)
	{
		if (!WasOnEnemy && Utils::IsViableTarget(static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(pLocal->InCross()))))
			AimLock::LockOnTarget(pLocal->InCross(), pCmd);
	}

	if (!PlayerID)
		WasOnEnemy = false;
	else if (Utils::IsViableTarget(static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(PlayerID))))
		WasOnEnemy = true;
	else
		WasOnEnemy = false;
}

int Aimbot::GetOptimalTarget()
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	int CurrentTarget = 0;
	float LastFOV = 999999999999.0f;
	auto wpn_data = pLocal->GetActiveWeapon()->GetCSWpnData();

	for (int i = 1; i < 65; i++)
	{
		auto pEntity = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(i));
		if (Utils::IsViableTarget(pEntity))
		{
			SourceEngine::Vector LocalEyePos = pLocal->GetEyePos();
			SourceEngine::Vector EntityPos;
			if (Config.g_iAimbotTarget == 0)
				EntityPos = pEntity->GetHeadCenterPos();
			else if (Config.g_iAimbotTarget == 1)
				EntityPos = pEntity->GetBoneByName(XorStr("spine_3"));
			else if (Config.g_iAimbotTarget == 2)
				EntityPos = pEntity->GetBoneByName(XorStr("spine_0"));

			if (Utils::DoesViewRayHitEntity(LocalEyePos, EntityPos))
			{
				float CurrentFOV = GetRelativeFOVAngle(i);
				if (CurrentFOV < LastFOV && CurrentFOV < Config.g_fAimbotFOV)
				{
					CurrentTarget = i;
					LastFOV = CurrentFOV;
				}
			}
		}
	}

	return CurrentTarget;
}

void Aimbot::Think(SourceEngine::CUserCmd* pCmd)
{
	int CurrentTarget = GetOptimalTarget();
	if (!CurrentTarget) CurrentSpeed = GetAngleDelta();
	SourceEngine::Interfaces::Engine()->GetViewAngles(LastAngle);
	if (CurrentTarget)
	{
		Step(pCmd);
	}
}

void Aimbot::Step(SourceEngine::CUserCmd* pCmd)
{
	auto Target = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(CurrentTarget));
	if (!Target || !Target->IsAlive())
	{
		CurrentTarget = 0;
		return;
	}

	auto pLocal = C_CSPlayer::GetLocalPlayer();
	float AccelRate = 0.25f;
	float DecelRate = 0.25f;
	float SpeedPos = fmin(pow(GetRelativeFOVAngle(CurrentTarget) / 15.0f, 2), Config.g_fAimbotSpeed);

	if (CurrentSpeed > SpeedPos)
		CurrentSpeed -= DecelRate;
	else if (CurrentSpeed < SpeedPos)
		CurrentSpeed += AccelRate;

	QAngle TargetAngles = (Target->GetOrigin() - pLocal->GetOrigin()).Angle();
	QAngle ViewAngles;
	Interfaces::Engine()->GetViewAngles(ViewAngles);
	TargetAngles -= ViewAngles;
	Utils::Clamp(TargetAngles);
	Vector temp = TargetAngles.Direction();

	QAngle DeltaAngle = (temp * CurrentSpeed).Angle();
	Interfaces::Engine()->SetViewAngles(ViewAngles + DeltaAngle);
	pCmd->viewangles = ViewAngles + DeltaAngle;

	if (CurrentTarget == pLocal->InCross())
	{
		Triggerbot::Shoot(pCmd);
	}
}