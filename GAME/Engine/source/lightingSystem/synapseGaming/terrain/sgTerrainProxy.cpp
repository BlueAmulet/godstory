//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "lightingSystem/synapseGaming/terrain/sgTerrainProxy.h"
#include "core/bitVector.h"
#include "lightingSystem/common/sceneLighting.h"
#include "sceneGraph/shadowVolumeBSP.h"
#include "T3D/staticShape.h"
#include "T3D/tsStatic.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "sceneGraph/lightingInterfaces.h"
#include "lightingSystem/common/sceneLighting.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"
#include "lightingSystem/common/sceneLightingGlobals.h"
#include "lightingSystem/synapseGaming/sgLightMap.h"
#include "lightingSystem/synapseGaming/terrain/sgTerrainLightmap.h"
#include "terrain/terrData.h"
#include <xmmintrin.h>
#include "terrain/sun.h"

#define TERRAIN_OVERRANGE 2.0f

extern SceneLighting *gLighting;

//------------------------------------------------------------------------------
// Class SceneLighting::TerrainChunk
//------------------------------------------------------------------------------
TerrainChunk::TerrainChunk()
{
   mChunkType = PersistChunk::TerrainChunkType;
	mShine = NULL;
	bakedBmp = NULL;
}

TerrainChunk::~TerrainChunk()
{
   //if(mLightmap)
   //   delete mLightmap;

	if (bakedBmp != NULL)
	{
		delete bakedBmp;
	}
	if (mShine != NULL)
	{
		delete mShine;
	}
}

//------------------------------------------------------------------------------

bool TerrainChunk::read(Stream & stream)
{
   if(!Parent::read(stream))
      return(false);

	mShine = new GBitmap;
	if (!mShine->readPNG(stream))
	{
		return false;
	}

	bakedBmp = new GBitmap;
	return bakedBmp->readPNG(stream);
}

bool TerrainChunk::write(Stream & stream)
{
   if(!Parent::write(stream))
      return(false);

	if (!mShine || !mShine->writePNG(stream))
	{
		return false;
	}
	if (!bakedBmp)
	{
		return false;
	}

	return bakedBmp->writePNG(stream);
}

/// adds the ability to bake point lights into terrain light maps.
void TerrainProxy::sgAddUniversalPoint(sgLightInfo *light)
{
   // add the light...
   sgLights.push_back(light);
}

void TerrainProxy::sgLightUniversalPoint(LightInfo *light, TerrainBlock * terrain)
{
   // only continue on the first light...
   if(sgLights.first() != light)
      return;

   // process the lighting...
   const F32 length = (F32)(terrain->getSquareSize() * TerrainBlock::BlockSize);
   const F32 halflength = 0 - (length * 0.5);

   // texel world space size...
   const F32 offset = length / ((F32)TerrainBlock::LightmapSize);

   sgTerrainLightMap *lightmap = new sgTerrainLightMap(TerrainBlock::LightmapSize, TerrainBlock::LightmapSize, terrain);
   lightmap->sgWorldPosition = Point3F(halflength, halflength, 0);
   lightmap->sgLightMapSVector = Point3F(offset, 0, 0);
   lightmap->sgLightMapTVector = Point3F(0, offset, 0);

   for(U32 i=0; i<sgLights.size(); i++)
      lightmap->sgCalculateLighting(sgLights[i]);

   lightmap->sgMergeLighting(sgBakedLightmap);

	delete lightmap;
#ifndef NTJ_SERVER
	//更新地形的,wqg,2009-5-5
	if (terrain->bakedlightmapAndcolorScale)
	{
		for (U32 j=0; j<TerrainBlock::LightmapSize*TerrainBlock::LightmapSize; j++)
		{
			//只更新rgb值，a用于保存colorscale
			terrain->bakedlightmapAndcolorScale[j].red = sgBakedLightmap[j].red;
			terrain->bakedlightmapAndcolorScale[j].green = sgBakedLightmap[j].green;
			terrain->bakedlightmapAndcolorScale[j].blue = sgBakedLightmap[j].blue;
		}
		GFXLockedRect* lockRect = terrain->tex2.lock();
		U32 w, h;
		w = h = TerrainBlock::LightmapSize;
		U32 bytePerPix = lockRect->pitch/w;
		for (U32 i=0; i<h; i++)
		{
			for (U32 j=0; j<w; j++)
			{
				lockRect->bits[i*lockRect->pitch + j *bytePerPix + 0] = terrain->bakedlightmapAndcolorScale[i*w + j].blue	* 255;// = terr->bakedlightmapAndcolorScale[i*w + j].red		* 255;
				lockRect->bits[i*lockRect->pitch + j *bytePerPix + 1] = terrain->bakedlightmapAndcolorScale[i*w + j].green	* 255;
				lockRect->bits[i*lockRect->pitch + j *bytePerPix + 2] = terrain->bakedlightmapAndcolorScale[i*w + j].red		* 255;// = terr->bakedlightmapAndcolorScale[i*w + j].blue	* 255;
				//lockRect->bits[i*lockRect->pitch + j *bytePerPix + 3] = terrain->bakedlightmapAndcolorScale[i*w + j].alpha	* 255;
			}
		}
		terrain->tex2.unlock();
	}
#endif
}

TerrainProxy::TerrainProxy(SceneObject * obj) :
Parent(obj)
{
   mLightmap = 0;

   sgBakedLightmap = 0;
}

TerrainProxy::~TerrainProxy()
{
   delete [] mLightmap;

   if(sgBakedLightmap)
      delete[] sgBakedLightmap;
}

//-------------------------------------------------------------------------------
void TerrainProxy::init()
{
   mLightmap = new ColorF[TerrainBlock::LightmapSize * TerrainBlock::LightmapSize];
   dMemset(mLightmap, 0, TerrainBlock::LightmapSize * TerrainBlock::LightmapSize * sizeof(ColorF));
   mShadowMask.setSize(TerrainBlock::BlockSize * TerrainBlock::BlockSize);

   sgBakedLightmap = new ColorF[TerrainBlock::LightmapSize * TerrainBlock::LightmapSize];
   dMemset(sgBakedLightmap, 0, TerrainBlock::LightmapSize * TerrainBlock::LightmapSize * sizeof(ColorF));
}

