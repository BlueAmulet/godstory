//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "sceneGraph/decalManager.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sceneState.h"
#include "ts/tsShapeInstance.h"
#include "core/bitStream.h"
#include "console/console.h"
#include "console/dynamicTypes.h"
#include "gfx/primBuilder.h"
#include "renderInstance/renderInstMgr.h"
#include "console/consoleTypes.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK

GFXStateBlock* DecalManager::mDisTexSB = NULL;
GFXStateBlock* DecalManager::mSetSB = NULL;
GFXStateBlock* DecalManager::mSelfTrueSB = NULL;
GFXStateBlock* DecalManager::mSelfFalseSB = NULL;
GFXStateBlock* DecalManager::mClearSB = NULL;


GFXStateBlock* ProjectDecalManager::mSetSB = NULL;
GFXStateBlock* ProjectDecalManager::mZWriteSB = NULL;
GFXStateBlock* ProjectDecalManager::mClearSB = NULL;

bool DecalManager::smDecalsOn = true;

bool DecalManager::sgThisIsSelfIlluminated = false;
bool DecalManager::sgLastWasSelfIlluminated = false;

const U32 DecalManager::csmFreePoolBlockSize = 256;
U32       DecalManager::smMaxNumDecals = 256;
U32       DecalManager::smDecalTimeout = 5000;

DecalManager* gDecalManager = NULL;
IMPLEMENT_CONOBJECT(DecalManager);
IMPLEMENT_CO_DATABLOCK_V1(DecalData);

namespace {

int QSORT_CALLBACK cmpDecalInstance(const void* p1, const void* p2)
{
   const DecalInstance** pd1 = (const DecalInstance**)p1;
   const DecalInstance** pd2 = (const DecalInstance**)p2;

   return int(((char *)(*pd1)->decalData) - ((char *)(*pd2)->decalData));
}

int QSORT_CALLBACK cmpProjectDecalInstance(const void* p1, const void* p2)
{
	const ProjectDecalInstance** pd1 = (const ProjectDecalInstance**)p1;
	const ProjectDecalInstance** pd2 = (const ProjectDecalInstance**)p2;

	return int(((char *)(*pd1)->mProjectDecalData) - ((char *)(*pd2)->mProjectDecalData));
}

} // namespace {}


//--------------------------------------------------------------------------
DecalData::DecalData()
{
   sizeX = 1;
   sizeY = 1;
   textureName = "";
   
   selfIlluminated = false;
   lifeSpan = DecalManager::smDecalTimeout;
}

DecalData::~DecalData()
{
   textureHandle = NULL;

   if(gDecalManager)
      gDecalManager->dataDeleted(this);
}


void DecalData::packData(BitStream* stream)
{
   Parent::packData(stream);

   stream->write(sizeX);
   stream->write(sizeY);
   stream->writeString(textureName);
   
   stream->write(selfIlluminated);
   stream->write(lifeSpan);
}

void DecalData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   stream->read(&sizeX);
   stream->read(&sizeY);
   textureName = stream->readSTString();
   
   stream->read(&selfIlluminated);
   stream->read(&lifeSpan);
}

bool DecalData::preload(bool server, char errorBuffer[256])
{
   if (Parent::preload(server, errorBuffer) == false)
      return false;

   if (sizeX < 0.0) {
      Con::warnf("DecalData::preload: sizeX < 0");
      sizeX = 0;
   }
   if (sizeY < 0.0) {
      Con::warnf("DecalData::preload: sizeX < 0");
      sizeY = 0;
   }
   if (textureName == NULL || textureName[0] == '\0') {
      Con::errorf("No texture name for decal!");
      return false;
   }

   if (!server) 
   {
      if(!textureHandle.set(textureName, &GFXDefaultStaticDiffuseProfile))
      {
         Con::errorf("Unable to load texture: %s for decal!", textureName);
         return false;
      }
   }

   return true;
}

//IMPLEMENT_SETDATATYPE(DecalData)
//IMPLEMENT_GETDATATYPE(DecalData)

void DecalData::initPersistFields()
{
   //Con::registerType("DecalDataPtr", TypeDecalDataPtr, sizeof(DecalData*),
   //                  REF_GETDATATYPE(DecalData),
   //                  REF_SETDATATYPE(DecalData));

   addField("sizeX",       TypeF32,       Offset(sizeX,       DecalData));
   addField("sizeY",       TypeF32,       Offset(sizeY,       DecalData));
   addField("textureName", TypeFilename,  Offset(textureName, DecalData));
   
   addField("selfIlluminated", TypeBool, Offset(selfIlluminated, DecalData));
   addField("lifeSpan", TypeS32, Offset(lifeSpan, DecalData));
}


DecalManager::DecalManager()
{
   mTypeMask |= DecalManagerObjectType;

   mObjBox.min.set(-1e7, -1e7, -1e7);
   mObjBox.max.set( 1e7,  1e7,  1e7);
   mWorldBox.min.set(-1e7, -1e7, -1e7);
   mWorldBox.max.set( 1e7,  1e7,  1e7);

   mFreePool = NULL;
   VECTOR_SET_ASSOCIATION(mDecalQueue);
   VECTOR_SET_ASSOCIATION(mFreePoolBlocks);
}


DecalManager::~DecalManager()
{
   mFreePool = NULL;
   for (S32 i = 0; i < mFreePoolBlocks.size(); i++)
   {
      delete [] mFreePoolBlocks[i];
   }
   mDecalQueue.clear();
}


DecalInstance* DecalManager::allocateDecalInstance()
{
   if (mFreePool == NULL)
   {
      // Allocate a new block of decals
      mFreePoolBlocks.push_back(new DecalInstance[csmFreePoolBlockSize]);

      // Init them onto the free pool chain
      DecalInstance* pNewInstances = mFreePoolBlocks.last();
      for (U32 i = 0; i < csmFreePoolBlockSize - 1; i++)
         pNewInstances[i].next = &pNewInstances[i + 1];
      pNewInstances[csmFreePoolBlockSize - 1].next = NULL;
      mFreePool = pNewInstances;
   }
   AssertFatal(mFreePool != NULL, "Error, should always have a free pool available here!");

   DecalInstance* pRet = mFreePool;
   mFreePool = pRet->next;
   pRet->next = NULL;
   return pRet;
}


void DecalManager::freeDecalInstance(DecalInstance* trash)
{
   AssertFatal(trash != NULL, "Error, no trash pointer to free!");

   trash->next = mFreePool;
   mFreePool = trash;
}


