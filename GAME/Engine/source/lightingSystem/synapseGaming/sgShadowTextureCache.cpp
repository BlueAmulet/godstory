//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "gfx/gfxDevice.h"
#include "gfx/primBuilder.h"
#include "math/mathUtils.h"
#include "sceneGraph/sceneGraph.h"

#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/synapseGaming/sgLighting.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"
#include "lightingSystem/synapseGaming/sgShadowTextureCache.h"
#include "lightingSystem/synapseGaming/sgFormatManager.h"

#define SG_UNUSED_TIMEOUT	2000
// this can be a long time, only used for cleaning
// up texture memory after an intensive scene...
#define SG_TEXTURE_TIMEOUT	30000

extern SceneGraph* gClientSceneGraph;


sgShadowTextureMultimap sgShadowTextureCache::sgShadowTextures;


bool sgTimeElapsed(U32 currenttime, U32 &lasttime, U32 period)
{
   if(currenttime < lasttime)
   {
      lasttime = 0;
      return false;
   }

   if((currenttime - lasttime) < period)
      return false;

   lasttime = currenttime;
   return true;
}

void sgShadowTextureCache::sgAcquire(GFXTexHandle &texture, Point2I size, U32 format)
{
	U32 hash = calculateCRC(&size.x, (sizeof(size.x) * 2));
	hash = calculateCRC(&format, sizeof(format), hash);

	sgShadowTextureMultimap *entry = sgShadowTextures.find(hash);
	AssertFatal((entry), "No Entry?");

	if(entry->object.size() > 0)
	{
		// copy...
		texture = entry->object.last();
		texture->cacheId = hash;
		// force dereference...
		entry->object.last() = NULL;
		// remove...
		entry->object.decrement(1);
		return;
	}

	texture = GFXTexHandle(size.x, size.y, GFXFormat(format), &ShadowTargetTextureProfile);
	texture->cacheId = hash;

	entry->info.sgFormat = GFXFormat(format);
	entry->info.sgSize = size;
	entry->info.sgCreateCount++;
}

void sgShadowTextureCache::sgRelease(GFXTexHandle &texture)
{
	if(!(GFXTextureObject *)texture)
		return;

	AssertFatal(((GFXTextureObject *)texture), "Bad texture reference!");

	if(texture->cacheId == 0)
	{
		texture = NULL;
		return;
	}

	//AssertFatal((texture->mRefCount == 1), "Non unique texture reference!");

	texture->cacheTime = Platform::getRealMilliseconds();

	sgShadowTextureMultimap *entry = sgShadowTextures.find(texture->cacheId);
	// add to the cache...
	entry->object.increment(1);
	// copy...
	entry->object.last() = texture;
	// remove the external reference...
	texture = NULL;
}

sgShadowTextureMultimap *sgShadowTextureCache::sgGetFirstEntry()
{
	return sgShadowTextures.find(0);
}

void sgShadowTextureCache::sgClear()
{
	sgShadowTextureMultimap *entry = sgGetFirstEntry();
	while(entry)
	{
		for(U32 i=0; i<entry->object.size(); i++)
		{
			// force delete...
			entry->object[i] = NULL;
		}

		entry->object.clear();
		entry->info.sgCreateCount = 0;
		entry = entry->linkHigh;
	}
}

void sgShadowTextureCache::sgCleanupUnused()
{
	// get rid of old stuff...
	// might need to pull textures
	// from the back of the list to
	// prevent textures from staying
	// warm...

	static U32 lasttime = 0;

	U32 time = Platform::getRealMilliseconds();

	if(!sgTimeElapsed(time, lasttime, SG_UNUSED_TIMEOUT))
		return;

	//Con::warnf("Checking for unused textures...");

	// SG_TEXTURE_TIMEOUT...
	sgShadowTextureMultimap *entry = sgGetFirstEntry();
	while(entry)
	{
		for(U32 i=0; i<entry->object.size(); i++)
		{
			GFXTextureObject *texture = entry->object[i];
			if(!texture)
				continue;

			if(!sgTimeElapsed(time, texture->cacheTime, SG_TEXTURE_TIMEOUT))
				continue;

			// dereference, this should be the last reference, which will kill the texture...
			entry->object[i] = NULL;
			entry->object.erase_fast(i);
			entry->info.sgCreateCount--;
			i--;
		}

		entry = entry->linkHigh;
	}

	sgPrintStats();

	//Con::warnf("Done.");
}

void sgShadowTextureCache::sgPrintStats()
{
	if(!sgLightManager::sgShowCacheStats)
		return;

	U32 totalalloc = 0;
	U32 totalcache = 0;
	U32 totalallocbytes = 0;
	U32 totalcachebytes = 0;

	Con::warnf("");
	Con::warnf("-------------------------------------");
	Con::warnf("Lighting System - Texture Cache Stats");

	sgShadowTextureMultimap *entry = sgGetFirstEntry();
	while(entry)
	{
		U32 texturebytes = (U32)GFX->formatByteSize(entry->info.sgFormat) * entry->info.sgSize.x * entry->info.sgSize.y;
		U32 alloc = texturebytes * entry->info.sgCreateCount;
		U32 cache = texturebytes * entry->object.size();

		totalalloc += entry->info.sgCreateCount;
		totalcache += entry->object.size();
		totalallocbytes += alloc;
		totalcachebytes += cache;

		Con::warnf(" %dx%d - alloc: %d, cached: %d, alloc size: %d, cached size: %d",
			entry->info.sgSize.x, entry->info.sgSize.y,
			entry->info.sgCreateCount, entry->object.size(), alloc, cache);

		entry = entry->linkHigh;
	}

	Con::warnf("");
	Con::warnf(" Total - alloc: %d, cached: %d, alloc size: %d, cached size: %d",
		totalalloc, totalcache, totalallocbytes, totalcachebytes);
	Con::warnf("-------------------------------------");
	Con::warnf("");
}
