//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "ts/tsSortedMesh.h"
#include "math/mMath.h"
#include "ts/tsShapeInstance.h"
#include "renderInstance/renderInstMgr.h"
#include "sceneGraph/lightManager.h"
#include "materials/matInstance.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* TSSortedMesh::mZWriteTSB = NULL;
GFXStateBlock* TSSortedMesh::mZWriteFSB = NULL;
// Not worth the effort, much less the effort to comment, but if the draw types
// are consecutive use addition rather than a table to go from index to command value...
/*
#if ((GL_TRIANGLES+1==GL_TRIANGLE_STRIP) && (GL_TRIANGLE_STRIP+1==GL_TRIANGLE_FAN))
   #define getDrawType(a) (GL_TRIANGLES+(a))
#else
   U32 drawTypes[] = { GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN };
   #define getDrawType(a) (drawTypes[a])
#endif
*/

// found in tsmesh
extern void forceFaceCamera(MatrixF& mat);

//-----------------------------------------------------
// TSSortedMesh render methods
//-----------------------------------------------------

extern DefaultMeshLightPlugin p_DefaultMeshLightPlugin;

void TSSortedMesh::render(S32 frame, S32 matFrame, TSMaterialList * materials)
{
	if( vertsPerFrame <= 0 ) return;

	F32 meshVisibility = TSMesh::overrideFadeVal * mVisibility;
	if (meshVisibility < VISIBILITY_EPSILON)
		return;

	if(mDynamic && meshType!=SkinMeshType)
		UpdateFrame(frame,matFrame);

	bool isReflectPass = gClientSceneGraph->isReflectPass();

	RenderInst *coreRI = gRenderInstManager.allocInst();
	if(TSMesh::smFieldMesh)
		coreRI->type = RenderInstManager::RIT_MeshField;
	else
		coreRI->type = RenderInstManager::RIT_Mesh;

	if (smSceneState)
	{
		if(TSMesh::overrideFadeVal<0.99)  //虚化物件特殊处理
			coreRI->fadeFlag = 5;

		// Calculate our sort point manually.
		if(smObject)	//对于场景对象
		{
			smObject->getWorldBox().getCenter(&coreRI->sortPoint);
		}
		else
		{   //对于模型
			MatrixF &ViewMx = gClientSceneGraph->getLastView2WorldMX();
			MatrixF objToWorld = ViewMx * GFX->getWorldMatrix();  //Ray:去掉视矩阵
			mBounds.getCenter(&coreRI->sortPoint);
			objToWorld.mulP(coreRI->sortPoint);
		}
	} else {
		coreRI->sortPoint.set(0,0,0);
	}

	Point3F cameraCenter;
	gClientSceneGraph->getLastView2WorldMX().getColumn(3,&cameraCenter);

	// setup transforms
	const MatrixF &objTrans = GFX->getWorldMatrix();
	MatrixF proj     = GFX->getProjectionMatrix();
	MatrixF world;

	world.mul( smCamTrans, objTrans );

	if (getFlags(Billboard))
		forceFaceCamera(world);  

	proj.mul( world);
	proj.transpose();

	coreRI->worldXform = gRenderInstManager.allocXform();
	*coreRI->worldXform = proj;

	coreRI->objXform = gRenderInstManager.allocXform();
	*coreRI->objXform = objTrans;

	coreRI->vertBuff = getVertexBuffer();
	coreRI->primBuff = &mPB;

	coreRI->obj = dynamic_cast<RenderableSceneObject *>(smObject);

	coreRI->GameRenderStatus = TSMesh::getGameRenderStatus();
	coreRI->overrideColorExposure = TSMesh::getOverrideColorExposure();

	coreRI->fogFactor = TSMesh::getFogFactor();

	//-----------------------------------------------------------------

	coreRI->light = gClientSceneGraph->getLightManager()->getDefaultLight();
	coreRI->lightSecondary = coreRI->light;
	coreRI->visibility = meshVisibility;  
	coreRI->cubemap = smCubemap;
	coreRI->backBuffTex = GFX->getSfxBackBuffer();
	coreRI->lightmap = smLightmap;

	setVertTexParam(coreRI);

	static Vector<RenderInst*> sRenderList;
	sRenderList.clear();

	if(isReflectPass)
		coreRI->GameRenderStatus &=  Material::SunLightOnly;

	bool disableLightProcess = GameRenderStatus& Material::DisableColor;

   Cluster * cluster;
   S32 nextCluster = startCluster[frame];
   S32 step = 0;
   do
   {
      // the cluster...
      cluster = &clusters[nextCluster];

      // render the cluster...
      for (S32 i=cluster->startPrimitive; i<cluster->endPrimitive; i++)
      {
		  TSDrawPrimitive & draw = primitives[i];

		  U32 test1 = TSShapeInstance::smRenderData.materialIndex ^ draw.matIndex;
		  S32 noMat = test1 & TSDrawPrimitive::NoMaterial;

#ifdef POWER_DEBUG
		  // for inspection if you happen to be running in a debugger and can't do bit 
		  // operations in your head.
		  S32 triangles = draw.matIndex & TSDrawPrimitive::Triangles;
		  S32 strip = draw.matIndex & TSDrawPrimitive::Strip;
		  S32 fan = draw.matIndex & TSDrawPrimitive::Fan;
		  S32 indexed = draw.matIndex & TSDrawPrimitive::Indexed;
		  S32 type = draw.matIndex & TSDrawPrimitive::TypeMask;
		  triangles, strip, fan, indexed, type;
#endif

		  U32 matIndex = draw.matIndex & TSDrawPrimitive::MaterialMask;
		  MatInstance *matInst = materials->getMaterialInst( matIndex );

		  if( matInst )
		  {
			  Material *pMat = matInst->getMaterial();
			  AssertFatal(pMat,"TSMesh::render() pMat==NULL");

			  //if(TSMesh::overrideFadeVal!=1.0f && pMat->effectLight)
			  // continue;

			  RenderInst *ri = gRenderInstManager.allocInst(false);
			  *ri = *coreRI;
			  ri->primitiveFirstPass = gRenderInstManager.allocPrimitiveFirstPass(); 
			  (*ri->primitiveFirstPass) = true;
			  ri->matInst = matInst;
			  ri->primBuffIndex = i;
			  ri->enableBlendColor = false;

			  if ( pMat->effectLight )
				  ri->dynamicLight = ri->dynamicLightSecondary = NULL;

			  // fill in misc texture if IFL - need it to override base texture since it changes frequently
			  if( pMat->isIFL() )
			  {
				  ri->miscTex = &*(materials->getMaterial(matIndex));
			  }

			  ri->meshDisableZ = disableZ;

			  //Ray: 如果有过滤色的话需要多次渲染
			  bool enableBlendColor  = pMat->haveFilterColor();
			  if(enableBlendColor)
			  {
				  RenderInst *fri = gRenderInstManager.allocInst(false);
				  *fri = *ri;
				  fri->primitiveFirstPass = NULL;
				  fri->enableBlendColor = true;
				  fri->maxBlendColor = pMat->getFilterColor();
				  fri->maxBlendColor.alpha = 1.0f;
				  fri->fadeFlag = -1; // 借用该标记来区分过滤色pass和原pass

				  TSShapeInstance::smRenderData.sortSeed++;
				  fri->SortedIndex = TSShapeInstance::smRenderData.sortSeed + sortOrder*1000;
				  gRenderInstManager.addInst(fri);
			  }

			  TSShapeInstance::smRenderData.sortSeed++;
			  ri->SortedIndex = TSShapeInstance::smRenderData.sortSeed + sortOrder*1000;

			  //pass it on         
			  if(disableLightProcess || pMat->emissive[0])
			  {
				  gRenderInstManager.addInst(ri);
			  }
			  else
				  sRenderList.push_back(ri);
		  }
		  else
		  {
			  // no material, need this to support .ifl animations
			  RenderInst *ri = gRenderInstManager.allocInst(false);
			  *ri = *coreRI;
			  ri->primBuffIndex = i;
			  TSShapeInstance::smRenderData.sortSeed++;
			  ri->SortedIndex = TSShapeInstance::smRenderData.sortSeed + sortOrder*1000;
			  ri->enableBlendColor = false;				//Ray: IFL的filter未加入

			  ri->meshDisableZ = disableZ;

			  if( materials->isIFL(matIndex) )
			  {
				  ri->matInst = NULL;
				  ri->worldXform = gRenderInstManager.allocXform();
				  *ri->worldXform = GFX->getWorldMatrix();
			  }
			  else
			  {
				  ri->matInst = gRenderInstManager.getWarningMat();
			  }

			  // noMat = no EXPORTER material, so can't fill in this RenderInst data
			  if( !noMat )
			  {
				  U32 matIndex = draw.matIndex & TSDrawPrimitive::MaterialMask;
				  U32 flags = materials->getFlags(matIndex);
				  ri->translucent = flags & TSMaterialList::Translucent;
				  if (flags & TSMaterialList::Additive)
					  ri->transFlags = ((GFXBlendSrcAlpha << 4) & 0xf0) | (GFXBlendOne & 0x0f);
				  else
					  ri->transFlags = ((GFXBlendSrcAlpha << 4) & 0xf0) | (GFXBlendInvSrcAlpha & 0x0f);
				  ri->miscTex = &*(materials->getMaterial(matIndex));
			  }
			  gRenderInstManager.addInst(ri);
		  }
      }

      // determine next cluster...
      if (cluster->frontCluster!=cluster->backCluster)
         nextCluster = (mDot(cluster->normal,cameraCenter) > cluster->k) ? cluster->frontCluster : cluster->backCluster;
      else
         nextCluster = cluster->frontCluster;
   } while (nextCluster>=0);

   if(!disableLightProcess && !sRenderList.empty())
   {
	   if (smLightPlugin == NULL)
		   smLightPlugin = &p_DefaultMeshLightPlugin;

	   U32 lightNum = smLightPlugin->prepareLight();
	   smLightPlugin->processRI(this, sRenderList);
   }

#ifdef STATEBLOCK
	AssertFatal(mZWriteTSB, "TSSortedMesh::render -- mZWriteTSB cannot be NULL.");
	mZWriteTSB->apply();
#else
   GFX->setZWriteEnable(true);
#endif

}

