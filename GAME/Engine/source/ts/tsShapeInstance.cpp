//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _POWERCONFIG_H_
#include "platform/EngineConfig.h"
#endif

#include "ts/tsShapeInstance.h"
#include "ts/tsLastDetail.h"
#include "console/consoleTypes.h"
#include "ts/tsDecal.h"
#include "platform/profiler.h"
#include "core/frameAllocator.h"
#include "gfx/gfxDevice.h"
#include "gfx/gfxCanon.h"
#include "materials/sceneData.h"
#include "materials/matInstance.h"
#include "sceneGraph/sceneGraph.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
#include "gfx/gfxCardProfile.h"

//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* TSShapeInstance::mSetSB = NULL;
GFXStateBlock* TSShapeInstance::mClearSB = NULL;

TSShapeInstance::RenderData   TSShapeInstance::smRenderData;
MatrixF *                     TSShapeInstance::ObjectInstance::smTransforms = NULL;
MatrixF *                     TSShapeInstance::ObjectInstance::smRenderTransforms = NULL;
S32                           TSShapeInstance::smMaxSnapshotScale = 2;
bool                          TSShapeInstance::smNoRenderTranslucent = false;
bool                          TSShapeInstance::smNoRenderNonTranslucent = false;
F32                           TSShapeInstance::smDetailAdjust = 1.0f;
F32                           TSShapeInstance::smScreenError = 5.0f;
bool                          TSShapeInstance::smFogExemptionOn = false;
S32                           TSShapeInstance::smNumSkipRenderDetails = 0;
bool                          TSShapeInstance::smSkipFirstFog = false;
bool                          TSShapeInstance::smSkipFog = false;

//Vector<QuatF>                 TSShapeInstance::smNodeCurrentRotations(__FILE__, __LINE__);
//Vector<Point3F>               TSShapeInstance::smNodeCurrentTranslations(__FILE__, __LINE__);
//Vector<F32>                   TSShapeInstance::smNodeCurrentUniformScales(__FILE__, __LINE__);
//Vector<Point3F>               TSShapeInstance::smNodeCurrentAlignedScales(__FILE__, __LINE__);
//Vector<TSScale>               TSShapeInstance::smNodeCurrentArbitraryScales(__FILE__, __LINE__);
//
//Vector<TSThread*>             TSShapeInstance::smRotationThreads(__FILE__, __LINE__);
//Vector<TSThread*>             TSShapeInstance::smTranslationThreads(__FILE__, __LINE__);
//Vector<TSThread*>             TSShapeInstance::smScaleThreads(__FILE__, __LINE__);

GFXTexHandle				  TSShapeInstance::smDefaultVertexTex = NULL;
GFXVertexBufferHandle<GFXVertexPT> *TSShapeInstance::smQuadVB = NULL;
GFXVertexBufferHandle<GFXVertexPN4R> *TSShapeInstance::smNullVB = NULL;
GFXVertexBufferHandle<GFXVertexPN4R> *TSShapeInstance::smDummyVB = NULL;

bool						  TSShapeInstance::supportsVertTex = false;
bool						  TSShapeInstance::supportsR2VB = false;
bool						  TSShapeInstance::isNormalSkin = false;


//-------------------------------------------------------------------------------------
// constructors, destructors, initialization
//-------------------------------------------------------------------------------------

TSShapeInstance::TSShapeInstance(const Resource<TSShape> & shape, bool loadMaterials)
{
   shadowDirty = true;
   mIsAnimationInited = false;
   mDirtyFlg = 0;

   VECTOR_SET_ASSOCIATION(mMeshObjects);
   VECTOR_SET_ASSOCIATION(mIflMaterialInstances);
   VECTOR_SET_ASSOCIATION(mNodeTransforms);
   VECTOR_SET_ASSOCIATION(mNodeReferenceRotations);
   VECTOR_SET_ASSOCIATION(mNodeReferenceTranslations);
   VECTOR_SET_ASSOCIATION(mNodeReferenceUniformScales);
   VECTOR_SET_ASSOCIATION(mNodeReferenceScaleFactors);
   VECTOR_SET_ASSOCIATION(mNodeReferenceArbitraryScaleRots);
   VECTOR_SET_ASSOCIATION(mThreadList);
   VECTOR_SET_ASSOCIATION(mTransitionThreads);

   hShape = shape;
   mShape = hShape;
   mData  = 0;
   mDirtyFlags = 0;
   buildInstanceData(shape, loadMaterials);
}

TSShapeInstance::TSShapeInstance(TSShape * _shape, bool loadMaterials)
{
   shadowDirty = true;
   mIsAnimationInited = false;
   mDirtyFlg = 0;
   mData  = 0;
   mDirtyFlags = 0;

   VECTOR_SET_ASSOCIATION(mMeshObjects);
   VECTOR_SET_ASSOCIATION(mIflMaterialInstances);
   VECTOR_SET_ASSOCIATION(mNodeTransforms);
   VECTOR_SET_ASSOCIATION(mNodeReferenceRotations);
   VECTOR_SET_ASSOCIATION(mNodeReferenceTranslations);
   VECTOR_SET_ASSOCIATION(mNodeReferenceUniformScales);
   VECTOR_SET_ASSOCIATION(mNodeReferenceScaleFactors);
   VECTOR_SET_ASSOCIATION(mNodeReferenceArbitraryScaleRots);
   VECTOR_SET_ASSOCIATION(mThreadList);
   VECTOR_SET_ASSOCIATION(mTransitionThreads);

   mShape = _shape;
   buildInstanceData(hShape, loadMaterials);
}

TSShapeInstance::~TSShapeInstance()
{
   S32 i;
   for (i=0; i<mMeshObjects.size(); i++)
      destructInPlace(&mMeshObjects[i]);

   while (mThreadList.size())
      destroyThread(mThreadList.last());

   setMaterialList(NULL);

   if (0 != mDirtyFlags)
    delete [] mDirtyFlags;

   mDirtyFlags = 0;
}