void DecalManager::dataDeleted(DecalData *data)
{
   for(S32 i = mDecalQueue.size() - 1; i >= 0; i--)
   {
      DecalInstance *inst = mDecalQueue[i];
      if(inst->decalData == data)
      {
         freeDecalInstance(inst);
         mDecalQueue.erase(U32(i));
      }
   }
}

void DecalManager::consoleInit()
{
   Con::addVariable("$pref::decalsOn",     TypeBool, &smDecalsOn);
   Con::addVariable("$pref::Decal::maxNumDecals", TypeS32, &smMaxNumDecals);
   Con::addVariable("$pref::Decal::decalTimeout", TypeS32, &smDecalTimeout);
}

void DecalManager::addDecal(const Point3F& pos,
                            Point3F normal,
                            DecalData* decalData)
{
   if (smMaxNumDecals == 0)
      return;

   // DMM: Rework this, should be based on time
   if(mDecalQueue.size() >= smMaxNumDecals)
   {
      findSpace();
   }

   Point3F vecX, vecY;
   DecalInstance* newDecal = allocateDecalInstance();
   newDecal->decalData = decalData;
   newDecal->allocTime = Platform::getVirtualMilliseconds();

   if(mFabs(normal.z) > 0.9f)
      mCross(normal, Point3F(0.0f, 1.0f, 0.0f), &vecX);
   else
      mCross(normal, Point3F(0.0f, 0.0f, 1.0f), &vecX);

   mCross(vecX, normal, &vecY);

   normal.normalizeSafe();
   Point3F position = Point3F(pos.x + (normal.x * 0.008), pos.y + (normal.y * 0.008), pos.z + (normal.z * 0.008));

   vecX.normalizeSafe();
   vecY.normalizeSafe();

   vecX *= decalData->sizeX;
   vecY *= decalData->sizeY;

   newDecal->point[0] = position + vecX + vecY;
   newDecal->point[1] = position + vecX - vecY;
   newDecal->point[2] = position - vecX - vecY;
   newDecal->point[3] = position - vecX + vecY;

   mDecalQueue.push_back(newDecal);
   mQueueDirty = true;
}

void DecalManager::addDecal(const Point3F& pos,
                            const Point3F& rot,
                            Point3F normal,
                            DecalData* decalData)
{
   if (smMaxNumDecals == 0)
      return;

    addDecal( pos, rot, normal, Point3F( 1, 1, 1 ), decalData );
}

void DecalManager::addDecal(const Point3F& pos,
                            const Point3F& rot,
                            Point3F normal,
                            const Point3F& scale,
                            DecalData* decalData)
{
   if (smMaxNumDecals == 0)
      return;

   if(mDot(rot, normal) < 0.98)
   {
      // DMM: Rework this, should be based on time
      if(mDecalQueue.size() >= smMaxNumDecals)
      {
         findSpace();
      }

      Point3F vecX, vecY;
      DecalInstance* newDecal = allocateDecalInstance();
      newDecal->decalData = decalData;
      newDecal->allocTime = Platform::getVirtualMilliseconds();

      mCross(rot, normal, &vecX);
      mCross(normal, vecX, &vecY);

      normal.normalize();
      Point3F position = Point3F(pos.x + (normal.x * 0.008), pos.y + (normal.y * 0.008), pos.z + (normal.z * 0.008));

      vecX.normalize();
      vecX.convolve( scale );
      vecY.normalize();
      vecY.convolve( scale );

      vecX *= decalData->sizeX;
      vecY *= decalData->sizeY;

      newDecal->point[0] = position + vecX + vecY;
      newDecal->point[1] = position + vecX - vecY;
      newDecal->point[2] = position - vecX - vecY;
      newDecal->point[3] = position - vecX + vecY;

      mDecalQueue.push_back(newDecal);
      mQueueDirty = true;
   }
}

bool DecalManager::prepRenderImage(SceneState* state, const U32 stateKey,
                                   const U32 /*startZone*/, const bool /*modifyBaseState*/)
{
   if (!smDecalsOn) return false;

   if (isLastState(state, stateKey))
      return false;
   setLastState(state, stateKey);

   if (mDecalQueue.size() == 0)
      return false;

   // This should be sufficient for most objects that don't manage zones, and
   //  don't need to return a specialized RenderImage...
   RenderInst *ri = gRenderInstManager.allocInst();
   ri->obj = this;
   ri->state = state;
   ri->type = RenderInstManager::RIT_Decal;
   //ri->calcSortPoint(this, state->getCameraPosition());
   gRenderInstManager.addInst(ri);

   U32 currMs = Platform::getVirtualMilliseconds();
   for (S32 i = mDecalQueue.size() - 1; i >= 0; i--)
   {
      U32 age = currMs - mDecalQueue[i]->allocTime;
     U32 timeout = mDecalQueue[i]->decalData->lifeSpan;
      if (age > timeout)
      {
         freeDecalInstance(mDecalQueue[i]);
         mDecalQueue.erase(i);
      }
      else if (age > ((3 * timeout) / 4))
      {
         mDecalQueue[i]->fade = 1.0f - (F32(age - ((3 * timeout) / 4)) / F32(timeout / 4));
      }
      else
      {
         mDecalQueue[i]->fade = 1.0f;
      }
   }

   if (mQueueDirty == true)
   {
      // Sort the decals based on the data pointers...
      dQsort(mDecalQueue.address(),
             mDecalQueue.size(),
             sizeof(DecalInstance*),
             cmpDecalInstance);
      mQueueDirty = false;
   }

   return false;
}

void DecalManager::renderObject(SceneState* state, RenderInst *)
{
   if (!smDecalsOn) return;

   MatrixF projection = GFX->getProjectionMatrix();
   RectI viewport = GFX->getViewport();

   GFX->pushWorldMatrix();
   GFX->disableShaders();

   renderDecal();

#ifdef STATEBLOCK
	AssertFatal(mDisTexSB, "DecalManager::renderObject -- mDisTexSB cannot be NULL.");
	mDisTexSB->apply();
#else
   GFX->setTextureStageColorOp(0, GFXTOPDisable);
   GFX->setTextureStageColorOp(1, GFXTOPDisable);
#endif


   GFX->popWorldMatrix();

   GFX->setViewport(viewport);
   GFX->setProjectionMatrix(projection);
}

