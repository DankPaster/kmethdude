#pragma once

#include "SourceEngine\SDK.hpp"
#include "CSGOStructs.hpp"
#include "Options.hpp"
#include "Utils.hpp"

namespace AutoWall
{
	float GetDamageAtPoint(Vector dst, int* Hitgroup = nullptr);
	float GetHitgroupDamage(int iHitGroup);
	float ScaleDamage(int Hitgroup, C_CSPlayer *pEntity, float WeaponArmorRatio, float Damage);
	bool HandleBulletPenetration(float WeaponPenetration, surfacedata_t* EnterSurfaceData, surfacedata_t* ExitSurfaceData, trace_t& Trace, float Thickness, int &HitsLeft, float& CurrentDamage);
	float SimulateFireBullet(Vector StartPos, Vector Point, int* HitGroup);
};

