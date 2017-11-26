#pragma once

#include "StudioHDR.h"

class IVModelInfo {
public:
	studiohdr_t* GetStudioModel(const model_t* m)
	{
		typedef studiohdr_t*(__thiscall* tGetStudioModel)(PVOID, const model_t*);
		return CallVFunction<tGetStudioModel>(this, 30)(this, m);
	}
	const char* GetModelName(const model_t* m)
	{
		typedef const char*(__thiscall* tGetModelName)(PVOID, const model_t*);
		return CallVFunction<tGetModelName>(this, 3)(this, m);
	}
};