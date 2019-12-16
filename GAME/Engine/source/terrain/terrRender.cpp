//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/consoleTypes.h"
#include "core/frameAllocator.h"
#include "platform/profiler.h"
#include "math/mMath.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sgUtil.h"
#include "gfx/gfxDevice.h"
#include "gfx/gBitmap.h"
#include "gfx/gfxCanon.h"
#include "materials/matInstance.h"
#include "terrain/terrData.h"
#include "terrain/sky.h"
#include "terrain/terrRender.h"
#include "terrain/terrBatch.h"
#include "terrain/waterBlock.h"
#include "gfx/gfxCardProfile.h"
#ifdef NTJ_EDITOR
#include "gui/missionEditor/terrainEditor.h"
#endif
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* TerrainRender::mSetSB = NULL;
GFXStateBlock* TerrainRender::mAlphaBlendFalseSB = NULL;
GFXStateBlock* TerrainRender::mSetBinFixSB = NULL;
GFXStateBlock* TerrainRender::mClearBinFixSB = NULL;
GFXStateBlock* TerrainRender::mVColorFalseSB = NULL;
GFXStateBlock* TerrainRender::mSetBiasSB = NULL;
GFXStateBlock* TerrainRender::mSetDetailShaderSB = NULL;
GFXStateBlock* TerrainRender::mDetailFixLerpSB = NULL;
GFXStateBlock* TerrainRender::mDetailFixForStageSB = NULL;
GFXStateBlock* TerrainRender::mClearDetailFixSB = NULL;
GFXStateBlock* TerrainRender::mSetLitSB = NULL;
GFXStateBlock* TerrainRender::mLitFogSB = NULL;
GFXStateBlock* TerrainRender::mLitFixSB = NULL;
GFXStateBlock* TerrainRender::mLitShaderSB = NULL;
GFXStateBlock* TerrainRender::mLitShaderAddrSB = NULL;
GFXStateBlock* TerrainRender::mFogSetSB = NULL;
GFXStateBlock* TerrainRender::mFogClearSB = NULL;

MatrixF     TerrainRender::mCameraToObject;
SceneState* TerrainRender::mSceneState;

S32 TerrainRender::mSquareSize;
F32 TerrainRender::mScreenSize;
F32 TerrainRender::mPixelError;
F32 TerrainRender::mScreenError;
F32 TerrainRender::mMinSquareSize;
F32 TerrainRender::mFarDistance;

TerrainBlock* TerrainRender::mCurrentBlock = NULL;
Point2F       TerrainRender::mBlockPos;
Point2I       TerrainRender::mBlockOffset;
Point2I       TerrainRender::mTerrainOffset;
PlaneF        TerrainRender::mClipPlane[MaxClipPlanes];
U32           TerrainRender::mNumClipPlanes = 4;
Point3F       TerrainRender::mCamPos;

TerrainLightingPlugin* TerrainRender::mTerrainLighting = NULL;
U32           TerrainRender::mDynamicLightCount;
bool          TerrainRender::mEnableTerrainDynLights     = true;
TerrLightInfo TerrainRender::mTerrainLights[MaxTerrainLights];

bool TerrainRender::mRenderingCommander = false;


void TerrainRender::init()
{
   mScreenError   = 4;
   mScreenSize    = 45;
   mMinSquareSize = 4;
   mFarDistance   = 500;

   Con::addVariable("screenSize", TypeF32, &mScreenSize);
   Con::addVariable("farDistance", TypeF32, &mFarDistance);

   Con::addVariable("inCommanderMap", TypeBool, &mRenderingCommander);

   Con::addVariable("pref::Terrain::dynamicLights", TypeBool, &mEnableTerrainDynLights);
   Con::addVariable("pref::Terrain::screenError", TypeF32, &mScreenError);

   TerrBatch::init();
}

void TerrainRender::shutdown()
{
	TerrBatch::clear();
}

void TerrainRender::buildClippingPlanes(bool flipClipPlanes)
{
   PROFILE_START(TerrainRender_buildClippingPlanes);

   F32 frustumParam[6];
   F64 realfrustumParam[6];
   
   GFX->getFrustum(&frustumParam[0], &frustumParam[1],
      &frustumParam[2], &frustumParam[3],
      &frustumParam[4], &frustumParam[5]);


   // Cast se can put them in the sg call.
   for(U32 i=0; i<6; i++)
      realfrustumParam[i] = frustumParam[i];

   Point3F osCamPoint(0, 0, 0);
   mCameraToObject.mulP(osCamPoint);
   sgComputeOSFrustumPlanes(realfrustumParam,
      mCameraToObject,
      osCamPoint,
      mClipPlane[4],
      mClipPlane[0],
      mClipPlane[1],
      mClipPlane[2],
      mClipPlane[3]);

   // no need
   mNumClipPlanes = 4;

   // near plane is needed as well...
   //PlaneF p(0, 1, 0, -frustumParam[4]);
   //mTransformPlane(mCameraToObject, Point3F(1,1,1), p, &mClipPlane[0]);

   if (flipClipPlanes) {
      mClipPlane[0].neg();
      mClipPlane[1].neg();
      mClipPlane[2].neg();
      mClipPlane[3].neg();
      mClipPlane[4].neg();
      mClipPlane[5].neg();
   }

   PROFILE_END(TerrainRender_buildClippingPlanes);
}

S32 TerrainRender::testSquareVisibility(Point3F &min, Point3F &max, S32 mask, F32 expand)
{
   PROFILE_SCOPE(TerrainRender_testSquareVisibility);
   S32 retMask = 0;
   Point3F minPoint, maxPoint;
   for(U32 i = 0; i < mNumClipPlanes; i++)
   {
      if(mask & (1 << i))
      {
         if(mClipPlane[i].x > 0)
         {
            maxPoint.x = max.x;
            minPoint.x = min.x;
         }
         else
         {
            maxPoint.x = min.x;
            minPoint.x = max.x;
         }
         if(mClipPlane[i].y > 0)
         {
            maxPoint.y = max.y;
            minPoint.y = min.y;
         }
         else
         {
            maxPoint.y = min.y;
            minPoint.y = max.y;
         }
         if(mClipPlane[i].z > 0)
         {
            maxPoint.z = max.z;
            minPoint.z = min.z;
         }
         else
         {
            maxPoint.z = min.z;
            minPoint.z = max.z;
         }
         F32 maxDot = mDot(maxPoint, mClipPlane[i]);
         F32 minDot = mDot(minPoint, mClipPlane[i]);
         F32 planeD = mClipPlane[i].d;
         if(maxDot <= -(planeD + expand))
         {
            return -1;
         }
         if(minDot <= -planeD)
            retMask |= (1 << i);
      }
   }
   return retMask;
}

ChunkCornerPoint *TerrainRender::allocInitialPoint(Point3F pos)
{
   ChunkCornerPoint *ret = (ChunkCornerPoint *) FrameAllocator::alloc(sizeof(ChunkCornerPoint));
   ret->x = pos.x;
   ret->y = pos.y;
   ret->z = fixedToFloat(mCurrentBlock->getHeight(pos.x/mCurrentBlock->getSquareSize(), pos.y/mCurrentBlock->getSquareSize()));
   ret->xfIndex = 0;
   
   return ret;
}

ChunkCornerPoint *TerrainRender::allocPoint(Point2I pos)
{
   ChunkCornerPoint *ret = (ChunkCornerPoint *) FrameAllocator::alloc(sizeof(ChunkCornerPoint));
   ret->x = pos.x * mSquareSize + mBlockPos.x;
   ret->y = pos.y * mSquareSize + mBlockPos.y;
   ret->z = fixedToFloat(mCurrentBlock->getHeight(pos.x, pos.y));
   ret->xfIndex = 0;

   return ret;
}

void TerrainRender::allocRenderEdges(U32 edgeCount, EdgeParent **dest, bool renderEdge)
{
   if(renderEdge)
   {
      for(U32 i = 0; i < edgeCount; i++)
      {
         ChunkEdge *edge = (ChunkEdge *) FrameAllocator::alloc(sizeof(ChunkEdge));
         edge->c1 = NULL;
         edge->c2 = NULL;
         edge->xfIndex = 0;
         edge->pointCount = 0;
         edge->pointIndex = 0;
         dest[i] = edge;
      }
   }
   else
   {
      for(U32 i = 0; i < edgeCount; i++)
      {
         ChunkScanEdge *edge = (ChunkScanEdge *) FrameAllocator::alloc(sizeof(ChunkScanEdge));
         edge->mp = NULL;
         dest[i] = edge;
      }
   }
}

void TerrainRender::subdivideChunkEdge(ChunkScanEdge *e, Point2I pos, bool chunkEdge)
{
   if(!e->mp)
   {
      allocRenderEdges(2, &e->e1, chunkEdge);
      e->mp = allocPoint(pos);

      e->e1->p1 = e->p1;
      e->e1->p2 = e->mp;
      e->e2->p1 = e->mp;
      e->e2->p2 = e->p2;
   }
}

F32 TerrainRender::getSquareDistance(const Point3F& minPoint, const Point3F& maxPoint, F32* zDiff)
{
   Point3F vec;
   if(mCamPos.z < minPoint.z)
      vec.z = minPoint.z - mCamPos.z;
   else if(mCamPos.z > maxPoint.z)
      vec.z = maxPoint.z - mCamPos.z;
   else
      vec.z = 0;

   if(mCamPos.x < minPoint.x)
      vec.x = minPoint.x - mCamPos.x;
   else if(mCamPos.x > maxPoint.x)
      vec.x = mCamPos.x - maxPoint.x;
   else
      vec.x = 0;

   if(mCamPos.y < minPoint.y)
      vec.y = minPoint.y - mCamPos.y;
   else if(mCamPos.y > maxPoint.y)
      vec.y = mCamPos.y - maxPoint.y;
   else
      vec.y = 0;

   // zDiff calculation yonked from AtlasGeomChunk::calculatePoints -- BTR
   if (mCamPos.z <= minPoint.z)
   {
      *zDiff = maxPoint.z - mCamPos.z;
   } else {
      if (mCamPos.z > maxPoint.z)
      {
         *zDiff = minPoint.z - mCamPos.z;
      } else {
         F32 zCenter = (maxPoint.z + minPoint.z) / 2.0f;
         if (mCamPos.z > zCenter)
            *zDiff = minPoint.z - mCamPos.z;
         else
            *zDiff = maxPoint.z - mCamPos.z;
      }
   }

   return vec.len();
}

struct BatchHelper
{
   typedef Vector<EmitChunk*> ChunkList;
   ChunkList mLightPasses[MaxTerrainLights][16];
   ChunkList mDiffusePasses[256], mFog, mDetail;
	ChunkList mDifPass[TerrainBlock::ChunkSize*TerrainBlock::ChunkSize];
	ChunkList mDif;
	ChunkList mLit[MaxTerrainLights];
   void reset()
   {
      for(S32 i=0; i<MaxTerrainLights; i++)
         for(S32 j=0; j<16; j++)
            mLightPasses[i][j].clear();

      for(S32 i=0; i<256; i++)
         mDiffusePasses[i].clear();

		for(S32 i=0; i<TerrainBlock::ChunkSize * TerrainBlock::ChunkSize; i++)
			mDifPass[i].clear();

      mFog.clear();
      mDetail.clear();
		mDif.clear();
		for(S32 i=0; i<MaxTerrainLights; i++)
			mLit[i].clear();
   }

   BatchHelper()
   {
      reset();
   }

   ~BatchHelper()
   {
   }

   void batchChunk(EmitChunk *ec, const F32 squareDistance, F32 zDelta)
   {
      // Figure out what level range is needed for this chunk, and put it
      // on the right list.

      const F32 blockSize = TerrainRender::mSquareSize * F32(TerrainBlock::BlockSize);

      // What will be the bounds of this chunk?
      RectF texBounds;
      texBounds.point.x = (F32(ec->x) * TerrainRender::mSquareSize) / blockSize;
      texBounds.point.y = (F32(ec->y) * TerrainRender::mSquareSize) / blockSize;
      texBounds.extent.x = (4.f * TerrainRender::mSquareSize) / blockSize;
      texBounds.extent.y = (4.f * TerrainRender::mSquareSize) / blockSize;

      // Get the near/far distances for this chunk.
      F32 fnear, dfar;
      fnear = squareDistance;
      dfar = squareDistance + (4.f * TerrainRender::mSquareSize);

#ifdef USE_CLIPMAP
      S32 startLevel, endLevel;
      TerrainRender::mCurrentBlock->mClipMap->calculateClipMapLevels
                              (fnear, dfar, texBounds, startLevel, endLevel);

      // Great - now encode levels and store into appropriate bin.
      U8 binCode = ((endLevel & 0xF)<<4) | (startLevel & 0xF);
      mDiffusePasses[binCode].push_back(ec);
#else
		U8 binId = (((ec->y/TerrainBlock::ChunkSquareWidth) & TerrainBlock::ChunkShiftMask)<<TerrainBlock::ChunkDownShift) | ((ec->x/TerrainBlock::ChunkSquareWidth) & TerrainBlock::ChunkShiftMask);
		mDifPass[binId].push_back(ec);
#endif
		mDif.push_back(ec);
      // Check if this chunk will get fogged - consider furthest point, and if
      // it'll be fogged then draw a fog pass.
      if(TerrainRender::mSceneState->getHazeAndFog(dfar, zDelta) > (1.f/256.f) 
         || TerrainRender::mSceneState->getHazeAndFog(dfar, -zDelta) > (1.f/256.f))
         mFog.push_back(ec);

      // Also, check for detail.
      if(fnear < TerrainRender::mCurrentBlock->mDetailDistance)
         mDetail.push_back(ec);

      // Add this to the right light lists if it has lighting.
#ifdef USE_CLIPMAP
      if(ec->lightMask)
      {
         for(S32 i=0; i<MaxTerrainLights; i++)
            if(ec->lightMask & BIT(i))
               mLightPasses[i][endLevel & 15].push_back(ec);
      }
#else
		if(ec->lightMask)
		{
			//for(S32 i=0; i<MaxTerrainLights; i++)
			//	if(ec->lightMask & BIT(i))
			//		mLightPasses[i][binId & 15].push_back(ec);

			for(S32 i=0; i<MaxTerrainLights; i++)
				if(ec->lightMask & BIT(i))
					mLit[i].push_back(ec);
		}
#endif
   }

   void renderChunkList(ChunkList &cl, MatInstance *m, SceneGraphData &sgData, TerrBatch::vertexType vtype, Point4F *clipmapMapping, const MatrixF &blockTransform, const Point3F &cameraPosition, LightInfo *light)
   {
      for(S32 i=0; i<cl.size(); i++)
      {
         EmitChunk *sq = cl[i];
         TerrBatch::begin();

         // Emit the appropriate geometry for our rendering mode...
         if(TerrainRender::mRenderingCommander)
            TerrainRender::renderChunkCommander(sq);
         else
            TerrainRender::renderChunkNormal(sq);

         TerrBatch::end(m, sgData, vtype, clipmapMapping, false, blockTransform, cameraPosition, light);
      }

      // force batch to flush
      TerrBatch::end(m, sgData, vtype, clipmapMapping, true, blockTransform, cameraPosition, light);
   }

} gChunkBatcher;

void TerrainRender::emitTerrChunk(SquareStackNode *n, F32 squareDistance, U32 lightMask, F32 zDelta, F32 squareDistanceLOD)
{
   GridChunk *gc = mCurrentBlock->findChunk(n->pos);

   EmitChunk *chunk = (EmitChunk *) FrameAllocator::alloc(sizeof(EmitChunk));
   chunk->x = n->pos.x + mBlockOffset.x + mTerrainOffset.x;
   chunk->y = n->pos.y + mBlockOffset.y + mTerrainOffset.y;
   chunk->lightMask = lightMask;

   gChunkBatcher.batchChunk(chunk, squareDistance, zDelta);

   if(mRenderingCommander)
      return;

   chunk->edge[0] = (ChunkEdge *) n->top;
   chunk->edge[1] = (ChunkEdge *) n->right;
   chunk->edge[2] = (ChunkEdge *) n->bottom;
   chunk->edge[3] = (ChunkEdge *) n->left;

   chunk->edge[0]->c2 = chunk;
   chunk->edge[1]->c1 = chunk;
   chunk->edge[2]->c1 = chunk;
   chunk->edge[3]->c2 = chunk;

   // holes only in the primary terrain block
   if (gc->emptyFlags && mBlockPos.x == 0 && mBlockPos.y == 0)
      chunk->emptyFlags = gc->emptyFlags;
   else
      chunk->emptyFlags = 0;

   S32 subDivLevel;
   F32 growFactor = 0;

   F32 minSubdivideDistance = 1000000;

   if(squareDistance < 1 || squareDistanceLOD < 1)
      subDivLevel = -1;
   else
   {
      for(subDivLevel = 2; subDivLevel >= 0; subDivLevel--)
      {
         F32 subdivideDistance = fixedToFloat(gc->heightDeviance[subDivLevel]) / mPixelError;
         if(subdivideDistance > minSubdivideDistance)
            subdivideDistance = minSubdivideDistance;

         if(squareDistance >= subdivideDistance)
            break;

         F32 clampDistance = subdivideDistance * 0.75f;

         if(squareDistance > clampDistance)
         {
            growFactor = (squareDistance - clampDistance) / (0.25f * subdivideDistance);
            subDivLevel--;
            break;
         }

         minSubdivideDistance = clampDistance;
      }
   }
   chunk->subDivLevel = subDivLevel;
   chunk->growFactor = growFactor;
}