void TSShapeInstance::boneAnimationInit()
{
	//Ray: 初始化默认顶点纹理
	supportsVertTex = GFX->getCardProfiler()->queryProfile("allowRGBA32FBitVertexTextures", false);
	supportsR2VB = GFX->getCardProfiler()->queryProfile("allowATIR2VB", false);

	if(supportsVertTex)
	{
		 smDefaultVertexTex.set(4,1,GFXFormatR32G32B32A32F,&GFXDefaultVertexMapProfile); 
		 GFXLockedRect *pRect = smDefaultVertexTex.lock();
		 MatrixF I(true);
		 memcpy(pRect->bits,(F32*)I,sizeof(F32)*16);
		 smDefaultVertexTex.unlock();
	}
	else
	if(supportsR2VB)
	{
		smQuadVB = new GFXVertexBufferHandle<GFXVertexPT>;
		smNullVB = new GFXVertexBufferHandle<GFXVertexPN4R>;
		smDummyVB = new GFXVertexBufferHandle<GFXVertexPN4R>;

		smQuadVB->set(GFX,4,GFXBufferTypeStatic);
		GFXVertexPT *vbVerts = smQuadVB->lock();
		vbVerts[0].point.set( -1,-1, 0 );
		vbVerts[1].point.set(  1,-1, 0 );
		vbVerts[2].point.set( -1, 1, 0 );
		vbVerts[3].point.set(  1, 1, 0 );
		vbVerts[0].texCoord.set( 0,  0 );
		vbVerts[1].texCoord.set( 1,  0 );
		vbVerts[2].texCoord.set( 0,  1 );
		vbVerts[3].texCoord.set( 1,  1 );
		smQuadVB->unlock();
		smQuadVB->declare(GFXDeclareP3T,ELEMENTOF(GFXDeclareP3T));

		smNullVB->set(GFX,0,GFXBufferTypeNull,1);
		smDummyVB->set(GFX,2,GFXBufferTypeStatic,1);
	}
}

void TSShapeInstance::init()
{
   Con::addVariable("$pref::TS::detailAdjust", TypeF32, &smDetailAdjust);
   Con::addVariable("$pref::TS::skipLoadDLs", TypeS32, &TSShape::smNumSkipLoadDetails);
   Con::addVariable("$pref::TS::skipRenderDLs", TypeS32, &smNumSkipRenderDetails);
   Con::addVariable("$pref::TS::skipFirstFog", TypeBool, &smSkipFirstFog);
   Con::addVariable("$pref::TS::screenError", TypeF32, &smScreenError);
}

void TSShapeInstance::destroy()
{
	smDefaultVertexTex = NULL;
	SAFE_DELETE(smQuadVB);
	SAFE_DELETE(smNullVB);
	SAFE_DELETE(smDummyVB);
}

void TSShapeInstance::buildInstanceData(const Resource<TSShape> & _shape, bool loadMaterials)
{
   if (0 == _shape)
       return;

   S32 i;

   //mShape = _shape;
   hShape = _shape;
   mShape = hShape;

   debrisRefCount = 0;

   mCurrentDetailLevel = 0;
   mCurrentIntraDetailLevel = 1.0f;

   // all triggers off at start
   mTriggerStates = 0;

   //
   mAlphaAlways = false;
   mAlphaAlwaysValue = 1.0f;

   mUseOverrideTexture = false;

   // material list...
   mMaterialList = NULL;
   mOwnMaterialList = false;

   //
   mData = 0;
   mScaleCurrentlyAnimated = false;

   if(loadMaterials)
   {
      setMaterialList(mShape->materialList);
   }

   // set up node data
   S32 numNodes = mShape->nodes.size();
   mNodeTransforms.setSize(numNodes);
   mRenderNodeTransforms.setSize(numNodes);

   // add objects to trees
   S32 numObjects = mShape->objects.size();
   mMeshObjects.setSize(numObjects);
   for (i=0; i<numObjects; i++)
   {
      const TSObject * obj = &mShape->objects[i];
      MeshObjectInstance * objInst = &mMeshObjects[i];

      // call objInst constructor
      constructInPlace(objInst);

      // hook up the object to it's node
      objInst->nodeIndex = obj->nodeIndex;

      // set up list of meshes
      if (obj->numMeshes)
         objInst->meshList = &mShape->meshes[obj->startMeshIndex];
      else
         objInst->meshList = NULL;

      objInst->object = obj;
   }

   // construct ifl material objects

   if(loadMaterials)
   {
      for (i=0; i<mShape->iflMaterials.size(); i++)
      {
         mIflMaterialInstances.increment();
         mIflMaterialInstances.last().iflMaterial = &mShape->iflMaterials[i];
         mIflMaterialInstances.last().frame = -1;
      }
   }

   // set up subtree data
   S32 ss = mShape->subShapeFirstNode.size(); // we have this many subtrees
   mDirtyFlags = new U32[ss];

   mGroundThread = NULL;
   mCurrentDetailLevel = 0;

   //初始结点的初始化，这个函数本来是在未加动画前做的
   Vector<TSThread*> t = mThreadList;
   mThreadList.clear();
   animateSubtrees();
   mThreadList = t;

   setDirty(mDirtyFlg);

   // Construct billboards if not done already
   if(loadMaterials)
      ((TSShape *) mShape)->setupBillboardDetails(this);
}

// Initialize material instances in material list
void TSShapeInstance::initMatInstances()
{
   SceneGraphData sgData;
   sgData.setDefaultLights();
   sgData.useLightDir = true;
   sgData.useFog = SceneGraph::renderFog;

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

		  matInst->init( sgData, vf );
      }
   }
}

void TSShapeInstance::setMaterialList(TSMaterialList * ml)
{
    if (0 == mShape)
        return;

   // get rid of old list
   if (mOwnMaterialList)
      delete mMaterialList;
   mMaterialList = ml;
   mOwnMaterialList = false;

   if (mMaterialList && StringTable) // material lists need the string table to load...
   {
      // read ifl materials if necessary -- this is here rather than in shape because we can't open 2 files at once :(
      if (mShape->materialList == mMaterialList)
         ((TSShape*)mShape)->readIflMaterials(hShape.getFilePath());

      mMaterialList->load(MeshTexture,hShape.getFilePath(),true);
      mMaterialList->mapMaterials();

      initMatInstances();
   }
}


void TSShapeInstance::loadMaterialList()
{
	if(mMaterialList)
		mMaterialList->load(MeshTexture,hShape.getFilePath(),true);
	else
	{
		mShape->preloadMaterialList();
		setMaterialList(mShape->materialList);
	}
}

void TSShapeInstance::freeMaterialList()
{
	if(mMaterialList)
		mMaterialList->unload();
}

