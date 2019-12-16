//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _CLIPMAP_H_
#define _CLIPMAP_H_

#include "gfx/gfxDevice.h"
#include "gfx/gfxTextureProfile.h"
#include "gfx/gfxTextureHandle.h"
#include "console/console.h"

class IClipMapImageCache;
/// A clipmap texturing helper. This class is responsible for allocating and
/// updating all of the textures & state information for a clipmap.
///
/// A clipmap is a virtualized texture resource, which allows very large
/// textures to be efficiently rendered using only a subset of the required
/// texture memory.
///
/// @see http://www.cs.virginia.edu/~gfx/Courses/2002/BigData/papers/Texturing/Clipmap.pdf 
///      for the canonical SGI clipmap paper.
class ClipMap
{
   S32 mTexCallbackHandle;
   static void texCB( GFXTexCallbackCode code, void *userData );

   /// We have a seperate shader for each number of clipmap levels. This
   /// is 2,3, or 4 currently.
   GFXShader *mShaders[4];

   IClipMapImageCache *mImageCache;

   /// How many texels have we replaced with new data in the course of
   /// operation?
   S32 mTexelsUpdated;

   /// How many update operations have we performed?
   S32 mTotalUpdates;

   U32 getMipLevel( F32 scale );

public:

   inline GFXShader *getShader(const U32 id)
   {
      AssertFatal(id > 0 && id < 4, "AtlasClipMap::getShader - invalid shader id!");
      return mShaders[id];
   }

   /// Clip a rectangle of size <= gridSpacing against grid lines spaced
   /// every gridSpacing units, starting at the origin.
   void clipAgainstGrid(const S32 gridSpacing, const RectI &rect, S32 *outCount, RectI *outBuffer);

   struct ClipStackEntry
   {
      /// Debug texture - typically a 4x4 color coded texture, used for mip
      /// visualization.
      GFXTexHandle mDebugTex;

      /// The texture resource tied to this level of the clipstack.
      GFXTexHandle mTex;

      /// Indicate center point in the master texture for this clipstack entry.
      Point2F      mClipCenter;

      /// Indicate the offset in entry-local texcoords to account for
      /// toroidal addressing.
      Point2I      mToroidalOffset;

      /// Scale factor. Goes from global TCs to local TCs.
      F32          mScale;

      void initDebugTexture(U32 level);

      ClipStackEntry()
      {
         mClipCenter.set(0.5, 0.5);
         mToroidalOffset.set(0, 0);
         mScale = 1.f;
      }

      ~ClipStackEntry()
      {
         mTex = NULL;
      }
   };

   /// Each layer of the clip stack is stored here.
   Vector<ClipStackEntry> mLevels;

   // This is needed by the fixedfunction fallback of terrBatch
   Point4F clipConsts[4];

   /// Dimensions of the textures we're working with.
   U32 mClipMapSize;

   /// How big of a virtual texture are we working with?
   U32 mTextureSize;

   /// How deep of a clipstack are we working with?
   ///
   /// @note this is redundant with mLevels.size()... mostly.
   U32 mClipStackDepth;

   /// How many texels, max, will we upload before we call it quits for
   /// this recenter operation?
   S32 mMaxTexelUploadPerRecenter;

   /// If set, then this clipmap tiles and we should interpret coordinates
   /// outside the unit square as being in repeats.
   bool mTile;

   /// Indicates we want to use debug visualization to make selection of
   /// clip layers more obvious.
   static bool smDebugTextures;
   static bool smSingleTexture;

   ClipMap();
   ~ClipMap();

   /// Generate the clipstack and intermediate values, given valid
   /// mClipMapSize and mTextureSize. Call me once everything (ie, caches)
   /// is initialized.
   void initClipStack();

   /// Fill all the texture levels with a brightly colored test pattern.
   void fillWithTestPattern();

   /// Fill all the textures levels with data from the cache for the current
   /// center.
   ///
   /// @return False if not all requests could be fulfilled.
   bool fillWithTextureData();

   /// Recenter all levels of the clipstack to point center, and do any
   /// reuploads required.
   void recenter(Point2F center);

   /// Calculate one or more rectangles that need to be updated based on the
   /// observed delta.
   void calculateModuloDeltaBounds(const RectI &oldData, const RectI &newData, 
                                    RectI *outRects, S32 *outRectCount, S32 i);

   /// Given some information about a piece of geometry, return what clipmap
   /// levels are most appropriate to use on it. It will give a max of 4
   /// levels, and always return levels such that the geometry will not
   /// exceed the currently loaded data for the specified clipmap levels.
   void calculateClipMapLevels(const F32 near, const F32 far, 
                              const RectF &texBounds, 
                              S32 &outStartLevel, S32 &outEndLevel);

   /// Set the cache we're currently using to service image data requests.
   void setCache(IClipMapImageCache *cache);

   /// Bind appropriate shader and textures for the specified levels of the
   /// clipmap. Note use of default values to specify number of layers we
   /// want bound.
   void bindShaderAndTextures(U32 layer1, U32 layer2, U32 layer3=-1, U32 layer4=-1);

   // Bind appropriate textures and set mapping vector for one layer of clipmap
   void bindTexturesFF(U32 layer, Point4F &clipmapMapping);

   /// Reinitialize the cache with our current settings.
   void refreshCache();
private:
	static GFXStateBlock* mFFSetSB;
	static GFXStateBlock* mFFClampSB;
	static GFXStateBlock* mFFLodBiasSB;
public:
	static void init();
	static void shutdown();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};

GFX_DeclareTextureProfile(ClipMapTextureProfile);
GFX_DeclareTextureProfile(ClipMapTextureRTProfile);

#endif
