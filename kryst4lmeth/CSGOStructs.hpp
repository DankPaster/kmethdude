#pragma once

#include <Windows.h>
#include "SourceEngine/SDK.hpp"
#include "NetvarManager.hpp"
#include "SourceEngine/Definitions.hpp"
#include "PushNotifications.h"
#include "XorStr.hpp"
#include "Options.hpp"

#define TIME_TO_TICKS( dt )	( ( int )( 0.5f + ( float )( dt ) / Interfaces::GlobalVars()->interval_per_tick ) )

extern float OldSimulationTimes[65];

class CHudTexture;

class WeaponInfo_t
{
public:

	virtual ~WeaponInfo_t() {};

	char*		consoleName;			// 0x0004
	char		pad_0008[12];			// 0x0008
	int			iMaxClip1;				// 0x0014
	int			iMaxClip2;				// 0x0018
	int			iDefaultClip1;			// 0x001C
	int			iDefaultClip2;			// 0x0020
	char		pad_0024[8];			// 0x0024
	char*		szWorldModel;			// 0x002C
	char*		szViewModel;			// 0x0030
	char*		szDroppedModel;			// 0x0034
	char		pad_0038[4];			// 0x0038
	char*		N0000023E;				// 0x003C
	char		pad_0040[56];			// 0x0040
	char*		szEmptySound;			// 0x0078
	char		pad_007C[4];			// 0x007C
	char*		szBulletType;			// 0x0080
	char		pad_0084[4];			// 0x0084
	char*		szHudName;				// 0x0088
	char*		szWeaponName;			// 0x008C
	char		pad_0090[56];			// 0x0090
	int 		WeaponType;				// 0x00C8
	int			iWeaponPrice;			// 0x00CC
	int			iKillAward;				// 0x00D0
	char*		szAnimationPrefix;		// 0x00D4
	float		flCycleTime;			// 0x00D8
	float		flCycleTimeAlt;			// 0x00DC
	float		flTimeToIdle;			// 0x00E0
	float		flIdleInterval;			// 0x00E4
	bool		bFullAuto;				// 0x00E8
	char		pad_0x00E5[3];			// 0x00E9
	int			iDamage;				// 0x00EC
	float		flArmorRatio;			// 0x00F0
	int			iBullets;				// 0x00F4
	float		flPenetration;			// 0x00F8
	float		flFlinchVelocityModifierLarge;	// 0x00FC
	float		flFlinchVelocityModifierSmall;	// 0x0100
	float		flRange;				// 0x0104
	float		flRangeModifier;		// 0x0108
	float		flThrowVelocity;		// 0x010C
	char		pad_0x010C[12];			// 0x0110
	bool		bHasSilencer;			// 0x011C
	char		pad_0x0119[3];			// 0x011D
	char*		pSilencerModel;			// 0x0120
	int			iCrosshairMinDistance;	// 0x0124
	int			iCrosshairDeltaDistance;// 0x0128 - iTeam?
	float		flMaxPlayerSpeed;		// 0x012C
	float		flMaxPlayerSpeedAlt;	// 0x0130
	float		flSpread;				// 0x0134
	float		flSpreadAlt;			// 0x0138
	float		flInaccuracyCrouch;		// 0x013C
	float		flInaccuracyCrouchAlt;	// 0x0140
	float		flInaccuracyStand;		// 0x0144
	float		flInaccuracyStandAlt;	// 0x0148
	float		flInaccuracyJumpInitial;// 0x014C
	float		flInaccuracyJump;		// 0x0150
	float		flInaccuracyJumpAlt;	// 0x0154
	float		flInaccuracyLand;		// 0x0158
	float		flInaccuracyLandAlt;	// 0x015C
	float		flInaccuracyLadder;		// 0x0160
	float		flInaccuracyLadderAlt;	// 0x0164
	float		flInaccuracyFire;		// 0x0168
	float		flInaccuracyFireAlt;	// 0x016C
	float		flInaccuracyMove;		// 0x0170
	float		flInaccuracyMoveAlt;	// 0x0174
	float		flInaccuracyReload;		// 0x0178
	int			iRecoilSeed;			// 0x017C
	float		flRecoilAngle;			// 0x0180
	float		flRecoilAngleAlt;		// 0x0184
	float		flRecoilAngleVariance;	// 0x0188
	float		flRecoilAngleVarianceAlt;	// 0x018C
	float		flRecoilMagnitude;		// 0x0190
	float		flRecoilMagnitudeAlt;	// 0x0194
	float		flRecoilMagnitudeVariance;	// 0x0198
	float		flRecoilMagnitudeVarianceAlt;	// 0x019C
	float		flRecoveryTimeCrouch;	// 0x01A0
	float		flRecoveryTimeStand;	// 0x01A4
	float		flRecoveryTimeCrouchFinal;	// 0x01A8
	float		flRecoveryTimeStandFinal;	// 0x01AC
	int			iRecoveryTransitionStartBullet;// 0x01B0 
	int			iRecoveryTransitionEndBullet;	// 0x01B4
	bool		bUnzoomAfterShot;		// 0x01B8
	bool		bHideViewModelZoomed;	// 0x01B9
	char		pad_0x01B5[2];			// 0x01BA
	char		iZoomLevels[3];			// 0x01BC
	int			iZoomFOV[2];			// 0x01C0
	float		fZoomTime[3];			// 0x01C4
	char*		szWeaponClass;			// 0x01D4
	float		flAddonScale;			// 0x01D8
	char		pad_0x01DC[4];			// 0x01DC
	char*		szEjectBrassEffect;		// 0x01E0
	char*		szTracerEffect;			// 0x01E4
	int			iTracerFrequency;		// 0x01E8
	int			iTracerFrequencyAlt;	// 0x01EC
	char*		szMuzzleFlashEffect_1stPerson; // 0x01F0
	char		pad_0x01F4[4];			 // 0x01F4
	char*		szMuzzleFlashEffect_3rdPerson; // 0x01F8
	char		pad_0x01FC[4];			// 0x01FC
	char*		szMuzzleSmokeEffect;	// 0x0200
	float		flHeatPerShot;			// 0x0204
	char*		szZoomInSound;			// 0x0208
	char*		szZoomOutSound;			// 0x020C
	float		flInaccuracyPitchShift;	// 0x0210
	float		flInaccuracySoundThreshold;	// 0x0214
	float		flBotAudibleRange;		// 0x0218
	char		pad_0x0218[8];			// 0x0220
	char*		pWrongTeamMsg;			// 0x0224
	bool		bHasBurstMode;			// 0x0228
	char		pad_0x0225[3];			// 0x0229
	bool		bIsRevolver;			// 0x022C
	bool		bCannotShootUnderwater;	// 0x0230
};

