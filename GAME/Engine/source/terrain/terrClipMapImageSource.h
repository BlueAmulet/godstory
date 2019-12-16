//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#pragma once
#include "clipmap/clipMapImageSource.h"

class TerrClipMapImageSource : public IClipMapImageSource
{
public:
   GFXTexHandle mTex;

   TerrClipMapImageSource(GFXTexHandle& tex, F32 offset);
   ~TerrClipMapImageSource();

   virtual const U32 getMipLevelCount();
   virtual void setInterestCenter(const Point2I origin, const U32 radius);
   virtual bool isDataAvailable(const U32 mipLevel, const RectI region);
   virtual void copyBits(const U32 mipLevel, const ClipMap::ClipStackEntry *cse,
      const RectI srcRegion, U8 *bits, const U32 pitch, const U32 expandonecomponent);
   virtual bool isTiling();
   virtual F32 texelOffset();
	void drawTex(const U32 mipLevel, const RectI srcRegion, GFXTexHandle &tex);
	static GFXShader* getShader();
private:
	static GFXShader* mShader;
   F32 texOffset;
	//state block
	static GFXStateBlock* mSetSB;
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