void TerrainRender::processBlock(SceneState*, EdgeParent *topEdge, EdgeParent *rightEdge, EdgeParent *bottomEdge, EdgeParent *leftEdge)
{
   SquareStackNode stack[TerrainBlock::BlockShift*4];
   Point3F minPoint, maxPoint;

   // Set up the root node of the stack...
   stack[0].level        = TerrainBlock::BlockShift;
   stack[0].clipFlags    = (BIT(mNumClipPlanes) - 1) | FarSphereMask;  // test all the planes
   stack[0].pos.set(0,0);
   stack[0].top          = topEdge;
   stack[0].right        = rightEdge;
   stack[0].bottom       = bottomEdge;
   stack[0].left         = leftEdge;
   stack[0].lightMask    = BIT(mDynamicLightCount) - 1; // test all the lights

   // Set up fog...
   Vector<SceneState::FogBand> *posFog = mSceneState->getPosFogBands();
   Vector<SceneState::FogBand> *negFog = mSceneState->getNegFogBands();
   bool clipAbove = posFog->size() > 0 && (*posFog)[0].isFog == false;
   bool clipBelow = negFog->size() > 0 && (*negFog)[0].isFog == false;
   bool clipOn    = posFog->size() > 0 && (*posFog)[0].isFog == true;

   if(posFog->size() != 0 || negFog->size() != 0)
      stack[0].clipFlags |= FogPlaneBoxMask;

   // Initialize the stack
   S32 curStackSize = 1;
   F32 squareDistance;

   while(curStackSize)
   {
      SquareStackNode *n = stack + curStackSize - 1;

      // see if it's visible
      GridSquare *sq = mCurrentBlock->findSquare(n->level, n->pos);

      // Calculate bounding points...
      minPoint.set(mSquareSize * n->pos.x + mBlockPos.x,
                   mSquareSize * n->pos.y + mBlockPos.y,
                   fixedToFloat(sq->minHeight));
      maxPoint.set(minPoint.x + (mSquareSize << n->level),
                   minPoint.y + (mSquareSize << n->level),
                   fixedToFloat(sq->maxHeight));

      // holes only in the primary terrain block
      if ((sq->flags & GridSquare::Empty) && mBlockPos.x == 0 && mBlockPos.y == 0)
      {
         curStackSize--;
         continue;
      }

      F32 zDiff;
      squareDistance = getSquareDistance(minPoint, maxPoint, &zDiff);

      S32 nextClipFlags = 0;

      if(n->clipFlags)
      {
         if(n->clipFlags & FogPlaneBoxMask)
         {
            F32 camZ = mCamPos.z;
            bool boxBelow = camZ > maxPoint.z;
            bool boxAbove = camZ < minPoint.z;
            bool boxOn = !(boxAbove || boxBelow);
            if( clipOn ||
               (clipAbove && boxAbove && (maxPoint.z - camZ > (*posFog)[0].cap)) ||
               (clipBelow && boxBelow && (camZ - minPoint.z > (*negFog)[0].cap)) ||
               (boxOn && (( clipAbove && maxPoint.z - camZ > (*posFog)[0].cap ) ||
               ( clipBelow && camZ - minPoint.z > (*negFog)[0].cap ))))
            {
               // Using the fxSunLight can cause the "sky" to extend down below the camera.
               // To avoid the sun showing through, the terrain must always be rendered.
               // If the fxSunLight is not being used, the following optimization can be
               // uncommented.
#if 0
               if(boxBelow && !mSceneState->isBoxFogVisible(squareDistance, maxPoint.z, minPoint.z))
               {
                  // Totally fogged terrain tiles can be thrown out as long as they are
                  // below the camera. If they are above, the sky will show through the
                  // fog.
                  curStackSize--;
                  continue;
               }
#endif
               nextClipFlags |= FogPlaneBoxMask;
            }
         }

         if(n->clipFlags & FarSphereMask)
         {
            // Reject stuff outside our visible range...
            if(squareDistance >= mFarDistance)
            {
               curStackSize--;
               continue;
            }

            // Otherwise clip againse the bounding sphere...
            S32 dblSquareSz = mSquareSize << (n->level+1);

            // We add a "fudge factor" to get the furthest possible point of the square...
            if(squareDistance + (maxPoint.z - minPoint.z) + dblSquareSz > mFarDistance)
               nextClipFlags |= FarSphereMask;
         }

         
         // zDelta for screen error height deviance.
         F32 zDelta = squareDistance * mPixelError;
         minPoint.z -= zDelta;
         maxPoint.z += zDelta;

         nextClipFlags |= testSquareVisibility(minPoint, maxPoint, n->clipFlags, F32(mSquareSize));
         if(nextClipFlags == -1)
         {
            // trivially rejected, so pop it off the stack
            curStackSize--;
            continue;
         }
      }
      // If we have been marked as lit we need to see if that still holds true...
      if(n->lightMask)
         n->lightMask = testSquareLights(sq, n->level, n->pos, n->lightMask);

      if(n->level == 2)
      {
         // We've tessellated far enough, stop here.

         F32 squareDistanceLOD = squareDistance;

         if (!mCurrentBlock->isTiling())
         {
            // Lock border LOD so no gaps
            if (n->pos.x == 0 || n->pos.x >= (TerrainBlock::BlockSize-4))
               squareDistanceLOD = 0.5f;
       
            if (n->pos.y == 0 || n->pos.y >= (TerrainBlock::BlockSize-4))
               squareDistanceLOD = 0.5f;         
         }

         emitTerrChunk(n, squareDistance, n->lightMask, zDiff, squareDistanceLOD);
         curStackSize--;
         continue;
      }

      // Ok, we have to tessellate further down...
      bool allocChunkEdges = (n->level == 3);

      Point2I pos = n->pos;

      ChunkScanEdge *top    = (ChunkScanEdge *) n->top;
      ChunkScanEdge *right  = (ChunkScanEdge *) n->right;
      ChunkScanEdge *bottom = (ChunkScanEdge *) n->bottom;
      ChunkScanEdge *left   = (ChunkScanEdge *) n->left;

      // subdivide this square and throw it on the stack
      S32 squareOneSize  = 1 << n->level;
      S32 squareHalfSize = squareOneSize >> 1;

      ChunkCornerPoint *midPoint = allocPoint(Point2I(pos.x + squareHalfSize, pos.y + squareHalfSize));
      S32 nextLevel = n->level - 1;

      subdivideChunkEdge(top,    Point2I(pos.x + squareHalfSize, pos.y + squareOneSize),  allocChunkEdges);
      subdivideChunkEdge(right,  Point2I(pos.x + squareOneSize,  pos.y + squareHalfSize), allocChunkEdges);
      subdivideChunkEdge(bottom, Point2I(pos.x + squareHalfSize, pos.y),                  allocChunkEdges);
      subdivideChunkEdge(left,   Point2I(pos.x,                  pos.y + squareHalfSize), allocChunkEdges);

      // cross edges go top, right, bottom, left
      EdgeParent *crossEdges[4];
      allocRenderEdges(4, crossEdges, allocChunkEdges);
      crossEdges[0]->p1 = top->mp;
      crossEdges[0]->p2 = midPoint;
      crossEdges[1]->p1 = midPoint;
      crossEdges[1]->p2 = right->mp;
      crossEdges[2]->p1 = midPoint;
      crossEdges[2]->p2 = bottom->mp;
      crossEdges[3]->p1 = left->mp;
      crossEdges[3]->p2 = midPoint;

      n->level     = nextLevel;
      n->clipFlags = nextClipFlags;

      // Set up data for our sub-squares
      for(S32 i = 1; i < 4; i++)
      {
         n[i].level        = nextLevel;
         n[i].clipFlags    = nextClipFlags;
         n[i].lightMask    = n->lightMask;
      }

      // push in reverse order of processing.
      n[3].pos = pos;
      n[3].top = crossEdges[3];
      n[3].right = crossEdges[2];
      n[3].bottom = bottom->e1;
      n[3].left = left->e2;

      n[2].pos.set(pos.x + squareHalfSize, pos.y);
      n[2].top = crossEdges[1];
      n[2].right = right->e2;
      n[2].bottom = bottom->e2;
      n[2].left = crossEdges[2];

      n[1].pos.set(pos.x, pos.y + squareHalfSize);
      n[1].top = top->e1;
      n[1].right = crossEdges[0];
      n[1].bottom = crossEdges[3];
      n[1].left = left->e1;

      n[0].pos.set(pos.x + squareHalfSize, pos.y + squareHalfSize);
      n[0].top = top->e2;
      n[0].right = right->e1;
      n[0].bottom = crossEdges[1];
      n[0].left = crossEdges[0];

      curStackSize += 3;
   }
}

//---------------------------------------------------------------
// Root block render function
//---------------------------------------------------------------
void TerrainRender::fixEdge(ChunkEdge *edge, S32 x, S32 y, S32 dx, S32 dy)
{
   PROFILE_SCOPE(TR_fixEdge);

   S32 minLevel, maxLevel;
   F32 growFactor;

   if(edge->c1)
   {
      minLevel = edge->c1->subDivLevel;
      maxLevel = edge->c1->subDivLevel;
      growFactor = edge->c1->growFactor;
      if(edge->c2)
      {
         if(edge->c2->subDivLevel < minLevel)
            minLevel = edge->c2->subDivLevel;
         else if(edge->c2->subDivLevel > maxLevel)
         {
            maxLevel = edge->c2->subDivLevel;
            growFactor = edge->c2->growFactor;
         }
         else if(edge->c2->growFactor > growFactor)
            growFactor = edge->c2->growFactor;
      }
   }
   else
   {
      minLevel = maxLevel = edge->c2->subDivLevel;
      growFactor = edge->c2->growFactor;
   }
   if(minLevel == 2)
   {
      edge->pointCount = 0;
      return;
   }

   // get the mid heights
   EdgePoint *pmid = &edge->pt[1];
   ChunkCornerPoint *p1 = edge->p1;
   ChunkCornerPoint *p2 = edge->p2;

   pmid->x = (p1->x + p2->x) * 0.5f;
   pmid->y = (p1->y + p2->y) * 0.5f;

   if(maxLevel == 2)
   {
      // pure interp
      pmid->z = (p1->z + p2->z) * 0.5f;

      if(minLevel >= 0)
      {
         edge->pointCount = 1;
         return;
      }
   }
   else
   {
      pmid->z = fixedToFloat(mCurrentBlock->getHeight(x + dx + dx, y + dy + dy));
      if(maxLevel == 1)
         pmid->z = pmid->z + growFactor * (((p1->z + p2->z) * 0.5f) - pmid->z);

      if(minLevel >= 0)
      {
         edge->pointCount = 1;
         return;
      }
   }

   // last case - minLevel == -1, midPoint calc'd
   edge->pointCount = 3;
   EdgePoint *pm1 = &edge->pt[0];
   EdgePoint *pm2 = &edge->pt[2];

   pm1->x = (p1->x + pmid->x) * 0.5f;
   pm1->y = (p1->y + pmid->y) * 0.5f;
   pm2->x = (p2->x + pmid->x) * 0.5f;
   pm2->y = (p2->y + pmid->y) * 0.5f;

   if(maxLevel != -1)
   {
      pm1->z = (p1->z + pmid->z) * 0.5f;
      pm2->z = (p2->z + pmid->z) * 0.5f;
      return;
   }

   // compute the real deals:
   pm1->z = fixedToFloat(mCurrentBlock->getHeight(x + dx, y + dy));
   pm2->z = fixedToFloat(mCurrentBlock->getHeight(x + dx + dx + dx, y + dy + dy + dy));

   if(growFactor)
   {
      pm1->z = pm1->z + growFactor * (((p1->z + pmid->z) * 0.5f) - pm1->z);
      pm2->z = pm2->z + growFactor * (((p2->z + pmid->z) * 0.5f) - pm2->z);
   }
}

void TerrainRender::renderChunkCommander(EmitChunk *chunk)
{
   PROFILE_START(TerrainRender_renderChunkCommander);

   // Emit our points
   U32 ll = TerrBatch::mCurVertex;

   for(U32 y = 0; y <= 64; y += 4)                   // 16 steps
      for(U32 x = (y & 4) ? 4 : 0; x <= 64; x += 8)  // 8 steps
         TerrBatch::emit(chunk->x + x,chunk->y + y); // 128 vertices

   // Emit a mesh overlaying 'em (768 indices)
   for(U32 y = 0; y < 8; y++)
   {
      for(U32 x = 0; x < 8; x++)
      {
         TerrBatch::emitTriangle(ll + 9, ll,      ll + 17);
         TerrBatch::emitTriangle(ll + 9, ll + 17, ll + 18);
         TerrBatch::emitTriangle(ll + 9, ll + 18, ll + 1);
         TerrBatch::emitTriangle(ll + 9, ll + 1,  ll);
         ll++;
      }
      ll += 9;
   }

   PROFILE_END(TerrainRender_renderChunkCommander);
}

