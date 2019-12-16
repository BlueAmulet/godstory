//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "glowBuffer.h"
#include "console/consoleTypes.h"
#include "gfx/gfxDevice.h"
#include "gfx/primBuilder.h"
#include "core/bitStream.h"
#include "sceneGraph/sceneGraph.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* GlowBuffer::mSetStateSB = NULL;
GFXStateBlock* GlowBuffer::mAlphaTestFalseSB = NULL;
GFXStateBlock* GlowBuffer::mSetCopyAlphaTrueSB = NULL;
GFXStateBlock* GlowBuffer::mSetCopyAlphaFalseSB = NULL;
GFXStateBlock* GlowBuffer::mClearCopySB = NULL;

#define GLOW_BUFF_SIZE 256

IMPLEMENT_CONOBJECT( GlowBuffer );

//****************************************************************************
// Glow Buffer
//****************************************************************************

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
GlowBuffer::GlowBuffer()
{
   mBlurShader = NULL;
   mBlurShaderName = NULL;

   mDisabled = false;
}

//--------------------------------------------------------------------------
// Init fields
//--------------------------------------------------------------------------
void GlowBuffer::initPersistFields()
{
   Parent::initPersistFields();
   
   addField("shader", TypeString,  Offset(mBlurShaderName, GlowBuffer));
   
}

//--------------------------------------------------------------------------
// onAdd
//--------------------------------------------------------------------------
bool GlowBuffer::onAdd()
{
   if( !Parent::onAdd() )
      return false;

   init();

   return true;
}

//--------------------------------------------------------------------------
// onRemove
//--------------------------------------------------------------------------
void GlowBuffer::onRemove()
{
   if( GFXDevice::devicePresent() )
   {
      GFX->unregisterTexCallback( mCallbackHandle );
   }

   Parent::onRemove();
}


//--------------------------------------------------------------------------
// Init
//--------------------------------------------------------------------------
void GlowBuffer::init()
{
   if( Con::getBoolVariable( "$pref::Video::disableGlowBuffer", true ) )
   {
      mDisabled = true;
      return;
   }

   if( GFXDevice::devicePresent() && mCallbackHandle != -1 )
   {
      GFX->registerTexCallback( texManagerCallback, (void*) this, mCallbackHandle );
   }

   mSurface[0].set( GLOW_BUFF_SIZE, GLOW_BUFF_SIZE, GFXFormatR8G8B8A8, &GFXDefaultRenderTargetProfile, 1 );
   mSurface[1].set( GLOW_BUFF_SIZE, GLOW_BUFF_SIZE, GFXFormatR8G8B8A8, &GFXDefaultRenderTargetProfile, 1 );

   mBlurShader = static_cast<ShaderData*>(Sim::findObject( mBlurShaderName ) );

   if( !mBlurShader )
      Con::errorf( "GlowBuffer: Could not find shader %s", mBlurShaderName );

   setupOrthoGeometry();

   // this is necessary for Show Tool use
   if( mBlurShader )
      mBlurShader->initShader();

   mTarget = GFX->allocRenderToTextureTarget();
}

//--------------------------------------------------------------------------
// Setup orthographic geometry for small buffer.
//--------------------------------------------------------------------------
void GlowBuffer::setupOrthoGeometry( )
{
   if( mVertBuff ) return;

   // 0.5 pixel offset - the render is spread from -1 to 1 ( 2 width )
   F32 copyOffset = 1.0 / GLOW_BUFF_SIZE;

   GFXVertexPT points[4];
   points[0].point      = Point3F( -1.0 - copyOffset, -1.0 + copyOffset, 0.0 );
   points[0].texCoord   = Point2F(  0.0, 1.0 );
   
   points[1].point      = Point3F( -1.0 - copyOffset,  1.0 + copyOffset, 0.0 );
   points[1].texCoord   = Point2F(  0.0, 0.0 );

   points[2].point      = Point3F(  1.0 - copyOffset,  1.0 + copyOffset, 0.0 );
   points[2].texCoord   = Point2F(  1.0, 0.0 );

   points[3].point      = Point3F(  1.0 - copyOffset, -1.0 + copyOffset, 0.0 );
   points[3].texCoord   = Point2F(  1.0, 1.0 );

   mVertBuff.set( GFX, 4, GFXBufferTypeStatic );
   GFXVertexPT *vbVerts = mVertBuff.lock();
   dMemcpy( vbVerts, points, sizeof(GFXVertexPT) * 4 );
   mVertBuff.unlock();

}

