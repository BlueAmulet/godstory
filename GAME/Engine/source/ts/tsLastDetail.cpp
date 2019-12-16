//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "ts/tsLastDetail.h"

#include "gfx/gfxDevice.h"
#include "ts/tsShape.h"
#include "ts/tsShapeInstance.h"
#include "gfx/gfxCanon.h"
#include "gfx/primBuilder.h"
#include "platform/profiler.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* TSLastDetail::mSetSB = NULL;
GFXStateBlock* TSLastDetail::mClearSB = NULL;

bool TSLastDetail::smDirtyMode = false;

Point2F TSLastDetail::smTVerts[4] = { Point2F(0,1),    Point2F(1,1),    Point2F(1,0),    Point2F(0,0) };
Point3F TSLastDetail::smNorms[4]  = { Point3F(0,-1,0), Point3F(0,-1,0), Point3F(0,-1,0), Point3F(0,-1,0) };

TSLastDetail::TSLastDetail(TSShapeInstance * shape,
                           U32 numEquatorSteps,
                           U32 numPolarSteps,
                           F32 polarAngle,
                           bool includePoles,
                           S32 dl, S32 dim)
{
   VECTOR_SET_ASSOCIATION(mBitmaps);
   VECTOR_SET_ASSOCIATION(mTextures);

   mNumEquatorSteps = numEquatorSteps;
   mNumPolarSteps = numPolarSteps;
   mPolarAngle = polarAngle;
   mIncludePoles = includePoles;

   F32 equatorStepSize = M_2PI_F / (F32) numEquatorSteps;
   F32 polarStepSize = numPolarSteps>0 ? (0.5f * M_PI_F - polarAngle) / (F32)numPolarSteps : 0.0f;

   PROFILE_START(TSLastDetail_snapshots);
   U32 i;
   F32 rotZ = 0;
   U32 start = Platform::getRealMilliseconds();
   for (i=0; i<numEquatorSteps; i++)
   {
      F32 rotX = numPolarSteps>0 ? polarAngle - 0.5f * M_PI_F : 0.0f;
      for (U32 j=0; j<2*numPolarSteps+1; j++)
      {
         MatrixF angMat;
         angMat.mul(MatrixF(EulerF(0,0,-M_PI_F+rotZ)),MatrixF(EulerF(rotX,0,0)));
         mBitmaps.push_back(shape->snapshot(dim,dim,true,angMat,dl,1.0f,true));
         rotX += polarStepSize;
      }
      rotZ += equatorStepSize;
   }

   if (includePoles)
   {
      MatrixF m1( EulerF( M_PI_F / 2.0f, 0, 0 ) );
      MatrixF m2( EulerF( -M_PI_F / 2.0f, 0, 0 ) );
      mBitmaps.push_back(shape->snapshot(dim,dim,true,m1,dl,1.0f,true));
      mBitmaps.push_back(shape->snapshot(dim,dim,true,m2,dl,1.0f,true));
   }
   
   Con::printf("Generated snapshots for TSLastDetail %s in %ums", shape->hShape->mSourceResource->getFullPath(),
      Platform::getRealMilliseconds() - start);
   PROFILE_END(TSLastDetail_snapshots);

   mTextures.setSize(mBitmaps.size());
   for (i=0; i<(U32)mBitmaps.size(); i++)
   {
      if (mBitmaps[i]) // snapshot routine may refuse to give us a bitmap sometimes...
         mTextures[i].set(mBitmaps[i], &GFXDefaultStaticDiffuseProfile, true);
      else
         mTextures[i] = NULL;
   }

   // bitmaps are gone (owned by the textures), clear list
   mBitmaps.clear();

   mPoints[0].set(-shape->mShape->radius,0, shape->mShape->radius);
   mPoints[1].set( shape->mShape->radius,0, shape->mShape->radius);
   mPoints[2].set( shape->mShape->radius,0,-shape->mShape->radius);
   mPoints[3].set(-shape->mShape->radius,0,-shape->mShape->radius);

   mCenter = shape->mShape->center;
}

TSLastDetail::~TSLastDetail()
{
   for (S32 i=0; i<mTextures.size(); i++) 
   {
      mTextures[i] = NULL;
   }

   AssertFatal(mBitmaps.size() == 0, "some bitmaps in this list, should be empty");
}

void TSLastDetail::chooseView(const MatrixF & mat, const Point3F & scale)
{
   scale; // ignore for now

   const F32 * m = (const F32*)&mat;
   F32 dotX = m[3] * m[0];
   F32 dotY = m[3] * m[1];
   F32 dotZ = m[3] * m[2];
   dotX    += m[7] * m[4];
   dotY    += m[7] * m[5];
   dotZ    += m[7] * m[6];
   dotX    += m[11] * m[8];
   dotY    += m[11] * m[9];
   dotZ    += m[11] * m[10];

   // which bmp to choose?
   F32 rotX = mIncludePoles || mNumPolarSteps ? mAcos(dotZ/mSqrt(dotX*dotX+dotY*dotY+dotZ*dotZ)) : 0.0f;
   F32 rotZ = 0.0f;
   AssertFatal(rotX>=0 && rotX<M_2PI,"TSLastDetail::chooseView: rotX out of range");
   if (mIncludePoles && (rotX<mPolarAngle || rotX>M_PI-mPolarAngle))
   {
      mBitmapIndex = mNumEquatorSteps * (2*mNumPolarSteps+1);
      if (rotX>mPolarAngle)
         mBitmapIndex++;
      mRotY = mAtan(m[5],m[4]);
   }
   else
   {
      F32 equatorStepSize = M_2PI_F / (F32) mNumEquatorSteps;
      F32 polarStepSize = mNumPolarSteps>0 ? (0.5f * M_PI_F - mPolarAngle) / (F32) mNumPolarSteps : 0.0f;
      rotZ = 0.999f * (mAtan(dotX,dotY) + M_PI_F); // the 0.99f makes sure we are in range
      AssertFatal(rotZ>=0 && rotZ<M_2PI_F,"TSLastDetail::chooseView: rotZ out of range");
      mBitmapIndex = ((S32)(rotZ/equatorStepSize)) * (S32)(2*mNumPolarSteps+1);
      if (mNumPolarSteps>0)
         mBitmapIndex = (U32)(mBitmapIndex + ((rotX-mPolarAngle) / polarStepSize));
      mRotY = mAtan(m[2],m[10]);
   }

   // make sure we don磘 get invalid bitmap index!
   mBitmapIndex = mClamp(mBitmapIndex, 0, mTextures.size()-1);
}