//-----------------------------------------------------
// TSSortedMesh collision methods
//-----------------------------------------------------

bool TSSortedMesh::buildPolyList(S32 frame, AbstractPolyList * polyList, U32 & surfaceKey)
{
   return Parent::buildPolyList(frame, polyList, surfaceKey);
}

bool TSSortedMesh::castRay(S32 frame, const Point3F & start, const Point3F & end, RayInfo * rayInfo)
{
   return Parent::castRay(frame, start, end, rayInfo);
}

bool TSSortedMesh::buildConvexHull()
{
	return Parent::buildConvexHull();
}

S32 TSSortedMesh::getNumPolys()
{
   S32 count = 0;
   S32 cIdx = !clusters.size() ? -1 : 0;
   while (cIdx>=0)
   {
      Cluster & cluster = clusters[cIdx];
      for (S32 i=cluster.startPrimitive; i<cluster.endPrimitive; i++)
      {
         if (primitives[i].matIndex & TSDrawPrimitive::Triangles)
            count += primitives[i].numElements / 3;
         else
            count += primitives[i].numElements - 2;
      }
      cIdx = cluster.frontCluster; // always use frontCluster...we assume about the same no matter what
   }
   return count;
}

//-----------------------------------------------------
// TSSortedMesh assembly/dissembly methods
// used for transfer to/from memory buffers
//-----------------------------------------------------

