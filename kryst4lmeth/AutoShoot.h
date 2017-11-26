#pragma once

#include "SourceEngine/SDK.hpp"
#include "Options.hpp"
#include "XorStr.hpp"
#include "AutoWall.h"
#include "Triggerbot.h"

#define HITGROUP_GENERIC    0
// Use groups 1 - 7
#define HITGROUP_HEAD        1
#define	HITGROUP_CHEST        2
#define HITGROUP_STOMACH    3
#define HITGROUP_LEFTARM    4    
#define HITGROUP_RIGHTARM    5
#define HITGROUP_LEFTLEG    6
#define HITGROUP_RIGHTLEG    7
// ----------------
#define HITGROUP_GEAR        10

struct HitScanResult
{
	bool IsValid = false;
	int Target = 0;
	int TotalHits = 0;
	float HitFraction[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	float DamageWeights[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	SourceEngine::Vector OptimalPoint[8];
};

namespace AutoShoot
{
	float GetHitChance(Vector Point);
	HitScanResult ScanTarget(int Target);
	HitScanResult ScanSkeleton(int Target);
	int FindOptimalTarget();
	HitScanResult FindOptimalTarget_HS();
	void LookAtPlayer(int PlayerID, CUserCmd* pCmd);
	void LookAtPoint(SourceEngine::Vector Point, SourceEngine::CUserCmd* pCmd);
	void Fire(CUserCmd* pCmd, byte* bSendPacket);
	void FireAtTarget(int Target, SourceEngine::CUserCmd* pCmd);
	void FireAtResult(HitScanResult& Result, SourceEngine::CUserCmd* pCmd);
	extern bool IsFiring;
	extern int LastTarget;
};

