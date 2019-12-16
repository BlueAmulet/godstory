//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "terrain/terrClipMapImageSource.h"
#include "materials/shaderData.h"
#include "sceneGraph/sceneGraph.h"

#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK
GFXStateBlock* TerrClipMapImageSource::mSetSB = NULL;

GFXShader* TerrClipMapImageSource::mShader = NULL;
TerrClipMapImageSource::TerrClipMapImageSource(GFXTexHandle& tex, F32 offset)
{
   mTex = tex;
   texOffset = offset;
}

TerrClipMapImageSource::~TerrClipMapImageSource()
{
	mTex.free();
}

const U32 TerrClipMapImageSource::getMipLevelCount()
{
   //[rene, 09/05/2008] Changed to return miplevel count rather than max index
   return getBinLog2(mTex->getWidth()) + 1;
}

void TerrClipMapImageSource::setInterestCenter( const Point2I origin, const U32 radius )
{
   // Do nothing, we don't page.
}

bool TerrClipMapImageSource::isDataAvailable( const U32 mipLevel, const RectI region )
{
   // Data always available, as we don't page.
   return true;
}

inline S32 safeModulo(S32 v, S32 m)
{
   while(v >= m)
      v -= m;
   while(v < 0)
      v += m;

   return v;
}

void clipAgainstGrid(const S32 gridSpacing, const RectI &rect, S32 &outCount, RectI *&outBuffer)
{
   // Check against X grids...
   const S32 startX = rect.point.x;
   const S32 endX   = rect.point.x + rect.extent.x;

   const S32 startGridX = mFloor(F32(startX) / F32(gridSpacing));
   const S32 endGridX   = mCeil(F32(endX) / F32(gridSpacing));

   // Check against Y grids...
   const S32 startY = rect.point.y;
   const S32 endY   = rect.point.y + rect.extent.y;

   const S32 startGridY = mFloor(F32(startY) / F32(gridSpacing));
   const S32 endGridY   = mCeil(F32(endY) / F32(gridSpacing));

   // Now we know the region of grid squares we overlap; so we can
   // allocate an array of RectI's to store our results in.
   outCount  = (endGridY - startGridY) * (endGridX - startGridX);
   outBuffer = new RectI[outCount];

   // Track where we are in the outBuffer.
   S32 emittedRects = 0;

   // All that's left is to run through all the grid squares our rect overlaps
   // and clip against each one in turn.
   for(S32 gridX=startGridX; gridX<endGridX; gridX++)
   {
      for(S32 gridY=startGridY; gridY<endGridY; gridY++)
      {
         // Ok - clip our rect into this grid square and store it out.
         RectI &outRect = outBuffer[emittedRects++];

         // Calculate extents of this grid square.
         const S32 minX = gridX * gridSpacing;
         const S32 maxX = minX + gridSpacing;
         const S32 minY = gridY * gridSpacing;
         const S32 maxY = minY + gridSpacing;

         // Now let's do some clamping!
         outRect.point.x = mClamp(startX, minX, maxX);
         outRect.point.y = mClamp(startY, minY, maxY);

         // This original version of this was adding 1, giving invalid extents (1025x1025 on 1024x1024 clipmap)
         outRect.extent.x = mClamp(endX, minX, maxX) - outRect.point.x;
         outRect.extent.y = mClamp(endY, minY, maxY) - outRect.point.y;

      }
   }
}

void TerrClipMapImageSource::copyBits( const U32 mipLevel, const ClipMap::ClipStackEntry *cse,
                                       const RectI srcRegion, U8 *bits, const U32 pitch, const U32 expandonecomponent )
{
}

bool TerrClipMapImageSource::isTiling()
   {
   return true;
}

F32 TerrClipMapImageSource::texelOffset()
{
   return texOffset;
}
GFXShader* TerrClipMapImageSource::getShader()
{
	if (mShader == NULL)
	{
		ShaderData *shader = NULL;
		if(!Sim::findObject("drawTex", shader) || !(mShader = shader->getShader()))
		{
			Con::errorf("TerrClipMapImageSource::getShader - could not find shader!");
		}
   }

	return mShader;
}

