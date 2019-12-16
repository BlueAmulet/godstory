//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _SGSHADOWTEXTURECACHE_H_
#define _SGSHADOWTEXTURECACHE_H_

#include "gfx/gfxDevice.h"
#include "lightingSystem/synapseGaming/sgHashMap.h"


struct sgShadowTextureEntryInfo
{
	GFXFormat sgFormat;
	Point2I sgSize;
	U32 sgCreateCount;
};

typedef hash_multimap<GFXTexHandle, sgShadowTextureEntryInfo> sgShadowTextureMultimap;

class sgShadowTextureCache
{
private:
	static sgShadowTextureMultimap sgShadowTextures;
	static sgShadowTextureMultimap *sgGetFirstEntry();

public:
	static void sgAcquire(GFXTexHandle &texture, Point2I size, U32 format);
	static void sgRelease(GFXTexHandle &texture);
	static void sgClear();
	static void sgCleanupUnused();
	static void sgPrintStats();
};


#endif//_SGSHADOWTEXTURECACHE_H_
