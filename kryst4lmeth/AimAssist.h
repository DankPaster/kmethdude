#pragma once

#include "SourceEngine/SDK.hpp"
#include "Options.hpp"
#include "NoRecoil.h"
#include "Utils.hpp"

namespace AimLock
{
	void LookAtPlayer(int PlayerID, SourceEngine::CUserCmd* pCmd);
	void LockOnTarget(int PlayerID, SourceEngine::CUserCmd* pCmd);
	void LookAtPoint(SourceEngine::Vector Point, SourceEngine::CUserCmd* pCmd);
	void Think(SourceEngine::CUserCmd* pCmd);
	extern bool WasOnEnemy;
};

namespace Aimbot
{
	extern int CurrentTarget;
	extern int CurrentSpeed;
	void Step(SourceEngine::CUserCmd* pCmd);
	void Think(SourceEngine::CUserCmd* pCmd);
	int GetOptimalTarget();
};