void TSShapeInstance::cloneMaterialList()
{
   if (mOwnMaterialList || 0 == mMaterialList)
      return;

   mMaterialList = new TSMaterialList(mMaterialList);
   mOwnMaterialList = true;

   initMatInstances();
}

void TSShapeInstance::cloneMaterialList(TSMaterialList * ml)
{
	// get rid of old list
	if (mOwnMaterialList)
		delete mMaterialList;

	mMaterialList = NULL;
	mOwnMaterialList = false;

	if(!ml)	return;

	mMaterialList = new TSMaterialList(ml);
	if (mMaterialList && StringTable) // material lists need the string table to load...
	{
		// read ifl materials if necessary -- this is here rather than in shape because we can't open 2 files at once :(
		if (mShape->materialList == mMaterialList)
			((TSShape*)mShape)->readIflMaterials(hShape.getFilePath());

		mMaterialList->load(MeshTexture,hShape.getFilePath(),true);
		strncpy( mMaterialList->mPath, hShape.getFilePath(), 128 );
		mMaterialList->mapMaterials();

		mOwnMaterialList = true;
		initMatInstances();
	}
}

static bool makeSkinPath(char* buffer, U32 bufferLength, const char* resourcePath,const char* oldSkinName, char newSkinTag)
{
   int Len = dStrlen(oldSkinName);
   if(oldSkinName[Len-2]=='_')  //Ray:贴图套数为 xxxxx_A开始
   {
	   dStrcpy(buffer, bufferLength, resourcePath);
	   dStrcat(buffer, bufferLength, "/");
	   dStrcat(buffer, bufferLength, oldSkinName);
	   if(newSkinTag)
	   {
		   Len = dStrlen(buffer);
		   buffer[Len-1] = newSkinTag;
	   }
	   else
	   {
		   buffer[Len-2] = 0;
	   }
   }
   else
   {
	   dStrcpy(buffer, bufferLength, resourcePath);
	   dStrcat(buffer, bufferLength, "/");
	   dStrcat(buffer, bufferLength, oldSkinName);
	   if(newSkinTag)
	   {
		   Len = dStrlen(buffer);
		   buffer[Len] = '_';
		   buffer[Len+1] = newSkinTag;
		   buffer[Len+2] = 0;
	   }
   }

   return true;
}


void TSShapeInstance::reSkin(char newSkinTag)
{
   //if(!(newSkinTag > 'a' && newSkinTag < 'z') && !(newSkinTag > 'A' && newSkinTag < 'Z'))
	  // return;
   static const int NAME_BUFFER_LENGTH = 256;
   static char pathName[NAME_BUFFER_LENGTH];

   if (ownMaterialList() == false)
      cloneMaterialList();

   const char* resourcePath = hShape.getFilePath();

   TSMaterialList* pMatList = getMaterialList();

   if (0 == pMatList)
       return;

   for (S32 j = 0; j < pMatList->mMaterialNames.size(); j++) 
   {
      const char* pName = pMatList->mMaterialNames[j];
      if (pName == NULL)
         continue;

      if(makeSkinPath(pathName, NAME_BUFFER_LENGTH, resourcePath,pName, newSkinTag))
		pMatList->setMaterial(j, pathName);	//Ray: 设置材质失败，不应该影响原来的材质
   }
}

void TSShapeInstance::reSkin(NetStringHandle &skin)
{
	//StringTableEntry skinTagStr = skin.getString();
	//if(skinTagStr && skinTagStr[0])
	//	reSkin(skinTagStr[0]);
}

void TSShapeInstance::renderQuery(const Point3F * objectScale)
{
	// if dl==-1, nothing to do
	if (mCurrentDetailLevel==-1)
		return;

	AssertFatal(mCurrentDetailLevel>=0 && mCurrentDetailLevel<mShape->details.size(),"TSShapeInstance::render");

	S32 i;

	const TSDetail * detail = &mShape->details[mCurrentDetailLevel];
	S32 ss = detail->subShapeNum;
	S32 od = detail->objectDetailNum;

	// set up static data
	setStatics(mCurrentDetailLevel,mCurrentIntraDetailLevel,objectScale);

	// if we're a billboard detail, draw it and exit
	if (ss<0)
	{
		if (!smNoRenderTranslucent)
			mShape->billboardDetails[mCurrentDetailLevel]->render(mAlphaAlways ? mAlphaAlwaysValue : 1.0f);
		return;
	}

	// run through the meshes
	smRenderData.currentTransform = NULL;
	smRenderData.sortSeed = 0;
	S32 start = smNoRenderNonTranslucent ? mShape->subShapeFirstTranslucentObject[ss] : mShape->subShapeFirstObject[ss];
	S32 end   = smNoRenderTranslucent ? mShape->subShapeFirstTranslucentObject[ss] : mShape->subShapeFirstObject[ss] + mShape->subShapeNumObjects[ss];
	for (i=start; i<end; i++)
	{
		smRenderData.currentObjectInstance = &mMeshObjects[i];
		// following line is handy for debugging, to see what part of the shape that it is rendering
		//const char *name = mShape->names[mMeshObjects[i].object->nameIndex];
		mMeshObjects[i].renderQuery(od);
	}

	// if we have a matrix pushed, pop it now
	if (smRenderData.currentTransform)
		GFX->popWorldMatrix();

	clearStatics();
}

