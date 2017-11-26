#pragma once

#include "CSGOStructs.hpp"
#include "DrawTools.h"

class EntityESP
{
public:
	EntityESP(C_CSPlayer* pEntity) : m_pEntity(pEntity) {}
	~EntityESP() {}

    void RenderESP();
	void RenderSkeleton();
    void RenderName();
	void RenderWeaponName();

private:
    C_CSPlayer* m_pEntity;
};