/// reroutes TerrainProxy::preLight for point light and TSStatic support.
bool TerrainProxy::preLight(LightInfo * light)
{
   SG_CHECK_LIGHT(light);
   if(!bool(mObj))
      return(false);

   if((light->mType != LightInfo::Vector) &&
      (light->mType != LightInfo::SGStaticPoint) &&
      (light->mType != LightInfo::SGStaticSpot))
      return(false);

   if((light->mType == LightInfo::SGStaticPoint) || (light->mType == LightInfo::SGStaticSpot))
      sgAddUniversalPoint(static_cast<sgLightInfo*>(light));

   mShadowMask.clear();
   return(true);
}

inline ColorF getValue(S32 row, S32 column, ColorF *lmap)
{
   while(row<0)
      row += TerrainBlock::LightmapSize;
   row = row % TerrainBlock::LightmapSize;

   while(column<0)
      column += TerrainBlock::LightmapSize;
   column = column% TerrainBlock::LightmapSize;

   U32 offset = row * TerrainBlock::LightmapSize + column;

   return lmap[offset];
}

void sgProcessLightMap(TerrainBlock *terrain, ColorF *mLightmap, ColorF *sgBakedLightmap)
{
   ColorF color;
   if(terrain->lightMap) delete terrain->lightMap;
   terrain->lightMap = new GBitmap(TerrainBlock::LightmapSize,TerrainBlock::LightmapSize, 0, GFXFormatR8G8B8A8);

   // Blur...
   F32 kernel[3][3] = { 1, 2, 1, 2, 4-1, 2, 1, 2, 1};
   F32 modifier = 1;
   F32 divisor = 0;
   for( U32 i=0; i<3; i++ )
   {
      for( U32 j=0; j<3; j++ )
      {
         if( i==1 && j==1 )
            kernel[i][j] = 1 + kernel[i][j] * modifier;
         else
            kernel[i][j] = kernel[i][j] * modifier;
         divisor += kernel[i][j];
      }
   }

   for( U32 i=0; i<TerrainBlock::LightmapSize; i++ )
   {
      for( U32 j=0; j<TerrainBlock::LightmapSize; j++ )
      {
         ColorF val;
         val  = getValue( i-1, j-1, mLightmap ) * kernel[0][0];
         val += getValue( i-1, j,   mLightmap ) * kernel[0][1];
         val += getValue( i-1, j+1, mLightmap ) * kernel[0][2];
         val += getValue(   i, j-1, mLightmap ) * kernel[1][0];
         val += getValue(   i, j,   mLightmap ) * kernel[1][1];
         val += getValue(   i, j+1, mLightmap ) * kernel[1][2];
         val += getValue( i+1, j-1, mLightmap ) * kernel[2][0];
         val += getValue( i+1, j,   mLightmap ) * kernel[2][1];
         val += getValue( i+1, j+1, mLightmap ) * kernel[2][2];

         U32 edge = 0;
         if( j == 0 || j == TerrainBlock::LightmapSize-1 )
            edge++;
         if( i == 0 || i == TerrainBlock::LightmapSize-1 )
            edge++;

         if( !edge )
            val = val / divisor;
         else
            val = mLightmap[(i)*TerrainBlock::LightmapSize + (j)];

         mLightmap [ (i * TerrainBlock::LightmapSize) + j]= val;
      }
   }

   // CodeReview: I took a look at this code and it appears to be 24/32-bit safe [5/11/2007 Pat]
   U8 * lPtr = terrain->lightMap->getAddress(0,0);
   AssertFatal(terrain->lightMap->bytesPerPixel == 4, "Expecting 32 bit bitmap but found 24 bit");
   for(U32 i = 0; i < (TerrainBlock::LightmapSize * TerrainBlock::LightmapSize); i++)
   {
      color.red = mLightmap[i].red + sgBakedLightmap[i].red;
      color.green = mLightmap[i].green + sgBakedLightmap[i].green;
      color.blue = mLightmap[i].blue + sgBakedLightmap[i].blue;
      color.clamp();
      lPtr[i*4+0] = (U8)(color.red   * 255);
      lPtr[i*4+1] = (U8)(color.green * 255);
      lPtr[i*4+2] = (U8)(color.blue  * 255);
   }

   if (!terrain->isTiling())
   {
      for (S32 y = 0; y < terrain->lightMap->getHeight(); y++)
      {
         ColorI c;
		 if (terrain->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_RIGHT])
         {
            terrain->lightMap->getColor(terrain->lightMap->getWidth()-1,y,c);
            terrain->lightMap->setColor(0,y,c);
            terrain->lightMap->setColor(1,y,c);
         }
         else
         {
            terrain->lightMap->getColor(0,y,c);
            terrain->lightMap->setColor(terrain->lightMap->getWidth()-1,y,c);
            terrain->lightMap->setColor(terrain->lightMap->getWidth()-2,y,c);
         }
      }

      for (S32 x = 0; x < terrain->lightMap->getHeight(); x++)
      {
         ColorI c;
         if (terrain->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_BOTTOM])
         {
            terrain->lightMap->getColor(x,terrain->lightMap->getHeight()-1,c);
            terrain->lightMap->setColor(x,0,c);
            terrain->lightMap->setColor(x,1,c);
         }
         else
         {
            terrain->lightMap->getColor(x,0,c);
            terrain->lightMap->setColor(x,terrain->lightMap->getHeight()-1,c);
            terrain->lightMap->setColor(x,terrain->lightMap->getHeight()-2,c);
         }
      }
   }
}

