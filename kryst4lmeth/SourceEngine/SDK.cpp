#include "SDK.hpp"

#include "../Utils.hpp"
#include "../XorStr.hpp"

using namespace XorCompileTime;

namespace SourceEngine
{
    IVEngineClient*     Interfaces::m_pEngine = nullptr;
    IBaseClientDLL*     Interfaces::m_pClient = nullptr;
    IClientEntityList*  Interfaces::m_pEntityList = nullptr;
    CGlobalVarsBase*    Interfaces::m_pGlobals = nullptr;
    IPanel*             Interfaces::m_pVGuiPanel = nullptr;
    ISurface*           Interfaces::m_pVGuiSurface = nullptr;
    CInput*             Interfaces::m_pInput = nullptr;
    IEngineTrace*       Interfaces::m_pEngineTrace = nullptr;
    ICvar*              Interfaces::m_pCVar = nullptr;
    IClientMode*        Interfaces::m_pClientMode = nullptr;
    IPhysicsSurfaceProps*  Interfaces::m_pPhysProps = nullptr;
    IPrediction*	Interfaces::m_pPrediction = nullptr;
    IGameMovement*	Interfaces::m_pGameMovement = nullptr;
    IGameEventManager2* Interfaces::m_pGameEventManager = nullptr;
    IVModelInfo*	Interfaces::m_pModelInfo = nullptr;
    IVModelRender*	Interfaces::m_pModelRender = nullptr;
    IMaterialSystem*    Interfaces::m_pMaterialSystem = nullptr;
    IVRenderView*	Interfaces::m_pRenderView = nullptr;

    CreateInterfaceFn GetFactory(HMODULE hMod)
    {
        return(CreateInterfaceFn)GetProcAddress(hMod, XorStr("CreateInterface"));
    }

    template<typename T>
    T* CaptureInterface(CreateInterfaceFn f, const char* szInterfaceVersion)
    {
        return (T*)f(szInterfaceVersion, NULL);
    }

