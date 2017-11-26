#pragma once

#include "CSGOStructs.hpp"
#include "AntiAim.h"
#include "Utils.hpp"

namespace CircleStrafe
{
	extern float RightMovement;
	extern bool IsActive;
	extern float StrafeAngle;
	void Start(SourceEngine::CUserCmd* pCmd);
	void Strafe(SourceEngine::CUserCmd* pCmd);
};