void sgProcessLightMapOptimize(TerrainBlock *terrain, ColorF *mLightmap, ColorF *sgBakedLightmap)
{
	//SimTime t1 = Platform::getRealMilliseconds();
   ColorF color;
   if(terrain->lightMap) delete terrain->lightMap;
   terrain->lightMap = new GBitmap(TerrainBlock::LightmapSize,TerrainBlock::LightmapSize, 0, GFXFormatR8G8B8A8);

   // Blur...
	F32 kernel[] = { 1, 2, 1, 2, 4, 2, 1, 2, 1};
   F32 modifier = 1;
   F32 divisor = 16;//sum of kernel

	//SimTime t2 = Platform::getRealMilliseconds();
	//Con::printf("t2-t1 = %d\n", t2 - t1);
	ColorF val;
	__m128 c, kel[9], dv;
	for (U32 k=0; k<9; k++)
	{
		kel[k] = _mm_set_ps1(kernel[k]);
	}
	dv = _mm_set_ps1(divisor);
	__m128 *mlm = (__m128*)mLightmap;
	U32 index = 0;

	U32 kk = 0;
	for( U32 i=1; i<TerrainBlock::LightmapSize-1; i++ )
	{
		for( U32 j=1; j<TerrainBlock::LightmapSize-1; j++ )
		{
			index = (i-1) * TerrainBlock::LightmapSize + j-1;
			c = _mm_mul_ps(*(__m128*)&mLightmap[index], kel[0]);
			c = _mm_add_ps(c, _mm_mul_ps(*(__m128*)&mLightmap[++index], kel[1]));
			c = _mm_add_ps(c, _mm_mul_ps(*(__m128*)&mLightmap[++index], kel[2]));
			index += TerrainBlock::LightmapSize - 2;
			c = _mm_add_ps(c, _mm_mul_ps(*(__m128*)&mLightmap[index], kel[3]));
			c = _mm_add_ps(c, _mm_mul_ps(*(__m128*)&mLightmap[++index], kel[4]));
			c = _mm_add_ps(c, _mm_mul_ps(*(__m128*)&mLightmap[++index], kel[5]));
			index += TerrainBlock::LightmapSize - 2;
			c = _mm_add_ps(c, _mm_mul_ps(*(__m128*)&mLightmap[index], kel[6]));
			c = _mm_add_ps(c, _mm_mul_ps(*(__m128*)&mLightmap[++index], kel[7]));
			c = _mm_add_ps(c, _mm_mul_ps(*(__m128*)&mLightmap[++index], kel[8]));
			mlm[index - TerrainBlock::LightmapSize - 1] = _mm_div_ps(c, dv);
		}
	}


	//SimTime t3 = Platform::getRealMilliseconds();
	//Con::printf("t3-t2 = %d\n", t3 - t2);
   // CodeReview: I took a look at this code and it appears to be 24/32-bit safe [5/11/2007 Pat]
   U8 * lPtr = terrain->lightMap->getAddress(0,0);
   AssertFatal(terrain->lightMap->bytesPerPixel == 4, "Expecting 32 bit bitmap but found 24 bit");
	U32 size = TerrainBlock::LightmapSize * TerrainBlock::LightmapSize;
	//__m128* ml = (__m128*)mLightmap;
	//__m128* mb = (__m128*)sgBakedLightmap;
	//__m128 mColor;
   for(U32 i = 0; i < size; i++)
   {
      color.red = mLightmap[i].red + sgBakedLightmap[i].red;
      color.green = mLightmap[i].green + sgBakedLightmap[i].green;
      color.blue = mLightmap[i].blue + sgBakedLightmap[i].blue;
		//mColor = _mm_add_ps(ml[i], mb[i]);
      color.clamp();
      lPtr[i*4+0] = (U8)(color.red   * 255);
      lPtr[i*4+1] = (U8)(color.green * 255);
      lPtr[i*4+2] = (U8)(color.blue  * 255);
   }

	//SimTime t4 = Platform::getRealMilliseconds();
	//	Con::printf("t4-t3 = %d\n", t4 - t3);
   if (!terrain->isTiling())
   {
		U32 height = terrain->lightMap->getHeight();
      for (S32 y = 0; y < height; y++)
      {
         ColorI c;
		 if (terrain->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_RIGHT])
         {
            terrain->lightMap->getColor(terrain->lightMap->getWidth()-1,y,c);
            terrain->lightMap->setColor(0,y,c);
            terrain->lightMap->setColor(1,y,c);
         }
         else
         {
            terrain->lightMap->getColor(0,y,c);
            terrain->lightMap->setColor(terrain->lightMap->getWidth()-1,y,c);
            terrain->lightMap->setColor(terrain->lightMap->getWidth()-2,y,c);
         }
      }

      for (S32 x = 0; x < height; x++)
      {
         ColorI c;
         if (terrain->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_BOTTOM])
         {
            terrain->lightMap->getColor(x,terrain->lightMap->getHeight()-1,c);
            terrain->lightMap->setColor(x,0,c);
            terrain->lightMap->setColor(x,1,c);
         }
         else
         {
            terrain->lightMap->getColor(x,0,c);
            terrain->lightMap->setColor(x,terrain->lightMap->getHeight()-1,c);
            terrain->lightMap->setColor(x,terrain->lightMap->getHeight()-2,c);
         }
      }
   }

	//SimTime t5 = Platform::getRealMilliseconds();
	//Con::printf("t5-t4 = %d\n", t5 - t4);
	//Con::printf("t5-t1 = %d\n", t5 - t1);
}

/// reroutes TerrainProxy::postLight for point light and TSStatic support.
void TerrainProxy::postLight(bool lastLight)
{
   TerrainBlock * terrain = getObject();
   if((!terrain) || (!lastLight))
      return;

   if(!terrain->mbEnableUse)
	   return;

   // set the lightmap...
   //ColorF *sgBakedLightmap = sgBakedLightmap;
   //sgProcessLightMap(terrain, mLightmap, sgBakedLightmap);
	//sgProcessLightMapOptimize(terrain, mLightmap, sgBakedLightmap);

	{
		Sun *sun = gClientSceneGraph->getCurrentSun();
		AssertFatal(sun, "TerrainProxy::postLight -- sun cannot be NULL");
		//if (sun)
		{
			terrain->freshShadowTex();
			terrain->freshLightmapByShader(sun->mLightAmbient * sun->mTerrColorScale, sun->mLightColor * sun->mTerrColorScale, sun->mBackColor * sun->mTerrColorScale, true);
			terrain->refreshLightmap();
			terrain->buildMaterialMap();
		}
	}
	Con::printf("Terrain id %d\n", terrain->getId());
}