void TerrainRender::renderChunkNormal(EmitChunk *chunk)
{
   PROFILE_SCOPE(TerrainRender_renderChunkNormal);

   bool skipEdge = false;

   if (!mCurrentBlock->isTiling())
   {
      if (chunk->x + 4 >= TerrainBlock::BlockSize || chunk->y + 4 >= TerrainBlock::BlockSize)
         skipEdge = true;
   }

   // Emits a max of 96 indices
   // Emits a max of 25 vertices
   ChunkEdge *e0 = chunk->edge[0];
   ChunkEdge *e1 = chunk->edge[1];
   ChunkEdge *e2 = chunk->edge[2];
   ChunkEdge *e3 = chunk->edge[3];

   if(e0->xfIndex != TerrBatch::mCurXF)
   {
      if(!e0->xfIndex)
         fixEdge(e0, chunk->x, chunk->y + 4, 1, 0);
      TerrBatch::emit( *e0 );
   }

   if(e1->xfIndex != TerrBatch::mCurXF)
   {
      if(!e1->xfIndex)
         fixEdge(e1, chunk->x + 4, chunk->y + 4, 0, -1);
      TerrBatch::emit( *e1 );
   }

   if(e2->xfIndex != TerrBatch::mCurXF)
   {
      if(!e2->xfIndex)
         fixEdge(e2, chunk->x, chunk->y, 1, 0);
      TerrBatch::emit( *e2 );
   }

   if(e3->xfIndex != TerrBatch::mCurXF)
   {
      if(!e3->xfIndex)
         fixEdge(e3, chunk->x, chunk->y + 4, 0, -1);
      TerrBatch::emit( *e3 );
   }

   // Edges above, give us 12 verts
   // Corners below are 4 more
   U16 p0 = TerrBatch::emit(e0->p1);
   U16 p1 = TerrBatch::emit(e0->p2);
   U16 p2 = TerrBatch::emit(e2->p2);
   U16 p3 = TerrBatch::emit(e2->p1);
 
   // build the interior points (one more vert):
   U16 ip0 = TerrBatch::emit(chunk->x + 2, chunk->y + 2);
   F32 growFactor = chunk->growFactor;

   // So now we have a total of 17 verts
   if(chunk->subDivLevel >= 1)
   {
      // just emit the fan for the whole square:
      S32 i;

      if(e0->pointCount)
      {
         TerrBatch::emitTriangle(ip0, p0, e0->pointIndex);  // 3 indices
         for(i = 1; i < e0->pointCount; i++)
            TerrBatch::emitFanStep(e0->pointIndex + i);     // 9 i
         TerrBatch::emitFanStep(p1);                        // 3 i
      }
      else 
      {
         TerrBatch::emitTriangle(ip0, p0, p1);
      }

      for(i = 0; i < e1->pointCount; i++)
         TerrBatch::emitFanStep(e1->pointIndex + i);        // 9 indices

      TerrBatch::emitFanStep(p2);                           // 3 indices
      for(i = e2->pointCount - 1; i >= 0; i--)
         TerrBatch::emitFanStep(e2->pointIndex + i);        // 6 i

      TerrBatch::emitFanStep(p3);                           // 3 i
      for(i = e3->pointCount - 1; i >= 0; i--)
         TerrBatch::emitFanStep(e3->pointIndex + i);        // 6 i

      TerrBatch::emitFanStep(p0);                           // 3 i

      // Total indices for this path:                          42 indices
   }
   else
   {
      if(chunk->subDivLevel == 0)
      {
         // Add yet four verts more to the list! (now we're at 21)
         U32 ip1 = TerrBatch::emitInterp(p0, ip0, chunk->x + 1, chunk->y + 3, growFactor);
         U32 ip2 = TerrBatch::emitInterp(p1, ip0, chunk->x + 3, chunk->y + 3, growFactor);
         U32 ip3 = TerrBatch::emitInterp(p2, ip0, chunk->x + 3, chunk->y + 1, growFactor);
         U32 ip4 = TerrBatch::emitInterp(p3, ip0, chunk->x + 1, chunk->y + 1, growFactor);

         // emit the 4 fans:
         if((chunk->emptyFlags & CornerEmpty_0_1) != CornerEmpty_0_1)
         {

            TerrBatch::emitTriangle(ip1, p0, e0->pointIndex);        // 3

            if(e0->pointCount == 3)
               TerrBatch::emitFanStep (e0->pointIndex + 1);          // 3

            TerrBatch::emitFanStep (ip0);                            // 3

            if(e3->pointCount == 1)
               TerrBatch::emitFanStep ( e3->pointIndex );            
            else
            {
               TerrBatch::emitFanStep ( e3->pointIndex + 1 );        // 3
               TerrBatch::emitFanStep ( e3->pointIndex );            // 3
            }

            TerrBatch::emitFanStep ( p0 );                           // 3

            // Total emitted indices                                    18
         }

         if((chunk->emptyFlags & CornerEmpty_1_1) != CornerEmpty_1_1)
         {
            TerrBatch::emitTriangle( ip2, p1, e1->pointIndex );

            if(e1->pointCount == 3)
               TerrBatch::emitFanStep ( e1->pointIndex + 1 );

            TerrBatch::emitFanStep ( ip0 );

            if(e0->pointCount == 1)
               TerrBatch::emitFanStep( e0->pointIndex );
            else
            {
               TerrBatch::emitFanStep( e0->pointIndex + 1 );
               TerrBatch::emitFanStep( e0->pointIndex + 2 );
            }
            TerrBatch::emitFanStep( p1 );
         }

         if((chunk->emptyFlags & CornerEmpty_1_0) != CornerEmpty_1_0)
         {

            if(e2->pointCount == 1)
               TerrBatch::emitTriangle( ip3, p2, e2->pointIndex );
            else
            {
               TerrBatch::emitTriangle( ip3, p2, e2->pointIndex + 2 );
               TerrBatch::emitFanStep( e2->pointIndex + 1 );
            }

            TerrBatch::emitFanStep( ip0 );

            if(e1->pointCount == 1)
               TerrBatch::emitFanStep( e1->pointIndex );
            else
            {
               TerrBatch::emitFanStep( e1->pointIndex + 1 );
               TerrBatch::emitFanStep( e1->pointIndex + 2 );
            }
            TerrBatch::emitFanStep( p2 );
         }

         if((chunk->emptyFlags & CornerEmpty_0_0) != CornerEmpty_0_0)
         {

            if(e3->pointCount == 1)
               TerrBatch::emitTriangle( ip4, p3, e3->pointIndex);
            else
            {
               TerrBatch::emitTriangle( ip4, p3, e3->pointIndex + 2);
               TerrBatch::emitFanStep( e3->pointIndex + 1 );
            }
            TerrBatch::emitFanStep( ip0 );

            if(e2->pointCount == 1)
               TerrBatch::emitFanStep(  e2->pointIndex );
            else
            {
               TerrBatch::emitFanStep(  e2->pointIndex + 1 );
               TerrBatch::emitFanStep(  e2->pointIndex );
            }
            TerrBatch::emitFanStep( p3 );
         }

         // Four fans of 18 indices =                                  72 indices
      }
      else
      {
         // subDiv == -1
         U32 ip1 = TerrBatch::emit(chunk->x + 1, chunk->y + 3);
         U32 ip2 = TerrBatch::emit(chunk->x + 3, chunk->y + 3);
         U32 ip3 = TerrBatch::emit(chunk->x + 3, chunk->y + 1);
         U32 ip4 = TerrBatch::emit(chunk->x + 1, chunk->y + 1);
         U32 ip5 = TerrBatch::emitInterp(e0->pointIndex + 1, ip0, chunk->x + 2, chunk->y + 3, growFactor);
         U32 ip6 = TerrBatch::emitInterp(e1->pointIndex + 1, ip0, chunk->x + 3, chunk->y + 2, growFactor);
         U32 ip7 = TerrBatch::emitInterp(e2->pointIndex + 1, ip0, chunk->x + 2, chunk->y + 1, growFactor);
         U32 ip8 = TerrBatch::emitInterp(e3->pointIndex + 1, ip0, chunk->x + 1, chunk->y + 2, growFactor);

         // Or eight more points, in which case we hit 25 verts total..

         // now do the squares:
         if(chunk->emptyFlags & CornerEmpty_0_1)
         {
            if((chunk->emptyFlags & CornerEmpty_0_1) != CornerEmpty_0_1)
            {
               if(!(chunk->emptyFlags & SquareEmpty_0_3))
               {
                  TerrBatch::emitTriangle(ip1, e3->pointIndex, p0);
                  TerrBatch::emitTriangle(ip1, p0, e0->pointIndex);
               }
               if(!(chunk->emptyFlags & SquareEmpty_1_3))
               {
                  TerrBatch::emitTriangle(ip1, e0->pointIndex, e0->pointIndex + 1);
                  TerrBatch::emitTriangle(ip1, e0->pointIndex + 1, ip5);
               }
               if(!(chunk->emptyFlags & SquareEmpty_1_2))
               {
                  TerrBatch::emitTriangle(ip1, ip5, ip0);
                  TerrBatch::emitTriangle(ip1, ip0, ip8);
               }
               if(!(chunk->emptyFlags & SquareEmpty_0_2))
               {
                  TerrBatch::emitTriangle(ip1, ip8, e3->pointIndex + 1);
                  TerrBatch::emitTriangle(ip1, e3->pointIndex + 1, e3->pointIndex);
               }
            }
            // 24 indices on this path ^^^
         }
         else
         {
            TerrBatch::emitTriangle( ip1, p0, e0->pointIndex );
            TerrBatch::emitFanStep( e0->pointIndex + 1 );
            TerrBatch::emitFanStep( ip5 );
            TerrBatch::emitFanStep( ip0 );
            TerrBatch::emitFanStep( ip8 );
            TerrBatch::emitFanStep( e3->pointIndex + 1 );
            TerrBatch::emitFanStep( e3->pointIndex );
            TerrBatch::emitFanStep( p0 );
            // And 24 here.
         }

         if(chunk->emptyFlags & CornerEmpty_1_1)
         {
            if((chunk->emptyFlags & CornerEmpty_1_1) != CornerEmpty_1_1)
            {

               if(!(chunk->emptyFlags & SquareEmpty_3_3))
               {
                  TerrBatch::emitTriangle(ip2, e0->pointIndex + 2, p1);
                  TerrBatch::emitTriangle(ip2, p1, e1->pointIndex);
               }
               if(!(chunk->emptyFlags & SquareEmpty_3_2))
               {
                  TerrBatch::emitTriangle(ip2, e1->pointIndex, e1->pointIndex + 1);
                  TerrBatch::emitTriangle(ip2, e1->pointIndex + 1, ip6);
               }
               if(!(chunk->emptyFlags & SquareEmpty_2_2))
               {
                  TerrBatch::emitTriangle(ip2, ip6, ip0);
                  TerrBatch::emitTriangle(ip2, ip0, ip5);
               }
               if(!(chunk->emptyFlags & SquareEmpty_2_3))
               {
                  TerrBatch::emitTriangle(ip2, ip5, e0->pointIndex + 1);
                  TerrBatch::emitTriangle(ip2, e0->pointIndex + 1, e0->pointIndex + 2);
               }
            }
         }
         else
         {
            TerrBatch::emitTriangle( ip2, p1, e1->pointIndex );
            TerrBatch::emitFanStep ( e1->pointIndex + 1 );
            TerrBatch::emitFanStep ( ip6 );
            TerrBatch::emitFanStep ( ip0 );
            TerrBatch::emitFanStep ( ip5 );
            TerrBatch::emitFanStep ( e0->pointIndex + 1 );
            TerrBatch::emitFanStep ( e0->pointIndex + 2 );
            TerrBatch::emitFanStep ( p1 );
         }

         if(chunk->emptyFlags & CornerEmpty_1_0)
         {
            if((chunk->emptyFlags & CornerEmpty_1_0) != CornerEmpty_1_0)
            {

               if(!(chunk->emptyFlags & SquareEmpty_3_0))
               {
                  TerrBatch::emitTriangle (ip3, e1->pointIndex + 2, p2);
                  TerrBatch::emitTriangle (ip3, p2, e2->pointIndex + 2);
               }
               if(!(chunk->emptyFlags & SquareEmpty_2_0))
               {
                  TerrBatch::emitTriangle (ip3, e2->pointIndex + 2, e2->pointIndex + 1);
                  TerrBatch::emitTriangle (ip3, e2->pointIndex + 1, ip7);
               }
               if(!(chunk->emptyFlags & SquareEmpty_2_1))
               {
                  TerrBatch::emitTriangle (ip3, ip7, ip0);
                  TerrBatch::emitTriangle (ip3, ip0, ip6);
               }
               if(!(chunk->emptyFlags & SquareEmpty_3_1))
               {
                  TerrBatch::emitTriangle (ip3, ip6, e1->pointIndex + 1);
                  TerrBatch::emitTriangle (ip3, e1->pointIndex + 1, e1->pointIndex + 2);
               }
            }
         }
         else
         {
            TerrBatch::emitTriangle(ip3, p2, e2->pointIndex + 2 );
            TerrBatch::emitFanStep( e2->pointIndex + 1 );
            TerrBatch::emitFanStep( ip7 );
            TerrBatch::emitFanStep( ip0 );
            TerrBatch::emitFanStep( ip6 );
            TerrBatch::emitFanStep( e1->pointIndex + 1 );
            TerrBatch::emitFanStep( e1->pointIndex + 2 );
            TerrBatch::emitFanStep( p2 );

         }

         if(chunk->emptyFlags & CornerEmpty_0_0)
         {
            if((chunk->emptyFlags & CornerEmpty_0_0) != CornerEmpty_0_0)
            {
               if(!(chunk->emptyFlags & SquareEmpty_0_0))
               {
                  TerrBatch::emitTriangle(ip4, e2->pointIndex, p3);
                  TerrBatch::emitTriangle(ip4, p3, e3->pointIndex + 2);
               }
               if(!(chunk->emptyFlags & SquareEmpty_0_1))
               {
                  TerrBatch::emitTriangle(ip4, e3->pointIndex + 2, e3->pointIndex + 1);
                  TerrBatch::emitTriangle(ip4, e3->pointIndex + 1, ip8);
               }
               if(!(chunk->emptyFlags & SquareEmpty_1_1))
               {
                  TerrBatch::emitTriangle(ip4, ip8, ip0);
                  TerrBatch::emitTriangle(ip4, ip0, ip7);
               }
               if(!(chunk->emptyFlags & SquareEmpty_1_0))
               {
                  TerrBatch::emitTriangle(ip4, ip7, e2->pointIndex + 1);
                  TerrBatch::emitTriangle(ip4, e2->pointIndex + 1, e2->pointIndex);
               }
            }
         }
         else
         {
            TerrBatch::emitTriangle( ip4, p3, e3->pointIndex + 2 );
            TerrBatch::emitFanStep( e3->pointIndex + 1 );
            TerrBatch::emitFanStep( ip8 );
            TerrBatch::emitFanStep( ip0 );
            TerrBatch::emitFanStep( ip7 );
            TerrBatch::emitFanStep( e2->pointIndex + 1 );
            TerrBatch::emitFanStep( e2->pointIndex );
            TerrBatch::emitFanStep( p3 );
         }
      }

      // 4 chunks with 24 indices each:                        96 indices
   }
}
#ifdef STATEBLOCK
void TerrainRender::renderBlock(TerrainBlock *block, SceneState *state, MatInstance * m, SceneGraphData& sgData)
{
	PROFILE_START(TerrainRender);

	PROFILE_START(TerrainRenderSetup);
	U32 storedWaterMark = FrameAllocator::getWaterMark();

	{
		mSceneState  = state;
		mFarDistance = state->getVisibleDistance();

		mCameraToObject = GFX->getWorldMatrix();
		mCameraToObject.inverse();
		mCameraToObject.getColumn(3, &mCamPos);

		mCurrentBlock = block;
		mSquareSize   = block->getSquareSize();

		const F32 blockSize = F32(mSquareSize * TerrainBlock::BlockSize);

#ifdef USE_CLIPMAP
		if(!gClientSceneGraph->isReflectPass())
		{   
			// Set up the clipmap -  have to update now so we put the right textures on
			// the right chunks.
			block->mClipMap->recenter(Point2F(mCamPos.x / blockSize, mCamPos.y / blockSize));
		}
#endif

		MatrixF proj = GFX->getProjectionMatrix();

		//The projection matrix is stored and set properly, but the Canonizer is still complaining
		//This is the only place GFX_Canonizer is used, everywhere else it is commented out... so....
		//GFX_Canonizer("TerrainRender::renderBlock", __FILE__, __LINE__);

		// compute pixelError
		if(mScreenError >= 0.001f)
			mPixelError = 1.f / GFX->projectRadius(mScreenError, 1.f);
		else
			mPixelError = 0.000001f;

		buildClippingPlanes(state->mFlipCull);
		buildLightArray();

		// Calculate the potentially viewable area...
		S32 xStart = (S32)mFloor( (mCamPos.x - mFarDistance) / blockSize );
		S32 xEnd   = (S32)mCeil ( (mCamPos.x + mFarDistance) / blockSize );
		S32 yStart = (S32)mFloor( (mCamPos.y - mFarDistance) / blockSize );
		S32 yEnd   = (S32)mCeil ( (mCamPos.y + mFarDistance) / blockSize );

		// If we disable repeat, then start/end are always one.
		if(!mCurrentBlock->isTiling())
		{
			xStart = yStart = 0;
			xEnd = yEnd = 1;
		}

		S32 xExt   = (S32)(xEnd - xStart);
		S32 yExt   = (S32)(yEnd - yStart);

		PROFILE_END(TerrainRenderSetup);
		PROFILE_START(TerrainRenderRecurse);

		// Come up with a dummy height to work with...
		F32 height = fixedToFloat(block->getHeight(0,0));

		EdgeParent **bottomEdges = (EdgeParent **) FrameAllocator::alloc(sizeof(ChunkScanEdge *) * xExt);

		TerrainRender::allocRenderEdges(xExt, bottomEdges, false);

		ChunkCornerPoint *prevCorner = TerrainRender::allocInitialPoint(
			Point3F(
			xStart * blockSize, 
			yStart * blockSize, 
			height
			)
			);

		mTerrainOffset.set(xStart * TerrainBlock::BlockSquareWidth, 
			yStart * TerrainBlock::BlockSquareWidth);

		for(S32 x = 0; x < xExt; x++)
		{
			bottomEdges[x]->p1 = prevCorner;

			prevCorner = TerrainRender::allocInitialPoint(
				Point3F(
				(xStart + x + 1) * blockSize,
				(yStart    ) * blockSize, 
				height
				)
				);

			bottomEdges[x]->p2 = prevCorner;
		}

		for(S32 y = 0; y < yExt; y++)
		{
			// allocate the left edge:
			EdgeParent *left;
			TerrainRender::allocRenderEdges(1, &left, false);
			left->p1 = TerrainRender::allocInitialPoint(Point3F(xStart * blockSize, (yStart + y + 1) * blockSize, height));
			left->p2 = bottomEdges[0]->p1;

			for(S32 x = 0; x < xExt; x++)
			{
				// Allocate the right,...
				EdgeParent *right;
				TerrainRender::allocRenderEdges(1, &right, false);
				right->p1 = TerrainRender::allocInitialPoint(
					Point3F(
					(xStart + x + 1) * blockSize, 
					(yStart + y + 1) * blockSize, 
					height
					)
					);
				right->p2 = bottomEdges[x]->p2;

				// Allocate the top...
				EdgeParent *top;
				TerrainRender::allocRenderEdges(1, &top, false);
				top->p1 = left->p1;
				top->p2 = right->p1;

				// Process this block...

				//    -  set up for processing the block...
				mBlockOffset.set(x << TerrainBlock::BlockShift,
					y << TerrainBlock::BlockShift);

				mBlockPos.set((xStart + x) * blockSize,
					(yStart + y) * blockSize);

				//    - Do it...
				TerrainRender::processBlock(state, top, right, bottomEdges[x], left);

				// Clean up...
				left = right;
				bottomEdges[x] = top;
			}
		}

		GFX->setProjectionMatrix(proj);

		PROFILE_END(TerrainRenderRecurse);
	}

	PROFILE_START(TerrainRenderEmit);

	// Set up for the render...

	bool fixedfunction = GFX->getPixelShaderVersion() == 0.0;

	const MatrixF blockTransform = block->getTransform();
	const Point3F cameraPosition = state->getCameraPosition();
	AssertFatal(mSetSB, "TerrainRender::renderBlock -- mSetSB cannot be NULL.");
	mSetSB->apply();
	// Run through all the bins and draw each range.
	for(S32 i=0; i<256; i++)
	{
		BatchHelper::ChunkList &cl = gChunkBatcher.mDiffusePasses[i];
		if(!cl.size())
			continue;

		PROFILE_START(TerrainRender_DrawDiffusePass);

		// There's stuff in this bin, so set up the clipmap and draw.
		U8 start = i&0xF;
		U8 end   = (i>>4)&0xF;
		AssertFatal(mAlphaBlendFalseSB, "TerrainRender::renderBlock -- mAlphaBlendFalseSB cannot be NULL.");
		mAlphaBlendFalseSB->apply();
		if (fixedfunction)
		{
			// multi-pass fixed function clipmapping
			Point4F clipmapMapping;
			AssertFatal(mSetBinFixSB, "TerrainRender::renderBlock -- mSetBinFixSB cannot be NULL.");
			mSetBinFixSB->apply();

			// render the layers in order of least detail to most detail, each
			// with a vertex color controlling alpha blend transitions
			for (S32 curLayer=end; curLayer>=start;curLayer--)
			{
				block->mClipMap->bindTexturesFF(curLayer, clipmapMapping);
				gChunkBatcher.renderChunkList(cl, NULL, sgData, TerrBatch::vertexTypeSingleTextureClipMapping, &clipmapMapping, blockTransform, cameraPosition, NULL);
				AssertFatal(mClearBinFixSB, "TerrainRender::renderBlock -- mClearBinFixSB cannot be NULL.");
				mClearBinFixSB->apply();
			}
			AssertFatal(mVColorFalseSB, "TerrainRender::renderBlock -- mVColorFalseSB cannot be NULL.");
			mVColorFalseSB->apply();

		}
		else
		{
			// single-pass shader clipmapping
			switch(end - start)
			{
			case 0:
			case 1:
				block->mClipMap->bindShaderAndTextures(end, start);
				break;
			case 2:
				block->mClipMap->bindShaderAndTextures(end, start+1, start);
				break;
			case 3:
				block->mClipMap->bindShaderAndTextures(end, start+2, start+1, start);
				break;
			default:
				AssertFatal(false, "TerrainRender::renderBlock - invalid level delta in clipmap setup!");
				break;
			}
			gChunkBatcher.renderChunkList(cl, NULL, sgData, TerrBatch::vertexTypeFullClipMapping, NULL, blockTransform, cameraPosition, NULL);
			AssertFatal(mSetBiasSB, "TerrainRender::renderBlock -- mSetBiasSB cannot be NULL.");
			mSetBiasSB->apply();
		}
		PROFILE_END(TerrainRender_DrawDiffusePass);
	}

	// Grab the shader for this pass - replaceme w/ real code.
	static ShaderData *sdAtlasShaderDetail = NULL;
	if(!sdAtlasShaderDetail)
	{
		if(!Sim::findObject("AtlasShaderDetail", sdAtlasShaderDetail) || !sdAtlasShaderDetail->getShader())
		{
			Con::errorf("AtlasClipMapBatcher::renderDetail - no shader 'AtlasShaderDetail' present!");
			mCurrentBlock = NULL;

			return;
		}
	}

	// Card profiler checks for what is needed to do these two implementations of 
	// fixed function detail blending. If neither are supported, it's going to 
	// look very strange.
	bool canDoLerpDetail = GFX->getCardProfiler()->queryProfile( "lerpDetailBlend", false );
	bool canDoFourStageDetail = GFX->getCardProfiler()->queryProfile( "fourStageDetailBlend", false );

	static bool sNoDetailWarning = false;

	if( fixedfunction && !( canDoLerpDetail || canDoFourStageDetail ) && !sNoDetailWarning )
	{
		Con::warnf( "---------------------------------------------------------------------------------------------" );
		Con::warnf( "TerrainRender::renderBlock -- Fixed function detail blending will fail without LERP blend op"  );
		Con::warnf( "                              or support for temp register, subtract op and 4 blend stages."   );
		Con::warnf( "---------------------------------------------------------------------------------------------" );
		sNoDetailWarning = true;
	}

	//相机位置放在这里设置，以免没有细节纹理的时候，不会设置相机位置常量，从而导致绘制雾出错
	//细节shader和雾shader共用相机位置常量
	if (!fixedfunction)
	{
		GFX->setVertexShaderConstF(20, &mCamPos.x, 1, 3);
	}

	if (TerrainRender::mCurrentBlock->mDetailTextureHandle)
	{

		if (!fixedfunction)
		{
			GFX->setShader(sdAtlasShaderDetail->getShader());
			Point4F detailConst(
				TerrainRender::mCurrentBlock->mDetailDistance / 2.0f,
				1.f / (TerrainRender::mCurrentBlock->mDetailDistance / 2.0f), 
				block->mDetailScale,
				0);
			GFX->setVertexShaderConstF(50, &detailConst[0], 1);
			//GFX->setVertexShaderConstF(20, &mCamPos.x, 1);//移到上面去了

			Point4F brightnessScale(TerrainRender::mCurrentBlock->mDetailBrightness, 0, 0, 0);
			GFX->setPixelShaderConstF(0, &brightnessScale.x, 1);

			// Ok - next up, draw the detail chunks.   
			GFX->setTexture(0, TerrainRender::mCurrentBlock->mDetailTextureHandle);
			AssertFatal(mSetDetailShaderSB, "TerrainRender::renderBlock -- mSetDetailShaderSB cannot be NULL.");
			mSetDetailShaderSB->apply();

			// Draw detail.
			gChunkBatcher.renderChunkList(gChunkBatcher.mDetail, NULL, sgData, TerrBatch::vertexTypeFullClipMapping, NULL, blockTransform, cameraPosition, NULL);
		}
		else
		{
			// Use a texture coord transform matrix to scale the detail texture
			MatrixF detailTexMatrix( true );
			detailTexMatrix.scale( Point3F( block->mDetailScale, block->mDetailScale, 0.0f ) );  

			GFX->setTextureMatrix( 0, detailTexMatrix );

			GFX->setTexture( 0, TerrainRender::mCurrentBlock->mDetailTextureHandle );

			// LERP blend if the card supports it
			if( canDoLerpDetail )
			{
				AssertFatal(mDetailFixLerpSB, "TerrainRender::renderBlock -- mDetailFixLerpSB cannot be NULL.");
				mDetailFixLerpSB->apply();
				GFX->setTexture( 1, NULL );
			}
			else if( canDoFourStageDetail )
			{
				AssertFatal(mDetailFixForStageSB, "TerrainRender::renderBlock -- mDetailFixForStageSB cannot be NULL.");
				mDetailFixForStageSB->apply();
				GFX->setTexture( 1, NULL );
				GFX->setTexture( 2, NULL );
				GFX->setTexture( 3, NULL );
			}

			// Draw detail.
			gChunkBatcher.renderChunkList( gChunkBatcher.mDetail, NULL, sgData, TerrBatch::vertexTypeDetail, NULL, blockTransform, cameraPosition, NULL );

			// Clean up all the odd states so future renders don't get all messed up
			AssertFatal(mClearDetailFixSB, "TerrainRender::renderBlock -- mClearDetailFixSB cannot be NULL.");
			mClearDetailFixSB->apply();
		}

	}

	// render dynamic light materials...
	if(m && (!mRenderingCommander))
	{
		Point4F clipmapMapping;

		// Make sure our material is initialized so that we get lighting info.
		m->init(sgData, (GFXVertexFlags)getGFXVertFlags((GFXVertexPCNTT*)NULL) );

		AssertFatal(mSetLitSB, "TerrainRender::renderBlock -- mSetLitSB cannot be NULL.");
		mSetLitSB->apply();

		for(U32 i=0; i<mDynamicLightCount; i++)
		{
			AssertFatal(mTerrainLighting, "Terrain lighting should be non-null if dynamic light count > 0, see TerrRender::buildLightArray");
			// Do we have any chunks for this light? Skip if not.
			U32 curLayer = 0;
			for (; curLayer<16; curLayer++)
				if(gChunkBatcher.mLightPasses[i][curLayer].size() > 0)
					break;
			if (curLayer == 16)
				continue; // nothing lit

			// Ok - there is real work to do here, so set up the light info
			// and batch up our geometry.         
			LightInfo *light      = mTerrainLights[i].light;
			MatInstance* dmat = NULL;
			TerrainRender::mTerrainLighting->setupLightStage(light, sgData, m, &dmat);
			if (!dmat)
				continue;

			dmat->init(sgData, (GFXVertexFlags)getGFXVertFlags((GFXVertexPCNTT*)NULL));
			while (dmat->setupPass(sgData))
			{
				// texture units for dlight materials are:
				// 0 : diffuseMap  (diffuse texture of base material)
				// 1 : blackfogMap (fog attenuation - optional)
				// 2 : dlightMap   (distance attenuation)
				// 3 : dlightMask  (directional color filter - optional)

				GFX->setTexture(1, sgData.blackfogTex);
				if(sgData.useFog)
				{
					AssertFatal(mLitFogSB, "TerrainRender::renderBlock -- mLitFogSB cannot be NULL.");
					mLitFogSB->apply();
				}
				else if (fixedfunction)
				{
					AssertFatal(mLitFixSB, "TerrainRender::renderBlock -- mLitFixSB cannot be NULL.");
					mLitFixSB->apply();				
				}
				else
				{
					AssertFatal(mLitShaderSB, "TerrainRender::renderBlock -- mLitShaderSB cannot be NULL.");
					mLitShaderSB->apply();				
				}

				if (!fixedfunction)
				{
					AssertFatal(mLitShaderAddrSB, "TerrainRender::renderBlock -- mLitShaderAddrSB cannot be NULL.");
					mLitShaderAddrSB->apply();	
				}

				for (; curLayer<16; curLayer++)
				{
					if(gChunkBatcher.mLightPasses[i][curLayer].size())
					{
						// yes this is used on both pixel shader and fixed function path
						block->mClipMap->bindTexturesFF(curLayer, clipmapMapping);
						gChunkBatcher.renderChunkList(gChunkBatcher.mLightPasses[i][curLayer], dmat, sgData, fixedfunction ? TerrBatch::vertexTypeDLight : TerrBatch::vertexTypeSingleTextureClipMapping, &clipmapMapping, blockTransform, cameraPosition, light);
					}
				}
			}
		}
		AssertFatal(mAlphaBlendFalseSB, "TerrainRender::renderBlock -- mAlphaBlendFalseSB cannot be NULL.");
		mAlphaBlendFalseSB->apply();
	}

	// And the fog chunks.
	static ShaderData *sdAtlasShaderFog = NULL;
	if(!sdAtlasShaderFog)
	{
		if(!Sim::findObject("AtlasShaderFog", sdAtlasShaderFog) || !sdAtlasShaderFog->getShader())
		{
			Con::errorf("AtlasClipMapBatcher::renderFog - no shader 'AtlasShaderFog' present!");
			mCurrentBlock = NULL;
			return;
		}
	}

	if (!fixedfunction)
	{
		// Set up the fog shader and texture.
		GFX->setShader(sdAtlasShaderFog->getShader());

		Point4F fogConst(
			gClientSceneGraph->getFogHeightOffset(), 
			gClientSceneGraph->getFogInvHeightRange(), 
			gClientSceneGraph->getVisibleDistanceMod(), 
			0);
		GFX->setVertexShaderConstF(22, &fogConst[0], 1);

		MatrixF objTrans = block->getTransform();
		objTrans.transpose();
		GFX->setVertexShaderConstF( 12 /*VC_OBJ_TRANS*/, (float*)&objTrans, 4 );
	}

	GFX->setTexture(0, gClientSceneGraph->getFogTexture());

	AssertFatal(mFogSetSB, "TerrainRender::renderBlock -- mFogSetSB cannot be NULL.");
	mFogSetSB->apply();

	gChunkBatcher.renderChunkList(gChunkBatcher.mFog, NULL, sgData, fixedfunction ? TerrBatch::vertexTypeFog : TerrBatch::vertexTypeFullClipMapping, NULL, blockTransform, cameraPosition, NULL);

	AssertFatal(mFogClearSB, "TerrainRender::renderBlock -- mFogClearSB cannot be NULL.");
	mFogClearSB->apply();

	gChunkBatcher.reset();

	FrameAllocator::setWaterMark(storedWaterMark);

	PROFILE_END(TerrainRenderEmit);
	PROFILE_END(TerrainRender);
	mCurrentBlock = NULL;
}

