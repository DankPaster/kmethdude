#include "FakeLag.h"

int CmdCounter = 0;
int StepCounter = 0;

void FakeLag::Choke(byte* bSendPacket, SourceEngine::CUserCmd* pCmd)
{
	if (!Config.g_iFakeLag_LagFactor) return;
	if (pCmd->buttons & IN_ATTACK) return;

	auto ActiveWeapon = C_CSPlayer::GetLocalPlayer()->GetActiveWeapon();
	if (pCmd->buttons & IN_JUMP && Config.g_bBunnyhop) *bSendPacket = true;
	if (ActiveWeapon->IsKnife())
	{
		*bSendPacket = true;
		return;
	}
	if (!Utils::GetEnemyPlayersAlive() && (Config.g_iAntiAim_Pitch || Config.g_iAntiAim_Yaw))
	{
		*bSendPacket = true;
		return;
	}
	if (Config.g_iFakeLag_Mode == 0) // Normal
	{
		if (CmdCounter >= Config.g_iFakeLag_LagFactor)
		{
			*bSendPacket = true;
			CmdCounter = 0;
		}
		else
			*bSendPacket = false;
		CmdCounter++;
	}
	if (Config.g_iFakeLag_Mode == 1) // Step
	{
		if (CmdCounter >= StepCounter)
		{
			StepCounter++;
			CmdCounter = 0;
			if (StepCounter >= Config.g_iFakeLag_LagFactor)
			{
				*bSendPacket = true;
				StepCounter = 0;
			}
		}
		else
		{
			*bSendPacket = false;
		}
		CmdCounter++;
	}

	else if (Config.g_iFakeLag_Mode == 2) // Reactive
	{
		if (CmdCounter >= Config.g_iFakeLag_LagFactor - StepCounter)
		{
			*bSendPacket = true;
			StepCounter--;
			CmdCounter = 0;
			if (StepCounter <= 0)
				StepCounter = Config.g_iFakeLag_LagFactor;
		}
		else
		{
			*bSendPacket = false;
		}
		CmdCounter++;
	}
}