#ifdef STATEBLOCK
//三角形列表
void TerrClipMapImageSource::drawTex(const U32 mipLevel, const RectI srcRegion, GFXTexHandle &tex)
{
	// Tricky thing here, we have to flip the rectangle - so y becomes x and
	// vice versa. This is to conform to existing ClipMap convention.
	
	// Clip our input level against our size at this mip level...
	RectI *clippedRects = NULL;
	S32 clippedRectCount = 0;
	U32 width = mTex.getWidth();
	clipAgainstGrid(width, srcRegion, clippedRectCount, clippedRects);
	// Now copy all the rows.
	MatrixF proj = GFX->getProjectionMatrix();
	RectI viewPort = GFX->getViewport();
	GFX->pushWorldMatrix();

	GFX->pushActiveRenderTarget();
	GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
	myTarg->attachTexture(GFXTextureTarget::Color0, tex );
	GFX->setActiveRenderTarget( myTarg );

	// And some render states.
	GFX->setWorldMatrix(MatrixF(true));
	GFX->setProjectionMatrix(MatrixF(true));
	U32 w = tex.getWidth();
	GFX->setClipRect(RectI(0,0, w, w));

	AssertFatal(mSetSB, "TerrClipMapImageSource::drawTex -- mSetSB cannot be NULL.");
	mSetSB->apply();
	/************************************************************************/
	/*					                shader1                                  */
	/************************************************************************/
	GFX->setShader(getShader());

	GFX->setTexture(0, mTex);
	GFXVertexBufferHandle<GFXVertexPT> verts(GFX, 6*clippedRectCount, GFXBufferTypeVolatile );
	if (gClientSceneGraph)
	{
		gClientSceneGraph->setRectCount(gClientSceneGraph->getRectCount() + clippedRectCount);
	}

	verts.lock();
	for(S32 rectIdx=0; rectIdx<clippedRectCount; rectIdx++)
	{
		RectI rt = clippedRects[rectIdx];
		rt.point.x = safeModulo(rt.point.x, w);
		rt.point.y = safeModulo(rt.point.y, w);

		F32 left = rt.point.y;
		F32 top = rt.point.x;
		F32 right = rt.point.y + rt.extent.y;
		F32 bot = rt.point.x + rt.extent.x;

		//纹理坐标
		F32 codleft = left / width;
		F32 codtop = top / width;
		F32 codright = right / width;
		F32 codbot = bot / width;

		//顶点坐标
		left = rt.point.y - srcRegion.point.y;
		top = rt.point.x - rt.point.x;
		right = left + rt.extent.y;
		bot = top + rt.extent.x;

		verts[rectIdx*6 + 0].point.set( left,		top,		0 );
		verts[rectIdx*6 + 0].texCoord.set(codleft, codtop);

		verts[rectIdx*6 + 1].point.set( right,	top,		0);
		verts[rectIdx*6 + 1].texCoord.set(codright, codtop);

		verts[rectIdx*6 + 2].point.set( left,	bot,		0 );
		verts[rectIdx*6 + 2].texCoord.set(codleft, codbot);

		verts[rectIdx*6 + 3].point.set( right,	top,		0);
		verts[rectIdx*6 + 3].texCoord.set(codright, codtop);

		verts[rectIdx*6 + 4].point.set( left,	bot,		0 );
		verts[rectIdx*6 + 4].texCoord.set(codleft, codbot);

		verts[rectIdx*6 + 5].point.set( right,	bot,		0);
		verts[rectIdx*6 + 5].texCoord.set(codright, codbot);


	}
	verts.unlock();
	GFX->setVertexBuffer( verts );
	GFX->drawPrimitive(GFXTriangleList, 0, 2*clippedRectCount);
	GFX->popActiveRenderTarget();

	// Don't forget to nuke our rect list.
	delete[] clippedRects;

	// Reset texture stages.
	// For sanity, let's just purge all our texture states.
	//for(S32 i=0; i<GFX->getNumSamplers(); i++)
	//	GFX->setTexture(i, NULL);

	// And clear render states.
	//GFX->setZEnable(true);
	//GFX->setZWriteEnable(true);
	GFX->popWorldMatrix();
	GFX->setProjectionMatrix(proj);
	GFX->setViewport(viewPort);
	////GFX->setAlphaBlendEnable(false);
	////GFX->setAlphaTestEnable(false);

	//// Purge state before we end the scene - just in case.
	//GFX->updateStates();
}



