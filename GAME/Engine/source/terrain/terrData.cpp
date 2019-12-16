//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gfx/gfxDevice.h"
#include "gfx/gfxEnums.h"
#include "gfx/primBuilder.h"
#include "gfx/gBitmap.h"
#include "terrain/terrData.h"
#include "terrain/terrRender.h"
#include "terrain/sky.h"
#include "math/mMath.h"
#include "math/mathIO.h"
#include "core/fileStream.h"
#include "core/bitStream.h"
#include "console/consoleTypes.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sceneState.h"
#include "sim/netConnection.h"
#include "renderInstance/renderInstMgr.h"
#include "lightingSystem/synapseGaming/sgSceneObjectLightingPlugin.h"


#ifdef NTJ_EDITOR
#include "gui/missionEditor/terrainEditor.h"
#include "gui/missionEditor/terraformer.h"
#endif

#ifdef NTJ_CLIENT
#include "gameplay/ClientGameplayState.h"
#endif

#pragma message(ENGINE(新增导航网格))
#include "Gameplay/ai/NaviGrid/NavigationCellMgr.h"
#include "Gameplay/ai/NaviGrid/NavigationBlock.h"

#ifdef NTJ_CLIENT
#include "Gameplay/LoadMgr/ObjectLoadManager.h"
#endif

#ifndef NTJ_SERVER
#pragma message(ENGINE(使用sse优化光度图计算))
#include <xmmintrin.h>
#endif
#include "terrain/sun.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
//set shader
GFXStateBlock* TerrainBlock::mSetShader1SB	= NULL;
GFXStateBlock* TerrainBlock::mSetShader2SB	= NULL;
GFXStateBlock* TerrainBlock::mClearSB	= NULL;

//cull mode
GFXStateBlock* TerrainBlock::mCullNoneSB	= NULL;
GFXStateBlock* TerrainBlock::mCullCWSB	= NULL;
GFXStateBlock* TerrainBlock::mCullCCWSB	= NULL;

IMPLEMENT_CO_NETOBJECT_V1(TerrainBlock);

TerrainBlock* TerrainBlock::mTerMap[TerrainBlock::TerrainSize][TerrainBlock::TerrainSize];
U32 TerrainBlock::mMaxTerrainWidth = 0;
U32 TerrainBlock::mMaxTerrainHeight = 0;
Box3F TerrainBlock::mTerrBox;
bool TerrainBlock::first = true;
#ifdef NTJ_CLIENT
bool TerrainBlock::smLightingDone = 0;
U32 TerrainBlock::smCurTerCRC = 0;
#endif




//RBP - Global function declared in Terrdata.h
TerrainBlock* getTerrainUnderWorldPoint(const Point3F & wPos)
{
	// Cast a ray straight down from the world position and see which
	// Terrain is the closest to our starting point
	Point3F startPnt = wPos;
	Point3F endPnt = wPos + Point3F(0.0f, 0.0f, -10000.0f);

	S32 blockIndex = -1;
	F32 nearT = 1.0f;

	SimpleQueryList queryList;
	gServerContainer.findObjects( TerrainObjectType, SimpleQueryList::insertionCallback, &queryList);

	for (U32 i = 0; i < queryList.mList.size(); i++)
	{
		Point3F tStartPnt, tEndPnt;
		TerrainBlock* terrBlock = dynamic_cast<TerrainBlock*>(queryList.mList[i]);
		terrBlock->getWorldTransform().mulP(startPnt, &tStartPnt);
		terrBlock->getWorldTransform().mulP(endPnt, &tEndPnt);

		RayInfo ri;
		if (terrBlock->castRayI(tStartPnt, tEndPnt, &ri, true))
		{
			if (ri.t < nearT)
			{
				blockIndex = i;
				nearT = ri.t;
			}
		}
	}

	if (blockIndex > -1)
		return (TerrainBlock*)(queryList.mList[blockIndex]);

	return NULL;
}

ConsoleFunction(getTerrainUnderWorldPoint, S32, 2, 4, "(Point3F x/y/z) Gets the terrain block that is located under the given world point.\n"
                                                      "@param x/y/z The world coordinates (floating point values) you wish to query at. " 
                                                      "These can be formatted as either a string (\"x y z\") or separately as (x, y, z)\n"
                                                      "@return Returns the ID of the requested terrain block (0 if not found).\n\n")
{
   Point3F pos;
   if(argc == 2)
      dSscanf(argv[1], "%f %f %f", &pos.x, &pos.y, &pos.z);
   else if(argc == 4)
   {
      pos.x = dAtof(argv[1]);
      pos.y = dAtof(argv[2]);
      pos.z = dAtof(argv[3]);
   }

   else
   {
      Con::errorf("getTerrainUnderWorldPoint(Point3F): Invalid argument count! Valid arguments are either \"x y z\" or x,y,z\n");
      return 0;
   }

   TerrainBlock* terrain = getTerrainUnderWorldPoint(pos);
   if(terrain != NULL)
   {
      return terrain->getId();
   }

   return 0;

}

DECLARE_SERIALIBLE( TerrainBlock );

//--------------------------------------
TerrainBlock::TerrainBlock()
{
   mbEnableUse = true;
   mSquareSize = 8;
   mDetailDistance = 25.f;
   mDetailBrightness = 1.f;
   mDetailScale = 256.0f;

   lightMap = NULL;
   mShine = NULL;
   bakedlightmapAndcolorScale = NULL;

   for(S32 i = BlockShift; i >= 0; i--)
      gridMap[i] = NULL;

   heightMap = NULL;
   materialMap = NULL;
   mBaseMaterialMap = NULL;
   mMaterialFileName = NULL;
   mTypeMask = TerrainObjectType | StaticObjectType | StaticRenderedObjectType | ShadowCasterObjectType;
   mNetFlags.set(Ghostable | ScopeAlways);
   mCollideEmpty = false;
   mDetailTextureName = NULL;
   mTile = true;

   mCRC = 0;
   flagMap = 0;
   mHasRendered = false;

   if( GFXDevice::devicePresent() )
   {
      GFX->registerTexCallback(texManagerCallback, this, mTexCallback);
   }

   mTexelsPerMeter = 32;
   mClipMapSizeLog2 = getBinLog2(2048);

   mLightmapSource = NULL;
   mClipBlender = NULL;
   mClipMap = NULL;

   for (U32 i = 0; i < 10; i++)
      mMPMIndex[i] = -1;

   mHeightIndex = 0;
   mWidthIndex = 0;
	unInit = true;
	mEdgeColor.set(1.0f, 0.0f, 1.0f, 0.5f);
	mNeedRenderSubTerrEdge = false;
}

//--------------------------------------
//extern Convex sTerrainConvexList;

TerrainBlock::~TerrainBlock()
{
   // Kill collision
   sTerrainConvexList.nukeList();

   freeRenderResource();

#ifdef NTJ_EDITOR
   TerrainEditor* editor = dynamic_cast<TerrainEditor*>(Sim::findObject("ETerrainEditor"));
   if (editor)
      editor->detachTerrain(this);
#endif
}

//--------------------------------------
void TerrainBlock::texManagerCallback(GFXTexCallbackCode code, void *userData)
{
   //TerrainBlock *localBlock = (TerrainBlock*)userData;

   if(code == GFXZombify)
   {
      // Clear the texture cache.
      //AllocatedTexture::flushCache();
      //localBlock->mBlender->zombify();
   }
   else if(code == GFXResurrect)
   {
      //localBlock->mBlender->resurrect();
		first = true;
   }
}

//--------------------------------------
void TerrainBlock::setFile(Resource<TerrainFile> terr)
{
   mFile = terr;
   for(U32 i = 0; i <= BlockShift; i++)
      gridMap[i] = mFile->mGridMap[i];

   mBaseMaterialMap = mFile->mBaseMaterialMap;
   mMaterialFileName= mFile->mMaterialFileName;
   mChunkMap = mFile->mChunkMap;
   materialMap = mFile->mMaterialMap;
   heightMap   = mFile->mHeightMap;
   flagMap = mFile->mFlagMap;
	mShadowMap = mFile->mShadowMap;
}


//--------------------------------------------------------------------------
bool TerrainBlock::save(const char *filename)
{
   return mFile->save(filename);
}


//--------------------------------------
static U16 calcDev(PlaneF &pl, Point3F &pt)
{
   F32 z = (pl.d + pl.x * pt.x + pl.y * pt.y) / -pl.z;
   F32 diff = z - pt.z;
   if(diff < 0)
      diff = -diff;

   if(diff > 0xFFFF)
      return 0xFFFF;
   else
      return U16(diff);
}

static U16 Umax(U16 u1, U16 u2)
{
   return u1 > u2 ? u1 : u2;
}

//------------------------------------------------------------------------------

bool TerrainBlock::unpackEmptySquares()
{
   U32 size = BlockSquareWidth * BlockSquareWidth;

   U32 i;
   for(i = 0; i < size; i++)
      materialMap[i].flags &= ~Material::Empty;

   // walk the pairs
   for(i = 0; i < mEmptySquareRuns.size(); i++)
   {
      U32 offset = mEmptySquareRuns[i] & 0xffff;
      U32 run = U32(mEmptySquareRuns[i]) >> 16;

      //
      for(U32 j = 0; j < run; j++)
      {
         if((offset+j) >= size)
         {
            Con::errorf(ConsoleLogEntry::General, "TerrainBlock::unpackEmpties: invalid entry.");
            return(false);
         }
         materialMap[offset+j].flags |= Material::Empty;
      }
   }

   rebuildEmptyFlags();
   return(true);
}

void TerrainBlock::packEmptySquares()
{
   AssertFatal(isServerObject(), "TerrainBlock::packEmptySquares: client!");
   mEmptySquareRuns.clear();

   // walk the map
   U32 run = 0;
   U32 offset = 0xFFFFFFFF;

   U32 size = BlockSquareWidth * BlockSquareWidth;
   for(U32 i = 0; i < size; i++)
   {
      if(materialMap[i].flags & Material::Empty)
      {
         if(!run)
            offset = i;
         run++;
      }
      else if(run)
      {
         AssertFatal(offset != 0xFFFFFFFF, "TerrainBlock::packEmptySquares");
         mEmptySquareRuns.push_back((run << 16) | offset);
         run = 0;

         // cap it
         if(mEmptySquareRuns.size() == MaxEmptyRunPairs)
            break;
      }
   }

   //
   if(run && mEmptySquareRuns.size() != MaxEmptyRunPairs)
      mEmptySquareRuns.push_back((run << 16) | offset);

   if(mEmptySquareRuns.size() == MaxEmptyRunPairs)
      Con::warnf(ConsoleLogEntry::General, "TerrainBlock::packEmptySquares: More than %d run pairs encountered.  Extras will not persist.", MaxEmptyRunPairs);

   //
   mNetFlags |= EmptyMask;
}

//------------------------------------------------------------------------------

void TerrainBlock::rebuildEmptyFlags()
{
   // rebuild entire maps empty flags!
   for(U32 y = 0; y < TerrainBlock::ChunkSquareWidth; y++)
   {
      for(U32 x = 0; x < TerrainBlock::ChunkSquareWidth; x++)
      {
         GridChunk &gc = *(mChunkMap + x + (y << TerrainBlock::ChunkShift));
         gc.emptyFlags = 0;
         U32 sx = x << TerrainBlock::ChunkDownShift;
         U32 sy = y << TerrainBlock::ChunkDownShift;
         for(U32 j = 0; j < 4; j++)
         {
            for(U32 i = 0; i < 4; i++)
            {
               TerrainBlock::Material *mat = getMaterial(sx + i, sy + j);
               if(mat->flags & TerrainBlock::Material::Empty)
                  gc.emptyFlags |= (1 << (j * 4 + i));
            }
         }
      }
   }

   for(S32 i = BlockShift; i >= 0; i--)
   {
      S32 squareCount = 1 << (BlockShift - i);
      S32 squareSize = (TerrainBlock::BlockSize) / squareCount;

      for(S32 squareX = 0; squareX < squareCount; squareX++)
      {
         for(S32 squareY = 0; squareY < squareCount; squareY++)
         {
            GridSquare *parent = NULL;
            if(i < BlockShift)
               parent = findSquare(i+1, Point2I(squareX * squareSize, squareY * squareSize));
            bool empty = true;

            for(S32 sizeX = 0; empty && sizeX <= squareSize; sizeX++)
            {
               for(S32 sizeY = 0; empty && sizeY <= squareSize; sizeY++)
               {
                  S32 x = squareX * squareSize + sizeX;
                  S32 y = squareY * squareSize + sizeY;

                  if(sizeX != squareSize && sizeY != squareSize)
                  {
                     TerrainBlock::Material *mat = getMaterial(x, y);
                     if(!(mat->flags & TerrainBlock::Material::Empty))
                        empty = false;
                  }
               }
            }
            GridSquare *sq = findSquare(i, Point2I(squareX * squareSize, squareY * squareSize));
            if(empty)
               sq->flags |= GridSquare::Empty;
            else
               sq->flags &= ~GridSquare::Empty;
         }
      }
   }
}

//------------------------------------------------------------------------------

void TerrainBlock::calcBounds()
{
   // We know our bounds x, y are going to go from 0, 0 to BlockSize * SquareSize
   // but our height will be variable according to the heightfield
   mBounds.min.set(0.0f, 0.0f, -1e10f);
   mBounds.max.set(BlockSize * (float)mSquareSize, BlockSize * (float)mSquareSize, 1e10f);

   // Little trick I picked up from the EditTSCtrl::renderMissionArea()
   // I *believe* that this works because the lowest level of detail has
   // the absolute max and min heights
   GridSquare * gs = findSquare(TerrainBlock::BlockShift, Point2I(0,0));
   mBounds.max.z = fixedToFloat(gs->maxHeight);
   mBounds.min.z = fixedToFloat(gs->minHeight);

   // If we aren't tiling go ahead and set our mObjBox to be equal to mBounds
   // This will get overridden with global bounds if tiling is on but it is useful to store
   if (!mTile)
   {
      if (mObjBox.max != mBounds.max || mObjBox.min != mBounds.min)
      {
         mObjBox = mBounds;
         resetWorldBox();
         setMaskBits(TerrainChangedMask);
      }
   }
}

void TerrainBlock::setHeight(const Point2I & pos, float height)
{
   // set the height
   U16 ht = floatToFixed(height);
   *((U16*)getHeightAddress(pos.x, pos.y)) = ht;
}

void TerrainBlock::setShadow(const Point2I & pos, ColorF shadow)
{
	mShadowMap[pos.y*BlockSize + pos.x] = shadow;
}

ColorF TerrainBlock::getShadow(const Point2I &pos)
{
	return mShadowMap[pos.y*BlockSize + pos.x];
}

inline void getMinMax(U16 &min, U16 &max, U16 height)
{
   if(height < min)
      min = height;
   if(height > max)
      max = height;
}

inline void checkSquareMinMax(GridSquare *parent, GridSquare *child)
{
   if(parent->minHeight > child->minHeight)
      parent->minHeight = child->minHeight;
   if(parent->maxHeight < child->maxHeight)
      parent->maxHeight = child->maxHeight;
}

void TerrainBlock::updateGridMaterials(Point2I min, Point2I max)
{
   // ok:
   // build the chunk materials flags for all the chunks in the bounding rect
   // ((min - 1) >> ChunkDownShift) up to ((max + ChunkWidth) >> ChunkDownShift)

   // we have to make sure to cover boundary conditions as as stated above
   // since, for example, x = 0 affects 2 chunks

   for (S32 y = getMax(min.y - 1, 0); y < max.y + 1; y++)
   {
      for (S32 x = getMax(min.x - 1, 0); x < max.x + 1; x++)
      {
         GridSquare *sq = findSquare(0, Point2I(x, y));
         sq->flags &= (GridSquare::MaterialStart -1);
         
         S32 xpl = (x + 1) & TerrainBlock::BlockMask;
         S32 ypl = (y + 1) & TerrainBlock::BlockMask;

         for(U32 i = 0; i < TerrainBlock::MaterialGroups; i++)
         {
            if (mFile->mMaterialAlphaMap[i] != NULL) 
            {
               U32 mapVal = (mFile->mMaterialAlphaMap[i][(y   << TerrainBlock::BlockShift) + x]     |
                             mFile->mMaterialAlphaMap[i][(ypl << TerrainBlock::BlockShift) + x]   |
                             mFile->mMaterialAlphaMap[i][(ypl << TerrainBlock::BlockShift) + xpl] |
                             mFile->mMaterialAlphaMap[i][(y   << TerrainBlock::BlockShift) + xpl]);
               if(mapVal)
                  sq->flags |= (GridSquare::MaterialStart << i);
            }
         }
      }
   }

   for (S32 y = min.y - 2; y < max.y + 2; y += 2)
   {
      for (S32 x= min.x - 2; x < max.x + 2; x += 2)
      {
         GridSquare *sq = findSquare(1, Point2I(x, y));
         GridSquare *s1 = findSquare(0, Point2I(x, y));
         GridSquare *s2 = findSquare(0, Point2I(x+1, y));
         GridSquare *s3 = findSquare(0, Point2I(x, y+1));
         GridSquare *s4 = findSquare(0, Point2I(x+1, y+1));
         sq->flags |= (s1->flags | s2->flags | s3->flags | s4->flags) & ~(GridSquare::MaterialStart -1);
      }
   }

   //AllocatedTexture::flushCacheRect(RectI(min, max - min));
#ifdef USE_CLIPMAP
   updateOpacity();
#else
	if (mNeedReloadTexture)
	{
		reloadTexture();
		mNeedReloadTexture = false;
	}
	//updateTexture();
	updateBlendTexture();
#endif


   // Signal anyone that cares that the opacity was changed.
   mUpdateSignal.trigger( OpacityUpdate, min, max );
}

