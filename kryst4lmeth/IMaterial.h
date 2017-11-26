#pragma once

#pragma once

#include "SourceEngine\Definitions.hpp"
#include "SourceEngine\IVEngineClient.hpp"
#include <stdint.h>
#include <Windows.h>
#include "XorStr.hpp"
#include <Psapi.h>

#define TEXTURE_GROUP_LIGHTMAP						XorStr("Lightmaps")
#define TEXTURE_GROUP_WORLD							XorStr("World textures")
#define TEXTURE_GROUP_MODEL							XorStr("Model textures")
#define TEXTURE_GROUP_VGUI							XorStr("VGUI textures")
#define TEXTURE_GROUP_PARTICLE						XorStr("Particle textures")
#define TEXTURE_GROUP_DECAL							XorStr("Decal textures")
#define TEXTURE_GROUP_SKYBOX						XorStr("SkyBox textures")
#define TEXTURE_GROUP_CLIENT_EFFECTS				XorStr("ClientEffect textures")
#define TEXTURE_GROUP_OTHER							XorStr("Other textures")
#define TEXTURE_GROUP_PRECACHED						XorStr("Precached")				// TODO: assign texture groups to the precached materials
#define TEXTURE_GROUP_CUBE_MAP						XorStr("CubeMap textures")
#define TEXTURE_GROUP_RENDER_TARGET					XorStr("RenderTargets")
#define TEXTURE_GROUP_UNACCOUNTED					XorStr("Unaccounted textures")	// Textures that weren't assigned a texture group.
//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER		"Static Vertex"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			XorStr("Static Indices")
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		XorStr("Displacement Verts")
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	XorStr("Lighting Verts")
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	XorStr("World Verts")
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	XorStr("Model Verts")
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	XorStr("Other Verts")
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			XorStr("Dynamic Indices")
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			XorStr("Dynamic Verts")
#define TEXTURE_GROUP_DEPTH_BUFFER					XorStr("DepthBuffer")
#define TEXTURE_GROUP_VIEW_MODEL					XorStr("ViewModel")
#define TEXTURE_GROUP_PIXEL_SHADERS					XorStr("Pixel Shaders")
#define TEXTURE_GROUP_VERTEX_SHADERS				XorStr("Vertex Shaders")
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			XorStr("RenderTarget Surfaces")
#define TEXTURE_GROUP_MORPH_TARGETS					XorStr("Morph Targets")

enum MaterialVarFlags_t
{
	MATERIAL_VAR_DEBUG = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),

	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
	MATERIAL_VAR_SELFILLUM = (1 << 6),
	MATERIAL_VAR_ADDITIVE = (1 << 7),
	MATERIAL_VAR_ALPHATEST = (1 << 8),
	MATERIAL_VAR_MULTIPASS = (1 << 9),
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17),
	MATERIAL_VAR_NOALPHAMOD = (1 << 18),
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19),
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23),
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_ENVMAPMODE = (1 << 25),
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
	MATERIAL_VAR_IGNORE_ALPHA_MODULATION = (1 << 30),
	MATERIAL_VAR_VERTEXFOG = (1 << 31),

	// NOTE: Only add flags here that either should be read from
	// .vmts or can be set directly from client code. Other, internal
	// flags should to into the flag enum in imaterialinternal.h
};

enum PreviewImageRetVal_t
{
	MATERIAL_PREVIEW_IMAGE_BAD = 0,
	MATERIAL_PREVIEW_IMAGE_OK,
	MATERIAL_NO_PREVIEW_IMAGE,
};

enum MaterialPropertyTypes_t
{
	MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0,					// bool
	MATERIAL_PROPERTY_OPACITY,								// int (enum MaterialPropertyOpacityTypes_t)
	MATERIAL_PROPERTY_REFLECTIVITY,							// vec3_t
	MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS				// bool
};

typedef uint64_t VertexFormat_t;
typedef unsigned int MorphFormat_t;

using namespace SourceEngine;

class ImageFormat;
class IMaterialVar;

