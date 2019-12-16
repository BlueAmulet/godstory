//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "T3D/tsStatic.h"
#include "core/bitStream.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/lightManager.h"
#include "math/mathIO.h"
#include "ts/tsShapeInstance.h"
#include "console/consoleTypes.h"
#include "T3D/shapeBase.h"
#include "sceneGraph/detailManager.h"
#include "sim/netConnection.h"
#include "gfx/gfxDevice.h"
#include "gfx/gBitmap.h"
#include "gfx/gfxTransformSaver.h"
#include "materials/sceneData.h"
#include "Gameplay/Data//GraphicsProfile.h"
#include "lightingSystem/synapseGaming/sgObjectBasedProjector.h"
#include "terrain/terrData.h"
#include "gfx/debugDraw.h"
#include "gfx/gfxDrawUtil.h"
#include "util/aniThread.h"


#ifdef NTJ_CLIENT
#include "Gameplay/LoadMgr/ObjectLoadManager.h"
#endif

#ifdef NTJ_EDITOR
#include "ui/dWorldEditor.h"
#endif
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK
GFXStateBlock* TSStatic::mSetSB = NULL;
GFXStateBlock* TSStatic::mClearSB = NULL;

F32 TSStatic::mFadePerMilliSec  = 0.003f;

#include "materials/matInstance.h"

#define BOX_QUERY

static bool useDebugBox = false;

extern bool gEditingMission;


IMPLEMENT_CO_NETOBJECT_V1(TSStatic);

DECLARE_SERIALIBLE(TSStatic);

//--------------------------------------------------------------------------
TSStatic::TSStatic()
{
   overrideOptions = false;

   mNetFlags.set(Ghostable | ScopeAlways);

   mTypeMask |= StaticObjectType | StaticTSObjectType |
	   StaticRenderedObjectType | ShadowCasterObjectType;

   mShapeName        = "";
   mShapeInstance    = NULL;
   mAmbientAnimation = NULL;
   mLightMap		 = NULL;
   mCrc				 = 0;

   mIsTransparency	 = false;
   mAllowTransparency= false;
   mVisibility     = 1.0f;
   mLastAnimateRenderTime = 0;

   mAllowReceiveShadow = false;
   mAllowPlayerStep = false;
   mAllowStaticShadowOnTerrain = false;
   mAllowDynamicShadowOnTerrain = false;
   mAllowReflection = false;

   mConvexList = new Convex;

   mTinyStaticLevel = 0;
   bShadowBoxDirty = true;

   receiveLMLighting = false;

   mAsField = true;
}

TSStatic::~TSStatic()
{
   delete mConvexList;
   mConvexList = NULL;

   SAFE_DELETE(mLightMap);
}

//--------------------------------------------------------------------------
void TSStatic::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Media");
   addField("shapeName", TypeFilename, Offset(mShapeName, TSStatic));
   endGroup("Media");

   addGroup("Lighting");
   addField("receiveSunLight", TypeBool, Offset(receiveSunLight, SceneObject));
   addField("receiveLMLighting", TypeBool, Offset(receiveLMLighting, SceneObject));
   addField("staticShadowOnTerrain", TypeBool, Offset(mAllowStaticShadowOnTerrain, TSStatic));
   addField("dynamicShadowOnTerrain", TypeBool, Offset(mAllowDynamicShadowOnTerrain, TSStatic));
   addField("ReceiveShadow", TypeBool, Offset(mAllowReceiveShadow, TSStatic));
   addField("waterReflection", TypeBool, Offset(mAllowReflection, TSStatic));
   addField("useAdaptiveSelfIllumination", TypeBool, Offset(useAdaptiveSelfIllumination, SceneObject));
   addField("useCustomAmbientLighting", TypeBool, Offset(useCustomAmbientLighting, SceneObject));
   //addField("customAmbientSelfIllumination", TypeBool, Offset(customAmbientForSelfIllumination, SceneObject));
   addField("customAmbientLighting", TypeColorF, Offset(customAmbientLighting, SceneObject));
   addField("lightGroupName", TypeString, Offset(lightGroupName, SceneObject));
   addField("AsField", TypeBool, Offset(mAsField,TSStatic));
   endGroup("Lighting");

   addGroup("Collision");
   addField("allowTransparency", TypeBool, Offset(mAllowTransparency, TSStatic));
   addField("allowPlayerStep", TypeBool, Offset(mAllowPlayerStep, TSStatic));
   addField("TinyStaticLevel", TypeS32, Offset(mTinyStaticLevel, TSStatic));
   endGroup("Collision");

}

void TSStatic::inspectPostApply()
{
   if(isServerObject()) {
      setMaskBits(advancedStaticOptionsMask);
   }
}

bool TSStatic::loadRenderResource()
{
	//mShapeInstance->loadMaterialList();
	return Parent::loadRenderResource();
}

void TSStatic::freeRenderResource()
{
 	//mShapeInstance->freeMaterialList();
	//if(mLightPlugin)
	//	mLightPlugin->freeResource();
	Parent::freeRenderResource();
}

