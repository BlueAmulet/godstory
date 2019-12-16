//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "ts/tsSimpleShape.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/detailManager.h"
#include "math/mathIO.h"
#include "ts/tsShapeInstance.h"
#include "console/consoleTypes.h"
#include "gfx/gfxDevice.h"
#include "gfx/gBitmap.h"
#include "Gameplay/Data//GraphicsProfile.h"
#include "materials/matInstance.h"
#include "effects/EffectPacket.h"

#include "util/aniThread.h"


IMPLEMENT_CONOBJECT(TSSimpleShape);

//--------------------------------------------------------------------------
TSSimpleShape::TSSimpleShape()
{
#ifdef NTJ_CLIENT
   mNetFlags.set(Ghostable | ScopeAlways | IsGhost);
#else
   mNetFlags.clear(Ghostable | ScopeAlways | IsGhost);
#endif

   mTypeMask |= StaticObjectType | StaticRenderedObjectType | ShadowCasterObjectType;

   mShapeName        = "";
   mShapeInstance    = NULL;
   mAmbientAnimation = NULL;

   mLastRenderTime   = 0;
   mLastAnimateRenderTime = 0;
   mLastRenderPos = 0;
   mInvisibleCycle = 0;
   mFadeVal = 0;
}

TSSimpleShape::~TSSimpleShape()
{
}

bool TSSimpleShape::loadRenderResource()
{
	//mShapeInstance->cloneMaterialList(mShape->materialList);
	return Parent::loadRenderResource();
}

void TSSimpleShape::freeRenderResource()
{
	//mShapeInstance->freeMaterialList();
	Parent::freeRenderResource();
}

//--------------------------------------------------------------------------
bool TSSimpleShape::onAdd()
{
   PROFILE_SCOPE(TSSimpleShape_onAdd);
   if(!Parent::onAdd())
      return false;

   if (!mShapeName || mShapeName[0] == '\0') {
      Con::errorf("TSSimpleShape::onAdd: no shape name!");
      return false;
   }
#ifdef NTJ_SERVER
   resetWorldBox();
   setRenderTransform(mObjToWorld);
   addToScene();
   return true;
#endif

   mShape = ResourceManager->load(mShapeName);
   if (bool(mShape) == false)
   {
      Con::errorf("TSSimpleShape::onAdd: unable to load shape: %s", mShapeName);
      return false;
   }
   bool matPreload = isClientObject();

   if(matPreload && !mShape->preloadMaterialList())
	   return false;

#ifdef NTJ_EDITOR
   mObjBox = mShape->bounds;
#endif

   resetWorldBox();
   setRenderTransform(mObjToWorld);

   mShapeInstance = new TSShapeInstance(mShape, matPreload);

	//Ray: 增加对动画物体的支持，规则和老插件一样。
	S32 sequence = mShape->findSequence("ambient");
	if(sequence != -1)
	{
		mAmbientAnimation= mShapeInstance->addThread();
		mShapeInstance->setPos(mAmbientAnimation, 0);
		mShapeInstance->setTimeScale(mAmbientAnimation, 1.0f);
		mShapeInstance->setSequence(mAmbientAnimation, sequence, 0);
	}

   addToScene();
   return true;
}

void TSSimpleShape::onRemove()
{
   removeFromScene();

#ifdef USE_MULTITHREAD_ANIMATE
   if(g_MultThreadWorkMgr && g_MultThreadWorkMgr->isEnable())
	   g_MultThreadWorkMgr->addInDeleteingShapeInsList(mShapeInstance);
   else
	   delete mShapeInstance;
#else
   delete mShapeInstance;
#endif

   mShapeInstance = NULL;

   Parent::onRemove();
}

//--------------------------------------------------------------------------
bool TSSimpleShape::prepRenderImage(SceneState* state, const U32 stateKey,
                                       const U32 /*startZone*/, const bool /*modifyBaseState*/)
{
	if(!isResourceLoaded())
		return false;

   if (isLastState(state, stateKey))
      return false;
   setLastState(state, stateKey);

   // This should be sufficient for most objects that don't manage zones, and
   //  don't need to return a specialized RenderImage...

   if( mShapeInstance && (state->isObjectRendered(this) || gClientSceneGraph->isReflectPass()) )
   {
      Point3F cameraOffset;
      getRenderTransform().getColumn(3,&cameraOffset);
      cameraOffset -= state->getCameraPosition();
      F32 dist = cameraOffset.len();
      if (dist < 0.01f)
         dist = 0.01f;
      F32 fogAmount = state->getHazeAndFog(dist,cameraOffset.z);
      if (fogAmount>0.99f)
         return false;

      F32 invScale = (1.0f/getMax(getMax(mObjScale.x,mObjScale.y),mObjScale.z));
      DetailManager::selectPotentialDetails(mShapeInstance,dist,invScale);
      if (mShapeInstance->getCurrentDetail()<0)
         return false;

      renderObject( state );
   }

   return false;
}


