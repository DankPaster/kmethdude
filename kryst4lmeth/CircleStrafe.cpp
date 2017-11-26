#include "CircleStrafe.h"

using namespace SourceEngine;

namespace CircleStrafe
{
	float RightMovement;
	bool IsActive;
	float StrafeAngle;
}

void CircleStrafe::Start(CUserCmd* pCmd)
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	StrafeAngle = 0;
	IsActive = true;

	QAngle CurrentAngles;
	Interfaces::Engine()->GetViewAngles(CurrentAngles);
	CurrentAngles.x = 0;
	Vector Forward = CurrentAngles.Direction();
	Vector Right = Forward.Cross(Vector(0, 0, 1));
	Vector Left = Vector(Right.x * -1.0f, Right.y * -1.0f, Right.z);

	float LeftPath = Utils::GetTraceFractionWorldProps(pLocal->GetOrigin() + Vector(0, 0, 10), pLocal->GetOrigin() + Left * 250.0f + Vector(0, 0, 10));
	float RightPath = Utils::GetTraceFractionWorldProps(pLocal->GetOrigin() + Vector(0, 0, 10), pLocal->GetOrigin() + Right * 250.0f + Vector(0, 0, 10));

	if (LeftPath > RightPath)
		RightMovement = -1;
	else
		RightMovement = 1;
}

void CircleStrafe::Strafe(CUserCmd* pCmd)
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	Vector Velocity = pLocal->GetVelocity();
	Velocity.z = 0;
	float Speed = Velocity.Length();
	if (Speed < 45) Speed = 45;

	float FinalPath = Utils::GetTraceFractionWorldProps(pLocal->GetOrigin() + Vector(0, 0, 10), pLocal->GetOrigin() + Vector(0, 0, 10) + Velocity / 3.0f);
	float DeltaAngle = RightMovement * fmax((275.0f / Speed) * (2.0f / FinalPath) * (128.0f / (1.7f / Interfaces::GlobalVars()->interval_per_tick)) * 4.20f, 2.0f);
	StrafeAngle += DeltaAngle;

	if (!(pLocal->GetFlags() & (int)EntityFlags::FL_ONGROUND))
		pCmd->buttons &= ~IN_JUMP;
	else
		pCmd->buttons |= IN_JUMP;

	if (fabs(StrafeAngle) >= 360.0f)
	{
		StrafeAngle = 0.0f;
		IsActive = false;
		RightMovement = 0;
	}
	else
	{
		pCmd->forwardmove = cos((StrafeAngle + 90 * RightMovement) * (M_PI / 180.0f)) * pLocal->GetMaxSpeed();
		pCmd->sidemove = sin((StrafeAngle + 90 * RightMovement) * (M_PI / 180.0f)) * pLocal->GetMaxSpeed();
	}
}