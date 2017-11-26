#pragma once

#include "studiohdr.h"
#include "SourceEngine\IClientRenderable.hpp"

namespace SourceEngine
{
	class IMatRenderContext;

	struct DrawModelState_t {
		studiohdr_t*			m_pStudioHdr;
		void*					m_pStudioHWData;
		IClientRenderable*		m_pRenderable;
		const matrix3x4_t			*m_pModelToWorld;
		unsigned short			m_decals;
		int						m_drawFlags;
		int						m_lod;
	};

	struct ModelRenderInfo_t
	{
		Vector origin;
		Vector angles;
		IClientRenderable *pRenderable;
		const model_t *pModel;
		const matrix3x4_t *pModelToWorld;
		const matrix3x4_t *pLightingOffset;
		const Vector *pLightingOrigin;
		int flags;
		int entity_index;
		int skin;
		int body;
		int hitboxset;
		unsigned short instance;

		ModelRenderInfo_t()
		{
			pModelToWorld = NULL;
			pLightingOffset = NULL;
			pLightingOrigin = NULL;
		}
	};

	enum OverrideType_t
	{
		OVERRIDE_NORMAL = 0,
		OVERRIDE_BUILD_SHADOWS,
		OVERRIDE_DEPTH_WRITE,
	};

	class IVModelRender {
	public:
		void ForcedMaterialOverride(IMaterial* m, OverrideType_t o, int i)
		{
			typedef void(__thiscall* tForcedMaterialOverride)(PVOID, IMaterial*, OverrideType_t, int);
			CallVFunction<tForcedMaterialOverride>(this, 1)(this, m, o, i);
		}
	};
}