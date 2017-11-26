#include "Hooks.hpp"
#include "Options.hpp"
#include "Utils.hpp"
#include "XorStr.hpp"

#include "EntityESP.hpp"

#include "Utils.hpp"
#include <ctime>
#include "Menu.h"

using namespace std;

int GlobalCmdCounter = 0;

namespace Hooks
{
	VFTableHook*	g_pClientModeHook;
	VFTableHook*	g_pClientHook;
	VFTableHook*	g_pPredictionHook;
	VFTableHook*	g_pModelRenderHook;
	VFTableHook*	g_pPanelHook;
	VFTableHook*	g_pMaterialSystemHook;

	DamageEvent* DamageListener = nullptr;
	DeathEvent* DeathListener = nullptr;

	/////////////////////
	// EVENT LISTENERS //
	/////////////////////

	void InitializeEventListeners()
	{
		DamageListener = new DamageEvent();
		DeathListener = new DeathEvent();
	}

	void DestroyEventListeners()
	{
		delete DamageListener;
		delete DeathListener;
	}

    void Initialize()
    {
		//AllocConsole();
		//AttachConsole(GetCurrentProcessId());
		//freopen("CON", "w", stdout);

        //Builds the netvar database
        NetvarManager::Instance()->CreateDatabase();
        //NetvarManager::Instance()->Dump(XorStr("netvar_dump.txt"));

		g_pClientModeHook = new VFTableHook(SourceEngine::Interfaces::ClientMode());
		g_pClientHook = new VFTableHook(SourceEngine::Interfaces::Client());
		g_pPredictionHook = new VFTableHook(SourceEngine::Interfaces::Prediction());
		g_pModelRenderHook = new VFTableHook(SourceEngine::Interfaces::ModelRender());
		g_pPanelHook = new VFTableHook(SourceEngine::Interfaces::VGUIPanel());

        g_pClientModeHook->Hook(24, (CreateMove_t)Hooked_CreateMove);
		g_pClientModeHook->Hook(18, (OverrideView_t)Hooked_OverrideView);
		g_pClientHook->Hook(36, (FrameStageNotify_t)Hooked_FrameStageNotify);
		g_pPredictionHook->Hook(19, (RunCommand_t)Hooked_RunCommand);
		g_pPanelHook->Hook(41, (PaintTraverse_t)Hooked_PaintTraverse);
		g_pModelRenderHook->Hook(21, (DrawModelExecute_t)Hooked_DrawModelExecute);

		InitializeEventListeners();
		Menu::InitializeMenu();
    }

    void Restore()
    {
		//DestroyEventListeners();

		Menu::DestroyObjects();

        //Remove the hooks
		g_pClientModeHook->RestoreTable();
		g_pClientHook->RestoreTable();
		g_pPredictionHook->RestoreTable();
		g_pModelRenderHook->RestoreTable();
		g_pPanelHook->RestoreTable();

		delete g_pClientModeHook;
		delete g_pClientHook;
		delete g_pPredictionHook;
		delete g_pModelRenderHook;
		delete g_pPanelHook;
    }

	void PollInputs()
	{
		static DWORD ToggleKeyState = false;

		if (((GetAsyncKeyState(0x48) < 0) && (GetAsyncKeyState(VK_MENU) < 0)) && ((GetAsyncKeyState(0x48) < 0) != ToggleKeyState))
		{
			switch (Config.g_iAimbotTarget)
			{
			case 0:
				Config.g_iAimbotTarget = 1;
				PushNotifications::Push(XorStr("Aim Target changed to Chest"));
				break;
			case 1:
				Config.g_iAimbotTarget = 2;
				PushNotifications::Push(XorStr("Aim Target changed to Stomach"));
				break;
			case 2:
				Config.g_iAimbotTarget = 0;
				PushNotifications::Push(XorStr("Aim Target changed to Head"));
				break;
			}
		}
		ToggleKeyState = (GetAsyncKeyState(0x48) < 0);
	}

	void __stdcall Hooked_RunCommand(IClientEntity* pEntity, CUserCmd* pCmd, void* pMoveHelper)
	{
		g_pPredictionHook->GetOriginal<RunCommand_t>(19)(Interfaces::Prediction(), pEntity, pCmd, pMoveHelper);
		g_pMoveHelper = pMoveHelper;
	}

