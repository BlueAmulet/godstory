//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "clipmap/clipMap.h"
#include "clipmap/clipMapCache.h"
#include "materials/shaderData.h"
#include "console/consoleTypes.h"
#include "terrain/terrRender.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"

#define CLIPMAPSIZE1 1
#define CLIPMAPSIZE2 2048
#define CLIPMAPSIZE3 2048
#ifdef NTJ_CLIENT
#define CLIPMAPSIZE4 1
#else
#define CLIPMAPSIZE4 2048
#endif

//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* ClipMap::mFFSetSB = NULL;
GFXStateBlock* ClipMap::mFFClampSB = NULL;
GFXStateBlock* ClipMap::mFFLodBiasSB = NULL;

GFX_ImplementTextureProfile(ClipMapTextureProfile, 
                            GFXTextureProfile::DiffuseMap, 
                            GFXTextureProfile::Static | GFXTextureProfile::PreserveSize,
                            GFXTextureProfile::None);

GFX_ImplementTextureProfile(ClipMapTextureRTProfile, 
                            GFXTextureProfile::DiffuseMap, 
                            GFXTextureProfile::Static | GFXTextureProfile::PreserveSize 
                                                      | GFXTextureProfile::RenderTarget,
                            GFXTextureProfile::None);

bool ClipMap::smDebugTextures = false;
bool ClipMap::smSingleTexture = false;

ClipMap::ClipMap()
{
   // Start with some modest requirements.
   mClipMapSize = 512;

   mMaxTexelUploadPerRecenter = mClipMapSize * mClipMapSize * 2;

   // Debug builds run slower so scale this down a bit - maybe to 50%
#ifdef POWER_DEBUG
   mMaxTexelUploadPerRecenter /= 2;
#endif

   mClipStackDepth = 0;

   // Use an 8k texture by default.
   mTextureSize = BIT(13);

   mImageCache = NULL;

   mTile = false;

   mTotalUpdates = mTexelsUpdated = 0;

   // Set shaders to NULL initially.
   for(S32 i=0; i<4; i++)
      mShaders[i] = 0;

   mTexCallbackHandle = -1;

   // Register our debug texture variable with the console.
   Con::addVariable("ClipMap::useDebugTextures", TypeBool, &smDebugTextures);
   Con::addVariable("ClipMap::useSingleTexture", TypeBool, &smSingleTexture);
}

ClipMap::~ClipMap()
{
   SAFE_DELETE(mImageCache);

   while(mLevels.size())
   {
      mLevels.last().mDebugTex = NULL;
      mLevels.last().mTex      = NULL;
      mLevels.pop_back();
   }

   if(mTexCallbackHandle != -1)
      GFX->unregisterTexCallback(mTexCallbackHandle);
}

void ClipMap::texCB( GFXTexCallbackCode code, void *userData )
{
   ClipMap *acm = (ClipMap*)userData;
   AssertFatal(acm, "ClipMap::texCB - got null userData");

   if( code == GFXZombify )
   {
      // Deallocate our textures.
      for(S32 i=0; i<acm->mLevels.size(); i++)
         acm->mLevels[i].mTex = NULL;
      return;
   }

   if( code == GFXResurrect )
   {
	   // Refill our levels.
	   U32 time = Platform::getRealMilliseconds();

	   // Reallocate our textures.
	   GFXTextureProfile *texProfile = acm->mImageCache->isRenderToTargetCache() ? 
		   &ClipMapTextureRTProfile : &ClipMapTextureProfile;
	   for(S32 i=0; i<acm->mLevels.size(); i++)
	   {
		   if(i == 0)
		   {
			   acm->mLevels[i].mTex.set(CLIPMAPSIZE1, CLIPMAPSIZE1, 
				   GFXFormatR8G8B8X8, texProfile, 0);
		   }
		   else if(i == 1)
		   {
			   acm->mLevels[i].mTex.set(CLIPMAPSIZE2, CLIPMAPSIZE2, 
				   GFXFormatR8G8B8X8, texProfile, 0);
		   }
		   if (i == 2)
		   {
			   acm->mLevels[i].mTex.set(CLIPMAPSIZE3, CLIPMAPSIZE3, 
				   GFXFormatR8G8B8X8, texProfile, 0);
		   }
		   else if(i == 3)
		   {
			   acm->mLevels[i].mTex.set(CLIPMAPSIZE4, CLIPMAPSIZE4, 
				   GFXFormatR8G8B8X8, texProfile, 0);
		   }
	   }

      acm->fillWithTextureData();
      U32 postTime = Platform::getRealMilliseconds();
      Con::printf("ClipMap::texCB - took %d ms to fill clipmap after zombification.", postTime - time);

      return;
   }
}

void ClipMap::initClipStack()
{
   PROFILE_SCOPE(ClipMap_initClipStack);

   // Clear out all the levels.
   while(mLevels.size())
   {
      mLevels.last().mDebugTex = NULL;
      mLevels.last().mTex = NULL;
      mLevels.pop_back();
   }

   // What texture profile are we going to be using?
   AssertFatal(mImageCache, "ClipMap::initClipStack - must have image cache by this point.");
   GFXTextureProfile *texProfile = mImageCache->isRenderToTargetCache() 
                                    ? &ClipMapTextureRTProfile : &ClipMapTextureProfile;

   // Figure out how many clipstack textures we'll have.
   mClipStackDepth = getBinLog2(mTextureSize) - getBinLog2(mClipMapSize) + 1;

   mLevels.setSize(mClipStackDepth);

   // Print a little report on our allocation.
   Con::printf("Allocating a %d px clipmap for a %dpx source texture.", mClipMapSize, mTextureSize);
   Con::printf("   - %d base clipstack entries, + 1 cap.", mClipStackDepth - 1);

   U32 baseTexSize = (mClipMapSize * mClipMapSize * 4);
   Con::printf("   - Using approximately %fMB of texture memory.", 
      (F32(baseTexSize * mClipStackDepth) * 1.33) / (1024.0*1024.0));

   // First do our base textures - they are not mipped.
   for(S32 i=0; i<mClipStackDepth; i++)
   {

	   if(i == 0)
	   {
		   mLevels[i].mScale = (F32)BIT(mClipStackDepth - (1 + i));
		   mLevels[i].mTex.set(CLIPMAPSIZE1, CLIPMAPSIZE1, GFXFormatR8G8B8X8, texProfile, 0);
	   }
	   else if (i == 1)
	   {
		   mLevels[i].mScale = (F32)BIT(mClipStackDepth - (1 + i));
		   mLevels[i].mTex.set(CLIPMAPSIZE2, CLIPMAPSIZE2, GFXFormatR8G8B8X8, texProfile, 0);
	   }
	   if (i == 2)
	   {
		   mLevels[i].mScale = (F32)BIT(mClipStackDepth - (1 + i));
		   mLevels[i].mTex.set(CLIPMAPSIZE3, CLIPMAPSIZE3, GFXFormatR8G8B8X8, texProfile, 0);
	   }
	   else if(i == 3)
	   {
		   mLevels[i].mScale = (F32)BIT(mClipStackDepth - (1 + i));
		   mLevels[i].mTex.set(CLIPMAPSIZE4, CLIPMAPSIZE4, GFXFormatR8G8B8X8, texProfile, 0);
	   }
   }

   // Some stuff can get skipped once we're set up.
   if(mTexCallbackHandle != -1)
	   return;

   // Don't forget to allocate our debug textures...
   for(S32 i=0; i<mClipStackDepth; i++)
	   mLevels[i].initDebugTexture(i);

   // Do shader lookup for 2,3,4 level shaders.
   for(S32 i=2; i<5; i++)
   {
      ShaderData *shader = NULL;
      const char *tmpBuff = avar("AtlasShader%d", i);
      if(!Sim::findObject(StringTable->insert(tmpBuff), shader) || !shader->getShader())
      {
         Con::errorf("ClipMap::initClipStack - could not find shader '%s'!",tmpBuff);
         continue;
      }
      
      mShaders[i-1] = shader->getShader();
   }

   // Grab a callback from the texture manager to deal with zombification.
   GFX->registerTexCallback(texCB, this, mTexCallbackHandle);
}

