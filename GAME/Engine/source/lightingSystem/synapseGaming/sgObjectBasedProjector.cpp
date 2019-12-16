//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
#include "gfx/gfxDevice.h"
#include "gfx/primBuilder.h"
#include "math/mathUtils.h"
#include "T3D/shapeBase.h"
#include "T3D/tsStatic.h"
#include "sceneGraph/sceneGraph.h"
#include "gfx/gfxCardProfile.h"

// TODO: This is a bad dependancy used in rendering
// WheeledVehicle tires as part of the chassis shadow.
#include "T3D/vehicles/wheeledVehicle.h"

#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/synapseGaming/sgLightInfo.h"
#include "lightingSystem/synapseGaming/sgLighting.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"
#include "lightingSystem/synapseGaming/sgObjectBasedProjector.h"
#include "lightingSystem/synapseGaming/sgShadowTextureCache.h"
#include "lightingSystem/synapseGaming/sgFormatManager.h"
#include "sceneGraph/lightingInterfaces.h"

// not great but necessary due to reduced float precision
// caused by one of the linked libs...
#include <float.h>
#include "terrain/sun.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
#include "gfx/D3D/DXATIUtil.h"

//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* sgShadowProjector::mSetOneSB = NULL;
GFXStateBlock* sgShadowProjector::mSetTwoSB = NULL;
GFXStateBlock* sgShadowProjector::mClearStatusSB = NULL;
GFXStateBlock* sgShadowProjector::mSetAllowFastSB = NULL;
GFXStateBlock* sgShadowProjector::mSetSB = NULL;
GFXStateBlock* sgShadowProjector::mClearSB = NULL;
GFXStateBlock* sgShadowProjector::mSetDebugSB = NULL;

extern SceneGraph* gClientSceneGraph;


Point2I sgShadowSharedZBuffer::sgSize = Point2I(0, 0);
GFXTexHandle sgShadowSharedZBuffer::sgZBuffer = NULL;


void sgShadowSharedZBuffer::sgPrepZBuffer(const Point2I &size)
{
	if((size.x > sgSize.x) || (size.y > sgSize.y) || (sgZBuffer.isNull()))
	{
      sgFormatManager::prepFormats();

		// recreate...
		sgZBuffer = NULL;
		sgSize = size;
		sgZBuffer = GFXTexHandle(size.x, size.y,
			sgFormatManager::sgShadowZTextureFormat, &ShadowZTargetNoMSAATextureProfile);
	}
}

GFXTexHandle &sgShadowSharedZBuffer::sgGetZBuffer()
{
	return sgZBuffer;
}

void sgShadowSharedZBuffer::sgClear()
{
	sgZBuffer = NULL;
}

//-----------------------------------------------

bool sgShadowProjector::sgShadow::sgSetLOD(S32 lod, Point2I size)
{
	if(lod == sgCurrentLOD)
		return false;
	sgRelease();

	sgShadowTextureCache::sgAcquire(sgShadowTexture, size, sgShadowTextureFormat);
	sgCurrentLOD = lod;
	return true;
}

void sgShadowProjector::sgShadow::sgRelease()
{
	sgCurrentLOD = -1;
	if((GFXTextureObject *)sgShadowTexture)
		sgShadowTextureCache::sgRelease(sgShadowTexture);
}

//-----------------------------------------------

GFXTexHandle *gTexture = NULL;

sgShadowProjector::sgShadowProjector(SceneObject *parentobject,LightInfo *light, TSShapeInstance *shapeinstance)
{
    sgParentObject = dynamic_cast<RenderableSceneObject*>(parentobject);
	sgLight = dynamic_cast<sgLightInfo*>(light);

	for(U32 i=0; i<sgspMaxLOD; i++)
		sgShadowLODSize[i] = Point2I(0, 0);

	sgShadowBuilderShader = NULL;
	sgShadowShader = NULL;
	//sgShadowShaderFastPartition = NULL;

	Sim::findObject("ShadowShaderFastPartition", sgShadowShaderFastPartition);

	//Sim::findObject("ShadowBuilderShader_3_0", sgShadowBuilderShader_3_0);
	Sim::findObject("ShadowBuilderShader_2_0", sgShadowBuilderShader_2_0);
	Sim::findObject("ShadowBuilderShader_1_1", sgShadowBuilderShader_1_1);
	Sim::findObject("ShadowBuilderShader_vt", sgShadowBuilderShader_vt);
	

	Sim::findObject("ShadowShaderHigh_2_0", sgShadowShaderHigh_2_0);
	Sim::findObject("ShadowShader_2_0", sgShadowShader_2_0);
	Sim::findObject("ShadowShader_1_1", sgShadowShader_1_1);

	sgShapeInstance = shapeinstance;

	sgEnable = false;
	sgCanMove = false;
	sgCanRTT = false;
	sgCanSelfShadow = false;

	sgRequestedShadowSize = 128;
	sgFrameSkip = 5;
	sgMaxVisibleDistance = 50.0f;
	sgProjectionDistance = 140.0f;
	sgSphereAdjust = 1.0;

	sgFirstMove = true;
	sgFirstRTT = true;
	sgShadowTypeDirty = true;
	sgLastFrame = 0;
	sgCachedTextureDetailSize = 0;
	sgCachedParentTransformHash = 0;
	sgPreviousShadowTime = 0;
	sgPreviousShadowLightingVector = VectorF(0, 0, -1);

	sgLastRenderTime = 0;

   // Grab a callback from the texture manager to deal with zombification.
   GFX->registerTexCallback(&sgShadowProjector::texCB, this, mTexCallbackHandle);

   // testing...
	//gTexture = &sgLastShadow;
}

sgShadowProjector::~sgShadowProjector() 
{
   GFX->unregisterTexCallback(mTexCallbackHandle);
   sgClear();
}

void sgShadowProjector::sgClear(bool texOnly)
{
	// free resources...
	sgShadowLODObject.sgRelease();

	for(U32 i=0; i<sgspMaxLOD; i++)
		sgShadowLODSize[i] = Point2I(0, 0);

	// setup for recalc on enable...
	sgShadowTypeDirty = true;
	sgFirstMove = true;
	sgFirstRTT = true;

	if(texOnly)
		return;

	sgShadowPolys.clear();
	sgShadowPoints.clear();
	sgShadowBuffer = NULL;
	mShadowPBuffer = NULL;
}

void sgShadowProjector::texCB( GFXTexCallbackCode code, void *userData )
{
   if (code == GFXZombify)
   {
      sgShadowProjector* sp = static_cast<sgShadowProjector*>(userData);
      sp->sgClear(true);
	  sp->sgShadowBuffer = NULL;
	  sp->mShadowPBuffer = NULL;
   }
}

void sgShadowProjector::sgGetVariables()
{
	sgParentObject->setShadowVariables(this);
}