//--------------------------------------------------------------------------
bool TSStatic::onAdd()
{
#ifdef NTJ_SERVER
	return false;
#endif
   PROFILE_SCOPE(TSStatic_onAdd);

   if(!Parent::onAdd())
      return false;

   if (!mShapeName || mShapeName[0] == '\0') {
      Con::errorf("TSStatic::onAdd: no shape name!");
      return false;
   }
    
   if (!mShape)
   {
       mShape = ResourceManager->load(mShapeName);

       if (bool(mShape) == false)
       {
           Con::errorf("TSStatic::onAdd: unable to load shape: %s", mShapeName);
           return false;
       }
   }
   
   bool matPreload = isClientObject();

//#ifdef NTJ_CLIENT
//   matPreload = false;		//客户端不再预载贴图
//   setResourceLoaded(matPreload);
//#endif

   if(matPreload && !mShape->preloadMaterialList() && NetConnection::filesWereDownloaded())
	   return false;

   char strtmp[1024];
   dStrcpy(strtmp, sizeof(strtmp), mShapeName);
   for(int i=0;i<4;i++)
   {
	   for(int j=0;j<4;j++)
	   {
			dSprintf(strtmp,sizeof(strtmp),"%s%.2f",strtmp,mObjToWorld(i,j));
	   }
   }
   mCrc = calculateCRC(strtmp,strlen(strtmp));

   mShapeInstance = new TSShapeInstance(mShape, matPreload);

   prepCollision();

#ifdef NTJ_EDITOR
   mObjBox = mShape->bounds;
#endif

   resetWorldBox();
   setRenderTransform(mObjToWorld);

#ifndef NTJ_SERVER
	//Ray: 增加对动画物体的支持，规则和老插件一样。
	S32 sequence = mShape->findSequence("ambient");
	if(sequence != -1)
	{
		mAmbientAnimation= mShapeInstance->addThread();
		mShapeInstance->setPos(mAmbientAnimation, 0);
		mShapeInstance->setTimeScale(mAmbientAnimation, 1.0f);
		mShapeInstance->setSequence(mAmbientAnimation, sequence, 0);
	}
#endif

   addToScene();

#ifdef DEBUG
   if(isClientObject())
		gClientSceneGraph->staticNum++;
#endif

#ifdef NTJ_CLIENT
    gClientObjectLoadMgr.registObject(this, getWorldBox());		//客户端接受载入管理
#endif
	// <Edit> [7/21/2009 joy] 设置可行走标志
	if(mAllowPlayerStep)
		mTypeMask |= PlayerStepObjectType;
	else
		mTypeMask &= ~PlayerStepObjectType;

#if (defined(BOX_QUERY) && !defined(NTJ_SERVER))
   if(!mpObjBoxVB)
   {
	   mpObjBoxVB = new GFXVertexBufferHandle<GFXVertexP>;
	   Box3F box = mObjBox;
	   box.min.convolve(mObjScale);
	   box.max.convolve(mObjScale);
	   Point3F size = (box.max - box.min) * 0.5;
	   Point3F center = (box.min + box.max) * 0.5;
	   GFX->getDrawUtil()->buildCube( size,center,mpObjBoxVB);
   }
#endif
   return true;
}

void TSStatic::GetLightMapSize(U32 &Width ,U32 &Height)
{
	Width = 64;
	Height = 64;
}

void TSStatic::prepCollision()
{
   // We're using the convex collision system,
   // so just do that.

   // Cleanup any old collision data.
   mCollisionDetails.clear();
   mLOSDetails.clear();

   // Scan out the collision hulls...
   U32 i;
   for (i = 0; i < mShape->details.size(); i++)
   {
      char* name = (char*)mShape->names[mShape->details[i].nameIndex];

      if (dStrstr((const char*)dStrlwr(name), "collision-"))
      {
         mCollisionDetails.push_back(i);

         // The way LOS works is that it will check to see if there is a LOS detail that matches
         // the the collision detail + 1 + MaxCollisionShapes (this variable name should change in
         // the future). If it can't find a matching LOS it will simply use the collision instead.
         // We check for any "unmatched" LOS's further down
         mLOSDetails.increment();

      char buff[128];
         dSprintf(buff, sizeof(buff), "LOS-%d", i + 1 + MaxCollisionShapes);
         U32 los = mShape->findDetail(buff);
         if (los == -1)
            mLOSDetails.last() = i;
         else
            mLOSDetails.last() = los;
      }
	  else if(!dStrnicmp(name,"detail",6))
	  {
		  mMeshDetails.push_back(i);
	  }
   }

   // Snag any "unmatched" LOS details
   for (i = 0; i < mShape->details.size(); i++)
   {
      char* name = (char*)mShape->names[mShape->details[i].nameIndex];

      if (dStrstr((const char*)dStrlwr(name), "los-"))
      {
         // See if we already have this LOS
         bool found = false;
         for (U32 j = 0; j < mLOSDetails.size(); j++)
         {
            if (mLOSDetails[j] == i)
            {
               found = true;
               break;
            }
         }

         if (!found)
            mLOSDetails.push_back(i);
      }
   }

   // Compute the hull accelerators (actually, just force the shape to compute them)
   for (i = 0; i < mCollisionDetails.size(); i++)
         mShapeInstance->getShape()->getAccelerator(mCollisionDetails[i]);

   //Ray: 美术建立的BOUNDS非常不可靠，不如靠自己计算。。。。。
   bool computsFlag = true;

   S32 sequence = mShape->findSequence("ambient");
   if(sequence != -1)
	   computsFlag = false;  //动画物体计算困难
   
   if(computsFlag)
   {
	   Box3F &bounds = mShapeInstance->getShape()->bounds;
	   bounds.min.set( 10E30f, 10E30f, 10E30f);
	   bounds.max.set(-10E30f,-10E30f,-10E30f);

	   Box3F tmpBounds;
	   for (i = 0; i < mMeshDetails.size(); i++)
	   {
		   mShapeInstance->getShape()->computeBounds(mMeshDetails[i], tmpBounds);
		   bounds.min.setMin(tmpBounds.min);
		   bounds.max.setMax(tmpBounds.max);
	   }
   }
}


void TSStatic::onRemove()
{
   mConvexList->nukeList();

#ifdef NTJ_CLIENT
   gClientObjectLoadMgr.unregistObject(this, getWorldBox());
#endif

#ifdef DEBUG
   if(isClientObject())
	   gClientSceneGraph->staticNum--;
#endif

   removeFromScene();

   if(mShapeInstance)
   {
#ifdef USE_MULTITHREAD_ANIMATE
	   if(g_MultThreadWorkMgr && g_MultThreadWorkMgr->isEnable())
		   g_MultThreadWorkMgr->addInDeleteingShapeInsList(mShapeInstance);
	   else
		   delete mShapeInstance;
#else
	   delete mShapeInstance;
#endif
	   mShapeInstance = NULL;
   }

   Parent::onRemove();
}