#define alloc TSShape::alloc

void TSSortedMesh::assemble(bool skip)
{
   CLockGuard guard(alloc.GetMutex());

   bool save1 = TSMesh::smUseTriangles;
   bool save2 = TSMesh::smUseOneStrip;
   TSMesh::smUseTriangles = false;
   TSMesh::smUseOneStrip = false;

   TSMesh::assemble(skip);

   TSMesh::smUseTriangles = save1;
   TSMesh::smUseOneStrip = save2;

   S32 numClusters = alloc.get32();
   S32 * ptr32 = alloc.copyToShape32(numClusters*8);
   clusters.set(ptr32,numClusters);

   S32 sz = alloc.get32();
   ptr32 = alloc.copyToShape32(sz);
   startCluster.set(ptr32,sz);

   sz = alloc.get32();
   ptr32 = alloc.copyToShape32(sz);
   firstVerts.set(ptr32,sz);

   sz = alloc.get32();
   ptr32 = alloc.copyToShape32(sz);
   numVerts.set(ptr32,sz);

   sz = alloc.get32();
   ptr32 = alloc.copyToShape32(sz);
   firstTVerts.set(ptr32,sz);

   alwaysWriteDepth = alloc.get32()!=0;

   alloc.checkGuard();
}

void TSSortedMesh::disassemble()
{
   CLockGuard guard(alloc.GetMutex());

   TSMesh::disassemble();

   alloc.set32(clusters.size());
   alloc.copyToBuffer32((S32*)clusters.address(),clusters.size()*8);

   alloc.set32(startCluster.size());
   alloc.copyToBuffer32((S32*)startCluster.address(),startCluster.size());

   alloc.set32(firstVerts.size());
   alloc.copyToBuffer32((S32*)firstVerts.address(),firstVerts.size());

   alloc.set32(numVerts.size());
   alloc.copyToBuffer32((S32*)numVerts.address(),numVerts.size());

   alloc.set32(firstTVerts.size());
   alloc.copyToBuffer32((S32*)firstTVerts.address(),firstTVerts.size());

   alloc.set32(alwaysWriteDepth ? 1 : 0);

   alloc.setGuard();
}


void TSSortedMesh::resetStateBlock()
{
	//mZWriteTSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->endStateBlock(mZWriteTSB);

	//mZWriteFSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->endStateBlock(mZWriteFSB);
}


void TSSortedMesh::releaseStateBlock()
{
	if (mZWriteTSB)
	{
		mZWriteTSB->release();
	}

	if (mZWriteFSB)
	{
		mZWriteFSB->release();
	}
}

void TSSortedMesh::init()
{
	if (mZWriteTSB == NULL)
	{
		mZWriteTSB = new GFXD3D9StateBlock;
		mZWriteTSB->registerResourceWithDevice(GFX);
		mZWriteTSB->mZombify = &releaseStateBlock;
		mZWriteTSB->mResurrect = &resetStateBlock;

		mZWriteFSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void TSSortedMesh::shutdown()
{
	SAFE_DELETE(mZWriteTSB);
	SAFE_DELETE(mZWriteFSB);
}


