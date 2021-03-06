//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/consoleTypes.h"
#include "math/mMath.h"
#include "core/frameAllocator.h"
#include "gfx/gfxDevice.h"
#include "gfx/gBitmap.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sgUtil.h"
#include "platform/profiler.h"
#include "materials/matInstance.h"
#include "terrain/sky.h"
#include "terrain/terrRender.h"
#include "terrain/terrBatch.h"
#include "terrain/terrRender.h"
#include "terrain/waterBlock.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
#include "util/powerStat.h"

//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK

namespace TerrBatch
{

   // We buffer everything here then send it off to the GPU
   GFXAtlasVert2*          mVertexStore;
   GFXVertexPCNT*          mVertexStorePCNT;
   GFXVertexPCNTT*         mVertexStorePCNTT;
   U16                     mCurVertex;
   U16*                    mIndexStore;
   U16                     mCurIndex;
   GFXPrimitive*           mPrimitiveStore;
   U16                     mCurPrim;
   U32                     mCurXF;

   // Store previous counts for batching
   U16                     mLastIndex;
   U16                     mLastVertex;

   // Statistics
   S32                     mBiggestVertexBatch;
   S32                     mBiggestIndexBatch;

   const U32               mVertexStoreSize    = 16535;
   const U32               mIndexStoreSize     = 16535;
   const U32               mPrimitiveStoreSize = 4096;

   void init()
   {
      // Allocate space...
      mVertexStore    = new GFXAtlasVert2[mVertexStoreSize];
      mVertexStorePCNT = new GFXVertexPCNT[mVertexStoreSize];
      mVertexStorePCNTT = new GFXVertexPCNTT[mVertexStoreSize];
      mIndexStore     = new U16[mIndexStoreSize];
      mPrimitiveStore = new GFXPrimitive[mPrimitiveStoreSize];

/*      // Gratuitous mem usage spew
      Con::printf("Allocated terrain buffers...");
      Con::printf("    - Vertices      (bytes): %d", mVertexStore   .memSize());
      Con::printf("    - VerticesPCNT  (bytes): %d", mVertexStorePCNT.memSize());
      Con::printf("    - VerticesPCNTT (bytes): %d", mVertexStorePCNTT.memSize());
      Con::printf("    - Indices       (bytes): %d", mIndexStore    .memSize());
      Con::printf("    - Prims         (bytes): %d", mPrimitiveStore.memSize());
      Con::printf("  === Total         (bytes): %d", mVertexStore.memSize() + mVertexStorePCNT.memSize() + mVertexStorePCNTT.memSize() + mIndexStore.memSize() + mPrimitiveStore.memSize());
*/
      // Initialize
      mCurVertex  = 0;
      mCurIndex   = 0;
      mCurXF      = 1;

      mLastIndex  = 0;
      mLastVertex = 0;

      // Init statistics
      mBiggestIndexBatch = 0;
      mBiggestVertexBatch = 0;

      Con::addVariable("TRender::mBiggestIndexBatch", TypeS32, &mBiggestIndexBatch);
      Con::addVariable("TRender::mBiggestVertexBatch", TypeS32, &mBiggestVertexBatch);
   }

   void begin()
   {
      // Do nothing for now...
      mLastVertex = mCurVertex;
   }

   Point4F texGenS;
   Point4F texGenT;

   void setTexGen(Point4F s, Point4F t)
   {
      // We have to assign texture coords to each square, this lets us set the
      // tex transforms applied to the next batch of geometry we get. (ie,
      // these changes are applied at the next call to end())
      texGenS = s;
      texGenT = t;
   }

   Point4F &getTexGenS()
   {
      return texGenS;
   }

   Point4F &getTexGenT()
   {
      return texGenT;
   }

