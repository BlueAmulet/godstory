//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _CLIPMAP_CLIPMAPBLENDERCACHE_H_
#define _CLIPMAP_CLIPMAPBLENDERCACHE_H_

#include "gfx/gfxDevice.h"
#include "clipmap/clipMapImageSource.h"
#include "clipmap/clipMapCache.h"

class GFXStateBlock;

class ClipMapBlenderCache : public IClipMapImageCache
{
   /// References to image source for opacity and lightmap information.
   VectorPtr<IClipMapImageSource*> mOpacitySources;
   VectorPtr<IClipMapImageSource*> mLightmapSources;

   // Whether mOpacityScratchTexturesFF should be used, amongst other things
   bool fixedfunction;

   /// Used for streaming video data to card for blend operations.
   Vector<GFXTexHandle> mOpacityScratchTextures;
   Vector<GFXTexHandle> mLightmapScratchTextures;

   Vector<F32> scaleFactors;
   Vector<F32> sourceWidths;

   /// SM2.0 shader used for one pass blending.
   GFXShader *mOnePass;

   /// SM1.0 shaders used for two pass blending.
   GFXShader *mTwoPass[2];

   /// Images that we're blending; in channel-usage order.
   Vector<GFXTexHandle> mSourceImages;

   /// The prefix to use for synthesizing shader names.
   /// This is basically nothing but a hack to allow legacy and Atlas to use
   /// separate shaders to account for differences in their treatment of lightmaps.
   StringTableEntry mShaderBaseName;

   U32 mClipMapSize;
   ClipMap *mOwningClipMap;


   /// Generate appropriate volatile geometry and set it as the current VB
   void setupGeometry( const RectF &srcRect0, const RectF &srcRect1,
                       const RectF &masterCoords, const RectI &dstRect);
   void setupGeometryFF( const RectF &srcRect0, const RectF &masterCoords,
                       const F32 scaleFactor, const RectI &dstRect);
public:

   ClipMapBlenderCache(IClipMapImageSource *opacitySource, IClipMapImageSource *lightmapSource);
   ClipMapBlenderCache(IClipMapImageSource *lightmapSource);
   virtual ~ClipMapBlenderCache();
   virtual void initialize(ClipMap *cm);
   virtual void setInterestCenter(Point2I origin);
   virtual void beginRectUpdates(ClipMap::ClipStackEntry &cse);
   virtual void doRectUpdate(U32 mipLevel, ClipMap::ClipStackEntry &cse, RectI srcRegion, RectI dstRegion);
   virtual void finishRectUpdates(ClipMap::ClipStackEntry &cse);
   virtual bool isDataAvailable(U32 mipLevel, RectI region);
   virtual bool isRenderToTargetCache();

   void clearSourceImages();
   void registerSourceImage(const char *imgPath);

   void clearOpacitySources();
   void registerOpacitySource(IClipMapImageSource *opacitySource);

   void createOpacityScratchTextures();

   void setShaderBaseName( StringTableEntry baseName )
   {
      mShaderBaseName = baseName;
   }

   // Lightmap support
   void setNewLightmapSource(IClipMapImageSource *lightmapSource);

private:
	//begin
	static GFXStateBlock* mBeginSB;
	static GFXStateBlock* mBeginFixSB;
	//do
	static GFXStateBlock* mOnePassSB;
	static GFXStateBlock* mTwoPassSB;
	/// FF
	static GFXStateBlock* mFFBaseLayerSB;
	static GFXStateBlock* mFFAdditionalLayersSB;
	static GFXStateBlock* mFFLightmapSB;
	//finish
	static GFXStateBlock* mFinishSB;

public:
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();

	static void init();
	static void shutdown();
};

#endif
