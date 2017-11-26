#pragma once

#include "SourceEngine\SDK.hpp"
#include "AutoShoot.h"
#include "Options.hpp"
#include <limits>

enum AAPitch : int
{
	Pitch_None = 0,
	Down = 1,
	Emotion = 2,
	FakeDown = 3,
	FakeAngel = 4,
	Rotary = 5,
	RandP = 6,
};

enum AAYaw : int
{
	Yaw_None = 0,
	Backward = 1,
	FakeBackward = 2,
	Jitter = 3,
	BackJitter = 4,
	JitterSynced = 5,
	Sideways = 6,
	FakeSideways = 7,
	FakeForward = 8,
	SlowSpin = 9,
	FastSpin = 10,
	Rand = 11,
	FakeSideSpin = 12,
};

enum AAYawOverride : int
{
	None_Override = 0,
	Backward_Override,
	BackJitter_Override,
	JitterSynced_Override,
	Sideways_Override,
	SlowSpin_Override,
	FastSpin_Override,
	Rand_Override,
};

namespace AntiAim
{
	extern SourceEngine::QAngle LastAntiAimAngles;
	void WarpAngles(SourceEngine::CUserCmd* pCmd, byte* bSendPacket);
	void MovementFix(SourceEngine::CUserCmd* pCmd);
	void PitchOverride(SourceEngine::CUserCmd* pCmd);
	void RealYawOverride(SourceEngine::CUserCmd* pCmd);
	void FakeYawOverride(SourceEngine::CUserCmd* pCmd);
};