//--------------------------------------------------------------------------

void TSStatic::PrepVisibility()
{
#ifndef NTJ_SERVER
	F32 dt = Platform::getVirtualMilliseconds() - mLastRenderTime;
	if(mFading)
	{
		mFadeElapsedTime += dt;
		if(mFadeElapsedTime > mFadeTime + mFadeDelay)
		{
			mFadeVal = F32(!mFadeOut);
			mFading = false;
		}
		else if(mFadeElapsedTime > mFadeDelay)
		{
			mFadeVal = (mFadeElapsedTime - mFadeDelay) / mFadeTime;
			if(mFadeOut)
				mFadeVal = 1 - mFadeVal;
		}
	}
	if(mAllowTransparency && (mIsTransparency || mVisibility < 0.999f))
	{
		F32 delta = dt * mFadePerMilliSec * mClampF(mVisibility,0.2f,1.0f); // 平滑虚化过程
		if(mIsTransparency)
			mVisibility -= delta;
		else
			mVisibility += delta;
		mVisibility = mClampF(mVisibility, 0.3f, 1);
		mIsTransparency = false;
	}
	mFinalVisibility = mFadeVal * mVisibility;
	mLastRenderTime = Platform::getVirtualMilliseconds();
#endif
}

bool TSStatic::prepRenderImage(SceneState* state, const U32 stateKey,
                                       const U32 /*startZone*/, const bool /*modifyBaseState*/)
{
	if(!isResourceLoaded())
		return false;

#ifdef NTJ_EDITOR
	if(gEditingMission && isHidden())
		return false;
#endif

	if(mTinyStaticLevel > g_GraphicsProfile.mTinyStaticLevel)
		return false;

	if(gClientSceneGraph->isReflectPass() && !mAllowReflection)
		return false;

#ifdef NTJ_EDITOR
	if(!WorldEditor::mRenderStatic)
		return false;
#endif 

   if (isLastState(state, stateKey))
      return false;
   setLastState(state, stateKey);

   // This should be sufficient for most objects that don't manage zones, and
   //  don't need to return a specialized RenderImage...

   if( mShapeInstance && (isOnlyRenderShadow() || gClientSceneGraph->isReflectPass() || state->isObjectRendered(this)) )
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

	  bool bQuery = false;
	  
	  if(!isOnlyRenderShadow() && !gClientSceneGraph->isReflectPass())
	  {
		  if(mLastRenderKey != SceneGraph::smFrameKey-1 || getQueryResult()<50)
				bQuery = true;

		  mLastRenderKey = SceneGraph::smFrameKey;

#ifdef NTJ_EDITOR
		  bQuery = bQuery && !gEditingMission;
#endif	  
	  
	  }

	  bQuery = false;

	  PrepRenderPushFade();
	  renderObject(bQuery, state );
	  PrepRenderPopFade();
   }

   return false;
}


void TSStatic::setTransform(const MatrixF & mat)
{
   Parent::setTransform(mat);

   // Since the interior is a static object, it's render transform changes 1 to 1
   //  with it's collision transform
   setRenderTransform(mat);
   bShadowBoxDirty = true;
}

void TSStatic::prepRenderQuery(SceneState* state)
{
	RenderInst *coreRI = gRenderInstManager.allocInst();
	coreRI->obj = this;
	coreRI->sortPoint  = mWorldBox.getClosestPoint(state->getCameraPosition());      

	//mObjBox.getCenter(&coreRI->sortPoint);

	//Point3F pos = state->getCameraPosition();
	//mWorldToObj.mulP(pos);
	//coreRI->sortPoint  = mObjBox.getClosestPoint(pos);      
	//mObjToWorld.mulP(coreRI->sortPoint);

	gRenderInstManager.addQueryInst(coreRI);
}

void TSStatic::prepRenderShadowQuery()
{
	RenderInst *coreRI = gRenderInstManager.allocInst();
	mShadowBox.getCenter(&coreRI->sortPoint);
	coreRI->obj = this;

	gRenderInstManager.addShadowQueryInst(coreRI);
}

void TSStatic::renderShadowQuery()
{
	//static ColorF color(1.0,1.0,1.0,1.0);
	//Point3F d = mShadowBox.min - mShadowBox.max;
	//GFX->getDrawUtil()->drawWireCube(d * 0.5, (mShadowBox.min + mShadowBox.max) * 0.5, color);

	if(useDebugBox)
	{
#ifdef STATEBLOCK
		AssertFatal(mSetSB, "TSStatic::renderShadowQuery -- mSetSB cannot be NULL.");
		mSetSB->apply();
#else
		GFX->setZWriteEnable( true );
		GFX->enableColorWrites(true,true,true,true);
#endif

	}

	GFX->setVertexBuffer(mpShadowBoxVB->getPointer());
	GFX->getDrawUtil()->drawCube();

	if(useDebugBox)
	{
#ifdef STATEBLOCK
		AssertFatal(mClearSB, "TSStatic::renderShadowQuery -- mClearSB cannot be NULL.");
		mClearSB->apply();
#else
		GFX->setZWriteEnable( false );
		GFX->enableColorWrites(false,false,false,false);
#endif
	}
}

void TSStatic::renderQuery()
{
	GFX->pushWorldMatrix();

	MatrixF mat = getRenderTransform();
#ifndef BOX_QUERY
	mat.scale( mObjScale );
#endif
	GFX->multWorld( mat );

#ifdef BOX_QUERY

	if(useDebugBox)
	{
#ifdef STATEBLOCK
		AssertFatal(mSetSB, "TSStatic::renderShadowQuery -- mSetSB cannot be NULL.");
		mSetSB->apply();
#else
		GFX->setZWriteEnable( true );
		GFX->enableColorWrites(true,true,true,true);
#endif
	}

	GFX->setVertexBuffer(mpObjBoxVB->getPointer());
	GFX->getDrawUtil()->drawCube();

	if(useDebugBox)
	{
#ifdef STATEBLOCK
		AssertFatal(mClearSB, "TSStatic::renderShadowQuery -- mClearSB cannot be NULL.");
		mClearSB->apply();
#else
		GFX->setZWriteEnable( false );
		GFX->enableColorWrites(false,false,false,false);
#endif
	}

#else
	mShapeInstance->renderQuery();
#endif

	GFX->popWorldMatrix();
}