//-------------------------------------------------------------------------------------
// Render & detail selection
//-------------------------------------------------------------------------------------
void TSShapeInstance::render(const Point3F * objectScale)
{
   if (mCurrentDetailLevel<0 || 0 == mShape)
      return;

   PROFILE_START(TSShapeInstanceRender);

   // alphaIn:  we start to alpha-in next detail level when intraDL > 1-alphaIn-alphaOut
   //           (finishing when intraDL = 1-alphaOut)
   // alphaOut: start to alpha-out this detail level when intraDL > 1-alphaOut
   // NOTE:
   //   intraDL is at 1 when if shape were any closer to us we'd be at dl-1,
   //   intraDL is at 0 when if shape were any farther away we'd be at dl+1
   F32 alphaOut = mShape->alphaOut[mCurrentDetailLevel];
   F32 alphaIn  = mShape->alphaIn[mCurrentDetailLevel];
   F32 saveAA = mAlphaAlways ? mAlphaAlwaysValue : 1.0f;

   if (mCurrentIntraDetailLevel>alphaIn+alphaOut)
      render(mCurrentDetailLevel,mCurrentIntraDetailLevel,objectScale);
   else if (mCurrentIntraDetailLevel>alphaOut)
   {
      // draw this detail level w/ alpha=1 and next detail level w/
      // alpha=1-(intraDl-alphaOut)/alphaIn

      // first draw next detail level
      if (mCurrentDetailLevel+1<mShape->details.size() && mShape->details[mCurrentDetailLevel+1].size>0.0f)
      {
         setAlphaAlways(saveAA * (alphaIn+alphaOut-mCurrentIntraDetailLevel)/alphaIn);
         render(mCurrentDetailLevel+1,0.0f,objectScale);
      }

      setAlphaAlways(saveAA);
      render(mCurrentDetailLevel,mCurrentIntraDetailLevel,objectScale);
   }
   else
   {
      // draw next detail level w/ alpha=1 and this detail level w/
      // alpha = 1-intraDL/alphaOut

      // first draw next detail level
      if (mCurrentDetailLevel+1<mShape->details.size() && mShape->details[mCurrentDetailLevel+1].size>0.0f)
         render(mCurrentDetailLevel+1,0.0f,objectScale);

      setAlphaAlways(saveAA * mCurrentIntraDetailLevel / alphaOut);
      render(mCurrentDetailLevel,mCurrentIntraDetailLevel,objectScale);
      setAlphaAlways(saveAA);
   }

   PROFILE_END(TSShapeInstanceRender);
}

bool TSShapeInstance::hasTranslucency()
{
   if(!mShape->details.size())
      return false;

   const TSDetail * detail = &mShape->details[0];
   S32 ss = detail->subShapeNum;

   return mShape->subShapeFirstTranslucentObject[ss] != mShape->subShapeFirstObject[ss] + mShape->subShapeNumObjects[ss];
}

bool TSShapeInstance::hasSolid()
{
   if(!mShape->details.size())
      return false;

   const TSDetail * detail = &mShape->details[0];
   S32 ss = detail->subShapeNum;

   return mShape->subShapeFirstTranslucentObject[ss] != mShape->subShapeFirstObject[ss];
}

void TSShapeInstance::render(S32 dl, F32 intraDL, const Point3F * objectScale)
{
   // if dl==-1, nothing to do
   if (dl==-1 || 0 == mShape)
      return;

   AssertFatal(dl>=0 && dl<mShape->details.size(),"TSShapeInstance::render");

   S32 i;

   const TSDetail * detail = &mShape->details[dl];
   S32 ss = detail->subShapeNum;
   S32 od = detail->objectDetailNum;

   // set up static data
   setStatics(dl,intraDL,objectScale);

   // if we're a billboard detail, draw it and exit
   if (ss<0)
   {
      if (!smNoRenderTranslucent)
         mShape->billboardDetails[dl]->render(mAlphaAlways ? mAlphaAlwaysValue : 1.0f);
      return;
   }

   // set up animating ifl materials
   for (i=0; i<mIflMaterialInstances.size(); i++)
   {
      IflMaterialInstance  * iflMaterialInstance = &mIflMaterialInstances[i];
      const TSShape::IflMaterial * iflMaterial = iflMaterialInstance->iflMaterial;
      mMaterialList->remap(iflMaterial->materialSlot, iflMaterial->firstFrame + iflMaterialInstance->frame);
   }

   // set up gl environment for drawing mesh materials

   PROFILE_START(TSShapeInstance_Render);
   // run through the meshes
   smRenderData.currentTransform = NULL;
   smRenderData.sortSeed = 0;
   S32 start = smNoRenderNonTranslucent ? mShape->subShapeFirstTranslucentObject[ss] : mShape->subShapeFirstObject[ss];
   S32 end   = smNoRenderTranslucent ? mShape->subShapeFirstTranslucentObject[ss] : mShape->subShapeFirstObject[ss] + mShape->subShapeNumObjects[ss];
   for (i=start; i<end; i++)
   {
      smRenderData.currentObjectInstance = &mMeshObjects[i];
      // following line is handy for debugging, to see what part of the shape that it is rendering
      //const char *name = mShape->names[mMeshObjects[i].object->nameIndex];
      mMeshObjects[i].render(od,mMaterialList);
   }

   // if we have a matrix pushed, pop it now
   if (smRenderData.currentTransform)
      GFX->popWorldMatrix();

   PROFILE_END(TSShapeInstance_Render);

   clearStatics();
}

void TSShapeInstance::setStatics(S32 dl, F32 intraDL, const Point3F * objectScale)
{
   smRenderData.objectScale = objectScale;
   smRenderData.detailLevel = dl;

   ObjectInstance::smTransforms = mNodeTransforms.address();
   ObjectInstance::smRenderTransforms = mRenderNodeTransforms.address();
   TSMesh::setShapeIns(this);

   smRenderData.currentObjectInstance = NULL;
}

void TSShapeInstance::clearStatics()
{
   ObjectInstance::smTransforms = NULL;
   ObjectInstance::smRenderTransforms = NULL;
   TSMesh::setShapeIns(NULL);

   smRenderData.currentObjectInstance = NULL;
}

S32 TSShapeInstance::getCurrentDetail()
{
   return mCurrentDetailLevel;
}

F32 TSShapeInstance::getCurrentIntraDetail()
{
   return mCurrentIntraDetailLevel;
}

void TSShapeInstance::setCurrentDetail(S32 dl, F32 intraDL)
{
   if (0 == mShape)
       return;

   mCurrentDetailLevel = dl;
   mCurrentIntraDetailLevel = intraDL>1.0f ? 1.0f : (intraDL<0.0f ? 0.0f : intraDL);

   // restrict chosen detail level by cutoff value
   S32 cutoff = getMin(smNumSkipRenderDetails,mShape->mSmallestVisibleDL);
   if (mCurrentDetailLevel>=0 && mCurrentDetailLevel<cutoff)
   {
      mCurrentDetailLevel = cutoff;
      mCurrentIntraDetailLevel = 1.0f;
   }
}

