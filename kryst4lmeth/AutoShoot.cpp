#include "AutoShoot.h"
#include <Psapi.h>
#include <Windows.h>
#include <cstdlib>
#include <cstdio>
#include <math.h>
#include <string>
#include <conio.h>
#include <vector>
#include "md5.h"
#include "Utils.hpp"
#include "Hooks.hpp"
#include "Resolver.h"

bool AutoShoot::IsFiring = false;
int LastWeaponIndex = 0;
int AutoShoot::LastTarget = 0;

inline float DegreesToRadians(float Angle) { return Angle * M_PI / 180.0f; }
ConVar* weapon_accuracy_nospread = SourceEngine::Interfaces::CVar()->FindVar(XorStr("weapon_accuracy_nospread"));

// I know, it's fucking terrible
float AutoShoot::GetHitChance(Vector Point)
{
	if (weapon_accuracy_nospread->GetInt() == 1) return 100.0f;
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	auto ActiveWeapon = pLocal->GetActiveWeapon();
	auto WeaponData = ActiveWeapon->GetCSWpnData();
	float SpreadCone = ActiveWeapon->GetAccuracyPenalty() * 256.0f / M_PI + ActiveWeapon->GetCSWpnData()->flInaccuracyMove * pLocal->GetVelocity().Length() / 3000.0f;
	float a = (Point - pLocal->GetEyePos()).Length();
	float b = sqrt(tan(SpreadCone * M_PI / 180.0f) * a);
	if (2.2f > b) return 100.0f;
	return (2.2f / fmax(b, 2.2f)) * 100.0f;
}

void AutoShoot::Fire(CUserCmd* pCmd, byte* bSendPacket)
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	if (pLocal->GetActiveWeapon()->IsKnife() || pLocal->GetActiveWeapon()->IsGrenade() || pLocal->GetActiveWeapon()->IsC4())
	{
		AutoShoot::IsFiring = false;
		AutoShoot::LastTarget = 0;
		return;
	}
	if (Config.g_iAutoShootMode == 0)
	{
		int Target = AutoShoot::FindOptimalTarget();
		if (Target)
		{
			/////////////////////////////////////////
			// I honestly fucking gave up on lagfix and pasted some shit cause anue wouldn't stop bitching.
			/////////////////////////////////////////
			auto pEntity = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(Target));
			if (Config.g_iFakeLagFix == 1)
			{
				if (pEntity->GetChokedPackets())
				{
					AutoShoot::IsFiring = false;
					return;
				}
			}
			AutoShoot::FireAtTarget(Target, pCmd);
			AutoShoot::IsFiring = true;
			AutoShoot::LastTarget = Target;
		}
		else
		{
			AutoShoot::IsFiring = false;
			AutoShoot::LastTarget = 0;
		}
	}
	else
	{
		auto Result = AutoShoot::FindOptimalTarget_HS();
		if (Result.IsValid)
		{
			auto pEntity = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(Result.Target));
			if (Config.g_iFakeLagFix == 1)
			{
				if (pEntity->GetChokedPackets())
				{
					AutoShoot::IsFiring = false;
					return;
				}
			}
			AutoShoot::FireAtResult(Result, pCmd);
			AutoShoot::IsFiring = true;
			AutoShoot::LastTarget = Result.Target;
		}
		else
		{
			AutoShoot::IsFiring = false;
			AutoShoot::LastTarget = 0;
		}
	}
	if (AutoShoot::IsFiring && Config.g_iViewMode)
		*bSendPacket = false;
}

