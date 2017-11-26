#include "Triggerbot.h"
#include "AutoShoot.h"

void Triggerbot::Shoot(SourceEngine::CUserCmd* pCmd)
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	auto ActiveWeapon = pLocal->GetActiveWeapon();
	if (!ActiveWeapon) return;
	if (!ActiveWeapon->CanFire()) return;
	if (ActiveWeapon->IsSniper())
		if (!pLocal->IsScoped()) return;
	if (ActiveWeapon->IsKnife())
	{
		Knife(pCmd);
		return;
	}
	pCmd->buttons |= IN_ATTACK;
}

void Triggerbot::Knife(SourceEngine::CUserCmd* pCmd, int ID)
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	int CrosshairID;
	if (!ID)
		CrosshairID = pLocal->InCross();
	else
		CrosshairID = ID;
	if (!CrosshairID) return;
	auto pEntity = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(CrosshairID));
	if (!Utils::IsViableTarget(pEntity)) return;

	float Distance = (pEntity->GetEyePos() - pLocal->GetEyePos()).Length();
	if (Distance < 85.6f)
	{
		SourceEngine::QAngle ClientAngles;
		SourceEngine::Interfaces::Engine()->GetViewAngles(ClientAngles);

		SourceEngine::Vector LocalYaw = SourceEngine::QAngle(0, ClientAngles.y, 0).Direction();
		SourceEngine::Vector EnemyYaw = SourceEngine::QAngle(0, pEntity->GetEyeAngles().y, 0).Direction();

		if ((pEntity->GetHealth() <= 55 && Distance < 72.4f) || (LocalYaw.Dot(EnemyYaw) > 0.8 && Distance < 72.4f))
			pCmd->buttons |= IN_ATTACK2;
		else
			pCmd->buttons |= IN_ATTACK;
	}
}

void Triggerbot::Think(SourceEngine::CUserCmd* pCmd)
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	if (pLocal->InCross())
	{
		SourceEngine::IClientEntity* Target = SourceEngine::Interfaces::EntityList()->GetClientEntity(pLocal->InCross());
		if (Utils::IsViableTarget(static_cast<C_CSPlayer*>(Target)))
			Triggerbot::Shoot(pCmd);
	}
}