S32 TSShapeInstance::selectCurrentDetail(bool ignoreScale)
{
   if (0 == mShape)
       return 0;

   if (mShape->mSmallestVisibleDL>=0 && mShape->details[0].maxError>=0)
      // use new scheme
      return selectCurrentDetailEx(ignoreScale);

   MatrixF toCam = GFX->getWorldMatrix();
   Point3F p;
   toCam.mulP(mShape->center,&p);
   F32 dist = mDot(p,p);
   F32 scale = 1.0f;

   if (!ignoreScale)
   {
      // any scale?
      Point3F x,y,z;
      toCam.getRow(0,&x);
      toCam.getRow(1,&y);
      toCam.getRow(2,&z);
      F32 scalex = mDot(x,x);
      F32 scaley = mDot(y,y);
      F32 scalez = mDot(z,z);
      scale = scalex;
      if (scaley > scale)
         scale = scaley;
      if (scalez > scale)
         scale = scalez;
   }

   dist /= scale;
   dist = mSqrt(dist);

   RectI viewport = GFX->getViewport();
   F32 pixelScale = viewport.extent.x * 1.6f / 640.0f;
   F32 pixelRadius = GFX->projectRadius(dist,mShape->radius) * pixelScale * smDetailAdjust;

   return selectCurrentDetail(pixelRadius);
}

S32 TSShapeInstance::selectCurrentDetailEx(bool ignoreScale)
{
   if (0 == mShape)
       return 0;

   Point3F p;
   MatrixF toCam = GFX->getWorldMatrix();
   toCam.mulP(mShape->center,&p);
   F32 dist = mDot(p,p);
   F32 scale = 1.0f;
   if (!ignoreScale)
   {
      // any scale?
      Point3F x,y,z;
      toCam.getRow(0,&x);
      toCam.getRow(1,&y);
      toCam.getRow(2,&z);
      F32 scalex = mDot(x,x);
      F32 scaley = mDot(y,y);
      F32 scalez = mDot(z,z);
      scale = scalex;
      if (scaley > scale)
         scale = scaley;
      if (scalez > scale)
         scale = scalez;
   }
   dist /= scale;
   dist = mSqrt(dist);

   // find tolerance
   RectI viewport = GFX->getViewport();
   F32 pixelScale = viewport.extent.x * 1.6f / 640.0f;
   F32 proj = GFX->projectRadius(dist,1.0f) * pixelScale; // pixel size of 1 meter at given distance
   if ( smFogExemptionOn )
      return selectCurrentDetailEx(F32(0.001)/proj);
   else
      return selectCurrentDetailEx(smScreenError/proj);
}

S32 TSShapeInstance::selectCurrentDetail(Point3F offset, F32 invScale)
{
   F32 dist = mSqrt(mDot(offset,offset));
   dist *= invScale;
   return selectCurrentDetail2(dist);
}

S32 TSShapeInstance::selectCurrentDetail2(F32 adjustedDist)
{
   if (0 == mShape)
      return 0;

   if (mShape->mSmallestVisibleDL>=0 && mShape->details[0].maxError>=0)
      // use new scheme
      return selectCurrentDetail2Ex(adjustedDist);

   RectI viewport = GFX->getViewport();
   F32 pixelScale = viewport.extent.x * 1.6f / 640.0f;
   F32 pixelRadius = GFX->projectRadius(adjustedDist,mShape->radius) * pixelScale;
   F32 adjustedPR = pixelRadius * smDetailAdjust;
   if(adjustedPR <= mShape->mSmallestVisibleSize)
      adjustedPR = mShape->mSmallestVisibleSize + 0.01f;

   return selectCurrentDetail(adjustedPR);
}

S32 TSShapeInstance::selectCurrentDetail2Ex(F32 adjustedDist)
{
   // find tolerance
   RectI viewport = GFX->getViewport();
   F32 pixelScale = viewport.extent.x * 1.6f / 640.0f;
   F32 proj = GFX->projectRadius(adjustedDist,1.0f) * pixelScale; // pixel size of 1 meter at given distance
   if ( smFogExemptionOn )
      return selectCurrentDetailEx(F32(0.001)/proj);
   else
      return selectCurrentDetailEx(smScreenError/proj);
}

S32 TSShapeInstance::selectCurrentDetail(F32 size)
{
   if (0 == mShape)
       return 0;

   // check to see if not visible first...
   if (size<=mShape->mSmallestVisibleSize)
   {
      // don't render...
      mCurrentDetailLevel=-1;
      mCurrentIntraDetailLevel = 0.0f;
      return -1;
   }

   // same detail level as last time?
   // only search for detail level if the current one isn't the right one already
   if ( mCurrentDetailLevel<0 ||
        (mCurrentDetailLevel==0 && size<=mShape->details[0].size) ||
        (mCurrentDetailLevel>0  && (size<=mShape->details[mCurrentDetailLevel].size || size>mShape->details[mCurrentDetailLevel-1].size)))
   {
      // scan shape for highest detail size smaller than us...
      // shapes details are sorted from largest to smallest...
      // a detail of size <= 0 means it isn't a renderable detail level (utility detail)
      for (S32 i=0; i<mShape->details.size(); i++)
      {
         if (size>mShape->details[i].size)
         {
            mCurrentDetailLevel = i;
            break;
         }
         if (i+1>=mShape->details.size() || mShape->details[i+1].size<0)
         {
            // We've run out of details and haven't found anything?
            // Let's just grab this one.
            mCurrentDetailLevel = i;
            break;
         }
      }
   }

   F32 curSize = mShape->details[mCurrentDetailLevel].size;
   F32 nextSize = mCurrentDetailLevel==0 ? 2.0f * curSize : mShape->details[mCurrentDetailLevel-1].size;
   mCurrentIntraDetailLevel = nextSize-curSize>0.01f ? (size-curSize) / (nextSize-curSize) : 1.0f;
   mCurrentIntraDetailLevel = mCurrentIntraDetailLevel>1.0f ? 1.0f : (mCurrentIntraDetailLevel<0.0f ? 0.0f : mCurrentIntraDetailLevel);

   // now restrict chosen detail level by cutoff value
   S32 cutoff = getMin(smNumSkipRenderDetails,mShape->mSmallestVisibleDL);
   if (mCurrentDetailLevel>=0 && mCurrentDetailLevel<cutoff)
   {
      mCurrentDetailLevel = cutoff;
      mCurrentIntraDetailLevel = 1.0f;
   }

   return mCurrentDetailLevel;
}