void TSStatic::renderObject(bool bQuery,SceneState* state)
{
	if(useDebugBox)
		bQuery = true;

   if(bQuery)
		prepRenderQuery(state);

   MatrixF proj = GFX->getProjectionMatrix();
   RectI viewport = GFX->getViewport();

   MatrixF world = GFX->getWorldMatrix();
   TSMesh::setCamTrans( world );
   TSMesh::setSceneState( state );
   TSMesh::setObject(this);
   TSMesh::setFieldMesh(mAllowReceiveShadow && mAsField);

   GFX->pushWorldMatrix();

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

   if(!bQuery && !gClientSceneGraph->isReflectPass() && mAmbientAnimation && g_GraphicsProfile.mStaticAnimation)
   {
	   F32 TimeDelta = (currentTime - mLastAnimateRenderTime) * 0.001f;
	   if(TimeDelta>0.05)
	   {
		   mLastAnimateRenderTime = currentTime;
		   mShapeInstance->advanceTime(TimeDelta, mAmbientAnimation);
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
	   }
   }

   bool bRenderShadow = !gClientSceneGraph->isReflectPass();

   if(!bQuery && !isOnlyRenderShadow())
   {
	   gClientSceneGraph->getLightManager()->setupLights(this);

	   //Ray: 为每个对象设置自己的lightmap，这个和统一的材质无关所以无法关联
	   //TSMesh::setLightmap(mLightMapTex);

       F32 oldFogFactor = TSMesh::getFogFactor();

	   mShapeInstance->render();
#ifdef DEBUG
	   gClientSceneGraph->addInCameraStatic(this);
#endif

	   //TSMesh::setLightmap(NULL);

	   gClientSceneGraph->getLightManager()->resetLights();
   }

#ifdef NTJ_EDITOR
   if(!dWorldEditor::m_enableRenderShadows)
	   bRenderShadow = false;
#endif

   if(bRenderShadow)
   {
	   if(mAllowDynamicShadowOnTerrain && g_GraphicsProfile.mStaticShadowLevel)
	   {
		   bool bShadowQuery = useDebugBox || getShadowQueryResult()<50;

#ifdef NTJ_EDITOR
		   bShadowQuery = bShadowQuery && !gEditingMission;
#endif	
		   bShadowQuery = false;

		   if(bShadowQuery)
		   {
			   if (bShadowBoxDirty)
				   calShadowBox();

			   prepRenderShadowQuery();
		   }
		   else
		   {
			   renderShadow( state );
#ifdef DEBUG
			   gClientSceneGraph->addInCameraShadow(this);
#endif
		   }
	   }
   }

   mLastRenderTime = currentTime;

   GFX->popWorldMatrix();


   GFX->setProjectionMatrix( proj );
   GFX->setViewport( viewport );

   TSMesh::setFieldMesh(false);
}

bool TSStatic::shadowTest(SceneState *state)
{
	if(!isResourceLoaded())
		return false;

	if(!mAllowDynamicShadowOnTerrain || !g_GraphicsProfile.mStaticShadowLevel)
		return false;

	if(gClientSceneGraph->isReflectPass())
		return false;

	if(!state)
		return false;

	if (bShadowBoxDirty)
		calShadowBox();

	const Frustum& frustum = state->getFrustum();
	if (frustum.intersects(mShadowBox))
	{
		return true;
	}
	return false;
}

void TSStatic::calShadowBox()
{
	//Ray:BOX直接从点计算获得
	getRenderTransform().mul(mShadowBox);
	bShadowBoxDirty = false;
}

void TSStatic::renderShadow(SceneState *state)
{
	if(mLightPlugin)
	{
		mLightPlugin->renderShadow(this, state);
	}
}

void TSStatic::setShadowVariables(sgShadowProjector *pShadow )
{
	pShadow->sgEnable = mAllowDynamicShadowOnTerrain;
#ifdef NTJ_EDITOR
	pShadow->sgCanMove = true;
#else
	pShadow->sgCanMove = false;
#endif
	pShadow->sgCanRTT = false;
	pShadow->sgFrameSkip = 10;
	pShadow->sgMaxVisibleDistance = 80.0f;
	pShadow->sgBias = 0.0005f;
	pShadow->sgDTSShadow = false;
	pShadow->sgIntensity = 1.0f;

	pShadow->sgDirtySync(pShadow->sgCanSelfShadow, bool(false));
	pShadow->sgDirtySync(pShadow->sgRequestedShadowSize, U32(64));
	pShadow->sgDirtySync(pShadow->sgProjectionDistance, F32(25.0f));
	pShadow->sgDirtySync(pShadow->sgSphereAdjust,F32(1.0));
}

void TSStatic::renderShadowBuff(sgShadowProjector *pShadow ,MatrixF &proj,MatrixF &lightspace,MatrixF &sgWorldToLightY,MatrixF &shadowscale)
{
	MatrixF newmat;
	newmat.mul(proj, lightspace);
	pShadow->sgRenderShape(mShapeInstance, newmat, 0, newmat, -1);
}

U64 TSStatic::packUpdate(NetConnection *con, U64 mask, BitStream *stream)
{
   U64 retMask = Parent::packUpdate(con, mask, stream);

   mathWrite(*stream, getTransform());
   mathWrite(*stream, getScale());
   stream->writeString(mShapeName);

   stream->writeFlag(mAsField);
   stream->writeFlag(mAllowTransparency);
   stream->writeFlag(mAllowReceiveShadow);
   stream->writeFlag(mAllowPlayerStep);
   stream->writeFlag(mAllowStaticShadowOnTerrain);
   stream->writeFlag(mAllowDynamicShadowOnTerrain);
   stream->writeInt(mTinyStaticLevel,8);

   if (mLightPlugin)
   {
      retMask |= mLightPlugin->packUpdate(this, advancedStaticOptionsMask, con, mask, stream);
   }

   return retMask;
}