bool TerrainProxy::sgIsCorrectStaticObjectType(SceneObject *obj)
{
   if((dynamic_cast<TSStatic *>(obj)) != NULL)
      return true;
   if((dynamic_cast<StaticShape *>(obj)) != NULL)
      return true;
   return false;
}

/// reroutes TerrainProxy::light for point light and TSStatic support.
void TerrainProxy::light(LightInfo * light)
{
   TerrainBlock * terrain = getObject();
   if(!terrain)
      return;

   AssertFatal(((light->mType == LightInfo::Vector) ||
      (light->mType == LightInfo::SGStaticPoint) ||
      (light->mType == LightInfo::SGStaticSpot)), "wrong light type");

   //S32 time = Platform::getRealMilliseconds();


   if((light->mType == LightInfo::SGStaticPoint) || (light->mType == LightInfo::SGStaticSpot))
   {
      //do baked point light here...
      sgLightUniversalPoint(light, terrain);

      //Con::printf("    = terrain lit in %3.3f seconds (sg)", (Platform::getRealMilliseconds()-time)/1000.f);
      return;
   }

   // reset
   //mShadowVolume = new ShadowVolumeBSP;

   //if((light->mType == LightInfo::Vector) && sgLightManager::sgAllowShadows())
   //{
   //   // build interior shadow volume
   //   for(ObjectProxy ** itr = gLighting->mLitObjects.begin(); itr != gLighting->mLitObjects.end(); itr++)
   //   {
   //      ObjectProxy* objproxy = *itr;
   //      if (markObjectShadow(objproxy))
   //         objproxy->addToShadowVolume(mShadowVolume, light, SceneLighting::SHADOW_DETAIL);
   //   }

   //   // Static object support...
   //   Vector<SceneObject *>   objects;
   //   gClientContainer.findObjects(ShadowCasterObjectType, sgFindObjectsCallback, &objects);

   //   // build static shadow volume
   //   for(U32 o=0; o<objects.size(); o++)
   //   {
   //      SceneObject *obj = objects[o];
   //      if(sgIsCorrectStaticObjectType(obj))
   //      {
   //         if(sgMarkStaticShadow(this, obj, light))
   //            gLighting->addStatic(mShadowVolume, obj, light, SceneLighting::SHADOW_DETAIL);
   //      }
   //   }
   //}

   //--------------------

   lightVector(light);

   //delete mShadowVolume;

   //Con::printf("    = terrain lit in %3.3f seconds", (Platform::getRealMilliseconds()-time)/1000.f);
}

//------------------------------------------------------------------------------
S32 TerrainProxy::testSquare(const Point3F & min, const Point3F & max, S32 mask, F32 expand, const Vector<PlaneF> & clipPlanes)
{
   expand = 0;
   S32 retMask = 0;
   Point3F minPoint, maxPoint;
   for(S32 i = 0; i < clipPlanes.size(); i++)
   {
      if(mask & (1 << i))
      {
         if(clipPlanes[i].x > 0)
         {
            maxPoint.x = max.x;
            minPoint.x = min.x;
         }
         else
         {
            maxPoint.x = min.x;
            minPoint.x = max.x;
         }
         if(clipPlanes[i].y > 0)
         {
            maxPoint.y = max.y;
            minPoint.y = min.y;
         }
         else
         {
            maxPoint.y = min.y;
            minPoint.y = max.y;
         }
         if(clipPlanes[i].z > 0)
         {
            maxPoint.z = max.z;
            minPoint.z = min.z;
         }
         else
         {
            maxPoint.z = min.z;
            minPoint.z = max.z;
         }
         F32 maxDot = mDot(maxPoint, clipPlanes[i]);
         F32 minDot = mDot(minPoint, clipPlanes[i]);
         F32 planeD = clipPlanes[i].d;
         if(maxDot <= -(planeD + expand))
            return(U16(-1));
         if(minDot <= -planeD)
            retMask |= (1 << i);
      }
   }
   return(retMask);
}

bool TerrainProxy::getShadowedSquares(const Vector<PlaneF> & clipPlanes, Vector<U16> & shadowList)
{
   TerrainBlock * terrain = getObject();
   if(!terrain)
      return(false);

   SquareStackNode stack[TerrainBlock::BlockShift * 4];   

   stack[0].mLevel = TerrainBlock::BlockShift;
   stack[0].mClipFlags = 0xff;
   stack[0].mPos.set(0,0);

   U32 stackSize = 1;

   Point3F blockPos;
   terrain->getTransform().getColumn(3, &blockPos);
   S32 squareSize = terrain->getSquareSize();
   F32 floatSquareSize = (F32)squareSize;

   bool marked = false;

   // push through all the levels of the quadtree
   while(stackSize)
   {
      SquareStackNode * node = &stack[stackSize - 1];

      S32 clipFlags = node->mClipFlags;
      Point2I pos = node->mPos;
      GridSquare * sq = terrain->findSquare(node->mLevel, pos);

      Point3F minPoint, maxPoint;
      minPoint.set(squareSize * pos.x + blockPos.x,
         squareSize * pos.y + blockPos.y,
         fixedToFloat(sq->minHeight));
      maxPoint.set(minPoint.x + (squareSize << node->mLevel),
         minPoint.y + (squareSize << node->mLevel),
         fixedToFloat(sq->maxHeight));

      // test the square against the current level
      if(clipFlags)
      {
         clipFlags = testSquare(minPoint, maxPoint, clipFlags, floatSquareSize, clipPlanes);
         if(clipFlags == U16(-1))
         {
            stackSize--;
            continue;
         }
      }

      // shadowed?
      if(node->mLevel == 0)
      {
         marked = true;
         shadowList.push_back(pos.x + (pos.y << TerrainBlock::BlockShift));
         stackSize--;
         continue;
      }

      // setup the next level of squares
      U8 nextLevel = node->mLevel - 1;
      S32 squareHalfSize = 1 << nextLevel;

      for(U32 i = 0; i < 4; i++)
      {
         node[i].mLevel = nextLevel;
         node[i].mClipFlags = clipFlags;
      }

      node[3].mPos = pos;
      node[2].mPos.set(pos.x + squareHalfSize, pos.y);
      node[1].mPos.set(pos.x, pos.y + squareHalfSize);
      node[0].mPos.set(pos.x + squareHalfSize, pos.y + squareHalfSize);

      stackSize += 3;
   }

   return(marked);
}

