#pragma once

#include <Windows.h>
#include <Psapi.h>

#define HITGROUP_GENERIC    0
#define HITGROUP_HEAD        1
#define	HITGROUP_CHEST        2
#define HITGROUP_STOMACH    3
#define HITGROUP_LEFTARM    4    
#define HITGROUP_RIGHTARM    5
#define HITGROUP_LEFTLEG    6
#define HITGROUP_RIGHTLEG    7
#define HITGROUP_GEAR        10

#include "SourceEngine/SDK.hpp"
#include "CSGOStructs.hpp"

#include "PushNotifications.h"

class Utils
{
public:
    static bool ScreenTransform(const SourceEngine::Vector& point, SourceEngine::Vector& screen)
    {
        const SourceEngine::VMatrix& w2sMatrix = SourceEngine::Interfaces::Engine()->WorldToScreenMatrix();
        screen.x = w2sMatrix.m[0][0] * point.x + w2sMatrix.m[0][1] * point.y + w2sMatrix.m[0][2] * point.z + w2sMatrix.m[0][3];
        screen.y = w2sMatrix.m[1][0] * point.x + w2sMatrix.m[1][1] * point.y + w2sMatrix.m[1][2] * point.z + w2sMatrix.m[1][3];
        screen.z = 0.0f;

        float w = w2sMatrix.m[3][0] * point.x + w2sMatrix.m[3][1] * point.y + w2sMatrix.m[3][2] * point.z + w2sMatrix.m[3][3];

		if (w < 0.001f)
			w = 0.001f;

        float invw = 1.0f / w;
        screen.x *= invw;
        screen.y *= invw;

        return false;
    }

    static bool WorldToScreen(const SourceEngine::Vector &origin, SourceEngine::Vector &screen)
    {
        if(!ScreenTransform(origin, screen)) {
            int iScreenWidth, iScreenHeight;
            SourceEngine::Interfaces::Engine()->GetScreenSize(iScreenWidth, iScreenHeight);

            screen.x = (iScreenWidth / 2.0f) + (screen.x * iScreenWidth) / 2;
            screen.y = (iScreenHeight / 2.0f) - (screen.y * iScreenHeight) / 2;

            return true;
        }
        return false;
    }

    static uint64_t FindSignature(const char* szModule, const char* szSignature)
    {
        //CREDITS: learn_more
    #define INRANGE(x,a,b)  (x >= a && x <= b) 
    #define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
    #define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

        MODULEINFO modInfo;
        GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(szModule), &modInfo, sizeof(MODULEINFO));
        DWORD startAddress = (DWORD)modInfo.lpBaseOfDll;
        DWORD endAddress = startAddress + modInfo.SizeOfImage;
        const char* pat = szSignature;
        DWORD firstMatch = 0;
        for(DWORD pCur = startAddress; pCur < endAddress; pCur++) {
            if(!*pat) return firstMatch;
            if(*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
                if(!firstMatch) firstMatch = pCur;
                if(!pat[2]) return firstMatch;
                if(*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
                else pat += 2;    //one ?
            } else {
                pat = szSignature;
                firstMatch = 0;
            }
        }
        return NULL;
    }

	using InitKeyValuesFn = void(__thiscall*)(void* thisptr, const char* name);
	static void InitKeyValues(KeyValues* pKeyValues, const char* name)
	{
		static InitKeyValuesFn InitKeyValuesEx = (InitKeyValuesFn)(FindSignature(XorStr("client.dll"), XorStr("55 8B EC 51 33 C0 C7 45")));
		InitKeyValuesEx(pKeyValues, name);
	}

	using LoadFromBufferFn = void(__thiscall*)(void* thisptr, const char* resourceName, const char* pBuffer, void* pFileSystem, const char* pPathID, void* pfnEvaluateSymbolProc);
	static void LoadFromBuffer(KeyValues* pKeyValues, const char* resourceName, const char* pBuffer, void* pFileSystem = nullptr, const char* pPathID = NULL, void* pfnEvaluateSymbolProc = nullptr)
	{
		static LoadFromBufferFn LoadFromBufferEx = (LoadFromBufferFn)(FindSignature(XorStr("client.dll"), XorStr("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04")));
		LoadFromBufferEx(pKeyValues, resourceName, pBuffer, pFileSystem, pPathID, pfnEvaluateSymbolProc);
	}