void TSStatic::unpackUpdate(NetConnection *con, BitStream *stream)
{
   Parent::unpackUpdate(con, stream);

   MatrixF mat;
   Point3F scale;
   mathRead(*stream, &mat);
   mathRead(*stream, &scale);
   setScale(scale);
   setTransform(mat);

   mShapeName = stream->readSTString();

   mAsField = stream->readFlag();
   mAllowTransparency = stream->readFlag();
   bool  AllowCastShadow = stream->readFlag();
   bool  polysoupChanged = (AllowCastShadow != mAllowReceiveShadow);

   mAllowPlayerStep = stream->readFlag();
   if(mAllowPlayerStep)
	   mTypeMask |= PlayerStepObjectType;
   else
	   mTypeMask &= ~PlayerStepObjectType;

   mAllowStaticShadowOnTerrain = stream->readFlag();
   mAllowDynamicShadowOnTerrain = stream->readFlag();

   mTinyStaticLevel	=	stream->readInt(8);


   mAllowReceiveShadow = AllowCastShadow;

   if ( mShapeInstance && polysoupChanged )
		prepCollision();

   if (mLightPlugin)
   {
      mLightPlugin->unpackUpdate(this, con, stream);
   }
}


//--------------------------------------------------------------------------
bool TSStatic::castShadow(const Point3F &start, const Point3F &end, RayInfo* info)
{
	//Ray: 静态阴影效果很差，去掉
	return false;

	if (!mShapeInstance || !mAllowStaticShadowOnTerrain || mTinyStaticLevel)
		return false;


	// Ray: 先用BOX加速碰撞排除
	F32 st,et,fst = 0.0f,fet = 1.0f;
	F32 *bmin = &mObjBox.min.x;
	F32 *bmax = &mObjBox.max.x;
	F32 const *si = &start.x;
	F32 const *ei = &end.x;

	for (int i = 0; i < 3; i++) {
		if (*si < *ei) {
			if (*si > *bmax || *ei < *bmin)
				return false;
			F32 di = *ei - *si;
			st = (*si < *bmin)? (*bmin - *si) / di: 0.0f;
			et = (*ei > *bmax)? (*bmax - *si) / di: 1.0f;
		}
		else {
			if (*ei > *bmax || *si < *bmin)
				return false;
			F32 di = *ei - *si;
			st = (*si > *bmax)? (*bmax - *si) / di: 0.0f;
			et = (*ei < *bmin)? (*bmin - *si) / di: 1.0f;
		}
		if (st > fst) fst = st;
		if (et < fet) fet = et;
		if (fet < fst)
			return false;
		bmin++; bmax++;
		si++; ei++;
	}

	//精细碰撞，巨慢!!
	for(int i=0;i<mMeshDetails.size();i++)
	{
		if (mShapeInstance->castShadow(start, end, info,mMeshDetails[i]))
			return true;
	}

	return false;
}

//----------------------------------------------------------------------------
bool TSStatic::castRay(const Point3F &start, const Point3F &end, RayInfo* info)
{
   if (!mShapeInstance)
      return false;

   RayInfo shortest;
   shortest.t = 1e8;

   info->object = NULL;
   for (U32 i = 0; i < mLOSDetails.size(); i++)
   {
      mShapeInstance->animate(mLOSDetails[i]);

      if (mShapeInstance->castRay(start, end, info, mLOSDetails[i]))
      {
         info->object = this;
         if (info->t < shortest.t)
            shortest = *info;
      }
   }

   if (info->object == this)
   {
      // Copy out the shortest time...
      *info = shortest;
      return true;
   }

   return false;
}

void TSStatic::LightingCompleted()
{
	if(mLightMap)
		mLightMapTex = GFXTexHandle(mLightMap,&GFXDefaultStaticDiffuseProfile,false);
	else
		mLightMapTex.free();

	//Ray:重新创建shader处理
	TSMaterialList* mMaterialList = mShapeInstance->getMaterialList();
	SceneGraphData sgData;
	sgData.setDefaultLights();
	sgData.useLightDir = true;
	sgData.useFog = SceneGraph::renderFog;
	sgData.lightmap = mLightMapTex;
	GFXVertexFlags vf;
	for( U32 i=0; i<mMaterialList->getMaterialCount(); i++ )
	{
		MatInstance *matInst = mMaterialList->getMaterialInst( i );
		if( matInst )
		{
			if(matInst->getMaterial()->enableVertexColor)
			{
				GFXVertexPCNT *tsVertex = NULL;
				vf = (GFXVertexFlags)getGFXVertFlags( tsVertex );
			}
			else
			{
				GFXVertexPNT *tsVertex = NULL;
				vf = (GFXVertexFlags)getGFXVertFlags( tsVertex );
			}

			//因为引擎统一材质shader处理的架构导致无法处理同一材质需要不同shader的情况
			//因此所有静态物件都做了统一的lightmapping处理
			matInst->init( sgData, vf );
			matInst->reInit();
		}
	}
}

//----------------------------------------------------------------------------
bool TSStatic::buildPolyList(AbstractPolyList* polyList, const Box3F &box, const SphereF &)
{
	if (!mShapeInstance)
		return false;

	polyList->setTransform(&mObjToWorld, mObjScale);
	polyList->setObject(this);

	//Ray: mesh投影太耗，还是使用碰撞体投影
//	for(int i=0;i<mMeshDetails.size();i++)
//		mShapeInstance->buildPolyList(polyList,mMeshDetails[i]);
	for(int i=0;i<mCollisionDetails.size();i++)
		mShapeInstance->buildPolyList(polyList,mCollisionDetails[i]);

return true;
}