bool TerrainProxy::markObjectShadow(ObjectProxy * proxy)
{
   if (!proxy->supportsShadowVolume())
      return false;

   // setup the clip planes
   Vector<PlaneF> clipPlanes;
   proxy->getClipPlanes(clipPlanes);

   Vector<U16> shadowList;
   if(!getShadowedSquares(clipPlanes, shadowList))
      return(false);

   // set the correct bit
   for(U32 i = 0; i < shadowList.size(); i++)
      mShadowMask.set(shadowList[i]);

   return(true);
}

//判断是否被遮挡，即该点是否是影子
//true--是影子
bool TerrainProxy::checkShadowed(const Point2F& lmPos, F32 lexelDim, const Point2F& worldOffset, const LightInfo* light, const Point3F& normal)
{
   TerrainBlock* terrain = getObject();
   AssertFatal(terrain, "We shouldn't be getting called if Terrain is invalid!");

   Point2I terrPos(lmPos.x, lmPos.y);
   terrPos.x >>= TerrainBlock::LightmapShift - TerrainBlock::BlockShift;
   terrPos.y >>= TerrainBlock::LightmapShift - TerrainBlock::BlockShift;

   // Terrain object space position
   Point2F tPos((lmPos.x) * lexelDim, (lmPos.y) * lexelDim);      
   Point3F pos(tPos.x, tPos.y, 0);   
   terrain->getHeight(tPos, &pos.z);   

   // Get world position
   Point2F wPos(worldOffset);
   wPos += tPos;
   
   Point3F raycastStart(wPos.x,wPos.y,pos.z);
   raycastStart += (normal * 0.1f);

   // set light pos for shadows...
   Point3F lightpos = light->mPos;
   if(light->mType == LightInfo::Vector)
   {
      lightpos = 1000.f * light->mDirection * -1;            
      lightpos = raycastStart + lightpos;
   }

   RayInfo info;
   //Ray: 修改这里依据模型生成更好的影子
   if(terrain->getContainer()->castShadow(raycastStart, lightpos, STATIC_COLLISION_MASK,&info, terrain->getLayerID()))
   {
      //We could add a shadowing factor so artists can control shadow darkness per interior/shape instance
      //this give a pretty nice darkness for shadow (directional light angle plays a big part for smaller shapes
      //too high of an angle and not much in the way of shadows
      return true;//0.1f;//0.35f;
   }

   //nothing hit
   return false;//1.0f;
}


//This is the original version of checkShadowed using Shadow Volumes, which doesn't work with polysoup TSStatics

/*
F32 TerrainProxy::checkShadowed(const Point2F& lmPos, F32 lexelDim, const Point2F& worldOffset, const LightInfo* light, const Point3F& normal)
{
   TerrainBlock* terrain = getObject();
   AssertFatal(terrain, "We shouldn't be getting called if Terrain is invalid!");

   Point2I terrPos(lmPos.x, lmPos.y);
   terrPos.x >>= TerrainBlock::LightmapShift - TerrainBlock::BlockShift;
   terrPos.y >>= TerrainBlock::LightmapShift - TerrainBlock::BlockShift;

   // Terrain object space position
   Point2F tPos((lmPos.x) * lexelDim, (lmPos.y) * lexelDim);      

   Point3F pos(tPos.x, tPos.y, 0);   
   terrain->getHeight(tPos, &pos.z);   
   Point3F raycastStart(pos + (normal * 0.1f));

   // set light pos for shadows...
   Point3F lightpos = light->mPos;
   if(light->mType == LightInfo::Vector)
   {
      lightpos = SG_STATIC_LIGHT_VECTOR_DIST * light->mDirection * -1;            
      lightpos = pos + lightpos;
   }
   
   RayInfo info;
   if (!terrain->castRay(raycastStart, lightpos, &info))
   {
      if (!mShadowMask.test(terrPos.x + (terrPos.y << TerrainBlock::BlockShift)))
         return 1.0f;

      F32 h0, h1, h2, h3;
      h0 = pos.z;      
      terrain->getHeight(Point2F(tPos.x + lexelDim, tPos.y), &h1);
      terrain->getHeight(Point2F(tPos.x + lexelDim, tPos.y + lexelDim), &h2);
      terrain->getHeight(Point2F(tPos.x, tPos.y + lexelDim), &h3);

      // Get world position
      Point2F wPos(worldOffset);
      wPos += tPos;

      ShadowVolumeBSP::SVPoly * poly = mShadowVolume->createPoly();
      poly->mWindingCount = 4;
      poly->mWinding[0].set(wPos.x, wPos.y, h0);
      poly->mWinding[1].set(wPos.x + lexelDim, wPos.y, h1);
      poly->mWinding[2].set(wPos.x + lexelDim, wPos.y + lexelDim, h2);
      poly->mWinding[3].set(wPos.x, wPos.y + lexelDim, h3);
      poly->mPlane.set(poly->mWinding[0], poly->mWinding[1], poly->mWinding[2]);

      F32 lexelSize = mShadowVolume->getPolySurfaceArea(poly);
      F32 intensity = mShadowVolume->getClippedSurfaceArea(poly) / lexelSize;
      return intensity;
   } else {
      return 0.0f;
   }
}
*/