//--------------------------------------------------------------------------
// Setup orthographic projection
//--------------------------------------------------------------------------
MatrixF GlowBuffer::setupOrthoProjection()
{
   // set ortho projection matrix
   MatrixF proj = GFX->getProjectionMatrix();
   MatrixF newMat(true);
   GFX->setProjectionMatrix( newMat );
   GFX->pushWorldMatrix();
   GFX->setWorldMatrix( newMat );   
   GFX->setVertexShaderConstF( 0, (float*)&newMat, 4 );
   return proj;
}

//--------------------------------------------------------------------------
// Setup render states
//--------------------------------------------------------------------------

#ifdef STATEBLOCK
void GlowBuffer::setupRenderStates()
{
	AssertFatal(mSetStateSB, "GlowBuffer::setupRenderStates -- mSetStateSB cannot be NULL.");
	mSetStateSB->apply();

	// No color information is provided in the vertex stream, use this generic shader -patw
	GFX->setupGenericShaders( GFXDevice::GSTexture );
}
#else
void GlowBuffer::setupRenderStates()
{
   GFX->setTextureStageColorOp( 0, GFXTOPModulate );
   GFX->setTextureStageColorOp( 1, GFXTOPDisable );
   
   for( U32 i=0; i<4; i++ )
   {
      GFX->setTextureStageAddressModeU( i, GFXAddressClamp );
      GFX->setTextureStageAddressModeV( i, GFXAddressClamp );
   }

   // No color information is provided in the vertex stream, use this generic shader -patw
   GFX->setupGenericShaders( GFXDevice::GSTexture );
   GFX->setZEnable(false);
   GFX->setZWriteEnable(false);
}
#endif


//--------------------------------------------------------------------------
// Setup pixel offsets for the blur shader
//--------------------------------------------------------------------------
void GlowBuffer::setupPixelOffsets( Point4F offsets, bool horizontal )
{
   Point2F offsetConsts[4];
   
   if( horizontal )
   {
      offsetConsts[0].set( offsets.x / GLOW_BUFF_SIZE, 0.0 );
      offsetConsts[1].set( offsets.y / GLOW_BUFF_SIZE, 0.0 );
      offsetConsts[2].set( offsets.z / GLOW_BUFF_SIZE, 0.0 );
      offsetConsts[3].set( offsets.w / GLOW_BUFF_SIZE, 0.0 );
   }
   else
   {
      offsetConsts[0].set( 0.0, offsets.x / GLOW_BUFF_SIZE );
      offsetConsts[1].set( 0.0, offsets.y / GLOW_BUFF_SIZE );
      offsetConsts[2].set( 0.0, offsets.z / GLOW_BUFF_SIZE );
      offsetConsts[3].set( 0.0, offsets.w / GLOW_BUFF_SIZE );
   }

   
   GFX->setVertexShaderConstF( 4, (float*)offsetConsts[0], 1, 2 );
   GFX->setVertexShaderConstF( 5, (float*)offsetConsts[1], 1, 2 );
   GFX->setVertexShaderConstF( 6, (float*)offsetConsts[2], 1, 2 );
   GFX->setVertexShaderConstF( 7, (float*)offsetConsts[3], 1, 2 );
}