void TSStatic::buildConvex(const Box3F& box, Convex* convex)
{
   if (mShapeInstance == NULL)
      return;

   // These should really come out of a pool
   mConvexList->collectGarbage();

   Box3F realBox = box;
   mWorldToObj.mul(realBox);
   realBox.min.convolveInverse(mObjScale);
   realBox.max.convolveInverse(mObjScale);

   if (realBox.isOverlapped(getObjBox()) == false)
      return;

   for (U32 i = 0; i < mCollisionDetails.size(); i++)
   {
         // If there is no convex "accelerator" for this detail,
         // there's nothing to collide with.
         TSShape::ConvexHullAccelerator* pAccel =
         mShapeInstance->getShape()->getAccelerator(mCollisionDetails[i]);
         if (!pAccel || !pAccel->numVerts)
            continue;

         // See if this hull exists in the working set already...
         Convex* cc = 0;
         CollisionWorkingList& wl = convex->getWorkingList();
         for (CollisionWorkingList* itr = wl.wLink.mNext; itr != &wl; itr = itr->wLink.mNext) {
            if (itr->mConvex->getType() == TSStaticConvexType &&
                (static_cast<TSStaticConvex*>(itr->mConvex)->pStatic == this &&
                 static_cast<TSStaticConvex*>(itr->mConvex)->hullId  == i)) {
               cc = itr->mConvex;
               break;
            }
         }
         if (cc)
            continue;

         // Create a new convex.
         TSStaticConvex* cp = new TSStaticConvex;
         mConvexList->registerObject(cp);
         convex->addToWorkingList(cp);
         cp->mObject    = this;
         cp->pStatic    = this;
         cp->hullId     = i;
         cp->box        = mObjBox;
         cp->findNodeTransform();
   }
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void TSStaticConvex::findNodeTransform()
{
   S32 dl = pStatic->mCollisionDetails[hullId];

   TSShapeInstance* si = pStatic->mShapeInstance;
   TSShape* shape = si->getShape();

   const TSShape::Detail* detail = &shape->details[dl];
   S32 subs = detail->subShapeNum;
   S32 start = shape->subShapeFirstObject[subs];
   S32 end = start + shape->subShapeNumObjects[subs];

   // Find the first object that contains a mesh for this
   // detail level. There should only be one mesh per
   // collision detail level.
   for (S32 i = start; i < end; i++) {
      const TSShape::Object* obj = &shape->objects[i];
      if (obj->numMeshes && detail->objectDetailNum < obj->numMeshes) {
         nodeTransform = &si->getNodeTransforms()[obj->nodeIndex];
         return;
      }
   }
   return;
}

const MatrixF& TSStaticConvex::getTransform() const
{
   // Multiply on the mesh shape offset
   // tg: Returning this static here is not really a good idea, but
   // all this Convex code needs to be re-organized.
   if (nodeTransform) {
      static MatrixF mat;
      mat.mul(mObject->getTransform(),*nodeTransform);
      return mat;
   }
   return mObject->getTransform();
}

Box3F TSStaticConvex::getBoundingBox() const
{
   return getBoundingBox(mObject->getTransform(), mObject->getScale());
}

Box3F TSStaticConvex::getBoundingBox(const MatrixF& mat, const Point3F& scale) const
{
   Box3F newBox = box;
   newBox.min.convolve(scale);
   newBox.max.convolve(scale);
   mat.mul(newBox);
   return newBox;
}

Point3F TSStaticConvex::support(const VectorF& v) const
{
   TSShape::ConvexHullAccelerator* pAccel =
      pStatic->mShapeInstance->getShape()->getAccelerator(pStatic->mCollisionDetails[hullId]);
   AssertFatal(pAccel != NULL, "Error, no accel!");

   F32 currMaxDP = mDot(pAccel->vertexList[0], v);
   U32 index = 0;
   for (U32 i = 1; i < pAccel->numVerts; i++) {
      F32 dp = mDot(pAccel->vertexList[i], v);
      if (dp > currMaxDP) {
         currMaxDP = dp;
         index = i;
      }
   }

   return pAccel->vertexList[index];
}


void TSStaticConvex::getFeatures(const MatrixF& mat, const VectorF& n, ConvexFeature* cf)
{
   cf->material = 0;
   cf->object = mObject;

   TSShape::ConvexHullAccelerator* pAccel =
      pStatic->mShapeInstance->getShape()->getAccelerator(pStatic->mCollisionDetails[hullId]);
   AssertFatal(pAccel != NULL, "Error, no accel!");

   F32 currMaxDP = mDot(pAccel->vertexList[0], n);
   U32 index = 0;
   U32 i;
   for (i = 1; i < pAccel->numVerts; i++) {
      F32 dp = mDot(pAccel->vertexList[i], n);
      if (dp > currMaxDP) {
         currMaxDP = dp;
         index = i;
      }
   }

   const U8* emitString = pAccel->emitStrings[index];
   U32 currPos = 0;
   U32 numVerts = emitString[currPos++];
   for (i = 0; i < numVerts; i++) {
      cf->mVertexList.increment();
      U32 index = emitString[currPos++];
      mat.mulP(pAccel->vertexList[index], &cf->mVertexList.last());
   }

   U32 numEdges = emitString[currPos++];
   for (i = 0; i < numEdges; i++) {
      U32 ev0 = emitString[currPos++];
      U32 ev1 = emitString[currPos++];
      cf->mEdgeList.increment();
      cf->mEdgeList.last().vertex[0] = ev0;
      cf->mEdgeList.last().vertex[1] = ev1;
   }

   U32 numFaces = emitString[currPos++];
   for (i = 0; i < numFaces; i++) {
      cf->mFaceList.increment();
      U32 plane = emitString[currPos++];
      mat.mulV(pAccel->normalList[plane], &cf->mFaceList.last().normal);
      for (U32 j = 0; j < 3; j++)
         cf->mFaceList.last().vertex[j] = emitString[currPos++];
   }
}


void TSStaticConvex::getPolyList(AbstractPolyList* list)
{
   list->setTransform(&pStatic->getTransform(), pStatic->getScale());
   list->setObject(pStatic);

   pStatic->mShapeInstance->animate(pStatic->mCollisionDetails[hullId]);
   pStatic->mShapeInstance->buildPolyList(list, pStatic->mCollisionDetails[hullId]);
}


//--------------------------------------------------------------------------

SceneObject* TSStaticPolysoupConvex::smCurObject = NULL;

TSStaticPolysoupConvex::TSStaticPolysoupConvex()
:  box( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f ),
   normal( 0.0f, 0.0f, 0.0f, 0.0f ),
   idx( 0 ),
   mesh( NULL )
{
   mType = TSPolysoupConvexType;

   for ( U32 i = 0; i < 4; ++i )
   {
      verts[i].set( 0.0f, 0.0f, 0.0f );
   }
}

Point3F TSStaticPolysoupConvex::support(const VectorF& vec) const
{
   F32 bestDot = mDot( verts[0], vec );

   const Point3F *bestP = &verts[0];
   for(S32 i=1; i<4; i++)
   {
      F32 newD = mDot(verts[i], vec);
      if(newD > bestDot)
      {
         bestDot = newD;
         bestP = &verts[i];
      }
   }

   return *bestP;
}

Box3F TSStaticPolysoupConvex::getBoundingBox() const
{
   Box3F wbox = box;
   wbox.min.convolve( mObject->getScale() );
   wbox.max.convolve( mObject->getScale() );
   mObject->getTransform().mul(wbox);
   return wbox;
}

Box3F TSStaticPolysoupConvex::getBoundingBox(const MatrixF& mat, const Point3F& scale) const
{
   AssertISV(false, "TSStaticPolysoupConvex::getBoundingBox(m,p) - Not implemented. -- XEA");
   return box;
}

void TSStaticPolysoupConvex::getPolyList(AbstractPolyList *list)
{
   // Transform the list into object space and set the pointer to the object
   MatrixF i( mObject->getTransform() );
   Point3F iS( mObject->getScale() );
   list->setTransform(&i, iS);
   list->setObject(mObject);

   // Add all the planes (no, not airplanes! geometry planes)
   S32 base =  list->addPoint(verts[0]);
               list->addPoint(verts[2]);
               list->addPoint(verts[1]);
               list->addPoint(verts[3]);

   list->begin(0, (U32)idx ^ (U32)mesh);
   list->vertex(base + 2);
   list->vertex(base + 1);
   list->vertex(base + 0);
   list->plane(base + 0, base + 1, base + 2);
   list->end();
   list->begin(0, (U32)idx ^ (U32)mesh);
   list->vertex(base + 2);
   list->vertex(base + 1);
   list->vertex(base + 3);
   list->plane(base + 3, base + 1, base + 2);
   list->end();
   list->begin(0, (U32)idx ^ (U32)mesh);
   list->vertex(base + 3);
   list->vertex(base + 1);
   list->vertex(base + 0);
   list->plane(base + 0, base + 1, base + 3);
   list->end();
   list->begin(0, (U32)idx ^ (U32)mesh);
   list->vertex(base + 2);
   list->vertex(base + 3);
   list->vertex(base + 0);
   list->plane(base + 0, base + 3, base + 2);
   list->end();
}

void TSStaticPolysoupConvex::getFeatures(const MatrixF& mat,const VectorF& n, ConvexFeature* cf)
{
   cf->material = 0;
   cf->object = mObject;

   // For a tetrahedron this is pretty easy... first
   // convert everything into world space.
   Point3F tverts[4];
   mat.mulP(verts[0], &tverts[0]);
   mat.mulP(verts[1], &tverts[1]);
   mat.mulP(verts[2], &tverts[2]);
   mat.mulP(verts[3], &tverts[3]);

   // points...
   S32 firstVert = cf->mVertexList.size();
   cf->mVertexList.increment(); cf->mVertexList.last() = tverts[0];
   cf->mVertexList.increment(); cf->mVertexList.last() = tverts[1];
   cf->mVertexList.increment(); cf->mVertexList.last() = tverts[2];
   cf->mVertexList.increment(); cf->mVertexList.last() = tverts[3];

   //    edges...
   cf->mEdgeList.increment();
   cf->mEdgeList.last().vertex[0] = firstVert+0;
   cf->mEdgeList.last().vertex[1] = firstVert+1;

   cf->mEdgeList.increment();
   cf->mEdgeList.last().vertex[0] = firstVert+1;
   cf->mEdgeList.last().vertex[1] = firstVert+2;

   cf->mEdgeList.increment();
   cf->mEdgeList.last().vertex[0] = firstVert+2;
   cf->mEdgeList.last().vertex[1] = firstVert+0;

   cf->mEdgeList.increment();
   cf->mEdgeList.last().vertex[0] = firstVert+3;
   cf->mEdgeList.last().vertex[1] = firstVert+0;

   cf->mEdgeList.increment();
   cf->mEdgeList.last().vertex[0] = firstVert+3;
   cf->mEdgeList.last().vertex[1] = firstVert+1;

   cf->mEdgeList.increment();
   cf->mEdgeList.last().vertex[0] = firstVert+3;
   cf->mEdgeList.last().vertex[1] = firstVert+2;

   //    triangles...
   cf->mFaceList.increment();
   cf->mFaceList.last().normal = PlaneF(tverts[2], tverts[1], tverts[0]);
   cf->mFaceList.last().vertex[0] = firstVert+2;
   cf->mFaceList.last().vertex[1] = firstVert+1;
   cf->mFaceList.last().vertex[2] = firstVert+0;

   cf->mFaceList.increment();
   cf->mFaceList.last().normal = PlaneF(tverts[1], tverts[0], tverts[3]);
   cf->mFaceList.last().vertex[0] = firstVert+1;
   cf->mFaceList.last().vertex[1] = firstVert+0;
   cf->mFaceList.last().vertex[2] = firstVert+3;

   cf->mFaceList.increment();
   cf->mFaceList.last().normal = PlaneF(tverts[2], tverts[1], tverts[3]);
   cf->mFaceList.last().vertex[0] = firstVert+2;
   cf->mFaceList.last().vertex[1] = firstVert+1;
   cf->mFaceList.last().vertex[2] = firstVert+3;

   cf->mFaceList.increment();
   cf->mFaceList.last().normal = PlaneF(tverts[0], tverts[2], tverts[3]);
   cf->mFaceList.last().vertex[0] = firstVert+0;
   cf->mFaceList.last().vertex[1] = firstVert+2;
   cf->mFaceList.last().vertex[2] = firstVert+3;

   // All done!
}

bool TSStatic::haveDynamicShadow()
{
	return mAllowDynamicShadowOnTerrain;
}

void TSStatic::saveDynamicShadowData(Stream *stream)
{
	mLightPlugin->saveDynamicShadowData(this,stream);
}

void TSStatic::setDynamicShadowData(void *p1,void *p2)
{
	mShadowBox.min.set( 10E30f, 10E30f, 10E30f);
	mShadowBox.max.set(-10E30f,-10E30f,-10E30f);

	stShadowParam *pShadowParam = (stShadowParam *)p1;
	if(pShadowParam && pShadowParam->sgShadowPoints.size())
	{
		bShadowBoxDirty = true;
		mShadowBox.min.set(pShadowParam->min);
		mShadowBox.max.set(pShadowParam->max);
	}
	else
	{
		mAllowDynamicShadowOnTerrain = false;
		bShadowBoxDirty = false;
	}

	mLightPlugin->setDynamicShadowData(this,p1,p2);
}

#ifdef USE_MULTITHREAD_ANIMATE
void TSStatic::deleteObject()
{
#ifndef NTJ_SERVER
	if(g_MultThreadWorkMgr->isEnable() && mAmbientAnimation)
	{
		removeObjectFromScene();
		g_MultThreadWorkMgr->addInDeleteingObjectInsList(this);
	}
	else
		Parent::deleteObject();
#else
	Parent::deleteObject();
#endif
}
#endif


ConsoleFunction( staticUseBox, void, 1, 1, "")
{
	if(useDebugBox)
		useDebugBox = false;
	else
		useDebugBox = true;
}


void TSStatic::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->setRenderState(GFXRSColorWriteEnable, GFXCOLORWRITEENABLE_RED|GFXCOLORWRITEENABLE_GREEN|GFXCOLORWRITEENABLE_BLUE|GFXCOLORWRITEENABLE_ALPHA);//
	GFX->endStateBlock(mSetSB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setRenderState(GFXRSColorWriteEnable, 0);//
	GFX->endStateBlock(mClearSB);
}


void TSStatic::releaseStateBlock()
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

void TSStatic::init()
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

void TSStatic::shutdown()
{
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mClearSB);
}

