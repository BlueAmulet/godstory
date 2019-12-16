//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include <d3dx9math.h>
#include "clipmap/clipMapBlenderCache.h"
#include "materials/shaderData.h"
#include "gfx/D3D9/gfxd3d9StateBlock.h"
#include "gfx/D3D9/gfxD3D9Device.h"


//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK

GFXStateBlock* ClipMapBlenderCache::mBeginSB = NULL;
GFXStateBlock* ClipMapBlenderCache::mBeginFixSB = NULL;
GFXStateBlock* ClipMapBlenderCache::mOnePassSB = NULL;
GFXStateBlock* ClipMapBlenderCache::mTwoPassSB = NULL;
GFXStateBlock* ClipMapBlenderCache::mFFBaseLayerSB = NULL;
GFXStateBlock* ClipMapBlenderCache::mFFAdditionalLayersSB = NULL;
GFXStateBlock* ClipMapBlenderCache::mFFLightmapSB = NULL;
GFXStateBlock* ClipMapBlenderCache::mFinishSB = NULL;
ClipMapBlenderCache::ClipMapBlenderCache( IClipMapImageSource *opacitySource, IClipMapImageSource *lightmapSource )
{
   mOpacitySources.push_back(opacitySource);
   mLightmapSources.push_back(lightmapSource);
   mOwningClipMap = NULL;
   mClipMapSize = -1;
   mShaderBaseName = StringTable->insert( "AtlasBlender" );
}

ClipMapBlenderCache::ClipMapBlenderCache(IClipMapImageSource *lightmapSource)
{
   mLightmapSources.push_back(lightmapSource);
   mOwningClipMap = NULL;
   mClipMapSize = -1;
   mShaderBaseName = StringTable->insert( "AtlasBlender" );
}

ClipMapBlenderCache::~ClipMapBlenderCache()
{
   clearSourceImages();
   clearOpacitySources();

   mOpacityScratchTextures.clear();
   mLightmapScratchTextures.clear();

   for (U32 i = 0; i < mLightmapSources.size(); i++)
      SAFE_DELETE(mLightmapSources[i]);

   mLightmapSources.clear();
}

void ClipMapBlenderCache::createOpacityScratchTextures()
{
   U32 numScratchTextures = 1;
   if (fixedfunction)
      numScratchTextures = 4;

   if (mOpacitySources.size() > 0)
      numScratchTextures *= mOpacitySources.size();

   for (U32 i = mOpacityScratchTextures.size(); i < numScratchTextures; i++)
   {
      mOpacityScratchTextures.increment();
      mOpacityScratchTextures.last().set( 256/*mClipMapSize*/, 256/*mClipMapSize*/, GFXFormatR8G8B8A8, &ClipMapTextureRTProfile, 1);
   }
}

void ClipMapBlenderCache::initialize( ClipMap *cm )
{
   mOwningClipMap = cm;
   mClipMapSize = cm->mClipMapSize;

   float pixVersion = GFX->getPixelShaderVersion();
   if (pixVersion == 0.0)
      fixedfunction = true;
   else
      fixedfunction = false;

   createOpacityScratchTextures();

   mLightmapScratchTextures.push_back(GFXTexHandle( mClipMapSize, mClipMapSize, GFXFormatR8G8B8X8, &ClipMapTextureRTProfile, 1));

   if( !fixedfunction )
   {
      // Find and init shaders.
      ShaderData *sd = NULL;

      bool haveShader = false;
      const char* shader20name = avar( "%s20Shader", mShaderBaseName );
      if( pixVersion >= 2.0 && !Sim::findObject( shader20name, sd ) || (sd->getShader() == NULL))
         Con::errorf("ClipMapBlenderCache::initialize - Couldn't find shader '%s'!", shader20name);
      else
      {
         mOnePass = sd->getShader();
         haveShader = true;
      }

      if( !haveShader && pixVersion >= 1.1 )
      {
         mTwoPass[ 0 ] = mTwoPass[ 1 ] = 0;

         const char* shader11Aname = avar( "%s11AShader", mShaderBaseName );
         if( !Sim::findObject(shader11Aname, sd) || (sd->getShader() == NULL))
            Con::errorf("ClipMapBlenderCache::initialize - Couldn't find shader '%s'!", shader11Aname);
         else
            mTwoPass[0] = sd->getShader();

         const char* shader11Bname = avar( "%s11BShader", mShaderBaseName );
         if( !Sim::findObject(shader11Bname, sd) || (sd->getShader() == NULL))
            Con::errorf("ClipMapBlenderCache::initialize - Couldn't find shader '%s'!", shader11Bname);
         else
            mTwoPass[1] = sd->getShader();

         if( mTwoPass[ 0 ] && mTwoPass[ 1 ] )
            haveShader = true;
      }

      if( !haveShader )
         fixedfunction = true;
   }
}

