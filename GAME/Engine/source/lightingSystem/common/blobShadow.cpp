//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gfx/gfxDevice.h"
#include "gfx/primBuilder.h"
#include "math/mathUtils.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/lightInfo.h"
#include "lightingSystem/common/blobShadow.h"
#include "sceneGraph/lightingInterfaces.h"
#include "T3D/shapeBase.h"
#include "ts/tsMesh.h"
#include "sceneGraph/lightManager.h"

#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* BlobShadow::mSetSB = NULL;
GFXStateBlock* BlobShadow::mSetZBiasSB = NULL;
GFXStateBlock* BlobShadow::mClearZBiasSB = NULL;

DepthSortList BlobShadow::smDepthSortList;
GFXTexHandle BlobShadow::smGenericShadowTexture = NULL;
S32 BlobShadow::smGenericShadowDim = 32;
// <Edit> [3/30/2009 joy] 修改投影的目标对象
U32 BlobShadow::smShadowMask = InteriorObjectType|StaticShapeObjectType|StaticObjectType|TerrainObjectType|AtlasObjectType;
F32 BlobShadow::smGenericRadiusSkew = 0.4f; // shrink radius of shape when it always uses generic shadow...

Box3F gBlobShadowBox;
SphereF gBlobShadowSphere;
Point3F gBlobShadowPoly[4];

//--------------------------------------------------------------

BlobShadow::BlobShadow(SceneObject* parentObject, LightInfo* light, TSShapeInstance* shapeInstance)
{
   mParentObject = parentObject;
   mShapeBase = dynamic_cast<ShapeBase*>(parentObject);
   mParentLight = light;
   mShapeInstance = shapeInstance;
   mRadius = 0.0f;
   mLastRenderTime = 0;

   generateGenericShadowBitmap(smGenericShadowDim);

   // Grab a callback from the texture manager to deal with zombification.
   GFX->registerTexCallback(&BlobShadow::texCB, this, mTexCallbackHandle);

   mShadowBuffer = NULL;
   mShadowPBuffer = NULL;
}

BlobShadow::~BlobShadow()
{
   mShadowBuffer = NULL;
   mShadowPBuffer = NULL;

   GFX->unregisterTexCallback(mTexCallbackHandle);
}

void BlobShadow::texCB( GFXTexCallbackCode code, void *userData )
{
	if (code == GFXZombify)
	{
		BlobShadow* sp = static_cast<BlobShadow*>(userData);
		sp->mShadowBuffer = NULL;
		sp->mShadowPBuffer = NULL;
	}
}


bool BlobShadow::shouldRender(F32 camDist)
{
   Point3F lightDir;

   if (mShapeBase && mShapeBase->getFadeVal() < TSMesh::VISIBILITY_EPSILON)
      return false;

   F32 shadowLen = 10.0f * mShapeInstance->getShape()->radius;
   Point3F pos = mShapeInstance->getShape()->center;

   // this is a bit of a hack...move generic shadows towards feet/base of shape
   pos *= 0.5f;
   pos.convolve(mParentObject->getScale());
   mParentObject->getRenderTransform().mulP(pos);

   //if(mParentLight->mType == LightInfo::Vector)
   //{
   //   lightDir = mParentLight->mDirection;
   //}
   //else
   //{
   //   lightDir = pos - mParentLight->mPos;
   //   lightDir.normalize();
   //}

   //Ray: 修改blobShadow为固定在脚底中心
   lightDir = Point3F(0,0,-1);

   // pos is where shadow will be centered (in world space)
   setRadius(mShapeInstance, mParentObject->getScale());
   bool render = prepare(pos, lightDir, shadowLen);
   return render;
}

