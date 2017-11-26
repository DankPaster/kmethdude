/////////////////////////////////////////
// zMenu 1.2, kryst4lmeth: Open Source //
//////// Made by toast/itoasti //////////
/////////////////////////////////////////

#pragma once
#include <vector>
#include "SourceEngine\SDK.hpp"
#include "DrawTools.h"

namespace Menu
{
	extern bool ShouldDraw;
	class CBaseRenderable;
	class CWindow;
	class CTab;
	void InitializeMenu();
	void Render();
	void DestroyObjects();
};