//--------------------------------------------------------------------------
// Perform 4 pass blur operation.  Ping pongs back and forth between 2
// render targets.  Does horizontal passes first, then vertical.
//--------------------------------------------------------------------------
void GlowBuffer::blur()
{
   // set blur shader
   if(!mBlurShader->getShader())
      return;
#ifdef STATEBLOCK
	AssertFatal(mAlphaTestFalseSB, "GlowBuffer::blur -- mAlphaTestFalseSB cannot be NULL.");
	mAlphaTestFalseSB->apply();
#else
   GFX->setAlphaTestEnable(false);
#endif

   mBlurShader->getShader()->process();

   // PASS 1
   //-------------------------------
   setupPixelOffsets( Point4F( 3.5, 2.5, 1.5, 0.5 ), true );

   mTarget->attachTexture( GFXTextureTarget::Color0, mSurface[1] );

   GFX->setTexture( 0, mSurface[0] );
   GFX->setTexture( 1, mSurface[0] );
   GFX->setTexture( 2, mSurface[0] );
   GFX->setTexture( 3, mSurface[0] );

   GFX->drawPrimitive( GFXTriangleFan, 0, 2 );
  
   
   // PASS 2
   //-------------------------------
   setupPixelOffsets( Point4F( -3.5, -2.5, -1.5, -0.5 ), true );

   mTarget->attachTexture( GFXTextureTarget::Color0, mSurface[0] );

   GFX->setTexture( 0, mSurface[1] );
   GFX->setTexture( 1, mSurface[1] );
   GFX->setTexture( 2, mSurface[1] );
   GFX->setTexture( 3, mSurface[1] );
   
   GFX->drawPrimitive( GFXTriangleFan, 0, 2 );

   
   // PASS 3
   //-------------------------------
   setupPixelOffsets( Point4F( 3.5, 2.5, 1.5, 0.5 ), false );

   mTarget->attachTexture( GFXTextureTarget::Color0, mSurface[1] );

   GFX->setTexture( 0, mSurface[0] );
   GFX->setTexture( 1, mSurface[0] );
   GFX->setTexture( 2, mSurface[0] );
   GFX->setTexture( 3, mSurface[0] );
   
   GFX->drawPrimitive( GFXTriangleFan, 0, 2 );

   
   // PASS 4
   //-------------------------------
   setupPixelOffsets( Point4F( -3.5, -2.5, -1.5, -0.5 ), false );

   mTarget->attachTexture( GFXTextureTarget::Color0, mSurface[0] );

   GFX->setTexture( 0, mSurface[1] );
   GFX->setTexture( 1, mSurface[1] );
   GFX->setTexture( 2, mSurface[1] );
   GFX->setTexture( 3, mSurface[1] );
   
   GFX->drawPrimitive( GFXTriangleFan, 0, 2 );

   mTarget->clearAttachments();
}