//addGroup("Media");
//addField("shapeName", TypeFilename, Offset(mShapeName, TSStatic));
//endGroup("Media");
//
//addGroup("Lighting");
//addField("receiveSunLight", TypeBool, Offset(receiveSunLight, SceneObject));
//addField("receiveLMLighting", TypeBool, Offset(receiveLMLighting, SceneObject));
//addField("staticShadowOnTerrain", TypeBool, Offset(mAllowStaticShadowOnTerrain, TSStatic));
//addField("dynamicShadowOnTerrain", TypeBool, Offset(mAllowDynamicShadowOnTerrain, TSStatic));
//addField("ReceiveShadow", TypeBool, Offset(mAllowReceiveShadow, TSStatic));
//addField("waterReflection", TypeBool, Offset(mAllowReflection, TSStatic));
//addField("useAdaptiveSelfIllumination", TypeBool, Offset(useAdaptiveSelfIllumination, SceneObject));
//addField("useCustomAmbientLighting", TypeBool, Offset(useCustomAmbientLighting, SceneObject));
////addField("customAmbientSelfIllumination", TypeBool, Offset(customAmbientForSelfIllumination, SceneObject));
//addField("customAmbientLighting", TypeColorF, Offset(customAmbientLighting, SceneObject));
//addField("lightGroupName", TypeString, Offset(lightGroupName, SceneObject));
//endGroup("Lighting");
//
//addGroup("Collision");
//addField("allowTransparency", TypeBool, Offset(mAllowTransparency, TSStatic));
//addField("allowPlayerStep", TypeBool, Offset(mAllowPlayerStep, TSStatic));
//addField("TinyStaticLevel", TypeS32, Offset(mTinyStaticLevel, TSStatic));
//endGroup("Collision");