void ClipMapBlenderCache::setInterestCenter( Point2I origin )
{
   AssertFatal(mClipMapSize != -1, "ClipMapBlenderCache::setInterestCenter - no one set mClipMapSize!");

   if (mOpacitySources.size() == 0)
   {
      Con::errorf("ClipMapBlenderCache::setInterestCenter() - tried to set the interest center without any opacity sources");
      return;
   }

   // For each source, scale origin, which is in texels at the
   // most detailed clipmap level, to be in texels at the most detailed
   // level of each source.
   for (U32 i = 0; i < mOpacitySources.size(); i++)
   {
      Point2I opacityOrigin = origin;
      if( mOpacitySources[i]->getMipLevelCount() != ( getBinLog2( mOwningClipMap->mTextureSize ) + 1 ) )
      {
         S32 scaleDelta = mOpacitySources[i]->getMipLevelCount() - 1 - getBinLog2(mOwningClipMap->mTextureSize);
         if(scaleDelta > 0)
         {
            opacityOrigin.x <<= scaleDelta;
            opacityOrigin.y <<= scaleDelta;
         }
         else
         {
            opacityOrigin.x >>= -scaleDelta;
            opacityOrigin.y >>= -scaleDelta;
         }
      }

      mOpacitySources[i]->setInterestCenter(opacityOrigin, mClipMapSize * 2);
   }

   Point2I lightmapOrigin = origin;

   for (U32 i = 0; i < mLightmapSources.size(); i++)
   {
      if(mLightmapSources[i]->getMipLevelCount() != ( getBinLog2(mOwningClipMap->mTextureSize) + 1 ) )
      {
         S32 scaleDelta = mLightmapSources[i]->getMipLevelCount() - 1 - getBinLog2(mOwningClipMap->mTextureSize);
         if(scaleDelta > 0)
         {
            lightmapOrigin.x <<= scaleDelta;
            lightmapOrigin.y <<= scaleDelta;
         }
         else
         {
            lightmapOrigin.x >>= -scaleDelta;
            lightmapOrigin.y >>= -scaleDelta;
         }
      }

      mLightmapSources[i]->setInterestCenter(lightmapOrigin, mClipMapSize * 2);
   }
}

bool ClipMapBlenderCache::isDataAvailable( U32 mipLevel, RectI region )
{
   // Scale the region to the available depths of the sources.
   for (U32 i = 0; i < mOpacitySources.size(); i ++)
   {
      RectI opacityRegion = region;
      S32 opacityLevel = mipLevel;
      if(mipLevel >= mOpacitySources[i]->getMipLevelCount())
      {
         opacityLevel = mOpacitySources[i]->getMipLevelCount()-1;
         S32 scaleDelta = mipLevel - mOpacitySources[i]->getMipLevelCount() + 1;
         opacityRegion.point.x = region.point.x >> scaleDelta;
         opacityRegion.point.y = region.point.y >> scaleDelta;
         opacityRegion.extent.x = region.extent.x >> scaleDelta;
         opacityRegion.extent.y = region.extent.y >> scaleDelta;
      }

      if(!mOpacitySources[i]->isDataAvailable(opacityLevel, opacityRegion))
      return false;
   }

   for (U32 i = 0; i < mLightmapSources.size(); i++)
   {
      RectI lightmapRegion = region;
      S32 lightmapLevel = mipLevel;
      if(mipLevel >= mLightmapSources[i]->getMipLevelCount())
      {
         lightmapLevel = mLightmapSources[i]->getMipLevelCount()-1;
         S32 scaleDelta = mipLevel - mLightmapSources[i]->getMipLevelCount() + 1;
         lightmapRegion.point.x = region.point.x >> scaleDelta;
         lightmapRegion.point.y = region.point.y >> scaleDelta;
         lightmapRegion.extent.x = region.extent.x >> scaleDelta;
         lightmapRegion.extent.y = region.extent.y >> scaleDelta;
      }

      if(!mLightmapSources[i]->isDataAvailable(lightmapLevel, lightmapRegion))
         return false;
   }

   return true;
}

bool ClipMapBlenderCache::isRenderToTargetCache()
{
   // We do render-target operations so must have a render-target allocated.
   return true;
}

// Ugly globals.
static MatrixF proj;
static RectI viewPort;
static bool mustEndScene = false;
static S32 pushDepth = 0;

#ifdef STATEBLOCK
void ClipMapBlenderCache::beginRectUpdates( ClipMap::ClipStackEntry &cse )
{
	pushDepth++;
	AssertFatal(pushDepth == 1, "BAD1");

	if(!GFX->canCurrentlyRender())
	{
		mustEndScene = true;
		GFX->beginScene();
	}
	else
	{
		mustEndScene = false;
	}

	proj = GFX->getProjectionMatrix();
	viewPort = GFX->getViewport();
	GFX->pushWorldMatrix();

	//// For sanity, let's just purge all our texture states.
	//for(S32 i=0; i<GFX->getNumSamplers(); i++)
	//   GFX->setTexture(i, NULL);
	//GFX->updateStates();

	// Set a render target...
	GFX->pushActiveRenderTarget();
	GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
	myTarg->attachTexture(GFXTextureTarget::Color0, cse.mTex );
	GFX->setActiveRenderTarget( myTarg );

	// And some render states.
	GFX->setWorldMatrix(MatrixF(true));
	GFX->setProjectionMatrix(MatrixF(true));
	//GFX->disableShaders();


	GFX->setClipRect(RectI(0,0, mClipMapSize, mClipMapSize));

	if (fixedfunction)
	{
		AssertFatal(mBeginSB, "ClipMapBlenderCache::beginRectUpdates -- mBeginSB cannot be NULL.");
		mBeginSB->apply();
	}
	else
	{
		// And pass it what it needs modelview wise...
		MatrixF proj = GFX->getProjectionMatrix();
		proj.mul(GFX->getWorldMatrix());
		proj.transpose();
		GFX->setVertexShaderConstF( 0, (float*)&proj, 4 );
		AssertFatal(mBeginFixSB, "ClipMapBlenderCache::beginRectUpdates -- mBeginFixSB cannot be NULL.");
		mBeginFixSB->apply();
	}
	// Scale the blended textures appropriately.
	U32 totalSize = mOwningClipMap->mTextureSize;

	//AssertFatal(mSourceImages.size() < 4,  "ClipMapBlenderCache::beginRectUpdates - too many sources, we only handle 4.");
	for(S32 i=0; i< mSourceImages.size(); i++)
	{
		if (i >= scaleFactors.size())
			scaleFactors.push_back(128.0f);

		F32 scale = totalSize / sourceWidths[i];
		scaleFactors[i] = scale;
	}

	if (!fixedfunction && scaleFactors.size() > 0)
		GFX->setVertexShaderConstF(50, scaleFactors.address(), 1);

	//GFX->updateStates();
}