//--------------------------------------------------------------------------
// Copy to screen
//--------------------------------------------------------------------------
void GlowBuffer::copyToScreen( RectI &viewport )
{
   if( !mBlurShader || !mBlurShader->getShader() || mDisabled )
      return;

   if( !mSurface[0] || !mSurface[1] || !mSurface[2] ) return;

   PROFILE_SCOPE(Glow);

   // setup
   GFX->disableShaders();
   GFX->pushActiveRenderTarget();

   setupRenderStates();
   MatrixF proj = setupOrthoProjection();

   // set blur kernel
   Point4F kernel( 0.175f, 0.275f, 0.375f, 0.475f );
   GFX->setPixelShaderConstF( 0, (float*)kernel, 1 );
   
   // draw from full-size glow buffer to smaller buffer for 4 pass shader work
   GFX->setTexture( 0, mSurface[2] );

   mTarget->attachTexture(GFXTextureTarget::Color0, mSurface[0] );
   mTarget->attachTexture(GFXTextureTarget::DepthStencil, NULL );

   GFX->setActiveRenderTarget( mTarget );
   GFX->setVertexBuffer( mVertBuff );
   GFX->drawPrimitive( GFXTriangleFan, 0, 2 );

   blur();

   mTarget->clearAttachments();

   GFX->popActiveRenderTarget();

   // When the render target is changed, it appears that the view port
   // is reset.  We need to put it back into place here.
   GFX->setViewport( viewport );

   // blend final result to back buffer

   // Mod color texture for this draw because we are providing color information to the PrimBuilder -patw
   GFX->disableShaders();
   GFX->setupGenericShaders( GFXDevice::GSModColorTexture ); 
   GFX->setTexture( 0, mSurface[0] );
#ifdef STATEBLOCK

	if (Con::getBoolVariable("$translucentGlow", true))
	{
		AssertFatal(mSetCopyAlphaTrueSB, "GlowBuffer::copyToScreen -- mSetCopyAlphaTrueSB cannot be NULL.");
		mSetCopyAlphaTrueSB->apply();
	} 
	else
	{
		AssertFatal(mSetCopyAlphaFalseSB, "GlowBuffer::copyToScreen -- mSetCopyAlphaFalseSB cannot be NULL.");
		mSetCopyAlphaFalseSB->apply();
	}
#else
	for (U32 i = 1; i < GFX->getNumSamplers(); i++)
	{
		GFX->setTextureStageColorOp(i, GFXTOPDisable);
	}

	GFX->setAlphaBlendEnable( Con::getBoolVariable("$translucentGlow", true) );
	GFX->setSrcBlend( GFXBlendOne );
	GFX->setDestBlend( GFXBlendOne );
	GFX->setAlphaTestEnable( true );
	GFX->setAlphaRef( 1 );
	GFX->setAlphaFunc( GFXCmpGreaterEqual );
#endif

   Point2I resolution = GFX->getActiveRenderTarget()->getSize();
   F32 copyOffsetX = 1.0f / (F32)resolution.x;
   F32 copyOffsetY = 1.0f / (F32)resolution.y;
   
   // setup geometry and draw
   PrimBuild::begin( GFXTriangleFan, 4 );
      PrimBuild::color4f( 1.0f, 1.0f, 1.0f, 1.0f );
      PrimBuild::texCoord2f( 0.0f, 1.0f );
      PrimBuild::vertex3f( -1.0f - copyOffsetX, -1.0f + copyOffsetY, 0.0f );
      
      PrimBuild::texCoord2f( 0.0f, 0.0f );
      PrimBuild::vertex3f( -1.0f - copyOffsetX,  1.0f + copyOffsetY, 0.0f );
      
      PrimBuild::texCoord2f( 1.0f, 0.0f );
      PrimBuild::vertex3f(  1.0f - copyOffsetX,  1.0f + copyOffsetY, 0.0f );
      
      PrimBuild::texCoord2f( 1.0f, 1.0f );
      PrimBuild::vertex3f(  1.0f - copyOffsetX, -1.0f + copyOffsetY, 0.0f );
   PrimBuild::end();

   
   GFX->setProjectionMatrix( proj );
#ifdef STATEBLOCK
	AssertFatal(mClearCopySB, "GlowBuffer::copyToScreen -- mClearCopySB cannot be NULL.");
	mClearCopySB->apply();
#else
	// render state cleanup
	GFX->setAlphaTestEnable( false );
    GFX->setZEnable(true);
    GFX->setZWriteEnable(true);
	GFX->setAlphaBlendEnable( false );
#endif

   GFX->popWorldMatrix();
   GFX->disableShaders();

   mTarget->clearAttachments();
}

//--------------------------------------------------------------------------
// Set glow buffer as render target
//--------------------------------------------------------------------------
void GlowBuffer::setAsRenderTarget()
{
   // Make sure we have a final display target of the same size as the view
   // we're rendering.
   Point2I goalResolution = gClientSceneGraph->getDisplayTargetResolution();
   if(mSurface[2].isNull() || goalResolution != Point2I(mSurface[2].getWidth(), mSurface[2].getHeight()))
   {
      Con::printf("GlowBuffer (0x%p) - Resizing glow texture to be %dx%dpx", this, goalResolution.x, goalResolution.y);
      mSurface[2].set( goalResolution.x, goalResolution.y, GFXFormatR8G8B8A8, &GFXDefaultRenderTargetZBufferProfile, 1 );
   }
 
   // Set up render target.
   mTarget->attachTexture(GFXTextureTarget::Color0, mSurface[2]);
   mTarget->attachTexture(GFXTextureTarget::DepthStencil, GFXTextureTarget::sDefaultDepthStencil );
   GFX->setActiveRenderTarget( mTarget );
   GFX->clear( GFXClearTarget, ColorI(0,0,0,0), 1.0f, 0 );
}