void TerrainProxy::lightVector(LightInfo* light)
{
   TerrainBlock* terrain = getObject();
   if (!terrain)
      return;
#ifndef NTJ_SERVER
	if (!terrain->bakedlightmapAndcolorScale)
	{
		terrain->bakedlightmapAndcolorScale = new ColorF[TerrainBlock::LightmapSize * TerrainBlock::LightmapSize];
		dMemset(terrain->bakedlightmapAndcolorScale, 0, TerrainBlock::LightmapSize * TerrainBlock::LightmapSize * sizeof(ColorF));
	}
#endif
   Point3F lightDir = -light->mDirection;
   lightDir.normalize();

   // Get the ratio between the light map pixel and world space (used for getNormal and shadow check)   
   F32 lmTerrRatio = (F32) TerrainBlock::BlockSize / (F32) TerrainBlock::LightmapSize;
   lmTerrRatio *= terrain->getSquareSize();

   // get the world offset of the terrain (used for shadow check)
   Point3F wo3d(terrain->getTransform().getPosition());
   Point2F worldOffset(wo3d.x, wo3d.y);

   // Smoothing offsets
   Point2F left(-lmTerrRatio, 0.0f);
   Point2F right(lmTerrRatio, 0.0f);
   Point2F top(0.0f, -lmTerrRatio);
   Point2F bottom(0.0f, lmTerrRatio);   

   U32 i = 0;
   for (U32 y = 0; y < TerrainBlock::LightmapSize; y++)
   {
      for (U32 x = 0; x < TerrainBlock::LightmapSize; x++)
      {
			ColorF color = terrain->mShadowMap[y/(TerrainBlock::LightmapSize/TerrainBlock::BlockSize)*TerrainBlock::BlockSize + x/(TerrainBlock::LightmapSize/TerrainBlock::BlockSize)]/255.0;

         if (color.alpha > MIN_BRUSH_COLOR_ALPHA)//刷的颜色
         {
				mLightmap[i++] = color;
         } 
         else
         {
	         Point2F lmPosition(x,y);
	         Point2F terrPosition(x, y);
	         terrPosition *= lmTerrRatio;         
	         
	         Point3F normal(0,0,0);
	         Point3F t;
	         // Cheesy normal smoothing.  At the edge of a square block, we'll end up 
	         // sampling neighbors, but at the center, we'll sample the same block
	         // over and over. 
	         if (terrain->getNormal(terrPosition, &t))
	            normal += t;
	         normal.normalize();
	
	         F32 colorScale = mClampF(mDot(normal, lightDir), 0.0f, 1.0f);
				bool shadowed = false;
	
	         // See if we are shadowed
	         if (colorScale > 0.0f)
	         { 
					shadowed = checkShadowed(lmPosition, lmTerrRatio, worldOffset, light, normal);
					if (shadowed)
					{
						colorScale *= 0.1;
					}
	         }
#ifndef NTJ_SERVER
				//alpha变量保存该处的colorScale值,wqg,2009-5-5
				terrain->bakedlightmapAndcolorScale[i].alpha = colorScale;  
				terrain->mShine[i] = !shadowed ? 1.0f : 0.0f;
#endif
				color.green = colorScale;
				color.alpha = SUN_ALPHA;//更新alpha
				mLightmap[i] += light->mAmbient + light->mColor * colorScale;
				mLightmap[i++].alpha = SUN_ALPHA;
         }
      }
   }

   // set the proper color
   for(i = 0; i < TerrainBlock::LightmapSize * TerrainBlock::LightmapSize; i++)
   {
	   //mLightmap[i] *= inverseRatioSquared;
		//注：alpha不除
	   mLightmap[i].red /= TERRAIN_OVERRANGE;
		mLightmap[i].green /= TERRAIN_OVERRANGE;
		mLightmap[i].blue /= TERRAIN_OVERRANGE;
	   mLightmap[i].clamp();
   }

#ifndef NTJ_SERVER
	GFXLockedRect* lockRect = terrain->tex4.lock();
	U32 w, h;
	w = h = TerrainBlock::LightmapSize;
	U32 bytePerPix = lockRect->pitch/w;
	for (U32 i=0; i<h; i++)
	{
		for (U32 j=0; j<w; j++)
		{
			ColorF color = terrain->mShadowMap[i/(TerrainBlock::LightmapSize/TerrainBlock::BlockSize)*TerrainBlock::BlockSize + j/(TerrainBlock::LightmapSize/TerrainBlock::BlockSize)];
			//ColorF color = mLightmap[i*TerrainBlock::LightmapSize + w];
			if (color.alpha > MIN_BRUSH_COLOR_ALPHA)//刷颜色
			{
				lockRect->bits[i*lockRect->pitch + j *bytePerPix + 0] = color.blue	* 255;
				lockRect->bits[i*lockRect->pitch + j *bytePerPix + 1] = color.green	* 255;
				lockRect->bits[i*lockRect->pitch + j *bytePerPix + 2] = color.red		* 255;
				lockRect->bits[i*lockRect->pitch + j *bytePerPix + 3] = color.alpha	* 255;
			} 
			else
			{
				lockRect->bits[i*lockRect->pitch + j *bytePerPix + 1] = terrain->bakedlightmapAndcolorScale[i*w + j].alpha	* 255;//该像素的colorScale值
				lockRect->bits[i*lockRect->pitch + j *bytePerPix + 3] = SUN_ALPHA * 255;//color.alpha	* 255;//alpha
			}

		}
	}
	terrain->tex4.unlock();
#endif

}

//--------------------------------------------------------------------------
U32 TerrainProxy::getResourceCRC()
{
   TerrainBlock * terrain = getObject();
   if(!terrain)
      return(0);
   return(terrain->getCRC());
}