#else
void ClipMapBlenderCache::beginRectUpdates( ClipMap::ClipStackEntry &cse )
{
   pushDepth++;
   AssertFatal(pushDepth == 1, "BAD1");

   if(!GFX->canCurrentlyRender())
   {
      mustEndScene = true;
      GFX->beginScene();
   }
   else
   {
      mustEndScene = false;
   }

   proj = GFX->getProjectionMatrix();
   viewPort = GFX->getViewport();
   GFX->pushWorldMatrix();

   //// For sanity, let's just purge all our texture states.
   //for(S32 i=0; i<GFX->getNumSamplers(); i++)
   //   GFX->setTexture(i, NULL);
   //GFX->updateStates();

   // Set a render target...
   GFX->pushActiveRenderTarget();
   GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
   myTarg->attachTexture(GFXTextureTarget::Color0, cse.mTex );
   GFX->setActiveRenderTarget( myTarg );

   // And some render states.
   GFX->setZEnable(false);
   GFX->setZWriteEnable(false);
   GFX->setWorldMatrix(MatrixF(true));
   GFX->setProjectionMatrix(MatrixF(true));
   //GFX->disableShaders();

   
   GFX->setClipRect(RectI(0,0, mClipMapSize, mClipMapSize));
   GFX->setCullMode( GFXCullNone );

   if (fixedfunction)
   {
      for (U32 i=0; i<2; i++)
      {
         GFX->setTextureStageMagFilter(i, GFXTextureFilterLinear);
         GFX->setTextureStageMinFilter(i, GFXTextureFilterLinear);
         GFX->setTextureStageMipFilter(i, GFXTextureFilterLinear);
      }
      GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
      GFX->setTextureStageAddressModeV(0, GFXAddressClamp);
      GFX->setTextureStageColorOp(0, GFXTOPSelectARG1);
      GFX->setTextureStageColorArg1(0, GFXTATexture);
      GFX->setTextureStageAddressModeU(1, GFXAddressWrap);
      GFX->setTextureStageAddressModeV(1, GFXAddressWrap);
      GFX->setTextureStageColorOp(1, GFXTOPModulate);
   }
   else
   {
      // And pass it what it needs modelview wise...
      MatrixF proj = GFX->getProjectionMatrix();
      proj.mul(GFX->getWorldMatrix());
      proj.transpose();
      GFX->setVertexShaderConstF( 0, (float*)&proj, 4 );

      for(S32 i=0; i<2; i++)
      {
         GFX->setTexture(i, NULL);
         GFX->setTextureStageMagFilter(i, GFXTextureFilterLinear);
         GFX->setTextureStageMinFilter(i, GFXTextureFilterLinear);
         GFX->setTextureStageMipFilter(i, GFXTextureFilterLinear);
         GFX->setTextureStageAddressModeU(i, GFXAddressClamp);
         GFX->setTextureStageAddressModeV(i, GFXAddressClamp);
      }

      for(S32 i=0; i<mClamp(mSourceImages.size(), 0, GFX->getNumSamplers()-2); i++)
      {
         GFX->setTexture(i+2,mSourceImages[i]);
         GFX->setTextureStageMagFilter(i+2, GFXTextureFilterLinear);
         GFX->setTextureStageMinFilter(i+2, GFXTextureFilterLinear);
         GFX->setTextureStageMipFilter(i+2, GFXTextureFilterLinear);
         GFX->setTextureStageAddressModeU(i+2, GFXAddressWrap);
         GFX->setTextureStageAddressModeV(i+2, GFXAddressWrap);
      }
   }

   // Set blend modes. (we want additive).
   GFX->setAlphaBlendEnable(true);
   GFX->setSrcBlend(GFXBlendOne);
   GFX->setDestBlend(GFXBlendOne);

   // No alpha test,
   GFX->setAlphaTestEnable(false);

   // Scale the blended textures appropriately.
   U32 totalSize = mOwningClipMap->mTextureSize;

   //AssertFatal(mSourceImages.size() < 4,  "ClipMapBlenderCache::beginRectUpdates - too many sources, we only handle 4.");
   for(S32 i=0; i< mSourceImages.size(); i++)
   {
      if (i >= scaleFactors.size())
         scaleFactors.push_back(128.0f);

      F32 scale = totalSize / sourceWidths[i];
      scaleFactors[i] = scale;
   }

   if (!fixedfunction && scaleFactors.size() > 0)
	   GFX->setVertexShaderConstF(50, scaleFactors.address(), 1);

   //GFX->updateStates();
}

#endif