#else

#ifdef USE_CLIPMAP
void TerrainRender::renderBlock(TerrainBlock *block, SceneState *state, MatInstance * m, SceneGraphData& sgData)
{
   PROFILE_START(TerrainRender);

   PROFILE_START(TerrainRenderSetup);
   U32 storedWaterMark = FrameAllocator::getWaterMark();

   {
   mSceneState  = state;
   mFarDistance = state->getVisibleDistance();

   mCameraToObject = GFX->getWorldMatrix();
   mCameraToObject.inverse();
   mCameraToObject.getColumn(3, &mCamPos);

   mCurrentBlock = block;
   mSquareSize   = block->getSquareSize();

   const F32 blockSize = F32(mSquareSize * TerrainBlock::BlockSize);

#ifdef USE_CLIPMAP
   if(!gClientSceneGraph->isReflectPass())
   {   
      // Set up the clipmap -  have to update now so we put the right textures on
      // the right chunks.
      block->mClipMap->recenter(Point2F(mCamPos.x / blockSize, mCamPos.y / blockSize));
   }
#endif

   MatrixF proj = GFX->getProjectionMatrix();

   //The projection matrix is stored and set properly, but the Canonizer is still complaining
   //This is the only place GFX_Canonizer is used, everywhere else it is commented out... so....
   //GFX_Canonizer("TerrainRender::renderBlock", __FILE__, __LINE__);

   // compute pixelError
   if(mScreenError >= 0.001f)
      mPixelError = 1.f / GFX->projectRadius(mScreenError, 1.f);
   else
      mPixelError = 0.000001f;

   buildClippingPlanes(state->mFlipCull);
   buildLightArray();

   // Calculate the potentially viewable area...
   S32 xStart = (S32)mFloor( (mCamPos.x - mFarDistance) / blockSize );
   S32 xEnd   = (S32)mCeil ( (mCamPos.x + mFarDistance) / blockSize );
   S32 yStart = (S32)mFloor( (mCamPos.y - mFarDistance) / blockSize );
   S32 yEnd   = (S32)mCeil ( (mCamPos.y + mFarDistance) / blockSize );

   // If we disable repeat, then start/end are always one.
   if(!mCurrentBlock->isTiling())
   {
      xStart = yStart = 0;
      xEnd = yEnd = 1;
   }

   S32 xExt   = (S32)(xEnd - xStart);
   S32 yExt   = (S32)(yEnd - yStart);

   PROFILE_END(TerrainRenderSetup);
   PROFILE_START(TerrainRenderRecurse);

   // Come up with a dummy height to work with...
   F32 height = fixedToFloat(block->getHeight(0,0));

   EdgeParent **bottomEdges = (EdgeParent **) FrameAllocator::alloc(sizeof(ChunkScanEdge *) * xExt);

   TerrainRender::allocRenderEdges(xExt, bottomEdges, false);
   
   ChunkCornerPoint *prevCorner = TerrainRender::allocInitialPoint(
                                    Point3F(
                                       xStart * blockSize, 
                                       yStart * blockSize, 
                                       height
                                     )
                                   );

   mTerrainOffset.set(xStart * TerrainBlock::BlockSquareWidth, 
                      yStart * TerrainBlock::BlockSquareWidth);

   for(S32 x = 0; x < xExt; x++)
   {
      bottomEdges[x]->p1 = prevCorner;
      
      prevCorner = TerrainRender::allocInitialPoint(
                     Point3F(
                        (xStart + x + 1) * blockSize,
                        (yStart    ) * blockSize, 
                        height
                     )
                   );

      bottomEdges[x]->p2 = prevCorner;
   }

   for(S32 y = 0; y < yExt; y++)
   {
      // allocate the left edge:
      EdgeParent *left;
      TerrainRender::allocRenderEdges(1, &left, false);
      left->p1 = TerrainRender::allocInitialPoint(Point3F(xStart * blockSize, (yStart + y + 1) * blockSize, height));
      left->p2 = bottomEdges[0]->p1;

      for(S32 x = 0; x < xExt; x++)
      {
         // Allocate the right,...
         EdgeParent *right;
         TerrainRender::allocRenderEdges(1, &right, false);
         right->p1 = TerrainRender::allocInitialPoint(
                        Point3F(
                           (xStart + x + 1) * blockSize, 
                           (yStart + y + 1) * blockSize, 
                           height
                        )
                     );
         right->p2 = bottomEdges[x]->p2;

         // Allocate the top...
         EdgeParent *top;
         TerrainRender::allocRenderEdges(1, &top, false);
         top->p1 = left->p1;
         top->p2 = right->p1;

         // Process this block...

         //    -  set up for processing the block...
         mBlockOffset.set(x << TerrainBlock::BlockShift,
                          y << TerrainBlock::BlockShift);

         mBlockPos.set((xStart + x) * blockSize,
                       (yStart + y) * blockSize);

         //    - Do it...
         TerrainRender::processBlock(state, top, right, bottomEdges[x], left);

         // Clean up...
         left = right;
         bottomEdges[x] = top;
      }
   }
   
   GFX->setProjectionMatrix(proj);

   PROFILE_END(TerrainRenderRecurse);

   }

   PROFILE_START(TerrainRenderEmit);

   // Set up for the render...

   bool fixedfunction = GFX->getPixelShaderVersion() == 0.0;

   const MatrixF blockTransform = block->getTransform();
   const Point3F cameraPosition = state->getCameraPosition();

   GFX->setTextureStageColorOp(0, GFXTOPModulate);
   GFX->setTextureStageColorOp(1, GFXTOPDisable);
   GFX->setAlphaBlendEnable(false);
   GFX->setAlphaTestEnable(false);
	//新加
	GFX->setZEnable(true);
	GFX->setZWriteEnable(true);

   // Run through all the bins and draw each range.
   for(S32 i=0; i<256; i++)
   {
      BatchHelper::ChunkList &cl = gChunkBatcher.mDiffusePasses[i];
      if(!cl.size())
         continue;

      PROFILE_START(TerrainRender_DrawDiffusePass);

      // There's stuff in this bin, so set up the clipmap and draw.
      U8 start = i&0xF;
      U8 end   = (i>>4)&0xF;

      GFX->setAlphaBlendEnable(false);
      if (fixedfunction)
      {
         // multi-pass fixed function clipmapping
         Point4F clipmapMapping;
         GFX->setVertexColorEnable(true);
         GFX->setSrcBlend(GFXBlendSrcAlpha);
         GFX->setDestBlend(GFXBlendInvSrcAlpha);
         GFX->setTextureStageColorOp(0, GFXTOPModulate);
         GFX->setTextureStageColorOp(1, GFXTOPDisable);
         // render the layers in order of least detail to most detail, each
         // with a vertex color controlling alpha blend transitions
         for (S32 curLayer=end; curLayer>=start;curLayer--)
         {
            block->mClipMap->bindTexturesFF(curLayer, clipmapMapping);
            gChunkBatcher.renderChunkList(cl, NULL, sgData, TerrBatch::vertexTypeSingleTextureClipMapping, &clipmapMapping, blockTransform, cameraPosition, NULL);
            GFX->setAlphaBlendEnable(true);
         
            // Reset any changes to mipmap bias
            GFX->setTextureStageLODBias(0, 0.0f);
         }
         GFX->setVertexColorEnable(false);
	  }
	  else
	  {
		  // single-pass shader clipmapping
		  switch(end - start)
		  {
		  case 0:
		  case 1:
			  block->mClipMap->bindShaderAndTextures(end, start);
			  break;
		  case 2:
			  block->mClipMap->bindShaderAndTextures(end, start+1, start);
			  break;
		  case 3:
			  block->mClipMap->bindShaderAndTextures(end, start+2, start+1, start);
			  break;
		  default:
			  AssertFatal(false, "TerrainRender::renderBlock - invalid level delta in clipmap setup!");
			  break;
		  }
		  gChunkBatcher.renderChunkList(cl, NULL, sgData, TerrBatch::vertexTypeFullClipMapping, NULL, blockTransform, cameraPosition, NULL);

		  // Reset any changes to mipmap bias
		  GFX->setTextureStageLODBias(0, 0.0f);
		  GFX->setTextureStageLODBias(1, 0.0f);
	  }
	  PROFILE_END(TerrainRender_DrawDiffusePass);
   }

   // Grab the shader for this pass - replaceme w/ real code.
   static ShaderData *sdAtlasShaderDetail = NULL;
   if(!sdAtlasShaderDetail)
   {
	   if(!Sim::findObject("AtlasShaderDetail", sdAtlasShaderDetail) || !sdAtlasShaderDetail->getShader())
	   {
		   Con::errorf("AtlasClipMapBatcher::renderDetail - no shader 'AtlasShaderDetail' present!");
		   mCurrentBlock = NULL;

		   return;
	   }
   }

   // Card profiler checks for what is needed to do these two implementations of 
   // fixed function detail blending. If neither are supported, it's going to 
   // look very strange.
   bool canDoLerpDetail = GFX->getCardProfiler()->queryProfile( "lerpDetailBlend", false );
   bool canDoFourStageDetail = GFX->getCardProfiler()->queryProfile( "fourStageDetailBlend", false );

   static bool sNoDetailWarning = false;

   if( fixedfunction && !( canDoLerpDetail || canDoFourStageDetail ) && !sNoDetailWarning )
   {
      Con::warnf( "---------------------------------------------------------------------------------------------" );
      Con::warnf( "TerrainRender::renderBlock -- Fixed function detail blending will fail without LERP blend op"  );
      Con::warnf( "                              or support for temp register, subtract op and 4 blend stages."   );
      Con::warnf( "---------------------------------------------------------------------------------------------" );
      sNoDetailWarning = true;
   }

   //相机位置放在这里设置，以免没有细节纹理的时候，不会设置相机位置常量，从而导致绘制雾出错
   //细节shader和雾shader共用相机位置常量
   if (!fixedfunction)
   {
	   GFX->setVertexShaderConstF(20, &mCamPos.x, 1, 3);
   }

   if (TerrainRender::mCurrentBlock->mDetailTextureHandle)
   {

      if (!fixedfunction)
      {
         GFX->setShader(sdAtlasShaderDetail->getShader());
         Point4F detailConst(
            TerrainRender::mCurrentBlock->mDetailDistance / 2.0f,
            1.f / (TerrainRender::mCurrentBlock->mDetailDistance / 2.0f), 
            block->mDetailScale,
            0);
         GFX->setVertexShaderConstF(50, &detailConst[0], 1);
         //GFX->setVertexShaderConstF(20, &mCamPos.x, 1);//移到上面去了

         Point4F brightnessScale(TerrainRender::mCurrentBlock->mDetailBrightness, 0, 0, 0);
         GFX->setPixelShaderConstF(0, &brightnessScale.x, 1);

         // Ok - next up, draw the detail chunks.   
         GFX->setTexture(0, TerrainRender::mCurrentBlock->mDetailTextureHandle);
         GFX->setTextureStageColorOp(0, GFXTOPModulate);
         GFX->setTextureStageAddressModeU(0, GFXAddressWrap);
         GFX->setTextureStageAddressModeV(0, GFXAddressWrap);
         GFX->setTextureStageColorOp(1, GFXTOPDisable);

         // This blend works, so don't question it. (It should be a multiply blend.)
         GFX->setAlphaBlendEnable(true);
         GFX->setSrcBlend(GFXBlendDestColor);
         GFX->setDestBlend(GFXBlendSrcColor);

         // Draw detail.
         gChunkBatcher.renderChunkList(gChunkBatcher.mDetail, NULL, sgData, TerrBatch::vertexTypeFullClipMapping, NULL, blockTransform, cameraPosition, NULL);
      }
      else
      {
         // Use a texture coord transform matrix to scale the detail texture
         MatrixF detailTexMatrix( true );
         detailTexMatrix.scale( Point3F( block->mDetailScale, block->mDetailScale, 0.0f ) );  
         
         GFX->setTextureStageTransform( 0, GFXTTFFCoord2D );
         GFX->setTextureMatrix( 0, detailTexMatrix );

         // Set up texture
         GFX->setTexture( 0, TerrainRender::mCurrentBlock->mDetailTextureHandle );
         GFX->setTextureStageAddressModeU( 0, GFXAddressWrap );
         GFX->setTextureStageAddressModeV( 0, GFXAddressWrap );

         // Multiply the diffuse color (vertex color) by the detail texture to adjust
         // the brightness properly
         GFX->setTextureStageColorOp( 0, GFXTOPModulate );
         GFX->setTextureStageColorArg1( 0, GFXTADiffuse );
         GFX->setTextureStageColorArg2( 0, GFXTATexture );

         // Set TFactor to ( 128, 128, 128, 255 ). These aren't magic numbers, detail
         // textures are created such that values > 0.5/128/0x80 will brighten the
         // area, and values less than 'middle' will darken the area.
         static const ColorI sColorGrey( 128, 128, 128, 255 );
         GFX->setTextureFactor( sColorGrey );

         // LERP blend if the card supports it
         if( canDoLerpDetail )
         {
            // Set up texture stage 1 with no texture. Still active, though. This will do
            // the LERP.
            //
            // LERP blend op will do:
            // Arg1 * Arg3 + (1 - Arg3) * Arg2
            GFX->setTexture( 1, NULL );
            GFX->setTextureStageColorOp( 1, GFXTOPLERP );
            GFX->setTextureStageColorArg1( 1, GFXTACurrent );
            GFX->setTextureStageColorArg2( 1, GFXTATFactor );
            GFX->setTextureStageColorArg3( 1, GFXTADiffuse | GFXTAAlphaReplicate );
         }
         else if( canDoFourStageDetail )
         {
            // Ok, no LERP blend op. Can still do a LERP, it'll just take more stages.
            // lerp( x, y, s ) = s * x + (1 - s) * y

            // s * x => temp
            GFX->setTexture( 1, NULL );
            GFX->setTextureStageColorOp( 1, GFXTOPModulate );
            GFX->setTextureStageColorArg1( 1, GFXTACurrent );
            GFX->setTextureStageColorArg2( 1, GFXTADiffuse | GFXTAAlphaReplicate );
            GFX->setTextureStageResultArg( 1, GFXTATemp );

            // (1 - s) * y => current
            GFX->setTexture( 2, NULL );
            GFX->setTextureStageColorOp( 2, GFXTOPModulate );
            GFX->setTextureStageColorArg1( 2, GFXTADiffuse | GFXTAAlphaReplicate | GFXTAComplement );
            GFX->setTextureStageColorArg2( 2, GFXTATFactor );

            // temp + current => current
            GFX->setTexture( 3, NULL );
            GFX->setTextureStageColorOp( 3, GFXTOPAdd );
            GFX->setTextureStageColorArg1( 3, GFXTACurrent );
            GFX->setTextureStageColorArg2( 3, GFXTATemp );
         }

         // Blend
         GFX->setAlphaBlendEnable(true);
         GFX->setSrcBlend(GFXBlendDestColor);
         GFX->setDestBlend(GFXBlendSrcColor);

         // Enable vertex color
         GFX->setVertexColorEnable(true);

         // Draw detail.
         gChunkBatcher.renderChunkList( gChunkBatcher.mDetail, NULL, sgData, TerrBatch::vertexTypeDetail, NULL, blockTransform, cameraPosition, NULL );

         // Clean up all the odd states so future renders don't get all messed up
         GFX->setTextureStageTransform( 0, GFXTTFFDisable );
         GFX->setVertexColorEnable(false);

         for( int i = 0; i < 4; i++ )
         {
            GFX->setTextureStageColorOp( i, GFXTOPDisable );
            GFX->setTextureStageColorArg1( i, GFXTACurrent );
            GFX->setTextureStageColorArg2( i, GFXTATexture );
            GFX->setTextureStageResultArg( i, GFXTACurrent );
         }

         GFX->setTextureStageColorOp( 0, GFXTOPModulate );
      }

   }

   // render dynamic light materials...
   if(m && (!mRenderingCommander))
   {
      Point4F clipmapMapping;

      // Make sure our material is initialized so that we get lighting info.
      m->init(sgData, (GFXVertexFlags)getGFXVertFlags((GFXVertexPCNTT*)NULL) );

      GFX->setTextureStageAlphaOp(2, GFXTOPDisable);
      GFX->setTextureStageAlphaOp(3, GFXTOPDisable);

      for(U32 i=0; i<mDynamicLightCount; i++)
      {
         AssertFatal(mTerrainLighting, "Terrain lighting should be non-null if dynamic light count > 0, see TerrRender::buildLightArray");
         // Do we have any chunks for this light? Skip if not.
         U32 curLayer = 0;
         for (; curLayer<16; curLayer++)
            if(gChunkBatcher.mLightPasses[i][curLayer].size() > 0)
               break;
         if (curLayer == 16)
            continue; // nothing lit

         // Ok - there is real work to do here, so set up the light info
         // and batch up our geometry.         
         LightInfo *light      = mTerrainLights[i].light;
         MatInstance* dmat = NULL;
         TerrainRender::mTerrainLighting->setupLightStage(light, sgData, m, &dmat);
         if (!dmat)
            continue;

         dmat->init(sgData, (GFXVertexFlags)getGFXVertFlags((GFXVertexPCNTT*)NULL));
         while (dmat->setupPass(sgData))
         {
            // texture units for dlight materials are:
            // 0 : diffuseMap  (diffuse texture of base material)
            // 1 : blackfogMap (fog attenuation - optional)
            // 2 : dlightMap   (distance attenuation)
            // 3 : dlightMask  (directional color filter - optional)
            GFX->setTextureStageAddressModeU(0, GFXAddressWrap );
            GFX->setTextureStageAddressModeV(0, GFXAddressWrap );
            GFX->setTextureStageColorOp(0, GFXTOPModulate);

            GFX->setTexture(1, sgData.blackfogTex);
            if(sgData.useFog)
            {
               GFX->setTextureStageAddressModeU(1, GFXAddressClamp );
               GFX->setTextureStageAddressModeV(1, GFXAddressClamp );
               GFX->setTextureStageColorOp(1, GFXTOPBlendTextureAlpha);
            }
            else if (fixedfunction)
               GFX->setTextureStageColorOp(1, GFXTOPDisable);
            else
               GFX->setTextureStageColorOp(1, GFXTOPModulate);

            if (!fixedfunction)
            {
               GFX->setTextureStageAddressModeU(2, GFXAddressClamp );
               GFX->setTextureStageAddressModeV(2, GFXAddressClamp );
            }

            GFX->setVertexColorEnable(true);
            GFX->setAlphaBlendEnable(true);
            GFX->setSrcBlend(GFXBlendOne);
            GFX->setDestBlend(GFXBlendOne);

            for (; curLayer<16; curLayer++)
            {
               if(gChunkBatcher.mLightPasses[i][curLayer].size())
               {
                  // yes this is used on both pixel shader and fixed function path
                  block->mClipMap->bindTexturesFF(curLayer, clipmapMapping);
                  gChunkBatcher.renderChunkList(gChunkBatcher.mLightPasses[i][curLayer], dmat, sgData, fixedfunction ? TerrBatch::vertexTypeDLight : TerrBatch::vertexTypeSingleTextureClipMapping, &clipmapMapping, blockTransform, cameraPosition, light);
               }
            }
         }
      }
      GFX->setAlphaBlendEnable(false);
   }

   // And the fog chunks.
   static ShaderData *sdAtlasShaderFog = NULL;
   if(!sdAtlasShaderFog)
   {
	   if(!Sim::findObject("AtlasShaderFog", sdAtlasShaderFog) || !sdAtlasShaderFog->getShader())
	   {
		   Con::errorf("AtlasClipMapBatcher::renderFog - no shader 'AtlasShaderFog' present!");
		   mCurrentBlock = NULL;
		   return;
	   }
   }

   if (!fixedfunction)
   {
      // Set up the fog shader and texture.
      GFX->setShader(sdAtlasShaderFog->getShader());

      Point4F fogConst(
         gClientSceneGraph->getFogHeightOffset(), 
         gClientSceneGraph->getFogInvHeightRange(), 
         gClientSceneGraph->getVisibleDistanceMod(), 
         0);
      GFX->setVertexShaderConstF(22, &fogConst[0], 1);

      MatrixF objTrans = block->getTransform();
      objTrans.transpose();
      GFX->setVertexShaderConstF( 12 /*VC_OBJ_TRANS*/, (float*)&objTrans, 4 );
   }

   GFX->setTexture(0, gClientSceneGraph->getFogTexture());
   GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
   GFX->setTextureStageAddressModeV(0, GFXAddressClamp);

   // We need the eye pos but previous code deals with setting this up.

   // Set blend mode and alpha test as well.
   GFX->setAlphaBlendEnable(true);
   GFX->setSrcBlend(GFXBlendSrcAlpha);
   GFX->setDestBlend(GFXBlendInvSrcAlpha);

   GFX->setAlphaTestEnable(true);
   GFX->setAlphaFunc(GFXCmpGreaterEqual);
   GFX->setAlphaRef(2);

   gChunkBatcher.renderChunkList(gChunkBatcher.mFog, NULL, sgData, fixedfunction ? TerrBatch::vertexTypeFog : TerrBatch::vertexTypeFullClipMapping, NULL, blockTransform, cameraPosition, NULL);

   GFX->setAlphaBlendEnable(false);
   GFX->setAlphaTestEnable(false);
   GFX->setTextureStageColorOp(0, GFXTOPModulate);
   GFX->setTextureStageColorOp(1, GFXTOPDisable);

   gChunkBatcher.reset();

   FrameAllocator::setWaterMark(storedWaterMark);

   PROFILE_END(TerrainRenderEmit);
   PROFILE_END(TerrainRender);
   mCurrentBlock = NULL;
}
#else
//void TerrainRender::renderBlock(TerrainBlock *block, SceneState *state, MatInstance * m, SceneGraphData& sgData)
//{
//	PROFILE_START(TerrainRender);
//
//	PROFILE_START(TerrainRenderSetup);
//	U32 storedWaterMark = FrameAllocator::getWaterMark();
//
//	{
//		mSceneState  = state;
//		mFarDistance = state->getVisibleDistance();
//
//		mCameraToObject = GFX->getWorldMatrix();
//		mCameraToObject.inverse();
//		mCameraToObject.getColumn(3, &mCamPos);
//
//		mCurrentBlock = block;
//		mSquareSize   = block->getSquareSize();
//
//		const F32 blockSize = F32(mSquareSize * TerrainBlock::BlockSize);
//
//		MatrixF proj = GFX->getProjectionMatrix();
//
//		//The projection matrix is stored and set properly, but the Canonizer is still complaining
//		//This is the only place GFX_Canonizer is used, everywhere else it is commented out... so....
//		//GFX_Canonizer("TerrainRender::renderBlock", __FILE__, __LINE__);
//
//		// compute pixelError
//		if(mScreenError >= 0.001f)
//			mPixelError = 1.f / GFX->projectRadius(mScreenError, 1.f);
//		else
//			mPixelError = 0.000001f;
//
//		buildClippingPlanes(state->mFlipCull);
//		buildLightArray();
//
//		// Calculate the potentially viewable area...
//		S32 xStart = (S32)mFloor( (mCamPos.x - mFarDistance) / blockSize );
//		S32 xEnd   = (S32)mCeil ( (mCamPos.x + mFarDistance) / blockSize );
//		S32 yStart = (S32)mFloor( (mCamPos.y - mFarDistance) / blockSize );
//		S32 yEnd   = (S32)mCeil ( (mCamPos.y + mFarDistance) / blockSize );
//
//		// If we disable repeat, then start/end are always one.
//		if(!mCurrentBlock->isTiling())
//		{
//			xStart = yStart = 0;
//			xEnd = yEnd = 1;
//		}
//
//		S32 xExt   = (S32)(xEnd - xStart);
//		S32 yExt   = (S32)(yEnd - yStart);
//
//		PROFILE_END(TerrainRenderSetup);
//		PROFILE_START(TerrainRenderRecurse);
//
//		// Come up with a dummy height to work with...
//		F32 height = fixedToFloat(block->getHeight(0,0));
//
//		EdgeParent **bottomEdges = (EdgeParent **) FrameAllocator::alloc(sizeof(ChunkScanEdge *) * xExt);
//
//		TerrainRender::allocRenderEdges(xExt, bottomEdges, false);
//
//		ChunkCornerPoint *prevCorner = TerrainRender::allocInitialPoint(
//			Point3F(
//			xStart * blockSize, 
//			yStart * blockSize, 
//			height
//			)
//			);
//
//		mTerrainOffset.set(xStart * TerrainBlock::BlockSquareWidth, 
//			yStart * TerrainBlock::BlockSquareWidth);
//
//		for(S32 x = 0; x < xExt; x++)
//		{
//			bottomEdges[x]->p1 = prevCorner;
//
//			prevCorner = TerrainRender::allocInitialPoint(
//				Point3F(
//				(xStart + x + 1) * blockSize,
//				(yStart    ) * blockSize, 
//				height
//				)
//				);
//
//			bottomEdges[x]->p2 = prevCorner;
//		}
//
//		for(S32 y = 0; y < yExt; y++)
//		{
//			// allocate the left edge:
//			EdgeParent *left;
//			TerrainRender::allocRenderEdges(1, &left, false);
//			left->p1 = TerrainRender::allocInitialPoint(Point3F(xStart * blockSize, (yStart + y + 1) * blockSize, height));
//			left->p2 = bottomEdges[0]->p1;
//
//			for(S32 x = 0; x < xExt; x++)
//			{
//				// Allocate the right,...
//				EdgeParent *right;
//				TerrainRender::allocRenderEdges(1, &right, false);
//				right->p1 = TerrainRender::allocInitialPoint(
//					Point3F(
//					(xStart + x + 1) * blockSize, 
//					(yStart + y + 1) * blockSize, 
//					height
//					)
//					);
//				right->p2 = bottomEdges[x]->p2;
//
//				// Allocate the top...
//				EdgeParent *top;
//				TerrainRender::allocRenderEdges(1, &top, false);
//				top->p1 = left->p1;
//				top->p2 = right->p1;
//
//				// Process this block...
//
//				//    -  set up for processing the block...
//				mBlockOffset.set(x << TerrainBlock::BlockShift,
//					y << TerrainBlock::BlockShift);
//
//				mBlockPos.set((xStart + x) * blockSize,
//					(yStart + y) * blockSize);
//
//				//    - Do it...
//				TerrainRender::processBlock(state, top, right, bottomEdges[x], left);
//
//				// Clean up...
//				left = right;
//				bottomEdges[x] = top;
//			}
//		}
//
//		GFX->setProjectionMatrix(proj);
//
//		PROFILE_END(TerrainRenderRecurse);
//
//	}
//
//	PROFILE_START(TerrainRenderEmit);
//
//	// Set up for the render...
//
//	bool fixedfunction = GFX->getPixelShaderVersion() == 0.0;
//
//	const MatrixF blockTransform = block->getTransform();
//	const Point3F cameraPosition = state->getCameraPosition();
//
//	GFX->setTextureStageColorOp(0, GFXTOPModulate);
//
//	GFX->setTextureStageColorOp(1, GFXTOPModulate);
//	GFX->setTextureStageColorOp(2, GFXTOPModulate);
//	GFX->setTextureStageColorOp(3, GFXTOPModulate);
//	GFX->setTextureStageColorOp(4, GFXTOPModulate);
//
//	GFX->setTextureStageColorOp(5, GFXTOPModulate);
//	GFX->setTextureStageColorOp(6, GFXTOPDisable);
//	//GFX->setTextureStageColorOp(7, GFXTOPModulate);
//	//GFX->setTextureStageColorOp(1, GFXTOPDisable);
//
//	GFX->setTextureStageAddressModeU(0, GFXAddressWrap);
//	GFX->setTextureStageAddressModeV(0, GFXAddressWrap);
//
//	GFX->setTextureStageAddressModeU(1, GFXAddressWrap);
//	GFX->setTextureStageAddressModeV(1, GFXAddressWrap);
//
//	GFX->setTextureStageAddressModeU(2, GFXAddressWrap);
//	GFX->setTextureStageAddressModeV(2, GFXAddressWrap);
//
//	GFX->setTextureStageAddressModeU(3, GFXAddressWrap);
//	GFX->setTextureStageAddressModeV(3, GFXAddressWrap);
//
//	GFX->setTextureStageAddressModeU(4, GFXAddressClamp);
//	GFX->setTextureStageAddressModeV(4, GFXAddressClamp);
//
//	GFX->setTextureStageAddressModeU(5, GFXAddressClamp);
//	GFX->setTextureStageAddressModeV(5, GFXAddressClamp);
//
//	//GFX->setTextureStageAddressModeU(6, GFXAddressWrap);
//	//GFX->setTextureStageAddressModeV(6, GFXAddressWrap);
//
//	//GFX->setTextureStageAddressModeU(7, GFXAddressWrap);
//	//GFX->setTextureStageAddressModeV(7, GFXAddressWrap);
//	GFX->setAlphaBlendEnable(false);
//	GFX->setAlphaTestEnable(false);
//	GFX->setTextureStageMinFilter(0, GFXTextureFilterLinear);
//	GFX->setTextureStageMinFilter(1, GFXTextureFilterLinear);
//	GFX->setTextureStageMinFilter(2, GFXTextureFilterLinear);
//	GFX->setTextureStageMinFilter(3, GFXTextureFilterLinear);
//	GFX->setTextureStageMinFilter(4, GFXTextureFilterLinear);
//	GFX->setTextureStageMinFilter(5, GFXTextureFilterLinear);
//
//	GFX->setTextureStageMagFilter(0, GFXTextureFilterLinear);
//	GFX->setTextureStageMagFilter(1, GFXTextureFilterLinear);
//	GFX->setTextureStageMagFilter(2, GFXTextureFilterLinear);
//	GFX->setTextureStageMagFilter(3, GFXTextureFilterLinear);
//	GFX->setTextureStageMagFilter(4, GFXTextureFilterLinear);
//	GFX->setTextureStageMagFilter(5, GFXTextureFilterLinear);
//
//	GFX->setTextureStageMipFilter(0, GFXTextureFilterLinear);
//	GFX->setTextureStageMipFilter(1, GFXTextureFilterLinear);
//	GFX->setTextureStageMipFilter(2, GFXTextureFilterLinear);
//	GFX->setTextureStageMipFilter(3, GFXTextureFilterLinear);
//	GFX->setTextureStageMipFilter(4, GFXTextureFilterLinear);
//	GFX->setTextureStageMipFilter(5, GFXTextureFilterLinear);
//	//新加
//	GFX->setZEnable(true);
//	GFX->setZWriteEnable(true);
//
//	static ShaderData *sdAtlasShaderDif = NULL;
//	if(!sdAtlasShaderDif)
//	{
//		if(!Sim::findObject("AtlasShaderDif", sdAtlasShaderDif) || !sdAtlasShaderDif->getShader())
//		{
//			Con::errorf("TerrainRender::renderBlock - no shader 'sdAtlasShaderDif' present!");
//			mCurrentBlock = NULL;
//			return;
//		}
//	}
//	if (!fixedfunction)
//	{
//		GFX->setShader(sdAtlasShaderDif->getShader());
//	}
//
//#ifdef NTJ_EDITOR
//	static TerrainEditor *spTerrainEditor = NULL;
//	if (spTerrainEditor == NULL)
//	{
//		Sim::findObject("ETerrainEditor", spTerrainEditor);
//	}
//#endif
//	// Run through all the bins and draw each range.
//	//for(S32 i=0; i<256; i++)
//	//block->updateTexture();
//	GFX->setTexture(5, block->tex3);
//	for(S32 i=0; i<TerrainBlock::ChunkSize*TerrainBlock::ChunkSize; i++)
//	{
//		//BatchHelper::ChunkList &cl = gChunkBatcher.mDiffusePasses[i];
//		BatchHelper::ChunkList &cl = gChunkBatcher.mDifPass[i];
//		if(!cl.size())
//			continue;
//
//		for (U32 j=0; j<TerrainBlock::Max_Textures; j++)
//		{
//#ifdef NTJ_CLIENT
//			GFX->setTexture(j, block->mTextures[i][j]);
//#elif defined NTJ_EDITOR
//			if (spTerrainEditor != NULL && j<TerrainBlock::Max_Texture_Layer)
//			{
//				if (spTerrainEditor->getUseLayer(j))
//				{
//					GFX->setTexture(j, block->mTextures[i][j]);
//				} 
//				else
//				{
//					GFX->setTexture(j, NULL);
//				}
//			}
//			else
//			{
//				GFX->setTexture(j, block->mTextures[i][j]);
//			}
//#endif
//		}
//
//		PROFILE_START(TerrainRender_DrawDiffusePass);
//
//		// There's stuff in this bin, so set up the clipmap and draw.
//		//U8 start = i&0xF;
//		//U8 end   = (i>>4)&0xF;
//		S32 x = i&TerrainBlock::ChunkShiftMask;
//		S32 y = (i>>4)&TerrainBlock::ChunkShiftMask;
//
//		GFX->setAlphaBlendEnable(false);
//		if (fixedfunction)
//		{
//			// multi-pass fixed function clipmapping
//			Point4F clipmapMapping;
//			GFX->setVertexColorEnable(true);
//			GFX->setSrcBlend(GFXBlendSrcAlpha);
//			GFX->setDestBlend(GFXBlendInvSrcAlpha);
//			GFX->setTextureStageColorOp(0, GFXTOPModulate);
//			GFX->setTextureStageColorOp(1, GFXTOPDisable);
//			// render the layers in order of least detail to most detail, each
//			// with a vertex color controlling alpha blend transitions
//			//for (S32 curLayer=end; curLayer>=start;curLayer--)
//			//{
//			//	block->mClipMap->bindTexturesFF(curLayer, clipmapMapping);
//			//	gChunkBatcher.renderChunkList(cl, NULL, sgData, TerrBatch::vertexTypeSingleTextureClipMapping, &clipmapMapping, blockTransform, cameraPosition, NULL);
//			//	GFX->setAlphaBlendEnable(true);
//
//			//	// Reset any changes to mipmap bias
//			//	GFX->setTextureStageLODBias(0, 0.0f);
//			//}
//			GFX->setVertexColorEnable(false);
//		}
//		else
//		{
//			Point3F offset = block->getSubBlockOffset(i);
//			GFX->setVertexShaderConstF(10, &offset.x, 1);
//			gChunkBatcher.renderChunkList(cl, NULL, sgData, TerrBatch::vertexTypeFullClipMapping, NULL, blockTransform, cameraPosition, NULL);
//
//			// Reset any changes to mipmap bias
//			GFX->setTextureStageLODBias(0, 0.0f);
//			GFX->setTextureStageLODBias(1, 0.0f);
//		}
//		PROFILE_END(TerrainRender_DrawDiffusePass);
//	}
//
//	// Grab the shader for this pass - replaceme w/ real code.
//	static ShaderData *sdAtlasShaderDetail = NULL;
//	if(!sdAtlasShaderDetail)
//	{
//		if(!Sim::findObject("AtlasShaderDetail", sdAtlasShaderDetail) || !sdAtlasShaderDetail->getShader())
//		{
//			Con::errorf("AtlasClipMapBatcher::renderDetail - no shader 'AtlasShaderDetail' present!");
//			mCurrentBlock = NULL;
//
//			return;
//		}
//	}
//
//	// Card profiler checks for what is needed to do these two implementations of 
//	// fixed function detail blending. If neither are supported, it's going to 
//	// look very strange.
//	bool canDoLerpDetail = GFX->getCardProfiler()->queryProfile( "lerpDetailBlend", false );
//	bool canDoFourStageDetail = GFX->getCardProfiler()->queryProfile( "fourStageDetailBlend", false );
//
//	static bool sNoDetailWarning = false;
//
//	if( fixedfunction && !( canDoLerpDetail || canDoFourStageDetail ) && !sNoDetailWarning )
//	{
//		Con::warnf( "---------------------------------------------------------------------------------------------" );
//		Con::warnf( "TerrainRender::renderBlock -- Fixed function detail blending will fail without LERP blend op"  );
//		Con::warnf( "                              or support for temp register, subtract op and 4 blend stages."   );
//		Con::warnf( "---------------------------------------------------------------------------------------------" );
//		sNoDetailWarning = true;
//	}
//
//	//相机位置放在这里设置，以免没有细节纹理的时候，不会设置相机位置常量，从而导致绘制雾出错
//	//细节shader和雾shader共用相机位置常量
//	if (!fixedfunction)
//	{
//		GFX->setVertexShaderConstF(20, &mCamPos.x, 1, 3);
//	}
//
//	if (TerrainRender::mCurrentBlock->mDetailTextureHandle)
//	{
//
//		if (!fixedfunction)
//		{
//			GFX->setShader(sdAtlasShaderDetail->getShader());
//			Point4F detailConst(
//				TerrainRender::mCurrentBlock->mDetailDistance / 2.0f,
//				1.f / (TerrainRender::mCurrentBlock->mDetailDistance / 2.0f), 
//				block->mDetailScale,
//				0);
//			GFX->setVertexShaderConstF(50, &detailConst[0], 1);
//			//GFX->setVertexShaderConstF(20, &mCamPos.x, 1);//移到上面去了
//
//			Point4F brightnessScale(TerrainRender::mCurrentBlock->mDetailBrightness, 0, 0, 0);
//			GFX->setPixelShaderConstF(0, &brightnessScale.x, 1);
//
//			// Ok - next up, draw the detail chunks.   
//			GFX->setTexture(0, TerrainRender::mCurrentBlock->mDetailTextureHandle);
//			GFX->setTextureStageColorOp(0, GFXTOPModulate);
//			GFX->setTextureStageAddressModeU(0, GFXAddressWrap);
//			GFX->setTextureStageAddressModeV(0, GFXAddressWrap);
//			GFX->setTextureStageColorOp(1, GFXTOPDisable);
//
//			// This blend works, so don't question it. (It should be a multiply blend.)
//			GFX->setAlphaBlendEnable(true);
//			GFX->setSrcBlend(GFXBlendDestColor);
//			GFX->setDestBlend(GFXBlendSrcColor);
//
//			// Draw detail.
//			gChunkBatcher.renderChunkList(gChunkBatcher.mDetail, NULL, sgData, TerrBatch::vertexTypeFullClipMapping, NULL, blockTransform, cameraPosition, NULL);
//		}
//		else
//		{
//			// Use a texture coord transform matrix to scale the detail texture
//			MatrixF detailTexMatrix( true );
//			detailTexMatrix.scale( Point3F( block->mDetailScale, block->mDetailScale, 0.0f ) );  
//
//			GFX->setTextureStageTransform( 0, GFXTTFFCoord2D );
//			GFX->setTextureMatrix( 0, detailTexMatrix );
//
//			// Set up texture
//			GFX->setTexture( 0, TerrainRender::mCurrentBlock->mDetailTextureHandle );
//			GFX->setTextureStageAddressModeU( 0, GFXAddressWrap );
//			GFX->setTextureStageAddressModeV( 0, GFXAddressWrap );
//
//			// Multiply the diffuse color (vertex color) by the detail texture to adjust
//			// the brightness properly
//			GFX->setTextureStageColorOp( 0, GFXTOPModulate );
//			GFX->setTextureStageColorArg1( 0, GFXTADiffuse );
//			GFX->setTextureStageColorArg2( 0, GFXTATexture );
//
//			// Set TFactor to ( 128, 128, 128, 255 ). These aren't magic numbers, detail
//			// textures are created such that values > 0.5/128/0x80 will brighten the
//			// area, and values less than 'middle' will darken the area.
//			static const ColorI sColorGrey( 128, 128, 128, 255 );
//			GFX->setTextureFactor( sColorGrey );
//
//			// LERP blend if the card supports it
//			if( canDoLerpDetail )
//			{
//				// Set up texture stage 1 with no texture. Still active, though. This will do
//				// the LERP.
//				//
//				// LERP blend op will do:
//				// Arg1 * Arg3 + (1 - Arg3) * Arg2
//				GFX->setTexture( 1, NULL );
//				GFX->setTextureStageColorOp( 1, GFXTOPLERP );
//				GFX->setTextureStageColorArg1( 1, GFXTACurrent );
//				GFX->setTextureStageColorArg2( 1, GFXTATFactor );
//				GFX->setTextureStageColorArg3( 1, GFXTADiffuse | GFXTAAlphaReplicate );
//			}
//			else if( canDoFourStageDetail )
//			{
//				// Ok, no LERP blend op. Can still do a LERP, it'll just take more stages.
//				// lerp( x, y, s ) = s * x + (1 - s) * y
//
//				// s * x => temp
//				GFX->setTexture( 1, NULL );
//				GFX->setTextureStageColorOp( 1, GFXTOPModulate );
//				GFX->setTextureStageColorArg1( 1, GFXTACurrent );
//				GFX->setTextureStageColorArg2( 1, GFXTADiffuse | GFXTAAlphaReplicate );
//				GFX->setTextureStageResultArg( 1, GFXTATemp );
//
//				// (1 - s) * y => current
//				GFX->setTexture( 2, NULL );
//				GFX->setTextureStageColorOp( 2, GFXTOPModulate );
//				GFX->setTextureStageColorArg1( 2, GFXTADiffuse | GFXTAAlphaReplicate | GFXTAComplement );
//				GFX->setTextureStageColorArg2( 2, GFXTATFactor );
//
//				// temp + current => current
//				GFX->setTexture( 3, NULL );
//				GFX->setTextureStageColorOp( 3, GFXTOPAdd );
//				GFX->setTextureStageColorArg1( 3, GFXTACurrent );
//				GFX->setTextureStageColorArg2( 3, GFXTATemp );
//			}
//
//			// Blend
//			GFX->setAlphaBlendEnable(true);
//			GFX->setSrcBlend(GFXBlendDestColor);
//			GFX->setDestBlend(GFXBlendSrcColor);
//
//			// Enable vertex color
//			GFX->setVertexColorEnable(true);
//
//			// Draw detail.
//			gChunkBatcher.renderChunkList( gChunkBatcher.mDetail, NULL, sgData, TerrBatch::vertexTypeDetail, NULL, blockTransform, cameraPosition, NULL );
//
//			// Clean up all the odd states so future renders don't get all messed up
//			GFX->setTextureStageTransform( 0, GFXTTFFDisable );
//			GFX->setVertexColorEnable(false);
//
//			for( int i = 0; i < 4; i++ )
//			{
//				GFX->setTextureStageColorOp( i, GFXTOPDisable );
//				GFX->setTextureStageColorArg1( i, GFXTACurrent );
//				GFX->setTextureStageColorArg2( i, GFXTATexture );
//				GFX->setTextureStageResultArg( i, GFXTACurrent );
//			}
//
//			GFX->setTextureStageColorOp( 0, GFXTOPModulate );
//		}
//
//	}
//
//	// render dynamic light materials...
//	if(m && (!mRenderingCommander))
//	{
//		Point4F clipmapMapping;
//
//		// Make sure our material is initialized so that we get lighting info.
//		m->init(sgData, (GFXVertexFlags)getGFXVertFlags((GFXVertexPCNTT*)NULL) );
//
//		GFX->setTextureStageAlphaOp(2, GFXTOPDisable);
//		GFX->setTextureStageAlphaOp(3, GFXTOPDisable);
//
//		for(U32 i=0; i<mDynamicLightCount; i++)
//		{
//			AssertFatal(mTerrainLighting, "Terrain lighting should be non-null if dynamic light count > 0, see TerrRender::buildLightArray");
//			// Do we have any chunks for this light? Skip if not.
//			U32 curLayer = 0;
//			for (; curLayer<16; curLayer++)
//				if(gChunkBatcher.mLightPasses[i][curLayer].size() > 0)
//					break;
//			if (curLayer == 16)
//				continue; // nothing lit
//
//			// Ok - there is real work to do here, so set up the light info
//			// and batch up our geometry.         
//			LightInfo *light      = mTerrainLights[i].light;
//			MatInstance* dmat = NULL;
//			TerrainRender::mTerrainLighting->setupLightStage(light, sgData, m, &dmat);
//			if (!dmat)
//				continue;
//
//			dmat->init(sgData, (GFXVertexFlags)getGFXVertFlags((GFXVertexPCNTT*)NULL));
//			while (dmat->setupPass(sgData))
//			{
//				// texture units for dlight materials are:
//				// 0 : diffuseMap  (diffuse texture of base material)
//				// 1 : blackfogMap (fog attenuation - optional)
//				// 2 : dlightMap   (distance attenuation)
//				// 3 : dlightMask  (directional color filter - optional)
//				GFX->setTextureStageAddressModeU(0, GFXAddressWrap );
//				GFX->setTextureStageAddressModeV(0, GFXAddressWrap );
//				GFX->setTextureStageColorOp(0, GFXTOPModulate);
//
//				GFX->setTexture(1, sgData.blackfogTex);
//				if(sgData.useFog)
//				{
//					GFX->setTextureStageAddressModeU(1, GFXAddressClamp );
//					GFX->setTextureStageAddressModeV(1, GFXAddressClamp );
//					GFX->setTextureStageColorOp(1, GFXTOPBlendTextureAlpha);
//				}
//				else if (fixedfunction)
//					GFX->setTextureStageColorOp(1, GFXTOPDisable);
//				else
//					GFX->setTextureStageColorOp(1, GFXTOPModulate);
//
//				if (!fixedfunction)
//				{
//					GFX->setTextureStageAddressModeU(2, GFXAddressClamp );
//					GFX->setTextureStageAddressModeV(2, GFXAddressClamp );
//				}
//
//				GFX->setVertexColorEnable(true);
//				GFX->setAlphaBlendEnable(true);
//				GFX->setSrcBlend(GFXBlendOne);
//				GFX->setDestBlend(GFXBlendOne);
//
//				for (; curLayer<16; curLayer++)
//				{
//					if(gChunkBatcher.mLightPasses[i][curLayer].size())
//					{
//						// yes this is used on both pixel shader and fixed function path
//						block->mClipMap->bindTexturesFF(curLayer, clipmapMapping);
//						gChunkBatcher.renderChunkList(gChunkBatcher.mLightPasses[i][curLayer], dmat, sgData, fixedfunction ? TerrBatch::vertexTypeDLight : TerrBatch::vertexTypeSingleTextureClipMapping, &clipmapMapping, blockTransform, cameraPosition, light);
//					}
//				}
//			}
//		}
//		GFX->setAlphaBlendEnable(false);
//	}
//
//	// And the fog chunks.
//	static ShaderData *sdAtlasShaderFog = NULL;
//	if(!sdAtlasShaderFog)
//	{
//		if(!Sim::findObject("AtlasShaderFog", sdAtlasShaderFog) || !sdAtlasShaderFog->getShader())
//		{
//			Con::errorf("AtlasClipMapBatcher::renderFog - no shader 'AtlasShaderFog' present!");
//			mCurrentBlock = NULL;
//			return;
//		}
//	}
//
//	if (!fixedfunction)
//	{
//		// Set up the fog shader and texture.
//		GFX->setShader(sdAtlasShaderFog->getShader());
//
//		Point4F fogConst(
//			gClientSceneGraph->getFogHeightOffset(), 
//			gClientSceneGraph->getFogInvHeightRange(), 
//			gClientSceneGraph->getVisibleDistanceMod(), 
//			0);
//		GFX->setVertexShaderConstF(22, &fogConst[0], 1);
//
//		MatrixF objTrans = block->getTransform();
//		objTrans.transpose();
//		GFX->setVertexShaderConstF( 12 /*VC_OBJ_TRANS*/, (float*)&objTrans, 4 );
//	}
//
//	GFX->setTexture(0, gClientSceneGraph->getFogTexture());
//	GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
//	GFX->setTextureStageAddressModeV(0, GFXAddressClamp);
//
//	// We need the eye pos but previous code deals with setting this up.
//
//	// Set blend mode and alpha test as well.
//	GFX->setAlphaBlendEnable(true);
//	GFX->setSrcBlend(GFXBlendSrcAlpha);
//	GFX->setDestBlend(GFXBlendInvSrcAlpha);
//
//	GFX->setAlphaTestEnable(true);
//	GFX->setAlphaFunc(GFXCmpGreaterEqual);
//	GFX->setAlphaRef(2);
//
//	gChunkBatcher.renderChunkList(gChunkBatcher.mFog, NULL, sgData, fixedfunction ? TerrBatch::vertexTypeFog : TerrBatch::vertexTypeFullClipMapping, NULL, blockTransform, cameraPosition, NULL);
//
//	GFX->setAlphaBlendEnable(false);
//	GFX->setAlphaTestEnable(false);
//	GFX->setTextureStageColorOp(0, GFXTOPModulate);
//	GFX->setTextureStageColorOp(1, GFXTOPDisable);
//
//	gChunkBatcher.reset();
//
//	FrameAllocator::setWaterMark(storedWaterMark);
//
//	PROFILE_END(TerrainRenderEmit);
//	PROFILE_END(TerrainRender);
//	mCurrentBlock = NULL;
//}