void BlobShadow::generateGenericShadowBitmap(S32 dim)
{
   if(smGenericShadowTexture)
      return;
   GBitmap * bitmap = new GBitmap(dim,dim,false,GFXFormatR8G8B8A8);
   U8 * bits = bitmap->getWritableBits();
   dMemset(bits, 0, dim*dim*4);
   S32 center = dim >> 1;
   F32 invRadiusSq = 1.0f / (F32)(center*center);
   F32 tmpF;
   for (S32 i=0; i<dim; i++)
   {
      for (S32 j=0; j<dim; j++)
      {
         tmpF = (F32)((i-center)*(i-center)+(j-center)*(j-center)) * invRadiusSq;
         U8 val = tmpF>0.99f ? 0 : (U8)(180.0f*(1.0f-tmpF)); // 180 out of 255 max
         bits[(i*dim*4)+(j*4)+3] = val;
      }
   }
   smGenericShadowTexture = GFX->getTextureManager()->createTexture(bitmap, &GFXDefaultStaticDiffuseProfile, false);
}

//--------------------------------------------------------------

void BlobShadow::setLightMatrices(const Point3F & lightDir, const Point3F & pos)
{
   AssertFatal(mDot(lightDir,lightDir)>0.0001f,"BlobShadow::setLightDir: light direction must be a non-zero vector.");

   // construct light matrix
   Point3F x,z;
   if (mFabs(lightDir.z)>0.001f)
   {
      // mCross(Point3F(1,0,0),lightDir,&z);
      z.x = 0.0f;
      z.y =  lightDir.z;
      z.z = -lightDir.y;
      z.normalize();
      mCross(lightDir,z,&x);
   }
   else
   {
      mCross(lightDir,Point3F(0,0,1),&x);
      x.normalize();
      mCross(x,lightDir,&z);
   }

   mLightToWorld.identity();
   mLightToWorld.setColumn(0,x);
   mLightToWorld.setColumn(1,lightDir);
   mLightToWorld.setColumn(2,z);
   mLightToWorld.setColumn(3,pos);

   mWorldToLight = mLightToWorld;
   mWorldToLight.inverse();
}

void BlobShadow::setRadius(F32 radius)
{
   mRadius = radius;
}

void BlobShadow::setRadius(TSShapeInstance * shapeInstance, const Point3F & scale)
{
   const Box3F & bounds = shapeInstance->getShape()->bounds;
   F32 dx = 0.5f * (bounds.max.x-bounds.min.x) * scale.x;
   F32 dy = 0.5f * (bounds.max.y-bounds.min.y) * scale.y;
   F32 dz = 0.5f * (bounds.max.z-bounds.min.z) * scale.z;
   mRadius = mSqrt(dx*dx+dy*dy+dz*dz);
}


//--------------------------------------------------------------

bool BlobShadow::prepare(const Point3F & pos, Point3F lightDir, F32 shadowLen)
{
   if (mPartition.empty() || mLastPos!=pos)
   {
      // --------------------------------------
      // 1.
      F32 dirMult = (1.0f) * (1.0f);
      if (dirMult < 0.99f)
      {
         lightDir.z *= dirMult;
         lightDir.z -= 1.0f - dirMult;
      }
      lightDir.normalize();
      shadowLen *= (1.0f) * (1.0f);

      // --------------------------------------
      // 2. get polys
      F32 radius = mRadius;
      radius *= smGenericRadiusSkew;
      buildPartition(pos,lightDir,radius,shadowLen);

	  mLastPos = pos;
   }
   updatePartition();
   if (mPartition.empty())
      // no need to draw shadow if nothing to cast it onto
      return false;


   return true;
}

//--------------------------------------------------------------