//-----------------------------------------------------------------------------
// Texture manager callback - for resetting textures on video change
//-----------------------------------------------------------------------------
void GlowBuffer::texManagerCallback( GFXTexCallbackCode code, void *userData )
{
   GlowBuffer *glowBuff = (GlowBuffer *) userData;
   
   if( code == GFXZombify )
   {
      glowBuff->mSurface[2] = NULL;
      return;
   }

   if( code == GFXResurrect )
   {
      GFXTarget *target = GFX->getActiveRenderTarget();
      if ( target )
      {
         Point2I res = GFX->getActiveRenderTarget()->getSize();
         glowBuff->mSurface[2].set( res.x, res.y, GFXFormatR8G8B8A8, &GFXDefaultRenderTargetZBufferProfile, 1 );
      }
      return;
   }

}

void GlowBuffer::resetStateBlock()
{
	//mSetStateSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, false);
	for(S32 i=0; i<4; i++)
	{
		GFX->setSamplerState(i, GFXSAMPAddressU, GFXAddressClamp);
		GFX->setSamplerState(i, GFXSAMPAddressV, GFXAddressClamp);
	}
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->endStateBlock(mSetStateSB);

	//mAlphaTestFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->endStateBlock(mAlphaTestFalseSB);

	//mSetCopyAlphaTrueSB
	GFX->beginStateBlock();
	for (U32 i = 1; i < GFX->getNumSamplers(); i++)
	{
		GFX->setTextureStageState(i, GFXTSSColorOp, GFXTOPDisable);
	}
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreaterEqual);
	GFX->setRenderState(GFXRSAlphaRef, 1);
	GFX->endStateBlock(mSetCopyAlphaTrueSB);

	//mSetCopyAlphaFalseSB
	GFX->beginStateBlock();
	for (U32 i = 1; i < GFX->getNumSamplers(); i++)
	{
		GFX->setTextureStageState(i, GFXTSSColorOp, GFXTOPDisable);
	}
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreaterEqual);
	GFX->setRenderState(GFXRSAlphaRef, 1);
	GFX->endStateBlock(mSetCopyAlphaFalseSB);

	//mClearCopySB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->endStateBlock(mClearCopySB);
}


void GlowBuffer::releaseStateBlock()
{
	if (mSetStateSB)
	{
		mSetStateSB->release();
	}

	if (mAlphaTestFalseSB)
	{
		mAlphaTestFalseSB->release();
	}

	if (mSetCopyAlphaTrueSB)
	{
		mSetCopyAlphaTrueSB->release();
	}

	if (mSetCopyAlphaFalseSB)
	{
		mSetCopyAlphaFalseSB->release();
	}

	if (mClearCopySB)
	{
		mClearCopySB->release();
	}
}

void GlowBuffer::initsb()
{
	if (mSetStateSB == NULL)
	{
		mSetStateSB = new GFXD3D9StateBlock;
		mSetStateSB->registerResourceWithDevice(GFX);
		mSetStateSB->mZombify = &releaseStateBlock;
		mSetStateSB->mResurrect = &resetStateBlock;

		mAlphaTestFalseSB = new GFXD3D9StateBlock;
		mSetCopyAlphaTrueSB = new GFXD3D9StateBlock;
		mSetCopyAlphaFalseSB = new GFXD3D9StateBlock;
		mClearCopySB = new GFXD3D9StateBlock;

		resetStateBlock();
	}
}

void GlowBuffer::shutdown()
{
	SAFE_DELETE(mSetStateSB);
	SAFE_DELETE(mAlphaTestFalseSB);
	SAFE_DELETE(mSetCopyAlphaTrueSB);
	SAFE_DELETE(mSetCopyAlphaFalseSB);
	SAFE_DELETE(mClearCopySB);
}