void TerrainRender::renderBlock(TerrainBlock *block, SceneState *state, MatInstance * m, SceneGraphData& sgData)
{
	PROFILE_START(TerrainRender);

	PROFILE_START(TerrainRenderSetup);
	U32 storedWaterMark = FrameAllocator::getWaterMark();

	{
		mSceneState  = state;
		mFarDistance = state->getVisibleDistance();

		mCameraToObject = GFX->getWorldMatrix();
		mCameraToObject.inverse();
		mCameraToObject.getColumn(3, &mCamPos);

		mCurrentBlock = block;
		mSquareSize   = block->getSquareSize();

		const F32 blockSize = F32(mSquareSize * TerrainBlock::BlockSize);

		MatrixF proj = GFX->getProjectionMatrix();

		//The projection matrix is stored and set properly, but the Canonizer is still complaining
		//This is the only place GFX_Canonizer is used, everywhere else it is commented out... so....
		//GFX_Canonizer("TerrainRender::renderBlock", __FILE__, __LINE__);

		// compute pixelError
		if(mScreenError >= 0.001f)
			mPixelError = 1.f / GFX->projectRadius(mScreenError, 1.f);
		else
			mPixelError = 0.000001f;

		buildClippingPlanes(state->mFlipCull);
		buildLightArray();

		// Calculate the potentially viewable area...
		S32 xStart = (S32)mFloor( (mCamPos.x - mFarDistance) / blockSize );
		S32 xEnd   = (S32)mCeil ( (mCamPos.x + mFarDistance) / blockSize );
		S32 yStart = (S32)mFloor( (mCamPos.y - mFarDistance) / blockSize );
		S32 yEnd   = (S32)mCeil ( (mCamPos.y + mFarDistance) / blockSize );

		// If we disable repeat, then start/end are always one.
		if(!mCurrentBlock->isTiling())
		{
			xStart = yStart = 0;
			xEnd = yEnd = 1;
		}

		S32 xExt   = (S32)(xEnd - xStart);
		S32 yExt   = (S32)(yEnd - yStart);

		PROFILE_END(TerrainRenderSetup);
		PROFILE_START(TerrainRenderRecurse);

		// Come up with a dummy height to work with...
		F32 height = fixedToFloat(block->getHeight(0,0));

		EdgeParent **bottomEdges = (EdgeParent **) FrameAllocator::alloc(sizeof(ChunkScanEdge *) * xExt);

		TerrainRender::allocRenderEdges(xExt, bottomEdges, false);

		ChunkCornerPoint *prevCorner = TerrainRender::allocInitialPoint(
			Point3F(
			xStart * blockSize, 
			yStart * blockSize, 
			height
			)
			);

		mTerrainOffset.set(xStart * TerrainBlock::BlockSquareWidth, 
			yStart * TerrainBlock::BlockSquareWidth);

		for(S32 x = 0; x < xExt; x++)
		{
			bottomEdges[x]->p1 = prevCorner;

			prevCorner = TerrainRender::allocInitialPoint(
				Point3F(
				(xStart + x + 1) * blockSize,
				(yStart    ) * blockSize, 
				height
				)
				);

			bottomEdges[x]->p2 = prevCorner;
		}

		for(S32 y = 0; y < yExt; y++)
		{
			// allocate the left edge:
			EdgeParent *left;
			TerrainRender::allocRenderEdges(1, &left, false);
			left->p1 = TerrainRender::allocInitialPoint(Point3F(xStart * blockSize, (yStart + y + 1) * blockSize, height));
			left->p2 = bottomEdges[0]->p1;

			for(S32 x = 0; x < xExt; x++)
			{
				// Allocate the right,...
				EdgeParent *right;
				TerrainRender::allocRenderEdges(1, &right, false);
				right->p1 = TerrainRender::allocInitialPoint(
					Point3F(
					(xStart + x + 1) * blockSize, 
					(yStart + y + 1) * blockSize, 
					height
					)
					);
				right->p2 = bottomEdges[x]->p2;

				// Allocate the top...
				EdgeParent *top;
				TerrainRender::allocRenderEdges(1, &top, false);
				top->p1 = left->p1;
				top->p2 = right->p1;

				// Process this block...

				//    -  set up for processing the block...
				mBlockOffset.set(x << TerrainBlock::BlockShift,
					y << TerrainBlock::BlockShift);

				mBlockPos.set((xStart + x) * blockSize,
					(yStart + y) * blockSize);

				//    - Do it...
				TerrainRender::processBlock(state, top, right, bottomEdges[x], left);

				// Clean up...
				left = right;
				bottomEdges[x] = top;
			}
		}

		GFX->setProjectionMatrix(proj);

		PROFILE_END(TerrainRenderRecurse);

	}

	PROFILE_START(TerrainRenderEmit);

	// Set up for the render...

	bool fixedfunction = GFX->getPixelShaderVersion() == 0.0;

	const MatrixF blockTransform = block->getTransform();
	const Point3F cameraPosition = state->getCameraPosition();
	//
	renderDif(block, state, m, sgData);

	//相机位置放在这里设置，以免没有细节纹理的时候，不会设置相机位置常量，从而导致绘制雾出错
	//细节shader和雾shader共用相机位置常量
	if (!fixedfunction)
	{
		GFX->setVertexShaderConstF(20, &mCamPos.x, 1, 3);
	}
	
	renderDetail(block, state, m, sgData);

	renderDynamicLights(block, state, m, sgData);

	renderFog(block, state, m, sgData);
	
	gChunkBatcher.reset();

	FrameAllocator::setWaterMark(storedWaterMark);

	PROFILE_END(TerrainRenderEmit);
	PROFILE_END(TerrainRender);
	mCurrentBlock = NULL;
}