void calcScaleRegionStuff( VectorPtr<IClipMapImageSource*>& sources, U32 levelSize, RectI srcRegion,
                           Vector<GFXTexHandle>& scratchTexs, RectF &outTexCoord, bool fixedFunction)
{
   // Quick pre-sanity check to make sure that there are enough scratch textures
   if (sources.size() == 0)
      return;
   if (scratchTexs.size() == 0)
      return;
   else if (fixedFunction && scratchTexs.size() < sources.size() * 4)
      return;
   else if (!fixedFunction && scratchTexs.size() < sources.size())
      return;

   // Beware! This function assumes that all of the sources and
   // scratchTexs are of the same size and formats
   //
   // source - the image source we're getting our data from
   // levelSize - size of this clipmap level in texels
   // srcRegion - region of this layer of the clipmap we're updating.
   // scratchTexs - textures we're uploading our data to.
   // tc - texture coordinates for the requested source data.
   Point2I scratchSize(1, 1);

   if (scratchTexs.size() > 0)
      scratchSize.set(scratchTexs[0].getWidth(), scratchTexs[0].getHeight());

   // What's the maximum resolution available for our source?
   S32 maxSourceSize = BIT(sources[0]->getMipLevelCount()-1);

   // Our upload will be noted here.
   RectI uploadRegion;
   U32 uploadLevel = -1;

   // Is this a non-scale situation?
   if(levelSize <= maxSourceSize)
   {
      // simple - just blast the right level up.
      uploadLevel = getBinLog2(levelSize);
      uploadRegion = srcRegion;

      outTexCoord.point.x = 0;
      outTexCoord.point.y = 0;
      outTexCoord.extent.x = F32(srcRegion.extent.x) / F32(scratchSize.x);
      outTexCoord.extent.y = F32(srcRegion.extent.y) / F32(scratchSize.y);
   }
   else
   {
      // harder - have to fudge it out by a texel in all directions, and
      //          generate TCs

      // First, figure scale factor
      S32 scaleDelta = getBinLog2(levelSize) - getBinLog2(maxSourceSize);
      F32 scaleFactor = F32(BIT(scaleDelta));

      // From this we know what level we're uploading.
      uploadLevel = getBinLog2(maxSourceSize);

      // Scale the rectangle.
      uploadRegion.point.x = (S32)mFloor(F32(srcRegion.point.x) / scaleFactor);
      uploadRegion.point.y = (S32)mFloor(F32(srcRegion.point.y) / scaleFactor);

      uploadRegion.extent.x = (S32)mCeil(F32(srcRegion.point.x + srcRegion.extent.x) / scaleFactor) - uploadRegion.point.x;
      uploadRegion.extent.y = (S32)mCeil(F32(srcRegion.point.y + srcRegion.extent.y) / scaleFactor) - uploadRegion.point.y;

      // Now, if possible we want a 1tx border around the region we're using.
      // This allows us to avoid nasty seam issues.

      // Nudging logic varies based on whether this is a tiling source or not.
      if(sources[0]->isTiling())
      {
         // We can always nudge since tiling sources support arbitrary size
         // uploads (since they tile).

         // We can nudge x, so do it.
         uploadRegion.point.x--;
         uploadRegion.extent.x++;
         uploadRegion.extent.x++;

         // We can nudge y, so do it.
         uploadRegion.point.y--;
         uploadRegion.extent.y++;
         uploadRegion.extent.y++;
      }
      else
      {
         // Nudge out left...
         if(uploadRegion.point.x > 0)
         {
            uploadRegion.point.x--;
            uploadRegion.extent.x++;
         }

         // Nudge out right..
         if((uploadRegion.extent.x + uploadRegion.point.x) < (maxSourceSize-1))
         uploadRegion.extent.x++;

         // Nudge top...
         if(uploadRegion.point.y > 0)
         {
            uploadRegion.point.y--;
            uploadRegion.extent.y++;
         }

         // Nudge bottom...
         if((uploadRegion.extent.y + uploadRegion.point.y) < (maxSourceSize-1))
            uploadRegion.extent.y++;
      }


      // Calculate the texture coordinates.
      outTexCoord.point.x = (F32(srcRegion.point.x) / scaleFactor - F32(uploadRegion.point.x)) / scratchSize.x;
      outTexCoord.point.y = (F32(srcRegion.point.y) / scaleFactor - F32(uploadRegion.point.y)) / scratchSize.y;
      outTexCoord.extent.x = (F32(srcRegion.extent.x) / scaleFactor) / scratchSize.x;
      outTexCoord.extent.y = (F32(srcRegion.extent.y) / scaleFactor) / scratchSize.y;

      outTexCoord.point.x += (sources[0]->texelOffset() / scratchSize.x);
      outTexCoord.point.y += (sources[0]->texelOffset() / scratchSize.y);
   }

//    Con::printf(" %x srcRegion=(%d, %d, %d, %d) outTexCoord=(%f, %f, %f, %f)",
//       source,
//       srcRegion.point.x, srcRegion.point.y, srcRegion.extent.x, srcRegion.extent.y,
//       outTexCoord.point.x, outTexCoord.point.y, outTexCoord.extent.x, outTexCoord.extent.y);


   AssertFatal(uploadLevel != -1, "calcScaleRegionStuff - didn't set uploadLevel, somehow skipped all our upload setup logic.");

   // Lock the texture and upload data from the source.

   // NOTE: The GFXTextureHandle::lock() call was broken at one time and
   // used a reversed axis standard.  It has since been fixed, but the Atlas
   // clip map logic has not be fully corrected for this.  So we're swapping
   // the lock axis here until it is fixed properly.
   RectI swappedLock(0, 0, uploadRegion.extent.y, uploadRegion.extent.x);

   for (U32 i = 0; i < sources.size(); i++)
   {
      U32 scratchPerSource = 1;

      if (fixedFunction)
         scratchPerSource = 4;

      for (U32 component = 0; component < scratchPerSource; component++)
      {
         U32 expand = 0;

         if (fixedFunction)
            expand = component < 3 ? (3 - component) : 4;

         //GFXLockedRect *glr = scratchTexs[i * scratchPerSource + component].lock(0, &swappedLock);
         //   AssertFatal(glr, "ClipMapBlenderCache::calcScaleRegionStuff - couldn't lock scratch texture");
         //sources[i]->copyBits(uploadLevel, NULL, uploadRegion, glr->bits, glr->pitch, expand );
         //scratchTexs[i * scratchPerSource + component].unlock();
			sources[i]->drawTex(uploadLevel, uploadRegion, scratchTexs[i*scratchPerSource + component]);
      }
   }

}