S32 TSShapeInstance::selectCurrentDetailEx(F32 errorTOL)
{
    if (0 == mShape)
        return 0;

   // note:  we use 10 time the average error as the metric...this is
   // more robust than the maxError...the factor of 10 is to put average error
   // on about the same scale as maxError.  The errorTOL is how much
   // error we are able to tolerate before going to a more detailed version of the
   // shape.  We look for a pair of details with errors bounding our errorTOL,
   // and then we select an interpolation parameter to tween betwen them.  Ok, so
   // this isn't exactly an error tolerance.  A tween value of 0 is the lower poly
   // model (higher detail number) and a value of 1 is the higher poly model (lower
   // detail number).

   // deal with degenerate case first...
   // if smallest detail corresponds to less than half tolerable error, then don't even draw
   F32 prevErr;
   if (mShape->mSmallestVisibleDL<0)
      prevErr=0.0f;
   else
      prevErr = 10.0f * mShape->details[mShape->mSmallestVisibleDL].averageError * 20.0f;
   if (mShape->mSmallestVisibleDL<0 || prevErr<errorTOL)
   {
      // draw last detail
      mCurrentDetailLevel=mShape->mSmallestVisibleDL;
      mCurrentIntraDetailLevel = 0.0f;
      return mCurrentDetailLevel;
   }

   // this function is a little odd
   // the reason is that the detail numbers correspond to
   // when we stop using a given detail level...
   // we search the details from most error to least error
   // until we fit under the tolerance (errorTOL) and then
   // we use the next highest detail (higher error)
   for (S32 i=mShape->mSmallestVisibleDL; i>=0; i--)
   {
      F32 err0 = 10.0f * mShape->details[i].averageError;
      if (err0 < errorTOL)
      {
         // ok, stop here

         // intraDL = 1 corresponds to fully this detail
         // intraDL = 0 corresponds to the next lower (higher number) detail
         mCurrentDetailLevel = i;
         mCurrentIntraDetailLevel = 1.0f - (errorTOL-err0)/(prevErr-err0);
         return mCurrentDetailLevel;
      }
      prevErr=err0;
   }

   // get here if we are drawing at DL==0
   mCurrentDetailLevel = 1;
   mCurrentIntraDetailLevel = 1.0f;
   return mCurrentDetailLevel;
}

GBitmap * TSShapeInstance::snapshot(TSShape * shape, U32 width, U32 height, bool mip, MatrixF & cameraPos, S32 dl, F32 intraDL, bool hiQuality)
{
   TSShapeInstance * shapeInstance = new TSShapeInstance(shape, true);
   shapeInstance->setCurrentDetail(dl,intraDL);
   shapeInstance->animate();
   GBitmap * bmp = shapeInstance->snapshot(width,height,mip,cameraPos,hiQuality);

   delete shapeInstance;

   return bmp;
}

GBitmap * TSShapeInstance::snapshot(U32 width, U32 height, bool mip, MatrixF & cameraPos, S32 dl, F32 intraDL, bool hiQuality)
{
   setCurrentDetail(dl,intraDL);
   animate();
   return snapshot(width,height,mip,cameraPos,hiQuality);
}