#ifdef STATEBLOCK
void DecalManager::renderDecal()
{
	AssertFatal(mSetSB, "DecalManager::renderDecal -- mSetSB cannot be NULL.");
	mSetSB->apply();
	sgThisIsSelfIlluminated = false;
	sgLastWasSelfIlluminated = false;

	DecalData* pLastData = NULL;
	for(S32 x=0; x<mDecalQueue.size(); x++)
	{
		if(mDecalQueue[x]->decalData != pLastData)
		{
			GFX->setTexture(0, mDecalQueue[x]->decalData->textureHandle);
			pLastData = mDecalQueue[x]->decalData;
		}

		sgThisIsSelfIlluminated = mDecalQueue[x]->decalData->selfIlluminated;
		if(sgThisIsSelfIlluminated != sgLastWasSelfIlluminated)
		{
			if(sgThisIsSelfIlluminated)
			{
				AssertFatal(mSelfTrueSB, "DecalManager::renderDecal -- mSelfTrueSB cannot be NULL.");
				mSelfTrueSB->apply();
			}
			else
			{
				AssertFatal(mSelfFalseSB, "DecalManager::renderDecal -- mSelfFalseSB cannot be NULL.");
				mSelfFalseSB->apply();
			}
			sgLastWasSelfIlluminated = sgThisIsSelfIlluminated;
		}

		PrimBuild::color4f(1, 1, 1, mDecalQueue[x]->fade);
		PrimBuild::begin(GFXTriangleFan, 4);
		PrimBuild::texCoord2f(0, 0);
		PrimBuild::vertex3fv(mDecalQueue[x]->point[3]);
		PrimBuild::texCoord2f(0, 1);
		PrimBuild::vertex3fv(mDecalQueue[x]->point[2]);
		PrimBuild::texCoord2f(1, 1);
		PrimBuild::vertex3fv(mDecalQueue[x]->point[1]);
		PrimBuild::texCoord2f(1, 0);
		PrimBuild::vertex3fv(mDecalQueue[x]->point[0]);
		PrimBuild::end();
	}

	AssertFatal(mClearSB, "DecalManager::renderDecal -- mClearSB cannot be NULL.");
	mClearSB->apply();
}

#else
void DecalManager::renderDecal()
{
   GFX->setCullMode(GFXCullNone);
   GFX->setLightingEnable(false);
   GFX->setAlphaBlendEnable(true);
   GFX->setZEnable(true);
   GFX->setZFunc(GFXCmpLessEqual);
   GFX->setZWriteEnable(false);
   GFX->setSrcBlend(GFXBlendSrcAlpha);
   GFX->setDestBlend(GFXBlendInvSrcAlpha);
   GFX->setAlphaTestEnable(true);
   GFX->setAlphaFunc(GFXCmpGreater);
   GFX->setAlphaRef(0);
   GFX->setTextureStageColorOp(0, GFXTOPModulate);
   GFX->setTextureStageColorOp(1, GFXTOPDisable);

   sgThisIsSelfIlluminated = false;
   sgLastWasSelfIlluminated = false;
   
   F32 depthbias = -0.00002f;
   F32 slopebias = -0.06f;
   GFX->setZBias(*((U32 *)&depthbias));
   GFX->setSlopeScaleDepthBias(*((U32 *)&slopebias));
   
   DecalData* pLastData = NULL;
   for(S32 x=0; x<mDecalQueue.size(); x++)
   {
      if(mDecalQueue[x]->decalData != pLastData)
      {
        GFX->setTexture(0, mDecalQueue[x]->decalData->textureHandle);
         pLastData = mDecalQueue[x]->decalData;
      }
     
     sgThisIsSelfIlluminated = mDecalQueue[x]->decalData->selfIlluminated;
     if(sgThisIsSelfIlluminated != sgLastWasSelfIlluminated)
     {
        if(sgThisIsSelfIlluminated)
        {
             GFX->setSrcBlend(GFXBlendSrcAlpha);
             GFX->setDestBlend(GFXBlendOne);
        }
        else
        {
             GFX->setSrcBlend(GFXBlendSrcAlpha);
             GFX->setDestBlend(GFXBlendInvSrcAlpha);
        }
        sgLastWasSelfIlluminated = sgThisIsSelfIlluminated;
     }
      
     PrimBuild::color4f(1, 1, 1, mDecalQueue[x]->fade);
     PrimBuild::begin(GFXTriangleFan, 4);
     PrimBuild::texCoord2f(0, 0);
     PrimBuild::vertex3fv(mDecalQueue[x]->point[3]);
     PrimBuild::texCoord2f(0, 1);
     PrimBuild::vertex3fv(mDecalQueue[x]->point[2]);
     PrimBuild::texCoord2f(1, 1);
     PrimBuild::vertex3fv(mDecalQueue[x]->point[1]);
     PrimBuild::texCoord2f(1, 0);
     PrimBuild::vertex3fv(mDecalQueue[x]->point[0]);
     PrimBuild::end();
   }

   GFX->setAlphaBlendEnable(false);
   GFX->setZEnable(true);
   GFX->setZWriteEnable(true);
   GFX->setAlphaTestEnable(false);
   GFX->setTextureStageColorOp(0, GFXTOPDisable);

   GFX->setZBias(0);
   GFX->setSlopeScaleDepthBias(0);
}

#endif

void DecalManager::findSpace()
{
   S32 besttime = S32_MAX;
   U32 bestindex = 0;
   DecalInstance *bestdecal = NULL;

   U32 time = Platform::getVirtualMilliseconds();

   for(U32 i=0; i<mDecalQueue.size(); i++)
   {
      DecalInstance *inst = mDecalQueue[i];
      U32 age = time - inst->allocTime;
      U32 timeleft = inst->decalData->lifeSpan - age;
      if(besttime > timeleft)
      {
         besttime = timeleft;
         bestindex = i;
         bestdecal = inst;
      }
   }

   AssertFatal((bestdecal), "No good decals?");

   mDecalQueue.erase_fast(bestindex);
   freeDecalInstance(bestdecal);
}