void TSLastDetail::render(F32 alpha)
{
   //glPushMatrix();
   GFX->pushWorldMatrix();

   // get camera matrix, adjust for shape center
   MatrixF mat;
   Point3F p,center;
   // JMQ: in TSE, object and camera transforms are separated at this point.  
   // multiple them now to get the equivalent of GL modelview
   mat.mul(TSMesh::getCamTrans(), GFX->getWorldMatrix());
   mat.getColumn(3,&p);
   mat.mulV(mCenter,&center);
   p += center;
   mat.setColumn(3,p);

   Point3F ones( 1, 1, 1 );
   chooseView(mat,ones);

   // following is a quicker version of mat.set(EulerF(0,rotY,0));
   // note:  we assume mat[12]=1 and mat[3]=mat[7]=mat[11]=0 to start with
   F32 * m = (F32*)mat;  // because [] operator isn't implemented on MatrixF, so it finds mat[0] ambiguous (const)
   AssertFatal(mFabs(m[15]-1.0f)<0.01f && mFabs(m[14])<0.01f && mFabs(m[13])<0.01f && mFabs(m[12])<0.01f,"TSLastDetail::render");
   if (mRotY*mRotY>0.0001f)
   {
      m[0] =  m[10] = mCos(mRotY);
      m[2] = mSin(mRotY); m[8] = -m[2];
      m[1] =  m[4] = m[6] = m[9] = 0.0f;
      m[5] =  1.0f;
   }
   else
   {
      m[0] = m[5] = m[10] = 1.0f;
      m[1] = m[2] = m[4] = m[6] = m[8] = m[9] = 0.0f;
   }

   GFX->setWorldMatrix(mat);
   renderBillboard(alpha);
   GFX->popWorldMatrix();
}

void TSLastDetail::renderBillboard(F32 alpha)
{
   // JMQ: this is a quick hack render so that I can test to see if the TSShape is generating
   // snapshot images properly.  this render doesn't handle fog and alpha and other things that it should.
   // this should be rendered with a shader too.

   //GFX_Canonizer("TSLastDetail::renderNoFog", __FILE__, __LINE__);
#ifdef STATEBLOCK
	AssertFatal(mSetSB, "TSLastDetail::renderBillboard -- mSetSB cannot be NULL.");
	mSetSB->apply();
	GFX->setTexture(0, mTextures[mBitmapIndex]);
#else
   GFX->setLightingEnable(false);

   // additive transparency
   GFX->setAlphaBlendEnable(true);
   GFX->setSrcBlend(GFXBlendSrcAlpha);
   GFX->setDestBlend(GFXBlendInvSrcAlpha);

   // texture
   GFX->setTextureStageColorOp(0, GFXTOPModulate);
   GFX->setTextureStageColorOp(1, GFXTOPDisable);

   GFX->setTexture(0, mTextures[mBitmapIndex]);

   GFX->setTextureStageMagFilter(0, GFXTextureFilterLinear );
   GFX->setTextureStageMinFilter(0, GFXTextureFilterLinear );
   GFX->setTextureStageMipFilter(0, GFXTextureFilterLinear);

   //glDepthMask(GL_FALSE);
   GFX->setZWriteEnable(false);
#endif


   // set color state
   PrimBuild::color4f(1,1,1,1);

   // should consider using a vertex buffer, not primitive builder...
   PrimBuild::begin( GFXTriangleFan, 4 );
   for (int i = 0; i < 4; ++i)
   {  
      // ignoring normals for now - not sure what kind of lighting model to have for these
      PrimBuild::texCoord2f( smTVerts[i].x, smTVerts[i].y );
      PrimBuild::vertex3f( mPoints[i].x, mPoints[i].y, mPoints[i].z ); 
   }
   PrimBuild::end();
#ifdef STATEBLOCK
	AssertFatal(mClearSB, "TSLastDetail::renderBillboard -- mClearSB cannot be NULL.");
	mClearSB->apply();
#else
   //GFX->setLightingEnable(true);
   //GFX->setZWriteEnable(true);
   GFX->setAlphaBlendEnable(false);
   GFX->setTextureStageColorOp(0, GFXTOPDisable);
#endif

}

void TSLastDetail::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPDisable );
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setSamplerState(0, GFXSAMPMagFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPMinFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPMipFilter, GFXTextureFilterLinear);
	GFX->endStateBlock(mSetSB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPDisable );
	GFX->endStateBlock(mClearSB);
}


void TSLastDetail::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mClearSB)
	{
		mClearSB->release();
	}
}

void TSLastDetail::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		mClearSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void TSLastDetail::shutdown()
{
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mClearSB);
}