// Temporary kappa paste
inline uint64_t FindSignature(const char* szModule, const char* szSignature)
{
	//CREDITS: learn_more
#define INRANGE(x,a,b)  (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

	MODULEINFO modInfo;
	GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(szModule), &modInfo, sizeof(MODULEINFO));
	DWORD startAddress = (DWORD)modInfo.lpBaseOfDll;
	DWORD endAddress = startAddress + modInfo.SizeOfImage;
	const char* pat = szSignature;
	DWORD firstMatch = 0;
	for (DWORD pCur = startAddress; pCur < endAddress; pCur++) {
		if (!*pat) return firstMatch;
		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
			if (!firstMatch) firstMatch = pCur;
			if (!pat[2]) return firstMatch;
			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
			else pat += 2;    //one ?
		}
		else {
			pat = szSignature;
			firstMatch = 0;
		}
	}
	return NULL;
}

namespace SourceEngine
{
	class KeyValues {
	public:
		KeyValues() {} //these arent referenced anywhere.
		
		KeyValues::KeyValues(const char* name) {
			static DWORD dwAddress = FindSignature(XorStr("client.dll"), XorStr("55 8B EC 51 33 C0 C7 45 ? ? ? ? ? 56 8B F1 81 26 ? ? ? ? C6 46 03 ? 89 46 10 89 46 18 89 46 14 89 46 1C 89 46 04 89 46 08 89 46 0C FF 15 ? ? ? ? 6A 01 FF 75 08 8D 4D FC 8B 10 51 8B C8 FF 52 24 8B 0E 33 4D FC 81 E1 ? ? ? ? 31 0E 88 46 03"));
			typedef void(__thiscall* InitKeyValues)(KeyValues*, const char*);
			((InitKeyValues)(dwAddress))(this, name);
		}
		
		/*
		bool KeyValues::LoadFromBuffer(char const *resourceName, const char *pBuffer) {
			static DWORD dwAddress = FindSignature(XorStr("client.dll"), XorStr("55 8B EC 83 EC 48 53 56 57 8B F9 89 7D F4"));
			typedef bool(__thiscall* LoadFromBuffer)(void*, const char*, const char*, void*, const char*, void*);
			return ((LoadFromBuffer)(dwAddress))(this, resourceName, pBuffer, 0, 0, 0);
		}
		*/
		byte pad[64U];
	};

	class IMaterial
	{
	public:
		// Get the name of the material.  This is a full path to 
		// the vmt file starting from "hl2/materials" (or equivalent) without
		// a file extension.
		virtual const char *	GetName() const = 0;
		virtual const char *	GetTextureGroupName() const = 0;

		// Get the preferred size/bitDepth of a preview image of a material.
		// This is the sort of image that you would use for a thumbnail view
		// of a material, or in WorldCraft until it uses materials to render.
		// separate this for the tools maybe
		virtual PreviewImageRetVal_t GetPreviewImageProperties(int *width, int *height,
			ImageFormat *imageFormat, bool* isTranslucent) const = 0;

		// Get a preview image at the specified width/height and bitDepth.
		// Will do resampling if necessary.(not yet!!! :) )
		// Will do color format conversion. (works now.)
		virtual PreviewImageRetVal_t GetPreviewImage(unsigned char *data,
			int width, int height,
			ImageFormat imageFormat) const = 0;
		// 
		virtual int				GetMappingWidth() = 0;
		virtual int				GetMappingHeight() = 0;

		virtual int				GetNumAnimationFrames() = 0;

		// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
		virtual bool			InMaterialPage(void) = 0;
		virtual	void			GetMaterialOffset(float *pOffset) = 0;
		virtual void			GetMaterialScale(float *pScale) = 0;
		virtual IMaterial		*GetMaterialPage(void) = 0;

		// find a vmt variable.
		// This is how game code affects how a material is rendered.
		// The game code must know about the params that are used by
		// the shader for the material that it is trying to affect.
		virtual IMaterialVar*	FindVar(const char *varName, bool *found, bool complain = true) = 0;

		// The user never allocates or deallocates materials.  Reference counting is
		// used instead.  Garbage collection is done upon a call to 
		// IMaterialSystem::UncacheUnusedMaterials.
		virtual void			IncrementReferenceCount(void) = 0;
		virtual void			DecrementReferenceCount(void) = 0;