void DecalManager::addDecal(const Point3F& pos, const Point3F& rot, Point3F normal,
                       const Point3F& scale, DecalData *decaldata, U32 ownerid)
{
   if(smMaxNumDecals == 0)
      return;

   if(mDot(rot, normal) < 0.98f)
   {
      if(mDecalQueue.size() >= smMaxNumDecals)
         findSpace();

      Point3F vecX, vecY;
      DecalInstance* newDecal = allocateDecalInstance();
      newDecal->decalData = decaldata;
      newDecal->allocTime = Platform::getVirtualMilliseconds();
      newDecal->ownerId = ownerid;

      mCross(rot, normal, &vecX);
      mCross(normal, vecX, &vecY);

      normal.normalize();
      Point3F position = Point3F(pos.x + (normal.x * 0.008f), pos.y + (normal.y * 0.008f), pos.z + (normal.z * 0.008f));

      vecX.normalize();
      vecX.convolve( scale );
      vecY.normalize();
      vecY.convolve( scale );

      vecX *= decaldata->sizeX;
      vecY *= decaldata->sizeY;

      newDecal->point[0] = position + vecX + vecY;
      newDecal->point[1] = position + vecX - vecY;
      newDecal->point[2] = position - vecX - vecY;
      newDecal->point[3] = position - vecX + vecY;

      mDecalQueue.push_back(newDecal);
      mQueueDirty = true;
   }
}

void DecalManager::ageDecal(U32 ownerid)
{
   for(U32 i=0; i<mDecalQueue.size(); i++)
   {
      DecalInstance *inst = mDecalQueue[i];
      if(inst->ownerId == ownerid)
      {
         freeDecalInstance(inst);
         mDecalQueue.erase(U32(i));
      }
   }
}

void DecalManager::resetStateBlock()
{

	//mDisTexSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPDisable );
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable );
	GFX->endStateBlock(mDisTexSB);

	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setRenderState(GFXRSZFunc, GFXCmpLessEqual);
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSAlphaRef, 0);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->setRenderState(GFXRSDepthBias, -0.00002f);
	GFX->setRenderState(GFXRSSlopeScaleDepthBias, -0.06f);
	GFX->endStateBlock(mSetSB);

	//mSelfTrueSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendOne);
	GFX->endStateBlock(mSelfTrueSB);

	//mSelfFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->endStateBlock(mSelfFalseSB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPDisable );
	GFX->setRenderState(GFXRSDepthBias, 0);
	GFX->setRenderState(GFXRSSlopeScaleDepthBias, 0);
	GFX->endStateBlock(mClearSB);
}


void DecalManager::releaseStateBlock()
{
	if (mDisTexSB)
	{
		mDisTexSB->release();
	}

	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mSelfTrueSB)
	{
		mSelfTrueSB->release();
	}

	if (mSelfFalseSB)
	{
		mSelfFalseSB->release();
	}

	if (mClearSB)
	{
		mClearSB->release();
	}
}

void DecalManager::init()
{
	if (mDisTexSB == NULL)
	{
		mDisTexSB = new GFXD3D9StateBlock;
		mDisTexSB->registerResourceWithDevice(GFX);
		mDisTexSB->mZombify = &releaseStateBlock;
		mDisTexSB->mResurrect = &resetStateBlock;

		mSetSB = new GFXD3D9StateBlock;
		mSelfFalseSB = new GFXD3D9StateBlock;
		mSelfTrueSB = new GFXD3D9StateBlock;
		mClearSB = new GFXD3D9StateBlock;

		resetStateBlock();
	}


}

void DecalManager::shutdown()
{
	SAFE_DELETE(mDisTexSB);
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mSelfFalseSB);
	SAFE_DELETE(mSelfTrueSB);
	SAFE_DELETE(mClearSB);
}

// ====================================================================================
//  投影模式
// ====================================================================================
#pragma message(ENGINE(新加Decal投影模式))

#define			DECAL_TEXTURE_COUNT			999			// 图片序列最大数，注意和ProjectDecalData::InitTexture()中的额字符串长度对应

const U32		ProjectDecalInstance::smProjectMask = InteriorObjectType|StaticShapeObjectType|StaticObjectType|TerrainObjectType|AtlasObjectType;

bool ProjectDecalManager::smDecalsOn = true;

bool ProjectDecalManager::sgThisIsSelfIlluminated = false;
bool ProjectDecalManager::sgLastWasSelfIlluminated = false;

const U32 ProjectDecalManager::csmFreePoolBlockSize = 8;
U32       ProjectDecalManager::smMaxNumDecals = 256;
U32       ProjectDecalManager::smDecalTimeout = 5000;
U32       ProjectDecalManager::mGlobeId = 0;

ProjectDecalManager* gProjectDecalManager = NULL;
IMPLEMENT_CONOBJECT(ProjectDecalManager);
IMPLEMENT_CO_DATABLOCK_V1(ProjectDecalData);


//---------------------------------------------------------------------------------
// ProjectDecalData

ProjectDecalData::ProjectDecalData()
{
	mInit = false;
	lifeSpan = ProjectDecalManager::smDecalTimeout;
	mInterval = 50;
	mTextureName = "";
	mRot = 0.0f;

	mDiameter = 1;
	mDiameterScale = 0.0f;                              // 直径缩放速率
	mMaxDiameter = mDiameter;						// 最大缩放尺寸
	mAlpha = 0.0f;								//初始alpha值
	mAlphaScale = 0.0005f;						//alpha变换速度
	mMaxAlpha = 1.0f;
	mLoop = false;								// 动画是否循环
	mLoopTimeStep = 0;						// 动画间隔
	mLoopType = 0;							    // 动画的循环类型
}

ProjectDecalData::~ProjectDecalData()
{
	mTextures.clear();

	if(gProjectDecalManager)
		gProjectDecalManager->dataDeleted(this);
}

void ProjectDecalData::packData(BitStream* stream)
{
	Parent::packData(stream);

	stream->write(lifeSpan);
	stream->write(mInterval);
	stream->writeString(mTextureName);
	stream->write(mRot);

	stream->write(mDiameter);
	stream->write(mDiameterScale);
	stream->write(mMaxDiameter);
	stream->write(mAlpha);
	stream->write(mAlphaScale);
	stream->write(mMaxAlpha);

	stream->writeFlag(mLoop);
	stream->write(mLoopTimeStep);
	stream->write(mLoopType);
}

void ProjectDecalData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);

	stream->read(&lifeSpan);
	stream->read(&mInterval);
	mTextureName = stream->readSTString();
    stream->read(&mRot);

	stream->read(&mDiameter);
	stream->read(&mDiameterScale);
	stream->read(&mMaxDiameter);
	stream->read(&mAlpha);
	stream->read(&mAlphaScale);
	stream->read(&mMaxAlpha);

	mLoop = stream->readFlag();
	stream->read(&mLoopTimeStep);
	stream->read(&mLoopType);
}