#else
//三角形列表
void TerrClipMapImageSource::drawTex(const U32 mipLevel, const RectI srcRegion, GFXTexHandle &tex)
{
	// Tricky thing here, we have to flip the rectangle - so y becomes x and
	// vice versa. This is to conform to existing ClipMap convention.
	
	// Clip our input level against our size at this mip level...
	RectI *clippedRects = NULL;
	S32 clippedRectCount = 0;
	U32 width = mTex.getWidth();
	clipAgainstGrid(width, srcRegion, clippedRectCount, clippedRects);
	// Now copy all the rows.
	MatrixF proj = GFX->getProjectionMatrix();
	RectI viewPort = GFX->getViewport();
	GFX->pushWorldMatrix();

	GFX->pushActiveRenderTarget();
	GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
	myTarg->attachTexture(GFXTextureTarget::Color0, tex );
	GFX->setActiveRenderTarget( myTarg );

	// And some render states.
	GFX->setZEnable(false);
	GFX->setZWriteEnable(false);
	GFX->setWorldMatrix(MatrixF(true));
	GFX->setProjectionMatrix(MatrixF(true));
	U32 w = tex.getWidth();
	GFX->setClipRect(RectI(0,0, w, w));
	GFX->setCullMode( GFXCullNone );


	GFX->setTextureStageMagFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageMinFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageMipFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
	GFX->setTextureStageAddressModeV(0, GFXAddressClamp);
	GFX->setTextureStageColorOp(0, GFXTOPSelectARG1);
	GFX->setTextureStageColorArg1(0, GFXTATexture);

	GFX->setAlphaTestEnable(false);
	GFX->setAlphaBlendEnable(false);

	/************************************************************************/
	/*					                shader1                                  */
	/************************************************************************/
	GFX->setShader(getShader());

	GFX->setTexture(0, mTex);
	GFXVertexBufferHandle<GFXVertexPT> verts(GFX, 6*clippedRectCount, GFXBufferTypeVolatile );
	if (gClientSceneGraph)
	{
		gClientSceneGraph->setRectCount(gClientSceneGraph->getRectCount() + clippedRectCount);
	}

	verts.lock();
	for(S32 rectIdx=0; rectIdx<clippedRectCount; rectIdx++)
	{
		RectI rt = clippedRects[rectIdx];
		rt.point.x = safeModulo(rt.point.x, w);
		rt.point.y = safeModulo(rt.point.y, w);

		F32 left = rt.point.y;
		F32 top = rt.point.x;
		F32 right = rt.point.y + rt.extent.y;
		F32 bot = rt.point.x + rt.extent.x;

		//纹理坐标
		F32 codleft = left / width;
		F32 codtop = top / width;
		F32 codright = right / width;
		F32 codbot = bot / width;

		//顶点坐标
		left = rt.point.y - srcRegion.point.y;
		top = rt.point.x - rt.point.x;
		right = left + rt.extent.y;
		bot = top + rt.extent.x;

		verts[rectIdx*6 + 0].point.set( left,		top,		0 );
		verts[rectIdx*6 + 0].texCoord.set(codleft, codtop);

		verts[rectIdx*6 + 1].point.set( right,	top,		0);
		verts[rectIdx*6 + 1].texCoord.set(codright, codtop);

		verts[rectIdx*6 + 2].point.set( left,	bot,		0 );
		verts[rectIdx*6 + 2].texCoord.set(codleft, codbot);

		verts[rectIdx*6 + 3].point.set( right,	top,		0);
		verts[rectIdx*6 + 3].texCoord.set(codright, codtop);

		verts[rectIdx*6 + 4].point.set( left,	bot,		0 );
		verts[rectIdx*6 + 4].texCoord.set(codleft, codbot);

		verts[rectIdx*6 + 5].point.set( right,	bot,		0);
		verts[rectIdx*6 + 5].texCoord.set(codright, codbot);


	}
	verts.unlock();
	GFX->setVertexBuffer( verts );
	GFX->drawPrimitive(GFXTriangleList, 0, 2*clippedRectCount);
	GFX->popActiveRenderTarget();

	// Don't forget to nuke our rect list.
	delete[] clippedRects;

	// Reset texture stages.
	// For sanity, let's just purge all our texture states.
	//for(S32 i=0; i<GFX->getNumSamplers(); i++)
	//	GFX->setTexture(i, NULL);

	// And clear render states.
	//GFX->setZEnable(true);
	//GFX->setZWriteEnable(true);
	GFX->popWorldMatrix();
	GFX->setProjectionMatrix(proj);
	GFX->setViewport(viewPort);
	////GFX->setAlphaBlendEnable(false);
	////GFX->setAlphaTestEnable(false);

	//// Purge state before we end the scene - just in case.
	//GFX->updateStates();
}



#endif

void TerrClipMapImageSource::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);

	GFX->setSamplerState(0, GFXSAMPMagFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPMinFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPMipFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressClamp);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPSelectARG1);
	GFX->setTextureStageState(0, GFXTSSColorArg1, GFXTATexture);

	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mSetSB);
}


void TerrClipMapImageSource::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}
}

void TerrClipMapImageSource::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;
		resetStateBlock();
	}
}