	static IMaterial* CreateMaterial(bool flat, bool ignorez, bool wireframed)
	{
		static int NumCustomTextures = 0;
		std::string type = (flat) ? XorStr("UnlitGeneric") : XorStr("VertexLitGeneric");

		std::string matdata = XorStr("\"") + type + XorStr("\"\n{\n\t\"$basetexture\" \"vgui/white_additive\"\n\t\"$envmap\"  \"\"\n\t\"$model\" \"1\"\n\t\"$flat\" \"1\"\n\t\"$nocull\"  \"0\"\n\t\"$selfillum\" \"1\"\n\t\"$halflambert\" \"1\"\n\t\"$nofog\"  \"0\"\n\t\"$znearer\" \"0\"\n\t\"$wireframe\" \"") + std::to_string(wireframed) + XorStr("\"\n\t\"$ignorez\" \"") + std::to_string(ignorez) + XorStr("\"\n}\n");

		std::string matname = XorStr("custom_") + std::to_string(NumCustomTextures);
		NumCustomTextures++;

		KeyValues* pKeyValues = new KeyValues(matname.c_str());
		InitKeyValues(pKeyValues, type.c_str());
		LoadFromBuffer(pKeyValues, matname.c_str(), matdata.c_str());

		IMaterial* createdMaterial = Interfaces::MaterialSystem()->CreateMaterial(matname.c_str(), pKeyValues);

		createdMaterial->IncrementReferenceCount();

		return createdMaterial;
	}