HitScanResult AutoShoot::ScanTarget(int Target)
{
	HitScanResult Result;
	for (int i = 0; i < 8; i++)
		Result.OptimalPoint[i] = SourceEngine::Vector(0, 0, 0);

	if (!Target) return HitScanResult();
	auto pTarget = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(Target));
	if (!pTarget) return HitScanResult();

	auto pLocal = C_CSPlayer::GetLocalPlayer();
	if (pLocal->GetActiveWeapon()->IsKnife()) return HitScanResult();

	SourceEngine::Vector LocalEyePos = pLocal->GetEyePos();
	SourceEngine::Vector LocalPos = pLocal->GetOrigin();
	SourceEngine::Vector HeadPos = pTarget->GetHeadCenterPos();
	SourceEngine::Vector EntityOrigin = pTarget->GetOrigin();

	if (GetHitChance(EntityOrigin) < Config.g_iMinHitChance) return HitScanResult();

	SourceEngine::QAngle ViewAngles;
	ViewAngles = (EntityOrigin - LocalPos).Angle();
	float yaw = -ViewAngles.y * M_PI / 180.0f;
	float pitch = -ViewAngles.x * M_PI / 180.0f;

	float Vert = pow(fabs(sin(pitch)), 3);
	float XBounds = Vert * 60.0f + (1.0f - Vert) * 50.0f;
	float YBounds = Vert * XBounds + (1.0f - Vert) * ((HeadPos.z + 12) - (EntityOrigin.z - 12));
	float XStep = ((1.0f - powf((Config.g_iHitScan_Resolution / 100.0f), 0.33f)) * 10.0f + 1) * 2.0f;
	float YStep = ((1.0f - powf((Config.g_iHitScan_Resolution / 100.0f), 0.33f)) * 10.0f + 1) * 2.0f;
	SourceEngine::Vector CenterPos = EntityOrigin + SourceEngine::Vector(0, 0, (YBounds / 2.0f - 12));
	auto wpn_data = pLocal->GetActiveWeapon()->GetCSWpnData();

	int HitCount[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	for (float XCoord = 0; XCoord <= XBounds; XCoord += XStep)
	{
		for (float YCoord = 0; YCoord <= YBounds; YCoord += YStep)
		{
			SourceEngine::Vector2D CurrentPoint;

			CurrentPoint.x = -(XBounds / 2.0f) + XCoord;
			CurrentPoint.y = -(YBounds / 2.0f) + YCoord;

			SourceEngine::Vector CurrentPosOffset;

			CurrentPosOffset.y = cos(yaw) * CurrentPoint.x + sin(yaw) * sin(pitch) * CurrentPoint.y;
			CurrentPosOffset.z = cos(pitch) * CurrentPoint.y;
			CurrentPosOffset.x = sin(yaw) * CurrentPoint.x + cos(yaw) * -sin(pitch) * CurrentPoint.y;

			SourceEngine::Vector CurrentPosition = CenterPos + CurrentPosOffset;
			SourceEngine::Vector OffsetProjection = (CurrentPosition - LocalEyePos).Normalized() * 18.0f;
			CurrentPosition += OffsetProjection;

			if (Config.g_bAutoWall)
			{
				int HitGroup = 0;
				float Damage = AutoWall::GetDamageAtPoint(CurrentPosition, &HitGroup);
				if (Damage > Config.g_iAutoWall_MinDamage || Damage + 1.0f > AutoWall::GetHitgroupDamage(HitGroup) * (wpn_data->iDamage * (float)(pow(wpn_data->flRangeModifier, (CurrentPosition - LocalEyePos).Length() * 0.002))))
				{
					Result.OptimalPoint[HitGroup] += CurrentPosition * Damage;
					Result.DamageWeights[HitGroup] += Damage;
					HitCount[HitGroup]++;
				}
			}
			else
			{
				SourceEngine::Ray_t ray;
				SourceEngine::CGameTrace tr;
				SourceEngine::CTraceFilter filter;
				filter.pSkip = C_CSPlayer::GetLocalPlayer();

				SourceEngine::Vector OffsetProjection = (CurrentPosition - LocalEyePos).Normalized() * 15.0f;

				ray.Init(LocalEyePos, CurrentPosition + OffsetProjection);
				SourceEngine::Interfaces::EngineTrace()->TraceRay(ray, 0x4600400B, &filter, &tr);

				if (tr.m_pEnt)
				{
					if (tr.m_pEnt == pTarget)
					{
						Result.OptimalPoint[tr.hitgroup] += CurrentPosition * (float)wpn_data->iDamage;
						Result.DamageWeights[tr.hitgroup] += (float)wpn_data->iDamage;
						HitCount[tr.hitgroup]++;
					}
				}
			}
		}
	}

	int TotalHits = 0;
	for (int i = 0; i < 8; i++)
		TotalHits += HitCount[i];

	Result.TotalHits = TotalHits;
	Result.Target = Target;

	if (TotalHits)
	{
		for (int i = 0; i < 8; i++)
			Result.HitFraction[i] = (float)HitCount[i] / (float)TotalHits;
	}

	for (int i = 0; i < 8; i++)
	{
		if (HitCount[i])
			Result.OptimalPoint[i] /= Result.DamageWeights[i];
	}

	Result.IsValid = true;

	return Result;
}