#endif	//use_clipmap

#endif
void TerrainRender::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPDisable );
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	//新加
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->endStateBlock(mSetSB);

	//mAlphaBlendFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mAlphaBlendFalseSB);

	//mSetBinFixSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSColorVertex, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPDisable );
	GFX->endStateBlock(mSetBinFixSB);

	//mClearBinFixSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPMipMapLODBias, 0.0f);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->endStateBlock(mClearBinFixSB);

	//mVColorFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSColorVertex, false);
	GFX->endStateBlock(mVColorFalseSB);

	//mSetBiasSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPMipMapLODBias, 0.0f);
	GFX->setSamplerState(1, GFXSAMPMipMapLODBias, 0.0f);
	GFX->endStateBlock(mSetBiasSB);

	//mSetDetailShaderSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressWrap);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendDestColor);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendSrcColor);
	GFX->endStateBlock(mSetDetailShaderSB);

	//mDetailFixLerpSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(0, GFXTSSTextureTransformFlags, GFXTTFFCoord2D);
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressWrap);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(0, GFXTSSColorArg1, GFXTADiffuse);
	GFX->setTextureStageState(0, GFXTSSColorArg2, GFXTATexture);
	static const ColorI sColorGrey( 128, 128, 128, 255 );
	GFX->setRenderState(GFXRSTextureFactor, sColorGrey.getARGBPack());
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendDestColor);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendSrcColor);
	GFX->setRenderState(GFXRSColorVertex, true);

	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPLERP);
	GFX->setTextureStageState(1, GFXTSSColorArg1, GFXTACurrent);
	GFX->setTextureStageState(1, GFXTSSColorArg2, GFXTATFactor);
	GFX->setTextureStageState(1, GFXTSSColorArg0, GFXTADiffuse | GFXTAAlphaReplicate);
	GFX->endStateBlock(mDetailFixLerpSB);

	//mDetailFixForStageSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(0, GFXTSSTextureTransformFlags, GFXTTFFCoord2D);
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressWrap);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(0, GFXTSSColorArg1, GFXTADiffuse);
	GFX->setTextureStageState(0, GFXTSSColorArg2, GFXTATexture);
	//static const ColorI sColorGrey( 128, 128, 128, 255 );
	GFX->setRenderState(GFXRSTextureFactor, sColorGrey.getARGBPack());
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendDestColor);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendSrcColor);
	GFX->setRenderState(GFXRSColorVertex, true);

	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSColorArg1, GFXTACurrent);
	GFX->setTextureStageState(1, GFXTSSColorArg2, GFXTADiffuse | GFXTAAlphaReplicate);
	GFX->setTextureStageState(1, GFXTSSResultArg, GFXTATemp);

	GFX->setTextureStageState(2, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(2, GFXTSSColorArg1, GFXTADiffuse | GFXTAAlphaReplicate | GFXTAComplement);
	GFX->setTextureStageState(2, GFXTSSColorArg2, GFXTATFactor);

	GFX->setTextureStageState(3, GFXTSSColorOp, GFXTOPAdd);
	GFX->setTextureStageState(3, GFXTSSColorArg1, GFXTACurrent);
	GFX->setTextureStageState(3, GFXTSSColorArg2, GFXTATemp);
	GFX->endStateBlock(mDetailFixForStageSB);

	//mClearDetailFixSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(0, GFXTSSTextureTransformFlags, GFXTTFFDisable);
	GFX->setRenderState(GFXRSColorVertex, false);
	
	for( int i = 0; i < 4; i++ )
	{
		GFX->setTextureStageState(i, GFXTSSColorOp, GFXTOPDisable);
		GFX->setTextureStageState(i, GFXTSSColorArg1, GFXTACurrent);
		GFX->setTextureStageState(i, GFXTSSColorArg2, GFXTATexture);
		GFX->setTextureStageState(i, GFXTSSResultArg, GFXTACurrent);
	}
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->endStateBlock(mClearDetailFixSB);

	//mSetLitSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(2, GFXTSSAlphaOp, GFXTOPDisable);
	GFX->setTextureStageState(3, GFXTSSAlphaOp, GFXTOPDisable);
	GFX->endStateBlock(mSetLitSB);

	//mLitFogSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressWrap);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSColorVertex, true);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPBlendTextureAlpha);
	GFX->setSamplerState(1, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(1, GFXSAMPAddressV, GFXAddressClamp);
	GFX->endStateBlock(mLitFogSB);

	//mLitFixSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressWrap);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setRenderState(GFXRSColorVertex, true);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->endStateBlock(mLitFixSB);

	//mLitShaderSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressWrap);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressWrap);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setRenderState(GFXRSColorVertex, true);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPModulate);
	GFX->endStateBlock(mLitShaderSB);

	//mLitShaderAddrSB
	GFX->beginStateBlock();
	GFX->setSamplerState(2, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(2, GFXSAMPAddressV, GFXAddressClamp);
	GFX->endStateBlock(mLitShaderAddrSB);

	//mFogSetSB
	GFX->beginStateBlock();
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressClamp);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaRef, 2);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreaterEqual);
	GFX->endStateBlock(mFogSetSB);

	//mFogClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->endStateBlock(mFogClearSB);
}