void BlobShadow::buildPartition(const Point3F & p, const Point3F & lightDir, F32 radius, F32 shadowLen)
{
   setLightMatrices(lightDir,p);

   Point3F extent(2.0f*radius,shadowLen,2.0f*radius);
   smDepthSortList.clear();
   smDepthSortList.set(mWorldToLight,extent);
   smDepthSortList.setInterestNormal(lightDir);

   if (shadowLen<1.0f)
      // no point in even this short of a shadow...
      shadowLen = 1.0f;
   mInvShadowDistance = 1.0f / shadowLen;

   // build world space box and sphere around shadow

   Point3F x,y,z;
   mLightToWorld.getColumn(0,&x);
   mLightToWorld.getColumn(1,&y);
   mLightToWorld.getColumn(2,&z);
   x *= radius;
   y *= shadowLen;
   z *= radius;
   gBlobShadowBox.max.set(mFabs(x.x)+mFabs(y.x)+mFabs(z.x),
      mFabs(x.y)+mFabs(y.y)+mFabs(z.y),
      mFabs(x.z)+mFabs(y.z)+mFabs(z.z));
   y *= 0.5f;
   gBlobShadowSphere.radius = gBlobShadowBox.max.len();
   gBlobShadowSphere.center = p + y;
   gBlobShadowBox.min  = y + p - gBlobShadowBox.max;
   gBlobShadowBox.max += y + p;

   // get polys

   gClientContainer.findObjects(gBlobShadowBox,smShadowMask,BlobShadow::collisionCallback,this);

   // setup partition list
   gBlobShadowPoly[0].set(-radius,0,-radius);
   gBlobShadowPoly[1].set(-radius,0, radius);
   gBlobShadowPoly[2].set( radius,0, radius);
   gBlobShadowPoly[3].set( radius,0,-radius);

   mPartition.clear();
   mPartitionVerts.clear();
   smDepthSortList.depthPartition(gBlobShadowPoly,4,mPartition,mPartitionVerts);

   updatePartition();
}

void BlobShadow::updatePartition()
{
	if(mPartitionVerts.empty())
		return;

	if(mShadowBuffer.isNull() || mShadowPBuffer.isNull())
	{
		F32 radius = mRadius;
		radius *= smGenericRadiusSkew;

		// now set up tverts & colors
		mShadowBuffer.set(GFX, mPartitionVerts.size(), GFXBufferTypeDynamic);
		mShadowBuffer.lock();

		F32 visibleAlpha = 255;
		if (mShapeBase && mShapeBase->getFadeVal())
			visibleAlpha = mClampF(255.0f * mShapeBase->getFadeVal(), 0, 255);
		F32 invRadius = 1.0f / radius;
		for (S32 i=0; i<mPartitionVerts.size(); i++)
		{
			Point3F vert = mPartitionVerts[i];
			mShadowBuffer[i].point.set(vert);
			mShadowBuffer[i].color.set(255, 255, 255, visibleAlpha);
			mShadowBuffer[i].texCoord.set(0.5f + 0.5f * mPartitionVerts[i].x * invRadius, 0.5f + 0.5f * mPartitionVerts[i].z * invRadius);
		};

		mShadowBuffer.unlock();

		size_t IndexNum = 0;
		mPBTriangleNum = 0;
		for(U32 p=0; p<mPartition.size(); p++)
		{
			DepthSortList::Poly &poly = mPartition[p];
			IndexNum += (3 * (poly.vertexCount - 2));
			mPBTriangleNum += (poly.vertexCount - 2); 
		}

		U16 *ibIndices;
		mShadowPBuffer.set( GFX, IndexNum, 0, GFXBufferTypeStatic  );
		mShadowPBuffer.lock( &ibIndices );
		size_t indexnum = 0;

		for(U32 p=0; p<mPartition.size(); p++)
		{
			DepthSortList::Poly &poly = mPartition[p];

			for(U32 v=1; v<poly.vertexCount - 1; v++)
			{
				ibIndices[indexnum] = poly.vertexStart;
				ibIndices[indexnum + 1] = poly.vertexStart + v;
				ibIndices[indexnum + 2] = poly.vertexStart + v + 1;
				indexnum += 3;
			} 
		}

		mShadowPBuffer.unlock();
	}
}

//--------------------------------------------------------------

void BlobShadow::collisionCallback(SceneObject * obj, void* thisPtr)
{
   //if (obj->getWorldBox().isOverlapped(gBlobShadowBox))
   {
      // only interiors clip...
      // <Edit> [3/30/2009 joy] 都进行裁剪
      //ClippedPolyList::allowClipping = (obj->getTypeMask() & gClientSceneGraph->getLightManager()->getSceneLightingInterface()->mClippingMask) != 0;
      obj->buildPolyList(&smDepthSortList,gBlobShadowBox,gBlobShadowSphere);
      //ClippedPolyList::allowClipping = true;
   }
}