   void end(MatInstance * m, SceneGraphData& sgData, vertexType vtype, Point4F *clipmapMapping, bool final, const MatrixF &blockTransform, const Point3F &cameraPosition, LightInfo *light)
   {
      PROFILE_START(Terrain_batchEnd);

      // Advance the cache - we can't keep old verts because of texture co-ordinates.
      mCurXF++;

      // If we have nothing to draw, quick-out
/*      if( (mCurVertex== 0 || (mCurIndex-mLastIndex) == 0) && !final)
         return;
*/

      AssertFatal(mLastVertex < U16_MAX && mCurVertex < U16_MAX, "TerrBatch::end - too many verts!");
      if(!final)
      {
         // If we're not up to the threshold don't draw anything, just store the primitive...

         // Store the primitive
         // GFXTriangleList, 0, mCurVertex, 0, mCurIndex / 3
         GFXPrimitive &tempPrim = mPrimitiveStore[mCurPrim++];
         tempPrim.type          = GFXTriangleList;
         tempPrim.minIndex      = 0;
         tempPrim.numVertices   = mCurVertex;
         tempPrim.startIndex    = mLastIndex;
         tempPrim.numPrimitives = (mCurIndex-mLastIndex) / 3;

         if(mCurIndex-mLastIndex > mBiggestIndexBatch)
            mBiggestIndexBatch = mCurIndex-mLastIndex;

         if(mCurVertex-mLastVertex > mBiggestVertexBatch)
            mBiggestVertexBatch = mCurVertex-mLastVertex;

         // Update our last-state
         mLastIndex  = mCurIndex;
         mLastVertex = mCurVertex;

         // NOTE: These thresholds were set by careful readthroughs of the
         //       terrRender code.
         //       If you change the terrain code these need to be updated.
         //
         // Some reference values...
         //    - Most indices  commander chunk can generate - 768
         //    - Most vertices commander chunk can generate - 128
         //    - Most indices  normal    chunk can generate - 96
         //    - Most vertices normal    chunk can generate - 25

         // Default, normal chunks...
         U32 idxThresh  = 96;
         U32 vertThresh = 25;

         if(TerrainRender::mRenderingCommander)
         {
            // Special case for commander map
            idxThresh  = 768;
            vertThresh = 128;
         }

         if(mCurVertex < mVertexStoreSize-vertThresh && mCurIndex < (mIndexStoreSize-idxThresh))
         {
            // And bail if we don't HAVE to draw now...
            PROFILE_END(Terrain_batchEnd);
            return;
         }
      }

      // Blast everything out to buffers and draw it.

      // If there's nothing to draw... draw nothing
      if(mCurVertex == 0 || mCurIndex  == 0)
      {
         PROFILE_END(Terrain_batchEnd);
         return;
      }

      PROFILE_START(Terrain_TexGen);

      Point3F relative;

      const F32 blockTexCoordScale = 1.0f / (TerrainRender::mCurrentBlock->getSquareSize() * TerrainBlock::BlockSize);

      // Apply texgen to the new verts...
      if (vtype == vertexTypeFullClipMapping)
      {
         for(U32 i=0; i<mCurVertex; i++)
         {
            mVertexStore[i].texCoord.x = mVertexStore[i].point.x * blockTexCoordScale;
            mVertexStore[i].texCoord.y = mVertexStore[i].point.y * blockTexCoordScale;
         }
      }
      else if (vtype == vertexTypeSingleTextureClipMapping)
      {
         // Compute the fixedfunction vert stuff now
         AssertFatal(clipmapMapping != NULL, "TerrBatch::end - vertexTypeSingleTextureClipMapping requires clipmapMapping variable!");
         const F32 fadeConstant = 3.0f;
         const F32 blockTexCoordScale2 = blockTexCoordScale * clipmapMapping->z;
         for(U32 i=0; i<mCurVertex; i++)
         {
            mVertexStorePCNT[i].point = mVertexStore[i].point;
            mVertexStorePCNT[i].normal = mVertexStore[i].normal;
            mVertexStorePCNT[i].texCoord.x = mVertexStore[i].point.x * blockTexCoordScale2;
            mVertexStorePCNT[i].texCoord.y = mVertexStore[i].point.y * blockTexCoordScale2;
            relative.x = mVertexStorePCNT[i].texCoord.x - clipmapMapping->x;
            relative.y = mVertexStorePCNT[i].texCoord.y - clipmapMapping->y;
            relative.z = 0;
            // note: this uses 128.0f - instead of 255.0f - to hide some
            // transition artifacts at the edges (which are not visible
            // in the shader path due to its use of /2 in the vertex
            // shader and saturate(fade*2) in the pixel shader, which
            // allows sharp transitions to be interpolated more cleanly)
            mVertexStorePCNT[i].color.set(255, 255, 255, (U8)mClampF(128.0f - (relative.len() * (2.0f * fadeConstant) - (fadeConstant - 1.0f)) * 255.0f, 0.0f, 255.0f));
         }
      }
      else if (vtype == vertexTypeDLight)
      {
         // Compute the fixedfunction vert stuff now
         AssertFatal(clipmapMapping != NULL, "TerrBatch::end - vertexTypeDLight requires clipmapMapping variable!");
         AssertFatal(light != NULL, "TerrBatch::end - vertexTypeDLight requires light variable!");
         AssertFatal(light->mRadius > 0, "TerrBatch::end - vertexTypeDLight requires light->mRadius > 0!");
         const F32 blockTexCoordScale2 = blockTexCoordScale * clipmapMapping->z;
         const F32 heightOffset = gClientSceneGraph->getFogHeightOffset();
         const F32 inverseHeightRange = gClientSceneGraph->getFogInvHeightRange();
         const F32 inverseVisibleDistanceMod = 1.0f / gClientSceneGraph->getVisibleDistanceMod();
         Point3F worldPoint;
         const F32 lightRadius = light->mRadius;
         const Point3F lightPosition = light->mPos;
         F32 intensity;
         const F32 inverseLightRadius = 1.0f / lightRadius;
         // note: this imitates sgLightingModel only very loosely for
         // performance reasons, it does look very similar to the shader path
         for(U32 i=0; i<mCurVertex; i++)
         {
            mVertexStorePCNTT[i].point = mVertexStore[i].point;
            mVertexStorePCNTT[i].normal = mVertexStore[i].normal;
            mVertexStorePCNTT[i].texCoord[0].x = mVertexStore[i].point.x * blockTexCoordScale2;
            mVertexStorePCNTT[i].texCoord[0].y = mVertexStore[i].point.y * blockTexCoordScale2;
            blockTransform.mulP(mVertexStore[i].point, &worldPoint);
            relative = worldPoint - cameraPosition;
            mVertexStorePCNTT[i].texCoord[1].x = 1.0 - (relative.len() * inverseVisibleDistanceMod);
            mVertexStorePCNTT[i].texCoord[1].y = (worldPoint.z - heightOffset) * inverseHeightRange;
            relative = worldPoint - lightPosition;
            intensity = getMax(1.0f - relative.len() * inverseLightRadius, 0.0f);
            intensity = 512.0f * intensity;
            if (intensity > 0)
               mVertexStorePCNTT[i].color.set((U8)getMin(light->mColor.red * intensity, 255.0f), (U8)getMin(light->mColor.green * intensity, 255.0f), (U8)getMin(light->mColor.blue * intensity, 255.0f), 255);
            else
               mVertexStorePCNTT[i].color.set(0, 0, 0, 255);
         }
      }
      else if (vtype == vertexTypeFog)
      {
         const F32 heightOffset = gClientSceneGraph->getFogHeightOffset();
         const F32 inverseHeightRange = gClientSceneGraph->getFogInvHeightRange();
         const F32 inverseVisibleDistanceMod = 1.0f / gClientSceneGraph->getVisibleDistanceMod();
         Point3F worldPoint;
         for(U32 i=0; i<mCurVertex; i++)
         {
            mVertexStorePCNT[i].point = mVertexStore[i].point;
            mVertexStorePCNT[i].normal = mVertexStore[i].normal;
            blockTransform.mulP(mVertexStore[i].point, &worldPoint);
            relative = worldPoint - cameraPosition;
            mVertexStorePCNT[i].texCoord.x = 1.0 - (relative.len() * inverseVisibleDistanceMod);
            mVertexStorePCNT[i].texCoord.y = (worldPoint.z - heightOffset) * inverseHeightRange;
            mVertexStorePCNT[i].color.set(255, 255, 255, 255);
         }
      }
      // The only time 'vertexTypeDetail' is used is during a fixed-function detail pass.
      else if( vtype == vertexTypeDetail )
      {
         // Get detail distance squared to save us from sqrt
         const F32 detailDistanceSquared = TerrainRender::mCurrentBlock->mDetailDistance * TerrainRender::mCurrentBlock->mDetailDistance;

         // Detail Brightness done via assignment of color values
         const U8 colorByte = mClamp( 255 * TerrainRender::mCurrentBlock->mDetailBrightness, 0, 255 );

         Point3F worldPoint;
         for( U32 i = 0; i < mCurVertex; i++ )
         {
            mVertexStorePCNT[i].point = mVertexStore[i].point;
            mVertexStorePCNT[i].normal = mVertexStore[i].normal;
            mVertexStorePCNT[i].texCoord.x = mVertexStore[i].point.x * blockTexCoordScale;
            mVertexStorePCNT[i].texCoord.y = mVertexStore[i].point.y * blockTexCoordScale;

            // Transform vertex into world space
            blockTransform.mulP( mVertexStore[i].point, &worldPoint );
            relative = worldPoint - cameraPosition;

            // Alpha
            const F32 alpha = TerrainRender::mCurrentBlock->mDetailBrightness * ( 1.0f - ( relative.lenSquared() / detailDistanceSquared ) );

            // Assign alpha value to vert so the detail texture blend fades
            mVertexStorePCNT[i].color.set( colorByte, colorByte, colorByte, mClamp( alpha * 255, 0, 255 ) );
         }
      }
      else
      {
         for(U32 i=0; i<mCurVertex; i++)
         {
            mVertexStorePCNT[i].point = mVertexStore[i].point;
            mVertexStorePCNT[i].normal = mVertexStore[i].normal;
            mVertexStorePCNT[i].texCoord.x = mVertexStore[i].point.x * blockTexCoordScale;
            mVertexStorePCNT[i].texCoord.y = mVertexStore[i].point.y * blockTexCoordScale;
            mVertexStorePCNT[i].color.set(255, 255, 255, 255);
         }
      }

      PROFILE_END(Terrain_TexGen);

      PROFILE_START(Terrain_bufferCopy);

      // Do vertexes
      if (vtype == vertexTypeFullClipMapping)
      {
         GFXVertexBufferHandle<GFXAtlasVert2>  v(GFX, mCurVertex, GFXBufferTypeVolatile);
         PROFILE_START(Terrain_bufferCopy_lockV);
         v.lock();
         PROFILE_END(Terrain_bufferCopy_lockV);

         dMemcpy(&v[0], &mVertexStore[0], sizeof(GFXAtlasVert2) * mCurVertex);

         PROFILE_START(Terrain_bufferCopy_unlockV);
         v.unlock();
         PROFILE_END(Terrain_bufferCopy_unlockV);
         GFX->setVertexBuffer(v);
        
         CStat::Instance()->SetDesc((unsigned int)v.getPointer(),"terrain vertexTypeFullClipMapping");
      }
      else if (vtype == vertexTypeDLight)
      {
         GFXVertexBufferHandle<GFXVertexPCNTT> vPCNTT(GFX, mCurVertex, GFXBufferTypeVolatile);
         PROFILE_START(Terrain_bufferCopy_lockVPCNTT);
         vPCNTT.lock();
         PROFILE_END(Terrain_bufferCopy_lockVPCNTT);

         dMemcpy(&vPCNTT[0], &mVertexStorePCNTT[0], sizeof(GFXVertexPCNTT) * mCurVertex);

         PROFILE_START(Terrain_bufferCopy_unlockVPCNTT);
         vPCNTT.unlock();
         PROFILE_END(Terrain_bufferCopy_unlockVPCNTT);
         GFX->setVertexBuffer(vPCNTT);

         CStat::Instance()->SetDesc((unsigned int)vPCNTT.getPointer(),"terrain vertexTypeDLight");
      }
      else
      {
         GFXVertexBufferHandle<GFXVertexPCNT> vPCNT(GFX, mCurVertex, GFXBufferTypeVolatile);
         PROFILE_START(Terrain_bufferCopy_lockVPCNT);
         vPCNT.lock();
         PROFILE_END(Terrain_bufferCopy_lockVPCNT);

         dMemcpy(&vPCNT[0], &mVertexStorePCNT[0], sizeof(GFXVertexPCNT) * mCurVertex);

         PROFILE_START(Terrain_bufferCopy_unlockVPCNT);
         vPCNT.unlock();
         PROFILE_END(Terrain_bufferCopy_unlockVPCNT);
         GFX->setVertexBuffer(vPCNT);

         CStat::Instance()->SetDesc((unsigned int)vPCNT.getPointer(),"terrain other");
      }

      // Do primitives/indices
      U16          *idxBuff;
      GFXPrimitive *primBuff;

      GFXPrimitiveBufferHandle              p(GFX, mCurIndex, 1, GFXBufferTypeVolatile);

      PROFILE_START(Terrain_bufferCopy_lockP);
      p.lock(&idxBuff, &primBuff);
      PROFILE_END(Terrain_bufferCopy_lockP);

      dMemcpy(idxBuff, &mIndexStore[0], sizeof(U16) * mCurIndex);

      PROFILE_START(Terrain_bufferCopy_unlockP);
      p.unlock();
      PROFILE_END(Terrain_bufferCopy_unlockP);

      PROFILE_END(Terrain_bufferCopy);

      PROFILE_START(TRender_DIP);

      CStat::Instance()->SetDesc((unsigned int)p.getPointer(),"terrain index");

      U32 dipCall=0;

      // Now... RENDER!!!
      GFX->setPrimitiveBuffer(p);
#ifdef STATEBLOCK
		// And cull mode in particular.
		if( gClientSceneGraph->isReflectPass() )
		{
			AssertFatal(mCullCW, "TerrBatch::renderObject -- mCullCW cannot be NULL.");
			mCullCW->apply();
		}
		else
		{
			AssertFatal(mCullCCW, "TerrBatch::renderObject -- mCullCCW cannot be NULL.");
			mCullCCW->apply();
		}
#else
      // And cull mode in particular.
      if( gClientSceneGraph->isReflectPass() )
         GFX->setCullMode( GFXCullCW );
      else
         GFX->setCullMode( GFXCullCCW );
#endif


      U32 curPrim = 0;

      do
      {
         // This is all triangle list geometry, so we can batch adjacent stuff. Yay!
         // We do a fixup when we submit so all indices are in "global" number-space.

         // Figure out what span we can do
         U32 startIndex = mPrimitiveStore[curPrim].startIndex;
         U32 numPrims   = mPrimitiveStore[curPrim].numPrimitives;
         U32 minIndex   = mPrimitiveStore[curPrim].minIndex;
         U32 numVerts   = mPrimitiveStore[curPrim].numVertices;

         // Advance right off.
         curPrim++;

		 PROFILE_START(TRender_DIP_while);
         while(curPrim < mCurPrim)
         {
            // Update our range. All of this is consecutive in the IB...
            AssertFatal(startIndex + numPrims * 3 == mPrimitiveStore[curPrim].startIndex,
               "TerrBatch::end - non-contiguous IB ranges!");

            // Increase our prim count.
            numPrims += mPrimitiveStore[curPrim].numPrimitives;

            // See if we need to update our index range.
            minIndex = getMin(mPrimitiveStore[curPrim].minIndex, minIndex);

            if(mPrimitiveStore[curPrim].minIndex + mPrimitiveStore[curPrim].numVertices > minIndex + numVerts)
               numVerts = mPrimitiveStore[curPrim].minIndex + mPrimitiveStore[curPrim].numVertices - minIndex;

            curPrim++;
         }
		 PROFILE_END(TRender_DIP_while);

         // Now issue an aggregate draw.
         if(numPrims)
		 {
		    PROFILE_START(TRender_DIP_draw);
            GFX->drawIndexedPrimitive(GFXTriangleList, minIndex, numVerts, startIndex, numPrims);
			PROFILE_END(TRender_DIP_draw);
		 }

         dipCall++;
      }
      while(curPrim < mCurPrim);

      //Con::printf("---------- terrain draw (%d prim, %d dips)", mCurPrim, dipCall);

      // Clear all our buffers
      mCurIndex   = 0;
      mLastIndex  = 0;
      mCurVertex  = 0;
      mLastVertex = 0;
      mCurPrim    = 0;
      mCurXF++;

      PROFILE_END(TRender_DIP);
      // All done!

      PROFILE_END(Terrain_batchEnd);
   }