void TerrainBlock::updateGrid(Point2I min, Point2I max)
{
   // On the client we just signal everyone that the height
   // map has changed... the server does the actual changes.
   if ( isClientObject() )
   {
      mUpdateSignal.trigger( HeightmapUpdate, min, max );
      return;
   }

   // ok:
   // build the chunk deviance for all the chunks in the bounding rect,
   // ((min - 1) >> ChunkDownShift) up to ((max + ChunkWidth) >> ChunkDownShift)

   // update the chunk deviances for the affected chunks
   // we have to make sure to cover boundary conditions as as stated above
   // since, for example, x = 0 affects 2 chunks

   for(S32 x = (min.x - 1) >> ChunkDownShift;x < (max.x + ChunkSize) >> ChunkDownShift; x++)
   {
      for(S32 y = (min.y - 1) >> ChunkDownShift;y < (max.y + ChunkSize) >> ChunkDownShift; y++)
      {
         S32 px = x;
         S32 py = y;
         if(px < 0)
            px += BlockSize >> ChunkDownShift;
         if(py < 0)
            py += BlockSize >> ChunkDownShift;

         buildChunkDeviance(px, py);
      }
   }

   // ok the chunk deviances are rebuilt... now rebuild the affected area
   // of the grid map:

   // here's how it works:
   // for the current terrain renderer we only care about
   // the minHeight and maxHeight on the GridSquare
   // so we do one pass through, updating minHeight and maxHeight
   // on the level 0 squares, then we loop up the grid map from 1 to
   // the top, expanding the bounding boxes as necessary.
   // this should end up being way, way, way, way faster for the terrain
   // editor

   for(S32 y = min.y - 1; y < max.y + 1; y++)
   {
      for(S32 x = min.x - 1; x < max.x + 1; x++)
      {
         S32 px = x;
         S32 py = y;
         if(px < 0)
            px += BlockSize;
         if(py < 0)
            py += BlockSize;

         GridSquare *sq = findSquare(0, px, py);

         sq->minHeight = 0xFFFF;
         sq->maxHeight = 0;

         getMinMax(sq->minHeight, sq->maxHeight, getHeight(x, y));
         getMinMax(sq->minHeight, sq->maxHeight, getHeight(x+1, y));
         getMinMax(sq->minHeight, sq->maxHeight, getHeight(x, y+1));
         getMinMax(sq->minHeight, sq->maxHeight, getHeight(x+1, y+1));
      }
   }

   // ok, all the level 0 grid squares are updated:
   // now update all the parent grid squares that need to be updated:

   for(S32 level = 1; level <= TerrainBlock::BlockShift; level++)
   {
      S32 size = 1 << level;
      S32 halfSize = size >> 1;
      for(S32 y = (min.y - 1) >> level; y < (max.y + size) >> level; y++)
      {
         for(S32 x = (min.x - 1) >> level; x < (max.x + size) >> level; x++)
         {
            S32 px = x << level;
            S32 py = y << level;

            GridSquare *square = findSquare(level, px, py);
            square->minHeight = 0xFFFF;
            square->maxHeight = 0;

            checkSquareMinMax(square, findSquare(level - 1, px, py));
            checkSquareMinMax(square, findSquare(level - 1, px + halfSize, py));
            checkSquareMinMax(square, findSquare(level - 1, px, py + halfSize));
            checkSquareMinMax(square, findSquare(level - 1, px + halfSize, py + halfSize));
         }
      }
   }

   calcBounds();

   // Signal again here for any server side observers.
   mUpdateSignal.trigger( HeightmapUpdate, min, max );

}


//--------------------------------------
bool TerrainBlock::getHeight(const Point2F &pos, F32 *height)
{
   float invSquareSize = 1.0f / (float)mSquareSize;
   float xp = pos.x * invSquareSize;
   float yp = pos.y * invSquareSize;
   int x = (S32)mFloor(xp);
   int y = (S32)mFloor(yp);
   xp -= (float)x;
   yp -= (float)y;
   x &= BlockMask;
   y &= BlockMask;
   GridSquare * gs = findSquare(0, Point2I(x,y));

   if (gs->flags & GridSquare::Empty)
      return false;

   float zBottomLeft = fixedToFloat(getHeight(x, y));
   float zBottomRight = fixedToFloat(getHeight(x + 1, y));
   float zTopLeft = fixedToFloat(getHeight(x, y + 1));
   float zTopRight = fixedToFloat(getHeight(x + 1, y + 1));

   if(gs->flags & GridSquare::Split45)
   {
      if (xp>yp)
         // bottom half
         *height = zBottomLeft + xp * (zBottomRight-zBottomLeft) + yp * (zTopRight-zBottomRight);
      else
         // top half
         *height = zBottomLeft + xp * (zTopRight-zTopLeft) + yp * (zTopLeft-zBottomLeft);
   }
   else
   {
      if (1.0f-xp>yp)
         // bottom half
         *height = zBottomRight + (1.0f-xp) * (zBottomLeft-zBottomRight) + yp * (zTopLeft-zBottomLeft);
      else
         // top half
         *height = zBottomRight + (1.0f-xp) * (zTopLeft-zTopRight) + yp * (zTopRight-zBottomRight);
   }
   return true;
}

bool TerrainBlock::getNormal(const Point2F & pos, Point3F * normal, bool normalize)
{
   float invSquareSize = 1.0f / (float)mSquareSize;
   float xp = pos.x * invSquareSize;
   float yp = pos.y * invSquareSize;
   int x = (S32)mFloor(xp);
   int y = (S32)mFloor(yp);
   xp -= (float)x;
   yp -= (float)y;
   x &= BlockMask;
   y &= BlockMask;
   GridSquare * gs = findSquare(0, Point2I(x,y));

   if (gs->flags & GridSquare::Empty)
      return false;

   float zBottomLeft = fixedToFloat(getHeight(x, y));
   float zBottomRight = fixedToFloat(getHeight(x + 1, y));
   float zTopLeft = fixedToFloat(getHeight(x, y + 1));
   float zTopRight = fixedToFloat(getHeight(x + 1, y + 1));

   if(gs->flags & GridSquare::Split45)
   {
      if (xp>yp)
         // bottom half
         normal->set(zBottomLeft-zBottomRight, zBottomRight-zTopRight, (F32)mSquareSize);
      else
         // top half
         normal->set(zTopLeft-zTopRight, zBottomLeft-zTopLeft, (F32)mSquareSize);
   }
   else
   {
      if (1.0f-xp>yp)
         // bottom half
         normal->set(zBottomLeft-zBottomRight, zBottomLeft-zTopLeft, (F32)mSquareSize);
      else
         // top half
         normal->set(zTopLeft-zTopRight, zBottomRight-zTopRight, (F32)mSquareSize);
   }
   if (normalize)
      normal->normalize();
   return true;
}

bool TerrainBlock::getNormalAndHeight(const Point2F & pos, Point3F * normal, F32 * height, bool normalize)
{
   float invSquareSize = 1.0f / (float)mSquareSize;
   float xp = pos.x * invSquareSize;
   float yp = pos.y * invSquareSize;
   int x = (S32)mFloor(xp);
   int y = (S32)mFloor(yp);
   xp -= (float)x;
   yp -= (float)y;

   // If we disable repeat, then skip non-primary block
   if(!mTile && (x&~BlockMask || y&~BlockMask))
      return false;

   x &= BlockMask;
   y &= BlockMask;


   GridSquare * gs = findSquare(0, Point2I(x,y));

   if (gs->flags & GridSquare::Empty)
      return false;

   float zBottomLeft  = fixedToFloat(getHeight(x, y));
   float zBottomRight = fixedToFloat(getHeight(x + 1, y));
   float zTopLeft     = fixedToFloat(getHeight(x, y + 1));
   float zTopRight    = fixedToFloat(getHeight(x + 1, y + 1));

   if(gs->flags & GridSquare::Split45)
   {
      if (xp>yp)
      {
         // bottom half
         normal->set(zBottomLeft-zBottomRight, zBottomRight-zTopRight, (F32)mSquareSize);
         *height = zBottomLeft + xp * (zBottomRight-zBottomLeft) + yp * (zTopRight-zBottomRight);
      }
      else
      {
         // top half
         normal->set(zTopLeft-zTopRight, zBottomLeft-zTopLeft, (F32)mSquareSize);
         *height = zBottomLeft + xp * (zTopRight-zTopLeft) + yp * (zTopLeft-zBottomLeft);
      }
   }
   else
   {
      if (1.0f-xp>yp)
      {
         // bottom half
         normal->set(zBottomLeft-zBottomRight, zBottomLeft-zTopLeft, (F32)mSquareSize);
         *height = zBottomRight + (1.0f-xp) * (zBottomLeft-zBottomRight) + yp * (zTopLeft-zBottomLeft);
      }
      else
      {
         // top half
         normal->set(zTopLeft-zTopRight, zBottomRight-zTopRight, (F32)mSquareSize);
         *height = zBottomRight + (1.0f-xp) * (zTopLeft-zTopRight) + yp * (zTopRight-zBottomRight);
      }
   }
   if (normalize)
      normal->normalize();
   return true;
}

//--------------------------------------


//--------------------------------------
void TerrainBlock::setBaseMaterials(S32 argc, const char *argv[])
{
   for (S32 i = 0; i < argc; i++)
      mMaterialFileName[i] = StringTable->insert(argv[i]);
   for (S32 j = argc; j < MaterialGroups; j++)
      mMaterialFileName[j] = NULL;
}

//static bool gLightmapDebug = false;

//------------------------------------------------------------------------------
void TerrainBlock::postLight(Vector<TerrainBlock *> &terrBlocks)
{
   /*
   if (!gLightmapDebug)
   {
      gLightmapDebug = true;

      GBitmap *b = new GBitmap(2048, 2048, false, GFXFormatR8G8B8);

      RectI r(0,0,1024,1024);
      b->copyRect(terrBlocks[0]->lightMap, r, Point2I(0,0));
      b->copyRect(terrBlocks[1]->lightMap, r, Point2I(1024,0));
      b->copyRect(terrBlocks[2]->lightMap, r, Point2I(0,1024));
      b->copyRect(terrBlocks[3]->lightMap, r, Point2I(1024,1024));
      b->writePNGDebug("super_lightmap.png");
   }
   */

}

//--------------------------------------

bool TerrainBlock::buildMaterialMap()
{
   PROFILE_SCOPE(TerrainBlock_BuildMaterialMap);
   // We have to make sure we have the right number of alpha maps - should
   // have one for every specified texture. So add or delete to conform.

   for(S32 i=0; i<TerrainBlock::MaterialGroups; i++)
   {
      if(mFile->mMaterialFileName[i])
      {
         if(!mFile->mMaterialAlphaMap[i])
         {
            mFile->mMaterialAlphaMap[i] = new U8[TerrainBlock::BlockSize*TerrainBlock::BlockSize];
            dMemset(mFile->mMaterialAlphaMap[i], 0, sizeof(U8)*TerrainBlock::BlockSize*TerrainBlock::BlockSize);

            addOpacitySources();

            if (mClipBlender)
               mClipBlender->createOpacityScratchTextures();
         }
      }
      else
      {
         if(mFile->mMaterialAlphaMap[i])
         {
            delete [] mFile->mMaterialAlphaMap[i];
            mFile->mMaterialAlphaMap[i] = NULL;
         }
      }
   }

	if(mClipMap)
	{
		//mLightmapSource->mTex = tex3;

		// Refill clipmap.
		mClipMap->fillWithTextureData();
	}

   // Reload the detail texture
   if(mDetailTextureName)
		mDetailTextureHandle.set( mDetailTextureName, &GFXDefaultStaticDiffuseProfile );

   // Signal anyone that cares on the lightmap update.
   mUpdateSignal.trigger( LightmapUpdate, Point2I(), Point2I() );

   return true;
}

bool TerrainBlock::initMMXBlender()
{
   U32 validMaterials = 0;
   S32 i;
   for (i = 0; i < MaterialGroups; i++) 
   {
      if (mMaterialFileName[i] && *mMaterialFileName[i])
         validMaterials++;
      else
         break;
   }
   AssertFatal(validMaterials != 0, "Error, must have SOME materials here!");

   updateOpacity();

   // ok, if the material list load failed...
   // if this is a local connection, we'll assume that's ok
   // and just have white textures...
   // otherwise we want to return false.
   return true;
}

void TerrainBlock::getMaterialAlpha(Point2I pos, U8 alphas[MaterialGroups])
{
   pos.x &= BlockMask;
   pos.y &= BlockMask;
   U32 offset = pos.x + pos.y * BlockSize;
   for(S32 i = 0; i < MaterialGroups; i++)
   {
      U8 *map = mFile->mMaterialAlphaMap[i];
      if(map)
         alphas[i] = map[offset];
      else
         alphas[i] = 0;
   }
}

void TerrainBlock::setMaterialAlpha(Point2I pos, const U8 alphas[MaterialGroups])
{
   pos.x &= BlockMask;
   pos.y &= BlockMask;
   U32 offset = pos.x + pos.y * BlockSize;
   for(S32 i = 0; i < MaterialGroups; i++)
   {
      U8 *map = mFile->mMaterialAlphaMap[i];
      if(map)
         map[offset] = alphas[i];
   }
}

S32 TerrainBlock::getMaterialAndAlpha(Point2I pos, Vector<U8>& alphas, Vector<std::string>& materials)
{
	alphas.clear();
	materials.clear();

	pos.x &= BlockMask;
	pos.y &= BlockMask;
	U32 offset = pos.x + pos.y * BlockSize;

	for(S32 i = 0; i < MaterialGroups; i++)
	{
		U8 *map = mFile->mMaterialAlphaMap[i];//map:纹理i中的透明度图,mMaterialAlphaMap:所有文件的透明度图
		if(map && map[offset]>0 && mMaterialFileName[i])
		{
			alphas.push_back(map[offset]);
			materials.push_back(mMaterialFileName[i]);
		}
	}

	return alphas.size();
}

S32 TerrainBlock::getMaterialAlphaIndex(const char *materialName)
{

   for (S32 i = 0; i < MaterialGroups; i++)
      if (mMaterialFileName[i] && *mMaterialFileName[i] && !dStricmp(materialName, mMaterialFileName[i]))
         return i;
   // ok, it wasn't in the list
   // see if we can add it:
   for (S32 i = 0; i < MaterialGroups; i++)
   {
      if (!mMaterialFileName[i] || !*mMaterialFileName[i])
      {
         mMaterialFileName[i] = StringTable->insert(materialName);
         mFile->mMaterialAlphaMap[i] = new U8[TerrainBlock::BlockSize * TerrainBlock::BlockSize];
         dMemset(mFile->mMaterialAlphaMap[i], 0, TerrainBlock::BlockSize * TerrainBlock::BlockSize);
         buildMaterialMap();
         return i;
      }
   }
   return -1;
}

//------------------------------------------------------------------------------

void TerrainBlock::refreshMaterialLists()
{
}

//------------------------------------------------------------------------------

void TerrainBlock::onEditorEnable()
{
   // need to load in the material base material lists
   if(isClientObject())
      refreshMaterialLists();
}

void TerrainBlock::onEditorDisable()
{

}

