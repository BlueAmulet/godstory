//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "processedMaterial.h"

#include "gfx/D3D9/gfxD3D9StateBlock.h"
GFXStateBlock *ProcessedMaterial::mAddSB			= NULL;
GFXStateBlock *ProcessedMaterial::mAddAlphaSB	= NULL;
GFXStateBlock *ProcessedMaterial::mMulSB			= NULL;
GFXStateBlock *ProcessedMaterial::mLerpAlphaSB	= NULL;
//////#define STATEBLOCK
void ProcessedMaterial::setBlendState(Material::BlendOp blendOp )
{
   switch( blendOp )
   {
   case Material::Add:
      {
#ifdef STATEBLOCK
			AssertFatal(mAddSB, "ProcessedMaterial::setBlendState -- mAddSB cannot be NULL.");
			mAddSB->apply();
#else
         GFX->setSrcBlend( GFXBlendOne );
         GFX->setDestBlend( GFXBlendOne );
#endif
         break;
      }
   case Material::AddAlpha:
      {
#ifdef STATEBLOCK
			AssertFatal(mAddAlphaSB, "ProcessedMaterial::setBlendState -- mAddAlphaSB cannot be NULL.");
			mAddAlphaSB->apply();
#else
         GFX->setSrcBlend( GFXBlendSrcAlpha );
         GFX->setDestBlend( GFXBlendOne );
#endif
         break;
      }
   case Material::Mul:
      {
#ifdef STATEBLOCK
			AssertFatal(mMulSB, "ProcessedMaterial::setBlendState -- mMulSB cannot be NULL.");
			mMulSB->apply();
#else
         GFX->setSrcBlend( GFXBlendDestColor );
         GFX->setDestBlend( GFXBlendZero );
#endif
         break;
      }
   case Material::LerpAlpha:
      {
#ifdef STATEBLOCK
			AssertFatal(mLerpAlphaSB, "ProcessedMaterial::setBlendState -- mLerpAlphaSB cannot be NULL.");
			mLerpAlphaSB->apply();
#else
         GFX->setSrcBlend( GFXBlendSrcAlpha );
         GFX->setDestBlend( GFXBlendInvSrcAlpha );
#endif
         break;
      }

   default:
      {
         // default to LerpAlpha
#ifdef STATEBLOCK
			AssertFatal(mLerpAlphaSB, "ProcessedMaterial::setBlendState -- mLerpAlphaSB cannot be NULL.");
			mLerpAlphaSB->apply();
#else
         GFX->setSrcBlend( GFXBlendSrcAlpha );
         GFX->setDestBlend( GFXBlendInvSrcAlpha );
#endif
         break;
      }
   }
}

void ProcessedMaterial::setBuffers(GFXVertexBufferHandleBase* vertBuffer, GFXPrimitiveBufferHandle* primBuffer)
{
   GFX->setVertexBuffer( *vertBuffer );
   GFX->setPrimitiveBuffer( *primBuffer );
}

GFXTexHandle ProcessedMaterial::createTexture( const char* filename, GFXTextureProfile *profile)
{
   // if '/', then path is specified, open normally
   if( dStrstr( filename, "/" ) )
   {
      return GFXTexHandle( filename, profile );
   }

   // otherwise, construct path
   char fullFilename[128];
   dStrncpy( fullFilename, sizeof(fullFilename), mMaterial->getPath(), dStrlen(mMaterial->getPath())+1 );
   dStrcat( fullFilename, sizeof(fullFilename), filename );

   return GFXTexHandle( fullFilename, profile );
}



void ProcessedMaterial::resetStateBlock()
{
	//mAddSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->endStateBlock(mAddSB);

	//mAddAlphaSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->endStateBlock(mAddAlphaSB);

	//mMulSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendDestColor);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendZero);
	GFX->endStateBlock(mMulSB);

	//mLerpAlphaSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->endStateBlock(mLerpAlphaSB);
}


void ProcessedMaterial::releaseStateBlock()
{
	if (mAddSB)
	{
		mAddSB->release();
	}

	if (mAddAlphaSB)
	{
		mAddAlphaSB->release();
	}

	if (mMulSB)
	{
		mMulSB->release();
	}

	if (mLerpAlphaSB)
	{
		mLerpAlphaSB->release();
	}
}

void ProcessedMaterial::initsb()
{
	if (mAddSB == NULL)
	{
		mAddSB = new GFXD3D9StateBlock;
		mAddSB->registerResourceWithDevice(GFX);
		mAddSB->mZombify = &releaseStateBlock;
		mAddSB->mResurrect = &resetStateBlock;

		mAddAlphaSB = new GFXD3D9StateBlock;
		mLerpAlphaSB = new GFXD3D9StateBlock;
		mMulSB = new GFXD3D9StateBlock;

		resetStateBlock();
	}
}

void ProcessedMaterial::shutdown()
{
	SAFE_DELETE(mAddSB);
	SAFE_DELETE(mAddAlphaSB);
	SAFE_DELETE(mLerpAlphaSB);
	SAFE_DELETE(mMulSB);
}