//--------------------------------------------------------------
void BlobShadow::clearRenderStatus(S32)
{
	  GFX->setBaseRenderState();
}
#ifdef STATEBLOCK
void BlobShadow::setRenderStatus(S32)
{
	GFX->disableShaders();
	
	AssertFatal(mSetSB, "BlobShadow::setRenderStatus -- mSetSB cannot be NULL.");
	mSetSB->apply();
	GFX->setTexture(0, smGenericShadowTexture);   

	GFX->setupGenericShaders( GFXDevice::GSModColorTexture );
}
#else
void BlobShadow::setRenderStatus(S32)
{
	GFX->disableShaders();
	GFX->setCullMode(GFXCullNone);
	GFX->setLightingEnable(false);

	GFX->setZEnable(true);
	GFX->setZWriteEnable(false);

	GFX->setAlphaBlendEnable(true);
	GFX->setSrcBlend(GFXBlendSrcAlpha);
	GFX->setDestBlend(GFXBlendInvSrcAlpha);

	GFX->setAlphaTestEnable(true);
	GFX->setAlphaFunc(GFXCmpGreater);
	GFX->setAlphaRef(0);

	GFX->setTexture(0, smGenericShadowTexture);
	GFX->setTextureStageColorOp(0, GFXTOPModulate);      

	GFX->setupGenericShaders( GFXDevice::GSModColorTexture );
}
#endif


void BlobShadow::preRender(F32 camDist)
{
	RenderInst *ri = gRenderInstManager.allocInst();         
	ri->obj = (RenderableSceneObject *)this;
	ri->SortedIndex = 3;
	ri->type = RenderInstManager::RIT_Shadow;
	gRenderInstManager.addInst(ri);
}

void BlobShadow::render(S32)
{
   mLastRenderTime = Platform::getRealMilliseconds();
   GFX->pushWorldMatrix();
   MatrixF world = GFX->getWorldMatrix();
   world.mul(mLightToWorld);
   GFX->setWorldMatrix(world);

#ifdef STATEBLOCK
	AssertFatal(mSetZBiasSB, "BlobShadow::render -- mSetZBiasSB cannot be NULL.");
	mSetZBiasSB->apply();
#else
	F32 depthbias = -0.0002f;
	GFX->setZBias(*((U32 *)&depthbias));
#endif


   bool zwrite = GFX->getRenderState(GFXRSZWriteEnable);

   GFX->setVertexBuffer(mShadowBuffer);
   //for(U32 p=0; p<mPartition.size(); p++)
   //   GFX->drawPrimitive(GFXTriangleFan, mPartition[p].vertexStart, (mPartition[p].vertexCount - 2));

   GFX->setPrimitiveBuffer(mShadowPBuffer);
   GFX->drawIndexedPrimitive(GFXTriangleList, 0, mPartitionVerts.size(), 0, mPBTriangleNum);
#ifdef STATEBLOCK
	AssertFatal(mClearZBiasSB, "BlobShadow::render -- mClearZBiasSB cannot be NULL.");
	mClearZBiasSB->apply();
#else
   GFX->setZBias(0);
#endif

   GFX->popWorldMatrix();
}

void BlobShadow::deleteGenericShadowBitmap()
{
   smGenericShadowTexture = NULL;
}

void BlobShadow::freeResource()
{
	mShadowBuffer = NULL;
	mShadowPBuffer = NULL; 
}


void BlobShadow::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreater);
	GFX->setRenderState(GFXRSAlphaRef, 0);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->endStateBlock(mSetSB);

	//mSetZBiasSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSDepthBias, -0.0002f);
	GFX->endStateBlock(mSetZBiasSB);

	//mClearZBiasSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSDepthBias, 0);
	GFX->endStateBlock(mClearZBiasSB);
}


void BlobShadow::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mSetZBiasSB)
	{
		mSetZBiasSB->release();
	}

	if (mClearZBiasSB)
	{
		mClearZBiasSB->release();
	}
}

void BlobShadow::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		mSetZBiasSB = new GFXD3D9StateBlock;
		mClearZBiasSB = new GFXD3D9StateBlock;

		resetStateBlock();
	}

}

void BlobShadow::shutdown()
{
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mSetZBiasSB);
	SAFE_DELETE(mClearZBiasSB);
}