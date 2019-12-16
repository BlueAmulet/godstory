//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "core/dnet.h"
#include "sfx/sfxSystem.h"
#include "T3D/gameConnection.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "ts/tsPartInstance.h"
#include "ts/tsShapeInstance.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sceneState.h"
#include "T3D/fx/explosion.h"
#include "T3D/shapeBase.h"
#include "terrain/waterBlock.h"
#include "T3D/debris.h"
#include "terrain/sky.h"
#include "T3D/physicalZone.h"
#include "sceneGraph/detailManager.h"
#include "math/mathUtils.h"
#include "math/mMatrix.h"
#include "math/mRandom.h"
#include "platform/profiler.h"
#include "gfx/gfxCubemap.h"
#include "gfx/gfxDrawUtil.h"
#include "renderInstance/renderInstMgr.h"
#include "collision/earlyOutPolyList.h"
#include "lightingSystem/synapseGaming/vectorProjector.h"
#include "lightingSystem/synapseGaming/sgSceneObjectLightingPlugin.h"
#include "ts/TSShapeRepository.h"
#include "lightingSystem/synapseGaming/sgObjectBasedProjector.h"
#include "Util/aniThread.h"

#include "Gameplay/Data/GraphicsProfile.h"
#include "T3D/fx/cameraFXMgr.h"
#ifdef NTJ_EDITOR
#include "ui/dWorldEditor.h"
#endif

#ifdef NTJ_CLIENT
#include "ui/dGuiMouseGamePlay.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/ClientGameplayState.h"
#include "Effects/EffectPacket.h"
#endif

// <Edit> [3/9/2009 joy] 精简MountedImage
extern bool gEditingMission;
IMPLEMENT_CO_DATABLOCK_V1(ShapeBaseData);


//----------------------------------------------------------------------------
// Timeout for non-looping sounds on a channel
static SimTime sAudioTimeout = 500;
bool ShapeBase::gRenderEnvMaps = true;
U32  ShapeBase::sLastCtrlSimId = 0;

static const char *sDamageStateName[] =
{
   // Index by enum ShapeBase::DamageState
   "Enabled",
   "Disabled",
};


//----------------------------------------------------------------------------

ShapeBaseData::ShapeBaseData()
{
   shadowEnable = false;
   shadowCanMove = false;
   shadowCanAnimate = false;
   shadowSelfShadow = false;
   shadowSize = 128;
   shadowAnimationFrameSkip = 2;
   shadowMaxVisibleDistance = 80.0f;
   shadowProjectionDistance = 10.0f;
   shadowSphereAdjust = 1.0;
   shadowBias = 0.0005f;
   shadowDTSShadow = false;
   shadowIntensity = 1.0f;

   shapesSetId = 0;
   effectPacketId = 0;
   shapeName = "";
   cloakTexName = "";
   mass = 1;
   drag = 0;
   density = 1;
   maxEnergy = 0;
   maxDamage = 1.0f;
   disabledLevel = 1.0f;
   destroyedLevel = 1.0f;
   repairRate = 0.0033f;
   eyeNode = -1;
   cameraNode = -1;
   //damageSequence = -1;
   //hulkSequence = -1;
   cameraMaxDist = 0;
   cameraMinDist = 0.2f;
   cameraDefaultFov = 90.f;
   cameraMinFov = 5.0f;
   cameraMaxFov = 120.f;
   emap = false;
   aiAvoidThis = false;
   isInvincible = false;
   renderWhenDestroyed = true;
   firstPersonOnly = false;
   useEyePoint = false;
   dynamicReflection = false;

   observeThroughObject = false;
   computeCRC = false;
   bInitialized = false;

   inheritEnergyFromMount = false;

   for(U32 j = 0; j < NumHudRenderImages; j++)
   {
      hudImageNameFriendly[j] = 0;
      hudImageNameEnemy[j] = 0;
      hudRenderCenter[j] = false;
      hudRenderModulated[j] = false;
      hudRenderAlways[j] = false;
      hudRenderDistance[j] = false;
      hudRenderName[j] = false;
   }
}

static ShapeBaseData gShapeBaseDataProto;

ShapeBaseData::~ShapeBaseData()
{

}

bool ShapeBaseData::preload(bool server, char errorBuffer[256])
{
   if (!Parent::preload(server, errorBuffer))
      return false;

#ifdef NTJ_EDITOR
   bool shapeError = false;

   // Resolve objects transmitted from server
   if (!server) {
   }

   //
   if (shapeName && shapeName[0]) {
       S32 i;

       // Resolve shapename
       // <Edit> [3/4/2009 joy] 修改了TSShape的数据管理，精简了多余数据
       TSShapeInfo* pInfo = g_TSShapeRepository.InitializeTSShape(shapeName, true, computeCRC,false);
       Resource<TSShape> shape = pInfo ? pInfo->GetShape() : NULL;
       if (!bool(shape)) {
           dSprintf(errorBuffer, 256, "ShapeBaseData: Couldn't load shape \"%s\"",shapeName);
           return false;
       }
       if(!server && !shape->preloadMaterialList() && NetConnection::filesWereDownloaded())
           shapeError = true;

       if(computeCRC)
       {
           Con::printf("Validation required for shape: %s", shapeName);
           if(server)
               mCRC = shape.getCRC();
           else if(mCRC != shape.getCRC())
           {
               dSprintf(errorBuffer, 256, "Shape \"%s\" does not match version on server.",shapeName);
               return false;
           }
       }
       // Resolve details and camera node indexes.
       for (i = 0; i < shape->details.size(); i++)
       {
           char* name = (char*)shape->names[shape->details[i].nameIndex];

           if (dStrstr((const char *)dStrlwr(name), "collision-"))
           {
               collisionDetails.push_back(i);
               collisionBounds.increment();

               shape->computeBounds(collisionDetails.last(), collisionBounds.last());
               shape->getAccelerator(collisionDetails.last());

               if (!shape->bounds.isContained(collisionBounds.last()))
               {
                   Con::warnf("Warning: shape %s collision detail %d (Collision-%d) bounds exceed that of shape.", shapeName, collisionDetails.size() - 1, collisionDetails.last());
                   collisionBounds.last() = shape->bounds;
               }
               else if (collisionBounds.last().isValidBox() == false)
               {
                   Con::errorf("Error: shape %s-collision detail %d (Collision-%d) bounds box invalid!", shapeName, collisionDetails.size() - 1, collisionDetails.last());
                   collisionBounds.last() = shape->bounds;
               }

               // The way LOS works is that it will check to see if there is a LOS detail that matches
               // the the collision detail + 1 + MaxCollisionShapes (this variable name should change in
               // the future). If it can't find a matching LOS it will simply use the collision instead.
               // We check for any "unmatched" LOS's further down
               LOSDetails.increment();

               char buff[128];
               dSprintf(buff, sizeof(buff), "LOS-%d", i + 1 + MaxCollisionShapes);
               U32 los = shape->findDetail(buff);
               if (los == -1)
                   LOSDetails.last() = i;
               else
                   LOSDetails.last() = los;
           }
       }

       // Snag any "unmatched" LOS details
       for (i = 0; i < shape->details.size(); i++)
       {
           char* name = (char*)shape->names[shape->details[i].nameIndex];

           if (dStrstr((const char *)dStrlwr(name), "los-"))
           {
               // See if we already have this LOS
               bool found = false;
               for (U32 j = 0; j < LOSDetails.size(); j++)
               {
                   if (LOSDetails[j] == i)
                   {
                       found = true;
                       break;
                   }
               }

               if (!found)
                   LOSDetails.push_back(i);
           }
       }

       eyeNode = shape->findNode("eye");
       cameraNode = shape->findNode("cam");
       if (cameraNode == -1)
           cameraNode = eyeNode;

       // Resolve mount point node indexes
       //for (i = 0; i < NumMountPoints; i++) {
       //    char fullName[256];
       //    dSprintf(fullName,sizeof(fullName),"Link%02dPoint",i);
       //    mountPointNode[i] = shape->findNode(fullName);
       //}

       //
       F32 w = shape->bounds.len_y() / 2;
       if (cameraMaxDist < w)
           cameraMaxDist = w;
   }

   if(!server)
   {
       /*
       // grab all the hud images
       for(U32 i = 0; i < NumHudRenderImages; i++)
       {
       if(hudImageNameFriendly[i] && hudImageNameFriendly[i][0])
       hudImageFriendly[i] = TextureHandle(hudImageNameFriendly[i], BitmapTexture);

       if(hudImageNameEnemy[i] && hudImageNameEnemy[i][0])
       hudImageEnemy[i] = TextureHandle(hudImageNameEnemy[i], BitmapTexture);
       }
       */
   }

   bInitialized = true;
   return !shapeError; 
#else
    return true;
#endif
}


void ShapeBaseData::initPersistFields()
{
   Parent::initPersistFields();


   addGroup("Shadows");
   addField("shadowEnable", TypeBool, Offset(shadowEnable, ShapeBaseData));
   addField("shadowCanMove", TypeBool, Offset(shadowCanMove, ShapeBaseData));
   addField("shadowCanAnimate", TypeBool, Offset(shadowCanAnimate, ShapeBaseData));
   addField("shadowSelfShadow", TypeBool, Offset(shadowSelfShadow, ShapeBaseData));
   addField("shadowSize", TypeS32, Offset(shadowSize, ShapeBaseData));
   addField("shadowAnimationFrameSkip", TypeS32, Offset(shadowAnimationFrameSkip, ShapeBaseData));
   addField("shadowMaxVisibleDistance", TypeF32, Offset(shadowMaxVisibleDistance, ShapeBaseData));
   addField("shadowProjectionDistance", TypeF32, Offset(shadowProjectionDistance, ShapeBaseData));
   addField("shadowSphereAdjust", TypeF32, Offset(shadowSphereAdjust, ShapeBaseData));
   addField("shadowBias", TypeF32, Offset(shadowBias, ShapeBaseData));
   addField("shadowDTSShadow", TypeBool, Offset(shadowDTSShadow, ShapeBaseData));
   addField("shadowIntensity", TypeF32, Offset(shadowIntensity, ShapeBaseData));
   endGroup("Shadows");


   addGroup("Render");
   addField("shapeFile",      TypeFilename, Offset(shapeName,      ShapeBaseData));
   addField("emap",           TypeBool,       Offset(emap,           ShapeBaseData));
   endGroup("Render");

   addGroup("Destruction", "Parameters related to the destruction effects of this object.");
   addField("renderWhenDestroyed",   TypeBool,  Offset(renderWhenDestroyed,   ShapeBaseData));
   endGroup("Destruction");

   addGroup("Physics");
   addField("mass",           TypeF32,        Offset(mass,           ShapeBaseData));
   addField("drag",           TypeF32,        Offset(drag,           ShapeBaseData));
   addField("density",        TypeF32,        Offset(density,        ShapeBaseData));
   endGroup("Physics");

   addGroup("Damage/Energy");
   addField("maxEnergy",      TypeF32,        Offset(maxEnergy,      ShapeBaseData));
   addField("maxDamage",      TypeF32,        Offset(maxDamage,      ShapeBaseData));
   addField("disabledLevel",  TypeF32,        Offset(disabledLevel,  ShapeBaseData));
   addField("destroyedLevel", TypeF32,        Offset(destroyedLevel, ShapeBaseData));
   addField("repairRate",     TypeF32,        Offset(repairRate,     ShapeBaseData));
   addField("inheritEnergyFromMount", TypeBool, Offset(inheritEnergyFromMount, ShapeBaseData));
   addField("isInvincible",   TypeBool,       Offset(isInvincible,   ShapeBaseData));
   endGroup("Damage/Energy");

   addGroup("Camera");
   addField("cameraMaxDist",  TypeF32,        Offset(cameraMaxDist,  ShapeBaseData));
   addField("cameraMinDist",  TypeF32,        Offset(cameraMinDist,  ShapeBaseData));
   addField("cameraDefaultFov", TypeF32,      Offset(cameraDefaultFov, ShapeBaseData));
   addField("cameraMinFov",   TypeF32,        Offset(cameraMinFov,   ShapeBaseData));
   addField("cameraMaxFov",   TypeF32,        Offset(cameraMaxFov,   ShapeBaseData));
   addField("firstPersonOnly", TypeBool,      Offset(firstPersonOnly, ShapeBaseData));
   addField("useEyePoint",     TypeBool,      Offset(useEyePoint,     ShapeBaseData));
   addField("observeThroughObject", TypeBool, Offset(observeThroughObject, ShapeBaseData));
   endGroup("Camera");

   // This hud code is going to get ripped out soon...
   addGroup("HUD", "@deprecated Likely to be removed soon.");
   addField("hudImageName",         TypeFilename,    Offset(hudImageNameFriendly, ShapeBaseData), NumHudRenderImages);
   addField("hudImageNameFriendly", TypeFilename,    Offset(hudImageNameFriendly, ShapeBaseData), NumHudRenderImages);
   addField("hudImageNameEnemy",    TypeFilename,    Offset(hudImageNameEnemy, ShapeBaseData), NumHudRenderImages);
   addField("hudRenderCenter",      TypeBool,      Offset(hudRenderCenter, ShapeBaseData), NumHudRenderImages);
   addField("hudRenderModulated",   TypeBool,      Offset(hudRenderModulated, ShapeBaseData), NumHudRenderImages);
   addField("hudRenderAlways",      TypeBool,      Offset(hudRenderAlways, ShapeBaseData), NumHudRenderImages);
   addField("hudRenderDistance",    TypeBool,      Offset(hudRenderDistance, ShapeBaseData), NumHudRenderImages);
   addField("hudRenderName",        TypeBool,      Offset(hudRenderName, ShapeBaseData), NumHudRenderImages);
   endGroup("HUD");

   addGroup("Misc");
   addField("aiAvoidThis",      TypeBool,        Offset(aiAvoidThis,    ShapeBaseData));
   addField("computeCRC",     TypeBool,       Offset(computeCRC,     ShapeBaseData));
   addField("dynamicReflection",  TypeBool,       Offset(dynamicReflection,     ShapeBaseData));
   endGroup("Misc");

}