//------------------------------------------------------------------------------
bool TerrainBlock::loadRenderResource()
{
#ifdef NTJ_CLIENT
	if(!mbEnableUse)
		return true;

	//Ray: 地形本地载入
	//LivenHotch: 本地地形不再验证CRC
	//if(!isScopeLocal())
	//{
	//	NetConnection::setLastError("Your terrain file doesn't match the version that is running on the server.");
	//	return false;
	//}

	mCRC = mFile.getCRC();
#endif

	bakedlightmapAndcolorScale = new ColorF[LightmapSize * LightmapSize];
	//dMemset(bakedlightmapAndcolorScale, 0, TerrainBlock::LightmapSize * TerrainBlock::LightmapSize * sizeof(ColorF));
	for (U32 i=0; i<LightmapSize*LightmapSize; i++)
	{
		bakedlightmapAndcolorScale->red = 0;
		bakedlightmapAndcolorScale->green = 0;
		bakedlightmapAndcolorScale->blue = 0;
		bakedlightmapAndcolorScale->alpha = 0;
	}

	mShine = new F32[LightmapSize * LightmapSize];
	for (U32 i=0; i<LightmapSize*LightmapSize; i++)
	{
		mShine[i] = 1.0f;
	}
	
	//到这里共25M

	tex.set(TerrainBlock::LightmapSize, TerrainBlock::LightmapSize, GFXFormatR8G8B8A8, &GFXDefaultRenderTargetProfile, 0);
	tex2.set(TerrainBlock::LightmapSize, TerrainBlock::LightmapSize, GFXFormatR8G8B8A8, &GFXDefaultPersistentProfile, 0);
	tex3.set(TerrainBlock::LightmapSize, TerrainBlock::LightmapSize, GFXFormatR8G8B8A8, &GFXDefaultRenderTargetProfile, 0);
	tex4.set(TerrainBlock::LightmapSize, TerrainBlock::LightmapSize, GFXFormatR8G8B8A8, &GFXDefaultPersistentProfile, 0);

	freshShadowTex();

	lightMap = new GBitmap(LightmapSize, LightmapSize, false, GFXFormatR8G8B8A8);

	if (!buildMaterialMap())
		return false;

#ifdef USE_CLIPMAP
	// Set up the blended clipmap.
	mClipMap = new ClipMap();
	mClipMap->mTextureSize = TerrainBlock::BlockSize * mSquareSize * mTexelsPerMeter;
	mClipMap->mClipMapSize = BIT(mClipMapSizeLog2);
	mClipMap->mTile = mTile;

	// Lightmap is quad based, so no offset needed. 
	mLightmapSource = new TerrClipMapImageSource(tex3, 0.0f);

	mClipBlender = new ClipMapBlenderCache(mLightmapSource);
	mClipBlender->setShaderBaseName( StringTable->insert( "LegacyBlender" ) );

	updateOpacity();

	//到这里共34M
	
	mClipMap->setCache(mClipBlender);
	//到这里共50M
	mClipMap->initClipStack();
	//到这里共115M

	updateOpacity();

	// We don't page data in legacy, so this shouldn't need to loop.
	mClipMap->fillWithTextureData();
#else
	reloadTexture();
	//updateTexture();
	updateBlendTexture();
#endif
	bool success = __super::loadRenderResource();
#ifdef NTJ_CLIENT
	//载入地形资源后的打光
	if(!TerrainBlock::smLightingDone || TerrainBlock::smCurTerCRC != mCRC)
	{
		Con::executef("lightScene","sceneLightingComplete", "");
		TerrainBlock::smCurTerCRC = mCRC;
		TerrainBlock::smLightingDone = true;
	}
#endif

	return success;
}

void TerrainBlock::freeRenderResource()
{
	if( GFXDevice::devicePresent() )
	{
		GFX->unregisterTexCallback(mTexCallback);
	}
	mDetailTextureHandle = NULL;

	for (U32 i=0; i<mOpacityTexs.size(); i++)
	{
		mOpacityTexs[i].free();
	}
	mOpacityTexs.clear();
	for (U32 i=0; i<mOpacitySources.size(); i++)
	{
		SAFE_DELETE(mOpacitySources[i]);
	}
	mOpacitySources.clear();
	mOpacitySources.setSize(13);
	//for (U32 i = 0; i < mOpacitySources.size(); i++)
	//{
	//	mOpacitySources[i] = NULL;
	//} 
	
	SAFE_DELETE(lightMap);
	SAFE_DELETE(mShine);
	SAFE_DELETE(bakedlightmapAndcolorScale);


	Parent::freeRenderResource();
}

bool TerrainBlock::onAdd()
{
#ifdef NTJ_SERVER
   TerrainBlock::setTerMap(mWidthIndex, mHeightIndex, this);
   return false;
#endif

   if(!Parent::onAdd())
      return false;

   if (mTile)
      setPosition(Point3F((F32)(-mSquareSize * (BlockSize >> 1)), (F32)(-mSquareSize * (BlockSize >> 1)), 0));

   if (dStrlen(mTerrFileName) == 0 || dStricmp(mTerrFileName, "0") == 0)
   {
       createNewTerrainFile(Con::getVariable("$Client::MissionFile"));
   }

   Resource<TerrainFile> terr = ResourceManager->load(mTerrFileName, true);
   if(!bool(terr))
   {
      if(isClientObject())
         NetConnection::setLastError("You are missing a file needed to play this mission: %s", mTerrFileName);
      return false;
   }

   setFile(terr);
   TerrainBlock::setTerMap(mWidthIndex, mHeightIndex, this);

   if (mFile->needsResaving)
   {
	   if (Platform::messageBox("Update Terrain File", "You appear to have a Terrain file in an older format. Do you want PowerEngine to update it?", MBOkCancel, MIQuestion) == MROk)
	   {
		   mFile->save(mFile->fileNamePath);
		   mFile->needsResaving = false;
	   }
   }

   if (mFile->fileVersion != TerrainFile::FILE_VERSION || mFile->needsResaving)
   {
	   Con::errorf(" ****************************************************");
	   Con::errorf(" ****************************************************");
	   Con::errorf(" ****************************************************");
	   Con::errorf(" PLEASE RESAVE THE TERRAIN FILE FOR THIS MISSION!  THANKS!");
	   Con::errorf(" ****************************************************");
	   Con::errorf(" ****************************************************");
	   Con::errorf(" ****************************************************");
   }

   calcBounds();

   //if (mTile)
	   setGlobalFlag();

   resetWorldBox();
   setRenderTransform(mObjToWorld);

   if (isClientObject())
   {
#ifdef NTJ_EDITOR
	   if(!loadRenderResource())
		   return false;
#endif
   }
   else
	   mCRC = mFile.getCRC();

   addToScene();

   if(!unpackEmptySquares())
	   return(false);

#ifdef NTJ_CLIENT
   setResourceLoaded(false);//客户端不再预载贴图
   gClientObjectLoadMgr.registBlock(this);
#endif
   return true;
}

//--------------------------------------
void TerrainBlock::createNewTerrainFile(StringTableEntry terrainFileName, U16* hmap, Vector<StringTableEntry> *materials)
{
   TerrainFile *file = new TerrainFile;

   if (!materials)
   {
      file->mMaterialFileName[0] = StringTable->insert("Default");
      file->mMaterialAlphaMap[0] = new U8[TerrainBlock::BlockSize*TerrainBlock::BlockSize];
      dMemset(file->mMaterialAlphaMap[0], 0, TerrainBlock::BlockSize*TerrainBlock::BlockSize*sizeof(U8));

   }
   else
   {
      for (S32 i = 0; i < materials->size(); i++)
      {
         file->mMaterialFileName[i] = StringTable->insert((*materials)[i]);
         file->mMaterialAlphaMap[i] = new U8[TerrainBlock::BlockSize*TerrainBlock::BlockSize];

         if (!i)
         {
            dMemset(file->mMaterialAlphaMap[i], 255, TerrainBlock::BlockSize*TerrainBlock::BlockSize*sizeof(U8));
         }
         else
         {
            dMemset(file->mMaterialAlphaMap[i], 0, TerrainBlock::BlockSize*TerrainBlock::BlockSize*sizeof(U8));

            // Need at last one valid alpha value otherwise serializer strips the material off!
            file->mMaterialAlphaMap[i][0] = U8(1);
         }         
      }
   }

   file->mTextureScript = dStrdup("");
   file->mHeightfieldScript = dStrdup("");

   // create circular cone in the middle of the map:
   for(U32 i = 0; i < TerrainBlock::BlockSize; i++)
   {
      for(U32 j = 0; j < TerrainBlock::BlockSize; j++)
      {
         U32 offset = i + (j << TerrainBlock::BlockShift);

         if (hmap)
            file->mHeightMap[offset] = hmap[offset];
         else
            file->mHeightMap[offset] = (U16)(200);

         

         file->mBaseMaterialMap[offset] = 0;
		 file->mShadowMap[offset].set(0.0f, 0.0f, 0.0f ,0.0f);
      }
   }

   // We need to construct a default file name
   char fileName[1024];
   fileName[0] = 0;

   // See if we know our current mission name
   char missionName[1024];
   dSprintf(missionName, sizeof(missionName), "%s\0", terrainFileName);
   char * dot = dStrstr((const char*)missionName, ".mis");
   if(dot)
	   *dot = '\0';

   dSprintf(fileName, sizeof(fileName), "%s.ter", missionName);

   // 覆盖原文件
   // Find the first file that doesn't exist
   /*
   for (U32 i = 0; i < 32; i++)
   {
      char testName[1024];

      if (dStrlen(missionName) == 0 || dStricmp(missionName, "0") == 0)
         dSprintf(testName, sizeof(testName), "%s/data/missions/terrain%d.ter\0", Con::getVariable( "$defaultGame" ), i);
      else
         dSprintf(testName, sizeof(testName), "%s%d.ter\0", missionName, i);

      char fullName[1024];
      Platform::makeFullPathName(testName, fullName, sizeof(fullName));
      
      if (!Platform::isFile(fullName))
      {
         dSprintf(fileName, sizeof(fileName), "%s", fullName);
         break;
      }
   }
   */

   // If we found a valid file name then save it out
   if (dStrlen(fileName) > 0)
   {
      file->save(fileName);

      mTerrFileName = StringTable->insert(fileName);
   }
   
   delete file;
}

//--------------------------------------
void TerrainBlock::onRemove()
{
#ifdef NTJ_CLIENT
   if(smCurTerCRC == getCRC())
   {
	   smLightingDone = false;
	   smCurTerCRC = 0;
   }
#endif
   RemoveTerMap(mWidthIndex, mHeightIndex);
   removeFromScene();
#ifdef NTJ_CLIENT
   gClientObjectLoadMgr.unregistBlock(this);
#endif

   mDetailTextureHandle = NULL;
   SAFE_DELETE(mClipMap);

   Parent::onRemove();
}

//--------------------------------------------------------------------------
bool TerrainBlock::prepRenderImage(SceneState* state, const U32 stateKey,
                                   const U32 /*startZone*/, const bool /*modifyBaseState*/)
{
   if(!mbEnableUse)
		return true;

   PROFILE_SCOPE(TerrainBlock_prepRenderImage);

   if (!isResourceLoaded())
	   return false;

   if (isLastState(state, stateKey))
   {
      return false;
   }
   setLastState(state, stateKey);

   // This should be sufficient for most objects that don't manage zones, and
   //  don't need to return a specialized RenderImage...
   bool render = false;
   if (state->isTerrainOverridden() == false)
      render = state->isObjectRendered(this);
   else
      render = true;

   // small hack to reduce "stutter" in framerate if terrain is suddenly seen (ie. from an interior)
   if( !mHasRendered )
   {
      mHasRendered = true;
      render = true;
      state->enableTerrainOverride();
   }


   if (render == true)
   {
      RenderInst *ri = gRenderInstManager.allocInst();
      ri->obj = this;
      ri->state = state;
      ri->type = RenderInstManager::RIT_Object;
      gRenderInstManager.addInst( ri );
   }
   return false;
}

void TerrainBlock::buildChunkDeviance(S32 x, S32 y)
{
   mFile->buildChunkDeviance(x, y);
}

void TerrainBlock::buildGridMap()
{
   mFile->buildGridMap();
}

//------------------------------------------------------------------------------
void TerrainBlock::setTransform(const MatrixF & mat)
{
   Parent::setTransform(mat);

   // Since the terrain is a static object, it's render transform changes 1 to 1
   //  with it's collision transform
   setRenderTransform(mat);
}

void TerrainBlock::renderObject(SceneState* state, RenderInst *ri)
{
#ifndef NTJ_SERVER
	//纹理变化后，需要重新计算光度图纹理（lightmap不需要跟新）
	if (first)
	{
		Sun *sun = gClientSceneGraph->getCurrentSun();
		AssertFatal(sun, "TerrainProxy::postLight -- sun cannot be NULL");
		freshLightmapByShader(sun->mLightAmbient * sun->mTerrColorScale, sun->mLightColor * sun->mTerrColorScale, sun->mBackColor * sun->mTerrColorScale);
		refreshLightmap();//不是必须的
#ifndef USE_CLIPMAP
		//updateTexture();
		updateBlendTexture();
#endif
		first = false;
	}
#endif

   MatrixF proj   = GFX->getProjectionMatrix();
   MatrixF cleanProj = proj;
   RectI viewport = GFX->getViewport();

   if (state->isTerrainOverridden())
      state->setupBaseProjection();
   else
      state->setupObjectProjection(this);
#ifdef STATEBLOCK
	if( gClientSceneGraph->isReflectPass() )
	{
		AssertFatal(mCullCWSB, "TerrainBlock::renderObject -- mCullCWSB cannot be NULL.");
		mCullCWSB->apply();
	}
	else
	{
		AssertFatal(mCullCCWSB, "TerrainBlock::renderObject -- mCullCCWSB cannot be NULL.");
		mCullCCWSB->apply();
	}
#else
	if( gClientSceneGraph->isReflectPass() )
		GFX->setCullMode( GFXCullCW );
	else
		GFX->setCullMode( GFXCullCCW );
#endif


   GFX->pushWorldMatrix();
   GFX->multWorld( mRenderObjToWorld );

   // Set up scenegraph data
   SceneGraphData sgData;
   sgData.setDefaultLights();
   sgData.setFogParams();

   // Set up world transform
   MatrixF world = GFX->getWorldMatrix();
   proj.mul(world);
   proj.transpose();
   GFX->setVertexShaderConstF( 0, (float*)&proj, 4 );
	GFX->setVertexShaderConstF( 5, (float*)&world, 4 );

   // Store object and camera transform data
   sgData.objTrans = getRenderTransform();
   sgData.camPos   = state->getCameraPosition();

   gClientSceneGraph->getLightManager()->setupLights(this, state->getCameraPosition(),
	   Point3F(0, 0, 0), state->getVisibleDistance(), MaxVisibleLights);
   //gClientSceneGraph->getLightManager()->getBestLights();

   // Actually render the thing.
   static MatInstance tMat("TerrainMaterial");

   TerrainRender::renderBlock(this, state, &tMat, sgData);

   gClientSceneGraph->getLightManager()->resetLights();


   // Clean up after...
#ifdef STATEBLOCK
	AssertFatal(mCullNoneSB, "TerrainBlock::renderObject -- mCullNoneSB cannot be NULL.");
	mCullNoneSB->apply();
#else
   GFX->setCullMode(GFXCullNone);
#endif
#ifdef NTJ_EDITOR
	if (mNeedRenderSubTerrEdge)
	{
		renderSubTerrEdge();
	}
#endif
   GFX->setBaseRenderState();
   GFX->popWorldMatrix();

   GFX->setViewport( viewport );
   GFX->setProjectionMatrix( cleanProj );
}

//--------------------------------------
void TerrainBlock::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Media");
   addField("detailTexture",     TypeFilename,  Offset(mDetailTextureName, TerrainBlock));
   addField("terrainFile",       TypeFilename,  Offset(mTerrFileName,      TerrainBlock));
   endGroup("Media");

   addGroup("Misc");
   addField("enableUser",        TypeBool,      Offset(mbEnableUse,		  TerrainBlock));
   addField("squareSize",        TypeS32,       Offset(mSquareSize,       TerrainBlock));
   addField("emptySquares",      TypeS32Vector, Offset(mEmptySquareRuns,  TerrainBlock));
   addField("tile",              TypeBool,      Offset(mTile,             TerrainBlock));
   addField("detailDistance",    TypeF32,       Offset(mDetailDistance,   TerrainBlock));
   addField("detailScale",		 TypeF32,       Offset(mDetailScale,	  TerrainBlock));
   addField("detailBrightness",  TypeF32,       Offset(mDetailBrightness, TerrainBlock));
   addField("texelsPerMeter",    TypeS32,       Offset(mTexelsPerMeter,   TerrainBlock));
   addField("clipMapSizeLog2",   TypeS32,       Offset(mClipMapSizeLog2,  TerrainBlock));
   addField("widthIndex",        TypeS32,       Offset(mWidthIndex,       TerrainBlock));
   addField("heightIndex",       TypeS32,       Offset(mHeightIndex,      TerrainBlock));
	addField("edgeColor",			TypeColorF,		Offset(mEdgeColor, TerrainBlock));
   endGroup("Misc");

   //removeField("position");
}