U32 ClipMap::getMipLevel( F32 scale )
{
	return getBinLog2( scale ) + getBinLog2( mClipMapSize );
}

void ClipMap::calculateModuloDeltaBounds(const RectI &oldData, const RectI &newData, 
                                              RectI *outRects, S32 *outRectCount, S32 iii)
{
   // Sanity checking.
   /*AssertFatal(oldData.point.x >= 0 && oldData.point.y >= 0 && oldData.isValidRect(),
      "ClipMap::calculateModuloDeltaBounds - negative oldData origin or bad rect!");

   AssertFatal(newData.point.x >= 0 && newData.point.y >= 0 && newData.isValidRect(),
      "ClipMap::calculateModuloDeltaBounds - negative newData origin or bad rect!");*/

   AssertFatal(newData.extent == oldData.extent, 
      "ClipMap::calculateModuloDeltaBounts - mismatching extents, can only work with matching extents!");

   // Easiest case - if they're the same then do nothing.
   if(oldData.point == newData.point)
   {
      *outRectCount = 0;
      return;
   }

   // Easy case - if there's no overlap then it's all new!
   if(!oldData.overlaps(newData))
   {
	   // Clip out to return buffer, and we're done.

	   if(iii == 0)
	   {
		   clipAgainstGrid(CLIPMAPSIZE1, newData, outRectCount, outRects);
	   }
	   else if (iii == 1)
	   {
		   clipAgainstGrid(CLIPMAPSIZE2, newData, outRectCount, outRects);
	   }
	   else if (iii == 2)
	   {
		   clipAgainstGrid(CLIPMAPSIZE3, newData, outRectCount, outRects);
	   }
	   else
	   {
		   clipAgainstGrid(CLIPMAPSIZE4, newData, outRectCount, outRects);
	   }
      return;
   }

   // Calculate some useful values for both X and Y. Delta is used a lot
   // in determining bounds, and the boundary values are important for
   // determining where to start copying new data in.
   const S32 xDelta = newData.point.x - oldData.point.x;
   const S32 yDelta = newData.point.y - oldData.point.y;

   S32 xBoundary;
   S32 yBoundary;

   if(iii==0)
   {
	   xBoundary = (oldData.point.x + oldData.extent.x) % CLIPMAPSIZE1;
	   yBoundary = (oldData.point.y + oldData.extent.y) % CLIPMAPSIZE1;
   }
   else if (iii == 1)
   {
	   xBoundary = (oldData.point.x + oldData.extent.x) % CLIPMAPSIZE2;
	   yBoundary = (oldData.point.y + oldData.extent.y) % CLIPMAPSIZE2;
   }
   else if (iii == 2)
   {
	   xBoundary = (oldData.point.x + oldData.extent.x) % CLIPMAPSIZE3;
	   yBoundary = (oldData.point.y + oldData.extent.y) % CLIPMAPSIZE3;
   }
   else
   {
	   xBoundary = (oldData.point.x + oldData.extent.x) % CLIPMAPSIZE4;
	   yBoundary = (oldData.point.y + oldData.extent.y) % CLIPMAPSIZE4;
   }


   //AssertFatal(xBoundary % mClipMapSize == oldData.point.x % mClipMapSize, 
   //   "ClipMap::calculateModuleDeltaBounds - we assume that left and "
   //   "right of the dataset are identical (ie, it's periodical on size of clipmap!) (x)");

   //AssertFatal(yBoundary % mClipMapSize == oldData.point.y % mClipMapSize, 
   //   "ClipMap::calculateModuleDeltaBounds - we assume that left and "
   //   "right of the dataset are identical (ie, it's periodical on size of clipmap!) (y)");

   // Now, let's build up our rects. We have one rect if we are moving
   // on the X or Y axis, two if both. We dealt with the no-move case
   // previously.
   if(xDelta == 0)
   {
      // Moving on Y! So generate and store clipped results.
      RectI yRect;

	  if(yDelta < 0)
	  {
		  // We need to generate the box from right of old to right of new.
		  yRect.point = newData.point;

		  if(iii == 0)
			  yRect.extent.x = CLIPMAPSIZE1;
		  else if (iii == 1)
			  yRect.extent.x = CLIPMAPSIZE2;

		  else if (iii == 2)
			  yRect.extent.x = CLIPMAPSIZE3;
		  else
			  yRect.extent.x = CLIPMAPSIZE4;

		  yRect.extent.y = -yDelta;
	  }
	  else
	  {
		  // We need to generate the box from left of old to left of new.
		  yRect.point.x = newData.point.x; // Doesn't matter which rect we get this from.
		  yRect.point.y = (oldData.point.y + oldData.extent.y);

		  if(iii == 0)
			  yRect.extent.x = CLIPMAPSIZE1;
		  else if (iii == 1)
			  yRect.extent.x = CLIPMAPSIZE2;

		  else if (iii == 2)
			  yRect.extent.x = CLIPMAPSIZE3;
		  else
			  yRect.extent.x = CLIPMAPSIZE4;

		  yRect.extent.y = yDelta;
	  }

	  // Clip out to return buffer, and we're done.

	  if(iii == 0)
	  {
		  clipAgainstGrid(CLIPMAPSIZE1, yRect, outRectCount, outRects);
	  }
	  else if (iii == 1)
	  {
		  clipAgainstGrid(CLIPMAPSIZE2, yRect, outRectCount, outRects);
	  }
	  else if (iii == 2)
	  {
		  clipAgainstGrid(CLIPMAPSIZE3, yRect, outRectCount, outRects);
	  }
	  else
	  {
		  clipAgainstGrid(CLIPMAPSIZE4, yRect, outRectCount, outRects);
	  }


      return;
   }
   else if(yDelta == 0)
   {
      // Moving on X! So generate and store clipped results.
      RectI xRect;

	  if(xDelta < 0)
	  {
		  // We need to generate the box from right of old to right of new.
		  xRect.point = newData.point;
		  xRect.extent.x = -xDelta;

		  if(iii == 0)
			  xRect.extent.y = CLIPMAPSIZE1;
		  else if (iii == 1)
			  xRect.extent.y = CLIPMAPSIZE2;

		  else if (iii == 2)
			  xRect.extent.y = CLIPMAPSIZE3;
		  else
			  xRect.extent.y = CLIPMAPSIZE4;

	  }
	  else
	  {
		  // We need to generate the box from left of old to left of new.
		  xRect.point.x = (oldData.point.x + oldData.extent.x);
		  xRect.point.y = newData.point.y; // Doesn't matter which rect we get this from.
		  xRect.extent.x = xDelta;

		  if(iii == 0)
			  xRect.extent.y = CLIPMAPSIZE1;
		  else if (iii == 1)
			  xRect.extent.y = CLIPMAPSIZE2;

		  else if (iii == 2)
			  xRect.extent.y = CLIPMAPSIZE3;
		  else
			  xRect.extent.y = CLIPMAPSIZE4;

	  }

	  // Clip out to return buffer, and we're done.

	  if(iii == 0)
	  {
		  clipAgainstGrid(CLIPMAPSIZE1, xRect, outRectCount, outRects);
	  }
	  else if (iii == 1)
	  {
		  clipAgainstGrid(CLIPMAPSIZE2, xRect, outRectCount, outRects);
	  }

	  else if (iii == 2)
	  {
		  clipAgainstGrid(CLIPMAPSIZE3, xRect, outRectCount, outRects);
	  }
	  else
	  {
		  clipAgainstGrid(CLIPMAPSIZE4, xRect, outRectCount, outRects);
	  }
	  return;
   }
   else
   {
      // Both! We have an L shape. So let's do the bulk of it in one rect,
      // and the remainder in the other. We'll choose X as the dominant axis.
      //
      // a-----b---------c   going from e to a.
      // |     |         |
      // |     |         |
      // d-----e---------f   So the dominant rect is abgh and the passive
      // |     |         |   rect is bcef. Obviously depending on delta we
      // |     |         |   have to switch things around a bit.
      // |     |         |          y+ ^
      // |     |         |             |  
      // g-----h---------i   x+->      |

	   RectI xRect, yRect;

	   if(xDelta < 0)
	   {
		   // Case in the diagram.
		   xRect.point = newData.point;
		   xRect.extent.x = -xDelta;
		   if(iii==0)
		   {
			   xRect.extent.y = CLIPMAPSIZE1;
			   yRect.extent.x = CLIPMAPSIZE1 - mAbs(xDelta);
		   }
		   else if (iii == 1)
		   {
			   xRect.extent.y = CLIPMAPSIZE2;
			   yRect.extent.x = CLIPMAPSIZE2 - mAbs(xDelta);
		   }
		   else if (iii == 2)
		   {
			   xRect.extent.y = CLIPMAPSIZE3;
			   yRect.extent.x = CLIPMAPSIZE3 - mAbs(xDelta);
		   }
		   else
		   {
			   xRect.extent.y = CLIPMAPSIZE4;
			   yRect.extent.x = CLIPMAPSIZE4 - mAbs(xDelta); 
		   }


		   // Set up what of yRect we know, too.
		   yRect.point.x = xRect.point.x + xRect.extent.x;

      }
	   else
	   {
		   // Opposite of case in diagram!
		   xRect.point.x = oldData.point.x + oldData.extent.x;
		   xRect.point.y = newData.point.y;
		   xRect.extent.x = xDelta;
		   if(iii == 0)
		   {
			   xRect.extent.y = CLIPMAPSIZE1;
			   yRect.extent.x = CLIPMAPSIZE1 - xRect.extent.x;
			   yRect.point.x = (xRect.point.x + xRect.extent.x )- CLIPMAPSIZE1;
		   }
		   else if (iii == 1)
		   {
			   xRect.extent.y = CLIPMAPSIZE2;
			   yRect.extent.x = CLIPMAPSIZE2 - xRect.extent.x;
			   yRect.point.x = (xRect.point.x + xRect.extent.x )- CLIPMAPSIZE2;
		   }

		   else if (iii == 2)
		   {
			   xRect.extent.y = CLIPMAPSIZE3;
			   yRect.extent.x = CLIPMAPSIZE3 - xRect.extent.x;
			   yRect.point.x = (xRect.point.x + xRect.extent.x )- CLIPMAPSIZE3;
		   }
		   else
		   {
			   xRect.extent.y = CLIPMAPSIZE4;
			   yRect.extent.x = CLIPMAPSIZE4 - xRect.extent.x;
			   yRect.point.x = (xRect.point.x + xRect.extent.x )- CLIPMAPSIZE4;
		   }

		   // Set up what of yRect we know,  too.
		  
	   }

      if(yDelta < 0)
      {
         // Case in the diagram.
         yRect.point.y = newData.point.y;
         yRect.extent.y = -yDelta;
      }
      else
      {
         // Opposite of case in diagram!
         yRect.point.y = oldData.point.y + oldData.extent.y;
         yRect.extent.y = yDelta;
      }

      // Make sure we don't overlap.
      AssertFatal(!yRect.overlaps(xRect), 
         "ClipMap::calculateModuloDeltaBounds - have overlap in result rects!");

	  // Ok, now run them through the clipper.
	  S32 firstCount;
	  if(iii == 0)
	  {
		  clipAgainstGrid(CLIPMAPSIZE1, xRect, &firstCount, outRects);
		  clipAgainstGrid(CLIPMAPSIZE1, yRect, outRectCount, outRects + firstCount);
	  }
	  else if (iii == 1)
	  {
		  clipAgainstGrid(CLIPMAPSIZE2, xRect, &firstCount, outRects);
		  clipAgainstGrid(CLIPMAPSIZE2, yRect, outRectCount, outRects + firstCount);
	  }

	  else if (iii == 2)
	  {
		  clipAgainstGrid(CLIPMAPSIZE3, xRect, &firstCount, outRects);
		  clipAgainstGrid(CLIPMAPSIZE3, yRect, outRectCount, outRects + firstCount);
	  }
	  else
	  {
		  clipAgainstGrid(CLIPMAPSIZE4, xRect, &firstCount, outRects);
		  clipAgainstGrid(CLIPMAPSIZE4, yRect, outRectCount, outRects + firstCount);
	  }

	  *outRectCount += firstCount;

      // All done!
      return;
   }
}