bool ProjectDecalData::preload(bool server, char errorBuffer[256])
{
	if (Parent::preload(server, errorBuffer) == false)
		return false;

	if (mTextureName == NULL || mTextureName[0] == '\0') {
		Con::errorf("No texture name for ProjectDecal!");
		return false;
	}

	if (!server) 
		InitTexture();

	return true;
}


void ProjectDecalData::InitTexture()
{
	if(mTextures.size() || mInit || !mTextureName || !mTextureName[0])
		return;

	S32 strLen = dStrlen(mTextureName) + 8;
	char* str = Con::getReturnBuffer(strLen);
	// 在mTextureName的末尾接着001、002作为图片名读取，从001开始
	// 当读取到第一张不存在的图片时结束
	for (S32 i=1; i<=DECAL_TEXTURE_COUNT; i++)
	{
		dSprintf(str, strLen, "%s%03d", mTextureName, i);
		mTextures.increment();
		if(!mTextures.last().set(str, &GFXDefaultStaticDiffuseProfile))
		{
			mTextures.decrement();
			break;
		}
	}

	mInit = true;
}

void ProjectDecalData::initPersistFields()
{
	addField("lifeSpan",		TypeS32,		Offset(lifeSpan,    ProjectDecalData));
	addField("Interval",		TypeS32,		Offset(mInterval,   ProjectDecalData));
	addField("TextureName",		TypeFilename,	Offset(mTextureName, ProjectDecalData));
    addField("Rot",		        TypeF32,	    Offset(mRot,        ProjectDecalData));

	addField("diameter",		TypeF32,		Offset(mDiameter,		ProjectDecalData));
	addField("diameterScale",	TypeF32,		Offset(mDiameterScale,  ProjectDecalData));
	addField("maxDiameter",		TypeF32,		Offset(mMaxDiameter,	ProjectDecalData));
	addField("alpha",		    TypeF32,		Offset(mAlpha,			ProjectDecalData));
	addField("alphaScale",		TypeF32,		Offset(mAlphaScale,		ProjectDecalData));
	addField("maxAlpha",		TypeF32,		Offset(mMaxAlpha,		ProjectDecalData));
	addField("loop",		    TypeBool,		Offset(mLoop,			ProjectDecalData));
	addField("loopTimeStep",	TypeS32,		Offset(mLoopTimeStep,	ProjectDecalData));
	addField("loopType",		TypeS8,			Offset(mLoopType,		ProjectDecalData));
}


//---------------------------------------------------------------------------------
// ProjectDecalInstance

ProjectDecalInstance::ProjectDecalInstance()
{
	mDecalMode = 0;
	fade = 1.0f;
	curIndex = 0;
	mRefresh = false;
	mRender = false;
	mNeedRender = false;
	mColor.set(255,255,255);
	mByHand = false;
	mAlpha = 0.0f;
	
	scaleLoopDir = false;
	alphaLoopDir = false;
}

ProjectDecalInstance::~ProjectDecalInstance()
{
	if(mByHand && mProjectDecalData)
		delete mProjectDecalData;
}

void ProjectDecalInstance::setColor(ColorI& color)
{
	mColor = color;
}

void ProjectDecalInstance::setPos(Point3F& pos)
{
	if(mPos != pos)
	{
		mPos = pos;
		mRefresh =true;
	}
}

void ProjectDecalInstance::setRotZ(F32 rotZ)
{
	if(mRotZ != rotZ)
	{
		mRotZ = rotZ;
		mRefresh = true;
	}
}

void ProjectDecalInstance::setRadius(F32 radius)
{
	if(mRadius != radius)
	{
		mRadius = getMin(radius,mProjectLen);
		mRefresh = true;
	}
}

void ProjectDecalInstance::setProjectLen(F32 len)
{
	if(mProjectLen != len)
	{
		mProjectLen = len;
		mRefresh = true;
	}
}

void ProjectDecalInstance::setFade(F32 val)
{
	if(fade != val)
	{
		fade = val;
		mRefresh = true;
	}
}

void ProjectDecalInstance::setNeedRender(bool val)
{
	if(mNeedRender != val)
	{
		mNeedRender = val;
		mRefresh = true;
	}
}

bool ProjectDecalInstance::prepare()
{
	if (fade < TSMesh::VISIBILITY_EPSILON)
		return false;
	if (mRadius <= 0)
		return false;
#pragma message(ENGINE(使用当前视景体对投影贴花对象进行裁剪，只要视景体内部的))
	if (gClientSceneGraph && gClientSceneGraph->getCurSceneState())
	{
		SceneState *state = gClientSceneGraph->getCurSceneState();
		const Frustum& frustum = state->getFrustum();
		Box3F box;
		F32 delta = 1e-6;
		box.min.set(mPos.x-mRadius, mPos.y-mRadius, mPos.z - delta);
		box.max.set(mPos.x+mRadius, mPos.y+mRadius, mPos.z + delta);
		if (!frustum.boxInFrustum(box))
		{
			return false;
		}
	}


	if(mRefresh)
	{
		mPartition.clear();

		// 方向固定从上倒下
		Point3F dir(0,0,-1);
		gVectorProjector.buildPartition( mPos + Point3F(0,0,mProjectLen/2.0f), dir, mRadius/2, mProjectLen, smProjectMask, mPartition, mPartitionVerts, &mLightToWorld);
		mRefresh = false;

		// 没有投影到任何东西，就不用渲染了
		if (mPartition.empty())
			return false;

		mObjBox.min.set(-0, -0, -0);
		mObjBox.max.set( 0,  0,  0);

		// now set up tverts & colors
		mVBuffer.set(GFX, mPartitionVerts.size(), GFXBufferTypeStatic);
		mVBuffer.lock();

		// 旋转贴图的角度
		EulerF Rot(0,mRotZ,0);
		MatrixF texMat(Rot);

		// 设置顶点信息
		mColor.alpha = mClampF(255.0f * fade *  mAlpha, 0, 255);
		F32 invRadius = 1.0f / (mRadius/2);
		for (S32 i=0; i<mPartitionVerts.size(); i++)
		{
			Point3F vert = mPartitionVerts[i];
			vert.y = getMax(0.0f, vert.y-0.02f);
			mVBuffer[i].point.set(vert);
			mVBuffer[i].color.set(mColor);

			mObjBox.min.setMin(vert);
			mObjBox.max.setMax(vert);

			// 改变纹理坐标
			if(mDecalMode & Rotation)
				texMat.mulP(vert);
			mVBuffer[i].texCoord.set(0.5f + 0.5f * vert.x * invRadius, 0.5f + 0.5f * vert.z * invRadius);
		};

		mVBuffer.unlock();
	}
	// 没有投影到任何东西，就不用渲染了
	if (mPartition.empty())
		return false;

	return true;
}