class C_BaseCombatWeapon;

class DT_EconEntity : public SourceEngine::IClientEntity
{
	template<class T>
	inline T GetFieldValue(int offset)
	{
		return *(T*)((DWORD)this + offset);
	}
	template<class T>
	T* GetFieldPointer(int offset)
	{
		return (T*)((DWORD)this + offset);
	}
public:
	void SetWear(float Wear)
	{

	}
	void SetPaintKit(int ID)
	{

	}
	void SetSeed(float Seed)
	{

	}
	void SetStatTrak(int Kills)
	{

	}
};

class C_CSPlayer : public SourceEngine::IClientEntity
{
	template<class T>
	inline T GetFieldValue(int offset)
	{
		return *(T*)((DWORD)this + offset);
	}
	template<class T>
	inline void SetFieldValue(int offset, T dat)
	{
		*(T*)((DWORD)this + offset) = dat;
	}
	template<class T>
	T* GetFieldPointer(int offset)
	{
		return (T*)((DWORD)this + offset);
	}
public:
	static C_CSPlayer* GetLocalPlayer()
	{
		return (C_CSPlayer*)SourceEngine::Interfaces::EntityList()->GetClientEntity(SourceEngine::Interfaces::Engine()->GetLocalPlayer());
	}
	C_BaseCombatWeapon* GetActiveWeapon()
	{
		using namespace SourceEngine;
		static int m_hActiveWeapon = GET_NETVAR(XorStr("DT_BaseCombatCharacter"), XorStr("m_hActiveWeapon"));
		return (C_BaseCombatWeapon*)Interfaces::EntityList()->GetClientEntityFromHandle(GetFieldValue<CHandle<IClientEntity>>(m_hActiveWeapon));
	}
	int GetHealth()
	{
		static int m_iHealth = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_iHealth"));
		return GetFieldValue<int>(m_iHealth);
	}
	bool IsAlive()
	{
		static int m_lifeState = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_lifeState"));
		return GetFieldValue<int>(m_lifeState) == 0;
	}
	int GetTeamNum()
	{
		static int m_iTeamNum = GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_iTeamNum"));
		return GetFieldValue<int>(m_iTeamNum);
	}
	int GetFlags()
	{
		static int m_fFlags = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_fFlags"));
		return GetFieldValue<int>(m_fFlags);
	}
	SourceEngine::Vector GetViewOffset()
	{
		static int m_vecViewOffset = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_vecViewOffset[0]"));
		return GetFieldValue<SourceEngine::Vector>(m_vecViewOffset);
	}
	SourceEngine::Vector GetVelocity()
	{
		static int m_vecVelocity = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_vecVelocity[0]"));
		return GetFieldValue<SourceEngine::Vector>(m_vecVelocity);
	}
	SourceEngine::Vector GetOrigin()
	{
		static int m_vecOrigin = GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_vecOrigin"));
		return GetFieldValue<SourceEngine::Vector>(m_vecOrigin);
	}
	SourceEngine::Vector GetEyePos()
	{
		return GetOrigin() + GetViewOffset();
	}
	SourceEngine::Vector* ViewPunch()
	{
		static int m_viewPunchAngle = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_Local"), XorStr("m_viewPunchAngle"));
		return GetFieldPointer<SourceEngine::Vector>(m_viewPunchAngle);
	}
	SourceEngine::Vector* AimPunch()
	{
		static int m_aimPunchAngle = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_Local"), XorStr("m_aimPunchAngle"));
		return GetFieldPointer<SourceEngine::Vector>(m_aimPunchAngle);
	}
	SourceEngine::QAngle GetEyeAngles()
	{
		static int m_viewAngle = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_angEyeAngles"));
		return GetFieldValue<SourceEngine::QAngle>(m_viewAngle);
	}
	SourceEngine::QAngle* EyeAngles()
	{
		static int m_viewAngle = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_angEyeAngles"));
		return GetFieldPointer<SourceEngine::QAngle>(m_viewAngle);
	}
	int InCross()
	{
		SourceEngine::Ray_t ray;
		SourceEngine::CGameTrace tr;
		SourceEngine::CTraceFilter filter;
		filter.pSkip = C_CSPlayer::GetLocalPlayer();

		Vector LocalEyePos = GetEyePos();
		QAngle EyeAngles;
		Interfaces::Engine()->GetViewAngles(EyeAngles);
		Vector EndPos = LocalEyePos + (EyeAngles).Direction() * 8192.0f;
		ray.Init(LocalEyePos, EndPos);
		SourceEngine::Interfaces::EngineTrace()->TraceRay(ray, 0x4600400B, &filter, &tr);

		if (tr.m_pEnt)
			return tr.m_pEnt->GetClientNetworkable()->EntIndex();
		return 0;
	}
	int GetMoveType()
	{
		return GetFieldValue<int>(0x258);
	}
	bool GetGunGameImmunity()
	{
		static int m_bGunGameImmunity = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_bGunGameImmunity"));
		return GetFieldValue<bool>(m_bGunGameImmunity);
	}
	Vector GetNetworkOrigin() {
		static int m_flFriction = GET_NETVAR("DT_BasePlayer", "localdata", "m_flFriction") - sizeof(Vector);
		return GetFieldValue<Vector>(m_flFriction);
	}
	float GetSimulationTime()
	{
		static int m_flSimulationTime = GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_flSimulationTime"));
		return GetFieldValue<float>(m_flSimulationTime);
	}
	float GetLowerBodyYawTarget()
	{
		static int m_flLowerBodyYawTarget = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_flLowerBodyYawTarget"));
		return GetFieldValue<float>(m_flLowerBodyYawTarget);
	}
	float GetPoseParameter()
	{
		static int m_flPoseParameter = GET_NETVAR(XorStr("DT_BaseAnimating"), XorStr("m_flPoseParameter"));
		return GetFieldValue<float>(m_flPoseParameter);
	}
	int GetChokedPackets()
	{
		if (GetSimulationTime() > OldSimulationTimes[EntIndex()])
			return TIME_TO_TICKS(fabs(GetSimulationTime() - OldSimulationTimes[EntIndex()]));
		return 0;
	}
	SourceEngine::Vector GetBone(int Bone)
	{
		SourceEngine::matrix3x4_t boneMatrix[128];
		if (!SetupBones(boneMatrix, 128, 0x00000100, SourceEngine::Interfaces::Engine()->GetLastTimeStamp()))
			return SourceEngine::Vector();
		SourceEngine::matrix3x4_t hitbox = boneMatrix[Bone];
		SourceEngine::Vector BonePos(hitbox[0][3], hitbox[1][3], hitbox[2][3]);
		if (GetChokedPackets() > 5 && Config.g_iFakeLagFix == 1)
		{
			BonePos -= GetOrigin();
			BonePos += GetNetworkOrigin();
		}
		return BonePos;
	}
	Vector GetBoneByName(std::string name)
	{
		auto Mdl = GetModel();
		if (!Mdl) return Vector();
		studiohdr_t* pStudioHdr = Interfaces::ModelInfo()->GetStudioModel(Mdl);
		if (!pStudioHdr) return Vector();
		for (int j = 0; j < pStudioHdr->numbones; j++)
		{
			mstudiobone_t* pBone = pStudioHdr->pBone(j);
			if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && std::string(pBone->pszName()) == name)
				return GetBone(j);
		}
		return Vector();
	}
	int HitboxSet()
	{
		static int m_nHitboxSet = GET_NETVAR(XorStr("DT_BaseAnimating"), XorStr("m_nHitboxSet"));
		return GetFieldValue<int>(m_nHitboxSet);
	}
	mstudiohitboxset_t* GetHitBoxSet()
	{
		if (!GetModel()) return false;
		studiohdr_t* pHeader = Interfaces::ModelInfo()->GetStudioModel(GetModel());
		if (!pHeader) return nullptr;
		mstudiohitboxset_t* pHitBoxSet = pHeader->pHitboxSet(HitboxSet());
		if (!pHitBoxSet) return nullptr;
		return pHitBoxSet;
	}
	Vector GetHeadCenterPos()
	{
		SourceEngine::Vector bonePos = GetBoneByName(XorStr("head_0"));
		if (!bonePos.IsValid()) return Vector();
		SourceEngine::Vector up = GetEyeAngles().Up();
		SourceEngine::Vector Offset = up * 1.15f;
		return bonePos + Offset;
	}
	bool IsScoped()
	{
		int m_bIsScoped = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_bIsScoped"));
		return GetFieldValue<bool>(m_bIsScoped);
	}
	int GetArmor()
	{
		static int m_ArmorValue = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_ArmorValue"));
		return GetFieldValue<int>(m_ArmorValue);
	}
	bool HasHelmet()
	{
		static int m_bHasHelmet = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_bHasHelmet"));
		return GetFieldValue<bool>(m_bHasHelmet);
	}
	int GetTickBase()
	{
		static int m_nTickBase = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_nTickBase"));
		return GetFieldValue<int>(m_nTickBase);
	}
	int GetMoveState()
	{
		static int m_iMoveState = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_iMoveState"));
		return GetFieldValue<int>(m_iMoveState);
	}
	int GetPlayerState()
	{
		static int m_iPlayerState = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_iPlayerState"));
		return GetFieldValue<int>(m_iPlayerState);
	}
	SourceEngine::Vector GetPunchAngleVelocity()
	{
		static int m_aimPunchAngleVel = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_Local"), XorStr("m_aimPunchAngleVel"));
		return GetFieldValue<SourceEngine::Vector>(m_aimPunchAngleVel);
	}
	SourceEngine::QAngle* PlayerAngles()
	{
		static int m_vecAngles = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("pl"), XorStr("deadflag")) + 0x4;
		return GetFieldPointer<SourceEngine::QAngle>(m_vecAngles);
	}
	char* GetLastPlaceName()
	{
		static int m_szLastPlaceName = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_szLastPlaceName"));
		return GetFieldPointer<char>(m_szLastPlaceName);
	}
	float GetMaxSpeed()
	{
		static int m_flMaxspeed = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_flMaxspeed"));
		return GetFieldValue<float>(m_flMaxspeed);
	}
	int GetScoreboardKills()
	{
		static int m_iMatchStats_Kills = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_iMatchStats_Kills"));
		return GetFieldValue<int>(m_iMatchStats_Kills);
	}
	int GetScoreboardDeaths()
	{
		static int m_iMatchStats_Deaths = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_iMatchStats_Deaths"));
		return GetFieldValue<int>(m_iMatchStats_Deaths);
	}
	float GetNextAttack()
	{
		static int m_flNextAttack = GET_NETVAR(XorStr("DT_BaseCombatCharacter"), XorStr("bcc_localdata"), XorStr("m_flNextAttack"));
		return GetFieldValue<float>(m_flNextAttack);
	}
	bool IsReloading()
	{
		return GetFieldValue<bool>(0x3235);
	}
};