HitScanResult AutoShoot::ScanSkeleton(int Target)
{
	HitScanResult Result;
	for (int i = 0; i < 8; i++)
		Result.OptimalPoint[i] = SourceEngine::Vector(0, 0, 0);

	if (!Target) return HitScanResult();
	auto pTarget = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(Target));
	if (!pTarget) return HitScanResult();

	auto pLocal = C_CSPlayer::GetLocalPlayer();
	if (pLocal->GetActiveWeapon()->IsKnife()) return HitScanResult();

	SourceEngine::Vector LocalEyePos = pLocal->GetEyePos();
	SourceEngine::Vector LocalPos = pLocal->GetOrigin();
	SourceEngine::Vector HeadPos = pTarget->GetHeadCenterPos();
	SourceEngine::Vector EntityOrigin = pTarget->GetOrigin();

	if (GetHitChance(EntityOrigin) < Config.g_iMinHitChance) return HitScanResult();

	float LinStep = Config.g_iHitScan_Resolution / 16.0f;
	auto wpn_data = pLocal->GetActiveWeapon()->GetCSWpnData();

	int HitCount[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	auto Mdl = pTarget->GetModel();
	if (!Mdl) return HitScanResult();
	studiohdr_t* pStudioHdr = Interfaces::ModelInfo()->GetStudioModel(Mdl);
	if (!pStudioHdr) return HitScanResult();

	Vector vParent, vChild, sParent, sChild;

	for (int j = 0; j < pStudioHdr->numbones; j++)
	{
		mstudiobone_t* pBone = pStudioHdr->pBone(j);
		if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1))
		{
			Vector vChild = pTarget->GetBone(j);
			Vector vParent = pTarget->GetBone(pBone->parent);

			for (float l = 0; l <= 1; l += 1.0f / LinStep)
			{
				Vector CurrentPosition = vChild * l + vParent * (1.0f - l);

				if (Config.g_bAutoWall)
				{
					int HitGroup = 0;
					float Damage = AutoWall::GetDamageAtPoint(CurrentPosition, &HitGroup);
					if (Damage > Config.g_iAutoWall_MinDamage || Damage + 1.0f > AutoWall::GetHitgroupDamage(HitGroup) * (wpn_data->iDamage * (float)(pow(wpn_data->flRangeModifier, (CurrentPosition - LocalEyePos).Length() * 0.002))))
					{
						Result.OptimalPoint[HitGroup] += CurrentPosition * Damage, 1.5f;
						Result.DamageWeights[HitGroup] += Damage;
						HitCount[HitGroup]++;
					}
				}
				else
				{
					SourceEngine::Ray_t ray;
					SourceEngine::CGameTrace tr;
					SourceEngine::CTraceFilter filter;
					filter.pSkip = C_CSPlayer::GetLocalPlayer();

					SourceEngine::Vector OffsetProjection = (CurrentPosition - LocalEyePos).Normalized() * 15.0f;

					ray.Init(LocalEyePos, CurrentPosition + OffsetProjection);
					SourceEngine::Interfaces::EngineTrace()->TraceRay(ray, 0x4600400B, &filter, &tr);

					if (tr.m_pEnt)
					{
						if (tr.m_pEnt == pTarget)
						{
							float Damage = wpn_data->iDamage;
							Result.OptimalPoint[tr.hitgroup] += CurrentPosition * Damage;
							Result.DamageWeights[tr.hitgroup] += Damage;
							HitCount[tr.hitgroup]++;
						}
					}
				}
			}
		}
	}

	int TotalHits = 0;
	for (int i = 0; i < 8; i++)
		TotalHits += HitCount[i];

	Result.TotalHits = TotalHits;
	Result.Target = Target;

	if (TotalHits)
	{
		for (int i = 0; i < 8; i++)
			Result.HitFraction[i] = (float)HitCount[i] / (float)TotalHits;
	}

	for (int i = 0; i < 8; i++)
	{
		if (HitCount[i])
			Result.OptimalPoint[i] /= Result.DamageWeights[i];
	}

	Result.IsValid = true;

	return Result;
}

HitScanResult AutoShoot::FindOptimalTarget_HS()
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	HitScanResult BestResult;

	struct T_s
	{
		int Index;
		float Distance;
		bool operator<(const T_s &rhs) const { return Distance < rhs.Distance; }
	};

	std::vector<T_s> Targets;

	for (int i = 1; i < Interfaces::Engine()->GetMaxClients(); i++)
	{
		auto pEntity = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(i));
		if (Utils::IsViableTarget(pEntity))
		{
			float PointDist = (pEntity->GetEyePos() - pLocal->GetEyePos()).Length();
			T_s CurrentTarget;
			CurrentTarget.Index = i;
			CurrentTarget.Distance = PointDist;
			Targets.push_back(CurrentTarget);
		}
	}

	std::sort(Targets.begin(), Targets.end());

	for (auto i = Targets.begin(); i != Targets.end(); i++)
	{
		HitScanResult Result;
		if (Config.g_iAutoShootMode == 1)
			Result = ScanTarget((*i).Index);
		else if (Config.g_iAutoShootMode == 2)
			Result = ScanSkeleton((*i).Index);
		if (Result.TotalHits == 0) continue;

		for (int x = 1; x < 8; x++)
		{
			if (Result.HitFraction[x] * AutoWall::GetHitgroupDamage(x) * Config.g_iScanWeights[x] > 0)
				return Result;
		}
	}
	return BestResult;
}