//--------------------------------------
U64 TerrainBlock::packUpdate(NetConnection *, U64 mask, BitStream *stream)
{
   if(stream->writeFlag(mask & InitMask))
   {
      mathWrite(*stream, getTransform());
      mathWrite(*stream, getScale());
      mathWrite(*stream, mBounds);

      stream->write(mCRC);
      stream->writeString(mTerrFileName);
      stream->writeString(mDetailTextureName);
      stream->write(mSquareSize);

      // write out the empty rle vector
      stream->write(mEmptySquareRuns.size());
      for(U32 i = 0; i < mEmptySquareRuns.size(); i++)
         stream->write(mEmptySquareRuns[i]);

      stream->write(mWidthIndex);
      stream->write(mHeightIndex);
   }
   else // normal update
   {
      if(stream->writeFlag(mask & EmptyMask))
      {
         // write out the empty rle vector
         stream->write(mEmptySquareRuns.size());
         for(U32 i = 0; i < mEmptySquareRuns.size(); i++)
            stream->write(mEmptySquareRuns[i]);
      }
      if(stream->writeFlag(mask & TerrainChangedMask))
      {
         // Update our bounds
         mathWrite(*stream, mBounds);
      }
      if(stream->writeFlag(mask & MoveMask))
      {
         // Update our position, scale, and bounds
         mathWrite(*stream, getTransform());
         mathWrite(*stream, getScale());
         mathWrite(*stream, mBounds);
      }
   }

   stream->write(mbEnableUse);
   stream->write(mTile);
   stream->write(mDetailDistance);
   stream->write(mDetailScale);
   stream->write(mDetailBrightness);
   stream->write(mTexelsPerMeter);
   stream->write(mClipMapSizeLog2);
	stream->write(mNeedReloadTexture);
	stream->write(mEdgeColor);
   return 0;
}

void TerrainBlock::unpackUpdate(NetConnection *, BitStream *stream)
{
   if(stream->readFlag())  // init
   {
      MatrixF mat;
      Point3F scale;
      mathRead(*stream, &mat);
      mathRead(*stream, &scale);
      setScale(scale);
      setTransform(mat);

      mathRead(*stream, &mBounds);

      stream->read(&mCRC);
      mTerrFileName = stream->readSTString();
      mDetailTextureName = stream->readSTString();
      stream->read(&mSquareSize);

      // read in the empty rle
      U32 size;
      stream->read(&size);
      mEmptySquareRuns.setSize(size);
      for(U32 i = 0; i < size; i++)
         stream->read(&mEmptySquareRuns[i]);

      stream->read(&mWidthIndex);
      stream->read(&mHeightIndex);
   }
   else // normal update
   {
      if(stream->readFlag())  // empty
      {
         // read in the empty rle
         U32 size;
         stream->read(&size);
         mEmptySquareRuns.setSize(size);
         for(U32 i = 0; i < size; i++)
            stream->read(&mEmptySquareRuns[i]);

         //
         if(materialMap)
            unpackEmptySquares();
      }
      if (stream->readFlag())
      {
         // Get the update to our bounds
         mathRead(*stream, &mBounds);

         // If we aren't tiled then update our object box
         if (!mTile)
         {
            mObjBox = mBounds;
            resetWorldBox();
         }
      }
      if (stream->readFlag())
      {
         // Get the update to our position, scale, and bounds
         MatrixF mat;
         Point3F scale;
         mathRead(*stream, &mat);
         mathRead(*stream, &scale);
         setScale(scale);
         setTransform(mat);

         mathRead(*stream, &mBounds);

         // If we aren't tiled then update our object box
         if (!mTile)
         {
            mObjBox = mBounds;
            resetWorldBox();
         }
      }
   }

   stream->read(&mbEnableUse);
   stream->read(&mTile);
   stream->read(&mDetailDistance);
   stream->read(&mDetailScale);
   stream->read(&mDetailBrightness);
   stream->read(&mTexelsPerMeter);
   stream->read(&mClipMapSizeLog2);
	stream->read(&mNeedReloadTexture);
	stream->read(&mEdgeColor);
   mClipMapSizeLog2 = 11;
   if (!mTile)
   {
      mObjBox.max = mBounds.max;
      mObjBox.min = mBounds.min;
      resetWorldBox();
   }

   // Refresh the clipmap and sources.
   if(mClipMap)
   {
      // See if we really need to update - check that new values don't equal old.
      U32 newTexSize = TerrainBlock::BlockSize * mSquareSize * mTexelsPerMeter;
      U32 newMapSize = BIT(mClipMapSizeLog2);

      if(!isPow2(newTexSize))
      {
         Con::errorf("TerrainBlock::unpackUpdate - new texture size %dpx isn't a pow2, aborting update.", newTexSize);
         return;
      }

      if(!isPow2(newMapSize))
      {
         Con::errorf("TerrainBlock::unpackUpdate - new clipmap size %dpx isn't a pow2, aborting update.", newMapSize);
         return;
      }

      mClipMap->mTile = mTile;

      if(mClipMap->mTextureSize == newTexSize && mClipMap->mClipMapSize == newMapSize)
         return;

      mClipMap->mTextureSize = newTexSize;
      mClipMap->mClipMapSize = newMapSize;
      mClipMap->refreshCache();
      mClipMap->initClipStack();
   }
}

//--------------------------------------
ConsoleFunction(makeTestTerrain, void, 2, 10, "(string fileName, ...) - makes a test terrain file - arguments after the fileName are the names of the initial terrain materials.")
{
   TerrainFile *file = new TerrainFile;
   S32 nMaterialTypes;
   argc -= 2;

   // Load materials
   if (argc > 0)
   {
      nMaterialTypes = argc;
      for (S32 i=0; i<TerrainBlock::MaterialGroups && i < argc; i++)
      {
         char material[256];
         char *ext;
         dStrcpy(material, sizeof(material), argv[i+2]);
         ext = dStrrchr(material, '.');
         if (ext)
            *ext = 0;
         file->mMaterialFileName[i] = StringTable->insert(material);
         file->mMaterialAlphaMap[i] = new U8[TerrainBlock::BlockSize*TerrainBlock::BlockSize];
         dMemset(file->mMaterialAlphaMap[i], 0, TerrainBlock::BlockSize*TerrainBlock::BlockSize*sizeof(U8));
         
         // Make sure at least one square uses the material so it isn't dropped.
         file->mMaterialAlphaMap[i][i] = 1;
      }
      for(S32 i=argc; i<TerrainBlock::MaterialGroups; i++)
         file->mMaterialFileName[i] = StringTable->insert("");

   }
   else
   {
      nMaterialTypes = 1;
      file->mMaterialFileName[0] = StringTable->insert("Default");
      file->mMaterialAlphaMap[0] = new U8[TerrainBlock::BlockSize*TerrainBlock::BlockSize];
      dMemset(file->mMaterialAlphaMap[0], 0, TerrainBlock::BlockSize*TerrainBlock::BlockSize*sizeof(U8));
   }

   file->mTextureScript = dStrdup("");
   file->mHeightfieldScript = dStrdup("");

   // create circular cone in the middle of the map:
   S32 i, j;
   for(i = 0; i < TerrainBlock::BlockSize; i++)
   {
      for(j = 0; j < TerrainBlock::BlockSize; j++)
      {
         S32 x = i & 0x7f;
         S32 y = j & 0x7f;

         F32 dist = mSqrt(F32((64 - x) * (64 - x) + (64 - y) * (64 - y)));
         dist /= 64.0f;

         if(dist > 1)
            dist = 1;

         U32 offset = i + (j << TerrainBlock::BlockShift);
         file->mHeightMap[offset] = (U16)((1 - dist) * (1 - dist) * 20000);
         file->mBaseMaterialMap[offset] = 0;
		 file->mShadowMap[offset].set(0.0f, 0.0f, 0.0f ,0.0f);
      }
   }

   //
   char filename[256];
   dStrcpy(filename, sizeof(filename), argv[1]);
   char* ext = dStrrchr(filename, '.');
   if (!ext || dStricmp(ext, ".ter") != 0)
      dStrcat(filename, sizeof(filename), ".ter");
   file->save(filename);
   delete file;
}


//--------------------------------------

void TerrainBlock::flushCache()
{
   //AllocatedTexture::flushCache();
}

U16 TerrainBlock::getHeight(U32 x, U32 y)
{
   return mFile->getHeight(x,y, isTiling()); 
}

//--------------------------------------

void TerrainBlock::setTerMap(U32 x, U32 y, TerrainBlock* tblock)
{
	AssertFatal( tblock, "check the terrains!");
	setMaxTerrainWidth(x+1);
	setMaxTerrainHeight(y+1);
	if(1 == mMaxTerrainWidth == mMaxTerrainHeight)
	{
		mTerrBox.min.set(tblock->getPosition());
		mTerrBox.max.set(tblock->getPosition());
	}
	else
		mTerrBox.extend(tblock->getPosition());
#ifdef NTJ_SERVER
	return;
#endif

	mTerMap[x][y] = tblock;

	// 建立与相邻地形的关系
	if (x > 0 && mTerMap[x-1][y])
	{
		mTerMap[x][y]->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_LEFT]=mTerMap[x-1][y]->getFile();
		mTerMap[x-1][y]->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_RIGHT]=mTerMap[x][y]->getFile();
	}
	if (y > 0 && mTerMap[x][y-1])
	{
		mTerMap[x][y]->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_TOP]=mTerMap[x][y-1]->getFile();
		mTerMap[x][y-1]->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_BOTTOM]=mTerMap[x][y]->getFile();
	}
	if (x < mMaxTerrainWidth-1 && mTerMap[x+1][y])
	{
		mTerMap[x][y]->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_RIGHT]=mTerMap[x+1][y]->getFile();
		mTerMap[x+1][y]->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_LEFT]=mTerMap[x][y]->getFile();
	}
	if (y < mMaxTerrainHeight-1 && mTerMap[x][y+1])
	{
		mTerMap[x][y]->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_BOTTOM]=mTerMap[x][y+1]->getFile();
		mTerMap[x][y+1]->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_TOP]=mTerMap[x][y]->getFile();
	}
}

TerrainBlock* TerrainBlock::getTerMap(U32 x, U32 y)
{
	if( x < mMaxTerrainWidth && y < mMaxTerrainHeight)
		return mTerMap[x][y];
	return NULL;
}

void TerrainBlock::RemoveTerMap(U32 x, U32 y)
{
	if( x < mMaxTerrainWidth && y < mMaxTerrainHeight)
	{
		// 建立与相邻地形的关系
		if (x > 0 && mTerMap[x-1][y])
			mTerMap[x-1][y]->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_RIGHT]=NULL;
		if (y > 0 && mTerMap[x][y-1])
			mTerMap[x][y-1]->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_BOTTOM]=NULL;
		if (x < mMaxTerrainWidth-1 && mTerMap[x+1][y])
			mTerMap[x+1][y]->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_LEFT]=NULL;
		if (y < mMaxTerrainHeight-1 && mTerMap[x][y+1])
			mTerMap[x][y+1]->getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_TOP]=NULL;
		mTerMap[x][y] = NULL;
	}
}

void TerrainBlock::clearTerMap()
{
	dMemset(TerrainBlock::mTerMap,0,sizeof(TerrainBlock*)*TerrainBlock::TerrainSize*TerrainBlock::TerrainSize);
	mMaxTerrainWidth = 0;
	mMaxTerrainHeight = 0;
}

void TerrainBlock::setMaxTerrainWidth(U32 width)
{
	AssertFatal( width < TerrainBlock::TerrainSize, "check the terrains!");
	mMaxTerrainWidth = getMax(width, mMaxTerrainWidth);
}

void TerrainBlock::setMaxTerrainHeight(U32 height)
{
	AssertFatal( height < TerrainBlock::TerrainSize, "check the terrains!");
	mMaxTerrainHeight = getMax(height, mMaxTerrainHeight);
}

void TerrainBlock::loadNaviGrid(StringTableEntry filename /* = NULL */)
{
	// 先取得所有地形的统一偏移
	Point3F center(0,0,0);
	mTerrBox.getCenter(&center);
	// 初始化导航网格基本信息
	NaviGrid::InitNaviGridInfo(mMaxTerrainWidth, mMaxTerrainHeight, center.x + singleTerrainOffest, center.y + singleTerrainOffest);
	g_NavigationMesh.InitCellArray(NaviGrid::TableSize);
	char fName[256] = "";

#ifdef NTJ_EDITOR
	if(!filename)
		return;

	//加载导航网格
	dStrcpy(fName, sizeof(fName), filename);
	char *ext = dStrrchr(fName, '.');
	//打开导航网络
	*ext = '\0';
	dStrcat(fName, sizeof(fName), ".ng");
	bool load = g_NavigationMesh.Open(fName);
	if(!load)
		Con::errorf("can not load NaviGrid File: %s !!",fName);
#endif // NTJ_EDITOR

	g_NavigationManager->Load();
}

//--------------------------------------
TerrainFile::TerrainFile()
{
   for(S32 i=0; i < TerrainBlock::MaterialGroups; i++)
   {
      mMaterialFileName[i] = NULL;
      mMaterialAlphaMap[i] = NULL;
   }
   mTextureScript = 0;
   mHeightfieldScript = 0;

   needsResaving = false;
   fileVersion = FILE_VERSION;
   fileNamePath = StringTable->insert("");

   for(int i=0; i<TerrainBlock::EDGE_MAX; ++i)
      mEdgeTerrainFiles[i] = NULL;

   //初始化颜色
   dMemset(mShadowMap, 0, sizeof(mShadowMap));

	//初始化透明度
	for (U32 i=0; i<TerrainBlock::ChunkSize*TerrainBlock::ChunkSize; i++)
	{
		dMemset(mTexIndex[i], 0, TerrainBlock::Max_Texture_Layer);
		for (U32 j=0; j<TerrainBlock::Max_Texture_Layer; j++)
		{
			mTexAlpha[i][j] = NULL;
		}
	}
}

TerrainFile::~TerrainFile()
{
   for(S32 i=0; i < TerrainBlock::MaterialGroups; i++) {
      delete[] mMaterialAlphaMap[i];
      mMaterialAlphaMap[i] = NULL;
   }
   dFree(mTextureScript);
   dFree(mHeightfieldScript);

	//释放透明度
	for (U32 i=0; i<TerrainBlock::ChunkSize*TerrainBlock::ChunkSize; i++)
	{
		for (U32 j=0; j<TerrainBlock::Max_Texture_Layer; j++)
		{
			delete [] mTexAlpha[i][j];
			mTexAlpha[i][j] = NULL;
		}
	}
}

bool TerrainFile::destoryMaterialMap(S32 num)
{
	if ((num >= 0) && (num < TerrainBlock::MaterialGroups))
	{
		if(mMaterialAlphaMap[num])
		{
			delete[] mMaterialAlphaMap[num];
			mMaterialAlphaMap[num] = NULL;
			mMaterialFileName[num] = NULL;
			return true;
		}
		return false;
	}
	return false;
}

U16 TerrainFile::getHeight(U32 x, U32 y, bool tiling)
{
   if ((x == TerrainBlock::BlockSize) && mEdgeTerrainFiles[TerrainBlock::EDGE_RIGHT] != NULL)
      return mEdgeTerrainFiles[TerrainBlock::EDGE_RIGHT]->getHeight(0,y);

   if ((y == TerrainBlock::BlockSize) && mEdgeTerrainFiles[TerrainBlock::EDGE_BOTTOM] != NULL)
      return mEdgeTerrainFiles[TerrainBlock::EDGE_BOTTOM]->getHeight(x,0);

   return mHeightMap[(x & TerrainBlock::BlockMask) + ((y & TerrainBlock::BlockMask) << TerrainBlock::BlockShift)];
}


void TerrainFile::setTextureScript(const char *script)
{
   dFree(mTextureScript);
   mTextureScript = dStrdup(script);
}

void TerrainFile::setHeightfieldScript(const char *script)
{
   dFree(mHeightfieldScript);
   mHeightfieldScript = dStrdup(script);
}

void TerrainBlock::getMinMaxHeight( F32* minHeight, F32* maxHeight ) const 
{
   *minHeight =  fixedToFloat( gridMap[BlockShift]->minHeight );
   *maxHeight =  fixedToFloat( gridMap[BlockShift]->maxHeight );
}