GBitmap * TSShapeInstance::snapshot_softblend(U32 width, U32 height, bool mip, MatrixF & cameraMatrix, bool hiQuality)
{
   // this version of the snapshot function renders the shape to a black texture, then to white, then reads bitmaps 
   // back for both renders and combines them, restoring the alpha and color values.  this is based on the
   // TGE implementation.  it is not fast due to the copy and software combination operations.  the generated bitmaps
   // are upside-down (which is how TGE generated them...)

   Point2I size = GFX->getActiveRenderTarget()->getSize();
   U32 screenWidth = size.x;
   U32 screenHeight = size.y;

   if (screenWidth==0 || screenHeight==0)
      return NULL; // probably in exporter...

   PROFILE_START(TSShapeInstance_snapshot_sb);

   AssertFatal(width<screenWidth && height<screenHeight,"TSShapeInstance::snapshot: bitmap cannot be larger than screen resolution");

   S32 scale = 1;
   if (hiQuality)
      while ((scale<<1)*width <= screenWidth && (scale<<1)*height <= screenHeight)
         scale <<= 1;
   if (scale>smMaxSnapshotScale)
      scale = smMaxSnapshotScale;

   // height and width of intermediate bitmaps
   U32 bmpWidth  = width*scale;
   U32 bmpHeight = height*scale;

   PROFILE_START(TSShapeInstance_snapshot_sb_setup);

   GFX->setActiveDevice( 0 );
   GFX->beginScene();

   RectI saveViewport = GFX->getViewport();
   const MatrixF saveProj = GFX->getProjectionMatrix();
   GFX->pushWorldMatrix();
   
   // setup viewport and frustrum (do orthographic projection)
   GFX->setViewport(RectI(0, 0, bmpWidth, bmpHeight));

   // TGE snapshots images are upside down.  For compatibility with rendering code that expects this, 
   // flip the projection vertically so that the images generated are upside down.
   GFX->setOrtho(-mShape->radius, mShape->radius, mShape->radius, -mShape->radius, 1, 20.0f * mShape->radius);

   // position camera...
   Point3F y;
   cameraMatrix.getColumn(1,&y);
   y *= -10.0f * mShape->radius; // move camera back ten units * shape radius
   y += mShape->center; // translate camera so that we're looking at center of shape
   cameraMatrix.setColumn(3,y); // y is now the new position in object space, set it in the matrix
   // store the camera position for the scene state structure below
   Point3F cp = y;
   cameraMatrix.inverse(); // make it into an object -> world transformation?

   // setup scene state required for TS mesh render...this is messy and inefficient; 
   // should have a mode where most of this is done just once (and then 
   // only the camera matrix changes between snapshots).
   // note that we use getFrustum here, but we set up an ortho projection above.  
   // it doesn't seem like the scene state object pays attention to whether the projection is 
   // ortho or not.  this could become a problem if some code downstream tries to 
   // reconstruct the projection matrix using the dimensions and doesn't 
   // realize it should be ortho.  at the moment no code is doing that.
   F32 left, right, top, bottom, nearPlane, farPlane;
   GFX->getFrustum( &left, &right, &bottom, &top, &nearPlane, &farPlane );
   U32 numFogVolumes;
   FogVolume* fogVolumes;
   gClientSceneGraph->getFogVolumes(numFogVolumes, fogVolumes);

   SceneState state(
      NULL,
      1,
      left, right,
      bottom, top,
      nearPlane,
      farPlane, //far plane
      GFX->getViewport(),
      cp,
      cameraMatrix,
      farPlane, // fog distance - aka disable fog
      farPlane, // vis distance mod
      ColorF(1.0f,1.0f,1.0f,1.0f), // fog color
      numFogVolumes,// num fog volumnes
      fogVolumes, // ptr to fog volumes
      farPlane); // vis factor

   // build the fog texture
   gClientSceneGraph->buildFogTexture(&state);

   TSMesh::setCamTrans( cameraMatrix );
   TSMesh::setSceneState( &state );

   // we don't support refraction and glow in the snapshots at this time.
   TSMesh::setGlow(false);
   TSMesh::setRefract(false);

   // TSMesh expects the world transform to contain the object's world transform.  
   // set it to identity (cameraMatrix contains all the transforming we need to do)
   GFX->setWorldMatrix(MatrixF(true));
#ifdef STATEBLOCK
	AssertFatal(mSetSB, "TSShapeInstance::snapshot_softblend -- mSetSB cannot be NULL.");
	mSetSB->apply();
#else
   // set some initial states
   GFX->setCullMode(GFXCullNone);
   GFX->setLightingEnable(false);
   GFX->setZEnable(true);
   GFX->setZFunc(GFXCmpLessEqual);
#endif


   // set gfx up for render to texture
//   GFX->pushActiveRenderSurfaces();

   PROFILE_END(TSShapeInstance_snapshot_sb_setup); // setup

   PROFILE_START(TSShapeInstance_snapshot_sb_renderblack);
   // take a snapshot of the shape with a black background...
   GFXTexHandle blackTex;
   blackTex.set( bmpWidth, bmpHeight, GFXFormatR8G8B8A8, &GFXDefaultRenderTargetProfile ); 
//   GFX->setActiveRenderSurface(blackTex);

   ColorI black(0,0,0,0);
   GFX->clear( GFXClearZBuffer | GFXClearStencil | GFXClearTarget, black, 1.0f, 0 );
   render(mCurrentDetailLevel,mCurrentIntraDetailLevel);
   PROFILE_END(TSShapeInstance_snapshot_sb_renderblack);

   PROFILE_START(TSShapeInstance_snapshot_sb_renderwhite);
   // take a snapshot of the shape with a white background...
   GFXTexHandle whiteTex;
   whiteTex.set( bmpWidth, bmpHeight, GFXFormatR8G8B8A8, &GFXDefaultRenderTargetProfile ); 
//   GFX->setActiveRenderSurface(whiteTex);

   ColorI white(255,255,255,255);
   GFX->clear( GFXClearZBuffer | GFXClearStencil | GFXClearTarget, white, 1.0f, 0 );
   render(mCurrentDetailLevel,mCurrentIntraDetailLevel);
   PROFILE_END(TSShapeInstance_snapshot_sb_renderwhite);

   PROFILE_START(TSShapeInstance_snapshot_sb_unsetup);
   
   // done rendering, reset render states

//   GFX->popActiveRenderSurfaces();
#ifdef STATEBLOCK
	AssertFatal(mClearSB, "TSShapeInstance::snapshot_softblend -- mClearSB cannot be NULL.");
	mClearSB->apply();
#else
   GFX->setZEnable(false);
#endif

   GFX->setBaseRenderState();

   GFX->setViewport(saveViewport);
   GFX->setProjectionMatrix(saveProj);
   GFX->popWorldMatrix();

   GFX->endScene();
   PROFILE_END(TSShapeInstance_snapshot_sb_unsetup);

   PROFILE_START(TSShapeInstance_snapshot_sb_separate);

   // copy the black render target data into a bitmap
   GBitmap * blackBmp = new GBitmap;
   blackBmp->allocateBitmap(bmpWidth, bmpHeight, false, GFXFormatR8G8B8);
   blackTex->copyToBmp(blackBmp);

   // copy the white target data into a bitmap
   GBitmap* whiteBmp = new GBitmap;
   whiteBmp->allocateBitmap(bmpWidth, bmpHeight, false, GFXFormatR8G8B8);
   whiteTex->copyToBmp(whiteBmp);

   // now separate the color and alpha channels
   GBitmap * bmp = new GBitmap;
   bmp->allocateBitmap(width, height, mip, GFXFormatR8G8B8A8);
   U8 * wbmp = (U8*)whiteBmp->getBits(0);
   U8 * bbmp = (U8*)blackBmp->getBits(0);
   U8 * dst  = (U8*)bmp->getBits(0);
   U32 i,j;
   
   if (hiQuality)
   {
      for (i=0; i<height; i++)
      {
         for (j=0; j<width; j++)
         {
            F32 alphaTally = 0.0f;
            S32 alphaIntTally = 0;
            S32 alphaCount = 0;
            F32 rTally = 0.0f;
            F32 gTally = 0.0f;
            F32 bTally = 0.0f;
            for (S32 k=0; k<scale; k++)
            {
               for (S32 l=0; l<scale; l++)
               {
                  // shape on black background is alpha * color, shape on white background is alpha * color + (1-alpha) * 255
                  // we want 255 * alpha, or 255 - (white - black)
                  U32 pos = 3*((i*scale+k)*bmpWidth+j*scale+l);
                  U32 alpha = 255 - (wbmp[pos+0] - bbmp[pos+0]);
                  alpha    += 255 - (wbmp[pos+1] - bbmp[pos+1]);
                  alpha    += 255 - (wbmp[pos+2] - bbmp[pos+2]);
                  F32 floatAlpha = ((F32)alpha)/(1.0f*255.0f);
                  if (alpha != 0)
                  {
                     rTally += bbmp[pos+0];
                     gTally += bbmp[pos+1];
                     bTally += bbmp[pos+2];
                     alphaCount++;
                  }
                  alphaTally += floatAlpha;
                  alphaIntTally += alpha;
               }
            }
            F32 invAlpha = alphaTally > 0.01f ? 1.0f / alphaTally : 0.0f;
            U32 pos = 4*(i*width+j);
            dst[pos+0] = (U8)(rTally * invAlpha);
            dst[pos+1] = (U8)(gTally * invAlpha);
            dst[pos+2] = (U8)(bTally * invAlpha);
            dst[pos+3] = (U8)(((F32)alphaIntTally) / (F32) (3*alphaCount));
         }
      }
   }
   else
   {
      // simpler, probably faster...
      for (i=0; i<height*width; i++)
      {
         // shape on black background is alpha * color, shape on white background is alpha * color + (1-alpha) * 255
         // we want 255 * alpha, or 255 - (white - black)
         // JMQ: or more verbosely:
         //  cB = alpha * color + (0 * (1 - alpha))
         //  cB = alpha * color
         //  cW = alpha * color + (255 * (1 - alpha))
         //  cW = cB + (255 * (1 - alpha))
         // solving for alpha
         //  cW - cB = 255 * (1 - alpha)
         //  (cW - cB)/255 = (1 - alpha)
         //  alpha = 1 - (cW - cB)/255
         // since we want alpha*255, multiply through by 255
         //  alpha * 255 = 255 - cW - cB
         U32 alpha = 255 - (wbmp[i*3+0] - bbmp[i*3+0]);
         alpha    += 255 - (wbmp[i*3+1] - bbmp[i*3+1]);
         alpha    += 255 - (wbmp[i*3+2] - bbmp[i*3+2]);

         if (alpha != 0)
         {
            F32 floatAlpha = ((F32)alpha)/(1.0f*255.0f); 
            dst[i*4+0] = (U8)(bbmp[i*3+0] / floatAlpha);
            dst[i*4+1] = (U8)(bbmp[i*3+1] / floatAlpha);
            dst[i*4+2] = (U8)(bbmp[i*3+2] / floatAlpha);
            dst[i*4+3] = (U8)(alpha/3);
         }
         else
         {
            dst[i*4+0] = dst[i*4+1] = dst[i*4+2] = dst[i*4+3] = 0;
         }
      }
   }
   PROFILE_END(TSShapeInstance_snapshot_sb_separate);
  
   PROFILE_START(TSShapeInstance_snapshot_sb_extrude);
   if (mip)
      bmp->extrudeMipLevels();
   PROFILE_END(TSShapeInstance_snapshot_sb_extrude);

   delete blackBmp;
   delete whiteBmp;

   blackTex = NULL;
   whiteTex = NULL;

   PROFILE_END(TSShapeInstance_snapshot_sb);

   return bmp;
}