void TSStatic::OnSerialize( CTinyStream& stream )
{
	__super::OnSerialize( stream );

	stream << receiveSunLight;
	stream << receiveLMLighting;
	stream << mAllowStaticShadowOnTerrain;
	stream << mAllowDynamicShadowOnTerrain;
	stream << mAllowReceiveShadow;
	stream << mAllowReflection;
	stream << useAdaptiveSelfIllumination;
	stream << useCustomAmbientLighting;
	stream << customAmbientLighting;
	stream << customAmbientForSelfIllumination;
	stream << mAsField;
	stream << mAllowTransparency;
	stream << mAllowPlayerStep;
	stream << mTinyStaticLevel;

	stream.writeString( mShapeName );
}

void TSStatic::OnUnserialize( CTinyStream& stream )
{
	__super::OnUnserialize( stream );

	stream >> receiveSunLight;
	stream >> receiveLMLighting;
	stream >> mAllowStaticShadowOnTerrain;
	stream >> mAllowDynamicShadowOnTerrain;
	stream >> mAllowReceiveShadow;
	stream >> mAllowReflection;
	stream >> useAdaptiveSelfIllumination;
	stream >> useCustomAmbientLighting;
	stream >> customAmbientLighting;
	stream >> customAmbientForSelfIllumination;
	if(tagTinySerializerFileHeader::CurrentHeader.version >= 0x10022615)
		stream >> mAsField;
	stream >> mAllowTransparency;
	stream >> mAllowPlayerStep;
	stream >> mTinyStaticLevel;

	char buf[1024];
	stream.readString( buf, 1024 );
	mShapeName = StringTable->insert( buf );
}