void ClipMap::calculateClipMapLevels(const F32 near, const F32 far, 
                                          const RectF &texBounds, 
                                          S32 &outStartLevel, S32 &outEndLevel)
{
   PROFILE_SCOPE(ClipMap_calculateClipMapLevels);

   // We also have to deal w/ the available data. So let's figure out if our
   // desired TCs are in the loaded textureset.

   // Adjust the specified TC range into a texel range.
   F32 ftexsize = F32(mTextureSize);
   RectF tcR(Point2F(texBounds.point.y * ftexsize, texBounds.point.x * ftexsize), ftexsize * texBounds.extent);

   // If we're tiling, make sure we're only insetting away from the clipmap bounds.
   // This avoids making bad LOD selections at clipmap boundaries.
   // Note: compress several compares into one since a*b=0 iff a==0 or b==0

   bool doInset = true;//mTile || (tcR.point.x * tcR.point.y * (tcR.extent.x+tcR.point.x-mTextureSize) * (tcR.extent.y+tcR.point.y-mTextureSize) != 0); 

   if(doInset)
      tcR.inset(-1, -1);

   // Put some safe defaults in for starters.
   outEndLevel   = mClipStackDepth-1;
   outStartLevel = getMax(outEndLevel-3, S32(0));

   // Now iterate over every clipstack entry and find the smallest that contains
   // the relevant TCs.
   S32 minLevelOverlap = mClipStackDepth - 1;
   S32 maxLevelOverlap = mClipStackDepth - 1;

   for(S32 i=mClipStackDepth-2; i>=0; i--)
   {
	   // Find region for entry at this level.
	   RectF r;
	   F32 biti = F32(BIT(i));
	   F32 biticms;
	   if(i == 0)
		   biticms = F32(BIT(i) * CLIPMAPSIZE1);
	   else if (i == 1)
	   {
		   biticms = F32(BIT(i) * CLIPMAPSIZE2);
	   }
	   if (i == 2)
	   {
		   biticms = F32(BIT(i) * CLIPMAPSIZE3);
	   }
	   else if(i == 3)
	   {
		   biticms = F32(BIT(i) * CLIPMAPSIZE4);
	   }

      r.point = Point2F(
         biti * mLevels[i].mToroidalOffset.x, 
         biti * mLevels[i].mToroidalOffset.y);
      r.extent.set(biticms,biticms);

      // Is our tex region fully contained?
      if(r.contains(tcR))
      {
         // If we're fully contained, then this is our new max.
         maxLevelOverlap = i;
         minLevelOverlap = i;
         continue;
      }

      // Or else maybe we've got overlap?
      if (!r.overlaps(tcR))
         break;

      // If we're overlapping then this is our new min...
      minLevelOverlap = getMin(minLevelOverlap, i);
   }

   // Given our level range, do a best fit. We ALWAYS have to have
   // enough for the minimum detail, so we fit that constraint then
   // do our best to give additional detail on top of that.

   // bias the minimum detail to allow smooth transitions to work properly,
   // this avoids a LOT of texture popping.
   if (!smSingleTexture)
      maxLevelOverlap++;

   outEndLevel   = mClamp(maxLevelOverlap, 0, mClipStackDepth-1);
   outStartLevel = mClamp(minLevelOverlap, outEndLevel - 3, outEndLevel - 1);

   // support the smSingleTexture option which shows only the lowest detail
   // (can't show the highest detail because the border pixels are invalid)
   if (smSingleTexture)
      outStartLevel = outEndLevel;

   // Make sure we're not exceeding our max delta.
   const S32 delta = outEndLevel - outStartLevel;
   AssertFatal(delta >= 1 && delta <= 4, 
      "ClipMap::calculateClipMapLevels - range in levels outside of 2..4 range!");
}