void ProjectDecalInstance::renderProjectDecal()
{
	if(!mRender || mProjectDecalData->mTextures.size() <= curIndex)
		return;

	GFX->pushWorldMatrix();
	MatrixF world = GFX->getWorldMatrix();
	world.mul(mLightToWorld);
	GFX->setWorldMatrix(world);

	GFX->setTexture(0, mProjectDecalData->mTextures[curIndex]);
	GFX->setVertexBuffer(mVBuffer);
	GFX->setupGenericShaders( GFXDevice::GSModColorTexture );

	for(U32 p=0; p<mPartition.size(); p++)
		GFX->drawPrimitive(GFXTriangleFan, mPartition[p].vertexStart, (mPartition[p].vertexCount - 2));

	GFX->popWorldMatrix();
}


//---------------------------------------------------------------------------------
// ProjectDecalManager

ProjectDecalManager::ProjectDecalManager()
{
	mTypeMask |= DecalManagerObjectType;

	mObjBox.min.set(-1e7, -1e7, -1e7);
	mObjBox.max.set( 1e7,  1e7,  1e7);
	mWorldBox.min.set(-1e7, -1e7, -1e7);
	mWorldBox.max.set( 1e7,  1e7,  1e7);

	mFreePool = NULL;
	VECTOR_SET_ASSOCIATION(mProjectDecalQueue);
	VECTOR_SET_ASSOCIATION(mFreePoolBlocks);
	mRenderQueue = false;
}

ProjectDecalManager::~ProjectDecalManager()
{
	mFreePool = NULL;
	for (S32 i = 0; i < mFreePoolBlocks.size(); i++)
	{
		delete [] mFreePoolBlocks[i];
	}
	mProjectDecalQueue.clear();
}

ProjectDecalInstance* ProjectDecalManager::allocateDecalInstance()
{
	if (mFreePool == NULL)
	{
		// Allocate a new block of decals
		mFreePoolBlocks.push_back(new ProjectDecalInstance[csmFreePoolBlockSize]);

		// Init them onto the free pool chain
		ProjectDecalInstance* pNewInstances = mFreePoolBlocks.last();
		for (U32 i = 0; i < csmFreePoolBlockSize - 1; i++)
			pNewInstances[i].next = &pNewInstances[i + 1];
		pNewInstances[csmFreePoolBlockSize - 1].next = NULL;
		mFreePool = pNewInstances;
	}
	AssertFatal(mFreePool != NULL, "Error, should always have a free pool available here!");

	ProjectDecalInstance* pRet = mFreePool;
	mFreePool = pRet->next;
	pRet->next = NULL;
	return pRet;
}

ProjectDecalInstance*  ProjectDecalManager::getDecal(U32 ownerid)
{
	for(U32 i=0; i<mProjectDecalQueue.size(); i++)
	{
		ProjectDecalInstance *inst = mProjectDecalQueue[i];
		if(inst->ownerId == ownerid)
		{
			return inst;
		}
	}

	return NULL;
}

ProjectDecalInstance*  ProjectDecalManager::addDecal(U32 mode,
													 ProjectDecalData* pData,
													 const Point3F& pos, U32& id,
													 F32 rotZ /* = 0 */,
													 F32 radius /* = 0 */,
													 F32 shadowLen /* = 0 */,
													 F32 fadeVal /* = 0 */, bool byhand)
{
	if(!pData->mInit)
		pData->InitTexture();

	if(id==0xFFFFFFFF)
		id = ++mGlobeId;

	ProjectDecalInstance* newDecal = allocateDecalInstance();
	newDecal->mProjectDecalData = pData;
	newDecal->mDecalMode = mode;
	newDecal->mPos = pos;
	newDecal->mRotZ = rotZ;
	newDecal->mRadius = radius;
	newDecal->mProjectLen = shadowLen;
	newDecal->fade = fadeVal;
	newDecal->ownerId = id;
	newDecal->mRefresh = true;
	newDecal->mRender = true;
	newDecal->mNeedRender = true;
	newDecal->allocTime = Platform::getVirtualMilliseconds();
	newDecal->lastTime1 = Platform::getVirtualMilliseconds();
	newDecal->lastTime2 = Platform::getVirtualMilliseconds();
	newDecal->lastTime3 = Platform::getVirtualMilliseconds();
	newDecal->lastTimeAnimate = 0;
	newDecal->mByHand = byhand;

	if(newDecal->mProjectDecalData->mMaxAlpha > newDecal->mProjectDecalData->mAlpha)
	{
		newDecal->alphaLoopDir = true;
		newDecal->mMinAlpha = newDecal->mProjectDecalData->mAlpha;
		newDecal->mMaxAlpha = newDecal->mProjectDecalData->mMaxAlpha;
	}
	else
	{
		newDecal->mMinAlpha = newDecal->mProjectDecalData->mMaxAlpha;
		newDecal->mMaxAlpha = newDecal->mProjectDecalData->mAlpha;
	}

	if(newDecal->mProjectDecalData->mMaxDiameter > newDecal->mProjectDecalData->mDiameterScale)
	{
		newDecal->scaleLoopDir = true;
		newDecal->mMinDiameter = newDecal->mProjectDecalData->mDiameter;
		newDecal->mMaxDiameter = newDecal->mProjectDecalData->mMaxDiameter;
	}
	else
	{
		newDecal->mMinDiameter = newDecal->mProjectDecalData->mMaxDiameter;
		newDecal->mMaxDiameter = newDecal->mProjectDecalData->mDiameter;

	}

	newDecal->mDiameterFinished = false;
	newDecal->mAlphaFinished = false;
	newDecal->timeResetFlag = false;

	mProjectDecalQueue.push_back(newDecal); 
	
	return newDecal;
}

void ProjectDecalManager::freeDecalInstance(ProjectDecalInstance* trash)
{
	AssertFatal(trash != NULL, "Error, no trash pointer to free!");

	trash->next = mFreePool;
	mFreePool = trash;
}


void ProjectDecalManager::dataDeleted(ProjectDecalData *data)
{
	for(S32 i = mProjectDecalQueue.size() - 1; i >= 0; i--)
	{
		ProjectDecalInstance *inst = mProjectDecalQueue[i];
		if(inst->mProjectDecalData == data)
		{
			freeDecalInstance(inst);
			mProjectDecalQueue.erase(U32(i));
		}
	}
}

