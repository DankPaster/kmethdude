#pragma once

#include "CSGOStructs.hpp"
#include "Options.hpp"
#include "AutoShoot.h"
#include "Utils.hpp"
#include "PushNotifications.h"
#include <cmath>
#include <algorithm>
#include <map>

struct ResolveData
{
	ResolveData()
	{
		LastAnglesHit.Invalidate();
	}
	int ID;
	bool IsStatic = false;
	bool IsSpinning = false;
	int ShotsFiredAtTarget = 0;
	QAngle LastAnglesHit;
	QAngle AdjustedAngles = QAngle(0, 0, 0);
	QAngle LastAttemptedAngles = QAngle(0, 0, 0);
	QAngle LastOriginalAngles = QAngle(0, 0, 0);
	QAngle LastSetAngles = QAngle(0, 0, 0);
	int MissedShots = 0;
	int KilledByCount = 0;
	int LastTickHit = 0;
	float AdjustedPitch = 0;
	float AdjustedYaw = 0;
};

namespace Resolver
{
	extern std::map<int, ResolveData> PlayerData;
	void LowerBodyYawFix(int CurrentTarget);
	void Resolve(int CurrentTarget);
	void DamageCallback(IGameEvent* pEvent);
	void AddShot(int EntityIndex);
};