void TerrainRender::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mAlphaBlendFalseSB)
	{
		mAlphaBlendFalseSB->release();
	}

	if (mSetBinFixSB)
	{
		mSetBinFixSB->release();
	}

	if (mClearBinFixSB)
	{
		mClearBinFixSB->release();
	}

	if (mVColorFalseSB)
	{
		mVColorFalseSB->release();
	}

	if (mSetBiasSB)
	{
		mSetBiasSB->release();
	}

	if (mSetDetailShaderSB)
	{
		mSetDetailShaderSB->release();
	}

	if (mDetailFixLerpSB)
	{
		mDetailFixLerpSB->release();
	}

	if (mDetailFixForStageSB)
	{
		mDetailFixForStageSB->release();
	}

	if (mClearDetailFixSB)
	{
		mClearDetailFixSB->release();
	}

	if (mSetLitSB)
	{
		mSetLitSB->release();
	}

	if (mLitFogSB)
	{
		mLitFogSB->release();
	}

	if (mLitFixSB)
	{
		mLitFixSB->release();
	}

	if (mLitShaderSB)
	{
		mLitShaderSB->release();
	}

	if (mLitShaderAddrSB)
	{
		mLitShaderAddrSB->release();
	}

	if (mFogSetSB)
	{
		mFogSetSB->release();
	}

	if (mFogClearSB)
	{
		mFogClearSB->release();
	}
}

void TerrainRender::initsb()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		mAlphaBlendFalseSB = new GFXD3D9StateBlock;
		mSetBinFixSB = new GFXD3D9StateBlock;
		mClearBinFixSB = new GFXD3D9StateBlock;
		mVColorFalseSB = new GFXD3D9StateBlock;
		mSetBiasSB = new GFXD3D9StateBlock;
		mSetDetailShaderSB = new GFXD3D9StateBlock;
		mDetailFixLerpSB = new GFXD3D9StateBlock;
		mDetailFixForStageSB = new GFXD3D9StateBlock;
		mClearDetailFixSB = new GFXD3D9StateBlock;
		mSetLitSB = new GFXD3D9StateBlock;
		mLitFogSB = new GFXD3D9StateBlock;
		mLitFixSB = new GFXD3D9StateBlock;
		mLitShaderSB = new GFXD3D9StateBlock;
		mLitShaderAddrSB = new GFXD3D9StateBlock;
		mFogSetSB = new GFXD3D9StateBlock;
		mFogClearSB = new GFXD3D9StateBlock;

		resetStateBlock();
	}
}

void TerrainRender::shutdownsb()
{
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mAlphaBlendFalseSB);
	SAFE_DELETE(mSetBinFixSB);
	SAFE_DELETE(mClearBinFixSB);
	SAFE_DELETE(mVColorFalseSB);
	SAFE_DELETE(mSetBiasSB);
	SAFE_DELETE(mSetDetailShaderSB);
	SAFE_DELETE(mDetailFixLerpSB);
	SAFE_DELETE(mDetailFixForStageSB);
	SAFE_DELETE(mClearDetailFixSB);
	SAFE_DELETE(mSetLitSB);
	SAFE_DELETE(mLitFogSB);
	SAFE_DELETE(mLitFixSB);
	SAFE_DELETE(mLitShaderSB);
	SAFE_DELETE(mLitShaderAddrSB);
	SAFE_DELETE(mFogSetSB);
	SAFE_DELETE(mFogClearSB);
}

void TerrainRender::renderDif( TerrainBlock *block, SceneState *state, MatInstance * m, SceneGraphData& sgData )
{
	BatchHelper::ChunkList &cl = gChunkBatcher.mDif;
	if(!cl.size())
	{
		return;
	}
	// Set up for the render...
	bool fixedfunction = GFX->getPixelShaderVersion() == 0.0;
	static ShaderData *sdAtlasShaderDif = NULL;
	if (!fixedfunction)
	{
		if(!sdAtlasShaderDif)
		{
			if(!Sim::findObject("AtlasShaderDif", sdAtlasShaderDif) || !sdAtlasShaderDif->getShader())
			{
				Con::errorf("TerrainRender::renderBlock - no shader 'sdAtlasShaderDif' present!");
				mCurrentBlock = NULL;
				return;
			}
		}
		GFX->setShader(sdAtlasShaderDif->getShader());
	}

	const MatrixF blockTransform = block->getTransform();
	const Point3F cameraPosition = state->getCameraPosition();

	GFX->setTextureStageColorOp(0, GFXTOPModulate);
	GFX->setTextureStageColorOp(1, GFXTOPModulate);
	GFX->setTextureStageColorOp(2, GFXTOPModulate);
	GFX->setTextureStageColorOp(3, GFXTOPModulate);
	GFX->setTextureStageColorOp(4, GFXTOPModulate);
	GFX->setTextureStageColorOp(5, GFXTOPModulate);
	GFX->setTextureStageColorOp(6, GFXTOPDisable);

	GFX->setTextureStageAddressModeU(0, GFXAddressWrap);
	GFX->setTextureStageAddressModeV(0, GFXAddressWrap);

	GFX->setTextureStageAddressModeU(1, GFXAddressWrap);
	GFX->setTextureStageAddressModeV(1, GFXAddressWrap);

	GFX->setTextureStageAddressModeU(2, GFXAddressWrap);
	GFX->setTextureStageAddressModeV(2, GFXAddressWrap);

	GFX->setTextureStageAddressModeU(3, GFXAddressWrap);
	GFX->setTextureStageAddressModeV(3, GFXAddressWrap);

	GFX->setTextureStageAddressModeU(4, GFXAddressClamp);
	GFX->setTextureStageAddressModeV(4, GFXAddressClamp);

	GFX->setTextureStageAddressModeU(5, GFXAddressClamp);
	GFX->setTextureStageAddressModeV(5, GFXAddressClamp);

	GFX->setAlphaBlendEnable(false);
	GFX->setAlphaTestEnable(false);
	GFX->setTextureStageMinFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageMinFilter(1, GFXTextureFilterLinear);
	GFX->setTextureStageMinFilter(2, GFXTextureFilterLinear);
	GFX->setTextureStageMinFilter(3, GFXTextureFilterLinear);
	GFX->setTextureStageMinFilter(4, GFXTextureFilterLinear);
	GFX->setTextureStageMinFilter(5, GFXTextureFilterLinear);

	GFX->setTextureStageMagFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageMagFilter(1, GFXTextureFilterLinear);
	GFX->setTextureStageMagFilter(2, GFXTextureFilterLinear);
	GFX->setTextureStageMagFilter(3, GFXTextureFilterLinear);
	GFX->setTextureStageMagFilter(4, GFXTextureFilterLinear);
	GFX->setTextureStageMagFilter(5, GFXTextureFilterLinear);

	GFX->setTextureStageMipFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageMipFilter(1, GFXTextureFilterLinear);
	GFX->setTextureStageMipFilter(2, GFXTextureFilterLinear);
	GFX->setTextureStageMipFilter(3, GFXTextureFilterLinear);
	GFX->setTextureStageMipFilter(4, GFXTextureFilterLinear);
	GFX->setTextureStageMipFilter(5, GFXTextureFilterLinear);
	//新加
	GFX->setZEnable(true);
	GFX->setZWriteEnable(true);

	GFX->setAlphaBlendEnable(false);
	if (fixedfunction)
	{
		// multi-pass fixed function clipmapping
		Point4F clipmapMapping;
		GFX->setVertexColorEnable(true);
		GFX->setSrcBlend(GFXBlendSrcAlpha);
		GFX->setDestBlend(GFXBlendInvSrcAlpha);
		GFX->setTextureStageColorOp(0, GFXTOPModulate);
		GFX->setTextureStageColorOp(1, GFXTOPDisable);
		// render the layers in order of least detail to most detail, each
		// with a vertex color controlling alpha blend transitions
		//for (S32 curLayer=end; curLayer>=start;curLayer--)
		//{
		//	block->mClipMap->bindTexturesFF(curLayer, clipmapMapping);
		//	gChunkBatcher.renderChunkList(cl, NULL, sgData, TerrBatch::vertexTypeSingleTextureClipMapping, &clipmapMapping, blockTransform, cameraPosition, NULL);
		//	GFX->setAlphaBlendEnable(true);

		//	// Reset any changes to mipmap bias
		//	GFX->setTextureStageLODBias(0, 0.0f);
		//}
		GFX->setVertexColorEnable(false);
	}
	else
	{
		//GFX->setVertexColorEnable(true);
		
		GFX->setSrcBlend(GFXBlendOne);
		GFX->setDestBlend(GFXBlendOne);

		//Point3F offset = block->getSubBlockOffset(i);
		//GFX->setVertexShaderConstF(10, &offset.x, 1);

		GFX->setTexture(5, block->tex3);
		for (U32 i=0; i<block->mTexNum/4 + 1; i++)
		{
			GFX->setTexture(0, block->mBaseMaterials[i * 4 + 0]);
			GFX->setTexture(1, block->mBaseMaterials[i * 4 + 1]);
			GFX->setTexture(2, block->mBaseMaterials[i * 4 + 2]);
			GFX->setTexture(3, block->mBaseMaterials[i * 4 + 3]);
			GFX->setTexture(4, block->mBlendTex[i]);
			gChunkBatcher.renderChunkList(cl, NULL, sgData, TerrBatch::vertexTypeFullClipMapping, NULL, blockTransform, cameraPosition, NULL);
			GFX->setAlphaBlendEnable(true);
		}
		//GFX->setVertexColorEnable(false);
		// Reset any changes to mipmap bias
		GFX->setTextureStageLODBias(0, 0.0f);
		GFX->setTextureStageLODBias(1, 0.0f);
	}
}