//--------------------------------------------------------------------------
bool TerrainProxy::setPersistInfo(PersistInfo::PersistChunk * info)
{
   if(!Parent::setPersistInfo(info))
      return(false);

   TerrainChunk * chunk = dynamic_cast<TerrainChunk*>(info);
   AssertFatal(chunk, "TerrainProxy::setPersistInfo: invalid info chunk!");

	TerrainBlock * terrain = getObject();
	if(!terrain)
		return(false);

	if(!terrain->mbEnableUse)
		return true;

	if (chunk->mShine == NULL)
	{
		return false;
	}
	for (U32 i=0; i<chunk->mShine->byteSize/4; i++)
	{
		terrain->mShine[i] = chunk->mShine->pBits[i*4] / 255.0;
	}

	if (chunk->bakedBmp == NULL)
	{
		return false;
	}
	if (chunk->bakedBmp->bytesPerPixel == 4)
	{
		for (U32 i=0; i<chunk->bakedBmp->byteSize/4; i++)
		{
			terrain->bakedlightmapAndcolorScale[i].red = chunk->bakedBmp->pBits[i*4 + 0] / 255.0;
			terrain->bakedlightmapAndcolorScale[i].green = chunk->bakedBmp->pBits[i*4 + 1] / 255.0;
			terrain->bakedlightmapAndcolorScale[i].blue = chunk->bakedBmp->pBits[i*4 + 2] / 255.0;
			terrain->bakedlightmapAndcolorScale[i].alpha = chunk->bakedBmp->pBits[i*4 + 3] / 255.0;
		}
	}


#ifndef NTJ_SERVER
	GFXLockedRect* lockRect = NULL;
	U32 w, h;
	w = h = TerrainBlock::LightmapSize;
	U32 bytePerPix = 0;
	//tex2
	lockRect = terrain->tex2.lock();
	bytePerPix = lockRect->pitch/w;
	for (U32 i=0; i<h; i++)
	{
		for (U32 j=0; j<w; j++)
		{
			lockRect->bits[i*lockRect->pitch + j *bytePerPix + 0] = terrain->bakedlightmapAndcolorScale[i*w + j].blue	* 255;// = terr->bakedlightmapAndcolorScale[i*w + j].red		* 255;
			lockRect->bits[i*lockRect->pitch + j *bytePerPix + 1] = terrain->bakedlightmapAndcolorScale[i*w + j].green	* 255;
			lockRect->bits[i*lockRect->pitch + j *bytePerPix + 2] = terrain->bakedlightmapAndcolorScale[i*w + j].red		* 255;// = terr->bakedlightmapAndcolorScale[i*w + j].blue	* 255;
		}
	}
	terrain->tex2.unlock();
	//tex4
	lockRect = terrain->tex4.lock();
	bytePerPix = lockRect->pitch/w;
	for (U32 i=0; i<h; i++)
	{
		for (U32 j=0; j<w; j++)
		{
			//lockRect->bits[i*lockRect->pitch + j * bytePerPix + 0] = 128;
			lockRect->bits[i*lockRect->pitch + j *bytePerPix + 1] = terrain->bakedlightmapAndcolorScale[i*w + j].alpha	* 255;
			lockRect->bits[i*lockRect->pitch + j * bytePerPix + 2] = terrain->mShine[i*w + j] * 255;//该像素是阴影还是受光
			lockRect->bits[i*lockRect->pitch + j *bytePerPix + 3] = 0;
		}
	}
	terrain->tex4.unlock();

	//LightInfo *sun = NULL;
	//LightManager* lm = gClientSceneGraph->getLightManager();
	//sun = lm->getSpecialLight(LightManager::slSunLightType);
	Sun *sun = gClientSceneGraph->getCurrentSun();
	AssertFatal(sun, "TerrainProxy::postLight -- sun cannot be NULL");
	terrain->freshShadowTex();
	terrain->freshLightmapByShader(sun->mLightAmbient * sun->mTerrColorScale, sun->mLightColor * sun->mTerrColorScale, sun->mBackColor * sun->mTerrColorScale, true);
	terrain->refreshLightmap();
#endif   
	return(true);
}

bool TerrainProxy::getPersistInfo(PersistInfo::PersistChunk * info)
{
   if(!Parent::getPersistInfo(info))
      return(false);

   TerrainChunk * chunk = dynamic_cast<TerrainChunk*>(info);
   AssertFatal(chunk, "TerrainProxy::getPersistInfo: invalid info chunk!");

   TerrainBlock * terrain = getObject();
   if(!terrain || !terrain->lightMap)
      return(false);

   if(!terrain->mbEnableUse)
   {
	   //Ray: 方便修改写堆乱数据进去
	   if (!chunk->mShine)
		   chunk->mShine = new GBitmap(TerrainBlock::LightmapSize, TerrainBlock::LightmapSize, false, GFXFormatR8G8B8A8);
	   if (!chunk->bakedBmp)
		   chunk->bakedBmp = new GBitmap(TerrainBlock::LightmapSize, TerrainBlock::LightmapSize, false, GFXFormatR8G8B8A8);
	   return true;
   }

	if (chunk->mShine)
	{
		delete chunk->mShine;
		chunk->mShine = NULL;
	}
	chunk->mShine = new GBitmap(TerrainBlock::LightmapSize, TerrainBlock::LightmapSize, false, GFXFormatR8G8B8A8);
	U8 *bits = chunk->mShine->getWritableBits();
	for (U32 i=0; i<chunk->mShine->byteSize/4; i++)
	{
		bits[i*4] = terrain->mShine[i] * 255;
	}

	if (chunk->bakedBmp)
	{
		delete chunk->bakedBmp;
		chunk->bakedBmp = NULL;
	}
	chunk->bakedBmp = new GBitmap(TerrainBlock::LightmapSize, TerrainBlock::LightmapSize, false, GFXFormatR8G8B8A8);
	bits = chunk->bakedBmp->getWritableBits();
	for (U32 i=0; i<chunk->bakedBmp->byteSize/4; i++)
	{
		bits[i*4 + 0] = terrain->bakedlightmapAndcolorScale[i].red	* 255;
		bits[i*4 + 1] = terrain->bakedlightmapAndcolorScale[i].green * 255;
		bits[i*4 + 2] = terrain->bakedlightmapAndcolorScale[i].blue	* 255;
		bits[i*4 + 3] = terrain->bakedlightmapAndcolorScale[i].alpha * 255;
	}

	return(true);
}