void ClipMap::fillWithTestPattern()
{
   AssertISV(false, "ClipMap::fillWithTestPattern - assumes bitmaps, which "
      "are no longer present, switch to lock() semantics if you need this!");

   // Table of random colors.
   U8 colorTbl[16][3] = 
   {
      { 0xFF, 0x00, 0x0F },
      { 0xFF, 0x00, 0xA0 },
      { 0xFF, 0x00, 0xFF },
      { 0x00, 0xA0, 0x00 },
      { 0x00, 0xA0, 0xAF },
      { 0x00, 0xA0, 0xF0 },
      { 0xA0, 0xFF, 0xA0 },
      { 0x00, 0xF0, 0xA0 },
      { 0x00, 0xF0, 0xFF },
      { 0xA0, 0x00, 0x00 },
      { 0xA0, 0x00, 0xAF },
      { 0xA0, 0x00, 0xF0 },
      { 0xA0, 0xF0, 0x0F },
      { 0xA0, 0xF0, 0xA0 },
      { 0xA0, 0xF0, 0xFF },
      { 0x00, 0xFF, 0x00 },
   };

   // Lock each layer of each texture and write a test pattern in.

   // Base levels first.
   U32 temp_ClipStackDepth;

   temp_ClipStackDepth = mClipStackDepth;

   for(S32 i=0; i<temp_ClipStackDepth; i++)
   {
      GFXTexHandle &gth = mLevels[i].mTex;
      U8 *bmpData = (U8*)gth.getBitmap()->getWritableBits(0);

      for(S32 x=0; x<gth.getHeight(); x++)
      {
         for(S32 y=0; y<gth.getWidth(); y++)
         {
            S32 xFlag = x & 4;
            S32 yFlag = y & 4;

            U32 offset = (x * gth.getWidth() + y) * 4;
            if(xFlag ^ yFlag)
            {
               // Set bright.
               bmpData[offset+0] = colorTbl[i][0];
               bmpData[offset+1] = colorTbl[i][1];
               bmpData[offset+2] = colorTbl[i][2];
               bmpData[offset+3] = 0xFF;
            }
            else
            {
               // Set dim.
               bmpData[offset+0] = colorTbl[i][0] / 3;
               bmpData[offset+1] = colorTbl[i][1] / 3;
               bmpData[offset+2] = colorTbl[i][2] / 3;
               bmpData[offset+3] = 0xFF;
            }
         }
      }

      if(i == temp_ClipStackDepth - 1)
         gth.getBitmap()->extrudeMipLevels();
      else
      {
         // Write black/translucent in the higher levels.
         GBitmap *gb = gth.getBitmap();
         for(S32 j=1; j<gb->numMipLevels; j++)
         {
            U8 *b = gb->getWritableBits(j);
            dMemset(b, 0, 4 * gb->getWidth(j) * gb->getHeight(j));
         }
      }

      gth.refresh();
   }
}