const char *TerrainFile::getTextureScript()
{
   return mTextureScript ? mTextureScript : "";
}

const char *TerrainFile::getHeightfieldScript()
{
   return mHeightfieldScript ? mHeightfieldScript : "";
}

//--------------------------------------

bool TerrainFile::save(const char *filename)
{
   char terFilePath[1024];
   Platform::makeFullPathName(filename, terFilePath, sizeof(terFilePath));
   char *ptr = dStrrchr(terFilePath, '/');
   if(ptr) *ptr = 0;

   FileStream writeFile;
   if (!ResourceManager->openFileForWrite(writeFile, filename))
      return false;

   // write the VERSION and HeightField
   //writeFile.write((U8)FILE_VERSION);
	writeFile.write((U8)5);
   for (S32 i=0; i < (TerrainBlock::BlockSize * TerrainBlock::BlockSize); i++)
      writeFile.write(mHeightMap[i]);

   // write the material group map, after merging the flags...
   TerrainBlock::Material * materialMap = (TerrainBlock::Material*)mMaterialMap;
   for (S32 j=0; j < (TerrainBlock::BlockSize * TerrainBlock::BlockSize); j++)
   {
      U8 val = mBaseMaterialMap[j];
      val |= materialMap[j].flags & TerrainBlock::Material::PersistMask;
      writeFile.write(val);
   }

   // write the MaterialList Info
   S32 k;
   for(k=0; k < TerrainBlock::MaterialGroups; k++)
   {
      // ok, only write out the material string if there is a non-zero
      // alpha material:

      /*
      if(mMaterialFileName[k] && mMaterialFileName[k][0])
      {
         S32 n;
         for(n = 0; n < TerrainBlock::BlockSize * TerrainBlock::BlockSize; n++)
            if(mMaterialAlphaMap[k][n])
               break;
         if(n == TerrainBlock::BlockSize * TerrainBlock::BlockSize)
            mMaterialFileName[k] = 0;
      }
      */

      StringTableEntry matRelPath = "";
      if(mMaterialFileName[k])
      {
         char matFullPath[1024];
         Platform::makeFullPathName(mMaterialFileName[k], matFullPath, sizeof(matFullPath));
         matRelPath = Platform::makeRelativePathName(matFullPath, terFilePath);
      }
      writeFile.writeString(matRelPath);
   }
   for(k=0; k < TerrainBlock::MaterialGroups; k++) {
      if(mMaterialFileName[k] && mMaterialFileName[k][0]) {
         AssertFatal(mMaterialAlphaMap[k] != NULL, "Error, must have a material map here!");
         writeFile.write(TerrainBlock::BlockSize * TerrainBlock::BlockSize, mMaterialAlphaMap[k]);
      }
   }
   if(mTextureScript)
   {
      U32 len = dStrlen(mTextureScript);
      writeFile.write(len);
      writeFile.write(len, mTextureScript);
   }
   else
      writeFile.write(U32(0));

   if(mHeightfieldScript)
   {
      U32 len = dStrlen(mHeightfieldScript);
      writeFile.write(len);
      writeFile.write(len, mHeightfieldScript);
   }
   else
      writeFile.write(U32(0));

   if (mShadowMap)
   {
	   U32 len = sizeof(mShadowMap);
	   writeFile.write(len);
	   writeFile.write(len, mShadowMap);
   } 
   else
   {
	   writeFile.write(U32(0));
   }
   return (writeFile.getStatus() == FileStream::Ok);
}

//--------------------------------------

void TerrainFile::heightDevLine(U32 p1x, U32 p1y, U32 p2x, U32 p2y, U32 pmx, U32 pmy, U16 *devPtr)
{
   S32 h1 = getHeight(p1x, p1y);
   S32 h2 = getHeight(p2x, p2y);
   S32 hm = getHeight(pmx, pmy);
   S32 dev = ((h2 + h1) >> 1) - hm;
   if(dev < 0)
      dev = -dev;
   if(dev > *devPtr)
      *devPtr = dev;
}

void TerrainFile::buildChunkDeviance(S32 x, S32 y)
{
   GridChunk &gc = *(mChunkMap + x + (y << TerrainBlock::ChunkShift));
   gc.emptyFlags = 0;
   U32 sx = x << TerrainBlock::ChunkDownShift;
   U32 sy = y << TerrainBlock::ChunkDownShift;

   gc.heightDeviance[0] = 0;

   heightDevLine(sx, sy, sx + 2, sy, sx + 1, sy, &gc.heightDeviance[0]);
   heightDevLine(sx + 2, sy, sx + 4, sy, sx + 3, sy, &gc.heightDeviance[0]);

   heightDevLine(sx, sy + 2, sx + 2, sy + 2, sx + 1, sy + 2, &gc.heightDeviance[0]);
   heightDevLine(sx + 2, sy + 2, sx + 4, sy + 2, sx + 3, sy + 2, &gc.heightDeviance[0]);

   heightDevLine(sx, sy + 4, sx + 2, sy + 4, sx + 1, sy + 4, &gc.heightDeviance[0]);
   heightDevLine(sx + 2, sy + 4, sx + 4, sy + 4, sx + 3, sy + 4, &gc.heightDeviance[0]);

   heightDevLine(sx, sy, sx, sy + 2, sx, sy + 1, &gc.heightDeviance[0]);
   heightDevLine(sx + 2, sy, sx + 2, sy + 2, sx + 2, sy + 1, &gc.heightDeviance[0]);
   heightDevLine(sx + 4, sy, sx + 4, sy + 2, sx + 4, sy + 1, &gc.heightDeviance[0]);

   heightDevLine(sx, sy + 2, sx, sy + 4, sx, sy + 3, &gc.heightDeviance[0]);
   heightDevLine(sx + 2, sy + 2, sx + 2, sy + 4, sx + 2, sy + 3, &gc.heightDeviance[0]);
   heightDevLine(sx + 4, sy + 2, sx + 4, sy + 4, sx + 4, sy + 3, &gc.heightDeviance[0]);

   gc.heightDeviance[1] = gc.heightDeviance[0];

   heightDevLine(sx, sy, sx + 2, sy + 2, sx + 1, sy + 1, &gc.heightDeviance[1]);
   heightDevLine(sx + 2, sy, sx, sy + 2, sx + 1, sy + 1, &gc.heightDeviance[1]);

   heightDevLine(sx + 2, sy, sx + 4, sy + 2, sx + 3, sy + 1, &gc.heightDeviance[1]);
   heightDevLine(sx + 2, sy + 2, sx + 4, sy, sx + 3, sy + 1, &gc.heightDeviance[1]);

   heightDevLine(sx, sy + 2, sx + 2, sy + 4, sx + 1, sy + 3, &gc.heightDeviance[1]);
   heightDevLine(sx + 2, sy + 4, sx, sy + 2, sx + 1, sy + 3, &gc.heightDeviance[1]);

   heightDevLine(sx + 2, sy + 2, sx + 4, sy + 4, sx + 3, sy + 3, &gc.heightDeviance[1]);
   heightDevLine(sx + 2, sy + 4, sx + 4, sy + 2, sx + 3, sy + 3, &gc.heightDeviance[1]);

   gc.heightDeviance[2] = gc.heightDeviance[1];

   heightDevLine(sx, sy, sx + 4, sy, sx + 2, sy, &gc.heightDeviance[2]);
   heightDevLine(sx, sy + 4, sx + 4, sy + 4, sx + 2, sy + 4, &gc.heightDeviance[2]);
   heightDevLine(sx, sy, sx, sy + 4, sx, sy + 2, &gc.heightDeviance[2]);
   heightDevLine(sx + 4, sy, sx + 4, sy + 4, sx + 4, sy + 2, &gc.heightDeviance[2]);

   for(U32 j = 0; j < 4; j++)
   {
      for(U32 i = 0; i < 4; i++)
      {
         TerrainBlock::Material *mat = getMaterial(sx + i, sy + j);
         if(mat->flags & TerrainBlock::Material::Empty)
            gc.emptyFlags |= (1 << (j * 4 + i));
      }
   }
}

void TerrainFile::buildGridMap()
{
   S32 y;
   for(y = 0; y < TerrainBlock::ChunkSquareWidth; y++)
      for(U32 x = 0; x < TerrainBlock::ChunkSquareWidth; x++)
         buildChunkDeviance(x, y);

   GridSquare * gs = mGridMapBase;
   S32 i;
   for(i = TerrainBlock::BlockShift; i >= 0; i--)
   {
      mGridMap[i] = gs;
      gs += 1 << (2 * (TerrainBlock::BlockShift - i));
   }
   for(i = TerrainBlock::BlockShift; i >= 0; i--)
   {
      S32 squareCount = 1 << (TerrainBlock::BlockShift - i);
      S32 squareSize = (TerrainBlock::BlockSize) / squareCount;

      for(S32 squareX = 0; squareX < squareCount; squareX++)
      {
         for(S32 squareY = 0; squareY < squareCount; squareY++)
         {
            U16 min = 0xFFFF;
            U16 max = 0;
            U16 mindev45 = 0;
            U16 mindev135 = 0;

            Point3F p1, p2, p3, p4;

            // determine max error for both possible splits.
            PlaneF pl1, pl2, pl3, pl4;

            p1.set(0, 0, getHeight(squareX * squareSize, squareY * squareSize));
            p2.set(0, (F32)squareSize, getHeight(squareX * squareSize, squareY * squareSize + squareSize));
            p3.set((F32)squareSize, (F32)squareSize, getHeight(squareX * squareSize + squareSize, squareY * squareSize + squareSize));
            p4.set((F32)squareSize, 0, getHeight(squareX * squareSize + squareSize, squareY * squareSize));

            // pl1, pl2 = split45, pl3, pl4 = split135
            pl1.set(p1, p2, p3);
            pl2.set(p1, p3, p4);
            pl3.set(p1, p2, p4);
            pl4.set(p2, p3, p4);
            bool parentSplit45 = false;
            GridSquare *parent = NULL;
            if(i < TerrainBlock::BlockShift)
            {
               parent = findSquare(i+1, Point2I(squareX * squareSize, squareY * squareSize));
               parentSplit45 = parent->flags & GridSquare::Split45;
            }
            bool empty = true;
            bool hasEmpty = false;

            for(S32 sizeX = 0; sizeX <= squareSize; sizeX++)
            {
               for(S32 sizeY = 0; sizeY <= squareSize; sizeY++)
               {
                  S32 x = squareX * squareSize + sizeX;
                  S32 y = squareY * squareSize + sizeY;

                  if(sizeX != squareSize && sizeY != squareSize)
                  {
                     TerrainBlock::Material *mat = getMaterial(x, y);
                     if(!(mat->flags & TerrainBlock::Material::Empty))
                        empty = false;
                     else
                        hasEmpty = true;
                  }
                  U16 ht = getHeight(x, y);

                  if(ht < min)
                     min = ht;
                  if(ht > max)
                     max = ht;
                  Point3F pt((F32)sizeX, (F32)sizeY, (F32)ht);
                  U16 dev;

                  if(sizeX < sizeY)
                     dev = calcDev(pl1, pt);
                  else if(sizeX > sizeY)
                     dev = calcDev(pl2, pt);
                  else
                     dev = Umax(calcDev(pl1, pt), calcDev(pl2, pt));

                  if(dev > mindev45)
                     mindev45 = dev;

                  if(sizeX + sizeY < squareSize)
                     dev = calcDev(pl3, pt);
                  else if(sizeX + sizeY > squareSize)
                     dev = calcDev(pl4, pt);
                  else
                     dev = Umax(calcDev(pl3, pt), calcDev(pl4, pt));

                  if(dev > mindev135)
                     mindev135 = dev;
               }
            }
            GridSquare *sq = findSquare(i, Point2I(squareX * squareSize, squareY * squareSize));
            sq->minHeight = min;
            sq->maxHeight = max;

            sq->flags = empty ? GridSquare::Empty : 0;
            if(hasEmpty)
               sq->flags |= GridSquare::HasEmpty;

            bool shouldSplit45 = ((squareX ^ squareY) & 1) == 0;
            bool split45;

            //split45 = shouldSplit45;
            if(i == 0)
               split45 = shouldSplit45;
            else if(i < 4 && shouldSplit45 == parentSplit45)
               split45 = shouldSplit45;
            else
               split45 = mindev45 < mindev135;

            //split45 = shouldSplit45;
            if(split45)
            {
               sq->flags |= GridSquare::Split45;
               sq->heightDeviance = mindev45;
            }
            else
               sq->heightDeviance = mindev135;
            if(parent)
               if(parent->heightDeviance < sq->heightDeviance)
                  parent->heightDeviance = sq->heightDeviance;
         }
      }
   }
   for (y = 0; y < TerrainBlock::BlockSize; y++)
   {
      for (S32 x=0; x < TerrainBlock::BlockSize; x++)
      {
         GridSquare *sq = findSquare(0, Point2I(x, y));
         S32 xpl = (x + 1) & TerrainBlock::BlockMask;
         S32 ypl = (y + 1) & TerrainBlock::BlockMask;
         for(U32 i = 0; i < TerrainBlock::MaterialGroups; i++)
         {
            if (mMaterialAlphaMap[i] != NULL) {
               U32 mapVal = (mMaterialAlphaMap[i][(y << TerrainBlock::BlockShift) + x]     +
                             mMaterialAlphaMap[i][(ypl << TerrainBlock::BlockShift) + x]   +
                             mMaterialAlphaMap[i][(ypl << TerrainBlock::BlockShift) + xpl] +
                             mMaterialAlphaMap[i][(y << TerrainBlock::BlockShift) + xpl]);
               if(mapVal)
                  sq->flags |= (GridSquare::MaterialStart << i);
            }
         }
      }
   }
   for (y = 0; y < TerrainBlock::BlockSize; y += 2)
   {
      for (S32 x=0; x < TerrainBlock::BlockSize; x += 2)
      {
         GridSquare *sq = findSquare(1, Point2I(x, y));
         GridSquare *s1 = findSquare(0, Point2I(x, y));
         GridSquare *s2 = findSquare(0, Point2I(x+1, y));
         GridSquare *s3 = findSquare(0, Point2I(x, y+1));
         GridSquare *s4 = findSquare(0, Point2I(x+1, y+1));
         sq->flags |= (s1->flags | s2->flags | s3->flags | s4->flags) & ~(GridSquare::MaterialStart -1);
      }
   }
   GridSquare *s = findSquare(1, Point2I(0, 0));
   U16 *dflags = mFlagMap;
   U16 *eflags = mFlagMap + TerrainBlock::FlagMapWidth * TerrainBlock::FlagMapWidth;

   for(;dflags != eflags;s++,dflags++)
      *dflags = s->flags;

}

