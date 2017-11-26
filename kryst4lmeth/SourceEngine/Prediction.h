#pragma once

#include "Definitions.hpp"
#include "GameMovement.h"

class SourceEngine::CUserCmd;

namespace SourceEngine
{
	class IPrediction {
	public:
		virtual void UnknownVirtual0() = 0;
		virtual void UnknownVirtual1() = 0;
		virtual void UnknownVirtual2() = 0;
		virtual void UnknownVirtual3() = 0;
		virtual void UnknownVirtual4() = 0;
		virtual void UnknownVirtual5() = 0;
		virtual void UnknownVirtual6() = 0;
		virtual void UnknownVirtual7() = 0;
		virtual void UnknownVirtual8() = 0;
		virtual void UnknownVirtual9() = 0;
		virtual void UnknownVirtual10() = 0;
		virtual void UnknownVirtual11() = 0;
		virtual void UnknownVirtual12() = 0;
		virtual void UnknownVirtual13() = 0;
		virtual void UnknownVirtual14() = 0;
		virtual void UnknownVirtual15() = 0;
		virtual void UnknownVirtual16() = 0;
		virtual void UnknownVirtual17() = 0;
		virtual void UnknownVirtual18() = 0;
		virtual void UnknownVirtual19() = 0;
		virtual void SetupMove(SourceEngine::IClientEntity* player, SourceEngine::CUserCmd* ucmd, void* pHelper, MoveData* move) = 0; //20
		virtual void FinishMove(SourceEngine::IClientEntity* player, SourceEngine::CUserCmd* ucmd, MoveData* move) = 0;
	};
};