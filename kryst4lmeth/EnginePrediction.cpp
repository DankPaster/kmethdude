#include "EnginePrediction.h"
#include "CSGOStructs.hpp"

void* g_pMoveHelper = nullptr;

typedef void(__thiscall* SetHost_t)(void*, void*);

void StartPrediction(CUserCmd* pCmd)
{
	if (!g_pMoveHelper) { return; }
	static uint32_t m_pCurrentCommand = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_hConstraintEntity")) - 0xC;
	static uint32_t m_fFlags = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_fFlags"));
	static SetHost_t SetHost = (SetHost_t)SourceEngine::CallVFunction<SetHost_t>(g_pMoveHelper, 1);

	auto pLocal = C_CSPlayer::GetLocalPlayer();
	int LocalFlags = pLocal->GetFlags();

	float OldFrametime = Interfaces::GlobalVars()->frametime;
	float OldCurtime = Interfaces::GlobalVars()->curtime;

	Interfaces::GlobalVars()->frametime = Interfaces::GlobalVars()->interval_per_tick;
	Interfaces::GlobalVars()->curtime = pLocal->GetTickBase() * Interfaces::GlobalVars()->interval_per_tick;

	*(CUserCmd**)((uintptr_t)pLocal + m_pCurrentCommand) = pCmd;

	MoveData data;
	memset(&data, 0, sizeof(MoveData));

	SetHost(g_pMoveHelper, pLocal);
	Interfaces::Prediction()->SetupMove(pLocal, pCmd, g_pMoveHelper, &data);
	Interfaces::GameMovement()->ProcessMovement(pLocal, &data);
	Interfaces::Prediction()->FinishMove(pLocal, pCmd, &data);
	SetHost(g_pMoveHelper, nullptr);

	*(CUserCmd**)((uintptr_t)pLocal + m_pCurrentCommand) = nullptr;

	Interfaces::GlobalVars()->curtime = OldCurtime;
	Interfaces::GlobalVars()->frametime = OldFrametime;
	*(int*)((uintptr_t)pLocal + m_fFlags) = LocalFlags;
}