void ClipMapBlenderCache::doRectUpdate( U32 mipLevel, ClipMap::ClipStackEntry &cse, RectI srcRegion, RectI dstRegion )
{
   AssertFatal(pushDepth==1, "ClipMapBlenderCache::doRectUpdate - "
                                             "someone pushed more than once.");
   // Hopefully we never hit these but it is a pretty quick check just in case
   if (fixedfunction && mOpacityScratchTextures.size() < mOpacitySources.size() * 4)
      createOpacityScratchTextures();
   else if (!fixedfunction && mOpacityScratchTextures.size() < mOpacitySources.size())
      createOpacityScratchTextures();

   // First, figure out what mip level of the image sources we need to copy
   // data from. Image sources actually work in real mip levels (level 0 is
   // full size, level N is 1px square), so we need to figure the level
   // we want and scale it up as appropriate.

   const F32 levelSize = cse.mScale * mClipMapSize;

   RectF opacityTC;
   calcScaleRegionStuff(mOpacitySources, levelSize, srcRegion,
      mOpacityScratchTextures, opacityTC, fixedfunction);

   RectF lightmapTC;
   calcScaleRegionStuff(mLightmapSources, levelSize, srcRegion,
      mLightmapScratchTextures, lightmapTC, false);

   RectF masterRect;
   masterRect.point.x = F32(srcRegion.point.x) / levelSize;
   masterRect.point.y = F32(srcRegion.point.y) / levelSize;
   masterRect.extent.x = F32(srcRegion.extent.x) / levelSize;
   masterRect.extent.y = F32(srcRegion.extent.y) / levelSize;

   if (fixedfunction)
   {
      // Do 5 pass (per opacity) fixedfunction
#ifdef STATEBLOCK
		AssertFatal(mFFBaseLayerSB, "ClipMapBlenderCache::doRectUpdate -- mFFBaseLayerSB cannot be NULL.");
		mFFBaseLayerSB->apply();
#else
      // Draw first layer
      GFX->setTextureStageColorOp(1, GFXTOPModulate);
      GFX->setAlphaBlendEnable(false);
#endif

      setupGeometryFF(opacityTC, masterRect, scaleFactors[0], dstRegion);
      GFX->setTexture(0, mOpacityScratchTextures[0]);
      GFX->setTexture(1, mSourceImages[0]);
      GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );
#ifdef STATEBLOCK
		AssertFatal(mFFAdditionalLayersSB, "ClipMapBlenderCache::doRectUpdate -- mFFAdditionalLayersSB cannot be NULL.");
		mFFAdditionalLayersSB->apply();
#else
      // Apply additional layers
      GFX->setAlphaBlendEnable(true);
      GFX->setSrcBlend(GFXBlendOne);
      GFX->setDestBlend(GFXBlendOne);
#endif

      for(S32 i=1; i<mOpacityScratchTextures.size(); i++)
      {
         setupGeometryFF(opacityTC, masterRect, scaleFactors[i], dstRegion);
         GFX->setTexture(0, mOpacityScratchTextures[i]);
         GFX->setTexture(1, mSourceImages[i]);
         GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );
      }
      // Multiply by lightmap*2
      if (mLightmapScratchTextures.size() > 0)
      {
         setupGeometryFF(lightmapTC, masterRect, 0, dstRegion);
#ifdef STATEBLOCK
			AssertFatal(mFFLightmapSB, "ClipMapBlenderCache::doRectUpdate -- mFFLightmapSB cannot be NULL.");
			mFFLightmapSB->apply();
#else
         GFX->setSrcBlend(GFXBlendDestColor);
         GFX->setDestBlend(GFXBlendSrcColor);
         GFX->setTextureStageColorOp(1, GFXTOPDisable);
#endif
         for (U32 i = 0; i < mLightmapScratchTextures.size(); i++)
         {
            GFX->setTexture(0, mLightmapScratchTextures[i]);
            GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );
         }
      }
   }
   else
   {
      // Do shader rendering...
      // Set up our geometry.
      setupGeometry(
         opacityTC,
         lightmapTC,
         masterRect,
         dstRegion);

      // And draw our blended quad...
      GFX->setTexture(0, mOpacityScratchTextures[0]);
      GFX->setTexture(1, mLightmapScratchTextures[0]);

      if(GFX->getPixelShaderVersion() < 2.0 || !mOnePass )
      {
         // Do two pass shader w/ additive blending.
         GFX->setShader(mTwoPass[0]);
         //GFX->setAlphaBlendEnable(false);
#ifdef STATEBLOCK
			AssertFatal(mOnePassSB, "ClipMapBlenderCache::doRectUpdate -- mOnePassSB cannot be NULL.");
			mOnePassSB->apply();
#else
         GFX->setAlphaBlendEnable(false);
#endif
         GFX->setTexture(2, (0 < mSourceImages.size()) ? mSourceImages[0] : NULL);
         GFX->setTexture(3, (1 < mSourceImages.size()) ? mSourceImages[1] : NULL);
         GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );

         GFX->setShader(mTwoPass[1]);
         //GFX->setAlphaBlendEnable(true);
#ifdef STATEBLOCK
			AssertFatal(mTwoPassSB, "ClipMapBlenderCache::doRectUpdate -- mTwoPassSB cannot be NULL.");
			mTwoPassSB->apply();
#else
         GFX->setAlphaBlendEnable(true);
#endif
         GFX->setTexture(2, (2 < mSourceImages.size()) ? mSourceImages[2] : NULL);
         GFX->setTexture(3, (3 < mSourceImages.size()) ? mSourceImages[3] : NULL);
         GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );

         // Additional passes for extra opacity maps
         for (U32 i = 1; i < mOpacityScratchTextures.size(); i++)
         {
            GFX->setTexture(0, mOpacityScratchTextures[i]);

            GFX->setVertexShaderConstF(50, scaleFactors.address() + i * 4, 1);

            GFX->setShader(mTwoPass[0]);
            GFX->setTexture(2, (i * 4     < mSourceImages.size()) ? mSourceImages[i * 4]     : NULL);
            GFX->setTexture(3, (i * 4 + 1 < mSourceImages.size()) ? mSourceImages[i * 4 + 1] : NULL);
            GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );

            GFX->setShader(mTwoPass[1]);
            GFX->setTexture(2, (i * 4 + 2 < mSourceImages.size()) ? mSourceImages[i * 4 + 2] : NULL);
            GFX->setTexture(3, (i * 4 + 3 < mSourceImages.size()) ? mSourceImages[i * 4 + 3] : NULL);
            GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );
         }
      }
      else
      {
         // Do one pass per opacity.
         GFX->setShader(mOnePass);

#ifdef STATEBLOCK
			AssertFatal(mOnePassSB, "ClipMapBlenderCache::doRectUpdate -- mOnePassSB cannot be NULL.");
			mOnePassSB->apply();
#else
         GFX->setAlphaBlendEnable(false);
#endif
         GFX->setTexture(2, (0 < mSourceImages.size()) ? mSourceImages[0] : NULL);
         GFX->setTexture(3, (1 < mSourceImages.size()) ? mSourceImages[1] : NULL);
         GFX->setTexture(4, (2 < mSourceImages.size()) ? mSourceImages[2] : NULL);
         GFX->setTexture(5, (3 < mSourceImages.size()) ? mSourceImages[3] : NULL);
         GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );

