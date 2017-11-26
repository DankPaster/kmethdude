#pragma once
#include "SourceEngine\SDK.hpp"
#include "CSGOStructs.hpp"
#include "Utils.hpp"

namespace Triggerbot
{
	void Shoot(SourceEngine::CUserCmd* pCmd);
	void Knife(SourceEngine::CUserCmd* pCmd, int ID = 0);
	void Think(SourceEngine::CUserCmd* pCmd);
};