void ProjectDecalManager::consoleInit()
{
	Con::addVariable("$pref::decalsOn",				TypeBool, &smDecalsOn);
	Con::addVariable("$pref::Decal::maxNumDecals",	TypeS32, &smMaxNumDecals);
	Con::addVariable("$pref::Decal::decalTimeout",	TypeS32, &smDecalTimeout);
}

bool ProjectDecalManager::prepRenderImage(SceneState* state, const U32 stateKey,
								   const U32 /*startZone*/, const bool /*modifyBaseState*/)
{
	if (!smDecalsOn) return false;

	mRenderQueue = false;
	if (isLastState(state, stateKey))
		return false;
	setLastState(state, stateKey);

	if (mProjectDecalQueue.size() == 0)
		return false;

	// This should be sufficient for most objects that don't manage zones, and
	//  don't need to return a specialized RenderImage...
	RenderInst *ri = gRenderInstManager.allocInst();
	ri->obj = this;
	ri->state = state;
	ri->type = RenderInstManager::RIT_Decal;
	//ri->calcSortPoint(this, state->getCameraPosition());
	gRenderInstManager.addInst(ri);

	U32 currMs = Platform::getVirtualMilliseconds();
	
	ProjectDecalInstance* curInstance = NULL;
	for (S32 i = mProjectDecalQueue.size() - 1; i >= 0; i--)
	{
		curInstance = mProjectDecalQueue[i];

		U32 age = currMs - curInstance->allocTime;
		// 淡出效果
		if(!(curInstance->mDecalMode & ProjectDecalInstance::Permanent))
		{
			U32 timeout = curInstance->mProjectDecalData->lifeSpan;
			if (age > timeout)
			{
				freeDecalInstance(curInstance);
				mProjectDecalQueue.erase(i);
				continue;
			}
			else if (age > ((3 * timeout) / 4))
			{
				curInstance->fade = 1.0f - (F32(age - ((3 * timeout) / 4)) / F32(timeout / 4));
			}
			else
			{
				curInstance->fade = 1.0f;
			}
		}
		// 纹理动画
		if(curInstance->mProjectDecalData->mTextures.size() > 1)
			curInstance->curIndex = (age/curInstance->mProjectDecalData->mInterval)%curInstance->mProjectDecalData->mTextures.size();

		if(curInstance->mProjectDecalData->mRot != 0.0f)
		{
			curInstance->mRotZ += F32(currMs - curInstance->lastTime1) * curInstance->mProjectDecalData->mRot;
			curInstance->lastTime1 = currMs;
			curInstance->mRefresh = true;
		}

		// 缩放动画
		if(!curInstance->mDiameterFinished && curInstance->mProjectDecalData->mDiameterScale!=0.0f)
		{
			if(curInstance->scaleLoopDir)
			{
				curInstance->mRadius += F32(currMs - curInstance->lastTime2) * curInstance->mProjectDecalData->mDiameterScale;
				if(curInstance->mRadius >= curInstance->mMaxDiameter)
				{
					curInstance->mRadius = curInstance->mMaxDiameter;
					curInstance->mDiameterFinished = true;
				}
			}
			else
			{
				curInstance->mRadius -= F32(currMs - curInstance->lastTime2) * curInstance->mProjectDecalData->mDiameterScale;
				if(curInstance->mRadius < curInstance->mMinDiameter)
				{
					curInstance->mRadius = curInstance->mMinDiameter;
					curInstance->mDiameterFinished = true;
				}
			}

			curInstance->mProjectLen = curInstance->mRadius * 2;
			curInstance->lastTime2 = currMs;
			curInstance->mRefresh = true;
		}

		//alpha动画
		if(!curInstance->mAlphaFinished && curInstance->mProjectDecalData->mAlphaScale!=0.0f)
		{
			if(curInstance->alphaLoopDir)
			{
				curInstance->mAlpha += F32(currMs - curInstance->lastTime3) * curInstance->mProjectDecalData->mAlphaScale;
				if(curInstance->mAlpha > curInstance->mMaxAlpha)
				{
					curInstance->mAlpha = curInstance->mMaxAlpha;
					curInstance->mAlphaFinished = true;
				}
			}
			else
			{
				curInstance->mAlpha -= F32(currMs - curInstance->lastTime3) * curInstance->mProjectDecalData->mAlphaScale;
				if(curInstance->mAlpha < curInstance->mMinAlpha)
				{
					curInstance->mAlpha = curInstance->mMinAlpha;
					curInstance->mAlphaFinished = true;
				}
			}

			curInstance->lastTime3 = currMs;
			curInstance->mRefresh = true;
		}
		
		//后续动画判断
		bool doLoop = true;

		if(curInstance->mProjectDecalData->mLoopTimeStep!=0)
		{
			if(curInstance->mDiameterFinished && curInstance->mAlphaFinished)
			{
				if(!curInstance->timeResetFlag)
				{
					curInstance->lastTimeAnimate = currMs;
					curInstance->timeResetFlag = true;
				}

				if((currMs - curInstance->lastTimeAnimate) >= curInstance->mProjectDecalData->mLoopTimeStep)
				{
					curInstance->timeResetFlag = false;
					curInstance->lastTimeAnimate = 0;
				}
				else
				{
					doLoop = false;
				}
			}
			else
				doLoop = false;
		}
		
		if(doLoop)
		{
			if(curInstance->mProjectDecalData->mLoop)
			{
				if(curInstance->mDiameterFinished)
				{
					if(curInstance->mProjectDecalData->mLoopType==0)
					{
						curInstance->mRadius = curInstance->mProjectDecalData->mDiameter;
					}
					else
					{
						curInstance->scaleLoopDir = !curInstance->scaleLoopDir;
					}
					curInstance->mDiameterFinished = false;
					curInstance->lastTime2 = currMs;
				}

				if(curInstance->mAlphaFinished)
				{
					if(curInstance->mProjectDecalData->mLoopType==0)
					{
						curInstance->mAlpha = curInstance->mProjectDecalData->mAlpha;
					}
					else
					{
						curInstance->alphaLoopDir = !curInstance->alphaLoopDir;
					}
					curInstance->mAlphaFinished = false;
					curInstance->lastTime3 = currMs;
				}
			}
		}

		curInstance->mRender = curInstance->mNeedRender && curInstance->prepare();
		mRenderQueue = mRenderQueue || curInstance->mRender;
	}

	if (mQueueDirty == true)
	{
		// Sort the decals based on the data pointers...
		dQsort( mProjectDecalQueue.address(),
				mProjectDecalQueue.size(),
				sizeof(DecalInstance*),
				cmpProjectDecalInstance);
		mQueueDirty = false;
	}

	return false;
}

