#include "AntiAim.h"
#include "Utils.hpp"
#include <vector>

namespace AntiAim
{
	SourceEngine::QAngle LastAntiAimAngles;
}

void AntiAim::WarpAngles(SourceEngine::CUserCmd* pCmd, byte* bSendPacket)
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	auto ActiveWeapon = pLocal->GetActiveWeapon();
	float pitch;
	float yaw;

	if (Config.g_iAntiAim_Direction == 0)
	{
		pitch = pCmd->viewangles.x;
		yaw = pCmd->viewangles.y;
	}
	else if (Config.g_iAntiAim_Direction == 1)
	{
		pitch = 0;
		yaw = 0;
	}
	else if (Config.g_iAntiAim_Direction == 2)
	{
		auto pLocal = C_CSPlayer::GetLocalPlayer();
		int CurrentTarget = 0;
		float LastDistance = 999999999999.0f;

		for (int i = 1; i < 65; i++)
		{
			auto pEntity = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(i));
			if (Utils::IsViableTarget(pEntity))
			{
				float CurrentDistance = (pEntity->GetOrigin() - pLocal->GetOrigin()).Length();
				if (!CurrentTarget || CurrentDistance < LastDistance)
				{
					CurrentTarget = i;
					LastDistance = CurrentDistance;
				}
			}
		}

		if (!CurrentTarget)
		{
			pitch = pCmd->viewangles.x;
			yaw = pCmd->viewangles.y;
		}
		else
		{
			auto pEntity = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(CurrentTarget));
			QAngle LookAtAngle = (pEntity->GetOrigin() - pLocal->GetOrigin()).Angle();

			pitch = LookAtAngle.x;
			yaw = LookAtAngle.y;
		}
	}

	// 8 = noclip, 9 = ladder
	int MoveType = pLocal->GetMoveType();
	if (MoveType == 8 || MoveType == 9) return;
	if (ActiveWeapon->IsGrenade()) return;

	if (ActiveWeapon->IsKnife() && (Config.g_iAntiAim_Pitch || Config.g_iAntiAim_Yaw))
	{
		if (!Config.g_bEnableUntrusted)
			Utils::Clamp(pCmd->viewangles);
		if (*bSendPacket)
			LastAntiAimAngles = pCmd->viewangles;
		return;
	}
	else if (!Utils::GetEnemyPlayersAlive() && (Config.g_iAntiAim_Pitch || Config.g_iAntiAim_Yaw))
	{
		pitch = -89.0f;
		yaw = SourceEngine::Interfaces::GlobalVars()->curtime * 1000.0f;
		Utils::NormalizeAngle(yaw);
		pCmd->viewangles = QAngle(pitch, yaw, 0);
		if (!Config.g_bEnableUntrusted)
			Utils::Clamp(pCmd->viewangles);
		if (*bSendPacket)
			LastAntiAimAngles = pCmd->viewangles;
		return;
	}

	if (Config.g_bAntiAim_OverrideAngles)
	{
		PitchOverride(pCmd);
		if (!(pCmd->command_number % 2) || !Config.g_bCustomAntiAim_ApplyFakeAngle)
		{
			RealYawOverride(pCmd);
			*bSendPacket = !Config.g_bCustomAntiAim_ApplyFakeAngle;
		}
		else if (Config.g_bCustomAntiAim_ApplyFakeAngle)
		{
			FakeYawOverride(pCmd);
		}

		if (!Config.g_bEnableUntrusted)
			Utils::Clamp(pCmd->viewangles);
		if (*bSendPacket)
			LastAntiAimAngles = pCmd->viewangles;
		return;
	}

	// PITCH
	if (!Config.g_iAntiAim_Pitch) {}
	else if (Config.g_iAntiAim_Pitch == AAPitch::Down) { pitch = 179.0f; }
	else if (Config.g_iAntiAim_Pitch == AAPitch::Emotion) { pitch = 89.0f; }
	else if (Config.g_iAntiAim_Pitch == AAPitch::FakeDown) { pitch = 180.001f; }
	else if (Config.g_iAntiAim_Pitch == AAPitch::FakeAngel) { pitch = 0.0f; }
	else if (Config.g_iAntiAim_Pitch == AAPitch::Rotary)
	{
		pitch = SourceEngine::Interfaces::GlobalVars()->curtime * 1400.0f;
		Utils::NormalizeAngle(pitch);
	}
	else if (Config.g_iAntiAim_Pitch == AAPitch::RandP) { pitch = (rand() % 179) - 89.0f; }

	// YAW
	if (!Config.g_iAntiAim_Yaw) {}
	else if (Config.g_iAntiAim_Yaw == AAYaw::Backward) { yaw += 180.0f; }
	else if (Config.g_iAntiAim_Yaw == AAYaw::FakeBackward)
	{
		if (!(pCmd->command_number % 2))
		{
			*bSendPacket = false;
		}
		else
		{
			yaw += 180.0f;
		}
	}
	else if (Config.g_iAntiAim_Yaw == AAYaw::Jitter)
	{
		int cmd_n = pCmd->command_number % 3;
		if (cmd_n == 1)
		{
			yaw -= 160.0f;
		}
		else if (cmd_n == 2)
		{
			yaw -= 200.0f;
			*bSendPacket = false;
		}
	}
	else if (Config.g_iAntiAim_Yaw == AAYaw::BackJitter)
	{
		if (pCmd->command_number % 2)
			yaw += 135.0f;
		else
			yaw -= 135.0f;
	}
	else if (Config.g_iAntiAim_Yaw == AAYaw::JitterSynced)
	{
		if (pCmd->command_number % 3)
			yaw += 200.0f;
	}
	else if (Config.g_iAntiAim_Yaw == AAYaw::Sideways) { yaw += 90.0f; }
	else if (Config.g_iAntiAim_Yaw == AAYaw::FakeSideways)
	{
		if (!(pCmd->command_number % 2))
		{
			yaw -= 90.0f;
			*bSendPacket = false;
		}
		else
		{
			yaw += 90.0f;
		}
	}
	else if (Config.g_iAntiAim_Yaw == AAYaw::FakeForward)
	{
		if (!(pCmd->command_number % 2))
		{
			yaw += 180.0f;
			*bSendPacket = false;
		}
	}
	else if (Config.g_iAntiAim_Yaw == AAYaw::SlowSpin)
	{ 
		yaw = SourceEngine::Interfaces::GlobalVars()->curtime * 1000.0f;
		Utils::NormalizeAngle(yaw);
	}
	else if (Config.g_iAntiAim_Yaw == AAYaw::FastSpin)
	{
		yaw = SourceEngine::Interfaces::GlobalVars()->curtime * 5000.0f;
		Utils::NormalizeAngle(yaw);
	}
	else if (Config.g_iAntiAim_Yaw == AAYaw::Rand) { yaw = (rand() % 360) - 180.0f; }
	else if (Config.g_iAntiAim_Yaw == AAYaw::FakeSideSpin)
	{
		if (!(pCmd->command_number % 2))
		{
			yaw = SourceEngine::Interfaces::GlobalVars()->curtime * 8000.0f + 90;
			*bSendPacket = false;
		}
		else
		{
			yaw = SourceEngine::Interfaces::GlobalVars()->curtime * 8000.0f - 90;
		}
		Utils::NormalizeAngle(yaw);
	}

	pCmd->viewangles = QAngle(pitch, yaw, 0);

	if (!Config.g_bEnableUntrusted)
		Utils::Clamp(pCmd->viewangles);
	if (*bSendPacket)
		LastAntiAimAngles = pCmd->viewangles;
}

