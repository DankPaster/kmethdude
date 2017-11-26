#pragma once

#include "SourceEngine/Definitions.hpp"

namespace SourceEngine
{
	class IMaterialSystem {
	public:
		IMaterial* CreateMaterial(const char* c, KeyValues* k)
		{
			typedef IMaterial*(__thiscall* tCreateMaterial)(PVOID, const char*, KeyValues*);
			return CallVFunction<tCreateMaterial>(this, 83)(this, c, k);
		}
		IMaterial* FindMaterial(const char* c1, const char* c2)
		{
			typedef IMaterial*(__thiscall* tFindMaterial)(PVOID, const char*, const char*);
			return CallVFunction<tFindMaterial>(this, 84)(this, c1, c2);
		}
	};
}