void sgShadowProjector::sgSetupShadowType(sgShadowOp flag)
{
	if(!sgShadowTypeDirty && (sgCalculateShaderModel() == sgCachedShaderModel) &&
	   (sgLightManager::sgDynamicShadowDetailSize == sgCachedTextureDetailSize))
		return;

	sgClear(flag==shadow_set);

   sgFormatManager::prepFormats();

	sgCachedShaderModel = sgCalculateShaderModel();
	sgCachedTextureDetailSize = sgLightManager::sgDynamicShadowDetailSize;

	U32 size = sgGetShadowSize();
	if(sgCachedShaderModel != sgsm_1_1)
	{
		sgShadowLODObject.sgShadowTextureFormat = sgFormatManager::sgShadowTextureFormat_2_0;
		for(U32 i=0; i<sgspMaxLOD; i++)
		{
			sgShadowLODSize[i] = Point2I(size, size);
			size = getMax((size >> 1), U32(4));
		}
		if(sgCachedShaderModel != sgsm_2_0)
		{
			sgShadowBuilderShader = sgShadowBuilderShader_2_0;
			sgShadowShader = sgShadowShaderHigh_2_0;
		}
		else
		{
			sgShadowBuilderShader = sgShadowBuilderShader_2_0;
			sgShadowShader = sgShadowShader_2_0;
		}
	}
	else
	{
		sgShadowLODObject.sgShadowTextureFormat = sgFormatManager::sgShadowTextureFormat_1_1;
		for(U32 i=0; i<sgspMaxLOD; i++)
		{
			sgShadowLODSize[i] = Point2I(size, size);
			size = getMax((size >> 1), U32(4));
		}

		if(GFX->getCardProfiler()->queryProfile("allowRGBA32FBitVertexTextures", false))
			sgShadowBuilderShader = sgShadowBuilderShader_vt;
		else
			sgShadowBuilderShader = sgShadowBuilderShader_1_1;

		sgShadowShader = sgShadowShader_1_1;
	}

	sgShadowSharedZBuffer::sgPrepZBuffer(Point2I(sgGetShadowSize(), sgGetShadowSize()));

	sgShadowTypeDirty = false;
}

sgShadowProjector::sgShaderModel sgShadowProjector::sgCalculateShaderModel()
{
#ifdef POWER_OS_XENON
   return sgsmHigh_2_0;
#endif

	U32 quality = sgLightManager::sgGetDynamicShadowQuality();

	if((!sgCanSelfShadow) || (quality >= 2))
		return sgsm_1_1;
	if(quality >= 1)
		return sgsm_2_0;
	return sgsmHigh_2_0;
}

MatrixF sgShadowProjector::sgGetAdjustedParentWorldTransform()
{
   MatrixF mat = sgParentObject->getRenderTransform();
   Box3F box = sgParentObject->getRenderWorldBox();
   Point3F center;
   box.getCenter(&center);
   mat.setPosition(center);

   return mat;
}

void sgShadowProjector::sgGetLightSpaceBasedOnY()
{
	Point3F s, t;
	if(mFabs(sgLightVector.x) > mFabs(sgLightVector.z))
	{
		mCross(sgLightVector, Point3F(0.0f, 0.0f, 1.0f), &t);
		t.normalizeSafe();
		mCross(sgLightVector, t, &s);
		s.normalizeSafe();
	}
	else
	{
		mCross(sgLightVector, Point3F(1.0f, 0.0f, 0.0f), &t);
		t.normalizeSafe();
		mCross(sgLightVector, t, &s);
		s.normalizeSafe();
	}

	sgLightSpaceY.identity();
	sgLightSpaceY.setRow(0, s);
	sgLightSpaceY.setRow(1, sgLightVector);
	sgLightSpaceY.setRow(2, t);
	sgLightSpaceY.transpose();

	MatrixF world;
	world.identity();
   world.setPosition(sgGetAdjustedParentWorldTransform().getPosition());
	sgLightToWorldY.mul(world, sgLightSpaceY);
	sgWorldToLightY = sgLightToWorldY;
	sgWorldToLightY.inverse();

	MatrixF lighttoworldproj;
	world.setPosition(sgCachedParentPos);
	lighttoworldproj.mul(world, sgLightSpaceY);
	sgLightProjToLightY = sgWorldToLightY;
	sgLightProjToLightY.mul(lighttoworldproj);
	sgWorldToLightProjY = lighttoworldproj;
	sgWorldToLightProjY.inverse();
}

