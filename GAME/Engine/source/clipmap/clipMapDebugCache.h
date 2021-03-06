//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _CLIPMAPDEBUGCACHE_H_
#define _CLIPMAPDEBUGCACHE_H_

#include "clipmap/clipMap.h"
#include "clipmap/clipMapCache.h"

/// A clipmap "cache" that produces a variety of debug patterns on the fly.
///
/// This is very useful for debugging the clipmap shaders as well as
/// visualizing various parts of the clipmap process (LOD selection,
/// cache centering behavior, etc.).
class ClipMapImageCache_Debug : public IClipMapImageCache
{
   /// One of DebugModes.
   U32 mCurDebugMode; 

   /// Render alternating 4px squares on top of the debug mode.
   bool mDoSquares; 

public:
   enum DebugModes
   {
      /// Render flat colors based on the clipmaplevel.
      LOD,

      /// Render a UV gradient based on position in the clipmap level.
      UVGradient,

      /// Render a checkerboard pattern, sized so that it's 1px on the
      /// highest clipmap level.
      Checkers,
   };

   ClipMapImageCache_Debug();
   virtual ~ClipMapImageCache_Debug()
   {
   }

   void setDebugMode(DebugModes dm, bool doSquares)
   {
      mCurDebugMode = dm;
      mDoSquares = doSquares;
   }

   virtual void initialize(U32 clipMapSize, U32 clipMapDepth)
   {
   }

   virtual void setInterestCenter(Point2I origin)
   {
   }

   virtual bool isDataAvailable(U32 mipLevel, RectI region);

   virtual void beginRectUpdates(ClipMap::ClipStackEntry &cse)
   {
   }

   /// This is the only place that does any real work - it copies the debug pattern.
   virtual void doRectUpdate(U32 mipLevel, ClipMap::ClipStackEntry &cse, RectI srcRegion, RectI dstRegion);
   
   virtual void finishRectUpdates(ClipMap::ClipStackEntry &cse)
   {
   }

   virtual bool isRenderToTargetCache()
   {
      return false;
   }
};

#endif