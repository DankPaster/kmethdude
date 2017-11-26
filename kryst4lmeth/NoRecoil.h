#pragma once

#include "CSGOStructs.hpp"
#include "Options.hpp"

namespace RCS
{
	void UpdatePunchAngles();
	SourceEngine::Vector GetLegitPunchAngles();
	SourceEngine::Vector GetDeltaLegitPunchAngles();
	SourceEngine::Vector GetCurrentPunchAngles();
}