void ClipMap::clipAgainstGrid(const S32 gridSpacing, const RectI &rect, S32 *outCount, RectI *outBuffer)
{
   // Check against X grids...
   const S32 startX = rect.point.x;
   const S32 endX   = rect.point.x + rect.extent.x;

   const S32 gridMask = ~(gridSpacing-1);
   const S32 startGridX = startX & gridMask;
   const S32 endGridX   = endX   & gridMask;

   RectI buffer[2];
   S32 rectCount = 0;

   // Check X...
   if(startGridX != endGridX && endX - endGridX > 0)
   {
      // We have a clip! Split against the grid multiple and store.
      rectCount = 2;
      buffer[0].point.x  = startX;
      buffer[0].point.y  = rect.point.y;
      buffer[0].extent.x = endGridX - startX;
      buffer[0].extent.y = rect.extent.y;

      buffer[1].point.x  = endGridX;
      buffer[1].point.y  = rect.point.y;
      buffer[1].extent.x = endX - endGridX;
      buffer[1].extent.y = rect.extent.y;
   }
   else
   {
      // Copy it in.
      rectCount = 1;
      buffer[0] = rect;
   }

   // Now, check Y for the one or two rects we have from above.
   *outCount = 0;
   for(S32 i=0; i<rectCount; i++)
   {
      // Figure our extent and grid information.
      const S32 startY = buffer[i].point.y;
      const S32 endY   = buffer[i].point.y + rect.extent.y;
      const S32 startGridY = startY & gridMask;
      const S32 endGridY   = endY   & gridMask;

      if(startGridY != endGridY && endY - endGridY > 0)
      {
         // We have a clip! Split against the grid multiple and store.
         RectI *buffA = outBuffer + *outCount;
         RectI *buffB = buffA + 1;
         (*outCount) += 2;

         buffA->point.x  = buffer[i].point.x;
         buffA->point.y  = startY;
         buffA->extent.x = buffer[i].extent.x;
         buffA->extent.y = endGridY - startY;

         buffB->point.x  = buffer[i].point.x;
         buffB->point.y  = endGridY;
         buffB->extent.x = buffer[i].extent.x;
         buffB->extent.y = endY - endGridY;
      }
      else
      {
         // Copy it in.
         outBuffer[*outCount] = buffer[i];
         (*outCount)++;
      }      
   }
}