GBitmap * TSShapeInstance::snapshot(U32 width, U32 height, bool mip, MatrixF & cameraMatrix,bool hiQuality)
{
   GBitmap * bmp = snapshot_softblend(width, height, mip, cameraMatrix, hiQuality);
   return bmp;
}

//-------------------------------------------------------------------------------------
// Object (MeshObjectInstance & PluginObjectInstance) render methods
//-------------------------------------------------------------------------------------

void TSShapeInstance::ObjectInstance::render(S32, TSMaterialList *)
{
   AssertFatal(0,"TSShapeInstance::ObjectInstance::render:  no default render method.");
}

bool TSShapeInstance::ObjectInstance::castRayOpcode( S32 objectDetail, const Point3F & start, const Point3F & end, RayInfo * )
{
   return false;
}

bool TSShapeInstance::ObjectInstance::buildPolyListOpcode( S32 objectDetail, AbstractPolyList *, U32 & surfaceKey )
{
   return false;
}

bool TSShapeInstance::ObjectInstance::buildConvexOpcode( MatrixF &mat, S32 objectDetail, const Box3F &bounds, Convex *c, Convex *list )
{
   return false;
}

void TSShapeInstance::MeshObjectInstance::renderQuery(S32 objectDetail)
{
	if (visible>0.01f)
	{
		TSMesh * mesh = getMesh(objectDetail);
		if (mesh)
		{
			MatrixF * transform = getTransform();
			{
				if (TSShapeInstance::smRenderData.currentTransform)
				{
					GFX->popWorldMatrix();
				}
				if (transform)
				{
					GFX->pushWorldMatrix();
					GFX->multWorld( *transform );
				}
				TSShapeInstance::smRenderData.currentTransform = transform;
			}
			mesh->render(NULL);
		}
	}
}

void TSShapeInstance::MeshObjectInstance::render(S32 objectDetail, TSMaterialList * materials)
{
   if (visible>0.01f)
   {
      TSMesh * mesh = getMesh(objectDetail);
      if (mesh)
      {
         MatrixF * transform = getTransform();
         {
            if (TSShapeInstance::smRenderData.currentTransform)
            {
               GFX->popWorldMatrix();
            }
            if (transform)
            {
               GFX->pushWorldMatrix();
               GFX->multWorld( *transform );
            }
            TSShapeInstance::smRenderData.currentTransform = transform;
         }
         mesh->setFade(visible);
         mesh->render(frame,matFrame,materials);
      }
   }
}

bool TSShapeInstance::MeshObjectInstance::castRayOpcode( S32 objectDetail, const Point3F & start, const Point3F & end, RayInfo *info )
{
   TSMesh * mesh = getMesh(objectDetail);
   if (mesh && visible>0.01f)
      return mesh->castRayOpcode(start, end, info);
   return false;
}

void TSShapeInstance::incDebrisRefCount()
{
   ++debrisRefCount;
}

void TSShapeInstance::decDebrisRefCount()
{
   if( debrisRefCount == 0 ) return;
   --debrisRefCount;
}

U32 TSShapeInstance::getDebrisRefCount()
{
   return debrisRefCount;
}


U32 TSShapeInstance::getNumDetails()
{
   if( mShape )
   {
      return mShape->details.size();
   }

   return 0;
}

void TSShapeInstance::prepCollision()
{
   // Iterate over all our meshes and call prepCollision on them...
   for(S32 i=0; i<hShape->meshes.size(); i++)
   {
      if(hShape->meshes[i])
         hShape->meshes[i]->prepOpcodeCollision();
   }
}

bool TSShapeInstance::MeshObjectInstance::buildConvexOpcode(MatrixF &mat, S32 objectDetail, const Box3F &bounds, Convex *c, Convex *list)
{
   TSMesh * mesh = getMesh(objectDetail);
   if (mesh && visible>0.01f)
      return mesh->buildConvexOpcode(mat, bounds, c, list);
   return false;
}


void TSShapeInstance::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSZFunc, GFXCmpLessEqual);
	GFX->endStateBlock(mSetSB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->endStateBlock(mClearSB);
}


void TSShapeInstance::releaseStateBlock()
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

void TSShapeInstance::initsb()
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

void TSShapeInstance::shutdown()
{
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mClearSB);
}