/// adds TSStatic objects as shadow casters.
bool TerrainProxy::sgMarkStaticShadow(void *terrainproxy,
                                      SceneObject *sceneobject, LightInfo *light)
{
   TerrainProxy *terrain = (TerrainProxy *)terrainproxy;
   if((!terrain) || (!sceneobject))
      return false;

   // create shadow volume of the bounding box of this object
   if(light->mType != LightInfo::Vector)
      return(false);

   Vector<ShadowVolumeBSP::SVPoly*> shadowvolumesurfaces;
   Vector<PlaneF> shadowplanes;

   const Box3F & objBox = sceneobject->getObjBox();
   const MatrixF & objTransform = sceneobject->getTransform();
   const VectorF & objScale = sceneobject->getScale();

   // grab the surfaces which form the shadow volume
   U32 numPlanes = 0;
   PlaneF testPlanes[3];
   U32 planeIndices[3];

   // grab the bounding planes which face the light
   U32 i;
   for(i = 0; (i < 6) && (numPlanes < 3); i++)
   {
      PlaneF plane;
      plane.x = BoxNormals[i].x;
      plane.y = BoxNormals[i].y;
      plane.z = BoxNormals[i].z;

      if(i&1)
         plane.d = (((const float*)objBox.min)[(i-1)>>1]);
      else
         plane.d = -(((const float*)objBox.max)[i>>1]);

      // project
      mTransformPlane(objTransform, objScale, plane, &testPlanes[numPlanes]);

      planeIndices[numPlanes] = i;

      if(mDot(testPlanes[numPlanes], light->mDirection) < gParellelVectorThresh)
         numPlanes++;
   }
   AssertFatal(numPlanes, "SceneLighting::InteriorProxy::preLight: no planes found");

   // project the points
   Point3F projPnts[8];
   for(i = 0; i < 8; i++)
   {  
      Point3F pnt;
      pnt.set(BoxPnts[i].x ? objBox.max.x : objBox.min.x, 
         BoxPnts[i].y ? objBox.max.y : objBox.min.y,
         BoxPnts[i].z ? objBox.max.z : objBox.min.z);

      // scale it
      pnt.convolve(objScale);
      objTransform.mulP(pnt, &projPnts[i]);
   }

   ShadowVolumeBSP *mBoxShadowBSP = new ShadowVolumeBSP;

   // insert the shadow volumes for the surfaces
   for(i = 0; i < numPlanes; i++)
   {
      ShadowVolumeBSP::SVPoly * poly = mBoxShadowBSP->createPoly();
      poly->mWindingCount = 4;

      U32 j;
      for(j = 0; j < 4; j++)
         poly->mWinding[j] = projPnts[BoxVerts[planeIndices[i]][j]];

      testPlanes[i].neg();
      poly->mPlane = testPlanes[i];

      mBoxShadowBSP->buildPolyVolume(poly, light);
      shadowvolumesurfaces.push_back(mBoxShadowBSP->copyPoly(poly));
      mBoxShadowBSP->insertPoly(poly);

      // create the opposite planes for testing against terrain
      Point3F pnts[3];
      for(j = 0; j < 3; j++)
         pnts[j] = projPnts[BoxVerts[planeIndices[i]^1][j]];
   }

   // grab the unique planes for terrain checks
   for(i = 0; i < numPlanes; i++)
   {
      U32 mask = 0;
      for(U32 j = 0; j < numPlanes; j++)
         mask |= BoxSharedEdgeMask[planeIndices[i]][planeIndices[j]];

      ShadowVolumeBSP::SVNode * traverse = mBoxShadowBSP->getShadowVolume(shadowvolumesurfaces[i]->mShadowVolume);
      while(traverse->mFront)
      {
         if(!(mask & 1))
            shadowplanes.push_back(mBoxShadowBSP->getPlane(traverse->mPlaneIndex));

         mask >>= 1;
         traverse = traverse->mFront;
      }
   }

   // there will be 2 duplicate node planes if there were only 2 planes lit
   if(numPlanes == 2)
   {
      for(S32 i = 0; i < shadowvolumesurfaces.size(); i++)
         for(U32 j = 0; j < shadowvolumesurfaces.size(); j++)
         {
            if(i == j)
               continue;

            if((mDot(shadowplanes[i], shadowplanes[j]) > gPlaneNormThresh) &&
               (mFabs(shadowplanes[i].d - shadowplanes[j].d) < gPlaneDistThresh))
            {
               shadowplanes.erase(i);
               i--;
               break;
            }
         }
   }

   // setup the clip planes: add the test and the lit planes
   Vector<PlaneF> clipPlanes;
   clipPlanes = shadowplanes;
   for(i = 0; i < shadowvolumesurfaces.size(); i++)
      clipPlanes.push_back(shadowvolumesurfaces[i]->mPlane);

   Vector<U16> shadowList;
   if(!terrain->getShadowedSquares(clipPlanes, shadowList))
   {
      delete mBoxShadowBSP;
      return(false);
   }

   // set the correct bit
   for(i = 0; i < shadowList.size(); i++)
      terrain->mShadowMask.set(shadowList[i]);

   delete mBoxShadowBSP;
   return(true);
}

void TerrainProxy::processTGELightProcessEvent(U32 curr, U32 max, LightInfo* currlight)
{ 
   Con::printf("      Lighting terrain, object %d of %d...", (curr+1), max); 
   light(currlight);
}

void TerrainProxy::processSGObjectProcessEvent(LightInfo* currLight)
{
   if (currLight->mType != LightInfo::Vector)
      light(currLight);
}