//--------------------------------------
ResourceInstance *constructTerrainFile(Stream &stream, ResourceObject *obj)
{
   char terFilePath[1024];
   char *ptr = NULL;

   if(obj)
   {
      Platform::makeFullPathName(obj->getFullPath(), terFilePath, sizeof(terFilePath));
      ptr = dStrrchr(terFilePath, '/');
      if(ptr) *ptr = 0;
   }
   else
   {
      terFilePath[0] = 0;
      ptr = terFilePath;
   }

   U8 version;
   stream.read(&version);
   if (version > TerrainFile::FILE_VERSION)
      return NULL;

   TerrainFile* ret = new TerrainFile;

   ret->fileVersion = version;
   ret->fileNamePath = obj->getFullPath();

   // read the HeightField
   for (S32 i=0; i < (TerrainBlock::BlockSize * TerrainBlock::BlockSize); i++)
      stream.read(&ret->mHeightMap[i]);

   // read the material group map and flags...
   dMemset(ret->mMaterialMap, 0, sizeof(ret->mMaterialMap));
   TerrainBlock::Material * materialMap = (TerrainBlock::Material*)ret->mMaterialMap;

   AssertFatal(!(TerrainBlock::Material::PersistMask & TerrainFile::MATERIAL_GROUP_MASK),
               "Doh! We have flag clobberage...");

   for (S32 j=0; j < (TerrainBlock::BlockSize * TerrainBlock::BlockSize); j++)
   {
      U8 val;
      stream.read(&val);

      //
      ret->mBaseMaterialMap[j] = val & TerrainFile::MATERIAL_GROUP_MASK;
      materialMap[j].flags = val & TerrainBlock::Material::PersistMask;
   }

   // read the MaterialList Info
   S32 k, maxMaterials = TerrainBlock::MaterialGroups;
   for(k=0; k < maxMaterials;)
   {
      // [tom, 5/16/2007] Paths are now stored relative to the terrain file
      StringTableEntry matRelFile = stream.readSTString(true);

      if(matRelFile && *matRelFile && version > 3)
      {
         char matFullPath[1024];
         Platform::makeFullPathName(matRelFile, matFullPath, sizeof(matFullPath), terFilePath);
         ret->mMaterialFileName[k] = StringTable->insert(matFullPath, true);
      }
      else
         ret->mMaterialFileName[k] = matRelFile;

      if(ret->mMaterialFileName[k] && ret->mMaterialFileName[k][0])
         k++;
      else
         maxMaterials--;
   }
   for(;k < TerrainBlock::MaterialGroups; k++)
      ret->mMaterialFileName[k] = NULL;

   if (version <= 3)
   {
      // Try to automatically fix up our material file names
      for (U32 i = 0; i < TerrainBlock::MaterialGroups; i++)
      {
         if (ret->mMaterialFileName[i] && ret->mMaterialFileName[i][0])
         {
            if (GBitmap::findBmpResource(ret->mMaterialFileName[i]))
               continue;

            // Most likely the first directory is the old game dir so replace it with $defaultGame
            char relBuffer[1024];
            relBuffer[0] = 0;

            const char* rel = dStrchr(ret->mMaterialFileName[i], '/');

            if (rel != ret->mMaterialFileName[i])
               dSprintf(relBuffer, sizeof(relBuffer), "%s%s", Con::getVariable( "$defaultGame" ), rel);
            else
               dStrncpy(relBuffer, sizeof(relBuffer), ret->mMaterialFileName[i], 1024);

            char matFullPath[1024];
            Platform::makeFullPathName(relBuffer, matFullPath, sizeof(matFullPath));

            if (GBitmap::findBmpResource(relBuffer))
            {
               ret->mMaterialFileName[i] = StringTable->insert(relBuffer);
               ret->needsResaving = true;
            }
         }
      }
   }

	if(version == 1)
	{
		for(S32 j = 0; j < (TerrainBlock::BlockSize * TerrainBlock::BlockSize); j++) {
			if (ret->mMaterialAlphaMap[ret->mBaseMaterialMap[j]] == NULL) {
				ret->mMaterialAlphaMap[ret->mBaseMaterialMap[j]] = new U8[TerrainBlock::BlockSize * TerrainBlock::BlockSize];
				dMemset(ret->mMaterialAlphaMap[ret->mBaseMaterialMap[j]], 0, TerrainBlock::BlockSize * TerrainBlock::BlockSize);
			}

			ret->mMaterialAlphaMap[ret->mBaseMaterialMap[j]][j] = 255;
		}
	}
	else
	{
		for(S32 k=0; k < TerrainBlock::MaterialGroups; k++) {
			if(ret->mMaterialFileName[k] && ret->mMaterialFileName[k][0]) {
				AssertFatal(ret->mMaterialAlphaMap[k] == NULL, "Bad assumption.  There should be no alpha map at this point...");
				ret->mMaterialAlphaMap[k] = new U8[TerrainBlock::BlockSize * TerrainBlock::BlockSize];
				stream.read(TerrainBlock::BlockSize * TerrainBlock::BlockSize, ret->mMaterialAlphaMap[k]);
			}
		}
	}

	if(version >= 3)
	{
		U32 len;
		stream.read(&len);
		ret->mTextureScript = (char *) dMalloc(len + 1);
		stream.read(len, ret->mTextureScript);
		ret->mTextureScript[len] = 0;

		stream.read(&len);
		ret->mHeightfieldScript = (char *) dMalloc(len + 1);
		stream.read(len, ret->mHeightfieldScript);
		ret->mHeightfieldScript[len] = 0;

		//读取颜色刷刷的颜色值
		len = 0;
		stream.read(&len);
		stream.read(len, ret->mShadowMap);
	}
	else
	{
		ret->mTextureScript = 0;
		ret->mHeightfieldScript = 0;
	}

   ret->buildGridMap();

   return ret;
}

void TerrainBlock::setBaseMaterial(U32 /*x*/, U32 /*y*/, U8 /*matGroup*/)
{

}

void TerrainBlock::addOpacitySources()
{
	for (U32 i = TerrainBlock::MaterialGroups-1; i >=0 ; i--)
	{
		if(mFile->mMaterialAlphaMap[i])
		{
			U32 num = (i+1) / 4;
			for (S32 j = mOpacityTexs.size(); j < num + 1; j++)
			{
				mOpacityTexs.increment();
				mOpacityTexs.last().set(TerrainBlock::BlockSize, TerrainBlock::BlockSize, GFXFormatR8G8B8A8, &ClipMapTextureProfile);
			}
			break;
		}
	}
	if (mOpacitySources.size() < 13)
	{
		mOpacitySources.setSize(13);
		for (U32 i = 0; i < mOpacitySources.size(); i++)
		{
			mOpacitySources[i] = NULL;
		} 
	}
	bool needRegister = false;
	for (U32 i=0; i<mOpacityTexs.size(); i++)
		{
		if (mOpacitySources[i] == NULL)
			{
				needRegister = true;
			mOpacitySources[i] = (new TerrClipMapImageSource(mOpacityTexs[i], 0.5f));
		}
	}

	if (needRegister)
	{
		if (mClipBlender)
		{
			mClipBlender->clearOpacitySources();

			for (U32 i = 0; i < mOpacitySources.size(); i++)
			{
				if(mOpacitySources[i])
					mClipBlender->registerOpacitySource(mOpacitySources[i]);
			}
		}
	}
}

void TerrainBlock::updateOpacity()
{
   PROFILE_SCOPE(TerrainBlock_updateOpacity);

   // Make sure we have enough mOpacityMaps
   addOpacitySources();

   // Copy our various opacity maps in.
	GFXLockedRect *rt = NULL;
	S32 layer = 0;
	for (S32 i=0; i<mOpacityTexs.size()/*13*/; i++)
	{
		GFXTexHandle& pTex = mOpacityTexs[i];
		rt = pTex.lock();
		U8 *tmpBits = NULL;
		for (S32 j=0; j<4; j++)
		{
			layer = i*4 + j;
			if (layer>=50 || !mFile->mMaterialAlphaMap[layer])
			{
				continue;
			}
			int swapj = j;
			if (swapj == 0)//0和2交换，以保持和纹理格式一致，代替原来的swizzle
			{
				swapj = 2;
			}
			else if (swapj == 2)
			{
				swapj = 0;
			}
			tmpBits = (U8*)rt->bits + swapj;
			U8 *opacityBits = mFile->mMaterialAlphaMap[layer];
			const U8 *opacityEnd = opacityBits + (TerrainBlock::BlockSize*TerrainBlock::BlockSize);

			while(opacityBits != opacityEnd)
			{
				*tmpBits = *opacityBits;
				tmpBits += 4; opacityBits++;
			}
		}

		pTex.unlock();
	}

   // Update the blender image cache with our current source materials.
   if(mClipBlender)
   {
	   mClipBlender->clearSourceImages();
	   for(S32 i=0; i<TerrainBlock::MaterialGroups; i++)
		   mClipBlender->registerSourceImage(mFile->mMaterialFileName[i]);
   }

   // Flush the clipmap (if it's active and has levels)
   if(mClipMap->mLevels.size())
      mClipMap->fillWithTextureData();
}

bool TerrainBlock::isTiling()
{
   return mTile;
}

void TerrainBlock::refillClipmap()
{
   mClipMap->fillWithTextureData();
}

void TerrainBlock::inspectPostApply()
{
   Parent::inspectPostApply();

   if (mClipMapSizeLog2 < 9)
      mClipMapSizeLog2 = 9;
   
   //if (mClipMapSizeLog2 > 10)
     // mClipMapSizeLog2 = 10;

   if (mTexelsPerMeter < 4)
      mTexelsPerMeter = 4;

   if (mTexelsPerMeter > 128)
      mTexelsPerMeter = 128;

   setMaskBits(MoveMask);
}

//addGroup("Media");
//addField("detailTexture",     TypeFilename,  Offset(mDetailTextureName, TerrainBlock));
//addField("terrainFile",       TypeFilename,  Offset(mTerrFileName,      TerrainBlock));
//endGroup("Media");
//
//addGroup("Misc");
//addField("enableUser",        TypeBool,      Offset(mbEnableUse,		  TerrainBlock));
//addField("squareSize",        TypeS32,       Offset(mSquareSize,       TerrainBlock));
//addField("emptySquares",      TypeS32Vector, Offset(mEmptySquareRuns,  TerrainBlock));
//addField("tile",              TypeBool,      Offset(mTile,             TerrainBlock));
//addField("detailDistance",    TypeF32,       Offset(mDetailDistance,   TerrainBlock));
//addField("detailScale",		 TypeF32,       Offset(mDetailScale,	  TerrainBlock));
//addField("detailBrightness",  TypeF32,       Offset(mDetailBrightness, TerrainBlock));
//addField("texelsPerMeter",    TypeS32,       Offset(mTexelsPerMeter,   TerrainBlock));
//addField("clipMapSizeLog2",   TypeS32,       Offset(mClipMapSizeLog2,  TerrainBlock));
//addField("widthIndex",        TypeS32,       Offset(mWidthIndex,       TerrainBlock));
//addField("heightIndex",       TypeS32,       Offset(mHeightIndex,      TerrainBlock));
//endGroup("Misc");

void TerrainBlock::OnSerialize( CTinyStream& stream )
{
	__super::OnSerialize( stream );
	stream.writeString( mDetailTextureName );
	stream.writeString( strstr( mTerrFileName , "gameres" ) );

	stream << mbEnableUse;
	stream << mSquareSize;
	int length = mEmptySquareRuns.size();
	stream << length;
	for( int i=0;i< length;i++)
		stream << mEmptySquareRuns[i];
	stream << mTile;
	stream << mDetailDistance;
	stream << mDetailScale;
	stream << mDetailBrightness;
	stream << mTexelsPerMeter;
	stream << mClipMapSizeLog2;
	stream << mWidthIndex;
	stream << mHeightIndex;
}

void TerrainBlock::OnUnserialize( CTinyStream& stream )
{
	__super::OnUnserialize( stream );
	char buf[1024];

	stream.readString( buf, 1024 );
	mDetailTextureName = StringTable->insert( buf );
	stream.readString( buf, 1024 );
	char fileName[1024];
	GetCurrentDirectoryA( 1024, fileName );
	strcat_s( fileName, 1024, "\\" );
	strcat_s( fileName, 1024, buf );
	mTerrFileName = StringTable->insert( fileName );


	stream >> mbEnableUse;
	stream >> mSquareSize;
	int length, v;
	stream >> length;
	for( int i = 0; i < length; i++ )
	{
		stream >> v;
		mEmptySquareRuns.push_back( v );
	}
	stream >> mTile;
	stream >> mDetailDistance;
	stream >> mDetailScale;
	stream >> mDetailBrightness;
	stream >> mTexelsPerMeter;
	stream >> mClipMapSizeLog2;
	stream >> mWidthIndex;
	stream >> mHeightIndex;
}


U8 TerrainBlock::getSubBlockIndex( F32 x, F32 y )
{
	Point3F terrPos = __super::getPosition();
	float invSquareSize = 1.0f / (float)mSquareSize;
	float xp = (x-terrPos.x) * invSquareSize;
	float yp = (y-terrPos.y) * invSquareSize;
	int ix = (S32)mFloor(xp);
	int iy = (S32)mFloor(yp);
	ix &= BlockMask;
	iy &= BlockMask;
	return (((iy/ChunkSquareWidth) & ChunkShiftMask)<<ChunkDownShift) | ((ix/ChunkSquareWidth) & ChunkShiftMask);
}

U8 TerrainBlock::getSubBlockIndex( U32 x, U32 y )
{
	return (((y/ChunkSquareWidth) & ChunkShiftMask)<<ChunkDownShift) | ((x/ChunkSquareWidth) & ChunkShiftMask);
}

void TerrainBlock::reloadTexture()
{
	//更新地形基础纹理
	StringTableEntry filename;
	for (U32 i=0; i<MaterialGroups; i++)
	{
		filename = mMaterialFileName[i];
		if(!filename || !filename[0])
		{
			// If we are fed blank, we need to have a blank source
			// for the shader.
			GBitmap* bmp = new GBitmap(2,2);
			dMemset(bmp->getWritableBits(0), 0, bmp->byteSize);
			mBaseMaterials[i].set(bmp, &GFXDefaultStaticDiffuseProfile, true);
			continue;
		}

		GBitmap *bmp  = GBitmap::load(filename);
		if(bmp)
		{
			mTexNum = i;
			bmp->extrudeMipLevels();
			mBaseMaterials[i].set(bmp, &GFXDefaultStaticDiffuseProfile, true);
		}
	}

	//加载纹理后，需要重新设置混合纹理
	unInit = true;
	//更新每个子块的基础纹理
	for (U32 ck=0; ck<ChunkSize*ChunkSize; ck++)
	{
		for (U32 i=0; i<Max_Texture_Layer; i++)
		{
			mTextures[ck][i] = mBaseMaterials[mFile->mTexIndex[ck][i]];
		}
	}
}

void TerrainBlock::updateTexture()
{
	//更新每个子块的基础纹理
	for (U32 ck=0; ck<ChunkSize*ChunkSize; ck++)
	{
		for (U32 i=0; i<Max_Texture_Layer; i++)
		{
			mTextures[ck][i] = mBaseMaterials[mFile->mTexIndex[ck][i]];
		}
	}
	//更新每个子块的透明度纹理(ChunkSquareWidth*ChunkSquareWidth)
	const U32 len = ChunkSquareWidth;
	for (U32 ck=0; ck<ChunkSize*ChunkSize; ck++)
	{
		GFXTexHandle &tex = mTextures[ck][Max_Texture_Layer];

		if (unInit)
		{
#ifdef NTJ_EDITOR
			tex.set(len, len, GFXFormatR8G8B8A8, &GFXDynamicDiffuseProfile);
#endif
#ifdef NTJ_CLIENT
			tex.set(len, len, GFXFormatR8G8B8A8, &GFXDefaultStaticDiffuseProfile);
#endif		
		}
		GFXLockedRect *lr = tex.lock();
		ColorI *color = (ColorI*)lr->bits;
		for (U32 row=0; row<len; row++)
		{
			for (U32 col=0; col<len; col++)
			{
				color[row*len + col].red	=	mFile->mTexAlpha[ck][2][row*len + col];//0
				color[row*len + col].green	=	mFile->mTexAlpha[ck][1][row*len + col];
				color[row*len + col].blue	=	mFile->mTexAlpha[ck][0][row*len + col];//2
				color[row*len + col].alpha	=	mFile->mTexAlpha[ck][3][row*len + col];
			}
		}
		tex.unlock();
	}
	unInit = false;
}

void TerrainBlock::updateBlendTexture()
{
	//更新每个子块的透明度纹理(ChunkSquareWidth*ChunkSquareWidth)
	const U32 len = BlockSize;
	for (U32 ck=0; ck<mTexNum/4 + 1; ck++)
	{
		GFXTexHandle &tex = mBlendTex[ck];

		if (unInit)
		{
#ifdef NTJ_EDITOR
			tex.set(len, len, GFXFormatR8G8B8A8, &GFXDynamicDiffuseProfile);
#endif
#ifdef NTJ_CLIENT
			tex.set(len, len, GFXFormatR8G8B8A8, &GFXDefaultStaticDiffuseProfile);
#endif		
		}
		GFXLockedRect *lr = tex.lock();
		ColorI *color = (ColorI*)lr->bits;
		for (U32 row=0; row<len; row++)
		{
			for (U32 col=0; col<len; col++)
			{
				color[row*len + col].red	= mFile->mMaterialAlphaMap[ck*4 +2]?mFile->mMaterialAlphaMap[ck*4 +2][row*len + col] : 0;//0
				color[row*len + col].green	= mFile->mMaterialAlphaMap[ck*4 +1]?mFile->mMaterialAlphaMap[ck*4 +1][row*len + col] : 0;
				color[row*len + col].blue	= mFile->mMaterialAlphaMap[ck*4 +0]?mFile->mMaterialAlphaMap[ck*4 +0][row*len + col] : 0;//2
				color[row*len + col].alpha	= mFile->mMaterialAlphaMap[ck*4 +3]?mFile->mMaterialAlphaMap[ck*4 +3][row*len + col] : 0;
			}
		}
		tex.unlock();
	}
	unInit = false;
}

//-----------------------------------------------------------------------------
// Console Methods
//-----------------------------------------------------------------------------