	inline void* CaptureInterfaceSZ(const char* chHandle, const char* chInterfaceName)
	{
		volatile auto HandleGotten = (GetModuleHandleA(chHandle) != nullptr);
		while (!GetModuleHandleA(chHandle)) Sleep(100);
		void* fnFinal = nullptr;
		char* PossibleInterfaceName = new char[strlen(chInterfaceName) + 4];
		auto TestInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA(chHandle), XorStr("CreateInterface")));
		for (auto i = 100; i > 0; i--)
		{
			w_sprintf(PossibleInterfaceName, XorStr("%s%03i"), chInterfaceName, i);
			fnFinal = static_cast<void*>(TestInterface(PossibleInterfaceName, nullptr));

			if (fnFinal != nullptr)
				break;
		}
		delete[] PossibleInterfaceName;
		return fnFinal;
	}

    IVEngineClient* Interfaces::Engine()
    {
        if(!m_pEngine) {
            m_pEngine = static_cast<IVEngineClient*>(CaptureInterfaceSZ(XorStr("engine.dll"), XorStr("VEngineClient")));
        }
        return m_pEngine;
    }
    IBaseClientDLL* Interfaces::Client()
    {
        if(!m_pClient) {
            m_pClient = static_cast<IBaseClientDLL*>(CaptureInterfaceSZ(XorStr("client.dll"), XorStr("VClient")));
        }
        return m_pClient;
    }
    IClientEntityList* Interfaces::EntityList()
    {
        if(!m_pEntityList) {
            m_pEntityList = static_cast<IClientEntityList*>(CaptureInterfaceSZ(XorStr("client.dll"), XorStr("VClientEntityList")));
        }
        return m_pEntityList;
    }
    CGlobalVarsBase* Interfaces::GlobalVars()
    {
        if(!m_pGlobals) {
			m_pGlobals = **reinterpret_cast<CGlobalVarsBase***>((*(DWORD**)Client())[0] + 0x1B);
        }
        return m_pGlobals;
    }
    IPanel* Interfaces::VGUIPanel()
    {
        if(!m_pVGuiPanel) {
            m_pVGuiPanel = static_cast<IPanel*>(CaptureInterfaceSZ(XorStr("vgui2.dll"), XorStr("VGUI_Panel")));
        }
        return m_pVGuiPanel;
    }
    ISurface* Interfaces::MatSurface()
    {
        if(!m_pVGuiSurface) {
            m_pVGuiSurface = static_cast<ISurface*>(CaptureInterfaceSZ(XorStr("vguimatsurface.dll"), XorStr("VGUI_Surface")));
        }
        return m_pVGuiSurface;
    }
    CInput* Interfaces::Input()
    {
        if(!m_pInput) {
            auto pClientVFTable = *(uint32_t**)Client();
            m_pInput = *(CInput**)(pClientVFTable[15] + 0x1);
        }
        return m_pInput;
    }
    IEngineTrace* Interfaces::EngineTrace()
    {
        if(!m_pEngineTrace) {
            m_pEngineTrace = static_cast<IEngineTrace*>(CaptureInterfaceSZ(XorStr("engine.dll"), XorStr("EngineTraceClient")));
        }
        return m_pEngineTrace;
    }
    ICvar* Interfaces::CVar()
    {
        if(!m_pCVar) {
            m_pCVar = static_cast<ICvar*>(CaptureInterfaceSZ(XorStr("vstdlib.dll"), XorStr("VEngineCvar")));
        }
        return m_pCVar;
    }
    IClientMode* Interfaces::ClientMode()
    {
        if(!m_pClientMode) {
            auto uAddress = *(DWORD*)(Utils::FindSignature(XorStr("client.dll"), XorStr("8B 0D ? ? ? ? 8B 01 5D FF")) + 2);
            m_pClientMode = *(IClientMode**)(uAddress);
        }
        return m_pClientMode;
    }
	IPhysicsSurfaceProps* Interfaces::PhysProps()
	{
		if (!m_pPhysProps)
			m_pPhysProps = static_cast<IPhysicsSurfaceProps*>(CaptureInterfaceSZ(XorStr("vphysics.dll"), XorStr("VPhysicsSurfaceProps")));
		return m_pPhysProps;
	}
	IPrediction* Interfaces::Prediction()
	{
		if (!m_pPrediction)
			m_pPrediction = static_cast<IPrediction*>(CaptureInterfaceSZ(XorStr("client.dll"), XorStr("VClientPrediction")));
		return m_pPrediction;
	}
	IGameMovement* Interfaces::GameMovement()
	{
		if (!m_pGameMovement)
			m_pGameMovement = static_cast<IGameMovement*>(CaptureInterfaceSZ(XorStr("client.dll"), XorStr("GameMovement")));
		return m_pGameMovement;
	}
	IGameEventManager2* Interfaces::GameEventManager()
	{
		if (!m_pGameEventManager)
		{
			m_pGameEventManager = static_cast<IGameEventManager2*>(CaptureInterfaceSZ(XorStr("engine.dll"), XorStr("GAMEEVENTSMANAGER")));
			if (!m_pGameEventManager->LoadEventsFromFile("resource/gameevents.res"))
				abort();
		}
		return m_pGameEventManager;
	}
	IVModelInfo* Interfaces::ModelInfo()
	{
		if (!m_pModelInfo)
			m_pModelInfo = static_cast<IVModelInfo*>(CaptureInterfaceSZ(XorStr("engine.dll"), XorStr("VModelInfoClient")));
		return m_pModelInfo;
	}
	IVModelRender* Interfaces::ModelRender()
	{
		if (!m_pModelRender)
			m_pModelRender = static_cast<IVModelRender*>(CaptureInterfaceSZ(XorStr("engine.dll"), XorStr("VEngineModel")));
		return m_pModelRender;
	}
	IMaterialSystem* Interfaces::MaterialSystem()
	{
		if (!m_pMaterialSystem)
			m_pMaterialSystem = static_cast<IMaterialSystem*>(CaptureInterfaceSZ(XorStr("materialsystem.dll"), XorStr("VMaterialSystem")));
		return m_pMaterialSystem;
	}
	IVRenderView* Interfaces::RenderView()
	{
		if (!m_pRenderView)
			m_pRenderView = static_cast<IVRenderView*>(CaptureInterfaceSZ(XorStr("engine.dll"), XorStr("VEngineRenderView")));
		return m_pRenderView;
	}
}