		inline void AddRef() { IncrementReferenceCount(); }
		inline void Release() { DecrementReferenceCount(); }

		// Each material is assigned a number that groups it with like materials
		// for sorting in the application.
		virtual int 			GetEnumerationID(void) const = 0;

		virtual void			GetLowResColorSample(float s, float t, float *color) const = 0;

		// This computes the state snapshots for this material
		virtual void			RecomputeStateSnapshots() = 0;

		// Are we translucent?
		virtual bool			IsTranslucent() = 0;

		// Are we alphatested?
		virtual bool			IsAlphaTested() = 0;

		// Are we vertex lit?
		virtual bool			IsVertexLit() = 0;

		// Gets the vertex format
		virtual VertexFormat_t	GetVertexFormat() const = 0;

		// returns true if this material uses a material proxy
		virtual bool			HasProxy(void) const = 0;

		virtual bool			UsesEnvCubemap(void) = 0;

		virtual bool			NeedsTangentSpace(void) = 0;

		virtual bool			NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
		virtual bool			NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;

		// returns true if the shader doesn't do skinning itself and requires
		// the data that is sent to it to be preskinned.
		virtual bool			NeedsSoftwareSkinning(void) = 0;

		// Apply constant color or alpha modulation
		virtual void			AlphaModulate(float alpha) = 0;
		virtual void			ColorModulate(float r, float g, float b) = 0;

		// Material Var flags...
		virtual void			SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
		virtual bool			GetMaterialVarFlag(MaterialVarFlags_t flag) const = 0;

		// Gets material reflectivity
		virtual void			GetReflectivity(Vector& reflect) = 0;

		// Gets material property flags
		virtual bool			GetPropertyFlag(MaterialPropertyTypes_t type) = 0;

		// Is the material visible from both sides?
		virtual bool			IsTwoSided() = 0;

		// Sets the shader associated with the material
		virtual void			SetShader(const char *pShaderName) = 0;

		// Can't be const because the material might have to precache itself.
		virtual int				GetNumPasses(void) = 0;

		// Can't be const because the material might have to precache itself.
		virtual int				GetTextureMemoryBytes(void) = 0;

		// Meant to be used with materials created using CreateMaterial
		// It updates the materials to reflect the current values stored in the material vars
		virtual void			Refresh() = 0;

		// GR - returns true is material uses lightmap alpha for blending
		virtual bool			NeedsLightmapBlendAlpha(void) = 0;

		// returns true if the shader doesn't do lighting itself and requires
		// the data that is sent to it to be prelighted
		virtual bool			NeedsSoftwareLighting(void) = 0;

		// Gets at the shader parameters
		virtual int				ShaderParamCount() const = 0;
		virtual IMaterialVar	**GetShaderParams(void) = 0;

		// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
		// the material can't be found.
		virtual bool			IsErrorMaterial() const = 0;

		virtual void			SetUseFixedFunctionBakedLighting(bool bEnable) = 0;

		// Gets the current alpha modulation
		virtual float			GetAlphaModulation() = 0;
		virtual void			GetColorModulation(float *r, float *g, float *b) = 0;

		// Gets the morph format
		virtual MorphFormat_t	GetMorphFormat() const = 0;

		// fast find that stores the index of the found var in the string table in local cache
		virtual IMaterialVar*	FindVarFast(char const *pVarName, unsigned int *pToken) = 0;

		// Sets new VMT shader parameters for the material
		virtual void			SetShaderAndParams(KeyValues *pKeyValues) = 0;
		virtual const char *	GetShaderName() const = 0;

		virtual void			DeleteIfUnreferenced() = 0;

		virtual bool			IsSpriteCard() = 0;

		virtual void			CallBindProxy(void *proxyData) = 0;

		virtual IMaterial		*CheckProxyReplacement(void *proxyData) = 0;

		virtual void			RefreshPreservingMaterialVars() = 0;

		virtual bool			WasReloadedFromWhitelist() = 0;

		virtual bool			IsPrecached() const = 0;
	};
}