ConsoleMethod(TerrainBlock, save, bool, 3, 3, "(string fileName) - saves the terrain block's terrain file to the specified file name.")
{
   char filename[256];
   dStrcpy(filename, sizeof(filename), argv[2]);
   char *ext = dStrrchr(filename, '.');
   if (!ext || dStricmp(ext, ".ter") != 0)
      dStrcat(filename, sizeof(filename), ".ter");
   return static_cast<TerrainBlock*>(object)->save(filename);
}

ConsoleMethod(TerrainBlock, saveAs, bool, 3, 3, "(string fileName) - saves the terrain block's terrain file to the specified file name.")
{
	TerrainBlock* obj = static_cast<TerrainBlock*>(object);
	if(!obj)
		return false;

	// 产生存储名字 与buildMegaTerrain一致
	// We need to construct a default file name
	char fileName[1024];
	fileName[0] = 0;

	// See if we know our current mission name
	char missionName[1024];
	dSprintf(missionName, sizeof(missionName), "%s\0", argv[2]);
	char * dot = dStrstr((const char*)missionName, ".mis");
	if(dot)
		*dot = '\0';

	//dSprintf(fileName, sizeof(fileName), "%s_megaterrain_%i_%i.ter", missionName, obj->mWidthIndex, obj->mHeightIndex);
	dSprintf(fileName, sizeof(fileName), "%s.ter", missionName);

	// If we found a valid file name then save it out
	if (dStrlen(fileName) > 0)
	{
		if(obj->save(fileName))
		{
			obj->mTerrFileName = StringTable->insert(fileName);
			TerrainBlock* tblock = TerrainBlock::getTerMap(obj->mWidthIndex,obj->mHeightIndex);
			if(tblock)
				tblock->mTerrFileName = StringTable->insert(fileName);

			return true;
		}
	}
	return false;
}

ConsoleFunction(getTerrainHeight, F32, 2, 3, "(Point2 pos) - gets the terrain height at the specified position."
				"@param pos The world space point, minus the z (height) value\n Can be formatted as either (\"x y\") or (x,y)\n"
				"@return Returns the terrain height at the given point as an F32 value.\n")
{
	Point2F pos;
	F32 height = 0.0f;

	if(argc == 2)
		dSscanf(argv[1],"%f %f",&pos.x,&pos.y);
	else if(argc == 3)
	{
		pos.x = dAtof(argv[1]);
		pos.y = dAtof(argv[2]);
	}

	TerrainBlock * terrain = getTerrainUnderWorldPoint(Point3F(pos.x, pos.y, 5000.0f));
	if(terrain)
		if(terrain->isServerObject())
		{
			Point3F offset;
			terrain->getTransform().getColumn(3, &offset);
			pos -= Point2F(offset.x, offset.y);
			terrain->getHeight(pos, &height);
		}
		return height;
}

ConsoleFunction(getTerrainHeightBelowPosition, F32, 2, 4, "(Point3F pos) - gets the terrain height at the specified position."
				"@param pos The world space point. Can be formatted as either (\"x y z\") or (x,y,z)\n"
				"@note This function is useful if you simply want to grab the terrain height underneath an object.\n"
				"@return Returns the terrain height at the given point as an F32 value.\n")
{
	Point3F pos;
	F32 height = 0.0f;

   if(argc == 2)
      dSscanf(argv[1], "%f %f %f", &pos.x, &pos.y, &pos.z);
   else if(argc == 4)
   {
      pos.x = dAtof(argv[1]);
      pos.y = dAtof(argv[2]);
      pos.z = dAtof(argv[3]);
   }

   else
   {
      Con::errorf("getTerrainHeightBelowPosition(Point3F): Invalid argument count! Valid arguments are either \"x y z\" or x,y,z\n");
      return 0;
   }

	TerrainBlock * terrain = getTerrainUnderWorldPoint(pos);
	
	Point2F nohghtPos(pos.x, pos.y);

	if(terrain)
	{
		if(terrain->isServerObject())
		{
			Point3F offset;
			terrain->getTransform().getColumn(3, &offset);
			nohghtPos -= Point2F(offset.x, offset.y);
			terrain->getHeight(nohghtPos, &height);
		}
	}
	
	return height;
}

ConsoleMethod(TerrainBlock, setTextureScript, void, 3, 3, "(string script) - sets the texture script associated with this terrain file.")
{
   TerrainBlock *terr = (TerrainBlock *) object;
   terr->getFile()->setTextureScript(argv[2]);
}

ConsoleMethod(TerrainBlock, setHeightfieldScript, void, 3, 3, "(string script) - sets the heightfield script associated with this terrain file.")
{
   TerrainBlock *terr = (TerrainBlock *) object;
   terr->getFile()->setHeightfieldScript(argv[2]);
}

ConsoleMethod(TerrainBlock, getTextureScript, const char *, 2, 2, "() - gets the texture script associated with the terrain file.")
{
   TerrainBlock *terr = (TerrainBlock *) object;
   return terr->getFile()->getTextureScript();
}

ConsoleMethod(TerrainBlock, getHeightfieldScript, const char *, 2, 2, "() - gets the heightfield script associated with the terrain file.")
{
   TerrainBlock *terr = (TerrainBlock *) object;
   return terr->getFile()->getHeightfieldScript();
}

ConsoleMethod(TerrainBlock, purgeClipmap, void, 2, 2, "Rebuilds the clipmap textures")
{
   TerrainBlock *terr = (TerrainBlock *) object;
   return terr->refillClipmap();
}

//World Map Helper
#ifdef NTJ_EDITOR
ConsoleFunction(buildMegaTerrain, void, 16, 16, "buildMegaTerrain helper function")
{
   if(gServerSceneGraph->getCurrentTerrain() != NULL)
   {
	   Con::executef("MessageBoxOK", "buildMegaTerrain", "Please delete the current terrain first!", "");
	   return;
   }

   SimObject *guiObject;
   SimGroup *missionGroup;

   TerrainEditor* editor = dynamic_cast<TerrainEditor*>(Sim::findObject("ETerrainEditor"));

   if( !editor )
   {
      Con::errorf("Missing ETerrainEditor!\n");
      return;
   }

   if( !Sim::findObject( "MissionGroup", missionGroup ) )
   {
      Con::errorf("Missing MissionGroup!\n");
      return;
   }

   if( !Sim::findObject( argv[1], guiObject ) )
   {
      Con::errorf("Missing GuiObjectBuilder object!\n");
      return;
   }

   S32 squareSize = dAtoi(argv[2]);
   const char* hmap = argv[3];
   F32 heightScalar = dAtof(argv[4]);

   if (squareSize != 2 && squareSize != 4 && squareSize != 8 && squareSize != 16 && squareSize != 32) 
   {
      Con::errorf("Square Size - must be 2, 4, 8, 16, or 32");
      return;
   }

   S32 width = dAtoi(argv[14]);
   S32 height = dAtoi(argv[15]);
   if (width < 1 && width > TerrainBlock::TerrainSize && height < 1 && height > TerrainBlock::TerrainSize && width+height < 2) 
   {
	   Con::errorf("terrain Size - must be 1 ~ %d",TerrainBlock::TerrainSize);
	   return;
   }

   if (heightScalar < 32) heightScalar = 32;
   if (heightScalar > 255) heightScalar = 255;

   const char* detail = StringTable->insert(argv[5]);

   Vector<StringTableEntry> materials;

   for (S32 i = 6; i < 14; i++)
   {
      const char* texture = argv[i];
      if (dStrlen(texture) != 0 && dStricmp(texture, "0"))
         materials.push_back(StringTable->insert(texture));
   }

   GBitmap* heightmap = NULL;

   if (dStrlen(hmap) != 0 && dStricmp(hmap, "0"))
   {
      heightmap = GBitmap::load(hmap);
      if (heightmap)
      {
         U32 hwidth = heightmap->getWidth();
         U32 hheight = heightmap->getHeight();

         if (hwidth != 512 || hheight != 512)
         {
            Con::errorf("Heightmap - height and width must be 512");
            return;
         }

         // Use heightmap width and height
         width = hwidth/256;
         height = hheight/256;

      }
   }

   SimGroup* mapGroup = new SimGroup();
   mapGroup->assignName("MegaTerrain");
   mapGroup->registerObject();

   char missionName[1024], terrainName[1024];
   dSprintf(missionName,sizeof(missionName),"%s",Con::getVariable("$Client::MissionFile"));
   char * dot = dStrstr((const char*)missionName, ".mis");
   if(dot)
      *dot = '\0';

   S32 terrainSize = squareSize * TerrainBlock::BlockSize;
   S32 worldY = (-terrainSize*height)/2;
   U16 hinfo[TerrainBlock::BlockSize * TerrainBlock::BlockSize];

   Terraformer* tformer = new Terraformer();
   tformer->setTerrainInfo(512, squareSize, 0.0f, heightScalar*16.0f, 0.0f);

   if (heightmap)
   {
      tformer->loadGreyscale(0, heightmap);

      // Smoothing could be exposed to object builder
      tformer->smooth(0, 0, 1.0f, 4);
   }

   TerrainBlock::clearTerMap();
   TerrainBlock::setMaxTerrainWidth(width);
   TerrainBlock::setMaxTerrainHeight(height);

   for (S32 y=0; y<height; y++, worldY+=terrainSize)
   {
      S32 worldX = (-terrainSize*width)/2;

      for (S32 x=0; x<width; x++, worldX+=terrainSize)
      {
         dSprintf(terrainName,sizeof(terrainName),"%s_megaterrain_%i_%i",missionName,x,y);

         TerrainBlock *tblock = new TerrainBlock();

         tformer->setHeightmap(0, hinfo, heightScalar*16.f, 0/*x*TerrainBlock::BlockSize*/, 0/*y*TerrainBlock::BlockSize*/);
         
         tblock->mTerrFileName = StringTable->insert(terrainName);
         tblock->createNewTerrainFile(terrainName,hinfo,&materials);
         
         tblock->setTiling(false);
         tblock->setSquareSize(squareSize);
         tblock->setPosition(Point3F(worldX,worldY,0));

         if (dStrlen(detail) != 0 && dStricmp(detail, "0"))
         {
            tblock->mDetailTextureName = detail;
         }
		 tblock->mWidthIndex = x;
		 tblock->mHeightIndex = y;

         tblock->registerObject();
         mapGroup->addObject(tblock);

         editor->attachTerrain(tblock);
      }
   }

   delete tformer;

   missionGroup->addObject(mapGroup);

   editor->setMissionDirty();
   editor->setDirty();

}
#endif

ConsoleFunction(clearTerMap, void, 1, 1, "clearTerMap")
{
	TerrainBlock::clearTerMap();
}

ConsoleFunction(loadNaviGrid, void, 1, 2, "loadNaviGrid")
{
	if(argc > 1)
		TerrainBlock::loadNaviGrid(argv[1]);
	else
		TerrainBlock::loadNaviGrid();
}

#ifdef STATEBLOCK
void TerrainBlock::freshLightmapByShader(ColorF ambient, ColorF frontColor, ColorF backColor, bool bScene)
{
	if (bScene)
	{
		GFX->beginScene();
	}

	MatrixF proj = GFX->getProjectionMatrix();
	RectI viewPort = GFX->getViewport();
	GFX->pushWorldMatrix();

	// For sanity, let's just purge all our texture states.
	for(S32 i=0; i<GFX->getNumSamplers(); i++)
		GFX->setTexture(i, NULL);

	bool bSetTarget = true;
	if (bSetTarget)
	{
		GFX->pushActiveRenderTarget();
		GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
		myTarg->attachTexture(GFXTextureTarget::Color0, tex );
		GFX->setActiveRenderTarget( myTarg );
	}

	// And some render states.
	GFX->setWorldMatrix(MatrixF(true));
	GFX->setProjectionMatrix(MatrixF(true));
	U32 w = LightmapSize;
	U32 h = LightmapSize;
	GFX->setViewport(RectI(0, 0, w, h));
	GFX->setClipRect(RectI(0,0, w, h));

	AssertFatal(mSetShader1SB, "TerrainBlock::freshLightmapByShader -- mSetShader1SB cannot be NULL.");
	mSetShader1SB->apply();
	GFXVertexBufferHandle<GFXVertexPT> verts(GFX, 4, GFXBufferTypeVolatile );

	verts.lock();
	U32 cod = LightmapSize;
	verts[0].point.set( 0,		0,		0 );
	verts[0].texCoord.set(0.0f, 0.0f);//

	verts[1].point.set( cod,	0,		0);
	verts[1].texCoord.set(1.0f, 0.0f);

	verts[2].point.set( 0,	cod,		0 );
	verts[2].texCoord.set(0.0f, 1.0f);

	verts[3].point.set(cod,	cod,		0);
	verts[3].texCoord.set(1.0f, 1.0f	);
	verts.unlock();


	GFX->setVertexBuffer( verts );


	/************************************************************************/
	/*					                shader1                                  */
	/************************************************************************/
	ShaderData *shader = NULL;
	if(!Sim::findObject("TerrainLightMap", shader) || !shader->getShader())
	{
		Con::errorf("freshLightmapByShader - could not find shader!");
		GFX->popActiveRenderTarget();
		GFX->setBaseRenderState();
		if (bScene)
		{
			GFX->endScene();
		}
		return;
	}
	GFX->setShader(shader->getShader());

	GFX->setTexture(0, tex4);
	float cfAmbient[] = {ambient.red, ambient.green, ambient.blue, ambient.alpha};
	float cffrontColor[] = {frontColor.red, frontColor.green, frontColor.blue, frontColor.alpha};
	float cfbackColor[] = {backColor.red, backColor.green, backColor.blue, backColor.alpha};

	//GFX->setPixelShaderConstF(9, (float*)&ambient, 1);
	//GFX->setPixelShaderConstF(10, (float*)&frontColor, 1);
	GFX->setPixelShaderConstF(9, cfAmbient, 1);
	GFX->setPixelShaderConstF(10, cffrontColor, 1);
	GFX->setPixelShaderConstF(12, cfbackColor, 1);
	GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );
	if (bSetTarget)
	{
		GFX->popActiveRenderTarget();
	}
	/************************************************************************/
	/*				                     shader2                                */
	/************************************************************************/
	AssertFatal(mSetShader2SB, "TerrainBlock::freshLightmapByShader -- mSetShader2SB cannot be NULL.");
	mSetShader2SB->apply();
	tex3.getLegal();
	bSetTarget = true;
	if (bSetTarget)
	{
		GFX->pushActiveRenderTarget();
		GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
		myTarg->attachTexture(GFXTextureTarget::Color0, tex3 );
		GFX->setActiveRenderTarget( myTarg );
	}

	if(!Sim::findObject("TerrainLightMap2", shader) || !shader->getShader())
	{
		Con::errorf("freshLightmapByShader - could not find shader !");
		GFX->popActiveRenderTarget();
		GFX->setBaseRenderState();
		if (bScene)
		{
			GFX->endScene();
		}
		return;
	}
	GFX->setShader(shader->getShader());
	GFX->setTexture(0, tex);
	GFX->setTexture(1, tex2);

	F32 kernel[] = { 1, 2, 1, 2, 4, 2, 1, 2, 1, 16, 0, 1};//kernel:9, divisor:1, min:1, max:1
	GFX->setPixelShaderConstF(0, &kernel[0], 3);
	GFX->drawPrimitive(GFXTriangleStrip, 0, 2);

	if (bSetTarget)
	{
		GFX->popActiveRenderTarget();
	}

	/************************************************************************/
	/*                               shader end                             */
	/************************************************************************/

	GFX->setVertexBuffer( NULL );


	// Reset texture stages.
	// For sanity, let's just purge all our texture states.
	for(S32 i=0; i<GFX->getNumSamplers(); i++)
		GFX->setTexture(i, NULL);

	// And clear render states.
	AssertFatal(mClearSB, "TerrainBlock::freshLightmapByShader -- mClearSB cannot be NULL.");
	mClearSB->apply();
	GFX->popWorldMatrix();
	GFX->setProjectionMatrix(proj);
	GFX->setViewport(viewPort);

	// Purge state before we end the scene - just in case.
	GFX->updateStates();
	if (bScene)
	{
		GFX->endScene();
	}

#ifdef USE_CLIPMAP
	if(mClipMap)
	{
		mClipMap->fillWithTextureData();
	}
#endif
}