#ifdef STATEBLOCK
			AssertFatal(mTwoPassSB, "ClipMapBlenderCache::doRectUpdate -- mTwoPassSB cannot be NULL.");
			mTwoPassSB->apply();
#else
         GFX->setAlphaBlendEnable(true);
#endif
         for (U32 i = 1; i < mOpacityScratchTextures.size(); i++)
         {
            GFX->setTexture(0, mOpacityScratchTextures[i]);

            GFX->setVertexShaderConstF(50, scaleFactors.address() + i * 4, 1);

            GFX->setTexture(2, (i * 4     < mSourceImages.size()) ? mSourceImages[i * 4]     : NULL);
            GFX->setTexture(3, (i * 4 + 1 < mSourceImages.size()) ? mSourceImages[i * 4 + 1] : NULL);
            GFX->setTexture(4, (i * 4 + 2 < mSourceImages.size()) ? mSourceImages[i * 4 + 2] : NULL);
            GFX->setTexture(5, (i * 4 + 3 < mSourceImages.size()) ? mSourceImages[i * 4 + 3] : NULL);
            GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );
         }
      }
   }

   for (U32 i = 0; i < 4; i++ )
       GFX->setTexture(i, NULL);

   GFX->setVertexBuffer( NULL );
}

void ClipMapBlenderCache::finishRectUpdates( ClipMap::ClipStackEntry &cse )
{
   pushDepth--;
   AssertFatal(pushDepth == 0, "BAD2");

   // And restore the render target.
   GFX->popActiveRenderTarget();
	{
		GFX->popWorldMatrix();
		GFX->setProjectionMatrix(proj);
		GFX->setViewport(viewPort);
		MatrixF proj = GFX->getProjectionMatrix();
		proj.mul(GFX->getWorldMatrix());
		proj.transpose();


#ifdef STATEBLOCK
		AssertFatal(mFinishSB, "ClipMapBlenderCache::finishRectUpdates -- mFinishSB cannot be NULL.");
		mFinishSB->apply();
#else
		GFX->setZEnable(true);
		GFX->setZWriteEnable(true);
		GFX->setAlphaBlendEnable(false);
		GFX->setAlphaTestEnable(false);
#endif

		GFX->setVertexShaderConstF( 0, (float*)&proj, 4 );
		if(mustEndScene)
			GFX->endScene();
	}

	return;
//#ifndef POWER_OS_XENON
//   // Extrude mips...
////   ((GFXD3D9TextureObject*)cse.mTex.getPointer())->get2DTex()->GenerateMipSubLevels();
//#else
//#  pragma message("Gotta do a stretchrect here pat, Love Pat." )
//#endif
//
//   // Reset texture stages.
//   // For sanity, let's just purge all our texture states.
//   for(S32 i=0; i<GFX->getNumSamplers(); i++)
//      GFX->setTexture(i, NULL);
//   GFX->updateStates();
//
//   // And clear render states.
//   GFX->setZEnable(true);
//   GFX->setZWriteEnable(true);
//   GFX->popWorldMatrix();
//   GFX->setProjectionMatrix(proj);
//   GFX->setViewport(viewPort);
//
//   GFX->setAlphaBlendEnable(false);
//   GFX->setAlphaTestEnable(false);
//
//   // Reset the shader modelview as well...
//   MatrixF proj = GFX->getProjectionMatrix();
//   proj.mul(GFX->getWorldMatrix());
//   proj.transpose();
//   GFX->setVertexShaderConstF( 0, (float*)&proj, 4 );
//
//   // Purge state before we end the scene - just in case.
//   GFX->updateStates();
//
//   if(mustEndScene)
//      GFX->endScene();
}