void sgShadowProjector::sgCalculateBoundingBox(sgShadowOp flag)
{
#if defined POWER_OS_WIN32
   // not great but necessary due to reduced float precision
   // caused by one of the linked libs...
   U32 oldval = U32_MAX;
#if _MSC_VER < 1400
   oldval = _controlfp(0, 0);
   _controlfp(_PC_53, _MCW_PC);
#else
   _controlfp_s(&oldval, 0, 0);
   _controlfp_s(NULL, _PC_53, _MCW_PC);
#endif
#endif

   PROFILE_START(sgShadowProjector_sgCalculateBoundingBox);

   Point3F vect = sgLight->mDirection;
   Point3F pos = sgGetAdjustedParentWorldTransform().getPosition();

   if(sgLightManager::sgMultipleDynamicShadows)
   {
      if(sgLight->mType != LightInfo::Vector)
      {
         vect = (pos - sgLight->mPos);
         vect.normalizeSafe();
      }
   }
   else
   {
      vect = sgGetCompositeShadowLightDirection();
   }

   SphereF sphere = sgParentObject->getShadowSphere();
   // help for objects with boxes that are too small...
   sphere.radius *= sgSphereAdjust;
   pos -= vect * sphere.radius * 0.25;

   //Ray: NPC的z坐标老是会上下抖动，需要排除
   bool move = (flag==shadow_save)|| ((vect != sgLightVector) || (pos.x != sgCachedParentPos.x) || (pos.y != sgCachedParentPos.y)) && sgCanMove;
   if(!move && !sgFirstMove)
   {
      PROFILE_END(sgShadowProjector_sgCalculateBoundingBox);

#if defined POWER_OS_WIN32
      // not great but necessary due to reduced float precision
      // caused by one of the linked libs...
#if _MSC_VER < 1400
      _controlfp(oldval, U32_MAX);
#else
      _controlfp_s(NULL, oldval, U32_MAX);
#endif
#endif

      return;
   }

   sgFirstMove = false;
   sgFirstRTT = true;
   sgCachedParentPos = pos;
   sgLightVector = vect;

   testRenderPoints[0] = Point3F( sphere.radius,  sphere.radius + sgProjectionDistance, sphere.radius);
   testRenderPoints[1] = Point3F(-sphere.radius,  sphere.radius + sgProjectionDistance, sphere.radius);
   testRenderPoints[2] = Point3F( sphere.radius, -sphere.radius, sphere.radius);
   testRenderPoints[3] = Point3F(-sphere.radius, -sphere.radius, sphere.radius);
   testRenderPoints[4] = Point3F( sphere.radius,  sphere.radius + sgProjectionDistance, -sphere.radius);
   testRenderPoints[5] = Point3F(-sphere.radius,  sphere.radius + sgProjectionDistance, -sphere.radius);
   testRenderPoints[6] = Point3F( sphere.radius, -sphere.radius, -sphere.radius);
   testRenderPoints[7] = Point3F(-sphere.radius, -sphere.radius, -sphere.radius);

   sgProjectionScale = 1.0 / sphere.radius;
   //sgProjectionInfo.x = (sphere.radius * 2.0) + shadowdist;
   sgProjectionInfo.x = sphere.radius + sgProjectionDistance;
   sgProjectionInfo.y = sphere.radius;
   sgProjectionInfo.z = sgProjectionScale;
   sgProjectionInfo.w = 0.0f;

   dMemcpy(((float *)testRenderPointsWorld[0]), ((float *)testRenderPoints[0]), sizeof(testRenderPoints));

   sgGetLightSpaceBasedOnY();

   for(U32 i=0; i<8; i++)
      sgLightToWorldY.mulP(testRenderPoints[i], &testRenderPointsWorld[i]);

   Point3F extent((2.0f * sphere.radius), sgProjectionDistance, (2.0f * sphere.radius));
   sgPolyGrinder.clear();
   sgPolyGrinder.set(sgWorldToLightProjY, extent);
   sgPolyGrinder.setInterestNormal(sgLightVector);

   // build world space box and sphere around shadow
   sgShadowBox.max.set(testRenderPointsWorld[0]);
   sgShadowBox.min.set(testRenderPointsWorld[0]);
   for(U32 i=1; i<8; i++)
   {
      sgShadowBox.max.setMax(testRenderPointsWorld[i]);
      sgShadowBox.min.setMin(testRenderPointsWorld[i]);
   }

   sgShadowBox.getCenter(&sgShadowSphere.center);
   sgShadowSphere.radius = Point3F(sgShadowBox.max - sgShadowSphere.center).len();


   if(flag!=shadow_set)
   {
	   PROFILE_START(sgShadowProjector_sgCalculateBoundingBox_find);

	   gClientContainer.findObjects(sgShadowBox,(AtlasObjectType | TerrainObjectType | InteriorObjectType | StaticTSObjectType), sgShadowProjector::collisionCallback, this);

	   sgShadowPoly[0].set(-sphere.radius, 0, -sphere.radius);
	   sgShadowPoly[1].set(-sphere.radius, 0,  sphere.radius);
	   sgShadowPoly[2].set( sphere.radius, 0,  sphere.radius);
	   sgShadowPoly[3].set( sphere.radius, 0, -sphere.radius);

	   sgShadowPolys.clear();
	   sgShadowPoints.clear();
	   sgPolyGrinder.depthPartition(sgShadowPoly, 4, sgShadowPolys, sgShadowPoints);

	   // done with this...
	   sgPolyGrinder.clear();

	   //Ray: sgShadowPolys里的垃圾剔除
	   for(S32 p=sgShadowPolys.size()-1; p>=0; p--)
	   {
		   DepthSortList::Poly &poly = sgShadowPolys[p];
		   if(poly.vertexCount <3)
		   {
			   sgShadowPolys.erase(p);
			   continue;
		   }

		   U32 a, b, c;
		   a = poly.vertexStart;
		   b = a + 1;
		   c = a + 2;

		   // apparently this is necessary under some conditions...
		   if((sgShadowPoints[a] == sgShadowPoints[b]) ||
			   (sgShadowPoints[a] == sgShadowPoints[c]) ||
			   (sgShadowPoints[b] == sgShadowPoints[c]))
		   {
			   sgShadowPolys.erase(p);
			   continue;
		   }

		   poly.plane.set(sgShadowPoints[a], sgShadowPoints[b], sgShadowPoints[c]);
	   }

	   //Ray:这里应该用SSE
	   for(U32 p=0;p<sgShadowPoints.size();p++)
	   {
		   if(flag == shadow_save)
		   {
			   if(p==0)			   
			   {
				   sgBoundingBox.min.set(sgShadowPoints[p]);
				   sgBoundingBox.max.set(sgShadowPoints[p]);
			   }
			   else
			   {
				   sgBoundingBox.min.setMin(sgShadowPoints[p]);
				   sgBoundingBox.max.setMax(sgShadowPoints[p]);
			   }
		   }

		   sgLightProjToLightY.mulP(sgShadowPoints[p]);
	   }

	   PROFILE_END(sgShadowProjector_sgCalculateBoundingBox_find);
   }

   if(sgShadowPoints.size() < 1)
   {
      PROFILE_END(sgShadowProjector_sgCalculateBoundingBox);

#if defined POWER_OS_WIN32
      // not great but necessary due to reduced float precision
      // caused by one of the linked libs...
#if _MSC_VER < 1400
      _controlfp(oldval, U32_MAX);
#else
      _controlfp_s(NULL, oldval, U32_MAX);
#endif
#endif

      return;
   }

   if(flag!=shadow_set || sgShadowBuffer.isNull())
   {
       size_t IndexNum = 0;
	   mPBTriangleNum = 0;

	   sgShadowBuffer.set(GFX, sgShadowPoints.size(), flag!=shadow_set ? GFXBufferTypeDynamic : GFXBufferTypeStatic );
	   sgShadowBuffer.lock();

	   for(U32 p=0; p<sgShadowPolys.size(); p++)
	   {
		   DepthSortList::Poly &poly = sgShadowPolys[p];

		   for(U32 v=0; v<poly.vertexCount; v++)
		   {
			   GFXVertexPN &vb = sgShadowBuffer[poly.vertexStart + v];
			   vb.point = sgShadowPoints[poly.vertexStart + v];
			   vb.normal = poly.plane;
		   } 
           IndexNum += (3 * (poly.vertexCount - 2));
		   mPBTriangleNum += (poly.vertexCount - 2); 
	   }

	   sgShadowBuffer.unlock();

	   U16 *ibIndices;
	   mShadowPBuffer.set( GFX, IndexNum, 0, GFXBufferTypeStatic  );
	   mShadowPBuffer.lock( &ibIndices );
	   size_t indexnum = 0;

	   for(U32 p=0; p<sgShadowPolys.size(); p++)
	   {
		   DepthSortList::Poly &poly = sgShadowPolys[p];

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

   PROFILE_END(sgShadowProjector_sgCalculateBoundingBox);


#if defined POWER_OS_WIN32
   // not great but necessary due to reduced float precision
   // caused by one of the linked libs...
#if _MSC_VER < 1400
   _controlfp(oldval, U32_MAX);
#else
   _controlfp_s(NULL, oldval, U32_MAX);
#endif
#endif
}

void sgShadowProjector::collisionCallback(SceneObject *obj, void *shadow)
{
	sgShadowProjector *shadowobj = reinterpret_cast<sgShadowProjector *>(shadow);
	//if(obj->getWorldBox().isOverlapped(shadowobj->sgShadowBox))
	if(obj!=shadowobj->sgParentObject)
	{
		if(obj->getTypeMask() & StaticTSObjectType)
		{
			TSStatic *tsstatic = dynamic_cast<TSStatic *>(obj);
			if(!tsstatic)
				return;
			if(!tsstatic->receiveShadow())
				return;

			ClippedPolyList::allowClipping = true;
		}
		else
		{
			ClippedPolyList::allowClipping = true;//(obj->getTypeMask() & gClientSceneGraph->getLightManager()->getSceneLightingInterface()->mClippingMask) != 0;  
		}

		obj->buildPolyList(&shadowobj->sgPolyGrinder, shadowobj->sgShadowBox, shadowobj->sgShadowSphere);
		ClippedPolyList::allowClipping = true;
	}
}

void sgShadowProjector::sgRenderShape(TSShapeInstance *shapeinst, const MatrixF &trans1,
		S32 vertconstindex1, const MatrixF &trans2, S32 vertconstindex2)
{
	U32 detaillevel = shapeinst->getCurrentDetail();
	if(detaillevel == -1)
		return;

	TSShape *shape = shapeinst->getShape();
	AssertFatal((detaillevel >= 0) && (detaillevel < shape->details.size()), "TSShapeInstance::renderShadow");
	const TSDetail *detail = &shape->details[detaillevel];
	AssertFatal((detail->subShapeNum >= 0), "TSShapeInstance::renderShadow: not with a billboard detail level");

	shapeinst->setStatics(detaillevel);

	S32 s = shape->subShapeFirstObject[detail->subShapeNum];
   S32 e = s+shape->subShapeNumObjects[detail->subShapeNum];   

   //may want to check translucent objects below and skip rendering
   //some DTS (spaceguy GDC 2008) seem to have subshapes we want rendered 
   //beyond first translucent and thus only partial shadow is renderered
	
   //S32 e = shape->subShapeFirstTranslucentObject[detail->subShapeNum];

	for(U32 i=s; i<e; i++)
	{
		TSMesh *mesh = shapeinst->mMeshObjects[i].getMesh(detail->objectDetailNum);
		if(mesh && (shapeinst->mMeshObjects[i].visible > 0.01))
		{
			MatrixF *meshtrans = shapeinst->mMeshObjects[i].getTransform();
			MatrixF mat;

			// one of those hidden issues with PowerEngine...
			TSShapeInstance::smRenderData.currentObjectInstance = &shapeinst->mMeshObjects[i];
			if(!mesh->getVertexBuffer())
				continue;

			if(meshtrans)
				mat.mul(trans1, (*meshtrans));
			else
				mat = trans1;
			mat.transpose();
			GFX->setVertexShaderConstF(vertconstindex1, mat, 4);

			if(vertconstindex2 > -1)
			{
				if(meshtrans)
					mat.mul(trans2, (*meshtrans));
				else
					mat = trans2;
				mat.transpose();
				GFX->setVertexShaderConstF(vertconstindex2, mat, 4);
			}

			mesh->render(shapeinst->getMaterialList());
		}
	}

	shapeinst->clearStatics();
}

void sgShadowProjector::sgRenderShadowBuffer()
{
	RectI originalview = GFX->getViewport();

    GFX->setTexture(0, NULL);
    GFX->setTexture(1, NULL);
    GFX->setTexture(2, NULL);
    GFX->setTexture(3, NULL);

	GFX->pushActiveRenderTarget();
   if (mShadowBufferTarget.isNull())
   {
      mShadowBufferTarget = GFX->allocRenderToTextureTarget();
   }   
   mShadowBufferTarget->attachTexture(GFXTextureTarget::Color0, sgShadowLODObject.sgShadowTexture );   
   mShadowBufferTarget->attachTexture(GFXTextureTarget::DepthStencil, sgShadowSharedZBuffer::sgGetZBuffer() );
   GFX->setActiveRenderTarget( mShadowBufferTarget );

	if(sgAllowSelfShadowing())
		GFX->clear(GFXClearTarget | GFXClearZBuffer, ColorI(255, 0, 0, 255), 1.0f, 0);
	else
		GFX->clear(GFXClearTarget | GFXClearZBuffer, ColorI(0, 0, 0, 0), 1.0f, 0);

	GFX->pushWorldMatrix();
	//GFX->setCullMode(GFXCullCCW);
	//GFX->setLightingEnable(false);
	//GFX->setAlphaBlendEnable(true);
	//GFX->setZEnable(true);
	//GFX->setZWriteEnable(true);
	//GFX->setSrcBlend(GFXBlendSrcAlpha);
	//GFX->setDestBlend(GFXBlendDestAlpha);

	//GFX->setTextureStageColorOp(0, GFXTOPDisable);

	//sgShadowBuilderShader->getShader()->process();

	MatrixF shapeworld;
	shapeworld = sgParentObject->getRenderTransform();
	shapeworld.scale(sgParentObject->getScale());

	MatrixF lightspace;
	lightspace.mul(sgWorldToLightY, shapeworld);

   MatrixF shadowscale;
   shadowscale.identity();
   shadowscale.scale(Point3F(sgProjectionScale, sgProjectionScale, sgProjectionScale));
   lightspace = shadowscale * lightspace;

	GFX->setVertexShaderConstF(4, sgProjectionInfo, 1);
	GFX->setPixelShaderConstF(1, Point4F(1.0, 0.0, 0.0, 0.0), 1);

	MatrixF proj;
	proj.identity();
	proj.scale(Point3F(1.0, (1.0 / sgProjectionScale), 1.0));

	sgParentObject->renderShadowBuff(this,proj,lightspace,sgWorldToLightY,shadowscale);

	GFX->popWorldMatrix();
	GFX->popActiveRenderTarget();

	GFX->setViewport(originalview);

}

bool sgShadowProjector::shouldRender(F32 camDist)
{
   if(sgFirstMove)
	   sgGetVariables();

   // Parent object visibility   
   if (sgParentObject->getFadeVal() < TSMesh::VISIBILITY_EPSILON)
     return false;      


   if(camDist > sgMaxVisibleDistance)
      return false;

   if(!sgEnable || !sgLightManager::sgAllowDynamicShadows())
   {
      sgClear();
      return false;
   }

   //Ray: 静态物件的动态阴影被预先计算了
   sgShadowOp flag = shadow_gen;
   if( (sgParentObject->getType() & StaticTSObjectType) && sgShadowPolys.size()) 
   {
	   flag = shadow_set;
   }

#ifdef NTJ_EDITOR
   if(gEditingMission)
   {
		flag = shadow_gen;
   }
#endif

   if(flag == shadow_gen || sgFirstMove)
   {
	   sgSetupShadowType(flag);
	   sgCalculateBoundingBox(flag);
   }

   if(!sgShadowBuilderShader || !sgShadowBuilderShader->getShader() ||
      !sgShadowShader || !sgShadowShader->getShader())
      return false;

   F32 attn = 2.0;
   if(sgLight->mType != LightInfo::Vector)
   {
      sgLightingModel &model = sgLightingModelManager::sgGetLightingModel(sgLight->sgLightingModelName);
      model.sgSetState(sgLight);
      F32 maxrad = model.sgGetMaxRadius(true);
      model.sgResetState();

      if(maxrad <= 0.0f)
         return false;

      Point3F distvect = sgParentObject->getRenderPosition() - sgLight->mPos;
      attn = distvect.len();
      attn = 1.0 - (attn / maxrad);
      attn *= 2.0;
      if(attn <= SG_MIN_LEXEL_INTENSITY)
         return false;
   }
   return true;
}

void sgShadowProjector::saveDynamicShadowData(Stream *stream)
{
	sgGetVariables();

	sgSetupShadowType(shadow_save);
	sgCalculateBoundingBox(shadow_save);

	S32 size;

	size = sgShadowPoints.size();
	stream->write(4, &size);
	for (S32 i = 0; i < size; i++)
	{
		stream->write(sizeof(Point3F), &sgShadowPoints[i]);
	}

	size = sgShadowPolys.size();
	stream->write(4, &size);
	for (S32 i = 0; i < size; i++)
	{
		stream->write(sizeof(PlaneF), &sgShadowPolys[i].plane);
		//stream->write(4, &sgShadowPolys[i].material);
		//stream->write(4, &sgShadowPolys[i].polyFlags);
		//stream->write(4, &sgShadowPolys[i].surfaceKey);
		stream->write(4, &sgShadowPolys[i].vertexCount);
		stream->write(4, &sgShadowPolys[i].vertexStart);
	}

	stream->write(sizeof(Point3F), &sgBoundingBox.min);
	stream->write(sizeof(Point3F), &sgBoundingBox.max);
}

void sgShadowProjector::setDynamicShadowData(void *p1,void *p2)
{
	stShadowParam *pShadowParam = (stShadowParam *)p1;

	sgShadowPolys = pShadowParam->sgShadowPolys;
	sgShadowPoints = pShadowParam->sgShadowPoints;
}

void sgShadowProjector::freeResource()
{
	sgClear(true);
	sgShadowBuffer = NULL;
	mShadowPBuffer = NULL; 
}

void sgShadowProjector::clearRenderStatus(S32 flag)
{
	if(flag==2)
	{
		GFX->setTexture(0, NULL);
		GFX->setTexture(1, NULL);
		GFX->setTexture(2, NULL);
		GFX->setTexture(3, NULL);
#ifdef STATEBLOCK
		AssertFatal(mClearStatusSB, "sgShadowProjector::clearRenderStatus -- mClearStatusSB cannot be NULL.");
		mClearStatusSB->apply();
#else
		GFX->setTextureStageColorOp(4, GFXTOPDisable);
		GFX->setTextureStageColorOp(3, GFXTOPDisable);
		GFX->setTextureStageColorOp(2, GFXTOPDisable);
		GFX->setTextureStageColorOp(1, GFXTOPDisable);
		GFX->setTextureStageMagFilter(0, GFXTextureFilterLinear);
		GFX->setTextureStageMinFilter(0, GFXTextureFilterLinear);
		GFX->setTextureStageColorOp(0, GFXTOPDisable);
		GFX->setZEnable(true);
		GFX->setZWriteEnable(true);
		GFX->setSrcBlend(GFXBlendSrcAlpha);
		GFX->setDestBlend(GFXBlendDestAlpha);
		GFX->setAlphaTestEnable(false);
		GFX->disableShaders();
		GFX->setZBias(0);
#endif

	}
}

void sgShadowProjector::setRenderStatus(S32 flag)
{
	//Ray: 这在这做一次统一的状态设置，而后批渲染阴影贴图或者阴影

	switch(flag)
	{
	case 1:
		{
#ifdef STATEBLOCK
			AssertFatal(mSetOneSB, "sgShadowProjector::setRenderStatus -- mSetOneSB cannot be NULL.");
			mSetOneSB->apply();
#else
			GFX->setCullMode(GFXCullCCW);
			GFX->setLightingEnable(false);
			GFX->setAlphaBlendEnable(true);
			GFX->setZEnable(true);
			GFX->setZWriteEnable(true);
			GFX->setSrcBlend(GFXBlendSrcAlpha);
			GFX->setDestBlend(GFXBlendDestAlpha);
			GFX->setTextureStageColorOp(0, GFXTOPDisable);
#endif
			sgShadowBuilderShader->getShader()->process();

		}
		break;
	case 2:
		{
			if(GFX->isDmapSet())
			{
				GFX->setPointSize(r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DIS));
				// Unbind the DMAP texture
				GFX->setTexture(VERTEX_TEXTURE_BUFF, NULL);
				GFX->updateStates();
				GFX->setPointSize(r2vbGlbEnable_Set(FALSE));
				TSShapeInstance::smNullVB->prepare();  //setup streamSource(1) = NULL
				GFX->updateStates();
				GFX->setDmap(false);
			}

#ifdef STATEBLOCK
			AssertFatal(mSetTwoSB, "sgShadowProjector::setRenderStatus -- mSetTwoSB cannot be NULL.");
			mSetTwoSB->apply();
#else
			GFX->setCullMode(GFXCullNone);
			GFX->setLightingEnable(false);
			GFX->setAlphaBlendEnable(true);
			GFX->setZEnable(true);
			GFX->setZWriteEnable(false);

#if 1
			//影子渲染方式:原来的
			GFX->setSrcBlend(GFXBlendDestColor);
			GFX->setDestBlend(GFXBlendZero);
#else
			//影子渲染采用Lerp混合方式
			GFX->setSrcBlend(GFXBlendSrcAlpha);
			GFX->setDestBlend(GFXBlendInvSrcAlpha);
#endif
			GFX->setAlphaTestEnable(true);
			GFX->setAlphaFunc(GFXCmpGreater);
			GFX->setAlphaRef(0);
#endif


			// <Edit> [3/23/2009 joy] 设置zbuff偏移
			//F32 depthbias = -0.00005f;
			//GFX->setZBias(*((U32 *)&depthbias));
#pragma message(ENGINE(不加偏移可能造成影子投影在地表上有些瑕疵，加了偏移物体影子瑕疵扩大))

			bool allowfastpartition = sgShadowShaderFastPartition &&
				sgFormatManager::sgShadowTextureFormat_2_0_AllowFastPartition &&
				(sgShadowShader != sgShadowShader_1_1);

			if(allowfastpartition)
				sgShadowShaderFastPartition->getShader()->process();
			else
				sgShadowShader->getShader()->process();
		}

		break;
	}
}

void sgShadowProjector::preRender(F32 camDist)
{
	sgAttn = 2.0;
	if(sgLight->mType != LightInfo::Vector)
	{
		sgLightingModel &model = sgLightingModelManager::sgGetLightingModel(sgLight->sgLightingModelName);
		model.sgSetState(sgLight);
		F32 maxrad = model.sgGetMaxRadius(true);
		model.sgResetState();

		Point3F distvect = sgGetAdjustedParentWorldTransform().getPosition() - sgLight->mPos;
		sgAttn = distvect.len();
		sgAttn = 1.0 - (sgAttn / maxrad);
		sgAttn *= 2.0;
	}

	// lod info...
	sgScaledcamdist = camDist * 0.25f;
	U32 lod = mClamp(U32(sgScaledcamdist), U32(0), U32(sgspMaxLOD-1));

	if(sgMaxVisibleDistance != 0.0f)
	{
		F32 distattn = camDist / sgMaxVisibleDistance;

		// intensity...
		F32 distattnadjusted = 1.0f - distattn;
		distattnadjusted *= 3.0f;
		distattnadjusted = mClampF(distattnadjusted, 0.0f, 1.0f);
		sgAttn *= distattnadjusted;

		// size...
		distattnadjusted = distattn * F32(sgspMaxLOD - 1);
		U32 distlod = mClamp(U32(distattnadjusted), U32(0), U32(sgspMaxLOD - 1));
		// get the best of both lod types...
		//   -dist lod sucks for short visible distances...
		//   -fixed lod falls off to quick for long distances...
		lod = getMin(lod, distlod);
	}

	sgAllowlodselfshadowing = lod <= sgspLastSelfShadowLOD;


	// ok, we're going to render...
	sgLastRenderTime = Platform::getRealMilliseconds();

	U32 hash = calculateCRC((float *)sgParentObject->getRenderTransform(), (sizeof(float) * 16));
	bool frame = ((sgLastFrame >= sgFrameSkip) || (sgAllowSelfShadowing() && sgAllowlodselfshadowing)) &&
		(sgShapeInstance->shadowDirty || (sgCachedParentTransformHash != hash) ||( sgCanRTT && (sgShadowLODObject.sgGetLOD() != lod)) );
	sgLastFrame++;
	if(frame || sgFirstRTT)
	{
		sgFirstRTT = false;
		sgLastFrame = 0;
		// this breaks multiple shadows - needs to be in the ObjectShadows class...
		//sgShapeInstance->shadowDirty = false;
		sgCachedParentTransformHash = hash;

		// can't rtt, means no lod switch so use highest texture...
		if(sgCanRTT)
			sgShadowLODObject.sgSetLOD(lod, sgShadowLODSize[lod]);
		else
			sgShadowLODObject.sgSetLOD(0, sgShadowLODSize[0]);

		RenderInst *ri = gRenderInstManager.allocInst();         
		ri->obj = (RenderableSceneObject *)this;
		ri->SortedIndex = 1;
		ri->type = RenderInstManager::RIT_Shadow;
		gRenderInstManager.addInst(ri);
	}

	RenderInst *ri = gRenderInstManager.allocInst();         
	ri->obj = (RenderableSceneObject *)this;
	ri->SortedIndex = 2;
	ri->type = RenderInstManager::RIT_Shadow;
	gRenderInstManager.addInst(ri);
}

void sgShadowProjector::render(S32 key)
{
	if(key==1)
	{
		sgRenderShadowBuffer();
		return;
	}

    if ( sgShadowPoints.size() == 0 )
        return;

	GFX->pushWorldMatrix();

	const MatrixF &worldmat = GFX->getWorldMatrix();
	const MatrixF &viewmat = GFX->getViewMatrix();
	const MatrixF &projmat = GFX->getProjectionMatrix();
	const MatrixF worldtoscreen = projmat * viewmat * worldmat;
	const MatrixF lighttoscreen = worldtoscreen * sgLightToWorldY;
	MatrixF objecttolight;
	MatrixF objecttoscreen;
	MatrixF transposetemp;

	bool allowfastpartition = sgShadowShaderFastPartition &&
		sgFormatManager::sgShadowTextureFormat_2_0_AllowFastPartition &&
		(sgShadowShader != sgShadowShader_1_1);

	if((sgShadowShader == sgShadowShader_1_1) || (allowfastpartition))
	{
#ifdef STATEBLOCK
		AssertFatal(mSetAllowFastSB, "sgShadowProjector::render -- mSetAllowFastSB cannot be NULL.");
		mSetAllowFastSB->apply();
		GFX->setTexture(0, sgShadowLODObject.sgShadowTexture);
		GFX->setTexture(1, sgShadowLODObject.sgShadowTexture);
		GFX->setTexture(2, sgShadowLODObject.sgShadowTexture);
		GFX->setTexture(3, sgShadowLODObject.sgShadowTexture);
#else
		GFX->setTextureStageColorOp(0, GFXTOPModulate);
		GFX->setTexture(0, sgShadowLODObject.sgShadowTexture);
		GFX->setTextureStageColorOp(1, GFXTOPModulate);
		GFX->setTexture(1, sgShadowLODObject.sgShadowTexture);
		GFX->setTextureStageColorOp(2, GFXTOPModulate);
		GFX->setTexture(2, sgShadowLODObject.sgShadowTexture);
		GFX->setTextureStageColorOp(3, GFXTOPModulate);
		GFX->setTexture(3, sgShadowLODObject.sgShadowTexture);
		GFX->setTextureStageColorOp(4, GFXTOPDisable);
#endif
	}
	else
	{
#ifdef STATEBLOCK
		AssertFatal(mSetSB, "sgShadowProjector::render -- mSetSB cannot be NULL.");
		mSetSB->apply();
#else
		GFX->setTextureStageColorOp(0, GFXTOPModulate);
		GFX->setTextureStageMagFilter(0, GFXTextureFilterPoint);
		GFX->setTextureStageMinFilter(0, GFXTextureFilterPoint);
		GFX->setTexture(0, sgShadowLODObject.sgShadowTexture);
		GFX->setTextureStageColorOp(1, GFXTOPDisable);
#endif

	}

	F32 size = (1.0 / (sgShadowLODObject.sgShadowTexture.getWidth() - 1)) * 1.25;
	Point4F stride(size, size, (F32)sgShadowLODObject.sgShadowTexture.getWidth(),
		(1.0f / (F32)sgShadowLODObject.sgShadowTexture.getWidth()));

	GFX->setPixelShaderConstF(0, stride, 1);
	GFX->setVertexShaderConstF(12, Point4F(0.0f, -1.0f, 0.0f, 0.0f), 1);

   // Parent object visibility
   F32 visibility = 1.0f;
   visibility = sgParentObject->getFadeVal();

	F32 adjustment = 0.8f;

	
#if 0
	sgAttn *= sgIntensity * 0.25f;
	sgAttn *= visibility;
	sgAttn = mClampF(sgAttn, 0.0f, 1.0f);
	Point4F color(sgLight->mColor.red, sgLight->mColor.green, sgLight->mColor.blue, sgAttn);
	color.x = (color.x + ((1.0f - color.x) * adjustment)) * sgAttn;
	color.y = (color.y + ((1.0f - color.y) * adjustment)) * sgAttn;
	color.z = (color.z + ((1.0f - color.z) * adjustment)) * sgAttn;
	color.x = mClampF(color.x, 0.0f, 1.0f);
	color.y = mClampF(color.y, 0.0f, 1.0f);
	color.z = mClampF(color.z, 0.0f, 1.0f);
#pragma message(ENGINE(动态阴影使用原来的颜色))
	GFX->setPixelShaderConstF(3, color, 1);
#elif 1
#pragma message(ENGINE(动态阴影使用原来的颜色，但是alpha值使用太阳的阴影色的alpha))
		Sun *sun = gClientSceneGraph->getCurrentSun();
		AssertFatal(sun, "TerrainProxy::postLight -- sun cannot be NULL");
		//alpha:用于调阴影的明暗,本处可以直接采用mColor.apha,暂时不用
		// 跟产生阴影的实体透明度相关
		F32 objVisibility = sgParentObject ? sgParentObject->getFinalVisibility() : 1.0f;
		Point4F color(sgLight->mColor.red, sgLight->mColor.green, sgLight->mColor.blue, sun->mDynShadowMod * objVisibility);
		//color.x = (color.x + ((1.0f - color.x) * adjustment)) * color.w;
		//color.y = (color.y + ((1.0f - color.y) * adjustment)) * color.w;
		//color.z = (color.z + ((1.0f - color.z) * adjustment)) * color.w;
		color.x = (color.x + ((1.0f - color.x) * adjustment));
		color.y = (color.y + ((1.0f - color.y) * adjustment));
		color.z = (color.z + ((1.0f - color.z) * adjustment));
		color.x = mClampF(color.x, 0.0f, 1.0f);
		color.y = mClampF(color.y, 0.0f, 1.0f);
		color.z = mClampF(color.z, 0.0f, 1.0f);

		GFX->setPixelShaderConstF(3, &color.x, 1);
#endif

	lighttoscreen.transposeTo(transposetemp);
	GFX->setVertexShaderConstF(0, transposetemp, 4);
	objecttolight.identity();
	objecttolight.transposeTo(transposetemp);
	GFX->setVertexShaderConstF(4, transposetemp, 4);
	GFX->setVertexShaderConstF(8, sgProjectionInfo, 1);
	GFX->setVertexShaderConstF(9, stride, 1);

	Point4F bias(sgBias, sgBias, 0.0f, 0.0f);
	Point3F center;
	sgParentObject->getWorldBox().getCenter(&center);
	center -= sgParentObject->getWorldBox().max;
	F32 rad = center.lenSquared();
	bias.x *= rad * sgSphereAdjust * sgSphereAdjust;
	GFX->setVertexShaderConstF(10, bias, 1);
	
	GFX->setVertexBuffer(sgShadowBuffer);
	GFX->setPrimitiveBuffer(mShadowPBuffer);
	GFX->drawIndexedPrimitive(GFXTriangleList, 0, sgShadowPoints.size(), 0, mPBTriangleNum);

	if(!sgAllowSelfShadowing() || (!sgAllowlodselfshadowing))
	{
		GFX->popWorldMatrix();
		return;
	}

#ifdef STATEBLOCK
	AssertFatal(mClearSB, "sgShadowProjector::render -- mClearSB cannot be NULL.");
	mClearSB->apply();  
	F32 lodattn = sgScaledcamdist / F32(sgspLastSelfShadowLOD + 1);
	lodattn = (1.0f - (lodattn * lodattn)) * 1.5f;
	sgAttn *= mClampF(lodattn, 0.0f, 1.0f);

	color = Point4F(sgLight->mColor.red, sgLight->mColor.green, sgLight->mColor.blue, sgAttn);
	color.x = (color.x + ((1.0 - color.x) * adjustment)) * sgAttn;
	color.y = (color.y + ((1.0 - color.y) * adjustment)) * sgAttn;
	color.z = (color.z + ((1.0 - color.z) * adjustment)) * sgAttn;
	//color.x = (1.0 - ((1.0 - color.x) * adjustment)) * attn;
	//color.y = (1.0 - ((1.0 - color.y) * adjustment)) * attn;
	//color.z = (1.0 - ((1.0 - color.z) * adjustment)) * attn;
	color.x = mClampF(color.x, 0.0, 1.0);
	color.y = mClampF(color.y, 0.0, 1.0);
	color.z = mClampF(color.z, 0.0, 1.0);
	GFX->setPixelShaderConstF(3, color, 1);
#else
	GFX->setCullMode(GFXCullCCW);   
	F32 lodattn = sgScaledcamdist / F32(sgspLastSelfShadowLOD + 1);
	lodattn = (1.0f - (lodattn * lodattn)) * 1.5f;
	sgAttn *= mClampF(lodattn, 0.0f, 1.0f);

	color = Point4F(sgLight->mColor.red, sgLight->mColor.green, sgLight->mColor.blue, sgAttn);
	color.x = (color.x + ((1.0 - color.x) * adjustment)) * sgAttn;
	color.y = (color.y + ((1.0 - color.y) * adjustment)) * sgAttn;
	color.z = (color.z + ((1.0 - color.z) * adjustment)) * sgAttn;
	//color.x = (1.0 - ((1.0 - color.x) * adjustment)) * attn;
	//color.y = (1.0 - ((1.0 - color.y) * adjustment)) * attn;
	//color.z = (1.0 - ((1.0 - color.z) * adjustment)) * attn;
	color.x = mClampF(color.x, 0.0, 1.0);
	color.y = mClampF(color.y, 0.0, 1.0);
	color.z = mClampF(color.z, 0.0, 1.0);
	GFX->setPixelShaderConstF(3, color, 1);

	// need to set these in case of fast partition rendering...
    GFX->setTextureStageMagFilter(0, GFXTextureFilterPoint);
	GFX->setTextureStageMinFilter(0, GFXTextureFilterPoint);

	GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
	GFX->setTextureStageAddressModeV(0, GFXAddressClamp);
#endif


	sgShadowReceivers.clear();
	// must be first object!!!
	sgShadowReceivers.push_back(sgParentObject);
	if(sgDTSShadow)
	{
		gClientContainer.findObjects(ShapeBaseObjectType,
			sgShadowProjector::dtsCallback, this);
	}

	for(U32 o=0; o<sgShadowReceivers.size(); o++)
	{
		// this class should handle any dts object type
		// to do this something needs to be done about the
		// ShapeBase cast, but for now only ShapeBase uses it...
		ShapeBase *obj = dynamic_cast<ShapeBase *>(sgShadowReceivers[o]);
		if(!obj)
			continue;

		TSShapeInstance *shape = obj->getShapeInstance();
		if(!shape)
			continue;

		MatrixF shapeworld;
		shapeworld = obj->getRenderTransform();
		shapeworld.scale(obj->getScale());
		objecttolight.mul(sgWorldToLightY, shapeworld);
		objecttoscreen.mul(worldtoscreen, shapeworld);
		sgRenderShape(shape, objecttolight, 4, objecttoscreen, 0);

		//ShapeBase *shapebase = dynamic_cast<ShapeBase *>(sgParentObject);
		for(U32 i=0; i<ShapeBase::MaxMountedImages; i++)
		{
			TSShapeInstance *mountinst = obj->getImageShapeInstance(i);
			if(!mountinst)
				continue;

			obj->getRenderImageTransform(i, &shapeworld);
			shapeworld.scale(obj->getScale());
			objecttolight.mul(sgWorldToLightY, shapeworld);
			objecttoscreen.mul(worldtoscreen, shapeworld);
			sgRenderShape(mountinst, objecttolight, 4, objecttoscreen, 0);
		}

		// this introduces too many visual issues, while providing
		// very little overhead reduction...
		// first, more, and allowed...
		//if(allowfastpartition && (sgShadowReceivers.size() > 1) && (o == 0))
		//	sgShadowShaderFastPartition->shader->process();
	}

	GFX->popWorldMatrix();
}

void sgShadowProjector::dtsCallback(SceneObject *obj, void *shadow)
{
	sgShadowProjector *shadowobj = reinterpret_cast<sgShadowProjector *>(shadow);
	if((obj != shadowobj->sgParentObject) &&
	   (obj->getWorldBox().isOverlapped(shadowobj->sgShadowBox)))
	{
		shadowobj->sgShadowReceivers.push_back(obj);
	}
}

void sgShadowProjector::sgDebugRenderProjectionVolume()
{
#ifdef STATEBLOCK
	AssertFatal(mSetDebugSB, "sgShadowProjector::sgDebugRenderProjectionVolume -- mSetDebugSB cannot be NULL.");
	mSetDebugSB->apply();
#else
	GFX->setSrcBlend(GFXBlendOne);
	//GFX->setDestBlend(GFXBlendOne);
	GFX->setDestBlend(GFXBlendZero);
	GFX->setTextureStageColorOp(0, GFXTOPDisable);
#endif


	GFX->disableShaders();

	PrimBuild::color(ColorF(1.0, 1.0, 0.0));
	PrimBuild::begin(GFXLineStrip, 20);
	PrimBuild::vertex3fv(testRenderPoints[0]);
	PrimBuild::vertex3fv(testRenderPoints[1]);
	PrimBuild::vertex3fv(testRenderPoints[3]);
	PrimBuild::vertex3fv(testRenderPoints[2]);
	PrimBuild::vertex3fv(testRenderPoints[0]);
	PrimBuild::vertex3fv(testRenderPoints[4]);
	PrimBuild::vertex3fv(testRenderPoints[5]);
	PrimBuild::vertex3fv(testRenderPoints[7]);
	PrimBuild::vertex3fv(testRenderPoints[6]);
	PrimBuild::vertex3fv(testRenderPoints[4]);
	PrimBuild::end();

	PrimBuild::begin(GFXLineStrip, 20);
	PrimBuild::vertex3fv(testRenderPoints[1]);
	PrimBuild::vertex3fv(testRenderPoints[5]);
	PrimBuild::end();

	PrimBuild::begin(GFXLineStrip, 20);
	PrimBuild::vertex3fv(testRenderPoints[2]);
	PrimBuild::vertex3fv(testRenderPoints[6]);
	PrimBuild::end();

	PrimBuild::begin(GFXLineStrip, 20);
	PrimBuild::vertex3fv(testRenderPoints[3]);
	PrimBuild::vertex3fv(testRenderPoints[7]);
	PrimBuild::end();
}

Point3F sgShadowProjector::sgGetCompositeShadowLightDirection()
{
	// don't want this to run too fast on newer systems (otherwise shadows snap into place)...
	U32 time = Platform::getRealMilliseconds();

	LightInfoList bestlights;
	SGLM->getBestLights(bestlights);

	if((time - sgPreviousShadowTime) < SG_DYNAMIC_SHADOW_TIME)
		return sgPreviousShadowLightingVector;

	sgPreviousShadowTime = time;

	// ok get started...
	U32 zone = sgParentObject->getCurrZone(0);

	U32 score;
	U32 maxscore[2] = {0, 0};
	LightInfo *light[2] = {NULL, NULL};
	VectorF vector[2] = {VectorF(0, 0, 0), VectorF(0, 0, 0)};

	for(U32 i=0; i<bestlights.size(); i++)
	{
		LightInfo *l = bestlights[i];

		if((l->mType == LightInfo::Ambient) || (l->mType == LightInfo::Vector))
			score = (U32)(l->mScore / SG_LIGHTMANAGER_SUN_PRIORITY);
		else if((l->mType == LightInfo::SGStaticPoint) || (l->mType == LightInfo::SGStaticSpot))
			score = (U32)(l->mScore / SG_LIGHTMANAGER_STATIC_PRIORITY);
		else
			score = (U32)(l->mScore / SG_LIGHTMANAGER_DYNAMIC_PRIORITY);

		if(score > maxscore[0])
		{
			light[1] = light[0];
			maxscore[1] = maxscore[0];

			light[0] = l;
			maxscore[0] = score;
		}
		else if(score > maxscore[1])
		{
			light[1] = l;
			maxscore[1] = score;
		}
	}

	for(U32 i=0; i<2; i++)
	{
		if(!light[i])
			break;

		if((light[i]->mType == LightInfo::Ambient) || (light[i]->mType == LightInfo::Vector))
		{
			if(zone == 0)
				vector[i] = light[i]->mDirection;
			else
				vector[i] = Point3F(0, 0, -1.0);
		}
		else
		{
			VectorF vect = sgParentObject->getPosition() - light[i]->mPos;
			vect.normalize();
			vector[i] = vect;
		}
	}

	VectorF vectcomposite = VectorF(0, 0, -1.0f);
	if(light[0])
	{
		if(light[1])
		{
			F32 ratio = F32(maxscore[0]) / F32(maxscore[0] + maxscore[1]);
			vectcomposite = (vector[0] * ratio) + (vector[1] * (1.0f - ratio));
		}
		else
			vectcomposite = vector[0];
	}

	VectorF step = (vectcomposite - sgPreviousShadowLightingVector) / SG_DYNAMIC_SHADOW_STEPS;
	sgPreviousShadowLightingVector += step;
	sgPreviousShadowLightingVector.normalize();

	return sgPreviousShadowLightingVector;
}


void sgShadowProjector::resetStateBlock()
{
	//mClearStatusSB
	GFX->beginStateBlock();
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPDisable );
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPDisable );
	GFX->setTextureStageState( 2, GFXTSSColorOp, GFXTOPDisable );
	GFX->setTextureStageState( 3, GFXTSSColorOp, GFXTOPDisable );
	GFX->setTextureStageState( 4, GFXTSSColorOp, GFXTOPDisable );
	GFX->setSamplerState(0, GFXSAMPMagFilter, GFXTextureFilterLinear);
	GFX->setSamplerState(0, GFXSAMPMinFilter, GFXTextureFilterLinear);
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendDestAlpha);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->setRenderState(GFXRSDepthBias, 0);
	GFX->endStateBlock(mClearStatusSB);

	//mSetOneSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendDestAlpha);
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPDisable );
	GFX->endStateBlock(mSetOneSB);

	//mSetTwoSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, false);