#else
void TerrainBlock::freshLightmapByShader(ColorF ambient, ColorF frontColor, ColorF backColor, bool bScene)
{
	if (bScene)
	{
		GFX->beginScene();
	}
	
	MatrixF proj = GFX->getProjectionMatrix();
	RectI viewPort = GFX->getViewport();
	GFX->pushWorldMatrix();

	// For sanity, let's just purge all our texture states.
	for(S32 i=0; i<GFX->getNumSamplers(); i++)
		GFX->setTexture(i, NULL);

	bool bSetTarget = true;
	if (bSetTarget)
	{
		GFX->pushActiveRenderTarget();
		GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
		myTarg->attachTexture(GFXTextureTarget::Color0, tex );
		GFX->setActiveRenderTarget( myTarg );
	}

	// And some render states.
	GFX->setZEnable(false);
	GFX->setZWriteEnable(false);
	GFX->setWorldMatrix(MatrixF(true));
	GFX->setProjectionMatrix(MatrixF(true));
	U32 w = LightmapSize;
	U32 h = LightmapSize;
	GFX->setViewport(RectI(0, 0, w, h));
	GFX->setClipRect(RectI(0,0, w, h));
	GFX->setCullMode( GFXCullNone );

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

	GFX->setAlphaTestEnable(false);
	GFX->setAlphaBlendEnable(false);

	GFXVertexBufferHandle<GFXVertexPT> verts(GFX, 4, GFXBufferTypeVolatile );

	verts.lock();
	U32 cod = LightmapSize;
	verts[0].point.set( 0,		0,		0 );
	verts[0].texCoord.set(0.0f, 0.0f);//

	verts[1].point.set( cod,	0,		0);
	verts[1].texCoord.set(1.0f, 0.0f);

	verts[2].point.set( 0,	cod,		0 );
	verts[2].texCoord.set(0.0f, 1.0f);

	verts[3].point.set(cod,	cod,		0);
	verts[3].texCoord.set(1.0f, 1.0f	);
	verts.unlock();


	GFX->setVertexBuffer( verts );


	/************************************************************************/
	/*					                shader1                                  */
	/************************************************************************/
	ShaderData *shader = NULL;
	if(!Sim::findObject("TerrainLightMap", shader) || !shader->getShader())
	{
		Con::errorf("freshLightmapByShader - could not find shader!");
		GFX->popActiveRenderTarget();
		GFX->setFillMode(GFXFillSolid);
		GFX->setBaseRenderState();
		if (bScene)
		{
			GFX->endScene();
		}
		return;
	}
	GFX->setShader(shader->getShader());

	GFX->setTexture(0, tex4);
	float cfAmbient[] = {ambient.red, ambient.green, ambient.blue, ambient.alpha};
	float cffrontColor[] = {frontColor.red, frontColor.green, frontColor.blue, frontColor.alpha};
	float cfbackColor[] = {backColor.red, backColor.green, backColor.blue, backColor.alpha};
	//GFX->setPixelShaderConstF(9, (float*)&ambient, 1);
	//GFX->setPixelShaderConstF(10, (float*)&frontColor, 1);
	GFX->setPixelShaderConstF(9, cfAmbient, 1);
	GFX->setPixelShaderConstF(10, cffrontColor, 1);
	GFX->setPixelShaderConstF(12, cfbackColor, 1);
	GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );
	if (bSetTarget)
	{
		GFX->popActiveRenderTarget();
	}
	/************************************************************************/
	/*				                     shader2                                */
	/************************************************************************/
	GFX->setTextureStageAddressModeU(1, GFXAddressWrap);
	GFX->setTextureStageAddressModeV(1, GFXAddressWrap);
	GFX->setTextureStageColorOp(1, GFXTOPModulate);

	tex3.getLegal();
	bSetTarget = true;
	if (bSetTarget)
	{
		GFX->pushActiveRenderTarget();
		GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
		myTarg->attachTexture(GFXTextureTarget::Color0, tex3 );
		GFX->setActiveRenderTarget( myTarg );
	}

	if(!Sim::findObject("TerrainLightMap2", shader) || !shader->getShader())
	{
		Con::errorf("freshLightmapByShader - could not find shader !");
		GFX->popActiveRenderTarget();
		GFX->setFillMode(GFXFillSolid);
		GFX->setBaseRenderState();
		if (bScene)
		{
			GFX->endScene();
		}
		return;
	}
	GFX->setShader(shader->getShader());
	GFX->setTexture(0, tex);
	GFX->setTexture(1, tex2);

	F32 kernel[] = { 1, 2, 1, 2, 4, 2, 1, 2, 1, 16, 0, 1};//kernel:9, divisor:1, min:1, max:1
	GFX->setPixelShaderConstF(0, &kernel[0], 3);
	GFX->drawPrimitive(GFXTriangleStrip, 0, 2);

	if (bSetTarget)
	{
		GFX->popActiveRenderTarget();
	}

	/************************************************************************/
	/*                               shader end                             */
	/************************************************************************/

	GFX->setVertexBuffer( NULL );


	// Reset texture stages.
	// For sanity, let's just purge all our texture states.
	for(S32 i=0; i<GFX->getNumSamplers(); i++)
		GFX->setTexture(i, NULL);

	// And clear render states.
	GFX->setZEnable(true);
	GFX->setZWriteEnable(true);
	GFX->popWorldMatrix();
	GFX->setProjectionMatrix(proj);
	GFX->setViewport(viewPort);

	GFX->setAlphaBlendEnable(false);
	GFX->setAlphaTestEnable(false);

	// Purge state before we end the scene - just in case.
	GFX->updateStates();
	if (bScene)
	{
		GFX->endScene();
	}

#ifdef USE_CLIPMAP
	if(mClipMap)
	{
		mClipMap->fillWithTextureData();
	}
#endif
}


#endif

bool TerrainBlock::destoryMaterialMap(S32 num)
{
	//mOpacityMaps.clear();
	if(!mFile->destoryMaterialMap(num))
		return false;
	updateGridMaterials(Point2I(0,0),Point2I(TerrainBlock::BlockSize,TerrainBlock::BlockSize));
	return true;
}

void TerrainBlock::freshShadowTex()
{
#ifdef NTJ_CLIENT
	if(!mbEnableUse)
		return;
#endif

	GFXLockedRect* lockRect = tex4.lock();
	U32 bytePerPix = lockRect->pitch/LightmapSize;
	if (mShadowMap)
	{
		for (U32 i=0; i<BlockSize; i++)
		{
			for (U32 j=0; j<BlockSize; j++)
			{
				//每个EmitChunk对应16个像素
				U32 offsetY = i*LightmapSize/BlockSize;
				U32 offsetX = j*LightmapSize/BlockSize;
				U32 offset = 0;
				U32 offsetMap = i*BlockSize + j;
				//只更新刷颜色的像素
				if (mShadowMap[offsetMap].alpha > MIN_SHADOW_ALPHA)
				{
					for (U32 k=0; k<4; k++)
					{
						for (U32 m=0; m<4; m++)
						{
							offset = ((offsetY+k)*LightmapSize + offsetX+m)*4;
							lockRect->bits[offset + 0] = mShadowMap[offsetMap].blue		* 255;//b
							lockRect->bits[offset + 1] = mShadowMap[offsetMap].green		* 255;//g
							lockRect->bits[offset + 2] = mShadowMap[offsetMap].red		* 255;//r
							if (mShadowMap[offsetMap].alpha < MIN_BRUSH_COLOR_ALPHA)
							{
								lockRect->bits[offset + 3] = 0;//a
								mShadowMap[offsetMap].alpha = SUN_ALPHA;
							} 
							else
							{
								lockRect->bits[offset + 3] = mShadowMap[offsetMap].alpha		* 255;//a
							}
						}
					}
				}
			}
		}
	}
	tex4.unlock();
}

bool TerrainBlock::refreshLighttex()
{
	U32 tryCount = 0;
	while (++tryCount<10 && !tex3.getLegal());
	if (tryCount < 10)
	{
		GFXLockedRect *rt;
		rt = tex3.lock();
		U8 * lPtr = lightMap->getAddress(0,0);
		for (U32 i=0; i<LightmapSize*LightmapSize; i++)
		{
			rt->bits[i*4 + 2] = lPtr[i*4 + 0];
			rt->bits[i*4 + 1] = lPtr[i*4 + 1];
			rt->bits[i*4 + 0] = lPtr[i*4 + 2];
		}
		tex3.unlock();
		return true;
	}

	return false;
}

bool TerrainBlock::refreshLightmap()
{
	SimTime tBegin = Platform::getRealMilliseconds();
	bool success = false;
	SimTime tMid = 0;
	U32 tryCount = 0;

	while (++tryCount<10 && !tex3.getLegal());
	if (tryCount < 10)
	{
		GFXLockedRect *rt;
		rt = tex3.lock();
		U8 * lPtr = lightMap->getAddress(0,0);
		for (U32 i=0; i<1024*1024; i++)
		{
			lPtr[i*4 + 0] = rt->bits[i*4 + 2];
			lPtr[i*4 + 1] = rt->bits[i*4 + 1];
			lPtr[i*4 + 2] = rt->bits[i*4 + 0];
		}
		tex3.unlock();

		if (!isTiling())
		{
			U32 height = lightMap->getHeight();
			for (S32 y = 0; y < height; y++)
			{
				ColorI c;
				if (getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_RIGHT])
				{
					lightMap->getColor(lightMap->getWidth()-1,y,c);
					lightMap->setColor(0,y,c);
					lightMap->setColor(1,y,c);
				}
				else
				{
					lightMap->getColor(0,y,c);
					lightMap->setColor(lightMap->getWidth()-1,y,c);
					lightMap->setColor(lightMap->getWidth()-2,y,c);
				}
			}
			for (S32 x = 0; x < height; x++)
			{
				ColorI c;
				if (getFile()->mEdgeTerrainFiles[TerrainBlock::EDGE_BOTTOM])
				{
					lightMap->getColor(x, lightMap->getHeight()-1,c);
					lightMap->setColor(x,0,c);
					lightMap->setColor(x,1,c);
				}
				else
				{
					lightMap->getColor(x,0,c);
					lightMap->setColor(x, lightMap->getHeight()-1,c);
					lightMap->setColor(x, lightMap->getHeight()-2,c);
				}
			}
		}

		tMid = Platform::getRealMilliseconds();
		success = true;

	}

	SimTime tEnd = Platform::getRealMilliseconds();

	if (success)
	{
		SimpleQueryList objects;
		gClientContainer.findObjects(StaticTSObjectType|ShapeBaseObjectType, &SimpleQueryList::insertionCallback, &objects);

		for(SceneObject ** itr = objects.mList.begin(); itr != objects.mList.end(); itr++)
		{
			SceneObject* obj = *itr;
			if (obj)
			{
				sgSceneObjectLightingPlugin* lightPlugin = dynamic_cast<sgSceneObjectLightingPlugin*>(obj->getLightingPlugin());
				AssertFatal(lightPlugin, "No lighting plug-in available!");
				if (lightPlugin)
				{
					lightPlugin->mLightingInfo.mInit = false;
				}
			}
		}
		Con::printf("elapsed time : tEnd-tBegin = %d, tMid-tBegin = %d", tEnd-tBegin, tMid-tBegin);

	}
	Con::printf("tryCount = %d", tryCount);
	return success;
}


void TerrainBlock::init()
{
	if (mSetShader1SB == NULL)
	{
		mSetShader1SB = new GFXD3D9StateBlock;
		mSetShader1SB->registerResourceWithDevice(GFX);
		mSetShader1SB->mResurrect = &resetStateBlock;
		mSetShader1SB->mZombify = &releaseStateBlock;

		mSetShader2SB = new GFXD3D9StateBlock;
		mClearSB = new GFXD3D9StateBlock;
		mCullNoneSB = new GFXD3D9StateBlock;
		mCullCWSB = new GFXD3D9StateBlock;
		mCullCCWSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void TerrainBlock::shutdown()
{
	SAFE_DELETE(mSetShader1SB);
	SAFE_DELETE(mSetShader2SB);
	SAFE_DELETE(mClearSB);
	SAFE_DELETE(mCullNoneSB);
	SAFE_DELETE(mCullCWSB);
	SAFE_DELETE(mCullCCWSB);
}

void TerrainBlock::resetStateBlock()
{
	//mSetShader1SB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);

	for (U32 i=0; i<2; i++)
	{
		GFX->setSamplerState(i, GFXSAMPMagFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMinFilter, GFXTextureFilterLinear);
		GFX->setSamplerState(i, GFXSAMPMipFilter, GFXTextureFilterLinear);
	}
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressClamp);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPSelectARG1);
	GFX->setTextureStageState(0, GFXTSSColorArg1, GFXTATexture);

	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mSetShader1SB);

	//mSetShader2SB
	GFX->beginStateBlock();
	GFX->setSamplerState(1, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(1, GFXSAMPAddressV, GFXAddressWrap);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPModulate);
	GFX->endStateBlock(mSetShader2SB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->setRenderState(GFXRSZEnable, true);
	//GFX->setRenderState(GFXRSAlphaTestEnable, false);
	//GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mClearSB);

	//mCullNoneSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mCullNoneSB);
	//mCullStoreSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCW);
	GFX->endStateBlock(mCullCWSB);
	//mCullCCWSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
	GFX->endStateBlock(mCullCCWSB);
}

void TerrainBlock::releaseStateBlock()
{
	if (mSetShader1SB)
	{
		mSetShader1SB->release();
	}

	if (mSetShader2SB)
	{
		mSetShader2SB->release();
	}

	if (mClearSB)
	{
		mClearSB->release();
	}

	if (mCullNoneSB)
	{
		mCullNoneSB->release();
	}

	if (mCullCWSB)
	{
		mCullCWSB->release();
	}

	if (mCullCCWSB)
	{
		mCullCCWSB->release();
	}
}

Point3F TerrainBlock::getSubBlockOffset( U8 index )
{
	Point3F pos = __super::getPosition();
	Point3F offset;
	U32 x = index & ChunkShiftMask;
	U32 y = (index>>ChunkDownShift)&ChunkShiftMask;
	//offset.x = pos.x + x * ChunkSquareWidth * mSquareSize;
	//offset.y = pos.y + y * ChunkSquareWidth * mSquareSize;
	offset.x = x * ChunkSquareWidth * mSquareSize;
	offset.y = y * ChunkSquareWidth * mSquareSize;
	return offset;
}

void TerrainBlock::renderSubTerrEdge()
{
#pragma message(ENGINE(渲染地形子块边界标志))
	GFXCullMode oldCull = GFX->getCullMode();
	GFX->setCullMode( GFXCullNone );
	GFX->setAlphaBlendEnable( true );
	GFX->setSrcBlend( GFXBlendSrcAlpha );
	GFX->setDestBlend( GFXBlendInvSrcAlpha );
	bool bZW = GFX->getRenderState(GFXRSZWriteEnable);
	GFX->setZWriteEnable(false);
	
	Point3F minPt(0, 0, 0);//__super::getPosition();
	Point3F maxPt(TerrainBlock::BlockSize*2, TerrainBlock::BlockSize*2, 0);
	//maxPt.set(minPt.x + TerrainBlock::BlockSize*2, minPt.y + TerrainBlock::BlockSize*2, 0);
	F32 deltaX = (maxPt.x - minPt.x)/TerrainBlock::ChunkSize;
	F32 deltaY = (maxPt.x - minPt.x)/TerrainBlock::ChunkSize;
	const U32 verts = 4;
	GFXPrimitiveType primType = GFXTriangleFan;
	PrimBuild::color( mEdgeColor );
	GridSquare * gs = findSquare(TerrainBlock::BlockShift, Point2I(0,0));
	F32 height = F32(gs->maxHeight) * 0.03125f + 10.f;
	for (U32 i=0; i<=TerrainBlock::ChunkSize; i++)
	{
		//横
		PrimBuild::begin( primType, verts );
		PrimBuild::vertex3f( minPt.x + i*deltaX, minPt.y, 0.f );
		PrimBuild::vertex3f( minPt.x + i*deltaX, maxPt.y, 0.f );
		PrimBuild::vertex3f( minPt.x + i*deltaX, maxPt.y, height ); 
		PrimBuild::vertex3f( minPt.x + i*deltaX, minPt.y, height );
		PrimBuild::end();
		//竖
		PrimBuild::begin( primType, verts );
		PrimBuild::vertex3f(minPt.x, minPt.y + i*deltaY,  0.f );
		PrimBuild::vertex3f(maxPt.x, minPt.y + i*deltaY,  0.f );
		PrimBuild::vertex3f(maxPt.x, minPt.y + i*deltaY,  height ); 
		PrimBuild::vertex3f(minPt.x, minPt.y + i*deltaY,  height );
		PrimBuild::end();
	}

	GFX->setAlphaBlendEnable( false );
	GFX->setCullMode( oldCull );
	GFX->setZWriteEnable(bZW);
}