void ClipMap::recenter(Point2F center)
{
   // Hack to enable PIX profiling.
   if(Con::getBoolVariable("$forceFullClipmapPurgeEveryFrame", false))
   {
      fillWithTextureData();
      return;
   }

   PROFILE_START(ClipMap_recenter);

   // Reset our budget.
   mMaxTexelUploadPerRecenter = mClipMapSize * mClipMapSize * 2;
   mMaxTexelUploadPerRecenter /= 2;

   AssertFatal(isPow2(mClipMapSize), 
      "ClipMap::recenter - require pow2 clipmap size!");

   // Clamp the center to the unit square.

   /*
   if(!mTile)
   {
      center.x = mClampF(center.x, 0.f, 1.f);
      center.y = mClampF(center.y, 0.f, 1.f);
   }
   */

   // Ok, we're going to do toroidal updates on each entry of the clipstack
   // (except for the cap, which covers the whole texture), based on this
   // new center point.
   {
      // Calculate the new texel at most detailed level.
      Point2F texelCenterF = center * F32(mClipMapSize) * mLevels[0].mScale;
      Point2I texelCenter((S32)mFloor(texelCenterF.y), (S32)mFloor(texelCenterF.x));
      
      // Update interest region.
      mImageCache->setInterestCenter(texelCenter);
   }

   // Note how many we were at so we can cut off at the right time.
   S32 lastTexelsUpdated = mTexelsUpdated;

   // For each texture...
   for(S32 i=mClipStackDepth-2; i>=0; i--)
   {
	   ClipStackEntry &cse = mLevels[i];

	   // Calculate new center point for this texture.
	   Point2F texelCenterF;
	    S32 texelMin;
	   if(i == 0)
	   {
		   texelCenterF = center * F32(CLIPMAPSIZE1) * cse.mScale;
           texelMin = CLIPMAPSIZE1/2.0f;
	   }
	   else if(i == 1)
	   {
		   texelCenterF = center * F32(CLIPMAPSIZE2) * cse.mScale;
		   texelMin = CLIPMAPSIZE2/2.0f;
	   }
	   else if (i == 2)
	   {
		   texelCenterF = center * F32(CLIPMAPSIZE3) * cse.mScale;
		   texelMin = CLIPMAPSIZE3/2.0f;
	   }
	   else
	   {
		   texelCenterF = center * F32(CLIPMAPSIZE4) * cse.mScale;
		   texelMin = CLIPMAPSIZE4/2;
	   }
      //const S32 texelMax = S32(F32(mClipMapSize) * cse.mScale) - texelMin;

      Point2I texelTopLeft;

      //if(mTile)
      //{
         texelTopLeft.x = S32(mFloor(texelCenterF.y)) - texelMin;
         texelTopLeft.y = S32(mFloor(texelCenterF.x)) - texelMin;
      //}
      //else
      //{
      //   texelTopLeft.x = mClamp(S32(mFloor(texelCenterF.y)), texelMin, texelMax) - texelMin;
      //   texelTopLeft.y = mClamp(S32(mFloor(texelCenterF.x)), texelMin, texelMax) - texelMin;
      //}

      // Also, prevent very small updates - the RT changes are costly.
      Point2I d = cse.mToroidalOffset - texelTopLeft;
      if(mAbs(d.x) <= 2 && mAbs(d.y) <= 2)
      {
         // Update the center; otherwise we get some weird conditions around
         // edges of the clipmap space.
         cse.mClipCenter = center;
         continue;
      }

	  // This + current toroid offset tells us what regions have to be blasted.
	  RectI oldData;
	  RectI newData;
	  if(i == 0)
	  {
		  oldData = RectI(cse.mToroidalOffset,  Point2I(CLIPMAPSIZE1, CLIPMAPSIZE1));
		  newData = RectI(texelTopLeft,         Point2I(CLIPMAPSIZE1, CLIPMAPSIZE1));
	  }
	  else if (i == 1)
	  {
		  oldData = RectI(cse.mToroidalOffset,  Point2I(CLIPMAPSIZE2, CLIPMAPSIZE2));
		  newData = RectI(texelTopLeft,         Point2I(CLIPMAPSIZE2, CLIPMAPSIZE2));
	  }
	  else if (i == 2)
	  {
		  oldData = RectI(cse.mToroidalOffset,  Point2I(CLIPMAPSIZE3, CLIPMAPSIZE3));
		  newData = RectI(texelTopLeft,         Point2I(CLIPMAPSIZE3, CLIPMAPSIZE3));
	  }
	  else 
	  {
		  oldData = RectI(cse.mToroidalOffset,  Point2I(CLIPMAPSIZE4, CLIPMAPSIZE4));
		  newData = RectI(texelTopLeft,         Point2I(CLIPMAPSIZE4, CLIPMAPSIZE4));
	  }
	  // Make sure we have available data...
	  if(i == 0)
	  {
		  if(!mImageCache->isDataAvailable(getMipLevel(cse.mScale), newData))
			  continue;
	  }
	  else if (i == 1)
	  {
		  if(!mImageCache->isDataAvailable(getMipLevel(cse.mScale), newData))
			  continue;
	  }
	  else if (i == 2)
	  {
		  if(!mImageCache->isDataAvailable(getMipLevel(cse.mScale), newData))
			  continue;
	  }
	  else
	  {
		  if(!mImageCache->isDataAvailable(getMipLevel(cse.mScale), newData))
			  continue;
	  }

      // Alright, determine the set of data we actually need to upload.
      RectI buffer[8];
      S32   rectCount = 0;

      calculateModuloDeltaBounds(oldData, newData, buffer, &rectCount, i);
      AssertFatal(rectCount < 8, "ClipMap::recenter - got too many rects back!");

      cse.mClipCenter      = center;
      cse.mToroidalOffset  = texelTopLeft;

      /*if(rectCount)
         Con::printf("    issuing %d updates to clipmap level %d (offset=%dx%d)", 
                        rectCount, i, texelTopLeft.x, texelTopLeft.y); */

      if(rectCount)
      {
         GFX->pushState(); // LordHavoc: this is just to prevent clipmapping from causing inconsistent rendering
         mImageCache->beginRectUpdates(cse);
         //Con::errorf("layer %x, %d updates", &cse,  rectCount);

         // And GO!
         for(S32 j=0; j<rectCount; j++)
         {
            PROFILE_START(ClipMap_recenter_upload);

            AssertFatal(buffer[j].isValidRect(),"ClipMap::recenter - got invalid rect!");

            // Note the rect, so we can then wrap and let the image cache do its thing.
            RectI srcRegion = buffer[j];
			if(i == 0)
			{
				buffer[j].point.x = srcRegion.point.x % CLIPMAPSIZE1;
				buffer[j].point.y = srcRegion.point.y % CLIPMAPSIZE1;
			}
			else if (i == 1)
			{
				buffer[j].point.x = srcRegion.point.x % CLIPMAPSIZE2;
				buffer[j].point.y = srcRegion.point.y % CLIPMAPSIZE2;
			}
			else if (i == 2)
			{
				buffer[j].point.x = srcRegion.point.x % CLIPMAPSIZE3;
				buffer[j].point.y = srcRegion.point.y % CLIPMAPSIZE3;
			}
			else 
			{
				buffer[j].point.x = srcRegion.point.x % CLIPMAPSIZE4;
				buffer[j].point.y = srcRegion.point.y % CLIPMAPSIZE4;
			}


            AssertFatal(newData.contains(srcRegion), 
               "ClipMap::recenter - got update buffer outside of expected new data bounds.");

            mTotalUpdates++;
            mTexelsUpdated += srcRegion.extent.x  * srcRegion.extent.y;

			//Con::printf("updating (%d %d %d %d)",
			//   buffer[j].point.x, buffer[j].point.y, buffer[j].extent.x, buffer[j].extent.y);

			if(i == 0)
			{
				mImageCache->doRectUpdate(getMipLevel(cse.mScale), cse, srcRegion, buffer[j]);
			}
			else if (i == 1)
			{
				mImageCache->doRectUpdate(getMipLevel(cse.mScale), cse, srcRegion, buffer[j]);
			}
			else if (i == 2)
			{
				mImageCache->doRectUpdate(getMipLevel(cse.mScale), cse, srcRegion, buffer[j]);
			}
			else
			{
				mImageCache->doRectUpdate(getMipLevel(cse.mScale), cse, srcRegion, buffer[j]);
			}


            PROFILE_END(ClipMap_recenter_upload);
         }

         mImageCache->finishRectUpdates(cse);
         GFX->popState(); // LordHavoc: this is just to prevent clipmapping from causing inconsistent rendering
      }

      // Check if we've overrun our budget.
      if((mTexelsUpdated - lastTexelsUpdated) > mMaxTexelUploadPerRecenter)
      {
         //Con::warnf("ClipMap::recenter - exceeded budget for this frame, deferring till next frame.");
         break;
      }

   }

   PROFILE_END(ClipMap_recenter);
}