#if 1
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendDestColor);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendZero);
#else
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
#endif
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreater);
	GFX->setRenderState(GFXRSAlphaRef, 0);
	GFX->endStateBlock(mSetTwoSB);

	//mSetAllowFastSB
	GFX->beginStateBlock();
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 2, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 3, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 4, GFXTSSColorOp, GFXTOPDisable );
	//新加 2009-10-22
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendDestColor);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendZero);
	GFX->endStateBlock(mSetAllowFastSB);

	//mSetSB
	GFX->beginStateBlock();
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 2, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 3, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 4, GFXTSSColorOp, GFXTOPDisable );
	//新加 2009-10-22
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendDestColor);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendZero);
	GFX->endStateBlock(mSetSB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
	GFX->setSamplerState(0, GFXSAMPMagFilter, GFXTextureFilterPoint);
	GFX->setSamplerState(0, GFXSAMPMinFilter, GFXTextureFilterPoint);
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressClamp);
	GFX->endStateBlock(mClearSB);

	//mSetDebugSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendZero);
	GFX->setTextureStageState(0 , GFXTSSColorOp, GFXTOPDisable );
	GFX->endStateBlock(mSetDebugSB);
}


void sgShadowProjector::releaseStateBlock()
{
	if (mSetOneSB)
	{
		mSetOneSB->release();
	}

	if (mSetTwoSB)
	{
		mSetTwoSB->release();
	}

	if (mClearStatusSB)
	{
		mClearStatusSB->release();
	}

	if (mSetAllowFastSB)
	{
		mSetAllowFastSB->release();
	}

	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mClearSB)
	{
		mClearSB->release();
	}

	if (mSetDebugSB)
	{
		mSetDebugSB->release();
	}
}

void sgShadowProjector::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		mSetOneSB = new GFXD3D9StateBlock;
		mSetTwoSB = new GFXD3D9StateBlock;
		mClearStatusSB = new GFXD3D9StateBlock;
		mSetAllowFastSB = new GFXD3D9StateBlock;
		mClearSB = new GFXD3D9StateBlock;
		mSetDebugSB = new GFXD3D9StateBlock;

		resetStateBlock();
	}
}

void sgShadowProjector::shutdown()
{
	SAFE_DELETE(mSetOneSB);
	SAFE_DELETE(mSetTwoSB);
	SAFE_DELETE(mClearStatusSB);
	SAFE_DELETE(mSetAllowFastSB);
	SAFE_DELETE(mClearSB);
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mSetDebugSB);
}
