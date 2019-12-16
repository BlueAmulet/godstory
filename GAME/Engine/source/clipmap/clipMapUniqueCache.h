//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _CLIPMAP_CLIPMAPUNIQUECACHE_H_
#define _CLIPMAP_CLIPMAPUNIQUECACHE_H_

#include "clipmap/clipMapCache.h"
#include "clipmap/clipMapImageSource.h"

/// Fill clipmap with data from a single large image source.
class ClipMapUniqueCache : public IClipMapImageCache
{
   IClipMapImageSource *mUniqueSource;
   U32 mClipMapSize;

public:
   ClipMapUniqueCache(IClipMapImageSource *uniqueSource);
   virtual ~ClipMapUniqueCache();
   virtual void initialize(ClipMap *cm);
   virtual void setInterestCenter(Point2I origin);
   virtual void beginRectUpdates(ClipMap::ClipStackEntry &cse);
   virtual void doRectUpdate(U32 mipLevel, ClipMap::ClipStackEntry &cse, RectI srcRegion, RectI dstRegion);
   virtual void finishRectUpdates(ClipMap::ClipStackEntry &cse);
   virtual bool isDataAvailable(U32 mipLevel, RectI region);
   virtual bool isRenderToTargetCache();
};

#endif