int AutoShoot::FindOptimalTarget()
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	int CurrentTarget = 0;
	float LastDistance = 999999999999.0f;
	auto wpn_data = pLocal->GetActiveWeapon()->GetCSWpnData();

	for (int i = 1; i < 65; i++)
	{
		auto pEntity = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(i));
		if (Utils::IsViableTarget(pEntity))
		{
			SourceEngine::Vector LocalEyePos = pLocal->GetEyePos();
			SourceEngine::Vector EntityPos;
			if (Config.g_iAutoShootTarget == 0)
				EntityPos = pEntity->GetHeadCenterPos();
			else if (Config.g_iAutoShootTarget == 1)
				EntityPos = pEntity->GetBoneByName(XorStr("spine_3"));
			else if (Config.g_iAutoShootTarget == 2)
				EntityPos = pEntity->GetBoneByName(XorStr("spine_0"));

			if (GetHitChance(EntityPos) < Config.g_iMinHitChance) continue;

			if (Utils::DoesViewRayHitEntity(LocalEyePos, EntityPos))
			{
				float CurrentDistance = (EntityPos - LocalEyePos).Length();
				if (!CurrentTarget || CurrentDistance < LastDistance)
				{
					CurrentTarget = i;
					LastDistance = CurrentDistance;
				}
			}
		}
	}
	if (!CurrentTarget && Config.g_bAutoWall)
	{
		float HighestDamage = 0;

		for (int i = 1; i < 65; i++)
		{
			auto pEntity = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(i));
			if (Utils::IsViableTarget(pEntity))
			{
				SourceEngine::Vector LocalEyePos = pLocal->GetEyePos();
				SourceEngine::Vector EntityPos;
				if (Config.g_iAutoShootTarget == 0)
					EntityPos = pEntity->GetHeadCenterPos();
				else if (Config.g_iAutoShootTarget == 1)
					EntityPos = pEntity->GetBoneByName(XorStr("spine_3"));
				else if (Config.g_iAutoShootTarget == 2)
					EntityPos = pEntity->GetBoneByName(XorStr("spine_0"));

				if (GetHitChance(EntityPos) < Config.g_iMinHitChance) continue;

				float Damage = AutoWall::GetDamageAtPoint(EntityPos);
				if (Damage > HighestDamage && Damage > Config.g_iAutoWall_MinDamage || (Damage + 1.0f > (wpn_data->iDamage * (float)(pow(wpn_data->flRangeModifier, (EntityPos - LocalEyePos).Length() * 0.002))) && Damage > HighestDamage))
				{
					CurrentTarget = i;
					HighestDamage = Damage;
				}
			}
		}
	}
	return CurrentTarget;
}

void AutoShoot::LookAtPlayer(int PlayerID, SourceEngine::CUserCmd* pCmd)
{
	if (!PlayerID) return;
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	auto ActiveWeapon = pLocal->GetActiveWeapon();

	auto Target = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(PlayerID));
	SourceEngine::Vector AimPos = SourceEngine::Vector(0, 0, 0);
	if (Config.g_iAutoShootTarget == 0)
		AimPos = Target->GetHeadCenterPos();
	else if (Config.g_iAutoShootTarget == 1)
		AimPos = Target->GetBoneByName(XorStr("spine_3"));
	else if (Config.g_iAutoShootTarget == 2)
		AimPos = Target->GetBoneByName(XorStr("spine_0"));

	pCmd->viewangles = (AimPos - pLocal->GetEyePos()).Angle();
	pCmd->viewangles -= RCS::GetCurrentPunchAngles();

	if (!Config.g_iViewMode)
		Interfaces::Engine()->SetViewAngles(pCmd->viewangles);
}

void AutoShoot::LookAtPoint(SourceEngine::Vector Point, SourceEngine::CUserCmd* pCmd)
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	auto ActiveWeapon = pLocal->GetActiveWeapon();

	SourceEngine::QAngle Angle = (Point - pLocal->GetEyePos()).Angle();
	Utils::Clamp(Angle);

	pCmd->viewangles = Angle;
	pCmd->viewangles -= RCS::GetCurrentPunchAngles();
}

