#pragma once

class IVRenderView {
public:
	void SetColorModulation(float const* float3)
	{
		typedef void(__thiscall* tSetColorModulation)(PVOID, float const*);
		CallVFunction<tSetColorModulation>(this, 6)(this, float3);
	}
	void SetBlend(float alpha)
	{
		typedef void(__thiscall* tSetBlend)(PVOID, float);
		CallVFunction<tSetBlend>(this, 4)(this, alpha);
	}
};