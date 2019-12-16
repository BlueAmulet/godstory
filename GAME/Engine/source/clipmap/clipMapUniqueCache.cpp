//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "clipmap/clipMapUniqueCache.h"

ClipMapUniqueCache::ClipMapUniqueCache( IClipMapImageSource *uniqueSource )
{
   mUniqueSource = uniqueSource;
   mClipMapSize = -1;
}

ClipMapUniqueCache::~ClipMapUniqueCache()
{
   SAFE_DELETE(mUniqueSource);
}

void ClipMapUniqueCache::initialize( ClipMap *cm )
{
   mClipMapSize = cm->mClipMapSize;
}

void ClipMapUniqueCache::setInterestCenter( Point2I origin )
{
   AssertFatal(mClipMapSize != -1, "ClipMapUniqueCache::setInterestCenter - no one set mClipMapSize!");
   mUniqueSource->setInterestCenter(origin, mClipMapSize);
}

void ClipMapUniqueCache::beginRectUpdates( ClipMap::ClipStackEntry &cse )
{
   // No setup required to copy bits.
}

void ClipMapUniqueCache::doRectUpdate( U32 mipLevel, ClipMap::ClipStackEntry &cse, RectI srcRegion, RectI dstRegion )
{
   // NOTE: The GFXTextureHandle::lock() call was broken at one time and
   // used a reversed axis standard.  It has since been fixed, but the
   // clipmap logic has not be fully corrected for this.  So we're swapping
   // the lock axis here until it is fixed properly.
   RectI swappedLock( dstRegion.point.y, dstRegion.point.x, dstRegion.extent.y, dstRegion.extent.x );

   AssertFatal(mipLevel < mUniqueSource->getMipLevelCount(), "ClipMapUniqueCache::doRectUpdate - out of range mip level.");

   // Get a lock and call our blitter.
   GFXLockedRect *lockRect = cse.mTex->lock(0, &swappedLock);
   mUniqueSource->copyBits(mipLevel, NULL, srcRegion, lockRect->bits, lockRect->pitch, 0);
   cse.mTex.unlock();
}

void ClipMapUniqueCache::finishRectUpdates( ClipMap::ClipStackEntry &cse )
{
   // All work is already done in doRectUpdate
}

bool ClipMapUniqueCache::isDataAvailable( U32 mipLevel, RectI region )
{
   return mUniqueSource->isDataAvailable(mipLevel, region);
}

bool ClipMapUniqueCache::isRenderToTargetCache()
{
   return false;
}