	GFXStateBlock* mCullCW = NULL;
	GFXStateBlock* mCullCCW = NULL;

	void resetStateBlock()
	{
		//mCullCW
		GFX->beginStateBlock();
		GFX->setRenderState(GFXRSCullMode, GFXCullCW);
		GFX->endStateBlock(mCullCW);

		//mCullCCW
		GFX->beginStateBlock();
		GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
		GFX->endStateBlock(mCullCCW);
	}


	void releaseStateBlock()
	{
		if (mCullCW)
		{
			mCullCW->release();
		}

		if (mCullCCW)
		{
			mCullCCW->release();
		}
	}

	void initsb()
	{
		if (mCullCW == NULL)
		{
			mCullCW = new GFXD3D9StateBlock;
			mCullCW->registerResourceWithDevice(GFX);
			mCullCW->mZombify = &releaseStateBlock;
			mCullCW->mResurrect = &resetStateBlock;

			mCullCCW = new GFXD3D9StateBlock;
			resetStateBlock();
		}
	}

	void shutdown()
	{
		SAFE_DELETE(mCullCW);
		SAFE_DELETE(mCullCCW);		
	}

	void clear()
	{
		SAFE_DELETE(mVertexStore);
		SAFE_DELETE(mVertexStorePCNT);
		SAFE_DELETE(mVertexStorePCNTT);
		SAFE_DELETE(mIndexStore);
		SAFE_DELETE(mPrimitiveStore);
	}
};


