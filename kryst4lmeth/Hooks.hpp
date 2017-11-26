#pragma once

#include <Windows.h>
#include <memory> //unique_ptr & make_unique

#include "SourceEngine/SDK.hpp"
#include "VFTableHook.hpp"

#include "AutoShoot.h"
#include "AntiAim.h"
#include "Triggerbot.h"
#include "AimAssist.h"
#include "Resolver.h"
#include "FakeLag.h"
#include "EnginePrediction.h"
#include "NoRecoil.h"
#include "CircleStrafe.h"
#include "NameChanger.h"
#include "PushNotifications.h"
#include "DamageEvent.h"
#include "DeathEvent.h"

namespace Hooks
{
    // Sets up and hooks some functions. This should be called when the dll is loaded.
    void Initialize();

    // Cleans up and removes the hooks. This should be called when the dll is unloaded.
    void Restore();

    //---------------------------------------------------
    // Hook prototypes
    //---------------------------------------------------

    typedef bool(__thiscall* CreateMove_t)(SourceEngine::IClientMode*, float, SourceEngine::CUserCmd*);
	typedef void(__thiscall* OverrideView_t)(SourceEngine::IClientMode*, SourceEngine::CViewSetup*);
	typedef void(__thiscall* FrameStageNotify_t)(SourceEngine::IBaseClientDLL*, SourceEngine::ClientFrameStage);
	typedef void(__thiscall* RunCommand_t)(SourceEngine::IPrediction*, IClientEntity*, CUserCmd*, void*);
	typedef void(__thiscall* DrawModelExecute_t)(IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
	//typedef IMaterial* MatPtr;
	//typedef MatPtr(__thiscall* FindMaterial_t)(IMaterialSystem*, const char*, char const*, bool, const char*);
	typedef void(__thiscall* PaintTraverse_t)(IPanel*, VPANEL, bool, bool);

    //---------------------------------------------------
    // Hooked functions
    //---------------------------------------------------

    bool      __stdcall Hooked_CreateMove(float sample_input_frametime, SourceEngine::CUserCmd* pCmd);
	void	  __stdcall Hooked_OverrideView(SourceEngine::CViewSetup* pSetup);
	void	  __stdcall Hooked_FrameStageNotify(SourceEngine::ClientFrameStage pStage);
	void	  __stdcall Hooked_RunCommand(IClientEntity* pEntity, CUserCmd* pCmd, void* pMoveHelper);
	void	  __stdcall Hooked_DrawModelExecute(IMatRenderContext* CTX, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld);
	//IMaterial* __stdcall Hooked_FindMaterial(const char *pMaterialName, const char *pTextureGroupName, bool bComplain, const char *pComplainPrefix);
	void	  __stdcall Hooked_PaintTraverse(VPANEL Panel, bool allowForce, bool forceRepaint);
}