void ClipMapBlenderCache::setupGeometry(
   const RectF &srcRect0,
   const RectF &srcRect1,
   const RectF &masterCoords,
   const RectI &dstRect)
{
   GFXVertexBufferHandle<GFXVertexPTTT> verts(GFX, 4, GFXBufferTypeVolatile );

   const F32 masterLeft   = F32(masterCoords.point.y);
   const F32 masterRight  = F32(masterCoords.point.y + masterCoords.extent.y);
   const F32 masterTop    = F32(masterCoords.point.x);
   const F32 masterBottom = F32(masterCoords.point.x + masterCoords.extent.x);

   const F32 tex0Left   = F32(srcRect0.point.y)                     / 1.f; //F32(mClipMapSize);
   const F32 tex0Right  = F32(srcRect0.point.y + srcRect0.extent.y) / 1.f; //F32(mClipMapSize);
   const F32 tex0Top    = F32(srcRect0.point.x)                     / 1.f; //F32(mClipMapSize);
   const F32 tex0Bottom = F32(srcRect0.point.x + srcRect0.extent.x) / 1.f; //F32(mClipMapSize);

   const F32 tex1Left   = F32(srcRect1.point.y)                     / 1.f; //F32(mClipMapSize);
   const F32 tex1Right  = F32(srcRect1.point.y + srcRect1.extent.y) / 1.f; //F32(mClipMapSize);
   const F32 tex1Top    = F32(srcRect1.point.x)                     / 1.f; //F32(mClipMapSize);
   const F32 tex1Bottom = F32(srcRect1.point.x + srcRect1.extent.x) / 1.f; //F32(mClipMapSize);

   const F32 screenLeft   = F32(dstRect.point.y);
   const F32 screenRight  = F32(dstRect.point.y + dstRect.extent.y);
   const F32 screenTop    = F32(dstRect.point.x);
   const F32 screenBottom = F32(dstRect.point.x + dstRect.extent.x);

   const F32 fillConv = GFX->getFillConventionOffset();

   verts.lock();

   verts[0].point.set( screenLeft  - fillConv, screenTop    - fillConv, 0.f );
   verts[0].texCoord1.set( tex0Left,  tex0Top );
   verts[0].texCoord2.set( tex1Left,  tex1Top );
   verts[0].texCoord3.set( masterLeft,  masterTop );

   verts[1].point.set( screenRight - fillConv, screenTop    - fillConv, 0.f );
   verts[1].texCoord1.set( tex0Right, tex0Top );
   verts[1].texCoord2.set( tex1Right, tex1Top );
   verts[1].texCoord3.set( masterRight, masterTop );

   verts[2].point.set( screenLeft  - fillConv, screenBottom - fillConv, 0.f );
   verts[2].texCoord1.set( tex0Left,  tex0Bottom );
   verts[2].texCoord2.set( tex1Left,  tex1Bottom );
   verts[2].texCoord3.set( masterLeft,  masterBottom );

   verts[3].point.set( screenRight - fillConv, screenBottom - fillConv, 0.f );
   verts[3].texCoord1.set( tex0Right, tex0Bottom );
   verts[3].texCoord2.set( tex1Right, tex1Bottom );
   verts[3].texCoord3.set( masterRight, masterBottom );

   verts.unlock();

   GFX->setVertexBuffer( verts );
}

void ClipMapBlenderCache::setupGeometryFF(
   const RectF &srcRect0,
   const RectF &masterCoords,
   const F32 scaleFactor,
   const RectI &dstRect)
{
   GFXVertexBufferHandle<GFXVertexPTT> verts(GFX, 4, GFXBufferTypeVolatile );

   const F32 masterLeft   = F32(masterCoords.point.y)*scaleFactor;
   const F32 masterRight  = F32(masterCoords.point.y + masterCoords.extent.y)*scaleFactor;
   const F32 masterTop    = F32(masterCoords.point.x)*scaleFactor;
   const F32 masterBottom = F32(masterCoords.point.x + masterCoords.extent.x)*scaleFactor;

   const F32 tex0Left   = F32(srcRect0.point.y)                     / 1.f; //F32(mClipMapSize);
   const F32 tex0Right  = F32(srcRect0.point.y + srcRect0.extent.y) / 1.f; //F32(mClipMapSize);
   const F32 tex0Top    = F32(srcRect0.point.x)                     / 1.f; //F32(mClipMapSize);
   const F32 tex0Bottom = F32(srcRect0.point.x + srcRect0.extent.x) / 1.f; //F32(mClipMapSize);

   const F32 screenLeft   = F32(dstRect.point.y);
   const F32 screenRight  = F32(dstRect.point.y + dstRect.extent.y);
   const F32 screenTop    = F32(dstRect.point.x);
   const F32 screenBottom = F32(dstRect.point.x + dstRect.extent.x);

   const F32 fillConv = GFX->getFillConventionOffset();

   verts.lock();

   verts[0].point.set( screenLeft  - fillConv, screenTop    - fillConv, 0.f );
   verts[0].texCoord1.set( tex0Left,  tex0Top );
   verts[0].texCoord2.set( masterLeft,  masterTop );

   verts[1].point.set( screenRight - fillConv, screenTop    - fillConv, 0.f );
   verts[1].texCoord1.set( tex0Right, tex0Top );
   verts[1].texCoord2.set( masterRight, masterTop );

   verts[2].point.set( screenLeft  - fillConv, screenBottom - fillConv, 0.f );
   verts[2].texCoord1.set( tex0Left,  tex0Bottom );
   verts[2].texCoord2.set( masterLeft,  masterBottom );

   verts[3].point.set( screenRight - fillConv, screenBottom - fillConv, 0.f );
   verts[3].texCoord1.set( tex0Right, tex0Bottom );
   verts[3].texCoord2.set( masterRight, masterBottom );

   verts.unlock();

   GFX->setVertexBuffer( verts );
}