void TSSimpleShape::setTransform(const MatrixF & mat)
{
   Parent::setTransform(mat);

   // Since the interior is a static object, it's render transform changes 1 to 1
   //  with it's collision transform
   setRenderTransform(mat);
}

void TSSimpleShape::setAnimationPos(F32 pos)
{
	if(mAmbientAnimation && mShapeInstance)
		mShapeInstance->setPos(mAmbientAnimation, pos);
}

F32 TSSimpleShape::setInvisibleCycle(S32 cycle)
{
	if(mAmbientAnimation && mShapeInstance && mShapeInstance->getDuration(mAmbientAnimation) > 0.0f)
	{
		mInvisibleCycle = cycle;
		return mInvisibleCycle * mShapeInstance->getDuration(mAmbientAnimation);
	}
	return 0.0f;
}

void TSSimpleShape::checkCycle(F32 dur)
{
	if(mAmbientAnimation && mShapeInstance && mInvisibleCycle > 0)
	{
		F32 c = mLastRenderPos + 0.1f + dur*mShapeInstance->getTimeScale(mAmbientAnimation)/mShapeInstance->getDuration(mAmbientAnimation);
		mInvisibleCycle -= mFloor(c);
		if(mInvisibleCycle <= 0)
			setFade(RenderableSceneObject::Fade_Eternal, true, false);
	}
}

void TSSimpleShape::renderObject(SceneState* state)
{
   MatrixF proj = GFX->getProjectionMatrix();
   RectI viewport = GFX->getViewport();

   MatrixF world = GFX->getWorldMatrix();
   TSMesh::setCamTrans( world );
   TSMesh::setSceneState( state );
   TSMesh::setObject(this);

   GFX->pushWorldMatrix();


   gClientSceneGraph->getLightManager()->setupLights(this);

   MatrixF mat = getRenderTransform();
   mat.scale( mObjScale );
   GFX->setWorldMatrix( mat );

   bool firstAnimate = false;
   U32 currentTime = Platform::getVirtualMilliseconds();
   if(!mLastAnimateRenderTime)
   {
	   firstAnimate = true;
	   mLastAnimateRenderTime = currentTime;
   }
   
   if(!gClientSceneGraph->isReflectPass() && mAmbientAnimation && g_GraphicsProfile.mStaticAnimation)
   {
	   F32 delta = (currentTime - mLastAnimateRenderTime) * 0.001f;
	   //if(delta>0.05)
	   {
		   mLastAnimateRenderTime = currentTime;
		   mShapeInstance->advanceTime(delta, mAmbientAnimation);
#if (defined(USE_MULTITHREAD_ANIMATE) && !defined(NTJ_SERVER))
		   if(g_MultThreadWorkMgr->isEnable() && mShapeInstance->isAnimationInited())
				g_MultThreadWorkMgr->addInAnimateShapeInsList(mShapeInstance);
		   else
		   {
			   mShapeInstance->setAnimationInited(true);
			   mShapeInstance->animate();
		   }
#else
		   mShapeInstance->animate();
#endif

		   checkCycle(delta);
		   mLastRenderPos = mShapeInstance->getPos(mAmbientAnimation);
	   }
   }

   PrepRenderPushFade();
   mShapeInstance->render();
   PrepRenderPopFade();

   gClientSceneGraph->getLightManager()->resetLights();
   mLastRenderTime = currentTime;

   GFX->popWorldMatrix();


   GFX->setProjectionMatrix( proj );
   GFX->setViewport( viewport );

}

void TSSimpleShape::getNodeMatrix(StringTableEntry nodeName,MatrixF* mat)
{
	if(!mShape || !mShapeInstance || !nodeName || !nodeName[0])
	{
		mat->identity();
		return;
	}

	U32 node = mShape->findNode(nodeName);
	if(node==-1)
	{
		mat->identity();
		return;
	}

	*mat = mShapeInstance->getNodeTransforms()[node];
}

void TSSimpleShape::getRenderNodeTransform(StringTableEntry nodeName,MatrixF* mat)
{
	if(!mShape || !mShapeInstance)
	{
		mat->mul(getRenderTransform());
		return;
	}
	// Returns point to world space transform
	S32 ni = mShape->findNode(nodeName);
	if (ni != -1) {
		MatrixF mountTransform = mShapeInstance->getNodeTransforms()[ni];
		const Point3F& scale = getScale();

		// The position of the node point needs to be scaled.
		Point3F position = mountTransform.getPosition();
		position.convolve( scale );
		mountTransform.setPosition( position );

		// Also we would like the object to be scaled to the model.
		mountTransform.scale( scale );
		mat->mul(getRenderTransform(), mountTransform);
		return;
	}
	*mat = getRenderTransform();
}

#ifdef USE_MULTITHREAD_ANIMATE
void TSSimpleShape::deleteObject()
{
#ifndef NTJ_SERVER
	if(g_MultThreadWorkMgr->isEnable() && mAmbientAnimation)
	{
		removeObjectFromScene();
		g_MultThreadWorkMgr->addInDeleteingObjectInsList(this);
	}else
		Parent::deleteObject();
#else
	Parent::deleteObject();
#endif
}
#endif