    static void Clamp(SourceEngine::QAngle &angles)
    {
		NormalizeAngles(angles);
        ClampAngles(angles);
    }
	static bool IsViewRayClear(SourceEngine::Vector start, SourceEngine::Vector end)
	{
		SourceEngine::Ray_t ray;
		SourceEngine::CGameTrace tr;
		SourceEngine::CTraceFilter filter;
		filter.pSkip = C_CSPlayer::GetLocalPlayer();

		SourceEngine::Vector _stub = (end - start).Normalized() * 4.5f;

		ray.Init(start, end - _stub);
		SourceEngine::Interfaces::EngineTrace()->TraceRay(ray, 0x4600400B, &filter, &tr);

		return tr.fraction > 0.97f;
	}
	static bool DoesViewRayHitEntity(SourceEngine::Vector start, SourceEngine::Vector end)
	{
		SourceEngine::Ray_t ray;
		SourceEngine::CGameTrace tr;
		SourceEngine::CTraceFilter filter;
		filter.pSkip = C_CSPlayer::GetLocalPlayer();

		SourceEngine::Vector _stub = (end - start).Normalized() * 4.5f;

		ray.Init(start, end - _stub);
		SourceEngine::Interfaces::EngineTrace()->TraceRay(ray, 0x4600400B, &filter, &tr);
		
		return tr.fraction > 0.97f;
	}
	static float GetTraceFraction(SourceEngine::Vector start, SourceEngine::Vector end, int mask = 0x4600400B)
	{
		SourceEngine::Ray_t ray;
		SourceEngine::CGameTrace tr;
		SourceEngine::CTraceFilter filter;
		filter.pSkip = C_CSPlayer::GetLocalPlayer();

		ray.Init(start, end);
		SourceEngine::Interfaces::EngineTrace()->TraceRay(ray, mask, &filter, &tr);

		return tr.fraction;
	}
	static float GetTraceFractionWorldProps(SourceEngine::Vector start, SourceEngine::Vector end)
	{
		SourceEngine::Ray_t ray;
		SourceEngine::CGameTrace tr;
		SourceEngine::CTraceFilterWorldAndPropsOnly filter;

		ray.Init(start, end);
		SourceEngine::Interfaces::EngineTrace()->TraceRay(ray, MASK_SOLID, &filter, &tr);

		return tr.fraction;
	}
	static bool IsViableTarget(C_CSPlayer* pEntity)
	{
		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();
		if (!pEntity) return false;
		if (pEntity->GetClientClass()->m_ClassID != SourceEngine::EClassIds::CCSPlayer) return false;
		if (pEntity == pLocal) return false;
		if (pEntity->GetTeamNum() == pLocal->GetTeamNum()) return false;
		if (pEntity->GetGunGameImmunity()) return false;
		if (!pEntity->IsAlive() || pEntity->IsDormant()) return false;
		return true;
	}
	inline static bool LocalCheck()
	{
		if (!(SourceEngine::Interfaces::Engine()->IsConnected() && SourceEngine::Interfaces::Engine()->IsInGame())) return false;
		auto pLocal = C_CSPlayer::GetLocalPlayer();
		if (!pLocal) return false;
		if (!pLocal->IsAlive()) return false;
		if (!pLocal->GetActiveWeapon()) return false;
		if (!pLocal->GetActiveWeapon()->GetItemDefinitionIndex()) return false;
		if (!pLocal->GetActiveWeapon()->GetCSWpnData()) return false;
		return true;
	}
	static int GetEnemyPlayersAlive() {
		int AlivePlayers = 0;
		auto pLocal = C_CSPlayer::GetLocalPlayer();
		for (int i = 0; i < 65; i++) {
			if (i == Interfaces::Engine()->GetLocalPlayer()) continue;
			C_CSPlayer* pEnt = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(i));
			if (!pEnt) continue;
			if (pEnt->GetTeamNum() == pLocal->GetTeamNum() || (pEnt->GetTeamNum() != 3 && pEnt->GetTeamNum() != 2)) continue;
			if (pEnt->IsAlive())
				AlivePlayers++;
		}
		return AlivePlayers;
	}
	static void SetClanTag(const char* tag)
	{
		static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(FindSignature(XorStr("engine.dll"), XorStr("53 56 57 8B DA 8B F9 FF 15")));
		pSetClanTag(tag, tag);
	}
	static void NormalizeAngle(float& flAngle)
	{
		if (std::isnan(flAngle)) flAngle = 0.0f;
		if (std::isinf(flAngle)) flAngle = 0.0f;

		float flRevolutions = flAngle / 360;

		if (flAngle > 180 || flAngle < -180)
		{
			if (flRevolutions < 0)
				flRevolutions = -flRevolutions;

			flRevolutions = round(flRevolutions);

			if (flAngle < 0)
				flAngle = (flAngle + 360 * flRevolutions);
			else
				flAngle = (flAngle - 360 * flRevolutions);
		}
	}
	static void NormalizeAngles(SourceEngine::QAngle &angles)
	{
		NormalizeAngle(angles.x);
		NormalizeAngle(angles.y);
		angles.z = 0;
	}
	static inline float DegreesToRadians(float Angle)
	{
		return Angle * M_PI / 180.0f;
	}
	// Criticise me on this as well my dudes
	typedef bool(__fastcall* TraceToExit_t)(Vector&, trace_t&, float, float, float, float, float, float, trace_t*);
	static bool TraceToExit(Vector& end, trace_t& tr, Vector start, Vector dir, trace_t* trace)
	{
		static TraceToExit_t TraceToExitF = (TraceToExit_t)Utils::FindSignature(XorStr("client.dll"), XorStr("55 8B EC 83 EC 30 F3 0F 10 75"));
		if (!TraceToExitF) { return false; }
		_asm
		{
			push trace
			push dir.z
			push dir.y
			push dir.x
			push start.z
			push start.y
			push start.x
			mov edx, tr
			mov ecx, end
			call TraceToExitF
			add esp, 0x1C
		}
	}
	typedef void(__fastcall* ClipTraceToPlayers_t)(const Vector&, const Vector&, unsigned int, ITraceFilter*, trace_t*);
	static void ClipTraceToPlayers(const Vector& start, const Vector& end, unsigned int mask, ITraceFilter* filter, trace_t* tr)
	{
		static ClipTraceToPlayers_t ClipTraceToPlayersF = (ClipTraceToPlayers_t)Utils::FindSignature(XorStr("client.dll"), XorStr("53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 8B 43 10"));
		ClipTraceToPlayersF(start, end, mask, filter, tr);
	}
private:
	static void ClampAngles(SourceEngine::QAngle &angles)
	{
		if (std::isnan(angles.x)) angles.x = 0.0f;
		if (std::isnan(angles.y)) angles.y = 0.0f;
		if (std::isnan(angles.z)) angles.z = 0.0f;
		if (std::isinf(angles.x)) angles.x = 0.0f;
		if (std::isinf(angles.y)) angles.y = 0.0f;
		if (std::isinf(angles.z)) angles.z = 0.0f;

		if (angles.x > 89.0f && angles.x <= 180.0f)
			angles.x = 89.0f;
		while (angles.x > 180.0f)
			angles.x -= 360.0f;
		while (angles.x < -89.0f)
			angles.x = -89.0f;
		while (angles.y > 180.0f)
			angles.y -= 360.0f;
		while (angles.y < -180.0f)
			angles.y += 360.0f;

		angles.z = 0;
	}
};