bool ClipMap::fillWithTextureData()
{
   PROFILE_SCOPE(ClipMap_fillWithTextureData);

   // Note our interest for the cache.
   {
      // Calculate the new texel at most detailed level.
      Point2F texelCenterF = mLevels[0].mClipCenter * F32(mClipMapSize) * mLevels[0].mScale;
      Point2I texelCenter((S32)mFloor(texelCenterF.y), (S32)mFloor(texelCenterF.x));

      // Update interest region.
      mImageCache->setInterestCenter(texelCenter);
   }

   // First generate our desired rects for each level.

   FrameTemp<RectI> desiredData(mClipStackDepth);
   for(S32 i=0; i<mClipStackDepth; i++)
   {
	   ClipStackEntry &cse = mLevels[i];

	   // Calculate new center point for this texture.
	   Point2F texelCenterF;
	   S32 texelMin;
	   if(i == 0)
	   {
		   texelCenterF = cse.mClipCenter * F32(CLIPMAPSIZE1) * cse.mScale;
		   texelMin= CLIPMAPSIZE1/2;
	   }
	   else if (i == 1)
	   {
		   texelCenterF = cse.mClipCenter * F32(CLIPMAPSIZE2) * cse.mScale;
		   texelMin= CLIPMAPSIZE2/2;
	   }
	   else if (i == 2)
	   {
		   texelCenterF = cse.mClipCenter * F32(CLIPMAPSIZE3) * cse.mScale;
		   texelMin= CLIPMAPSIZE3/2;
	   }
	   else
	   {
		   texelCenterF = cse.mClipCenter * F32(CLIPMAPSIZE4) * cse.mScale;
		   texelMin= CLIPMAPSIZE4/2;
	   }
	   //const S32 texelMax = S32(F32(mClipMapSize) * cse.mScale) - texelMin;

	   Point2I texelTopLeft;

	   //if(mTile)
	   //{
	   texelTopLeft.x = S32(mFloor(texelCenterF.y)) - texelMin;
	   texelTopLeft.y = S32(mFloor(texelCenterF.x)) - texelMin;
	   //}
	   //else
	   //{
	   //   texelTopLeft.x = mClamp(S32(mFloor(texelCenterF.y)), texelMin, texelMax) - texelMin;
	   //   texelTopLeft.y = mClamp(S32(mFloor(texelCenterF.x)), texelMin, texelMax) - texelMin;
	   //}
	   if(i == 0)
		   desiredData[i] = RectI(texelTopLeft, Point2I(CLIPMAPSIZE1, CLIPMAPSIZE1));
	   else if (i == 1)
	   {
		   desiredData[i] = RectI(texelTopLeft, Point2I(CLIPMAPSIZE2, CLIPMAPSIZE2));
	   }
	   else if (i == 2)
	   {
		   desiredData[i] = RectI(texelTopLeft, Point2I(CLIPMAPSIZE3, CLIPMAPSIZE3));
	   }
	   else
		   desiredData[i] = RectI(texelTopLeft, Point2I(CLIPMAPSIZE4, CLIPMAPSIZE4));
	   // Make sure we have available data...
	   if(i == 0)
	   {
		   if(!mImageCache->isDataAvailable(getMipLevel(cse.mScale), desiredData[i]))
			   return false;
	   }
	   else if (i == 1)
	   {
		   if(!mImageCache->isDataAvailable(getMipLevel(cse.mScale), desiredData[i]))
			   return false;
	   }
	   else if (i == 2)
	   {
		   if(!mImageCache->isDataAvailable(getMipLevel(cse.mScale), desiredData[i]))
			   return false;
	   }
	   else
	   {
		   if(!mImageCache->isDataAvailable(getMipLevel(cse.mScale), desiredData[i]))
			   return false;
	   }

   }

   // Upload all the textures...
   for(S32 i=0; i<mClipStackDepth; i++)
   {
      ClipStackEntry &cse = mLevels[i];

	  RectI buffer[8];
	  S32   rectCount = 0;
	  if(i == 0)
	  {
		  clipAgainstGrid(CLIPMAPSIZE1, desiredData[i], &rectCount, buffer);
	  }
	  else if (i == 1)
	  {
		  clipAgainstGrid(CLIPMAPSIZE2, desiredData[i], &rectCount, buffer);
	  }
	  else if (i == 2)
	  {
		  clipAgainstGrid(CLIPMAPSIZE3, desiredData[i], &rectCount, buffer);
	  }
	  else 
	  {
		  clipAgainstGrid(CLIPMAPSIZE4, desiredData[i], &rectCount, buffer);
	  }

	  AssertFatal(rectCount < 8, "ClipMap::fillWithTextureData - got too many rects back!");

	  if(rectCount)
	  {
		  mImageCache->beginRectUpdates(cse);

		  for(S32 j=0; j<rectCount; j++)
		  {
			  RectI srcRegion = buffer[j];
			  if(i == 0)
			  {
				  buffer[j].point.x = srcRegion.point.x % CLIPMAPSIZE1;
				  buffer[j].point.y = srcRegion.point.y % CLIPMAPSIZE1;
			  }
			  else if (i == 1)
			  {
				  buffer[j].point.x = srcRegion.point.x % CLIPMAPSIZE2;
				  buffer[j].point.y = srcRegion.point.y % CLIPMAPSIZE2;
			  }

			  else if (i == 2)
			  {
				  buffer[j].point.x = srcRegion.point.x % CLIPMAPSIZE3;
				  buffer[j].point.y = srcRegion.point.y % CLIPMAPSIZE3;
			  }
			  else
			  {
				  buffer[j].point.x = srcRegion.point.x % CLIPMAPSIZE4;
				  buffer[j].point.y = srcRegion.point.y % CLIPMAPSIZE4;
			  }
			  if(i == 0)
			  {
				  mImageCache->doRectUpdate(getMipLevel(cse.mScale), cse, srcRegion, buffer[j]);
			  }
			  else if (i == 1)
			  {
				  mImageCache->doRectUpdate(getMipLevel(cse.mScale), cse, srcRegion, buffer[j]);
			  }

			  else if (i == 2)
			  {
				  mImageCache->doRectUpdate(getMipLevel(cse.mScale), cse, srcRegion, buffer[j]);
			  }
			  else
			  {
				  mImageCache->doRectUpdate(getMipLevel(cse.mScale), cse, srcRegion, buffer[j]);
			  }

		  }

         mImageCache->finishRectUpdates(cse);
      }

      cse.mToroidalOffset  = desiredData[i].point;
   }

   // Success!
   return true;
}

void ClipMap::setCache(IClipMapImageCache *cache)
{
   mImageCache = cache;
   cache->initialize(this);
}

void ClipMap::bindShaderAndTextures( U32 level1, U32 level2, U32 level3/*=-1*/, U32 level4/*=-1*/ )
{
   // Figure out how many layers we have.
   U32 levelCount = 2;
   if(level3 != -1) levelCount++;
   if(level4 != -1) levelCount++;

   // Bind appropriate shader.
   GFX->setShader(getShader(levelCount-1));
   
   // Set up constants, bind textures.
   for(S32 i=0; i<levelCount; i++)
   {
      U32 curLevel = -1;
      switch(i)
      {
      case 0: curLevel = level1; break;
      case 1: curLevel = level2; break;
      case 2: curLevel = level3; break;
      case 3: curLevel = level4; break;
      }

      AssertFatal(curLevel != -1, "ClipMap::bindShaderAndTextures - tried to draw an unspecified layer!");
      AssertFatal(curLevel < mLevels.size(), "ClipMap::bindShaderAndTextures - out of range level specified!");

      // Grab references to relevant data.
      Point4F &pt = clipConsts[i];
      ClipMap::ClipStackEntry &cse = mLevels[curLevel];

	  // Bind debug or real textures, depending on flag.

	  if(smDebugTextures)
		  GFX->setTexture(i, cse.mDebugTex);
	  else
		  GFX->setTexture(i, cse.mTex);
#ifdef STATEBLOCK
	  GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
	  GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
	  GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);
	  GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressWrap);
	  GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressWrap);
#else
      GFX->setTextureStageAddressModeU(i, GFXAddressWrap);
      GFX->setTextureStageAddressModeV(i, GFXAddressWrap);
      GFX->setTextureStageMagFilter(i, GFXTextureFilterLinear);
      GFX->setTextureStageMinFilter(i, GFXTextureFilterLinear);
      GFX->setTextureStageMipFilter(i, GFXTextureFilterLinear);