void AntiAim::PitchOverride(SourceEngine::CUserCmd* pCmd)
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	auto ActiveWeapon = pLocal->GetActiveWeapon();
	float pitch = pCmd->viewangles.x;

	// PITCH
	if (!Config.g_iCustomAntiAim_PitchOverride) {}
	else if (Config.g_iCustomAntiAim_PitchOverride == AAPitch::Down) { pitch = 179.0f; }
	else if (Config.g_iCustomAntiAim_PitchOverride == AAPitch::Emotion) { pitch = 89.0f; }
	else if (Config.g_iCustomAntiAim_PitchOverride == AAPitch::FakeDown) { pitch = 180.001f; }
	else if (Config.g_iCustomAntiAim_PitchOverride == AAPitch::FakeAngel) { pitch = 0.0f; }
	else if (Config.g_iCustomAntiAim_PitchOverride == AAPitch::Rotary)
	{
		pitch = SourceEngine::Interfaces::GlobalVars()->curtime * 1400.0f;
		Utils::NormalizeAngle(pitch);
	}
	else if (Config.g_iCustomAntiAim_PitchOverride == AAPitch::RandP) { pitch = (rand() % 179) - 89.0f; }

	pCmd->viewangles.x = pitch;
}

void AntiAim::RealYawOverride(SourceEngine::CUserCmd* pCmd)
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	auto ActiveWeapon = pLocal->GetActiveWeapon();
	float yaw;

	if (Config.g_iCustomAntiAim_RealDirectionOverride == 0)
		yaw = pCmd->viewangles.y;
	else if (Config.g_iCustomAntiAim_RealDirectionOverride == 1)
		yaw = 0;
	else if (Config.g_iCustomAntiAim_RealDirectionOverride == 2)
	{
		auto pLocal = C_CSPlayer::GetLocalPlayer();
		int CurrentTarget = 0;
		float LastDistance = 999999999999.0f;

		for (int i = 1; i < 65; i++)
		{
			auto pEntity = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(i));
			if (Utils::IsViableTarget(pEntity))
			{
				float CurrentDistance = (pEntity->GetOrigin() - pLocal->GetOrigin()).Length();
				if (!CurrentTarget || CurrentDistance < LastDistance)
				{
					CurrentTarget = i;
					LastDistance = CurrentDistance;
				}
			}
		}

		if (!CurrentTarget)
			yaw = pCmd->viewangles.y;
		else
		{
			auto pEntity = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(CurrentTarget));
			QAngle LookAtAngle = (pEntity->GetOrigin() - pLocal->GetOrigin()).Angle();
			yaw = LookAtAngle.y;
		}
	}

	// YAW
	if (!Config.g_iCustomAntiAim_RealYawOverride) {}
	else if (Config.g_iCustomAntiAim_RealYawOverride == AAYawOverride::Backward_Override) { yaw += 180.0f; }
	else if (Config.g_iCustomAntiAim_RealYawOverride == AAYawOverride::BackJitter_Override)
	{
		if ((pCmd->command_number % 4) < 2)
			yaw += 135.0f;
		else
			yaw -= 135.0f;
	}
	else if (Config.g_iCustomAntiAim_RealYawOverride == AAYawOverride::JitterSynced_Override)
	{
		if (pCmd->command_number % 3)
			yaw += 200.0f;
	}
	else if (Config.g_iCustomAntiAim_RealYawOverride == AAYawOverride::Sideways_Override) { yaw += 90.0f; }
	else if (Config.g_iCustomAntiAim_RealYawOverride == AAYawOverride::SlowSpin_Override)
	{
		yaw = SourceEngine::Interfaces::GlobalVars()->curtime * 1000.0f;
		Utils::NormalizeAngle(yaw);
	}
	else if (Config.g_iCustomAntiAim_RealYawOverride == AAYawOverride::FastSpin_Override)
	{
		yaw = SourceEngine::Interfaces::GlobalVars()->curtime * 5000.0f;
		Utils::NormalizeAngle(yaw);
	}
	else if (Config.g_iCustomAntiAim_RealYawOverride == AAYawOverride::Rand_Override) { yaw = (rand() % 360) - 180.0f; }

	pCmd->viewangles.y = yaw;
}