void ShapeBaseData::packData(BitStream* stream)
{
   Parent::packData(stream);

   if(stream->writeFlag(computeCRC))
      stream->write(mCRC);

   
   stream->writeFlag(shadowEnable);
   stream->writeFlag(shadowCanMove);
   stream->writeFlag(shadowCanAnimate);
   stream->writeFlag(shadowSelfShadow);
   stream->write(shadowSize);
   stream->write(shadowAnimationFrameSkip);
   stream->write(shadowMaxVisibleDistance);
   stream->write(shadowProjectionDistance);
   stream->write(shadowSphereAdjust);
   stream->write(shadowBias);
   stream->writeFlag(shadowDTSShadow);
   stream->write(shadowIntensity);


   stream->writeString(shapeName);
   stream->writeString(cloakTexName);
   if(stream->writeFlag(mass != gShapeBaseDataProto.mass))
      stream->write(mass);
   if(stream->writeFlag(drag != gShapeBaseDataProto.drag))
      stream->write(drag);
   if(stream->writeFlag(density != gShapeBaseDataProto.density))
      stream->write(density);
   if(stream->writeFlag(maxEnergy != gShapeBaseDataProto.maxEnergy))
      stream->write(maxEnergy);
   if(stream->writeFlag(cameraMaxDist != gShapeBaseDataProto.cameraMaxDist))
      stream->write(cameraMaxDist);
   if(stream->writeFlag(cameraMinDist != gShapeBaseDataProto.cameraMinDist))
      stream->write(cameraMinDist);
   cameraDefaultFov = mClampF(cameraDefaultFov, cameraMinFov, cameraMaxFov);
   if(stream->writeFlag(cameraDefaultFov != gShapeBaseDataProto.cameraDefaultFov))
      stream->write(cameraDefaultFov);
   if(stream->writeFlag(cameraMinFov != gShapeBaseDataProto.cameraMinFov))
      stream->write(cameraMinFov);
   if(stream->writeFlag(cameraMaxFov != gShapeBaseDataProto.cameraMaxFov))
      stream->write(cameraMaxFov);

   stream->writeFlag(observeThroughObject);

   stream->writeFlag(emap);
   stream->writeFlag(isInvincible);
   stream->writeFlag(renderWhenDestroyed);

   stream->writeFlag(inheritEnergyFromMount);
   stream->writeFlag(firstPersonOnly);
   stream->writeFlag(useEyePoint);
   stream->writeFlag(dynamicReflection);
}

void ShapeBaseData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);
   computeCRC = stream->readFlag();
   if(computeCRC)
      stream->read(&mCRC);


   shadowEnable = stream->readFlag();
   shadowCanMove = stream->readFlag();
   shadowCanAnimate = stream->readFlag();
   shadowSelfShadow = stream->readFlag();
   stream->read(&shadowSize);
   stream->read(&shadowAnimationFrameSkip);
   stream->read(&shadowMaxVisibleDistance);
   stream->read(&shadowProjectionDistance);
   stream->read(&shadowSphereAdjust);
   stream->read(&shadowBias);
   shadowDTSShadow = stream->readFlag();
   stream->read(&shadowIntensity);


   shapeName = stream->readSTString();
   cloakTexName = stream->readSTString();
   if(stream->readFlag())
      stream->read(&mass);
   else
      mass = gShapeBaseDataProto.mass;

   if(stream->readFlag())
      stream->read(&drag);
   else
      drag = gShapeBaseDataProto.drag;

   if(stream->readFlag())
      stream->read(&density);
   else
      density = gShapeBaseDataProto.density;

   if(stream->readFlag())
      stream->read(&maxEnergy);
   else
      maxEnergy = gShapeBaseDataProto.maxEnergy;

   if(stream->readFlag())
      stream->read(&cameraMaxDist);
   else
      cameraMaxDist = gShapeBaseDataProto.cameraMaxDist;

   if(stream->readFlag())
      stream->read(&cameraMinDist);
   else
      cameraMinDist = gShapeBaseDataProto.cameraMinDist;

   if(stream->readFlag())
      stream->read(&cameraDefaultFov);
   else
      cameraDefaultFov = gShapeBaseDataProto.cameraDefaultFov;

   if(stream->readFlag())
      stream->read(&cameraMinFov);
   else
      cameraMinFov = gShapeBaseDataProto.cameraMinFov;

   if(stream->readFlag())
      stream->read(&cameraMaxFov);
   else
      cameraMaxFov = gShapeBaseDataProto.cameraMaxFov;

   observeThroughObject = stream->readFlag();

   emap = stream->readFlag();
   isInvincible = stream->readFlag();
   renderWhenDestroyed = stream->readFlag();

   inheritEnergyFromMount = stream->readFlag();
   firstPersonOnly = stream->readFlag();
   useEyePoint = stream->readFlag();
   dynamicReflection = stream->readFlag();
}