	void __stdcall Hooked_FrameStageNotify(SourceEngine::ClientFrameStage pStage)
	{
		SourceEngine::QAngle OriginalPlayerAngles;
		SourceEngine::Vector OldViewPunch, OldAimPunch;
		OriginalPlayerAngles.Invalidate();
		OldViewPunch.Invalidate();
		OldAimPunch.Invalidate();

		bool IsSafe = Utils::LocalCheck();

		if (Interfaces::Engine()->IsConnected() && Interfaces::Engine()->IsInGame())
		{
			if (pStage == SourceEngine::ClientFrameStage::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
			{
				if (Config.g_bLowerBodyYawFix)
				{
					for (int i = 1; i < Interfaces::Engine()->GetMaxClients(); i++)
						Resolver::LowerBodyYawFix(i);
				}
				if (Config.g_bResolve)
				{
					for (int i = 1; i < Interfaces::Engine()->GetMaxClients(); i++)
						Resolver::Resolve(i);
				}
			}
		}
		else
			Resolver::PlayerData.clear();

		if (IsSafe)
		{
			auto pLocal = C_CSPlayer::GetLocalPlayer();
			if (pStage == SourceEngine::ClientFrameStage::FRAME_RENDER_START && *(byte*)((DWORD)Interfaces::Input() + 0xA5))
			{
				OriginalPlayerAngles = *pLocal->PlayerAngles();
				*pLocal->PlayerAngles() = AntiAim::LastAntiAimAngles;
			}
			if (pStage == SourceEngine::ClientFrameStage::FRAME_RENDER_START)
			{
				if (Config.g_bNoVisualRecoil)
				{
					OldViewPunch = *(pLocal->ViewPunch());
					OldAimPunch = *(pLocal->AimPunch());
					pLocal->ViewPunch()->Zero();
					pLocal->AimPunch()->Zero();
				}
			}
		}

		g_pClientHook->GetOriginal<FrameStageNotify_t>(36)(SourceEngine::Interfaces::Client(), pStage);

		if (OldViewPunch.IsValid())
			*C_CSPlayer::GetLocalPlayer()->ViewPunch() = OldViewPunch;
		if (OldAimPunch.IsValid())
			*C_CSPlayer::GetLocalPlayer()->AimPunch() = OldAimPunch;
		if (OriginalPlayerAngles.IsValid() && pStage == FRAME_RENDER_START && *(byte*)((DWORD)Interfaces::Input() + 0xA5))
			*C_CSPlayer::GetLocalPlayer()->PlayerAngles() = OriginalPlayerAngles;
	}

	void __stdcall Hooked_OverrideView(SourceEngine::CViewSetup* pSetup)
	{
		if (SourceEngine::Interfaces::Engine()->IsConnected() && SourceEngine::Interfaces::Engine()->IsInGame())
		{
			if (!C_CSPlayer::GetLocalPlayer()->IsScoped())
			{
				pSetup->fov = Config.g_iFOV;
				pSetup->viewmodel_fov = Config.g_iFOV;
			}
		}
		g_pClientModeHook->GetOriginal<OverrideView_t>(18)(SourceEngine::Interfaces::ClientMode(), pSetup);
	}

	bool __stdcall Hooked_CreateMove(float sample_input_frametime, SourceEngine::CUserCmd* pCmd)
	{
		//Call original CreateMove
		GlobalCmdCounter = pCmd->command_number;

		if (pCmd->command_number == 0 || !Interfaces::Engine()->IsInGame())
			return false;

		uintptr_t* FPointer; __asm { MOV FPointer, EBP }
		byte* SendPacket = (byte*)(*FPointer - 0x1C);

		if (Utils::LocalCheck())
		{
			if (Config.g_iFakeLag_LagFactor)
				FakeLag::Choke(SendPacket, pCmd);

			AntiAim::WarpAngles(pCmd, SendPacket);

			auto pLocal = C_CSPlayer::GetLocalPlayer();
			auto ActiveWeapon = pLocal->GetActiveWeapon();
			AutoShoot::IsFiring = false;

			int MoveType = pLocal->GetMoveType();
			if (Config.g_bCircleStrafe && !CircleStrafe::IsActive && (pCmd->buttons & IN_SPEED) && !(MoveType == 8 || MoveType == 9))
				CircleStrafe::Start(pCmd);

			if (Config.g_bBunnyhop && (pCmd->buttons & IN_JUMP) && !(pLocal->GetFlags() & (int)SourceEngine::EntityFlags::FL_ONGROUND))
				pCmd->buttons &= ~IN_JUMP;

			if (Config.g_bAutoStrafe && !(MoveType == 8 || MoveType == 9) && !CircleStrafe::IsActive)
			{
				if (!(pLocal->GetFlags() & (int)SourceEngine::EntityFlags::FL_ONGROUND))
				{
					if (pCmd->mousedx > 1 || pCmd->mousedx < -1) {
						pCmd->sidemove = pCmd->mousedx < 0.0f ? -pLocal->GetMaxSpeed() : pLocal->GetMaxSpeed();
					}
					else {
						pCmd->forwardmove = 4200.0f / pLocal->GetVelocity().Length();
						pCmd->sidemove = (pCmd->command_number % 2) == 0 ? -pLocal->GetMaxSpeed() : pLocal->GetMaxSpeed();
						if (pCmd->forwardmove > pLocal->GetMaxSpeed())
							pCmd->forwardmove = pLocal->GetMaxSpeed();
					}
				}
				else if (pCmd->buttons & IN_JUMP) {
						pCmd->forwardmove = pLocal->GetMaxSpeed();
					}
			}

			if (Config.g_bCircleStrafe && CircleStrafe::IsActive)
				CircleStrafe::Strafe(pCmd);
			else if (CircleStrafe::IsActive)
				CircleStrafe::IsActive = false;

			StartPrediction(pCmd);

			RCS::UpdatePunchAngles();

			if (Config.g_bAutoShoot && ActiveWeapon->CanFire() && !pLocal->GetActiveWeapon()->IsC4())
				AutoShoot::Fire(pCmd, SendPacket);

			if (!Config.g_bAutoShoot)
			{
				if (Config.g_iAimAssistMode == 1)
					AimLock::Think(pCmd);
				else if (Config.g_iAimAssistMode == 2)
					Aimbot::Think(pCmd);

				if (Config.g_bTriggerbot)
					Triggerbot::Think(pCmd);
			}

			// AIM/SHOOT
			if (!ActiveWeapon->GetClip1()) {
				pCmd->buttons &= ~IN_ATTACK;
				pCmd->buttons |= IN_RELOAD;
			}

			if (!ActiveWeapon->CanFire() && !ActiveWeapon->IsAutomatic() && !ActiveWeapon->IsC4())
				pCmd->buttons &= ~IN_ATTACK;

			if (!AutoShoot::IsFiring && (pCmd->buttons & IN_ATTACK) && ActiveWeapon->CanFire() && (Config.g_iAntiAim_Yaw || Config.g_iAntiAim_Pitch) || pCmd->buttons & IN_USE)
			{
				SourceEngine::QAngle ClientAngles;
				SourceEngine::Interfaces::Engine()->GetViewAngles(ClientAngles);
				pCmd->viewangles = ClientAngles;
			}
			if (!AutoShoot::IsFiring)
			{
				if (Config.g_iRCSAccuracy)
					pCmd->viewangles -= RCS::GetLegitPunchAngles();
			}

			if (Config.g_bAirStuck && !(pCmd->buttons & IN_ATTACK) && GetAsyncKeyState(0x56) && Config.g_bEnableUntrusted) // V key
				pCmd->tick_count = 0xFFFFFF;

			if (!Config.g_bEnableUntrusted)
			{
				Utils::Clamp(pCmd->viewangles);
				if (pCmd->forwardmove > 450.0f) pCmd->forwardmove = 450.0f;
				if (pCmd->forwardmove < -450.0f) pCmd->forwardmove = -450.0f;
				if (pCmd->sidemove > 450.0f) pCmd->sidemove = 450.0f;
				if (pCmd->sidemove < -450.0f) pCmd->sidemove = -450.0f;
			}

			AntiAim::MovementFix(pCmd);

			for (int i = 1; i < 65; i++)
			{
				auto pEnt = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(i));
				if (pEnt && !pEnt->IsDormant() && pEnt->IsAlive()) OldSimulationTimes[i] = pEnt->GetSimulationTime();
			}
		}
		return false;
	}
	void __stdcall Hooks::Hooked_DrawModelExecute(IMatRenderContext* CTX, const DrawModelState_t &state, const ModelRenderInfo_t &renderInfo, matrix3x4_t *pCustomBoneToWorld)
	{
		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();
		IClientEntity* pEntity = Interfaces::EntityList()->GetClientEntity(renderInfo.entity_index);
		if (pEntity)
		{
			const char* ModelName = Interfaces::ModelInfo()->GetModelName(renderInfo.pModel);
			static IMaterial* ChamMaterialSolid;
			static IMaterial* ChamMaterialWireframe;

			if (!ChamMaterialSolid)
				ChamMaterialSolid = Utils::CreateMaterial(false, true, false);
			if (!ChamMaterialWireframe)
				ChamMaterialWireframe = Utils::CreateMaterial(false, true, true);;

			if (Config.g_iPlayerChams && renderInfo.pModel && renderInfo.entity_index && strstr(Interfaces::ModelInfo()->GetModelName(renderInfo.pModel), XorStr("models/player")))
			{
				C_CSPlayer* pEnt = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(renderInfo.entity_index));
				C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();
				if (pEnt && pLocal)
				{
					if (Config.g_iPlayerChams == 1)
						Interfaces::ModelRender()->ForcedMaterialOverride(ChamMaterialSolid, OVERRIDE_NORMAL, 0);
					else if (Config.g_iPlayerChams == 2)
						Interfaces::ModelRender()->ForcedMaterialOverride(ChamMaterialWireframe, OVERRIDE_NORMAL, 0);
					if (pEnt->GetTeamNum() == pLocal->GetTeamNum())
					{
						float flColor[3] = { Config.g_iChams_TeamColor[0] / 255.0f, Config.g_iChams_TeamColor[1] / 255.0f, Config.g_iChams_TeamColor[2] / 255.0f };
						Interfaces::RenderView()->SetColorModulation(flColor);
					}
					else
					{
						float HealthFrac = pEnt->GetHealth() / 100.0f;
						Vector4D EnemyColor;
						if (HealthFrac > 0.5f)
							EnemyColor = Vector4D((1 - HealthFrac) * 2.0f, 1, 0, 1);
						else
							EnemyColor = Vector4D(1, HealthFrac * 2.0f, 0, 1);
						float flColor[3] = { EnemyColor.x, EnemyColor.y, EnemyColor.z };
						Interfaces::RenderView()->SetColorModulation(flColor);
					}
					Interfaces::RenderView()->SetBlend(Config.g_fChams_Alpha);
				}
			}
		}
		g_pModelRenderHook->Unhook(21);
		g_pModelRenderHook->GetOriginal<DrawModelExecute_t>(21)(Interfaces::ModelRender(), CTX, state, renderInfo, pCustomBoneToWorld);
		g_pModelRenderHook->Hook(21, (DrawModelExecute_t)Hooked_DrawModelExecute);
		Interfaces::ModelRender()->ForcedMaterialOverride(0, OVERRIDE_NORMAL, 0);
	}
	void __stdcall Hooked_PaintTraverse(VPANEL Panel, bool allowForce, bool forceRepaint)
	{
		g_pPanelHook->GetOriginal<PaintTraverse_t>(41)(Interfaces::VGUIPanel(), Panel, allowForce, forceRepaint);

		static unsigned int overlayPanel;
		if (!overlayPanel)
			if (strstr(Interfaces::VGUIPanel()->GetName(Panel), XorStr("MatSystemTopPanel")))
				overlayPanel = Panel;
		if (!overlayPanel || overlayPanel != Panel) return;

		if (Interfaces::Engine()->IsConnected() && Interfaces::Engine()->IsInGame())
		{
			PollInputs();

			for (int i = 1; i < SourceEngine::Interfaces::Engine()->GetMaxClients(); i++)
			{
				if (!C_CSPlayer::GetLocalPlayer()) break;
				if (i == SourceEngine::Interfaces::Engine()->GetLocalPlayer()) continue;
				auto pEntity = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(i));
				if (!pEntity) continue;
				if (!pEntity->IsAlive() || pEntity->IsDormant()) continue;
				if (pEntity->GetClientClass()->m_ClassID == SourceEngine::EClassIds::CCSPlayer)
				{
					EntityESP esp(pEntity);

					if (Config.g_bESPShowSkeletons)
						esp.RenderSkeleton();
					if (Config.g_bESPShowBoxes)
						esp.RenderESP();
					if (Config.g_bESPShowPlayerNames)
						esp.RenderName();
					if (Config.g_bESPShowWeaponNames)
						esp.RenderWeaponName();
				}
			}
		}

		PushNotifications::Update();
		Menu::Render();

		if (Config.g_bAutoNameSteal)
			NameChanger::AutoNameStealer();
	}
}