void AntiAim::FakeYawOverride(SourceEngine::CUserCmd* pCmd)
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	auto ActiveWeapon = pLocal->GetActiveWeapon();
	float yaw;

	if (Config.g_iCustomAntiAim_FakeDirectionOverride == 0)
		yaw = pCmd->viewangles.y;
	else if (Config.g_iCustomAntiAim_FakeDirectionOverride == 1)
		yaw = 0;
	else if (Config.g_iCustomAntiAim_FakeDirectionOverride == 2)
	{
		auto pLocal = C_CSPlayer::GetLocalPlayer();
		int CurrentTarget = 0;
		float LastDistance = 999999999999.0f;

		for (int i = 1; i < 65; i++)
		{
			auto pEntity = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(i));
			if (Utils::IsViableTarget(pEntity))
			{
				float CurrentDistance = (pEntity->GetOrigin() - pLocal->GetOrigin()).Length();
				if (!CurrentTarget || CurrentDistance < LastDistance)
				{
					CurrentTarget = i;
					LastDistance = CurrentDistance;
				}
			}
		}

		if (!CurrentTarget)
			yaw = pCmd->viewangles.y;
		else
		{
			auto pEntity = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(CurrentTarget));
			QAngle LookAtAngle = (pEntity->GetOrigin() - pLocal->GetOrigin()).Angle();
			yaw = LookAtAngle.y;
		}
	}

	// YAW
	if (!Config.g_iCustomAntiAim_FakeYawOverride) {}
	else if (Config.g_iCustomAntiAim_FakeYawOverride == AAYawOverride::Backward_Override) { yaw += 180.0f; }
	else if (Config.g_iCustomAntiAim_FakeYawOverride == AAYawOverride::BackJitter_Override)
	{
		if ((pCmd->command_number % 4) < 2)
			yaw += 135.0f;
		else
			yaw -= 135.0f;
	}
	else if (Config.g_iCustomAntiAim_FakeYawOverride == AAYawOverride::JitterSynced_Override)
	{
		if (pCmd->command_number % 3)
			yaw += 200.0f;
	}
	else if (Config.g_iCustomAntiAim_FakeYawOverride == AAYawOverride::Sideways_Override) { yaw += 90.0f; }
	else if (Config.g_iCustomAntiAim_FakeYawOverride == AAYawOverride::SlowSpin_Override)
	{
		yaw = SourceEngine::Interfaces::GlobalVars()->curtime * 1000.0f;
		Utils::NormalizeAngle(yaw);
	}
	else if (Config.g_iCustomAntiAim_FakeYawOverride == AAYawOverride::FastSpin_Override)
	{
		yaw = SourceEngine::Interfaces::GlobalVars()->curtime * 5000.0f;
		Utils::NormalizeAngle(yaw);
	}
	else if (Config.g_iCustomAntiAim_FakeYawOverride == AAYawOverride::Rand_Override) { yaw = (rand() % 360) - 180.0f; }

	pCmd->viewangles.y = yaw;
}

void AntiAim::MovementFix(SourceEngine::CUserCmd* pCmd)
{
	SourceEngine::QAngle ClientViewAngles;
	SourceEngine::Interfaces::Engine()->GetViewAngles(ClientViewAngles);

	SourceEngine::QAngle CmdAngle = pCmd->viewangles;
	Utils::NormalizeAngles(CmdAngle);

	Vector vMove = Vector(pCmd->forwardmove, pCmd->sidemove, 0.0f);
	float flSpeed = vMove.Length();
	Vector qMove = vMove.Angle();
	float flYaw = Utils::DegreesToRadians((CmdAngle.y - ClientViewAngles.y) + qMove.y);
	if (CmdAngle.x >= 90.0f || CmdAngle.x <= -90.0f) pCmd->forwardmove = -cos(flYaw) * flSpeed;
	else pCmd->forwardmove = cos(flYaw) * flSpeed;
	pCmd->sidemove = sin(flYaw) * flSpeed;
}