#ifdef STATEBLOCK
void ProjectDecalManager::renderObject(SceneState* state, RenderInst *)
{
	//GFX->setZBias(0);
	///*
	if (!smDecalsOn || !mRenderQueue) return;

	MatrixF projection = GFX->getProjectionMatrix();
	RectI viewport = GFX->getViewport();
	GFX->pushWorldMatrix();
	GFX->disableShaders();

	//
	GFX->setTexture(1, NULL);

	AssertFatal(mZWriteSB, "ProjectDecalManager::renderObject -- mZWriteSB cannot be NULL.");
	mZWriteSB->capture();
	AssertFatal(mSetSB, "ProjectDecalManager::renderObject -- mSetSB cannot be NULL.");
	mSetSB->apply();

	for (S32 i=0; i<mProjectDecalQueue.size(); i++)
		mProjectDecalQueue[i]->renderProjectDecal();

	GFX->setVertexBuffer(NULL);
	GFX->setTexture(0, NULL);

	AssertFatal(mZWriteSB, "ProjectDecalManager::renderObject -- mZWriteSB cannot be NULL.");
	mZWriteSB->apply();
	GFX->setBaseRenderState();

	GFX->popWorldMatrix();
	GFX->setViewport(viewport);
	GFX->setProjectionMatrix(projection);
	//	*/
}

#else

void ProjectDecalManager::renderObject(SceneState* state, RenderInst *)
{
	//GFX->setZBias(0);
///*
	if (!smDecalsOn || !mRenderQueue) return;

	MatrixF projection = GFX->getProjectionMatrix();
	RectI viewport = GFX->getViewport();
	GFX->pushWorldMatrix();
	GFX->disableShaders();

	//
	GFX->setTexture(1, NULL);

	GFX->setTextureStageColorOp(0, GFXTOPModulate);      
	F32 depthbias = -0.00005f;
	GFX->setZBias(*((U32 *)&depthbias));
	bool zwrite = GFX->getRenderState(GFXRSZWriteEnable);

	GFX->setCullMode(GFXCullNone);
	GFX->setLightingEnable(false);

	GFX->setZEnable(true);
	GFX->setZWriteEnable(false);

	GFX->setAlphaBlendEnable(true);
	GFX->setSrcBlend(GFXBlendSrcAlpha);
	GFX->setDestBlend(GFXBlendOne);
	GFX->setAlphaTestEnable(false);

	U32 TexAddressModeU = GFX->getSamplerState(0, GFXSAMPAddressU);
	U32 TexAddressModeV = GFX->getSamplerState(0, GFXSAMPAddressV);
	GFX->setTextureStageAddressModeU( 0, GFXAddressClamp );
	GFX->setTextureStageAddressModeV( 0, GFXAddressClamp );
    GFX->setTextureStageColorOp(1, GFXTOPDisable);

	for (S32 i=0; i<mProjectDecalQueue.size(); i++)
		mProjectDecalQueue[i]->renderProjectDecal();

	GFX->setTextureStageAddressModeU( 0, (GFXTextureAddressMode)TexAddressModeU );
	GFX->setTextureStageAddressModeV( 0, (GFXTextureAddressMode)TexAddressModeV );
	GFX->setAlphaBlendEnable(false);
	GFX->setZWriteEnable(zwrite);
	GFX->setVertexBuffer(NULL);
	GFX->setTexture(0, NULL);

	GFX->setZBias(0);
	GFX->setBaseRenderState();

	GFX->setTextureStageColorOp(0, GFXTOPDisable);
	GFX->setTextureStageColorOp(1, GFXTOPDisable);

	GFX->popWorldMatrix();
	GFX->setViewport(viewport);
	GFX->setProjectionMatrix(projection);
//	*/
}

#endif


void ProjectDecalManager::findSpace()
{
	S32 besttime = S32_MAX;
	U32 bestindex = 0;
	ProjectDecalInstance *bestdecal = NULL;

	U32 time = Platform::getVirtualMilliseconds();

	for(U32 i=0; i<mProjectDecalQueue.size(); i++)
	{
		ProjectDecalInstance *inst = mProjectDecalQueue[i];
		U32 age = time - inst->allocTime;
		U32 timeleft = inst->mProjectDecalData->lifeSpan - age;
		if(besttime > timeleft)
		{
			besttime = timeleft;
			bestindex = i;
			bestdecal = inst;
		}
	}

	AssertFatal((bestdecal), "No good decals?");

	mProjectDecalQueue.erase_fast(bestindex);
	freeDecalInstance(bestdecal);
}

void ProjectDecalManager::ageDecal(U32 ownerid)
{
	for(U32 i=0; i<mProjectDecalQueue.size(); i++)
	{
		ProjectDecalInstance *inst = mProjectDecalQueue[i];
		if(inst->ownerId == ownerid)
		{
			freeDecalInstance(inst);
			mProjectDecalQueue.erase(U32(i));
		}
	}
}

void ProjectDecalManager::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setRenderState(GFXRSDepthBias, -0.00005f);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressClamp);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->endStateBlock(mSetSB);

	//mZWriteSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZWriteEnable, false);	
	GFX->setSamplerState(0, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(0, GFXSAMPAddressV, GFXAddressClamp);
	GFX->endStateBlock(mZWriteSB);


	//mClearSB
	GFX->beginStateBlock();
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPDisable);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->setRenderState(GFXRSDepthBias, 0.0f);
	//GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	//GFX->setTextureStageColorOp( 0, GFXTOPDisable );
	//GFX->setTextureStageColorOp( 1, GFXTOPDisable );
	GFX->setRenderState(GFXRSDepthBias, 0);
	GFX->endStateBlock(mClearSB);
}


void ProjectDecalManager::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mZWriteSB)
	{
		mZWriteSB->release();
	}

	if (mClearSB)
	{
		mClearSB->release();
	}
}

void ProjectDecalManager::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		mZWriteSB = new GFXD3D9StateBlock;
		mClearSB = new GFXD3D9StateBlock;

		resetStateBlock();
	}


}

void ProjectDecalManager::shutdown()
{
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mZWriteSB);
	SAFE_DELETE(mClearSB);
}