#endif
      
      if (TerrainRender::mCurrentBlock && !TerrainRender::mCurrentBlock->isTiling())
      {
         // At scale 1.0 we can clamp the UV edges avoiding edge bleed between terrain blocks
         if (cse.mScale == 1.0f)
         {
#ifdef STATEBLOCK
				GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressClamp);
				GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressClamp);
#else
            GFX->setTextureStageAddressModeU(i, GFXAddressClamp);
            GFX->setTextureStageAddressModeV(i, GFXAddressClamp);
#endif
 
         }

         // Ideally, we want to clamp at scale 2.0 as well.
         // however, we can't clamp as the shader is scaling the UV coords
         // edge bleed is caused by mipmaps wrapping, so let's bias the mipmap selection
         // out a little bit, this is only necessary for level 2 of the clip map
         if (cse.mScale == 2.0f)
         {
#ifdef STATEBLOCK
				GFX->setSamplerState(i, GFXSAMPMipMapLODBias, -1.0f);
#else
            GFX->setTextureStageLODBias(i, -1.0f);
#endif
         }
      }


      // Note the offset and center for this level as well.
      pt.x = cse.mClipCenter.x * cse.mScale;
      pt.y = cse.mClipCenter.y * cse.mScale;
      pt.z = cse.mScale;
      pt.w = 0.f;
   }

   // Set all the constants in one go.
   GFX->setVertexShaderConstF(50, &clipConsts[0].x, levelCount);

   // Ok - all bound!
}

void ClipMap::bindTexturesFF(U32 curLevel, Point4F &pt)
{
   // Set up constants, bind textures.
   AssertFatal(curLevel != -1, "ClipMap::bindTextureFF - tried to draw an unspecified layer!");
   AssertFatal(curLevel < mLevels.size(), "ClipMap::bindTextureFF - out of range level specified!");

   // Grab references to relevant data.
   ClipMap::ClipStackEntry &cse = mLevels[curLevel];

   // Bind debug or real textures, depending on flag.
   if(smDebugTextures)
      GFX->setTexture(0, cse.mDebugTex);
   else
      GFX->setTexture(0, cse.mTex);
#ifdef STATEBLOCK
	AssertFatal(mFFSetSB, "ClipMap::bindTexturesFF -- mFFSetSB cannot be NULL.");
	mFFSetSB->apply();
#else
   GFX->setTextureStageMagFilter(0, GFXTextureFilterLinear);
   GFX->setTextureStageMinFilter(0, GFXTextureFilterLinear);
   GFX->setTextureStageMipFilter(0, GFXTextureFilterLinear);
   GFX->setTextureStageAddressModeU(0, GFXAddressWrap);
   GFX->setTextureStageAddressModeV(0, GFXAddressWrap);
#endif

   if (TerrainRender::mCurrentBlock && !TerrainRender::mCurrentBlock->isTiling())
   {
      // At scale 1.0 we can clamp the UV edges avoiding edge bleed between terrain blocks
      if (cse.mScale == 1.0f)
      {
#ifdef STATEBLOCK
			AssertFatal(mFFClampSB, "ClipMap::bindTexturesFF -- mFFClampSB cannot be NULL.");
			mFFClampSB->apply();
#else
         GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
         GFX->setTextureStageAddressModeV(0, GFXAddressClamp);
#endif
      }

      // Ideally, we want to clamp at scale 2.0 as well.
      // however, we can't clamp as the shader is scaling the UV coords
      // edge bleed is caused by mipmaps wrapping, so let's bias the mipmap selection
      // out a little bit, this is only necessary for level 2 of the clip map
      if (cse.mScale == 2.0f)
      {
#ifdef STATEBLOCK
			AssertFatal(mFFLodBiasSB, "ClipMap::bindTexturesFF -- mFFLodBiasSB cannot be NULL.");
			mFFLodBiasSB->apply();
#else
         GFX->setTextureStageLODBias(0, -1.0f);
#endif
      }
   }


   // Note the offset and center for this level as well.
   pt.x = cse.mClipCenter.x * cse.mScale;
   pt.y = cse.mClipCenter.y * cse.mScale;
   pt.z = cse.mScale;
   pt.w = 0.f;
}

void ClipMap::refreshCache()
{
   mImageCache->initialize(this);
}

//-----------------------------------------------------------------------------

void ClipMap::ClipStackEntry::initDebugTexture(U32 level)
{
   static U8 colors[10][3] = 
   {
      {0xFF, 0x00, 0x00},
      {0xFF, 0xFF, 0x00},
      {0x00, 0xFF, 0x00},
      {0x00, 0xFF, 0xFF},
      {0x00, 0x00, 0xFF},
      {0xFF, 0x00, 0xFF},
      {0xFF, 0xFF, 0xFF},
      {0xaa, 0x00, 0xaa},
      {0xaa, 0xaa, 0x00},
      {0x00, 0xaa, 0xaa},
   };

   GBitmap *debugBmp = new GBitmap(4, 4);

   ColorI debugClr(colors[level][0], colors[level][1], colors[level][2]);
   ColorI edgeClr(colors[level][2], colors[level][1], colors[level][0]);

   for(S32 i=0; i<4; i++)
   {
      for(S32 j=0; j<4; j++)
      {
         bool edge = i==0 || j==0 || i==3 || j==3;
         debugBmp->setColor(i,j, edge ? edgeClr : debugClr);
      }
   }

   mDebugTex.set(debugBmp, &GFXDefaultStaticDiffuseProfile, true);
}


void ClipMap::resetStateBlock()
{
	//mFFSetSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPMagFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPMinFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPMipFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressWrap);
	GFX->endStateBlock(mFFSetSB);

	//mFFClampSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressClamp);
	GFX->endStateBlock(mFFClampSB);

	//mFFLodBiasSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPMipMapLODBias, -1.0f);
	GFX->endStateBlock(mFFLodBiasSB);
}


void ClipMap::releaseStateBlock()
{
	if (mFFSetSB)
	{
		mFFSetSB->release();
	}

	if (mFFClampSB)
	{
		mFFClampSB->release();
	}

	if (mFFLodBiasSB)
	{
		mFFLodBiasSB->release();
	}
}

void ClipMap::init()
{
	if (mFFSetSB == NULL)
	{
		mFFSetSB = new GFXD3D9StateBlock;
		mFFSetSB->registerResourceWithDevice(GFX);
		mFFSetSB->mZombify = &releaseStateBlock;
		mFFSetSB->mResurrect = &resetStateBlock;

		mFFClampSB = new GFXD3D9StateBlock;
		mFFLodBiasSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void ClipMap::shutdown()
{
	SAFE_DELETE(mFFSetSB);
	SAFE_DELETE(mFFClampSB);
	SAFE_DELETE(mFFLodBiasSB);
}