void ClipMapBlenderCache::registerSourceImage( const char *filename )
{
   mSourceImages.increment();
   sourceWidths.push_back(256.0f);

   if(!filename || !filename[0])
   {
      // If we are fed blank, we need to have a blank source
      // for the shader.
      GBitmap* bmp = new GBitmap(2,2);
      dMemset(bmp->getWritableBits(0), 0, bmp->byteSize);
      mSourceImages.last().set(bmp, &GFXDefaultStaticDiffuseProfile, true);
      return;
   }

   GBitmap *bmp  = GBitmap::load(filename);
   if(bmp)
   {
      bmp->extrudeMipLevels();
      mSourceImages.last().set(bmp, &GFXDefaultStaticDiffuseProfile, true);
   }

   if(mSourceImages.last().isValid())
      sourceWidths.last() = mSourceImages.last().getWidth();
      

   // If we can't find it, complain.
   if(!mSourceImages.last() && filename && filename[0])
      Con::warnf("AtlasClipMapImageCache_Blender::registerSourceImage - couldn't locate texture '%s' for blending.", filename);
}

void ClipMapBlenderCache::clearSourceImages()
{
	for (U32 i=0; i<mSourceImages.size(); i++)
	{
		mSourceImages[i].free();
	}
   mSourceImages.clear();
   scaleFactors.clear();
}

void ClipMapBlenderCache::clearOpacitySources()
{
   mOpacitySources.clear();
}

void ClipMapBlenderCache::registerOpacitySource(IClipMapImageSource *opacitySource)
{
   mOpacitySources.push_back(opacitySource);
}

void ClipMapBlenderCache::setNewLightmapSource(IClipMapImageSource *lightmapSource)
{
   IClipMapImageSource * currLightMapSource = mLightmapSources.last();
   SAFE_DELETE(currLightMapSource);
   mLightmapSources.last() = lightmapSource;
}

void ClipMapBlenderCache::releaseStateBlock()
{
	if (mBeginSB)
	{
		mBeginSB->release();
	}

	if (mBeginFixSB)
	{
		mBeginFixSB->release();
	}

	if (mOnePassSB)
	{
		mOnePassSB->release();
	}

	if (mTwoPassSB)
	{
		mTwoPassSB->release();
	}

	if (mFFBaseLayerSB)
	{
		mFFBaseLayerSB->release();
	}

	if (mFFAdditionalLayersSB)
	{
		mFFAdditionalLayersSB->release();
	}

	if (mFFLightmapSB)
	{
		mFFLightmapSB->release();
	}

	if (mFinishSB)
	{
		mFinishSB->release();
	}
}

void ClipMapBlenderCache::resetStateBlock()
{
	//mBeginSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);

	for(S32 i=0; i<2; i++)
	{
		GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressClamp);
		GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressClamp);
	}
	for(S32 i=2; i<6; i++)
	{
		GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressWrap);
		GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressWrap);
	}
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->endStateBlock(mBeginSB);

	//mBeginFixSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);

	for(S32 i=0; i<2; i++)
	{
		GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);
	}

	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressClamp);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPSelectARG1);
	GFX->setTextureStageState(0, GFXTSSColorArg1, GFXTATexture);

	GFX->setSamplerState(1, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(1, GFXSAMPAddressV, GFXAddressWrap);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPModulate);

	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->endStateBlock(mBeginFixSB);

	//mOnePassSB
	GFX->beginStateBlock();
	for(S32 i=0; i<2; i++)
	{
		GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressClamp);
		GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressClamp);
	}
	for(S32 i=2; i<6; i++)
	{
		GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);
	}
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mOnePassSB);

	//mTwoPassSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->endStateBlock(mTwoPassSB);

	//mFFBaseLayerSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPModulate);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mFFBaseLayerSB);

	//mFFAdditionalLayersSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->endStateBlock(mFFAdditionalLayersSB);

	//mFFLightmapSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendDestColor);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendSrcColor);
	GFX->endStateBlock(mFFLightmapSB);

	//mFinishSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mFinishSB);
}


void ClipMapBlenderCache::init()
{
	if (mOnePassSB == NULL)
	{
		mOnePassSB = new GFXD3D9StateBlock;
		mOnePassSB->registerResourceWithDevice(GFX);
		mOnePassSB->mZombify = &releaseStateBlock;
		mOnePassSB->mResurrect = &resetStateBlock;

		mTwoPassSB = new GFXD3D9StateBlock;
		mBeginSB = new GFXD3D9StateBlock;
		mBeginFixSB = new GFXD3D9StateBlock;
		mFFBaseLayerSB = new GFXD3D9StateBlock;
		mFFAdditionalLayersSB = new GFXD3D9StateBlock;
		mFFLightmapSB = new GFXD3D9StateBlock;
		mFinishSB = new GFXD3D9StateBlock;

		resetStateBlock();
	}
}

void ClipMapBlenderCache::shutdown()
{
	SAFE_DELETE(mBeginSB);
	SAFE_DELETE(mBeginFixSB);
	SAFE_DELETE(mOnePassSB);
	SAFE_DELETE(mTwoPassSB);
	SAFE_DELETE(mFFBaseLayerSB);
	SAFE_DELETE(mFFAdditionalLayersSB);
	SAFE_DELETE(mFFLightmapSB);
	SAFE_DELETE(mFinishSB);
}