void TerrainRender::renderDifPass( TerrainBlock *block, SceneState *state, MatInstance * m, SceneGraphData& sgData )
{

	// Set up for the render...

	bool fixedfunction = GFX->getPixelShaderVersion() == 0.0;

	const MatrixF blockTransform = block->getTransform();
	const Point3F cameraPosition = state->getCameraPosition();

	GFX->setTextureStageColorOp(0, GFXTOPModulate);

	GFX->setTextureStageColorOp(1, GFXTOPModulate);
	GFX->setTextureStageColorOp(2, GFXTOPModulate);
	GFX->setTextureStageColorOp(3, GFXTOPModulate);
	GFX->setTextureStageColorOp(4, GFXTOPModulate);

	GFX->setTextureStageColorOp(5, GFXTOPModulate);
	GFX->setTextureStageColorOp(6, GFXTOPDisable);
	//GFX->setTextureStageColorOp(7, GFXTOPModulate);
	//GFX->setTextureStageColorOp(1, GFXTOPDisable);

	GFX->setTextureStageAddressModeU(0, GFXAddressWrap);
	GFX->setTextureStageAddressModeV(0, GFXAddressWrap);

	GFX->setTextureStageAddressModeU(1, GFXAddressWrap);
	GFX->setTextureStageAddressModeV(1, GFXAddressWrap);

	GFX->setTextureStageAddressModeU(2, GFXAddressWrap);
	GFX->setTextureStageAddressModeV(2, GFXAddressWrap);

	GFX->setTextureStageAddressModeU(3, GFXAddressWrap);
	GFX->setTextureStageAddressModeV(3, GFXAddressWrap);

	GFX->setTextureStageAddressModeU(4, GFXAddressClamp);
	GFX->setTextureStageAddressModeV(4, GFXAddressClamp);

	GFX->setTextureStageAddressModeU(5, GFXAddressClamp);
	GFX->setTextureStageAddressModeV(5, GFXAddressClamp);

	//GFX->setTextureStageAddressModeU(6, GFXAddressWrap);
	//GFX->setTextureStageAddressModeV(6, GFXAddressWrap);

	//GFX->setTextureStageAddressModeU(7, GFXAddressWrap);
	//GFX->setTextureStageAddressModeV(7, GFXAddressWrap);
	GFX->setAlphaBlendEnable(false);
	GFX->setAlphaTestEnable(false);
	GFX->setTextureStageMinFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageMinFilter(1, GFXTextureFilterLinear);
	GFX->setTextureStageMinFilter(2, GFXTextureFilterLinear);
	GFX->setTextureStageMinFilter(3, GFXTextureFilterLinear);
	GFX->setTextureStageMinFilter(4, GFXTextureFilterLinear);
	GFX->setTextureStageMinFilter(5, GFXTextureFilterLinear);

	GFX->setTextureStageMagFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageMagFilter(1, GFXTextureFilterLinear);
	GFX->setTextureStageMagFilter(2, GFXTextureFilterLinear);
	GFX->setTextureStageMagFilter(3, GFXTextureFilterLinear);
	GFX->setTextureStageMagFilter(4, GFXTextureFilterLinear);
	GFX->setTextureStageMagFilter(5, GFXTextureFilterLinear);

	GFX->setTextureStageMipFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageMipFilter(1, GFXTextureFilterLinear);
	GFX->setTextureStageMipFilter(2, GFXTextureFilterLinear);
	GFX->setTextureStageMipFilter(3, GFXTextureFilterLinear);
	GFX->setTextureStageMipFilter(4, GFXTextureFilterLinear);
	GFX->setTextureStageMipFilter(5, GFXTextureFilterLinear);
	//新加
	GFX->setZEnable(true);
	GFX->setZWriteEnable(true);

	static ShaderData *sdAtlasShaderDif = NULL;
	if(!sdAtlasShaderDif)
	{
		if(!Sim::findObject("AtlasShaderDif", sdAtlasShaderDif) || !sdAtlasShaderDif->getShader())
		{
			Con::errorf("TerrainRender::renderBlock - no shader 'sdAtlasShaderDif' present!");
			mCurrentBlock = NULL;
			return;
		}
	}
	if (!fixedfunction)
	{
		GFX->setShader(sdAtlasShaderDif->getShader());
	}

#ifdef NTJ_EDITOR
	static TerrainEditor *spTerrainEditor = NULL;
	if (spTerrainEditor == NULL)
	{
		Sim::findObject("ETerrainEditor", spTerrainEditor);
	}
#endif
	// Run through all the bins and draw each range.
	//for(S32 i=0; i<256; i++)
	//block->updateTexture();
	GFX->setTexture(5, block->tex3);
	for(S32 i=0; i<TerrainBlock::ChunkSize*TerrainBlock::ChunkSize; i++)
	{
		//BatchHelper::ChunkList &cl = gChunkBatcher.mDiffusePasses[i];
		BatchHelper::ChunkList &cl = gChunkBatcher.mDifPass[i];
		if(!cl.size())
			continue;

		for (U32 j=0; j<TerrainBlock::Max_Textures; j++)
		{
#ifdef NTJ_CLIENT
			GFX->setTexture(j, block->mTextures[i][j]);
#elif defined NTJ_EDITOR
			if (spTerrainEditor != NULL && j<TerrainBlock::Max_Texture_Layer)
			{
				if (1/*spTerrainEditor->getUseLayer(j)*/)
				{
					GFX->setTexture(j, block->mTextures[i][j]);
				} 
				else
				{
					GFX->setTexture(j, NULL);
				}
			}
			else
			{
				GFX->setTexture(j, block->mTextures[i][j]);
			}
#endif
		}

		PROFILE_START(TerrainRender_DrawDiffusePass);

		// There's stuff in this bin, so set up the clipmap and draw.
		//U8 start = i&0xF;
		//U8 end   = (i>>4)&0xF;
		S32 x = i&TerrainBlock::ChunkShiftMask;
		S32 y = (i>>4)&TerrainBlock::ChunkShiftMask;

		GFX->setAlphaBlendEnable(false);
		if (fixedfunction)
		{
			// multi-pass fixed function clipmapping
			Point4F clipmapMapping;
			GFX->setVertexColorEnable(true);
			GFX->setSrcBlend(GFXBlendSrcAlpha);
			GFX->setDestBlend(GFXBlendInvSrcAlpha);
			GFX->setTextureStageColorOp(0, GFXTOPModulate);
			GFX->setTextureStageColorOp(1, GFXTOPDisable);
			// render the layers in order of least detail to most detail, each
			// with a vertex color controlling alpha blend transitions
			//for (S32 curLayer=end; curLayer>=start;curLayer--)
			//{
			//	block->mClipMap->bindTexturesFF(curLayer, clipmapMapping);
			//	gChunkBatcher.renderChunkList(cl, NULL, sgData, TerrBatch::vertexTypeSingleTextureClipMapping, &clipmapMapping, blockTransform, cameraPosition, NULL);
			//	GFX->setAlphaBlendEnable(true);

			//	// Reset any changes to mipmap bias
			//	GFX->setTextureStageLODBias(0, 0.0f);
			//}
			GFX->setVertexColorEnable(false);
		}
		else
		{
			Point3F offset = block->getSubBlockOffset(i);
			GFX->setVertexShaderConstF(10, &offset.x, 1);
			gChunkBatcher.renderChunkList(cl, NULL, sgData, TerrBatch::vertexTypeFullClipMapping, NULL, blockTransform, cameraPosition, NULL);

			// Reset any changes to mipmap bias
			GFX->setTextureStageLODBias(0, 0.0f);
			GFX->setTextureStageLODBias(1, 0.0f);
		}
		PROFILE_END(TerrainRender_DrawDiffusePass);
	}

}

void TerrainRender::renderDetail( TerrainBlock *block, SceneState *state, MatInstance * m, SceneGraphData& sgData )
{

	// Grab the shader for this pass - replaceme w/ real code.
	static ShaderData *sdAtlasShaderDetail = NULL;
	if(!sdAtlasShaderDetail)
	{
		if(!Sim::findObject("AtlasShaderDetail", sdAtlasShaderDetail) || !sdAtlasShaderDetail->getShader())
		{
			Con::errorf("AtlasClipMapBatcher::renderDetail - no shader 'AtlasShaderDetail' present!");
			mCurrentBlock = NULL;

			return;
		}
	}

	bool fixedfunction = GFX->getPixelShaderVersion() == 0.0;
	const MatrixF blockTransform = block->getTransform();
	const Point3F cameraPosition = state->getCameraPosition();
	// Card profiler checks for what is needed to do these two implementations of 
	// fixed function detail blending. If neither are supported, it's going to 
	// look very strange.
	bool canDoLerpDetail = GFX->getCardProfiler()->queryProfile( "lerpDetailBlend", false );
	bool canDoFourStageDetail = GFX->getCardProfiler()->queryProfile( "fourStageDetailBlend", false );

	static bool sNoDetailWarning = false;

	if( fixedfunction && !( canDoLerpDetail || canDoFourStageDetail ) && !sNoDetailWarning )
	{
		Con::warnf( "---------------------------------------------------------------------------------------------" );
		Con::warnf( "TerrainRender::renderBlock -- Fixed function detail blending will fail without LERP blend op"  );
		Con::warnf( "                              or support for temp register, subtract op and 4 blend stages."   );
		Con::warnf( "---------------------------------------------------------------------------------------------" );
		sNoDetailWarning = true;
	}


	if (TerrainRender::mCurrentBlock->mDetailTextureHandle)
	{

		if (!fixedfunction)
		{
			GFX->setShader(sdAtlasShaderDetail->getShader());
			Point4F detailConst(
				TerrainRender::mCurrentBlock->mDetailDistance / 2.0f,
				1.f / (TerrainRender::mCurrentBlock->mDetailDistance / 2.0f), 
				block->mDetailScale,
				0);
			GFX->setVertexShaderConstF(50, &detailConst[0], 1);
			//GFX->setVertexShaderConstF(20, &mCamPos.x, 1);//移到上面去了

			Point4F brightnessScale(TerrainRender::mCurrentBlock->mDetailBrightness, 0, 0, 0);
			GFX->setPixelShaderConstF(0, &brightnessScale.x, 1);

			// Ok - next up, draw the detail chunks.   
			GFX->setTexture(0, TerrainRender::mCurrentBlock->mDetailTextureHandle);
			GFX->setTextureStageColorOp(0, GFXTOPModulate);
			GFX->setTextureStageAddressModeU(0, GFXAddressWrap);
			GFX->setTextureStageAddressModeV(0, GFXAddressWrap);
			GFX->setTextureStageColorOp(1, GFXTOPDisable);

			// This blend works, so don't question it. (It should be a multiply blend.)
			GFX->setAlphaBlendEnable(true);
			GFX->setSrcBlend(GFXBlendDestColor);
			GFX->setDestBlend(GFXBlendSrcColor);

			// Draw detail.
			gChunkBatcher.renderChunkList(gChunkBatcher.mDetail, NULL, sgData, TerrBatch::vertexTypeFullClipMapping, NULL, blockTransform, cameraPosition, NULL);
		}
		else
		{
			// Use a texture coord transform matrix to scale the detail texture
			MatrixF detailTexMatrix( true );
			detailTexMatrix.scale( Point3F( block->mDetailScale, block->mDetailScale, 0.0f ) );  

			GFX->setTextureStageTransform( 0, GFXTTFFCoord2D );
			GFX->setTextureMatrix( 0, detailTexMatrix );

			// Set up texture
			GFX->setTexture( 0, TerrainRender::mCurrentBlock->mDetailTextureHandle );
			GFX->setTextureStageAddressModeU( 0, GFXAddressWrap );
			GFX->setTextureStageAddressModeV( 0, GFXAddressWrap );

			// Multiply the diffuse color (vertex color) by the detail texture to adjust
			// the brightness properly
			GFX->setTextureStageColorOp( 0, GFXTOPModulate );
			GFX->setTextureStageColorArg1( 0, GFXTADiffuse );
			GFX->setTextureStageColorArg2( 0, GFXTATexture );

			// Set TFactor to ( 128, 128, 128, 255 ). These aren't magic numbers, detail
			// textures are created such that values > 0.5/128/0x80 will brighten the
			// area, and values less than 'middle' will darken the area.
			static const ColorI sColorGrey( 128, 128, 128, 255 );
			GFX->setTextureFactor( sColorGrey );

			// LERP blend if the card supports it
			if( canDoLerpDetail )
			{
				// Set up texture stage 1 with no texture. Still active, though. This will do
				// the LERP.
				//
				// LERP blend op will do:
				// Arg1 * Arg3 + (1 - Arg3) * Arg2
				GFX->setTexture( 1, NULL );
				GFX->setTextureStageColorOp( 1, GFXTOPLERP );
				GFX->setTextureStageColorArg1( 1, GFXTACurrent );
				GFX->setTextureStageColorArg2( 1, GFXTATFactor );
				GFX->setTextureStageColorArg3( 1, GFXTADiffuse | GFXTAAlphaReplicate );
			}
			else if( canDoFourStageDetail )
			{
				// Ok, no LERP blend op. Can still do a LERP, it'll just take more stages.
				// lerp( x, y, s ) = s * x + (1 - s) * y

				// s * x => temp
				GFX->setTexture( 1, NULL );
				GFX->setTextureStageColorOp( 1, GFXTOPModulate );
				GFX->setTextureStageColorArg1( 1, GFXTACurrent );
				GFX->setTextureStageColorArg2( 1, GFXTADiffuse | GFXTAAlphaReplicate );
				GFX->setTextureStageResultArg( 1, GFXTATemp );

				// (1 - s) * y => current
				GFX->setTexture( 2, NULL );
				GFX->setTextureStageColorOp( 2, GFXTOPModulate );
				GFX->setTextureStageColorArg1( 2, GFXTADiffuse | GFXTAAlphaReplicate | GFXTAComplement );
				GFX->setTextureStageColorArg2( 2, GFXTATFactor );

				// temp + current => current
				GFX->setTexture( 3, NULL );
				GFX->setTextureStageColorOp( 3, GFXTOPAdd );
				GFX->setTextureStageColorArg1( 3, GFXTACurrent );
				GFX->setTextureStageColorArg2( 3, GFXTATemp );
			}

			// Blend
			GFX->setAlphaBlendEnable(true);
			GFX->setSrcBlend(GFXBlendDestColor);
			GFX->setDestBlend(GFXBlendSrcColor);

			// Enable vertex color
			GFX->setVertexColorEnable(true);

			// Draw detail.
			gChunkBatcher.renderChunkList( gChunkBatcher.mDetail, NULL, sgData, TerrBatch::vertexTypeDetail, NULL, blockTransform, cameraPosition, NULL );

			// Clean up all the odd states so future renders don't get all messed up
			GFX->setTextureStageTransform( 0, GFXTTFFDisable );
			GFX->setVertexColorEnable(false);

			for( int i = 0; i < 4; i++ )
			{
				GFX->setTextureStageColorOp( i, GFXTOPDisable );
				GFX->setTextureStageColorArg1( i, GFXTACurrent );
				GFX->setTextureStageColorArg2( i, GFXTATexture );
				GFX->setTextureStageResultArg( i, GFXTACurrent );
			}

			GFX->setTextureStageColorOp( 0, GFXTOPModulate );
		}

	}

}

void TerrainRender::renderDynamicLights( TerrainBlock *block, SceneState *state, MatInstance * m, SceneGraphData& sgData )
{
	bool fixedfunction = GFX->getPixelShaderVersion() == 0.0;
	const MatrixF blockTransform = block->getTransform();
	const Point3F cameraPosition = state->getCameraPosition();
	// render dynamic light materials...
	if(m && (!mRenderingCommander))
	{
		Point4F clipmapMapping;

		// Make sure our material is initialized so that we get lighting info.
		m->init(sgData, (GFXVertexFlags)getGFXVertFlags((GFXVertexPCNTT*)NULL) );

		GFX->setTextureStageAlphaOp(2, GFXTOPDisable);
		GFX->setTextureStageAlphaOp(3, GFXTOPDisable);

		for(U32 i=0; i<mDynamicLightCount; i++)
		{
			AssertFatal(mTerrainLighting, "Terrain lighting should be non-null if dynamic light count > 0, see TerrRender::buildLightArray");
			// Do we have any chunks for this light? Skip if not.
			//U32 curLayer = 0;
			//for (; curLayer<16; curLayer++)
			//	if(gChunkBatcher.mLightPasses[i][curLayer].size() > 0)
			//		break;
			//if (curLayer == 16)
			//	continue; // nothing lit
			if (gChunkBatcher.mLit[i].size() < 1)
			{
				continue;
			}

			// Ok - there is real work to do here, so set up the light info
			// and batch up our geometry.         
			LightInfo *light      = mTerrainLights[i].light;
			//light->mColor.set(0.1, 1, 0.1, 1);
			MatInstance* dmat = NULL;
			TerrainRender::mTerrainLighting->setupLightStage(light, sgData, m, &dmat);
			if (!dmat)
				continue;

			dmat->init(sgData, (GFXVertexFlags)getGFXVertFlags((GFXVertexPCNTT*)NULL));
			while (dmat->setupPass(sgData))
			{
				// texture units for dlight materials are:
				// 0 : diffuseMap  (diffuse texture of base material)
				// 1 : blackfogMap (fog attenuation - optional)
				// 2 : dlightMap   (distance attenuation)
				// 3 : dlightMask  (directional color filter - optional)
				GFX->setTextureStageAddressModeU(0, GFXAddressWrap );
				GFX->setTextureStageAddressModeV(0, GFXAddressWrap );
				GFX->setTextureStageColorOp(0, GFXTOPModulate);

				GFX->setTexture(1, sgData.blackfogTex);
				if(sgData.useFog)
				{
					GFX->setTextureStageAddressModeU(1, GFXAddressClamp );
					GFX->setTextureStageAddressModeV(1, GFXAddressClamp );
					GFX->setTextureStageColorOp(1, GFXTOPBlendTextureAlpha);
				}
				else if (fixedfunction)
					GFX->setTextureStageColorOp(1, GFXTOPDisable);
				else
					GFX->setTextureStageColorOp(1, GFXTOPModulate);

				if (!fixedfunction)
				{
					GFX->setTextureStageAddressModeU(2, GFXAddressClamp );
					GFX->setTextureStageAddressModeV(2, GFXAddressClamp );
				}

				GFX->setVertexColorEnable(true);
				GFX->setAlphaBlendEnable(true);
				GFX->setSrcBlend(GFXBlendSrcColor);
				GFX->setDestBlend(GFXBlendOne);
				gChunkBatcher.renderChunkList(gChunkBatcher.mLit[i], dmat, sgData, fixedfunction ? TerrBatch::vertexTypeDLight : TerrBatch::vertexTypeSingleTextureClipMapping, &clipmapMapping, blockTransform, cameraPosition, light);

				//for (; curLayer<16; curLayer++)
				//{
				//	if(gChunkBatcher.mLightPasses[i][curLayer].size())
				//	{
				//		// yes this is used on both pixel shader and fixed function path
				//		//block->mClipMap->bindTexturesFF(curLayer, clipmapMapping);
				//		gChunkBatcher.renderChunkList(gChunkBatcher.mLightPasses[i][curLayer], dmat, sgData, fixedfunction ? TerrBatch::vertexTypeDLight : TerrBatch::vertexTypeSingleTextureClipMapping, &clipmapMapping, blockTransform, cameraPosition, light);
				//	}
				//}
			}
		}
		GFX->setAlphaBlendEnable(false);
	}

}

void TerrainRender::renderFog( TerrainBlock *block, SceneState *state, MatInstance * m, SceneGraphData& sgData )
{
	bool fixedfunction = GFX->getPixelShaderVersion() == 0.0;
	const MatrixF blockTransform = block->getTransform();
	const Point3F cameraPosition = state->getCameraPosition();

	// And the fog chunks.
	static ShaderData *sdAtlasShaderFog = NULL;
	if(!sdAtlasShaderFog)
	{
		if(!Sim::findObject("AtlasShaderFog", sdAtlasShaderFog) || !sdAtlasShaderFog->getShader())
		{
			Con::errorf("AtlasClipMapBatcher::renderFog - no shader 'AtlasShaderFog' present!");
			mCurrentBlock = NULL;
			return;
		}
	}

	if (!fixedfunction)
	{
		// Set up the fog shader and texture.
		GFX->setShader(sdAtlasShaderFog->getShader());

		Point4F fogConst(
			gClientSceneGraph->getFogHeightOffset(), 
			gClientSceneGraph->getFogInvHeightRange(), 
			gClientSceneGraph->getVisibleDistanceMod(), 
			0);
		GFX->setVertexShaderConstF(22, &fogConst[0], 1);

		MatrixF objTrans = block->getTransform();
		objTrans.transpose();
		GFX->setVertexShaderConstF( 12 /*VC_OBJ_TRANS*/, (float*)&objTrans, 4 );
	}

	GFX->setTexture(0, gClientSceneGraph->getFogTexture());
	GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
	GFX->setTextureStageAddressModeV(0, GFXAddressClamp);

	// We need the eye pos but previous code deals with setting this up.

	// Set blend mode and alpha test as well.
	GFX->setAlphaBlendEnable(true);
	GFX->setSrcBlend(GFXBlendSrcAlpha);
	GFX->setDestBlend(GFXBlendInvSrcAlpha);

	GFX->setAlphaTestEnable(true);
	GFX->setAlphaFunc(GFXCmpGreaterEqual);
	GFX->setAlphaRef(2);

	gChunkBatcher.renderChunkList(gChunkBatcher.mFog, NULL, sgData, fixedfunction ? TerrBatch::vertexTypeFog : TerrBatch::vertexTypeFullClipMapping, NULL, blockTransform, cameraPosition, NULL);

	GFX->setAlphaBlendEnable(false);
	GFX->setAlphaTestEnable(false);
	GFX->setTextureStageColorOp(0, GFXTOPModulate);
	GFX->setTextureStageColorOp(1, GFXTOPDisable);
}