bool ShapeBaseData::initDataBlock()
{
	if(bInitialized)
		return true;

	bool server = false;
#ifdef NTJ_SERVER
	server = true;
#endif

	bool shapeError = false;
	// Resolve objects transmitted from server
	if (!server) {
	}

	//
	const ShapesSet* pSS = g_ShapesSetRepository.GetSet(shapesSetId);
	if (pSS && pSS->shapeName && pSS->shapeName[0]) 
	{
		g_TSShapeRepository.InitializeTSShape(pSS->shapeName,true,computeCRC,true);
	}
	else
	{
		AssertFatal(false, avar("ShapeBaseData::initDataBlock() - datablock %d error!!", dataBlockId));
		return false;
	}

	bInitialized = true;
	return !shapeError;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

Chunker<ShapeBase::CollisionTimeout> sTimeoutChunker;
ShapeBase::CollisionTimeout* ShapeBase::sFreeTimeoutList = 0;


//----------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(ShapeBase);

ShapeBase::ShapeBase()
{
   mTypeMask |= ShapeBaseObjectType;

   mDrag = 0;
   mBuoyancy = 0;
   mWaterCoverage = 0;
   mLiquidType = 0;
   mLiquidHeight = 0.0f;
   //mControllingClient = 0;
   mControllingObject = 0;
   mObjectName = StringTable->insert("");
   mEdgeBlurTextureName = NetStringHandle();

   mGravityMod = 1.0;
   mAppliedForce.set(0, 0, 0);

   mTimeoutList = 0;
   mDataBlock = NULL;
   mTSSahpeInfo = NULL;
   mShapeInstance = 0;
   mDamageState = Enabled;
   mLastRenderFrame = 0;
   mLastRenderDistance = 0;

   mCloaked    = false;
   mCloakLevel = 0.0;

   mMount.object = 0;
   mMount.link = 0;
   mMount.list = 0;
   mMount.node = 0;
   mMount.myNode = 0;

   mHidden = false;

   S32 i;

   for (i = 0; i < MaxTriggerKeys; i++)
      mTrigger[i] = false;

   mIsControlled = false;

   mConvexList = new Convex;
   mCameraFov = 90.f;

   mFadeOut = true;
   mFading = false;
   mFadeVal = 1.0f;
   mFadeTime = 1.0f;
   mFadeElapsedTime = 0.0f;
   mFadeDelay = 0.0f;
   mFlipFadeVal = false;
   mColorExposure = 1.0f;
   mLightTime = 0;

   mDynamicCubemap = NULL;

   mSkinTag = 0;
   mLastZ = 0.0f;
   mShapeEPItem = 0;
   mFluidLight = 0;
   mShapeShifting = false;
#ifdef NTJ_CLIENT
   for (S32 i=0; i<MaxEdgeBlur; ++i)
	   mEdgeBlur[i] = NULL;
   for (S32 i=0; i<EdgeLinkpoints; ++i)
	   mEdgePts[i] = -1;
#endif
}


ShapeBase::~ShapeBase()
{
   delete mConvexList;
   mConvexList = NULL;

   AssertFatal(mMount.link == 0,"ShapeBase::~ShapeBase: An object is still mounted");

   CollisionTimeout* ptr = mTimeoutList;
   while (ptr) {
      CollisionTimeout* cur = ptr;
      ptr = ptr->next;
      cur->next = sFreeTimeoutList;
      sFreeTimeoutList = cur;
   }

   if( mDynamicCubemap )
   {
      delete mDynamicCubemap;
      mDynamicCubemap = NULL;
   }
#ifdef NTJ_CLIENT
   for (S32 i=0; i<MaxEdgeBlur; ++i)
   {
	   SAFE_DELETE(mEdgeBlur[i]);
   }
#endif
}


//----------------------------------------------------------------------------

bool ShapeBase::onAdd()
{
   if(!Parent::onAdd())
      return false;

   if (isClientObject())
   {
      if( mDataBlock && mDataBlock->dynamicReflection )
      {
         SimSet *reflectSet = dynamic_cast<SimSet*>( Sim::findObject( "reflectiveSet" ) );
         AssertFatal(reflectSet, "ShapeBase::onAdd - no reflectiveSet!");
         reflectSet->addObject( this );

         mDynamicCubemap = GFX->createCubemap();
         mDynamicCubemap->initDynamic( 256 );
      }

      activeEPItems();
   }

   return true;
}

void ShapeBase::onRemove()
{
   mConvexList->nukeList();

   unmount();
   Parent::onRemove();

   if(mShapeInstance)
   {
#ifdef USE_MULTITHREAD_ANIMATE
	   if(	g_MultThreadWorkMgr && g_MultThreadWorkMgr->isEnable())
		   g_MultThreadWorkMgr->addInDeleteingShapeInsList(mShapeInstance);
	   else
		   delete mShapeInstance;
#else
		delete mShapeInstance;
#endif
		mShapeInstance = NULL;
   }

}


void ShapeBase::onSceneRemove()
{
   mConvexList->nukeList();
   Parent::onSceneRemove();
}

bool ShapeBase::onNewDataBlock(GameBaseData* dptr)
{
   if (Parent::onNewDataBlock(dptr) == false)
      return false;

   mDataBlock = dynamic_cast<ShapeBaseData*>(dptr);

   if (!mDataBlock)
      return false;

   mDataBlock->parentId = this->GetLoadId();

   // <Edit> [3/12/2009 joy] 初始化DataBlock数据，如载入模型等，相当于原来preload的操作
   // <Edit> [2/27/2010 joy] server不做dataBlock的初始化，不加载多余的数据
#ifndef NTJ_SERVER
   if (!mDataBlock->initDataBlock())
   {
	   Con::errorf("Can't Load Data");
	   return false;
   }
#endif
   
   setMaskBits(DamageMask);
    
   mShapeStack.add(ShapeStack::Stack_Base, mDataBlock->shapesSetId, mDataBlock->effectPacketId);
   mShapeStack.refresh(this);

   updateShapes();

   //if (0 == mShapeInstance)
	  // return false;

   mDamageState = Enabled;
   updateMass();

   mDrag = mDataBlock->drag;
   mCameraFov = mDataBlock->cameraDefaultFov;

   return true;
}

void ShapeBase::__CreateShapeInstance(void)
{
    if(mShapeInstance)
    {
#ifdef USE_MULTITHREAD_ANIMATE
        if(	g_MultThreadWorkMgr && g_MultThreadWorkMgr->isEnable())
            g_MultThreadWorkMgr->addInDeleteingShapeInsList(mShapeInstance);
        else
            delete mShapeInstance;
#else
        delete mShapeInstance;
#endif
    }

    mShapeInstance = new TSShapeInstance(0,isClientObject());
    
    if (mTSSahpeInfo && mTSSahpeInfo->IsInitialized()) 
    {
        //__InitShapeInstance();  
    }
}

void ShapeBase::__InitShapeInstance()
{
    if (0 == mShapeInstance)
        return;

    // Even if loadShape succeeds, there may not actually be
    // a shape assigned to this object.
    if (mTSSahpeInfo && mTSSahpeInfo->IsInitialized()) 
    {
        //假如shape instance未初始化模型,则绑定
        if (!mShapeInstance->getShape())
            mShapeInstance->buildInstanceData(mTSSahpeInfo->GetShape(),isClientObject());

        if (isClientObject())
           mShapeInstance->cloneMaterialList();

        // 更新阴影信息
        sgSceneObjectLightingPlugin* plug = dynamic_cast<sgSceneObjectLightingPlugin*>(mLightPlugin);
        if(plug)
            plug->resetShapeInst(mShapeInstance);
    }
}

void ShapeBase::updateShapes()
{
#ifdef NTJ_SERVER
	StringTableEntry curShape = mTSSahpeInfo ? mTSSahpeInfo->m_TSShapeId : NULL;
	if(mShapeStack.getShapes().shapeName != curShape && mShapeStack.getShapes().shapeName && mShapeStack.getShapes().shapeName[0])
	{

		addEPItems(-1, mShapeStack.getShapes().effectId);

		updateShape();
		setMaskBits(SkinMask);
	}

	NetStringHandle netStr;
	for (S32 i=0; i<ShapeBase::MaxMountedImages; ++i)
	{
		curShape = mMountedImageList[i].shapeName;
		if(mShapeStack.getShapes().subImages[i].shapeName != curShape)
		{
			mountImage(mShapeStack.getShapes().subImages[i].shapeName, i, netStr, mShapeStack.getShapes().subImages[i].mountPoint, mShapeStack.getShapes().subImages[i].mountPointSelf);
			addEPItems(i, mShapeStack.getShapes().subImages[i].effectId);
		}
	}
#else
    if (!mShapeStack.isFinalShapeChanged())
    {
        m_isLoading = __CheckModelLoadings();
        return;
    }

    StringTableEntry curShape = mTSSahpeInfo ? mTSSahpeInfo->m_TSShapeId : NULL;

    if(mShapeStack.getShapes().shapeName != curShape && mShapeStack.getShapes().shapeName && mShapeStack.getShapes().shapeName[0])
    {
        m_loadingModels.shapeName = mShapeStack.getShapes().shapeName;
        m_loadingModels.skinName  = mShapeStack.getShapes().skinName;
        m_loadingModels.effectId  = mShapeStack.getShapes().effectId;

        //加载
        __LoadModel(m_loadingModels.shapeName);

        updateShape();
    }

    for (S32 i = 0; i < ShapeBase::MaxMountedImages; ++i)
    {
        curShape = mMountedImageList[i].shapeName;

        if(mShapeStack.getShapes().subImages[i].shapeName != curShape && mShapeStack.getShapes().subImages[i].shapeName)
        {
            m_loadingModels.subImages[i] = mShapeStack.getShapes().subImages[i];
            __LoadModel(m_loadingModels.subImages[i].shapeName);
        }
    }

    m_isLoading = __CheckModelLoadings();
    mShapeStack.clrFinalShapeChanged();
#endif
}

bool ShapeBase::updateShape()
{
	//创建shape instance
	TSShapeInfo* pInfo = g_TSShapeRepository.GetTSShapeInfo(mShapeStack.getShapes().shapeName);

	if(pInfo)
    {
		mTSSahpeInfo = pInfo;
		//if(isServerObject())
		//	return true;

		// <Edit> [3/16/2010 joy] 统一使用策划填写的box
		mObjBox = mTSSahpeInfo->m_Box;
		resetWorldBox();

#ifdef NTJ_SERVER
		return true;
        if (!mTSSahpeInfo->IsInitialized())
        {
            g_TSShapeRepository.InitializeTSShape(mShapeStack.getShapes().shapeName,true,false,isClientObject());
        }
#endif

		__CreateShapeInstance();

		return true;
    }
	AssertFatal(false, avar("can't find %s in ModelRepository !", mShapeStack.getShapes().shapeName));
	return false;
}

bool ShapeBase::setShapeShifting(bool val)
{
	if(mShapeShifting != val)
	{
		mShapeShifting = val;
#ifdef NTJ_SERVER
		setMaskBits(SkinMask);
#endif
		return true;
	}
	return false;
}

bool ShapeBase::onChangeDataBlock(GameBaseData* dptr)
{
	U32 ssId = mDataBlock->shapesSetId;
	mDataBlock = dynamic_cast<ShapeBaseData*>(dptr);
	if (!mDataBlock || !Parent::onChangeDataBlock(dptr))
		AssertRelease(false, "ShapeBase::onChangeDataBlock");

   // <Edit> [3/12/2009 joy] 初始化DataBlock数据，如载入模型等，相当于原来preload的操作
   if (!mDataBlock->initDataBlock())
   {
	   Con::errorf("Can't Load Data");
	   return false;
   }

   mShapeStack.remove(ShapeStack::Stack_Base, ssId);
   mShapeStack.add(ShapeStack::Stack_Base, mDataBlock->shapesSetId, mDataBlock->effectPacketId);
   mShapeStack.refresh(this);
   updateShapes();

#ifdef NTJ_SERVER
   mShapeEPItem = 0;
#endif
#ifdef NTJ_CLIENT
   addEPItems(-1, 0);
#endif

   //
   updateMass();
   
   mDrag = mDataBlock->drag;
   mCameraFov = mDataBlock->cameraDefaultFov;
   return true;
}

void ShapeBase::onDeleteNotify(SimObject* obj)
{
   if (obj == dynamic_cast<ShapeBase*>(getProcessAfter()))
      clearProcessAfter();
   Parent::onDeleteNotify(obj);
   if (obj == mMount.object)
      unmount();
}

void ShapeBase::onImpact(SceneObject* obj, VectorF vec)
{
   if (!isGhost()) {
      char buff1[256];
      char buff2[32];

      dSprintf(buff1,sizeof(buff1),"%g %g %g",vec.x, vec.y, vec.z);
      dSprintf(buff2,sizeof(buff2),"%g",vec.len());
      Con::executef(mDataBlock,"onImpact",scriptThis(), obj->getIdString(), buff1, buff2);
   }
}

void ShapeBase::onImpact(VectorF vec)
{
   if (!isGhost()) {
      char buff1[256];
      char buff2[32];

      dSprintf(buff1,sizeof(buff1),"%g %g %g",vec.x, vec.y, vec.z);
      dSprintf(buff2,sizeof(buff2),"%g",vec.len());
      Con::executef(mDataBlock,"onImpact",scriptThis(), "0", buff1, buff2);
   }
}


//----------------------------------------------------------------------------

void ShapeBase::processTick(const Move* move)
{ 
   mShapeStack.refresh(this);
   updateShapes();
   hideImage();

   // Call script on trigger state changes
   if (move && mDataBlock && isServerObject()) {
      for (S32 i = 0; i < MaxTriggerKeys; i++) {
         if (move->trigger[i] != mTrigger[i]) {
            mTrigger[i] = move->trigger[i];
            char buf1[20],buf2[20];
            dSprintf(buf1,sizeof(buf1),"%d",i);
            dSprintf(buf2,sizeof(buf2),"%d",(move->trigger[i]?1:0));
            Con::executef(mDataBlock, "onTrigger",scriptThis(),buf1,buf2);
         }
      }
   }
}

void ShapeBase::advanceTime(F32 dt)
{
   // On the client, the images are
   // advanced at framerate.
   for (int i = 0; i < MaxMountedImages; i++)
      if (mMountedImageList[i].shapeName)
         updateImageAnimation(i,dt);

   // Cloaking takes 0.5 seconds
   if (mCloaked && mCloakLevel != 1.0) {
      mCloakLevel += dt * 2;
      if (mCloakLevel >= 1.0)
         mCloakLevel = 1.0;
   } else if (!mCloaked && mCloakLevel != 0.0) {
      mCloakLevel -= dt * 2;
      if (mCloakLevel <= 0.0)
         mCloakLevel = 0.0;
   }
}


//----------------------------------------------------------------------------

//void ShapeBase::setControllingClient(GameConnection* client)
//{
//   mControllingClient = client;
//
//   // piggybacks on the cloak update
//   setMaskBits(CloakMask);
//}

void ShapeBase::setControllingObject(ShapeBase* obj)
{
   if (obj) {
      setProcessTick(false);
      // Even though we don't processTick, we still need to
      // process after the controller in case anyone is mounted
      // on this object.
      processAfter(obj);
   }
   else {
      setProcessTick(true);
      clearProcessAfter();
      // Catch the case of the controlling object actually
      // mounted on this object.
      if (mControllingObject->mMount.object == this)
         mControllingObject->processAfter(this);
   }
   mControllingObject = obj;
}

ShapeBase* ShapeBase::getControlObject()
{
   return 0;
}

void ShapeBase::setControlObject(ShapeBase*)
{
}

bool ShapeBase::isFirstPerson()
{
   // Always first person as far as the server is concerned.
   if (!isGhost())
      return true;

   if (GameConnection* con = getControllingClient())
      return con->getControlObject() == this && con->isFirstPerson();
   return false;
}

// Camera: (in degrees) ------------------------------------------------------
F32 ShapeBase::getCameraFov()
{
   return(mCameraFov);
}

F32 ShapeBase::getDefaultCameraFov()
{
   return(mDataBlock->cameraDefaultFov);
}

bool ShapeBase::isValidCameraFov(F32 fov)
{
   return((fov >= mDataBlock->cameraMinFov) && (fov <= mDataBlock->cameraMaxFov));
}

void ShapeBase::setCameraFov(F32 fov)
{
   mCameraFov = mClampF(fov, mDataBlock->cameraMinFov, mDataBlock->cameraMaxFov);
}

//----------------------------------------------------------------------------
static void scopeCallback(SceneObject* obj, void *conPtr)
{
   NetConnection * ptr = reinterpret_cast<NetConnection*>(conPtr);
   if (obj->isScopeable())
      ptr->objectInScope(obj);
}

void ShapeBase::onCameraScopeQuery(NetConnection *cr, CameraScopeQuery * query)
{
   // update the camera query
   query->camera = this;
   // bool grabEye = true;
   GameConnection * con;
   con = dynamic_cast<GameConnection*>(cr);
   if( con )
   {
      // get the fov from the connection (in deg) 
      F32 fov;
      if (con->getControlCameraFov(&fov))
      {
         query->fov = mDegToRad(fov/2);
         query->sinFov = mSin(query->fov);
         query->cosFov = mCos(query->fov);
      }
   }

   // use eye rather than camera transform (good enough and faster)
   MatrixF eyeTransform;
   getEyeTransform(&eyeTransform);
   eyeTransform.getColumn(3, &query->pos);
   eyeTransform.getColumn(1, &query->orientation);

   // grab the visible distance from the sky
   Sky * sky = gServerSceneGraph->getCurrentSky();
   if(sky)
      query->visibleDistance = sky->getObjSenseDistance();
   else
      query->visibleDistance = 1000.f;

   // First, we are certainly in scope, and whatever we're riding is too...
   cr->objectInScope(this);
   if (isMounted())
      cr->objectInScope(mMount.object);

   int nLayerId = 0;
   if( con )
	   nLayerId = con->GetLayerId();

   if (mSceneManager == NULL)
   {
      // Scope everything...
      gServerContainer.findObjects(0xFFFFFFFF,scopeCallback,cr, nLayerId);
      return;
   }

   // update the scenemanager
   mSceneManager->scopeScene(query->pos, query->visibleDistance, cr);

   // let the (game)connection do some scoping of its own (commandermap...)
   cr->doneScopingScene();
}


//----------------------------------------------------------------------------

static F32 sWaterDensity   = 1;
static F32 sWaterViscosity = 15;
static F32 sWaterCoverage  = 0;
static U32 sWaterType      = 0;
static F32 sWaterHeight    = 0.0f;

static void waterFind(SceneObject* obj,void * key)
{
/*
   ShapeBase* shape = reinterpret_cast<ShapeBase*>(key);
   WaterBlock* wb   = dynamic_cast<WaterBlock*>(obj);
   AssertFatal(wb != NULL, "Error, not a water block!");
   if (wb == NULL) {
      sWaterCoverage = 0;
      return;
   }

   const Box3F& wbox = obj->getWorldBox();
   const Box3F& sbox = shape->getWorldBox();
   sWaterType = 0;
   if (wbox.isOverlapped(sbox)) {
      sWaterType = wb->getLiquidType();
      if (wbox.max.z < sbox.max.z)
         sWaterCoverage = (wbox.max.z - sbox.min.z) / (sbox.max.z - sbox.min.z);
      else
         sWaterCoverage = 1;

      sWaterViscosity = wb->getViscosity();
      sWaterDensity   = wb->getDensity();
      sWaterHeight    = wb->getSurfaceHeight();
   }
*/
}

void physicalZoneFind(SceneObject* obj, void *key)
{
   ShapeBase* shape = reinterpret_cast<ShapeBase*>(key);
   PhysicalZone* pz = dynamic_cast<PhysicalZone*>(obj);
   AssertFatal(pz != NULL, "Error, not a physical zone!");
   if (pz == NULL || pz->testObject(shape) == false) {
      return;
   }

   if (pz->isActive()) {
      shape->mGravityMod   *= pz->getGravityMod();
      shape->mAppliedForce += pz->getForce();
   }
}

void findRouter(SceneObject* obj, void *key)
{
   if (obj->getTypeMask() & WaterObjectType)
      waterFind(obj, key);
   else if (obj->getTypeMask() & PhysicalZoneObjectType)
      physicalZoneFind(obj, key);
   else {
      AssertFatal(false, "Error, must be either water or physical zone here!");
   }
}

void ShapeBase::updateContainer()
{
   // Update container drag and buoyancy properties
   mDrag = mDataBlock->drag;
   mBuoyancy = 0;
   sWaterCoverage = 0;
   mGravityMod = 1.0;
   mAppliedForce.set(0, 0, 0);

   //Ray: waterFind被屏蔽physicalZoneFind又未启用，因此去掉
   //mContainer->findObjects(getWorldBox(), WaterObjectType|PhysicalZoneObjectType,findRouter,this);

   sWaterCoverage = mClampF(sWaterCoverage,0,1);
   mWaterCoverage = sWaterCoverage;
   mLiquidType    = sWaterType;
   mLiquidHeight  = sWaterHeight;
   if (mWaterCoverage >= 0.1f) {
      mDrag = mDataBlock->drag * sWaterViscosity * sWaterCoverage;
      mBuoyancy = (sWaterDensity / mDataBlock->density) * sWaterCoverage;
   }
}


void ShapeBase::setDamageState(DamageState state)
{
   if (mDamageState == state)
      return;

   const char* script = 0;
   const char* lastState = 0;

   if (!isGhost())
      setMaskBits(DamageMask);

   lastState = getDamageStateName();
   switch (state) {
      case Disabled:
         {
            script = "onDisabled";
			onDisabled();
         }
         break;
      case Enabled:
         {
            script = "onEnabled";
            onEnabled();
         }
         break;
   }

   mDamageState = state;
   if (script) {
      // Like to call the scripts after the state has been intialize.
      // This should only end up being called on the server.
      Con::executef(mDataBlock, script,scriptThis(),lastState);
   }
}

bool ShapeBase::setDamageState(const char* state)
{
   for (S32 i = 0; i < NumDamageStates; i++)
      if (!dStricmp(state,sDamageStateName[i])) {
         setDamageState(DamageState(i));
         return true;
      }
   return false;
}

const char* ShapeBase::getDamageStateName()
{
   return sDamageStateName[mDamageState];
}


//----------------------------------------------------------------------------

void ShapeBase::blowUp()
{
}


//----------------------------------------------------------------------------
void ShapeBase::mountObject(ShapeBase* obj,StringTableEntry node,StringTableEntry myNode)
{
//   if (obj->mMount.object == this)
//      return;
   if (obj->mMount.object)
      obj->unmount();

   // Since the object is mounting to us, nothing should be colliding with it for a while
   obj->mConvexList->nukeList();

   obj->mMount.object = this;
   obj->mMount.node = node;
   obj->mMount.myNode = myNode;
   obj->mMount.link = mMount.list;
   mMount.list = obj;
   if (obj != getControllingObject())
      obj->processAfter(this);
   obj->deleteNotify(this);
   obj->setMaskBits(MountedMask);
   obj->onMount(this,node);
}


void ShapeBase::unmountObject(ShapeBase* obj)
{
   if (obj->mMount.object == this) {

      // Find and unlink the object
      for(ShapeBase **ptr = & mMount.list; (*ptr); ptr = &((*ptr)->mMount.link) )
      {
         if(*ptr == obj)
         {
            *ptr = obj->mMount.link;
            break;
         }
      }
      if (obj != getControllingObject())
         obj->clearProcessAfter();
      obj->clearNotify(this);
      obj->mMount.object = 0;
      obj->mMount.link = 0;
      obj->setMaskBits(MountedMask);
      obj->onUnmount(this,obj->mMount.node);

	  if(isServerObject())
	  {
		  unregisterObject();
		  return;
	  }
   }
}

void ShapeBase::unmount()
{
   if (mMount.object)
      mMount.object->unmountObject(this);
}

void ShapeBase::onMount(ShapeBase* obj,StringTableEntry node)
{
   if (!isGhost()) {
      char buff1[32];
      dSprintf(buff1,sizeof(buff1),"%d",node);
      Con::executef(mDataBlock, "onMount",scriptThis(),obj->scriptThis(),buff1);
   }

#ifdef NTJ_CLIENT
   resetShadowSphere();
#endif
}

void ShapeBase::onUnmount(ShapeBase* obj,StringTableEntry node)
{
   if (!isGhost()) {
      char buff1[32];
      dSprintf(buff1,sizeof(buff1),"%d",node);
      Con::executef(mDataBlock, "onUnmount",scriptThis(),obj->scriptThis(),buff1);
   }

#ifdef NTJ_CLIENT
   resetShadowSphere();
#endif
}

ShapeBase* ShapeBase::getRootMount() const
{
	for (ShapeBase* itr = (ShapeBase*)this; itr; itr = itr->mMount.object)
		if (!itr->mMount.object)
			return itr;
	return (ShapeBase*)this;
}

S32 ShapeBase::getMountedObjectCount()
{
   S32 count = 0;
   for (ShapeBase* itr = mMount.list; itr; itr = itr->mMount.link)
      count++;
   return count;
}

S32 ShapeBase::getMountedObjectIndex(ShapeBase* pObj)
{
	S32 count = 0;
	for (ShapeBase* itr = mMount.list; itr; itr = itr->mMount.link, ++count)
		if (itr == pObj)
			return count;
	return -1;
}

S32 ShapeBase::getMountIndex() const
{
	if(!mMount.object)
		return -1;
	S32 count = 0;
	for (ShapeBase* itr = mMount.object->mMount.list; itr; itr = itr->mMount.link, ++count)
		if (itr == this)
			return count;
	return -1;
}

ShapeBase* ShapeBase::getMountedObject(S32 idx)
{
   if (idx >= 0) {
      S32 count = 0;
      for (ShapeBase* itr = mMount.list; itr; itr = itr->mMount.link)
         if (count++ == idx)
            return itr;
   }
   return 0;
}

StringTableEntry ShapeBase::getMountedObjectNode(S32 idx)
{
   if (idx >= 0) {
      S32 count = 0;
      for (ShapeBase* itr = mMount.list; itr; itr = itr->mMount.link)
         if (count++ == idx)
            return itr->mMount.node;
   }
   return NULL;
}

ShapeBase* ShapeBase::getMountNodeObject(StringTableEntry node)
{
   for (ShapeBase* itr = mMount.list; itr; itr = itr->mMount.link)
      if (itr->mMount.node == node)
         return itr;
   return 0;
}

//----------------------------------------------------------------------------

Point3F ShapeBase::getPosition() const
{
	ShapeBase* root = getRootMount();
	if(root)
		return root->Parent::getPosition();
	return Parent::getPosition();
}

void ShapeBase::getEyeTransform(MatrixF* mat)
{
   // Returns eye to world space transform
   S32 eyeNode = mDataBlock->eyeNode;
   if (eyeNode != -1 && mShapeInstance)
      mat->mul(getTransform(), mShapeInstance->getNodeTransforms()[eyeNode]);
   else
      *mat = getTransform();
}

void ShapeBase::getRenderEyeTransform(MatrixF* mat)
{
   // Returns eye to world space transform
   S32 eyeNode = mDataBlock->eyeNode;
   if (eyeNode != -1 && mShapeInstance)
      mat->mul(getRenderTransform(), mShapeInstance->getNodeTransforms()[eyeNode]);
   else
      *mat = getRenderTransform();
}

static Vector<DepthSortList::Poly> mPartition;
static Vector<Point3F> mPartitionVerts;
static unsigned int CameraMask = TerrainObjectType;
static unsigned int CameraFadeMask = StaticRenderedObjectType;
static Point3F lastPos;
void ShapeBase::getCameraTransform(F32* pos,MatrixF* mat)
{
   // Returns camera to world space transform
   // Handles first person / third person camera position

	//Ray: 顶视调试------------------
	//Point3F y,z;
	//MatrixF id(true);
	//id.getColumn(1,&y);
	//id.getColumn(2,&z);
	//z=-z;
	//id.setColumn(1,z);
	//id.setColumn(2,y);

	//Point3F ps;
	//getRenderTransform().getColumn(3,&ps);
	//ps.z+=20;
	//id.setColumn(3,ps);
	//*mat = id;
	//return;
    //--------------------------------

   if (isServerObject() && mShapeInstance)
      mShapeInstance->animateNodeSubtrees(true);

   if (*pos != 0)
   {
      F32 min,max;
      Point3F offset;
      MatrixF eye,rot;
      getCameraParameters(&min,&max,&offset,&rot);
      getRenderEyeTransform(&eye);
      mat->mul(eye,rot);

      // Use the eye transform to orient the camera
      VectorF vp,vec;
      vp.x = vp.z = 0;
	  #pragma message(ENGINE(摄像机和人物距离的设置))
      //vp.y = -(max - min) * *pos;
      vp.y = -1; // vec的长度在0,1之间
      eye.mulV(vp,&vec);

      // Use the camera node's pos.
      Point3F osp,sp;
      if (mDataBlock->cameraNode != -1 && mShapeInstance) 
      {
         mShapeInstance->getNodeTransforms()[mDataBlock->cameraNode].getColumn(3,&osp);

         // Scale the camera position before applying the transform
         const Point3F& scale = getScale();
         osp.convolve( scale );

         getRenderTransform().mulP(osp,&sp);
      }
      else
	  {
         getRenderTransform().getColumn(3,&sp);
		 // <Edit> [3/6/2009 joy] 修改摄像机焦点  随zoom而变化
		 F32 focus = 0;
#ifdef NTJ_EDITOR
		 focus = dWorldEditor::GetCurrentFocus(getWorldBox().len_z());
#endif // NTJ_EDITOR
#ifdef NTJ_CLIENT
		 focus = dGuiMouseGamePlay::GetCurrentFocus(getWorldBox().len_z());
#endif // NTJ_CLIENT
		 
		 sp.z = sp.z + 0.1f + focus;
		 // <Edit> [10/9/2009 joy] 平滑点sp
		 if(getId() != sLastCtrlSimId)
		 {
			 if(sLastCtrlSimId == 0)
			 {
				 lastPos = sp;
				 sLastCtrlSimId = getId();
			 }
			 else
			 {
				 sLastCtrlSimId = getId();
				 Point3F off = sp - lastPos;
				 gCamFXMgr.mCamSlerp.setSlerp(0.8f,off);
			 }
		 }
		 if(lastPos != sp)
			lastPos = sp;
		 sp -= gCamFXMgr.mCamSlerp.getOffset();
	  }

      // Make sure we don't extend the camera into anything solid
      Point3F ep = sp + (*pos * vec) + offset;
      disableCollision();
      if (isMounted())
         getObjectMount()->disableCollision();
	  // <Edit> [3/10/2009 joy] 摄像机只碰撞地形
	  gVectorProjector.buildPartition(sp,vec,0.08f,*pos,CameraMask,mPartition,mPartitionVerts,NULL,false);
	  F32 newPos = *pos;
	  if(mPartitionVerts.size() > 0)
	  {
		  for (S32 i=0; i<mPartitionVerts.size(); i++)
			  if(mPartitionVerts[i].y < newPos)
				  newPos = mPartitionVerts[i].y;
		  if (newPos == 0.0f)
			  eye.getColumn(3,&ep);
		  else
			  ep = sp + offset + (vec * (newPos - 0.01));
	  }

	  // <Edit> [3/10/2009 joy] 两次射线碰撞虚化
	  NetConnection* pConn = getNetConnection();
	  if( !( pConn && pConn->isRobotConnection() ))
	  {
		  sp = getPosition();
		  mContainer->castRayFade(sp, ep, CameraFadeMask);
		  sp.z += getWorldBox().len_z();
		  mContainer->castRayFade(sp, ep, CameraFadeMask);
	  } 

      mat->setColumn(3,ep);
      if (isMounted())
         getObjectMount()->enableCollision();
      enableCollision();
   }
   else
   {
      getRenderEyeTransform(mat);
   }
   mat->mul(gCamFXMgr.getTrans());
}

// void ShapeBase::getCameraTransform(F32* pos,MatrixF* mat)
// {
//    // Returns camera to world space transform
//    // Handles first person / third person camera position

//    if (isServerObject() && mShapeInstance)
//       mShapeInstance->animateNodeSubtrees(true);

//    if (*pos != 0) {
//       F32 min,max;
//       Point3F offset;
//       MatrixF eye,rot;
//       getCameraParameters(&min,&max,&offset,&rot);
//       getRenderEyeTransform(&eye);
//       mat->mul(eye,rot);

//       // Use the eye transform to orient the camera
//       VectorF vp,vec;
//       vp.x = vp.z = 0;
//       vp.y = -(max - min) * *pos;
//       eye.mulV(vp,&vec);

//       // Use the camera node's pos.
//       Point3F osp,sp;
//       if (mDataBlock->cameraNode != -1) {
//          mShapeInstance->getNodeTransforms()[mDataBlock->cameraNode].getColumn(3,&osp);
//          getRenderTransform().mulP(osp,&sp);
//       }
//       else
//          getRenderTransform().getColumn(3,&sp);

//       // Make sure we don't extend the camera into anything solid
//       Point3F ep = sp + vec;
//       ep += offset;
//       disableCollision();
//       if (isMounted())
//          getObjectMount()->disableCollision();
//       RayInfo collision;
//       if (mContainer->castRay(sp,ep,(0xFFFFFFFF & ~(WaterObjectType|ForceFieldObjectType|GameBaseObjectType|DefaultObjectType)),&collision)) {
//          *pos = collision.t *= 0.9;
//          if (*pos == 0)
//             eye.getColumn(3,&ep);
//          else
//             ep = sp + vec * *pos;
//       }
//       mat->setColumn(3,ep);
//       if (isMounted())
//          getObjectMount()->enableCollision();
//       enableCollision();
//    }
//    else
//    {
//       getRenderEyeTransform(mat);
//    }
// }


// void ShapeBase::getRenderCameraTransform(F32* pos,MatrixF* mat)
// {
//    // Returns camera to world space transform
//    // Handles first person / third person camera position

//    if (isServerObject() && mShapeInstance)
//       mShapeInstance->animateNodeSubtrees(true);

//    if (*pos != 0) {
//       F32 min,max;
//       Point3F offset;
//       MatrixF eye,rot;
//       getCameraParameters(&min,&max,&offset,&rot);
//       getRenderEyeTransform(&eye);
//       mat->mul(eye,rot);

//       // Use the eye transform to orient the camera
//       VectorF vp,vec;
//       vp.x = vp.z = 0;
//       vp.y = -(max - min) * *pos;
//       eye.mulV(vp,&vec);

//       // Use the camera node's pos.
//       Point3F osp,sp;
//       if (mDataBlock->cameraNode != -1) {
//          mShapeInstance->getNodeTransforms()[mDataBlock->cameraNode].getColumn(3,&osp);
//          getRenderTransform().mulP(osp,&sp);
//       }
//       else
//          getRenderTransform().getColumn(3,&sp);

//       // Make sure we don't extend the camera into anything solid
//       Point3F ep = sp + vec;
//       ep += offset;
//       disableCollision();
//       if (isMounted())
//          getObjectMount()->disableCollision();
//       RayInfo collision;
//       if (mContainer->castRay(sp,ep,(0xFFFFFFFF & ~(WaterObjectType|ForceFieldObjectType|GameBaseObjectType|DefaultObjectType)),&collision)) {
//          *pos = collision.t *= 0.9;
//          if (*pos == 0)
//             eye.getColumn(3,&ep);
//          else
//             ep = sp + vec * *pos;
//       }
//       mat->setColumn(3,ep);
//       if (isMounted())
//          getObjectMount()->enableCollision();
//       enableCollision();
//    }
//    else
//    {
//       getRenderEyeTransform(mat);
//    }
// }

void ShapeBase::getCameraParameters(F32 *min,F32* max,Point3F* off,MatrixF* rot)
{
   *min = mDataBlock->cameraMinDist;
   *max = mDataBlock->cameraMaxDist;
   off->set(0,0,0);
   rot->identity();
}


//----------------------------------------------------------------------------

bool ShapeBase::onlyFirstPerson() const
{
   return mDataBlock->firstPersonOnly;
}

bool ShapeBase::useObjsEyePoint() const
{
   return mDataBlock->useEyePoint;
}


//----------------------------------------------------------------------------
void ShapeBase::setVelocity(const VectorF&)
{
}

void ShapeBase::applyImpulse(const Point3F&,const VectorF&)
{
}


//----------------------------------------------------------------------------

TSShape const* ShapeBase::getShape()
{
   return mShapeInstance? mShapeInstance->getShape(): 0;
}


void ShapeBase::calcClassRenderData()
{
   // This is truly lame, but I didn't want to duplicate the whole preprender logic
   //  in the player as well as the renderImage logic.  DMM
}

bool ShapeBase::prepRenderImage(SceneState* state, const U32 stateKey,
                                const U32 startZone, const bool modifyBaseState)
{
	if(gEditingMission && isHidden() || getNetConnection()->isRobotConnection() )
		return false;

   AssertFatal(modifyBaseState == false, "Error, should never be called with this parameter set");
   AssertFatal(startZone == 0xFFFFFFFF, "Error, startZone should indicate -1");

   PROFILE_SCOPE(ShapeBase_prepRenderImage);

   if (isLastState(state, stateKey))
   {
      return false;
   }
   setLastState(state, stateKey);

   // Select detail levels on mounted items
   // but... always draw the control object's mounted images
   // in high detail (I can't believe I'm commenting this hack :)
   bool self = false;
   F32 saveError = TSShapeInstance::smScreenError;
   GameConnection *con = GameConnection::getConnectionToServer();
   bool fogExemption = false;
   ShapeBase *co = NULL;
   if(con && ( (co = dynamic_cast<ShapeBase*>(con->getControlObject())) != NULL) )
   {
      if(co == this || co->getObjectMount() == this)
      {
         TSShapeInstance::smScreenError = 0.001f;
         fogExemption = true;
		 self = true;
      }
   }

#ifdef NTJ_EDITOR
   if(!self && !WorldEditor::mRenderNPC)
   {
	   return false;
   }
#endif 
 

   bool bRenderShadow = !gClientSceneGraph->isReflectPass() && !isMounted();

   if(!isOnlyRenderShadow())
   {
	   if(mShapeInstance && mShapeInstance->getShape() && (gClientSceneGraph->isReflectPass() || state->isObjectRendered(this)) )
	   {
		   // get shape detail and fog information...we might not even need to be drawn
		   Point3F cameraOffset;
		   getRenderTransform().getColumn(3,&cameraOffset);
		   cameraOffset -= state->getCameraPosition();
		   F32 dist = cameraOffset.len();
		   if (dist < 0.01f)
			   dist = 0.01f;
		   F32 fogAmount = state->getHazeAndFog(dist,cameraOffset.z);
		   F32 invScale = (1.0f/getMax(getMax(mObjScale.x,mObjScale.y),mObjScale.z));

		   if (mShapeInstance && mShapeInstance->getShape())
			   DetailManager::selectPotentialDetails(mShapeInstance,dist,invScale);

		   if(mShapeInstance && !gClientSceneGraph->isReflectPass())
		   {
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

		   if ((fogAmount>0.99f && fogExemption == false) ||
			   (mShapeInstance && mShapeInstance->getCurrentDetail()<0) ||
			   (!mShapeInstance && !gShowBoundingBox)) 
		   {
			   // no, don't draw anything
			   return false;
		   }
		   PrepRenderPushFade();
		   TSMesh::setFluidLightParam(mFluidLight);

		   gClientSceneGraph->addInCameraObject(this);

		   for (U32 i = 0; i < MaxMountedImages; i++)
		   {
			   MountedImage& image = mMountedImageList[i];
			   if (image.shapeName && image.shapeInstance && image.shapeInstance->getShape() && image.isRender)
			   {
				   DetailManager::selectPotentialDetails(image.shapeInstance,dist,invScale);

				   // <Edit> [3/4/2009 joy] hasSolid??
				   if (mCloakLevel == 0.0f/* && image.shapeInstance->hasSolid()*/ && mFadeVal == 1.0f)
				   {
					   prepBatchRender( state, i );

					   // render debug
					   if( gShowBoundingBox )
					   {
						   RenderInst *ri = gRenderInstManager.allocInst();
						   ri->obj = this;
						   ri->state = state;
						   ri->type = RenderInstManager::RIT_Object;
						   ri->mountedObjIdx = i;
						   gRenderInstManager.addInst( ri );
					   }
				   }
			   }
		   }

		   TSShapeInstance::smScreenError = saveError;

		   // render debug
		   if( gShowBoundingBox )
		   {
			   RenderInst *ri = gRenderInstManager.allocInst();
			   ri->obj = this;
			   ri->state = state;
			   ri->type = RenderInstManager::RIT_Object;
			   ri->mountedObjIdx = -1;
			   gRenderInstManager.addInst( ri );
		   }

		   prepBatchRender( state, -1 );

		   // calcClassRenderData();

		   mLastRenderFrame = SceneGraph::smFrameKey;
#ifdef NTJ_CLIENT
		   for (S32 j=0; j<MaxEdgeBlur; ++j)
		   {
			   if (mEdgeBlur[j] && mEdgePts[j*2] != -1 && mEdgePts[j*2+1] != -1)
			   {
				   static MatrixF mat1, mat2;
				   static ColorF color(1,1,1);

				   Point3F point;
				   getWorldBox().getCenter(&point);

				   if(!isEdgeBlurPause())
				   {
					   if(mMountedImageList[GameObject::Slot_LeftHand].shapeInstance || mMountedImageList[GameObject::Slot_RightHand].shapeInstance)
					   {
						   U32 imageSlot = (j < (MaxEdgeBlur>>1)) ? GameObject::Slot_LeftHand : GameObject::Slot_RightHand;
						   getRenderImageTransform(imageSlot, mEdgePts[j*2], &mat1);
						   getRenderImageTransform(imageSlot, mEdgePts[j*2+1], &mat2);
					   }
					   else
					   {
						   getRenderNodeTransform(mEdgePts[j*2], &mat1);
						   getRenderNodeTransform(mEdgePts[j*2+1], &mat2);
					   }
					   mEdgeBlur[j]->SetOpenBlurState(true);
					   mEdgeBlur[j]->FrameMove();
					   mEdgeBlur[j]->Add(mat1.getPosition(), mat2.getPosition(), color);
					   mEdgeBlur[j]->Render(point);
				   }
				   else
				   {
					   mEdgeBlur[j]->SetOpenBlurState(false);
					   mEdgeBlur[j]->FrameMove();
					   mEdgeBlur[j]->Render(point);
				   }
			   }
		   }
#endif
		   PrepRenderPopFade();
		   TSMesh::setFluidLightParam(false);
	   }
   }

#ifdef NTJ_EDITOR
   if(!dWorldEditor::m_enableRenderShadows)
	   bRenderShadow = false;
#endif

   if(bRenderShadow && mShapeInstance && mShapeInstance->getShape())
   {
	   if(mDataBlock->shadowEnable && g_GraphicsProfile.mShadowLevel)
	   {
		   renderShadow( state );

#ifdef DEBUG
		   gClientSceneGraph->addInCameraShadow(this);
#endif
	   }
   }
   return false;
}

//----------------------------------------------------------------------------
// prepBatchRender
//----------------------------------------------------------------------------
void ShapeBase::prepBatchRender(SceneState* state, S32 mountedImageIndex )
{
   // CHANGES IN HERE SHOULD BE DUPLICATED IN TSSTATIC!

   RectI viewport = GFX->getViewport();
   MatrixF proj = GFX->getProjectionMatrix();
   GFX->pushWorldMatrix();

   MatrixF world = GFX->getWorldMatrix();
   TSMesh::setCamTrans( world );
   TSMesh::setSceneState( state );
   TSMesh::setCubemap( mDynamicCubemap );
   TSMesh::setObject( this );
   TSMesh::setOverrideColorExposure(mColorExposure);

   if(!gClientSceneGraph->isReflectPass())
		gClientSceneGraph->getLightManager()->setupLights(this);

   if( mountedImageIndex != -1 )
   {
      MountedImage& image = mMountedImageList[mountedImageIndex];

      //对于绑定的装备需要等待主模型加载完毕后显示
      if( image.shapeName && image.shapeInstance && image.shapeInstance->getShape() &&
          mShapeInstance && mShapeInstance->getShape() && 
          DetailManager::selectCurrentDetail(image.shapeInstance))
      {
         MatrixF mat;
         getRenderImageTransform(mountedImageIndex, &mat);
         GFX->setWorldMatrix( mat );

		 if(image.shapeInstance && !gClientSceneGraph->isReflectPass())
		 {
#if (defined(USE_MULTITHREAD_ANIMATE) && !defined(NTJ_SERVER))
			 if(g_MultThreadWorkMgr->isEnable() && image.shapeInstance->isAnimationInited())
				 g_MultThreadWorkMgr->addInAnimateShapeInsList(image.shapeInstance);
			 else
			 {
				 image.shapeInstance->setAnimationInited(true);
				 image.shapeInstance->animate();
			 }
#else
			 image.shapeInstance->animate();
#endif
		 }
	 
         image.shapeInstance->render();
      }

   }
   else
   {
      MatrixF mat = getRenderTransform();
      mat.scale( mObjScale );
      GFX->setWorldMatrix( mat );

	  MatrixF prj = GFX->getProjectionMatrix();
	  MatrixF wld = world;
	  Point3F pos;
	  pos = mWorldBox.getClosestPoint(state->getCameraPosition());
	  Point4F pos4(pos.x,pos.y,pos.z,1.0f);
	  prj.mul(wld);
	  prj.mul(pos4);
	  mLastZ = pos4.z/pos4.w;

      if (mShapeInstance)
			mShapeInstance->render();
   }
   
   if(!gClientSceneGraph->isReflectPass())
		gClientSceneGraph->getLightManager()->resetLights();
   

   GFX->popWorldMatrix();
   GFX->setProjectionMatrix( proj );
   GFX->setViewport( viewport );

   TSMesh::setOverrideColorExposure(1.0f);
}

//----------------------------------------------------------------------------
// renderObject - render debug data
//----------------------------------------------------------------------------
void ShapeBase::renderObject( SceneState *state, RenderInst *ri )
{
   RectI viewport = GFX->getViewport();
   MatrixF proj = GFX->getProjectionMatrix();

   // Debugging Bounding Box
   if( !mShapeInstance || gShowBoundingBox )
   {
      GFX->pushWorldMatrix();
      GFX->multWorld( getRenderTransform() );

      Point3F box1, box2;
      box1 = (mObjBox.max - mObjBox.min) * 0.5;
      box2 = (mObjBox.min + mObjBox.max) * 0.5;
      wireCube( box1, box2 );

      GFX->popWorldMatrix();
   }
}

void ShapeBase::renderShadow(SceneState *state)
{
   if (mLightPlugin)
	   mLightPlugin->renderShadow(this, state);
}

void ShapeBase::setShadowVariables(sgShadowProjector *pShadow )
{
	if(mDataBlock)
	{
		pShadow->sgEnable = mDataBlock->shadowEnable;
		pShadow->sgCanMove = mDataBlock->shadowCanMove;
		pShadow->sgCanRTT = mDataBlock->shadowCanAnimate;
		pShadow->sgFrameSkip = mDataBlock->shadowAnimationFrameSkip;
		pShadow->sgMaxVisibleDistance = mDataBlock->shadowMaxVisibleDistance;
		pShadow->sgBias = mDataBlock->shadowBias;
		pShadow->sgDTSShadow = mDataBlock->shadowDTSShadow;
		pShadow->sgIntensity = mDataBlock->shadowIntensity;

		pShadow->sgDirtySync(pShadow->sgCanSelfShadow, mDataBlock->shadowSelfShadow);
		pShadow->sgDirtySync(pShadow->sgRequestedShadowSize, mDataBlock->shadowSize);
		pShadow->sgDirtySync(pShadow->sgProjectionDistance, mDataBlock->shadowProjectionDistance);
		pShadow->sgDirtySync(pShadow->sgSphereAdjust, mDataBlock->shadowSphereAdjust);
	}
}

void ShapeBase::resetShadowSphere()
{
	//在这里重新调整mShadowSphere，要包含武器装备等，防止阴影被截断
	mShadowSphere = getWorldSphere();
}

const SphereF& ShapeBase::getShadowSphere() const   
{ 
	return mShadowSphere; 
}

void ShapeBase::renderShadowBuff(sgShadowProjector *pShadow ,MatrixF &proj,MatrixF &lightspace,MatrixF &sgWorldToLightY,MatrixF &shadowscale)
{
	if(mFading || !mShapeInstance || !mShapeInstance->getShape())
		return;

	MatrixF newmat;
	newmat.mul(proj, lightspace);
	pShadow->sgRenderShape(mShapeInstance, newmat, 0, newmat, -1);

	for(U32 i=0; i<ShapeBase::MaxMountedImages; i++)
	{
		if(!isRenderImage(i))
			continue;

		TSShapeInstance *mountinst = getImageShapeInstance(i);
		if(!mountinst)
			continue;

		MatrixF shapeworld;
		getRenderImageTransform(i, &shapeworld);
		shapeworld.scale(getScale());

		lightspace.mul(sgWorldToLightY, shapeworld);
	     lightspace = shadowscale * lightspace;

		newmat.mul(proj, lightspace);
		pShadow->sgRenderShape(mountinst, newmat, 0, newmat, -1);
	}

	for (ShapeBase* itr = mMount.list; itr; itr = itr->mMount.link)
	{
		MatrixF shapeworld;
		shapeworld = itr->getRenderTransform();
		shapeworld.scale(getScale());

		lightspace.mul(sgWorldToLightY, shapeworld);
		lightspace = shadowscale * lightspace;

		newmat.mul(proj, lightspace);
		itr->renderShadowBuff(pShadow ,proj,lightspace,sgWorldToLightY,shadowscale);
	}
}

bool ShapeBase::shadowTest(SceneState *state)
{
	return false; //Ray: 对于玩家怪物等阴影不会提前与主体显示

	if(!mShapeInstance || !mShapeInstance->getShape())
		return false;

	if(g_GraphicsProfile.mShadowLevel<2)
		return false;

	if(gClientSceneGraph->isReflectPass())
		return false;

	if(!state)
		return false;

	const Frustum& frustum = state->getFrustum();
	if(frustum.sphereInFrustum(getRenderPosition(),mShapeInstance->getShape()->radius*1.5))
		return true;

	return false;
}
//----------------------------------------------------------------------------
// This is a callback for objects that have reflections and are added to
// the "reflectiveSet" SimSet.
//----------------------------------------------------------------------------
void ShapeBase::updateReflection()
{
   if( mDynamicCubemap )
   {
      mDynamicCubemap->updateDynamic( getPosition() );
   }
}


//----------------------------------------------------------------------------

static ColorF cubeColors[8] = {
   ColorF(0, 0, 0), ColorF(1, 0, 0), ColorF(0, 1, 0), ColorF(0, 0, 1),
   ColorF(1, 1, 0), ColorF(1, 0, 1), ColorF(0, 1, 1), ColorF(1, 1, 1)
};

static Point3F cubePoints[8] = {
   Point3F(-1, -1, -1), Point3F(-1, -1,  1), Point3F(-1,  1, -1), Point3F(-1,  1,  1),
   Point3F( 1, -1, -1), Point3F( 1, -1,  1), Point3F( 1,  1, -1), Point3F( 1,  1,  1)
};

static U32 cubeFaces[6][4] = {
   { 0, 2, 6, 4 }, { 0, 2, 3, 1 }, { 0, 1, 5, 4 },
   { 3, 2, 6, 7 }, { 7, 6, 4, 5 }, { 3, 7, 5, 1 }
};

void ShapeBase::wireCube(const Point3F& size, const Point3F& pos)
{
   GFX->getDrawUtil()->drawWireCube( size, pos, ColorI( 255, 255, 255 ) );

/*
   glDisable(GL_CULL_FACE);

   for(int i = 0; i < 6; i++) {
      glBegin(GL_LINE_LOOP);
      for(int vert = 0; vert < 4; vert++) {
         int idx = cubeFaces[i][vert];
         glVertex3f(cubePoints[idx].x * size.x + pos.x, cubePoints[idx].y * size.y + pos.y, cubePoints[idx].z * size.z + pos.z);
      }
      glEnd();
   }
*/
}


//----------------------------------------------------------------------------

bool ShapeBase::castRay(const Point3F &start, const Point3F &end, RayInfo* info)
{
   if (mShapeInstance) 
   {
      RayInfo shortest;
      shortest.t = 1e8;

      info->object = NULL;
      for (U32 i = 0; i < mDataBlock->LOSDetails.size(); i++)
      {
         mShapeInstance->animate(mDataBlock->LOSDetails[i]);
         if (mShapeInstance->castRay(start, end, info, mDataBlock->LOSDetails[i]))
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
   }

   return false;
}


//----------------------------------------------------------------------------

bool ShapeBase::buildPolyList(AbstractPolyList* polyList, const Box3F &, const SphereF &)
{
   if (mShapeInstance) {
      bool ret = false;

      polyList->setTransform(&mObjToWorld, mObjScale);
      polyList->setObject(this);

      for (U32 i = 0; i < mDataBlock->collisionDetails.size(); i++)
      {
            mShapeInstance->buildPolyList(polyList,mDataBlock->collisionDetails[i]);
            ret = true;
         }

      return ret;
   }

   return false;
}


void ShapeBase::buildConvex(const Box3F& box, Convex* convex)
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

   for (U32 i = 0; i < mDataBlock->collisionDetails.size(); i++)
   {
         Box3F newbox = mDataBlock->collisionBounds[i];
         newbox.min.convolve(mObjScale);
         newbox.max.convolve(mObjScale);
         mObjToWorld.mul(newbox);
         if (box.isOverlapped(newbox) == false)
            continue;

         // See if this hull exists in the working set already...
         Convex* cc = 0;
         CollisionWorkingList& wl = convex->getWorkingList();
         for (CollisionWorkingList* itr = wl.wLink.mNext; itr != &wl; itr = itr->wLink.mNext) {
            if (itr->mConvex->getType() == ShapeBaseConvexType &&
                (static_cast<ShapeBaseConvex*>(itr->mConvex)->pShapeBase == this &&
                 static_cast<ShapeBaseConvex*>(itr->mConvex)->hullId     == i)) {
               cc = itr->mConvex;
               break;
            }
         }
         if (cc)
            continue;

         // Create a new convex.
         ShapeBaseConvex* cp = new ShapeBaseConvex;
         mConvexList->registerObject(cp);
         convex->addToWorkingList(cp);
         cp->mObject    = this;
         cp->pShapeBase = this;
         cp->hullId     = i;
         cp->box        = mDataBlock->collisionBounds[i];
         cp->transform = 0;
         cp->findNodeTransform();
   }
}


//----------------------------------------------------------------------------

void ShapeBase::queueCollision(ShapeBase* obj, const VectorF& vec)
{
   // Add object to list of collisions.
   SimTime time = Sim::getCurrentTime();
   S32 num = obj->getId();

   CollisionTimeout** adr = &mTimeoutList;
   CollisionTimeout* ptr = mTimeoutList;
   while (ptr) {
      if (ptr->objectNumber == num) {
         if (ptr->expireTime < time) {
            ptr->expireTime = time + CollisionTimeoutValue;
            ptr->object = obj;
            ptr->vector = vec;
         }
         return;
      }
      // Recover expired entries
      if (ptr->expireTime < time) {
         CollisionTimeout* cur = ptr;
         *adr = ptr->next;
         ptr = ptr->next;
         cur->next = sFreeTimeoutList;
         sFreeTimeoutList = cur;
      }
      else {
         adr = &ptr->next;
         ptr = ptr->next;
      }
   }

   // New entry for the object
   if (sFreeTimeoutList != NULL)
   {
      ptr = sFreeTimeoutList;
      sFreeTimeoutList = ptr->next;
      ptr->next = NULL;
   }
   else
   {
      ptr = sTimeoutChunker.alloc();
   }

   ptr->object = obj;
   ptr->objectNumber = obj->getId();
   ptr->vector = vec;
   ptr->expireTime = time + CollisionTimeoutValue;
   ptr->next = mTimeoutList;

   mTimeoutList = ptr;
}

void ShapeBase::notifyCollision()
{
   // Notify all the objects that were just stamped during the queueing
   // process.
   SimTime expireTime = Sim::getCurrentTime() + CollisionTimeoutValue;
   for (CollisionTimeout* ptr = mTimeoutList; ptr; ptr = ptr->next)
   {
      if (ptr->expireTime == expireTime && ptr->object)
      {
         SimObjectPtr<ShapeBase> safePtr(ptr->object);
         SimObjectPtr<ShapeBase> safeThis(this);
         onCollision(ptr->object,ptr->vector);
         ptr->object = 0;

         if(!bool(safeThis))
            return;

         if(bool(safePtr))
            safePtr->onCollision(this,ptr->vector);

         if(!bool(safeThis))
            return;
      }
   }
}

void ShapeBase::onCollision(ShapeBase* object,VectorF vec)
{
   if (!isGhost())  {
      char buff1[256];
      char buff2[32];

      dSprintf(buff1,sizeof(buff1),"%g %g %g",vec.x, vec.y, vec.z);
      dSprintf(buff2,sizeof(buff2),"%g",vec.len());
      Con::executef(mDataBlock, "onCollision",scriptThis(),object->scriptThis(), buff1, buff2);
   }
}

//--------------------------------------------------------------------------
bool ShapeBase::pointInWater( Point3F &point )
{
   SimpleQueryList sql;
   if (isServerObject())
      gServerSceneGraph->getWaterObjectList(sql);
   else
      gClientSceneGraph->getWaterObjectList(sql);

   for (U32 i = 0; i < sql.mList.size(); i++)
   {
      WaterBlock* pBlock = dynamic_cast<WaterBlock*>(sql.mList[i]);
      if (pBlock && pBlock->isUnderwater( point ))
         return true;
   }

   return false;
}

//----------------------------------------------------------------------------

void ShapeBase::writePacketData(GameConnection *connection, BitStream *stream)
{
   Parent::writePacketData(connection, stream);
}

void ShapeBase::readPacketData(GameConnection *connection, BitStream *stream)
{
   Parent::readPacketData(connection, stream);
}

F32 ShapeBase::getUpdatePriority(CameraScopeQuery *camInfo, U32 updateMask, S32 updateSkips)
{
   // If it's the scope object, must be high priority
   if (camInfo->camera == this) {
      // Most priorities are between 0 and 1, so this
      // should be something larger.
      return 10.0f;
   }
   if (camInfo->camera && (camInfo->camera->getType() & ShapeBaseObjectType))
   {
      // see if the camera is mounted to this...
      // if it is, this should have a high priority
      if(((ShapeBase *) camInfo->camera)->getObjectMount() == this)
         return 10.0f;
   }
   return Parent::getUpdatePriority(camInfo, updateMask, updateSkips);
}

U64 ShapeBase::packUpdate(NetConnection *con, U64 mask, BitStream *stream)
{
   U64 retMask = Parent::packUpdate(con, mask, stream);

   if (mask & InitialUpdateMask) {
      S32 i;
      // mask off images that aren't updated
      for(i = 0; i < MaxMountedImages; i++)
         if(!mMountedImageList[i].shapeName)
            mask &= ~(ImageMaskN << i);
   }

   if(!stream->writeFlag(mask & (NameMask | DamageMask |
         ImageMask | CloakMask | MountedMask |
         SkinMask | DataBlockMask)))
      return retMask;

   if (stream->writeFlag(mask & DataBlockMask)) 
   {
      if(stream->writeFlag(mShapeEPItem))
         stream->writeInt(mShapeEPItem, Base::Bit32);
   }

   if (stream->writeFlag(mask & DamageMask)) 
   {
      stream->writeInt(mDamageState,NumDamageStateBits);
   }

   if (stream->writeFlag(mask & ImageMask)) 
   {
      for (int i = 0; i < MaxMountedImages; i++)
      {
         if (stream->writeFlag(mask & (ImageMaskN << i))) 
         {
            MountedImage& image = mMountedImageList[i];

            if (stream->writeFlag(image.shapeName && image.shapeName[0]))
            {
               stream->writeString(image.shapeName);
               stream->writeString(image.mountPoint);
               stream->writeString(image.mountPointSelf);
               con->packNetStringHandleU(stream, image.skinNameHandle);
               if(stream->writeFlag(image.imageEPItem))
                  stream->writeInt(image.imageEPItem, Base::Bit32);
            }
         }
      }
   }

   // Group some of the uncommon stuff together.
   if (stream->writeFlag(mask & (NameMask | CloakMask | SkinMask))) {
      if (stream->writeFlag(mask & CloakMask)) {
         // cloaking
         stream->writeFlag( mCloaked );

         // piggyback control update
         stream->writeFlag(bool(getControllingClient()));

         //// fading
         //if(stream->writeFlag(mFading && mFadeElapsedTime >= mFadeDelay)) {
         //   stream->writeFlag(mFadeOut);
         //   stream->write(mFadeTime);
         //}
         //else
         //   stream->writeFlag(mFadeVal == 1.0f);
      }
      if (stream->writeFlag(mask & NameMask)) {
         con->packNetStringHandleU(stream, mShapeNameHandle);
      }

      if (stream->writeFlag(mask & SkinMask)) 
      {
         stream->writeString(mShapeStack.getShapes().shapeName);
		 stream->writeInt(mSkinTag,8);
		 con->packNetStringHandleU(stream, mEdgeBlurTextureName);
		 if(stream->writeFlag(mShapeEPItem))
			 stream->writeInt(mShapeEPItem, Base::Bit32);
		 stream->writeFlag(mShapeShifting);
		 stream->writeFlag(mFluidLight);
      }
   }

   if (mask & MountedMask) {
      if (mMount.object) {
         S32 gIndex = con->getGhostIndex(mMount.object);
         if (stream->writeFlag(gIndex != -1)) {
            stream->writeFlag(true);
            stream->writeInt(gIndex,NetConnection::GhostIdBitSize);
            stream->writeString(mMount.node);
			stream->writeString(mMount.myNode);
         }
         else
            // Will have to try again later
            retMask |= MountedMask;
      }
      else
         // Unmount if this isn't the initial packet
         if (stream->writeFlag(!(mask & InitialUpdateMask)))
            stream->writeFlag(false);
   }
   else
      stream->writeFlag(false);

   return retMask;
}

void ShapeBase::unpackUpdate(NetConnection *con, BitStream *stream)
{
   Parent::unpackUpdate(con, stream);
   mLastRenderFrame = SceneGraph::smFrameKey; // make sure we get a process after the event...

   if(!stream->readFlag())
      return;

   if (stream->readFlag()) 
   {
      if(stream->readFlag())
         addEPItems(-1, stream->readInt(Base::Bit32));
      else
         addEPItems(-1, 0);
   }

   if (stream->readFlag()) 
   {
      setDamageState(DamageState(stream->readInt(NumDamageStateBits)));
   }

   if (stream->readFlag()) 
   {
      for (int i = 0; i < MaxMountedImages; i++)
      {
         if (stream->readFlag())
         {
            if (stream->readFlag())
            {
               //把当前模型放置到加载列表
               m_isLoading = true;

               char buf[256];
                
               ShapesSet::subImage shapeImage;

               stream->readString(buf,256);
               shapeImage.shapeName = StringTable->insert(buf);

			   stream->readString(buf,256);
			   shapeImage.mountPoint = StringTable->insert(buf);

			   stream->readString(buf,256);
			   shapeImage.mountPointSelf = StringTable->insert(buf);
               
			   NetStringHandle skinDesiredNameHandle = con->unpackNetStringHandleU(stream);
              
               shapeImage.effectId = 0;

               if(stream->readFlag())
                   shapeImage.effectId = stream->readInt(Base::Bit32);
               else
                   shapeImage.effectId = 0;

               mShapeStack.setShapes(i,shapeImage.shapeName,shapeImage.skinName,shapeImage.mountPoint,shapeImage.mountPointSelf,shapeImage.effectId);
            }
			else
            {
               unmountImage(i);

			   mShapeStack.setShapes(i,NULL,NULL,NULL,NULL,0);
               //unset loading list
               m_loadingModels.subImages[i].shapeName = 0;
#ifdef NTJ_CLIENT
			   if(getControllingClient()){
				   g_ClientGameplayState->setPlayerModelView();
				   g_ClientGameplayState->refreshPlayerModelView();
			   }
#endif
           }
         }
      }
   }

   if (stream->readFlag())
   {
      if(stream->readFlag())     // Cloaked and control
      {
         setCloakedState(stream->readFlag());
         mIsControlled = stream->readFlag();

         //if (( mFading = stream->readFlag()) == true) {
         //   mFadeOut = stream->readFlag();
         //   if(mFadeOut)
         //      mFadeVal = 1.0f;
         //   else
         //      mFadeVal = 0;
         //   stream->read(&mFadeTime);
         //   mFadeDelay = 0;
         //   mFadeElapsedTime = 0;
         //}
         //else
         //   mFadeVal = F32(stream->readFlag());
      }
      if (stream->readFlag())  { // NameMask
         mShapeNameHandle = con->unpackNetStringHandleU(stream);
      }

      if (stream->readFlag()) 
      {  
          // SkinMask
         char shape[256];
         stream->readString(shape,256);
         
	     U8 SkinTag = stream->readInt(8);

		 mSkinTag = SkinTag;
		 NetStringHandle EdgeBlurTexture = con->unpackNetStringHandleU(stream);
		 setEdgeBlurTexture(EdgeBlurTexture);
         
         U32 effectId = 0;

		 if(stream->readFlag())
         {
             effectId = stream->readInt(Base::Bit32);
         }
		 else
			 addEPItems(-1, 0);

         mShapeStack.setShapes(-1, StringTable->insert(shape),NULL,0,0,effectId);

		 setShapeShifting(stream->readFlag());
		 mFluidLight = stream->readFlag();
      }
   }

   if (stream->readFlag()) {
      if (stream->readFlag()) {
         S32 gIndex = stream->readInt(NetConnection::GhostIdBitSize);
         ShapeBase* obj = dynamic_cast<ShapeBase*>(con->resolveGhost(gIndex));
		 char buff[256];
		 stream->readString(buff,256);
         StringTableEntry node = StringTable->insert(buff);
		 stream->readString(buff,256);
		 StringTableEntry myNode = StringTable->insert(buff);
         if(!obj)
         {
            con->setLastError("Invalid packet from server.");
            return;
         }
         obj->mountObject(this,node,myNode);
      }
      else
         unmount();
   }
}

void ShapeBase::setEdgeBlurTexture(StringTableEntry textName)
{
	if(!textName)
		textName = StringTable->insert("");
	NetStringHandle newtext(textName);
	setEdgeBlurTexture(newtext);
}

void ShapeBase::setEdgeBlurTexture(NetStringHandle& newtext)
{
	if(mEdgeBlurTextureName == newtext)
		return;
	mEdgeBlurTextureName = newtext;
#ifdef NTJ_CLIENT
	for (S32 i=0; i<MaxEdgeBlur; ++i)
		if(mEdgeBlur[i])
			mEdgeBlur[i]->SetTexture(mEdgeBlurTextureName.getString());
#endif
	setMaskBits(SkinMask);
}

bool ShapeBase::addEPItems(S32 slot, U32 id)
{
#ifdef NTJ_SERVER
	if(slot == -1)
	{
		mShapeEPItem = id;
		// setMaskBits();
		return true;
	}
	else if(slot >= 0 && slot < MaxMountedImages)
	{
		MountedImage& image = mMountedImageList[slot];
		if(image.shapeName)
		{
			image.imageEPItem = id;
			setMaskBits(ImageMaskN << slot);
			return true;
		}
	}
#endif
#ifdef NTJ_CLIENT
	if(!isProperlyAdded())
	{
		if(slot == -1)
		{
			mShapeEPItem = id;
		}
		else if(slot >= 0 && slot < MaxMountedImages)
		{
			MountedImage& image = mMountedImageList[slot];
			if(image.shapeName)
				image.imageEPItem = id;
		}
	}
	else
	{
		if(slot == -1)
		{
			if(mShapeEPItem)
				g_EffectPacketContainer.removeEffectPacket(mShapeEPItem);
			mShapeEPItem = id ? g_EffectPacketContainer.addEffectPacket(id, this, getRenderTransform(), this, getRenderPosition()) : 0;
		}
		else if(slot >= 0 && slot < MaxMountedImages)
		{
			MountedImage& image = mMountedImageList[slot];
			if(image.shapeName)
			{
				if(image.imageEPItem)
					g_EffectPacketContainer.removeEffectPacket(image.imageEPItem);
				image.imageEPItem = id ? g_EffectPacketContainer.addEffectPacket(id, this, getRenderTransform(), this, getRenderPosition()) : 0;
			}
		}
	}
	return true;
#endif
	return false;
}

void ShapeBase::activeEPItems()
{
#ifdef NTJ_CLIENT
	if(mShapeEPItem)
		mShapeEPItem = g_EffectPacketContainer.addEffectPacket(mShapeEPItem, this, getRenderTransform(), this, getRenderPosition());

	for (S32 j=0; j<MaxMountedImages; ++j)
	{
		MountedImage& image = mMountedImageList[j];
		if(image.shapeName && image.imageEPItem)
			image.imageEPItem = g_EffectPacketContainer.addEffectPacket(image.imageEPItem, this, getRenderTransform(), this, getRenderPosition());
	}
#endif
}

void ShapeBase::addFluidLight(S32 count)
{
#ifdef NTJ_SERVER
	mFluidLight += count;
	AssertFatal(mFluidLight >= 0 && mFluidLight < 0xFFF, "FluidLight!");
	setMaskBits(SkinMask);
#endif
}

//--------------------------------------------------------------------------

void ShapeBase::forceUncloak(const char * reason)
{
   AssertFatal(isServerObject(), "ShapeBase::forceUncloak: server only call");
   if(!mCloaked)
      return;

   Con::executef(mDataBlock, "onForceUncloak", scriptThis(), reason ? reason : "");
}

void ShapeBase::setCloakedState(bool cloaked)
{
   if (cloaked == mCloaked)
      return;

   if (isServerObject())
      setMaskBits(CloakMask);

   // Have to do this for the client, if we are ghosted over in the initial
   //  packet as cloaked, we set the state immediately to the extreme
   if (isProperlyAdded() == false) {
      mCloaked = cloaked;
      if (mCloaked)
         mCloakLevel = 1.0;
      else
         mCloakLevel = 0.0;
   } else {
      mCloaked = cloaked;
   }
}


//--------------------------------------------------------------------------

//void ShapeBase::setHidden(bool hidden)
//{
//   if (hidden != mHidden) {
//      // need to set a mask bit to make the ghost manager delete copies of this object
//      // hacky, but oh well.
//      setMaskBits(CloakMask);
//      if (mHidden)
//         addToScene();
//      else
//         removeFromScene();
//
//      mHidden = hidden;
//   }
//}

//--------------------------------------------------------------------------

void ShapeBaseConvex::findNodeTransform()
{
   S32 dl = pShapeBase->mDataBlock->collisionDetails[hullId];

   TSShapeInstance* si = pShapeBase->getShapeInstance();
   TSShape* shape = si->getShape();

   const TSShape::Detail* detail = &shape->details[dl];
   const S32 subs = detail->subShapeNum;
   const S32 start = shape->subShapeFirstObject[subs];
   const S32 end = start + shape->subShapeNumObjects[subs];

   // Find the first object that contains a mesh for this
   // detail level. There should only be one mesh per
   // collision detail level.
   for (S32 i = start; i < end; i++) 
   {
      const TSShape::Object* obj = &shape->objects[i];
      if (obj->numMeshes && detail->objectDetailNum < obj->numMeshes) 
      {
         nodeTransform = &si->getNodeTransforms()[obj->nodeIndex];
         return;
      }
   }
   return;
}

const MatrixF& ShapeBaseConvex::getTransform() const
{
   // If the transform isn't specified, it's assumed to be the
   // origin of the shape.
   const MatrixF& omat = (transform != 0)? *transform: mObject->getTransform();

   // Multiply on the mesh shape offset
   // tg: Returning this static here is not really a good idea, but
   // all this Convex code needs to be re-organized.
   if (nodeTransform) {
      static MatrixF mat;
      mat.mul(omat,*nodeTransform);
      return mat;
   }
   return omat;
}

Box3F ShapeBaseConvex::getBoundingBox() const
{
   const MatrixF& omat = (transform != 0)? *transform: mObject->getTransform();
   return getBoundingBox(omat, mObject->getScale());
}

Box3F ShapeBaseConvex::getBoundingBox(const MatrixF& mat, const Point3F& scale) const
{
   Box3F newBox = box;
   newBox.min.convolve(scale);
   newBox.max.convolve(scale);
   mat.mul(newBox);
   return newBox;
}

Point3F ShapeBaseConvex::support(const VectorF& v) const
{
   if (!pShapeBase->mShapeInstance)
       return Point3F(0,0,0);

   TSShape::ConvexHullAccelerator* pAccel =
      pShapeBase->mShapeInstance->getShape()->getAccelerator(pShapeBase->mDataBlock->collisionDetails[hullId]);
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


void ShapeBaseConvex::getFeatures(const MatrixF& mat, const VectorF& n, ConvexFeature* cf)
{
   if (!pShapeBase->mShapeInstance)
       return;

   cf->material = 0;
   cf->object = mObject;

   TSShape::ConvexHullAccelerator* pAccel =
      pShapeBase->mShapeInstance->getShape()->getAccelerator(pShapeBase->mDataBlock->collisionDetails[hullId]);
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


void ShapeBaseConvex::getPolyList(AbstractPolyList* list)
{
   if (!pShapeBase->mShapeInstance)
       return;

   list->setTransform(&pShapeBase->getTransform(), pShapeBase->getScale());
   list->setObject(pShapeBase);

   pShapeBase->mShapeInstance->animate(pShapeBase->mDataBlock->collisionDetails[hullId]);
   pShapeBase->mShapeInstance->buildPolyList(list,pShapeBase->mDataBlock->collisionDetails[hullId]);
}


//--------------------------------------------------------------------------

bool ShapeBase::isInvincible()
{
   if( mDataBlock )
   {
      return mDataBlock->isInvincible;
   }
   return false;
}

//--------------------------------------------------------------------------

void ShapeBase::setShapeName(const char* name)
{
   if (!isGhost()) {
      if (name[0] != '\0') {
         // Use tags for better network performance
         // Should be a tag, but we'll convert to one if it isn't.
         if (name[0] == StringTagPrefixByte)
            mShapeNameHandle = NetStringHandle(U32(dAtoi(name + 1)));
         else
            mShapeNameHandle = NetStringHandle(name);
      }
      else {
         mShapeNameHandle = NetStringHandle();
      }
      setMaskBits(NameMask);
   }
}


void ShapeBase::setSkinTag(const char name)
{
   if (!isGhost()) 
   {
	   mSkinTag = name;

       setMaskBits(SkinMask);
   }
}

//----------------------------------------------------------------------------
void ShapeBase::consoleInit()
{
	Con::addVariable("pref::environmentMaps", TypeBool, &gRenderEnvMaps);
}

#ifdef USE_MULTITHREAD_ANIMATE
void ShapeBase::deleteObject()
{
#ifndef NTJ_SERVER
	if(	g_MultThreadWorkMgr && g_MultThreadWorkMgr->isEnable())
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

bool ShapeBase::__CheckModelLoadings(void)
{
    if(!m_isLoading)
        return false;

	if (0 == mShapeInstance)
		return m_isLoading;

    bool isLoading = false;

    //判断主模型是否载入,如果主模型没有载入则忽略其他所有挂件
    if (0 != m_loadingModels.shapeName)
    {
        TSShapeInfo* pShapeInfo = g_TSShapeRepository.GetTSShapeInfo(m_loadingModels.shapeName);

        if (0 != pShapeInfo)
        {
            mTSSahpeInfo = pShapeInfo;
        }

        if (0 == pShapeInfo || !pShapeInfo->IsInitialized())
            return true;

        //加载主模型
        __InitShapeInstance();

        //更换成当前皮肤
        mShapeInstance->reSkin(mSkinTag);
        
        //加载特效
        addEPItems(-1,m_loadingModels.effectId);

        //重新挂载当前的所有mount
        for (U32 i = 0; i < MaxMountedImages; ++i)
        {   
            MountedImage& curImg = mMountedImageList[i];

            if (0 == curImg.shapeName || 0 == strlen(curImg.shapeName) || 0 == m_loadingModels.subImages[i].shapeName)
            {
                continue;
            }
            
            unmountImage(i);
            setImage(i,curImg.shapeName,NetStringHandle(),curImg.mountPoint,curImg.mountPointSelf);
        }

        //done!
        m_loadingModels.shapeName = 0;
#ifdef NTJ_CLIENT
		if(getControllingClient()){
			g_ClientGameplayState->setPlayerModelView();
			g_ClientGameplayState->refreshPlayerModelView();
		}
#endif
    }

    //加载每个挂件
    for (U32 i = 0; i < MaxMountedImages; ++i)
    {
        if (0 == mShapeInstance)
            return true;

        StringTableEntry shapeName = m_loadingModels.subImages[i].shapeName;

        if (0 == shapeName)
            continue;
        
        TSShapeInfo* pShapeInfo = g_TSShapeRepository.GetTSShapeInfo(shapeName);
        
        if (0 == pShapeInfo)
            continue;

        if (!pShapeInfo->IsInitialized())
        {
            isLoading = true;
            continue;
        }

        //do mount
        unmountImage(i);
        setImage(i,shapeName,NetStringHandle(),m_loadingModels.subImages[i].mountPoint,m_loadingModels.subImages[i].mountPointSelf);

        //加载特效
        addEPItems(i,m_loadingModels.subImages[i].effectId);

        m_loadingModels.subImages[i].shapeName = 0;
#ifdef NTJ_CLIENT
		if(getControllingClient()){
			g_ClientGameplayState->setPlayerModelView();
			g_ClientGameplayState->refreshPlayerModelView();
		}
#endif
    }

    return isLoading;
}

void ShapeBase::__LoadModel(const char* model)
{
    if (0 == model)
        return;

    m_isLoading = true;
    g_TSShapeRepository.InitializeTSShape(model,true,false,isClientObject());
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

ConsoleMethod( ShapeBaseData, checkDeployPos, bool, 3, 3, "(Transform xform)")
{
	return false;
	//if (bool(object->shape) == false)
	//	return false;

	//Point3F pos(0, 0, 0);
	//AngAxisF aa(Point3F(0, 0, 1), 0);
	//dSscanf(argv[2],"%g %g %g %g %g %g %g",
	//	&pos.x,&pos.y,&pos.z,&aa.axis.x,&aa.axis.y,&aa.axis.z,&aa.angle);
	//MatrixF mat;
	//aa.setMatrix(&mat);
	//mat.setColumn(3,pos);

	//Box3F objBox = object->shape->bounds;
	//Point3F boxCenter = (objBox.min + objBox.max) * 0.5f;
	//objBox.min = boxCenter + (objBox.min - boxCenter) * 0.9f;
	//objBox.max = boxCenter + (objBox.max - boxCenter) * 0.9f;

	//Box3F wBox = objBox;
	//mat.mul(wBox);

	//EarlyOutPolyList polyList;
	//polyList.mNormal.set(0,0,0);
	//polyList.mPlaneList.clear();
	//polyList.mPlaneList.setSize(6);
	//polyList.mPlaneList[0].set(objBox.min,VectorF(-1,0,0));
	//polyList.mPlaneList[1].set(objBox.max,VectorF(0,1,0));
	//polyList.mPlaneList[2].set(objBox.max,VectorF(1,0,0));
	//polyList.mPlaneList[3].set(objBox.min,VectorF(0,-1,0));
	//polyList.mPlaneList[4].set(objBox.min,VectorF(0,0,-1));
	//polyList.mPlaneList[5].set(objBox.max,VectorF(0,0,1));

	//for (U32 i = 0; i < 6; i++)
	//{
	//	PlaneF temp;
	//	mTransformPlane(mat, Point3F(1, 1, 1), polyList.mPlaneList[i], &temp);
	//	polyList.mPlaneList[i] = temp;
	//}

	//if (gServerContainer.buildPolyList(wBox, InteriorObjectType | StaticShapeObjectType, &polyList))
	//	return false;
	//return true;
}


ConsoleMethod(ShapeBaseData, getDeployTransform, const char *, 4, 4, "(Point3F pos, Point3F normal)")
{
	Point3F normal;
	Point3F position;
	dSscanf(argv[2], "%g %g %g", &position.x, &position.y, &position.z);
	dSscanf(argv[3], "%g %g %g", &normal.x, &normal.y, &normal.z);
	normal.normalize();

	VectorF xAxis;
	if( mFabs(normal.z) > mFabs(normal.x) && mFabs(normal.z) > mFabs(normal.y))
		mCross( VectorF( 0, 1, 0 ), normal, &xAxis );
	else
		mCross( VectorF( 0, 0, 1 ), normal, &xAxis );

	VectorF yAxis;
	mCross( normal, xAxis, &yAxis );

	MatrixF testMat(true);
	testMat.setColumn( 0, xAxis );
	testMat.setColumn( 1, yAxis );
	testMat.setColumn( 2, normal );
	testMat.setPosition( position );

	char *returnBuffer = Con::getReturnBuffer(256);
	Point3F pos;
	testMat.getColumn(3,&pos);
	AngAxisF aa(testMat);
	dSprintf(returnBuffer,256,"%g %g %g %g %g %g %g",
		pos.x,pos.y,pos.z,aa.axis.x,aa.axis.y,aa.axis.z,aa.angle);
	return returnBuffer;
}



//--------------------------------------------------------------------------
//----------------------------------------------------------------------------
ConsoleMethod( ShapeBase, setHidden, void, 3, 3, "(bool show)")
{
   object->setHidden(dAtob(argv[2]));
}

ConsoleMethod( ShapeBase, isHidden, bool, 2, 2, "")
{
   return object->isHidden();
}


//----------------------------------------------------------------------------
ConsoleMethod( ShapeBase, mountObject, bool, 4, 5, "( ShapeBase object, int slot )"
              "Mount ourselves on an object in the specified slot.")
{
   ShapeBase *target;
   if (Sim::findObject(argv[2],target)) {
      StringTableEntry node = StringTable->insert(argv[3]);
	  object->mountObject(target,node,(argc == 5) ? StringTable->insert(argv[4]) : 0);
      return true;
   }
   return false;
}

ConsoleMethod( ShapeBase, unmountObject, bool, 3, 3, "(ShapeBase obj)"
              "Unmount an object from ourselves.")
{
   ShapeBase *target;
   if (Sim::findObject(argv[2],target)) {
      object->unmountObject(target);
      return true;
   }
   return false;
}

ConsoleMethod( ShapeBase, unmount, void, 2, 2, "Unmount from the currently mounted object if any.")
{
   object->unmount();
}

ConsoleMethod( ShapeBase, isMounted, bool, 2, 2, "Are we mounted?")
{
   return object->isMounted();
}

ConsoleMethod( ShapeBase, getObjectMount, S32, 2, 2, "Returns the ShapeBase we're mounted on.")
{
   return object->isMounted()? object->getObjectMount()->getId(): 0;
}

ConsoleMethod( ShapeBase, getobjectName, const char*, 2, 2, "%obj.GetObjectName()")
{
	return object->getObjectName();
}

ConsoleMethod( ShapeBase, getMountedObjectCount, S32, 2, 2, "")
{
   return object->getMountedObjectCount();
}

ConsoleMethod( ShapeBase, getMountedObject, S32, 3, 3, "(int slot)")
{
   ShapeBase* mobj = object->getMountedObject(dAtoi(argv[2]));
   return mobj? mobj->getId(): 0;
}

ConsoleMethod( ShapeBase, getMountedObjectNode, const char*, 3, 3, "(int node)")
{
   return object->getMountedObjectNode(dAtoi(argv[2]));
}

ConsoleMethod( ShapeBase, getMountNodeObject, S32, 3, 3, "(int node)")
{
   ShapeBase* mobj = object->getMountNodeObject(StringTable->insert(argv[2]));
   return mobj? mobj->getId(): 0;
}


//----------------------------------------------------------------------------
ConsoleMethod( ShapeBase, mountImage, bool, 6, 7, "(ShapeBaseImageData image, int slot, bool loaded=true, string skinTag=NULL)")
{
   if (argv[2] && argv[2][0]) {
      U32 slot = dAtoi(argv[3]);
      NetStringHandle team;
      StringTableEntry mountPoint = argv[4];
      StringTableEntry mountPointSelf = argv[5];
      if(argv[6] && argv[6][0] != 0)
         team = NetStringHandle(argv[6]);
      if (slot >= 0 && slot < ShapeBase::MaxMountedImages)
         object->mountImage( argv[2],slot,team,mountPoint,mountPointSelf);
   }
   return false;
}

ConsoleMethod( ShapeBase, unmountImage, bool, 3, 3, "(int slot)")
{
   int slot = dAtoi(argv[2]);
   if (slot >= 0 && slot < ShapeBase::MaxMountedImages)
      return object->unmountImage(slot);
   return false;
}

ConsoleMethod( ShapeBase, getMountedImage, const char*, 3, 3, "(int slot)")
{
   int slot = dAtoi(argv[2]);
   if (slot >= 0 && slot < ShapeBase::MaxMountedImages)
      if (object->getMountedImage(slot))
      {
         char* buff = Con::getReturnBuffer(100);
         dSprintf(buff, 100, object->getMountedImage(slot));
         return buff;
      }
   return "";
}

ConsoleMethod( ShapeBase, isImageMounted, bool, 3, 3, "(ShapeBaseImageData db)")
{
   if (argv[2] && argv[2][0])
      return object->isImageMounted(argv[2]);
   return false;
}

ConsoleMethod( ShapeBase, getMountSlot, S32, 3, 3, "(ShapeBaseImageData db)")
{
   if (argv[2] && argv[2][0])
      return object->getMountSlot(argv[2]);
   return -1;
}

ConsoleMethod( ShapeBase, getImageSkinTag, S32, 3, 3, "(int slot)")
{
   int slot = dAtoi(argv[2]);
   if (slot >= 0 && slot < ShapeBase::MaxMountedImages)
      return object->getImageSkinTag(slot).getIndex();
   return -1;
}

//ConsoleMethod( ShapeBase, getSlotTransform, const char*, 3, 3, "(int slot)")
//{
//   int slot = dAtoi(argv[2]);
//   MatrixF xf(true);
//   if (slot >= 0 && slot < ShapeBase::MaxMountedImages)
//      object->getMountTransform(slot,&xf);
//
//   Point3F pos;
//   xf.getColumn(3,&pos);
//   AngAxisF aa(xf);
//   char* buff = Con::getReturnBuffer(200);
//   dSprintf(buff,200,"%g %g %g %g %g %g %g",
//            pos.x,pos.y,pos.z,aa.axis.x,aa.axis.y,aa.axis.z,aa.angle);
//   return buff;
//}
//
ConsoleMethod( ShapeBase, getVelocity, const char *, 2, 2, "")
{
   const VectorF& vel = object->getVelocity();
   char* buff = Con::getReturnBuffer(100);
   dSprintf(buff,100,"%g %g %g",vel.x,vel.y,vel.z);
   return buff;
}

ConsoleMethod( ShapeBase, setVelocity, bool, 3, 3, "(Vector3F vel)")
{
   VectorF vel(0,0,0);
   dSscanf(argv[2],"%g %g %g",&vel.x,&vel.y,&vel.z);
   object->setVelocity(vel);
   return true;
}

ConsoleMethod( ShapeBase, applyImpulse, bool, 4, 4, "(Point3F Pos, VectorF vel)")
{
   Point3F pos(0,0,0);
   VectorF vel(0,0,0);
   dSscanf(argv[2],"%g %g %g",&pos.x,&pos.y,&pos.z);
   dSscanf(argv[3],"%g %g %g",&vel.x,&vel.y,&vel.z);
   object->applyImpulse(pos,vel);
   return true;
}

ConsoleMethod( ShapeBase, getEyeVector, const char*, 2, 2, "")
{
   MatrixF mat;
   object->getEyeTransform(&mat);
   VectorF v2;
   mat.getColumn(1,&v2);
   char* buff = Con::getReturnBuffer(100);
   dSprintf(buff, 100,"%g %g %g",v2.x,v2.y,v2.z);
   return buff;
}

ConsoleMethod( ShapeBase, getEyePoint, const char*, 2, 2, "")
{
   MatrixF mat;
   object->getEyeTransform(&mat);
   Point3F ep;
   mat.getColumn(3,&ep);
   char* buff = Con::getReturnBuffer(100);
   dSprintf(buff, 100,"%g %g %g",ep.x,ep.y,ep.z);
   return buff;
}

ConsoleMethod( ShapeBase, getEyeTransform, const char*, 2, 2, "")
{
   MatrixF mat;
   object->getEyeTransform(&mat);

   Point3F pos;
   mat.getColumn(3,&pos);
   AngAxisF aa(mat);
   char* buff = Con::getReturnBuffer(100);
   dSprintf(buff,100,"%g %g %g %g %g %g %g",
            pos.x,pos.y,pos.z,aa.axis.x,aa.axis.y,aa.axis.z,aa.angle);
   return buff;
}

//----------------------------------------------------------------------------
// 获取游戏对象的数据ID
ConsoleMethod(ShapeBase, GetDataName, S32, 2, 2, "%obj.GetDataID()")
{
	return object ? object->getDataID() : 0;
}

ConsoleMethod( ShapeBase, setDamageState, bool, 3, 3, "(string state)")
{
   return object->setDamageState(argv[2]);
}

ConsoleMethod( ShapeBase, getDamageState, const char*, 2, 2, "")
{
   return object->getDamageStateName();
}

ConsoleMethod( ShapeBase, isDisabled, bool, 2, 2, "True if the state is not Enabled.")
{
   return object->getDamageState() != ShapeBase::Enabled;
}

ConsoleMethod( ShapeBase, isEnabled, bool, 2, 2, "")
{
   return object->getDamageState() == ShapeBase::Enabled;
}

ConsoleMethod( ShapeBase, getControllingClient, S32, 2, 2, "Returns a GameConnection.")
{
   if (GameConnection* con = object->getControllingClient())
      return con->getId();
   return 0;
}

ConsoleMethod( ShapeBase, getControllingObject, S32, 2, 2, "")
{
   if (ShapeBase* con = object->getControllingObject())
      return con->getId();
   return 0;
}

// return true if can cloak, otherwise the reason why object cannot cloak
ConsoleMethod( ShapeBase, canCloak, bool, 2, 2, "")
{
   return true;
}

ConsoleMethod( ShapeBase, setCloaked, void, 3, 3, "(bool isCloaked)")
{
   bool cloaked = dAtob(argv[2]);
   if (object->isServerObject())
      object->setCloakedState(cloaked);
}

ConsoleMethod( ShapeBase, isCloaked, bool, 2, 2, "")
{
   return object->getCloakedState();
}

ConsoleMethod( ShapeBase, getCameraFov, F32, 2, 2, "")
{
   if (object->isServerObject())
      return object->getCameraFov();
   return 0.0;
}

ConsoleMethod( ShapeBase, setCameraFov, void, 3, 3, "(float fov)")
{
   if (object->isServerObject())
      object->setCameraFov(dAtof(argv[2]));
}

ConsoleMethod( ShapeBase, startFade, void, 5, 5, "( int fadeTimeMS, int fadeDelayMS, bool fadeOut )")
{
   U32   fadeTime;
   U32   fadeDelay;
   bool  fadeOut;

   dSscanf(argv[2], "%d", &fadeTime );
   dSscanf(argv[3], "%d", &fadeDelay );
   fadeOut = dAtob(argv[4]);

   //object->startFade( fadeTime / 1000.0, fadeDelay / 1000.0, fadeOut );
}

ConsoleMethod( ShapeBase, setShapeName, void, 3, 3, "(string tag)")
{
   object->setShapeName(argv[2]);
}

ConsoleMethod( ShapeBase, getShapeName, const char*, 2, 2, "")
{
   return object->getShapeName();
}