class C_BaseCombatWeapon : public SourceEngine::IClientEntity
{
	template<class T>
	inline T GetFieldValue(int offset)
	{
		return *(T*)((DWORD)this + offset);
	}
public:
    C_CSPlayer* GetOwner()
    {
        using namespace SourceEngine;
        static int m_hOwnerEntity = GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_hOwnerEntity"));
        return (C_CSPlayer*)Interfaces::EntityList()->GetClientEntityFromHandle(GetFieldValue<CHandle<C_CSPlayer>>(m_hOwnerEntity));
    }
    float NextPrimaryAttack()
    {
        static int m_flNextPrimaryAttack = GET_NETVAR(XorStr("DT_BaseCombatWeapon"), XorStr("LocalActiveWeaponData"), XorStr("m_flNextPrimaryAttack"));
        return GetFieldValue<float>(m_flNextPrimaryAttack);
    }
	int GetClip1()
	{
		static int m_iClip1 = GET_NETVAR(XorStr("DT_BaseCombatWeapon"), XorStr("m_iClip1"));
		return GetFieldValue<int>(m_iClip1);
	}
	int GetClip2()
	{
		static int m_iClip2 = GET_NETVAR(XorStr("DT_BaseCombatWeapon"), XorStr("m_iClip2"));
		return GetFieldValue<int>(m_iClip2);
	}
	float GetAccuracyPenalty()
	{
		static int m_fAccuracyPenalty = GET_NETVAR(XorStr("DT_WeaponCSBase"), XorStr("m_fAccuracyPenalty"));
		return GetFieldValue<float>(m_fAccuracyPenalty);
	}
	SourceEngine::EItemDefinitionIndex GetItemDefinitionIndex()
	{
		static int m_iItemDefinitionIndex = GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iItemDefinitionIndex"));
		return GetFieldValue<SourceEngine::EItemDefinitionIndex>(m_iItemDefinitionIndex);
	}
	WeaponInfo_t* GetCSWpnData()
	{
		typedef WeaponInfo_t*(__thiscall* tGetCSWpnData)(void*);
		return SourceEngine::CallVFunction<tGetCSWpnData>(this, 446)(this);
	}
	bool IsAutomatic()
	{
		int Index = GetItemDefinitionIndex();
		if (Index == 7 || Index == 8 || Index == 10 || Index == 11 || Index == 13 || Index == 14 || Index == 16 || Index == 17 || Index == 19 || Index == 24 || Index == 25 ||
			Index == 26 || Index == 28 || Index == 33 || Index == 34 || Index == 38 || Index == 39 || Index == 63)
			return true;
		return false;
	}
	bool IsSemiAutomatic()
	{
		return !IsAutomatic();
	}
	bool IsKnife()
	{
		int Index = GetItemDefinitionIndex();
		if (Index == 42 || Index == 59 || Index >= 500)
			return true;
		return false;
	}
	bool IsSniper()
	{
		int Index = GetItemDefinitionIndex();
		if (Index == 9 || Index == 11 || Index == 40 || Index == 38)
			return true;
		return false;
	}
	bool IsAR()
	{
		int Index = GetItemDefinitionIndex();
		if (Index == 7 || Index == 8 || Index == 10 || Index == 13 || Index == 16 || Index == 39 || Index == 60)
			return true;
		return false;
	}
	bool IsPistol()
	{
		int Index = GetItemDefinitionIndex();
		if (Index == 1 || Index == 2 || Index == 3 || Index == 4 || Index == 30 || Index == 36 || Index == 61 || Index == 63 || Index == 64)
			return true;
		return false;
	}
	bool IsGrenade()
	{
		int Index = GetItemDefinitionIndex();
		if (Index == 43 || Index == 44 || Index == 45 || Index == 46 || Index == 47 || Index == 48)
			return true;
		return false;
	}
	bool IsC4()
	{
		int Index = GetItemDefinitionIndex();
		if (Index == 49)
			return true;
		return false;
	}
	bool CanFire()
	{
		auto Owner = GetOwner();
		if (!Owner) return false;
		return !(Owner->IsReloading()) && NextPrimaryAttack() <= Owner->GetTickBase() * SourceEngine::Interfaces::GlobalVars()->interval_per_tick && Owner->GetNextAttack() <= Owner->GetTickBase() * SourceEngine::Interfaces::GlobalVars()->interval_per_tick;
	}
	std::string GetWeaponName()
	{
		int ID = GetItemDefinitionIndex();
		using namespace SourceEngine;
		if (ID == weapon_deagle) return XorStr("Desert Eagle");
		if (ID == weapon_elite) return XorStr("Dual Berettas");
		if (ID == weapon_fiveseven) return XorStr("Five-SeveN");
		if (ID == weapon_glock) return XorStr("Glock-18");
		if (ID == weapon_ak47) return XorStr("AK-47");
		if (ID == weapon_aug) return XorStr("AUG");
		if (ID == weapon_awp) return XorStr("AWP");
		if (ID == weapon_famas) return XorStr("FAMAS");
		if (ID == weapon_g3sg1) return XorStr("G3SG1");
		if (ID == weapon_galilar) return XorStr("Galil AR");
		if (ID == weapon_m249) return XorStr("M249");
		if (ID == weapon_m4a1) return XorStr("M4A4");
		if (ID == weapon_mac10) return XorStr("MAC-10");
		if (ID == weapon_p90) return XorStr("P90");
		if (ID == weapon_ump) return XorStr("UMP-45");
		if (ID == weapon_xm1014) return XorStr("XM1014");
		if (ID == weapon_bizon) return XorStr("PP-Bizon");
		if (ID == weapon_mag7) return XorStr("MAG-7");
		if (ID == weapon_negev) return XorStr("Negev");
		if (ID == weapon_sawedoff) return XorStr("Sawed-Off");
		if (ID == weapon_tec9) return XorStr("Tec-9");
		if (ID == weapon_taser) return XorStr("Zeus");
		if (ID == weapon_hkp2000) return XorStr("P2000");
		if (ID == weapon_mp7) return XorStr("MP7");
		if (ID == weapon_mp9) return XorStr("MP9");
		if (ID == weapon_nova) return XorStr("Nova");
		if (ID == weapon_p250) return XorStr("P250");
		if (ID == weapon_scar20) return XorStr("SCAR-20");
		if (ID == weapon_sg556) return XorStr("SG-553");
		if (ID == weapon_ssg08) return XorStr("SSG-08");
		if (ID == weapon_knife) return XorStr("Knife");
		if (ID == weapon_flashbang) return XorStr("Flashbang");
		if (ID == weapon_hegrenade) return XorStr("HE Grenade");
		if (ID == weapon_smokegrenade) return XorStr("Smoke Grenade");
		if (ID == weapon_molotov) return XorStr("Molotov");
		if (ID == weapon_decoy) return XorStr("Decoy");
		if (ID == weapon_incgrenade) return XorStr("Incendiary");
		if (ID == weapon_c4) return XorStr("C4 Explosive");
		if (ID == weapon_knife_t) return XorStr("Knife");
		if (ID == weapon_m4a1_silencer) return XorStr("M4A1-S");
		if (ID == weapon_usp_silencer) return XorStr("USP-S");
		if (ID == weapon_cz75a) return XorStr("CZ75-Auto");
		if (ID == weapon_revolver) return XorStr("R8 Revolver");
		if (ID == weapon_bayonet) return XorStr("Bayonet");
		if (ID == weapon_knife_flip) return XorStr("Flip Knife");
		if (ID == weapon_knife_gut) return XorStr("Gut Knife");
		if (ID == weapon_knife_karambit) return XorStr("Karambit");
		if (ID == weapon_knife_m9_bayonet) return XorStr("M9 Bayonet");
		if (ID == weapon_knife_tactical) return XorStr("Huntsman Knife");
		if (ID == weapon_knife_falchion) return XorStr("Falchion Knife");
		if (ID == weapon_knife_survival_bowie) return XorStr("Bowie Knife");
		if (ID == weapon_knife_butterfly) return XorStr("Butterfly Knife");
		if (ID == weapon_knife_push) return XorStr("Shadow Daggers");
		return XorStr("");
	}
};