void TerrClipMapImageSource::shutdown()
{
	SAFE_DELETE(mSetSB);
}

//矩形
//void TerrClipMapImageSource::drawTex(const U32 mipLevel, const RectI srcRegion, GFXTexHandle &tex)
//{
//	// Tricky thing here, we have to flip the rectangle - so y becomes x and
//	// vice versa. This is to conform to existing ClipMap convention.
//	// Clip our input level against our size at this mip level...
//	RectI *clippedRects = NULL;
//	S32 clippedRectCount = 0;
//	U32 width = mTex.getWidth();
//	clipAgainstGrid(width, srcRegion, clippedRectCount, clippedRects);
//	// Now copy all the rows.
//	MatrixF proj = GFX->getProjectionMatrix();
//	RectI viewPort = GFX->getViewport();
//	GFX->pushWorldMatrix();
//
//	GFX->pushActiveRenderTarget();
//	GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
//	myTarg->attachTexture(GFXTextureTarget::Color0, tex );
//	GFX->setActiveRenderTarget( myTarg );
//
//	// And some render states.
//	GFX->setZEnable(false);
//	GFX->setZWriteEnable(false);
//	GFX->setWorldMatrix(MatrixF(true));
//	GFX->setProjectionMatrix(MatrixF(true));
//	U32 w = tex.getWidth();
//	GFX->setClipRect(RectI(0,0, w, w));
//	GFX->setCullMode( GFXCullNone );
//	GFX->setTextureStageMagFilter(0, GFXTextureFilterLinear);
//	GFX->setTextureStageMinFilter(0, GFXTextureFilterLinear);
//	GFX->setTextureStageMipFilter(0, GFXTextureFilterLinear);
//	GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
//	GFX->setTextureStageAddressModeV(0, GFXAddressClamp);
//	GFX->setTextureStageColorOp(0, GFXTOPSelectARG1);
//	GFX->setTextureStageColorArg1(0, GFXTATexture);
//	GFX->setAlphaTestEnable(false);
//	GFX->setAlphaBlendEnable(false);
//
//	/************************************************************************/
//	/*					                shader1                                  */
//	/************************************************************************/
//	GFX->setShader(getShader());
//
//	GFX->setTexture(0, mTex);
//	GFXVertexBufferHandle<GFXVertexPT> verts(GFX, 4, GFXBufferTypeVolatile );
//	if (gClientSceneGraph)
//	{
//		gClientSceneGraph->setRectCount(gClientSceneGraph->getRectCount() + clippedRectCount);
//	}
//
//	for(S32 rectIdx=0; rectIdx<clippedRectCount; rectIdx++)
//	{
//		RectI rt = clippedRects[rectIdx];
//		rt.point.x = safeModulo(rt.point.x, w);
//		rt.point.y = safeModulo(rt.point.y, w);
//
//		F32 left = rt.point.y;
//		F32 top = rt.point.x;
//		F32 right = rt.point.y + rt.extent.y;
//		F32 bot = rt.point.x + rt.extent.x;
//		//纹理坐标
//		F32 codleft = left / width;
//		F32 codtop = top / width;
//		F32 codright = right / width;
//		F32 codbot = bot / width;
//		//顶点坐标
//		left = rt.point.y - srcRegion.point.y;
//		top = rt.point.x - rt.point.x;
//		right = left + rt.extent.y;
//		bot = top + rt.extent.x;
//
//		verts.lock();
//		verts[0].point.set( left,		top,		0 );
//		verts[0].texCoord.set(codleft, codtop);
//		verts[1].point.set( right,	top,		0);
//		verts[1].texCoord.set(codright, codtop);
//		verts[2].point.set( left,	bot,		0 );
//		verts[2].texCoord.set(codleft, codbot);
//		verts[3].point.set( right,	bot,		0);
//		verts[3].texCoord.set(codright, codbot);
//		verts.unlock();
//		GFX->setVertexBuffer( verts );
//
//		GFX->drawPrimitive(GFXTriangleStrip, 0, 2);
//
//	}
//
//	GFX->popActiveRenderTarget();
//
//	// Don't forget to nuke our rect list.
//	delete[] clippedRects;
//
//	// Reset texture stages.
//	// For sanity, let's just purge all our texture states.
//	//for(S32 i=0; i<GFX->getNumSamplers(); i++)
//	//	GFX->setTexture(i, NULL);
//	// And clear render states.
//	//GFX->setZEnable(true);
//	//GFX->setZWriteEnable(true);
//	GFX->popWorldMatrix();
//	GFX->setProjectionMatrix(proj);
//	GFX->setViewport(viewPort);
//
//}
//