void AutoShoot::FireAtResult(HitScanResult& Result, SourceEngine::CUserCmd* pCmd)
{
	if (!Result.IsValid) return;
	if (!Result.TotalHits) return;
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	auto ActiveWeapon = pLocal->GetActiveWeapon();
	if (ActiveWeapon->IsSniper()) { if (!pLocal->IsScoped()) return; }
	float NextPrimaryAttack = ActiveWeapon->NextPrimaryAttack();

	// Get largest area

	SourceEngine::Vector BestPoint = SourceEngine::Vector(0, 0, 0);
	float GreatestHitFraction = 0;
	for (int i = 1; i < 8; i++)
	{
		float AdjustedHitFraction = Result.HitFraction[i] * AutoWall::GetHitgroupDamage(i) * Config.g_iScanWeights[i];
		if (AdjustedHitFraction > GreatestHitFraction)
		{
			BestPoint = Result.OptimalPoint[i];
			GreatestHitFraction = AdjustedHitFraction;
		}
	}

	if (ActiveWeapon->IsKnife())
	{
		if (BestPoint.DistTo(pLocal->GetOrigin()) > 85.6)
		{
			AutoShoot::IsFiring = false;
			return;
		}

		// Fix the knife

//		Triggerbot::Knife(pCmd, Target);
		if (pCmd->buttons & IN_ATTACK || pCmd->buttons & IN_ATTACK2)
		{
			AutoShoot::IsFiring = true;
			LookAtPoint(BestPoint, pCmd);
		}
		return;
	}

	if (ActiveWeapon->IsSemiAutomatic() && !ActiveWeapon->CanFire() && (pCmd->buttons & IN_ATTACK))
	{
		pCmd->buttons &= ~IN_ATTACK;
		return;
	}

	LookAtPoint(BestPoint, pCmd);

	if (!(pCmd->buttons & IN_ATTACK))
	{
		AutoShoot::IsFiring = true;
		pCmd->buttons |= IN_ATTACK;
	}

	// Let go on weapon switch
	if (LastWeaponIndex != ActiveWeapon->GetItemDefinitionIndex() && (pCmd->buttons & IN_ATTACK))
		pCmd->buttons &= ~IN_ATTACK;
	LastWeaponIndex = ActiveWeapon->GetItemDefinitionIndex();
}

void AutoShoot::FireAtTarget(int Target, SourceEngine::CUserCmd* pCmd)
{
	if (!Target) return;
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	auto ActiveWeapon = pLocal->GetActiveWeapon();
	static ConVar* weapon_accuracy_nospread = Interfaces::CVar()->FindVar(XorStr("weapon_accuracy_nospread"));
	if (ActiveWeapon->IsSniper()) {if (!pLocal->IsScoped() && !(weapon_accuracy_nospread->GetInt() == 1)) return;}
	float NextPrimaryAttack = ActiveWeapon->NextPrimaryAttack();

	if (ActiveWeapon->IsKnife())
	{
		// return, fix this later.
		return;

		auto pEntity = static_cast<C_CSPlayer*>(SourceEngine::Interfaces::EntityList()->GetClientEntity(Target));
		if (!pEntity)
		{
			AutoShoot::IsFiring = false;
			return;
		}
		if (pEntity->GetOrigin().DistTo(pLocal->GetOrigin()) > 85.6)
		{
			AutoShoot::IsFiring = false;
			return;
		}

		Triggerbot::Knife(pCmd, Target);
		if (pCmd->buttons & IN_ATTACK || pCmd->buttons & IN_ATTACK2)
		{
			AutoShoot::IsFiring = true;

			// Foce a bodyshot
			int OldAimLockTarget = Config.g_iAutoShootTarget;
			Config.g_iAutoShootTarget = 1;
			LookAtPlayer(Target, pCmd);
			Config.g_iAutoShootTarget = OldAimLockTarget;
		}
		return;
	}

	if (ActiveWeapon->IsSemiAutomatic() && !ActiveWeapon->CanFire() && (pCmd->buttons & IN_ATTACK))
	{
		pCmd->buttons &= ~IN_ATTACK;
		return;
	}

	LookAtPlayer(Target, pCmd);

	if (!(pCmd->buttons & IN_ATTACK))
	{
		AutoShoot::IsFiring = true;
		pCmd->buttons |= IN_ATTACK;
	}
	
	// Let go on weapon switch
	if (LastWeaponIndex != ActiveWeapon->GetItemDefinitionIndex() && (pCmd->buttons & IN_ATTACK))
		pCmd->buttons &= ~IN_ATTACK;
	LastWeaponIndex = ActiveWeapon->GetItemDefinitionIndex();
	Resolver::AddShot(Target);
}