//-----------------------------------------------------------------------------
// PowerEngine Shader Engine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "ts/tsMesh.h"
#include "math/mMath.h"
#include "math/mathIO.h"
#include "math/mHalf.h"
#include "ts/tsShape.h"
#include "console/console.h"
#include "ts/tsShapeInstance.h"
#include "sceneGraph/sceneObject.h"
#include "ts/tsSortedMesh.h"
#include "core/bitRender.h"
#include "collision/convex.h"
#include "core/frameAllocator.h"
#include "platform/profiler.h"
#include "materials/sceneData.h"
#include "sceneGraph/sceneGraph.h"
#include "materials/matInstance.h"
#include "gfx/gfxCanon.h"
#include "renderInstance/renderInstMgr.h"
#include "T3D/tsStatic.h"
#include "ts/tsShapeInstance.h"
#include "gfx/gfxCardProfile.h"

#include "util/processMgr.h"
#include "util/aniThread.h"

#include "opcode/Opcode.h"
#include "opcode/Ice/IceAABB.h"
#include "opcode/Ice/IcePoint.h"
#include "opcode/OPC_AABBTree.h"
#include "opcode/OPC_AABBCollider.h"

#include SHADER_CONSTANT_INCLUDE_FILE
#include "gfx/D3D/DXATIUtil.h"

GFXPrimitiveType drawTypes[] = { GFXTriangleList, GFXTriangleStrip, GFXTriangleFan };
#define getDrawType(a) (drawTypes[a])


static bool gOpcodeInitialized = false;

// structures used to share data between detail levels...
// used (and valid) during load only
Vector<Point3F*> TSMesh::smVertsList;
Vector<ColorI*>	 TSMesh::smVColList;
Vector<Point3F*> TSMesh::smNormsList;
Vector<U8*>      TSMesh::smEncodedNormsList;
Vector<Point2F*> TSMesh::smTVertsList;
Vector<bool>     TSMesh::smDataCopied;


Vector<MatrixF*> TSSkinMesh::smInitTransformList;
Vector<S32*>     TSSkinMesh::smVertexIndexList;
Vector<S32*>     TSSkinMesh::smBoneIndexList;
Vector<F32*>     TSSkinMesh::smWeightList;
Vector<S32*>     TSSkinMesh::smNodeIndexList;

MatrixF TSMesh::smCamTrans(true);
SceneState * TSMesh::smSceneState = NULL;
SceneObject * TSMesh::smObject = NULL;
TSShapeInstance *TSMesh::smShapeIns = NULL;
GFXCubemap * TSMesh::smCubemap = NULL;
GFXTextureObject *TSMesh::smLightmap = NULL;
bool TSMesh::smGlowPass = false;
bool TSMesh::smRefractPass = false;
bool TSMesh::smFieldMesh = false;


F32 TSMesh::overrideFadeVal = 1.0;
F32 TSMesh::overrideColorExposure = 1.0f;
F32 TSMesh::fogFactor = 1.0f;
bool TSMesh::smUseTriangles = false; // convert all primitives to triangle lists on load
bool TSMesh::smUseOneStrip  = true; // join triangle strips into one long strip on load
S32  TSMesh::smMinStripSize = 1;     // smallest number of _faces_ allowed per strip (all else put in tri list)
bool TSMesh::smUseEncodedNormals = false;
U32  TSMesh::GameRenderStatus = 0;
bool TSMesh::smFluidLight = false; 
F32 TSMesh::smFluidLightX = 0.0001;
F32 TSMesh::smFluidLightY = 0.0001;

const F32 TSMesh::VISIBILITY_EPSILON = 0.0001f;

// quick function to force object to face camera -- currently throws out roll :(
void forceFaceCamera(MatrixF& mat)
{
   Point4F p;

   mat.getColumn(3,&p);
   mat.identity();
   mat.setColumn(3,p);
   if (TSShapeInstance::smRenderData.objectScale)
   {
      MatrixF scale(true);
      scale.scale(Point3F(TSShapeInstance::smRenderData.objectScale->x,
                          TSShapeInstance::smRenderData.objectScale->y,
                          TSShapeInstance::smRenderData.objectScale->z));
      mat.mul(scale);
   }
}

void forceFaceCameraZAxis()
{
}

void DefaultMeshLightPlugin::processRI(TSMesh* mesh, Vector<RenderInst*>& list)
{
	for (Vector<RenderInst*>::iterator it = list.begin(); it != list.end(); it++)
	{
		RenderInst* ri = *it;
		ri->light = gClientSceneGraph->getLightManager()->getSpecialLight(LightManager::slSunLightType);
		ri->lightSecondary = ri->light;

		gRenderInstManager.addInst(ri);
	}
}

DefaultMeshLightPlugin p_DefaultMeshLightPlugin;

TSMeshLightPlugin* TSMesh::smLightPlugin = NULL; 

void TSMesh::registerLightPlugin(TSMeshLightPlugin* mlp)
{
   AssertFatal(mlp != NULL, "A NULL light plugin is invalid!");
   if (mlp != NULL)
   {
      smLightPlugin = mlp;
   }   
}

void TSMesh::unregisterLightPlugin(TSMeshLightPlugin* mlp)
{
   smLightPlugin = &p_DefaultMeshLightPlugin;      
}

//-----------------------------------------------------
// TSMesh render methods
//-----------------------------------------------------

void TSMesh::render(TSMaterialList * materials)
{
	if(!getVertexBuffer())
		return;

   GFX->setVertexBuffer(getVertexBuffer()->getPointer());
   GFX->setPrimitiveBuffer(mPB);

   RenderInst *coreRI = gRenderInstManager.allocInst();
   setVertTexParam(coreRI);
   F32 registerArry;

   if(TSShapeInstance::supportsVertTex)
   {
	   GFX->setTexture(VERTEX_TEXTURE_STAGE0 ,coreRI->vertexTex);
	   if(coreRI->vertexTex)
	   {
		   registerArry = coreRI->boneNum;
		   GFX->setVertexShaderConstF( VC_VERTEX_TEX, (float*)&registerArry, 1,1 );
	   }
	   else
	   {
		   registerArry = 0.0f;
		   GFX->setVertexShaderConstF( VC_VERTEX_TEX, (float*)&registerArry, 1,1 );
	   }

	   GFX->setRenderState(GFXRSZEnable,GFX->getRenderState(GFXRSZEnable));
   }
   else if(TSShapeInstance::supportsR2VB)
   {
	   if(coreRI->vertexTex)
	   {
		   if(!GFX->isDmapSet())
		   {
			   GFX->setPointSize(r2vbGlbEnable_Set(TRUE));
			   TSShapeInstance::smDummyVB->prepare();  //setup streamSource(1) = Dummy
			   GFX->updateStates();
			   GFX->setPointSize(r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DMAP));	
			   GFX->setDmap(true);
		   }
		   // Tell the driver that stream 1 is to be fetched from the DMAP texture
		   GFX->setTexture(VERTEX_TEXTURE_BUFF, coreRI->vertexTex);
	   }
	   else
	   {
		   if(GFX->isDmapSet())
		   {
			   GFX->setPointSize(r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DIS));
			   // Unbind the DMAP texture
			   GFX->setTexture(VERTEX_TEXTURE_BUFF, NULL);
			   GFX->updateStates();
			   GFX->setPointSize(r2vbGlbEnable_Set(FALSE));
			   TSShapeInstance::smNullVB->prepare();  //setup streamSource(1) = NULL
			   GFX->setDmap(false);
		   }
	   }

	   GFX->setRenderState(GFXRSZEnable,GFX->getRenderState(GFXRSZEnable));
   }

    U32 matIndex = 0;
	MatInstance *matInst = NULL;
	ProcessedMaterial* pmat = NULL;
	RenderPassData *pass = NULL;
	Material *material = NULL;
	for(S32 p=0; p<primitives.size(); p++)
	{
		if (materials)
		{
			TSDrawPrimitive &draw = primitives[p];
			matIndex = draw.matIndex & TSDrawPrimitive::MaterialMask;
			matInst = materials->getMaterialInst( matIndex );
			if (matInst && ((material = matInst->getMaterial())/* && material->translucent*/) && (pmat = matInst->getProcessedMaterial()) && (pass = pmat->getPass(matInst->getCurPass())))
			{
				GFX->setTexture(0, pass->tex[0]);
			}
			else
			{
				GFX->setTexture(0, NULL);
			}
		}
		GFX->drawPrimitive(p);
	}
}

GFXVertexBufferHandleBase *TSMesh::getVertexBuffer() 
{
	if(mDynamic)
		return TSShapeInstance::smRenderData.currentObjectInstance->mpVB;
	else
		return mpVB;
}

void TSMesh::setVertexBuffer(GFXVertexBufferHandleBase *pVB) 
{
	if(mDynamic)
		TSShapeInstance::smRenderData.currentObjectInstance->mpVB = pVB;
	else
		mpVB = pVB;
}

GFXTextureObject *TSMesh::getRVertexTexture(void *)
{
	return TSShapeInstance::smDefaultVertexTex.getPointer();
}

GFXTextureObject *TSMesh::getCVertexTexture(void *)
{
	return TSShapeInstance::smDefaultVertexTex.getPointer();
}

void TSMesh::frameCallback(void *param)
{
	FrameParam *pFrameData = (FrameParam *)param;
	pFrameData->pMesh->doUpdateFrame(pFrameData);
	delete pFrameData;
}

void TSMesh::doUpdateFrame(FrameParam *pParam)
{
	createVBIB(pParam->frame,pParam->matFrame,pParam->pVB);   //创建PB不变，VB更新
}

void TSMesh::UpdateFrame(S32 frame, S32 matFrame)
{
	TSShapeInstance::MeshObjectInstance* pObjectInstance = TSShapeInstance::smRenderData.currentObjectInstance;
	if(!pObjectInstance)
		return;

	PROFILE_SCOPE(UpdateFrame);

	bool dirty = pObjectInstance->dirty  && !smRefractPass;
	bool nullPoint = !pObjectInstance->mpVB;
	bool nullData = true; 
	if(!nullPoint)
		nullData = pObjectInstance->mpVB->getPointer()->isNull();

	if(dirty || nullPoint || nullData)
	{
		FrameParam *pFrameData = new FrameParam;
		pFrameData->frame = frame;
		pFrameData->matFrame = matFrame;
		pFrameData->pVB = pObjectInstance->mpVB;
		pFrameData->pMesh = this;

		if(nullPoint || nullData)
		{
			//Ray: 因为涉及到mPB的填充，为了不加锁同步处理顶点的初始操作.
			if( !initialTangents.size() )
			{
				createTangents();
			}

			frameCallback(pFrameData);
		}
		else
		{
#ifdef USE_MULTITHREAD_ANIMATE
			if(g_MultThreadWorkMgr->isEnable())
			{
				stThreadParam *pThreadData = new stThreadParam;
				pThreadData->type = MeshType;
				pThreadData->pObj = smObject;
				pThreadData->pParam = pFrameData;
				pThreadData->pFunc = frameCallback;

				g_MultThreadWorkMgr->addInProcessList(pThreadData);
			}
			else
				frameCallback(pFrameData);
#else
			frameCallback(pFrameData);
#endif
		}

		pObjectInstance->dirty = false;
	}
}

void TSMesh::render(S32 frame, S32 matFrame, TSMaterialList * materials)
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

   if(TSMesh::overrideFadeVal<0.99)  //虚化物件特殊处理
	   coreRI->fadeFlag = 5;

   if (smObject)
   {
	   smObject->getWorldBox().getCenter(&coreRI->sortPoint);
   } 
   else 
   {
	   MatrixF &ViewMx = gClientSceneGraph->getLastView2WorldMX();
	   MatrixF objToWorld = ViewMx * GFX->getWorldMatrix();  //Ray:去掉视矩阵
	   mBounds.getCenter(&coreRI->sortPoint);
	   objToWorld.mulV(coreRI->sortPoint);
	   //Box3F wBox = mBounds;
	   //objToWorld.mul(wBox);
	   //coreRI->sortPoint = ViewMx.getPosition();
	   //coreRI->sortPoint = wBox.getClosestPoint(coreRI->sortPoint);
   }

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

   TSMesh::getFluidLightParam(coreRI->enableFluidLight,coreRI->FluidLightX,coreRI->FluidLightY);

      
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

   U32 primSize = primitives.size();
   for (S32 i=0; i<primSize; i++)
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

	     //Ray: 如果有过滤色的话需要特殊渲染
	     bool enableBlendColor  = pMat->haveFilterColor();
	     if(enableBlendColor)
	     {
			 //if(gClientSceneGraph->isReflectPass())  //水面反射时都不做特效渲染
				// continue;

			 //ri->enableBlendColor = true;
			 //ri->maxBlendColor = pMat->getFilterColor();
			 //ri->maxBlendColor.alpha = 1.0f;

			 //TSShapeInstance::smRenderData.sortSeed++;
			 //ri->SortedIndex = TSShapeInstance::smRenderData.sortSeed + sortOrder*1000;

			 //if(ri->obj && !ri->obj->m_pChain)
			 //{
				// ri->obj->m_pChain = ri;
			 //}
			 //else
			 //{
				//RenderInst *tmp = ri->obj->m_pChain;
				//ri->obj->m_pChain = ri;
				//ri->obj->m_pChain->pInstNext = tmp;
			 //}
	     }
		 else
		 {
			 TSShapeInstance::smRenderData.sortSeed++;
			 ri->SortedIndex = TSShapeInstance::smRenderData.sortSeed + sortOrder*1000;
		 }


        //pass it on         
	    if(disableLightProcess || enableBlendColor || pMat->emissive[0] )
            gRenderInstManager.addInst(ri);
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

   if(!disableLightProcess && !sRenderList.empty())
   {
       if (smLightPlugin == NULL)
	       smLightPlugin = &p_DefaultMeshLightPlugin;

	   U32 lightNum = smLightPlugin->prepareLight();
       smLightPlugin->processRI(this, sRenderList);
   }

}

// set up materials for mesh rendering
// keeps track of flags via TSShapeInstance::smRenderData.materialFlags and only changes what needs to be changed
// keeps track of material index via TSShapeInstance::smRenderData.materialIndex
void TSMesh::setMaterial(S32 matIndex, TSMaterialList* materials)
{

   if ((matIndex|TSShapeInstance::smRenderData.materialIndex) & TSDrawPrimitive::NoMaterial)
   {
      if (matIndex & TSDrawPrimitive::NoMaterial)
      {
         TSShapeInstance::smRenderData.materialIndex = matIndex;
         return;
      }
   }

   matIndex &= TSDrawPrimitive::MaterialMask;
   TSShapeInstance::smRenderData.materialIndex = matIndex;
}

//-----------------------------------------------------
// TSMesh collision methods
//-----------------------------------------------------

bool TSMesh::buildPolyList(S32 frame, AbstractPolyList * polyList, U32 & surfaceKey)
{
   S32 firstVert  = vertsPerFrame * frame, i, base = 0;

   // add the verts...
   if (vertsPerFrame)
   {
      base = polyList->addPoint(verts[firstVert]);
      for (i=1; i<vertsPerFrame; i++)
         polyList->addPoint(verts[i+firstVert]);
   }

   // add the polys...
   for (i=0; i<primitives.size(); i++)
   {
      TSDrawPrimitive & draw = primitives[i];
      U32 start = draw.start;

      AssertFatal(draw.matIndex & TSDrawPrimitive::Indexed,"TSMesh::buildPolyList (1)");

      U32 material = draw.matIndex & TSDrawPrimitive::MaterialMask;

      // gonna depend on what kind of primitive it is...
      if ( (draw.matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles)
      {
         for (S32 j=0; j<draw.numElements; )
         {
            U32 idx0 = base + indices[start + j + 0];
            U32 idx1 = base + indices[start + j + 1];
            U32 idx2 = base + indices[start + j + 2];
            polyList->begin(material,surfaceKey++);
            polyList->vertex(idx0);
            polyList->vertex(idx1);
            polyList->vertex(idx2);
            polyList->plane(idx0,idx1,idx2);
            polyList->end();
            j += 3;
         }
      }
      else
      {
         AssertFatal((draw.matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Strip,"TSMesh::buildPolyList (2)");

         U32 idx0 = base + indices[start + 0];
         U32 idx1;
         U32 idx2 = base + indices[start + 1];
         U32 * nextIdx = &idx1;
         for (S32 j=2; j<draw.numElements; j++)
         {
            *nextIdx = idx2;
            // nextIdx = (j%2)==0 ? &idx0 : &idx1;
            nextIdx = (U32*) ( (dsize_t)nextIdx ^ (dsize_t)&idx0 ^ (dsize_t)&idx1);
            idx2 = base + indices[start + j];
            if (idx0 == idx1 || idx0 == idx2 || idx1 == idx2)
               continue;

            polyList->begin(material,surfaceKey++);
            polyList->vertex(idx0);
            polyList->vertex(idx1);
            polyList->vertex(idx2);
            polyList->plane(idx0,idx1,idx2);
            polyList->end();
         }
      }
   }
   return true;
}

bool TSMesh::buildPolyListOpcode(const S32 od, AbstractPolyList * polyList, const Box3F &nodeBox)
{
   Opcode::AABBCollider opCollider;
   Opcode::AABBCache opCache;

   IceMaths::AABB opBox;
   opBox.SetMinMax(
      Point(nodeBox.min.x,nodeBox.min.y,nodeBox.min.z),
      Point(nodeBox.max.x,nodeBox.max.y,nodeBox.max.z)
      );

   Opcode::CollisionAABB opCBox(opBox);

   //   opCollider.SetTemporalCoherence(true);
   opCollider.SetPrimitiveTests(true);

   if(opCollider.Collide(opCache, opCBox, *mOptTree))
   {
      U32 cnt = opCollider.GetNbTouchedPrimitives();
      const udword *idx = opCollider.GetTouchedPrimitives();

      Opcode::VertexPointers vp;
      for(S32 i=0; i<cnt; i++)
      {
         // Get the triangle...
         mOptTree->GetMeshInterface()->GetTriangle(vp, idx[i]);

         // And register it in the polylist...
         polyList->begin(0, i);

         U32 plIdx[3];
         for(S32 j=2; j>-1; j--)
         {
            const Point3F tmp(vp.Vertex[j]->x, vp.Vertex[j]->y, vp.Vertex[j]->z);
            plIdx[j] = polyList->addPoint(tmp);
            polyList->vertex(plIdx[j]);
         }

         polyList->plane(plIdx[0], plIdx[2], plIdx[1]);

         polyList->end();

      }

      return true;
   }

   return false;
}

bool TSMesh::buildConvexOpcode(const MatrixF &meshToObjectMat, const Box3F &nodeBox, Convex *convex, Convex *list)
{
   Opcode::AABBCollider opCollider;
   Opcode::AABBCache opCache;

   IceMaths::AABB opBox;
   opBox.SetMinMax(
      Point(nodeBox.min.x,nodeBox.min.y,nodeBox.min.z),
      Point(nodeBox.max.x,nodeBox.max.y,nodeBox.max.z)
      );

   Opcode::CollisionAABB opCBox(opBox);

   //   opCollider.SetTemporalCoherence(true);
   opCollider.SetPrimitiveTests(true);

   if(opCollider.Collide(opCache, opCBox, *mOptTree))
   {
      U32 cnt = opCollider.GetNbTouchedPrimitives();
      const udword *idx = opCollider.GetTouchedPrimitives();

      Opcode::VertexPointers vp;
      for(S32 i=0; i<cnt; i++)
      {
         // First, check our active convexes for a potential match (and clean things
         // up, too.)
         const U32 curIdx = idx[i];

         // See if the square already exists as part of the working set.
         bool gotMatch = false;
         CollisionWorkingList& wl = convex->getWorkingList();
         for (CollisionWorkingList* itr = wl.wLink.mNext; itr != &wl; itr = itr->wLink.mNext)
         {
            if(itr->mConvex->getType() != TSPolysoupConvexType)
               continue;

            const TSStaticPolysoupConvex *chunkc = static_cast<TSStaticPolysoupConvex*>(itr->mConvex);

            if(chunkc->mesh != this)
               continue;

            if(chunkc->idx != curIdx)
               continue;

            // A match! Don't need to add it.
            gotMatch = true;
            break;
         }

         if(gotMatch)
            continue;

         // Get the triangle...
         mOptTree->GetMeshInterface()->GetTriangle(vp, idx[i]);

         Point3F a(vp.Vertex[0]->x,vp.Vertex[0]->y,vp.Vertex[0]->z);
         Point3F b(vp.Vertex[1]->x,vp.Vertex[1]->y,vp.Vertex[1]->z);
         Point3F c(vp.Vertex[2]->x,vp.Vertex[2]->y,vp.Vertex[2]->z);

         // Transform the result into object space!
         meshToObjectMat.mulP(a);
         meshToObjectMat.mulP(b);
         meshToObjectMat.mulP(c);

         PlaneF p(c,b,a);
         Point3F peak = ((a+b+c)/3.0f) - (p*0.15f);

         // Set up the convex...
         TSStaticPolysoupConvex *cp = new TSStaticPolysoupConvex();

         list->registerObject(cp);
         convex->addToWorkingList(cp);

         cp->mesh    = this;
         cp->idx     = curIdx;
         cp->mObject = TSStaticPolysoupConvex::smCurObject;

         cp->normal = p;
         cp->verts[0] = a;
         cp->verts[1] = b;
         cp->verts[2] = c;
         cp->verts[3] = peak;

         // Update the bounding box.
         Box3F &bounds = cp->box;
         bounds.min.set( F32_MAX,  F32_MAX,  F32_MAX);
         bounds.max.set(-F32_MAX, -F32_MAX, -F32_MAX);

         bounds.min.setMin(a);
         bounds.min.setMin(b);
         bounds.min.setMin(c);
         bounds.min.setMin(peak);

         bounds.max.setMax(a);
         bounds.max.setMax(b);
         bounds.max.setMax(c);
         bounds.max.setMax(peak);
      }

      return true;
   }

   return false;
}

bool TSMesh::getFeatures(S32 frame, const MatrixF& mat, const VectorF&, ConvexFeature* cf, U32&)
{
	if(!vertsPerFrame)
		return true;

   S32 firstVert = vertsPerFrame * frame;
   S32 i;
   S32 base = cf->mVertexList.size();

   for (i = 0; i < vertsPerFrame; i++) {
      cf->mVertexList.increment();
      mat.mulP(verts[firstVert + i], &cf->mVertexList.last());
	  //Ray: 获得UV信息
	  cf->mUVList.increment();
	  cf->mUVList.last() = tverts[firstVert + i];
   }

   // add the polys...
   for (i=0; i < primitives.size(); i++)
   {
      TSDrawPrimitive & draw = primitives[i];
      U32 start = draw.start;

      AssertFatal(draw.matIndex & TSDrawPrimitive::Indexed,"TSMesh::buildPolyList (1)");

      // gonna depend on what kind of primitive it is...
      if ( (draw.matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles)
      {
         for (S32 j=0; j<draw.numElements; j+=3)
         {
            PlaneF plane(cf->mVertexList[base + indices[start + j + 0]],
                         cf->mVertexList[base + indices[start + j + 1]],
                         cf->mVertexList[base + indices[start + j + 2]]);

            cf->mFaceList.increment();
            cf->mFaceList.last().normal = plane;

            cf->mFaceList.last().vertex[0] = base + indices[start + j + 0];
            cf->mFaceList.last().vertex[1] = base + indices[start + j + 1];
            cf->mFaceList.last().vertex[2] = base + indices[start + j + 2];

            for (U32 l = 0; l < 3; l++) {
               U32 newEdge0, newEdge1;
               U32 zero = base + indices[start + j + l];
               U32 one  = base + indices[start + j + ((l+1)%3)];
               newEdge0 = getMin(zero, one);
               newEdge1 = getMax(zero, one);
               bool found = false;
               for (S32 k = 0; k < cf->mEdgeList.size(); k++) {
                  if (cf->mEdgeList[k].vertex[0] == newEdge0 &&
                      cf->mEdgeList[k].vertex[1] == newEdge1) {
                     found = true;
                     break;
                  }
               }
               if (!found) {
                  cf->mEdgeList.increment();
                  cf->mEdgeList.last().vertex[0] = newEdge0;
                  cf->mEdgeList.last().vertex[1] = newEdge1;
               }
            }
         }
      }
      else
      {
         AssertFatal((draw.matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Strip,"TSMesh::buildPolyList (2)");

         U32 idx0 = base + indices[start + 0];
         U32 idx1;
         U32 idx2 = base + indices[start + 1];
         U32 * nextIdx = &idx1;
         for (S32 j=2; j<draw.numElements; j++)
         {
            *nextIdx = idx2;
            nextIdx = (U32*) ( (dsize_t)nextIdx ^ (dsize_t)&idx0 ^ (dsize_t)&idx1);
            idx2 = base + indices[start + j];
            if (idx0 == idx1 || idx0 == idx2 || idx1 == idx2)
               continue;

            PlaneF plane(cf->mVertexList[idx0],
                         cf->mVertexList[idx1],
                         cf->mVertexList[idx2]);

            cf->mFaceList.increment();
            cf->mFaceList.last().normal = plane;

            cf->mFaceList.last().vertex[0] = idx0;
            cf->mFaceList.last().vertex[1] = idx1;
            cf->mFaceList.last().vertex[2] = idx2;

            U32 newEdge0, newEdge1;
            newEdge0 = getMin(idx0, idx1);
            newEdge1 = getMax(idx0, idx1);
            bool found = false;
            S32 k;
            for (k = 0; k < cf->mEdgeList.size(); k++) {
               if (cf->mEdgeList[k].vertex[0] == newEdge0 &&
                   cf->mEdgeList[k].vertex[1] == newEdge1) {
                  found = true;
                  break;
               }
            }
            if (!found) {
               cf->mEdgeList.increment();
               cf->mEdgeList.last().vertex[0] = newEdge0;
               cf->mEdgeList.last().vertex[1] = newEdge1;
            }

            newEdge0 = getMin(idx1, idx2);
            newEdge1 = getMax(idx1, idx2);
            found = false;
            for (k = 0; k < cf->mEdgeList.size(); k++) {
               if (cf->mEdgeList[k].vertex[0] == newEdge0 &&
                   cf->mEdgeList[k].vertex[1] == newEdge1) {
                  found = true;
                  break;
               }
            }
            if (!found) {
               cf->mEdgeList.increment();
               cf->mEdgeList.last().vertex[0] = newEdge0;
               cf->mEdgeList.last().vertex[1] = newEdge1;
            }

            newEdge0 = getMin(idx0, idx2);
            newEdge1 = getMax(idx0, idx2);
            found = false;
            for (k = 0; k < cf->mEdgeList.size(); k++) {
               if (cf->mEdgeList[k].vertex[0] == newEdge0 &&
                   cf->mEdgeList[k].vertex[1] == newEdge1) {
                  found = true;
                  break;
               }
            }
            if (!found) {
               cf->mEdgeList.increment();
               cf->mEdgeList.last().vertex[0] = newEdge0;
               cf->mEdgeList.last().vertex[1] = newEdge1;
            }
         }
      }
   }

   return false;
}


void TSMesh::support(S32 frame, const Point3F& v, F32* currMaxDP, Point3F* currSupport)
{
   if (vertsPerFrame == 0)
      return;

   U32 waterMark = FrameAllocator::getWaterMark();
   F32* pDots = (F32*)FrameAllocator::alloc(sizeof(F32) * vertsPerFrame);

   S32 firstVert = vertsPerFrame * frame;
   m_point3F_bulk_dot(&v.x,
                      &verts[firstVert].x,
                      vertsPerFrame,
                      sizeof(Point3F),
                      pDots);

   F32 localdp = *currMaxDP;
   S32 index   = -1;

   for (S32 i = 0; i < vertsPerFrame; i++)
   {
      if (pDots[i] > localdp)
      {
         localdp = pDots[i];
         index   = i;
      }
   }

   FrameAllocator::setWaterMark(waterMark);

   if (index != -1)
   {
      *currMaxDP   = localdp;
      *currSupport = verts[index + firstVert];
   }
}

bool TSMesh::castRay(S32 frame, const Point3F & start, const Point3F & end, RayInfo * rayInfo)
{
#ifdef NTJ_EDITOR
    prepOpcodeCollision();
    rayInfo->t = 1.0f;
    return castRayOpcode(start,end,rayInfo);
#else
   //
   if (planeNormals.empty())
      // if haven't done it yet...
      buildConvexHull();

   // Keep track of startTime and endTime.  They start out at just under 0 and just over 1, respectively.
   // As we check against each plane, prune start and end times back to represent current intersection of
   // line with all the planes (or rather with all the half-spaces defined by the planes).
   // But, instead of explicitly keeping track of startTime and endTime, keep track as numerator and denominator
   // so that we can avoid as many divisions as possible.

   //   F32 startTime = -0.01f;
   F32 startNum = -0.01f;
   F32 startDen =  1.00f;
   //   F32 endTime   = 1.01f;
   F32 endNum = 1.01f;
   F32 endDen = 1.00f;

   S32 curPlane = 0;
   U32 curMaterial = 0;
   bool found = false;

   // the following block of code is an optimization...
   // it isn't necessary if the longer version of the main loop is used
   bool tmpFound;
   S32 tmpPlane;
   F32 sgn = -1.0f;
   F32 * pnum = &startNum;
   F32 * pden = &startDen;
   S32 * pplane = &curPlane;
   bool * pfound = &found;

   S32 startPlane = frame * planesPerFrame;
   for (S32 i=startPlane; i<startPlane+planesPerFrame; i++)
   {
      // if start & end outside, no collision
      // if start & end inside, continue
      // if start outside, end inside, or visa versa, find intersection of line with plane
      //    then update intersection of line with hull (using startTime and endTime)
      F32 dot1 = mDot(planeNormals[i],start) - planeConstants[i];
      F32 dot2 = mDot(planeNormals[i],end) - planeConstants[i];
      if (dot1*dot2>0.0f)
      {
         // same side of the plane...which side -- dot==0 considered inside
         if (dot1>0.0f)
            // start and end outside of this plane, no collision
            return false;
			
         // start and end inside plane, continue
         continue;
      }

	  F32 k = 0;
	  if(dot1!=dot2)
		k = dot1/(dot1-dot2);
      AssertFatal(k>=0.0f && k<=1.0f,"TSMesh::castRay (1)");

      // find intersection (time) with this plane...
      // F32 time = dot1 / (dot1-dot2);
      F32 num = mFabs(dot1);
      F32 den = mFabs(dot1-dot2);

      // the following block of code is an optimized version...
      // this can be commented out and the following block of code used instead
      // if debugging a problem in this code, that should probably be done
      // if you want to see how this works, look at the following block of code,
      // not this one...
      // Note that this does not get optimized appropriately...it is included this way
      // as an idea for future optimization.
      if (sgn*dot1>=0)
      {
         sgn *= -1.0f;
         pnum = (F32*) ((dsize_t)pnum ^ (dsize_t)&endNum ^ (dsize_t)&startNum);
         pden = (F32*) ((dsize_t)pden ^ (dsize_t)&endDen ^ (dsize_t)&startDen);
         pplane = (S32*) ((dsize_t)pplane ^ (dsize_t)&tmpPlane ^ (dsize_t)&curPlane);
         pfound = (bool*) ((dsize_t)pfound ^ (dsize_t)&tmpFound ^ (dsize_t)&found);
      }
      bool noCollision = num*endDen*sgn<endNum*den*sgn && num*startDen*sgn<startNum*den*sgn;
      if (num * *pden * sgn < *pnum * den * sgn && !noCollision)
      {
         *pnum = num;
         *pden = den;
         *pplane = i;
         *pfound = true;
      }
      else if (noCollision)
         return false;
		 

//      if (dot1<=0.0f)
//      {
//         // start is inside plane, end is outside...chop off end
//         if (num*endDen<endNum*den) // if (time<endTime)
//         {
//            if (num*startDen<startNum*den) //if (time<startTime)
//               // no intersection of line and hull
//               return false;
//            // endTime = time;
//            endNum = num;
//            endDen = den;
//         }
//         // else, no need to do anything, just continue (we've been more inside than this)
//      }
//      else // dot2<=0.0f
//      {
//         // end is inside poly, start is outside...chop off start
//         AssertFatal(dot2<=0.0f,"TSMesh::castRay (2)");
//         if (num*startDen>startNum*den) // if (time>startTime)
//        {
//            if (num*endDen>endNum*den) //if (time>endTime)
//               // no intersection of line and hull
//               return false;
//            // startTime   = time;
//            startNum = num;
//            startDen = den;
//            curPlane    = i;
//            curMaterial = planeMaterials[i-startPlane];
//            found = true;
//         }
//         // else, no need to do anything, just continue (we've been more inside than this)
//      }
   }

   // setup rayInfo
   if (found && rayInfo)
   {
      rayInfo->t        = (F32)startNum/(F32)startDen; // finally divide...
      rayInfo->normal   = planeNormals[curPlane];
      rayInfo->material = curMaterial;
      return true;
   }
   else if (found)
      return true;

   // only way to get here is if start is inside hull...
   // we could return null and just plug in garbage for the material and normal...
   return false;

#endif
}

bool TSMesh::fillData(shadowParam &param,const Point3F &p0,const Point3F &p1,const Point3F &p2, const Point2F &t0,const Point2F &t1,const Point2F &t2)
{
	Point3F &normal = param.N;
	mCross(p2-p0,p1-p0,&param.N);
	if (mDot(param.N,param.N)<0.001f)
	{
		mCross(p0-p1,p2-p1,&normal);
		if (mDot(normal,normal)<0.001f)
		{
			mCross(p1-p2,p0-p2,&normal);
			if (mDot(normal,normal)<0.001f)
				return false;
		}
	}
	param.D = mDot(param.N,p0);

	param.p0 = p0;
	param.p1 = p1;
	param.p2 = p2;

	float max_val = getMax(getMax(param.N.x,param.N.y),param.N.z);
	if(param.N.x == max_val){
		param.type = 0;
		param.x1 = p1.y - p0.y;
		param.x2 = p2.y - p0.y;
		param.y1 = p1.z - p0.z;
		param.y2 = p2.z - p0.z;
	}else if(param.N.y == max_val){
		param.type = 1;
		param.x1 = p1.x - p0.x;
		param.x2 = p2.x - p0.x;
		param.y1 = p1.z - p0.z;
		param.y2 = p2.z - p0.z;
	}else if(param.N.z == max_val){
		param.type = 2;
		param.x1 = p1.x - p0.x;
		param.x2 = p2.x - p0.x;
		param.y1 = p1.y - p0.y;
		param.y2 = p2.y - p0.y;
	}

	float temp = param.x1* param.y2 - param.y1 * param.x2;
	if(temp == 0.0f)
		return false;


	Point2F a,b,c;

	if (param.type == 0)
	{
		a = Point2F(p0.y,p0.z);
		b = Point2F(p1.y,p1.z);
		c = Point2F(p2.y,p2.z);
	}
	else if (param.type == 1)
	{
		a = Point2F(p0.x,p0.z);
		b = Point2F(p1.x,p1.z);
		c = Point2F(p2.x,p2.z);
	}
	else if (param.type == 2)
	{
		a = Point2F(p0.x,p0.y);
		b = Point2F(p1.x,p1.y);
		c = Point2F(p2.x,p2.y);
	}

	float x0 = a.x;
	float y0 = a.y;
	float x1 = b.x;
	float y1 = b.y;
	float x2 = c.x;
	float y2 = c.y;

	float u0 = t0.x;
	float v0 = t0.y;
	float u1 = t1.x;
	float v1 = t1.y;
	float u2 = t2.x;
	float v2 = t2.y;

	float denominator = (y0-y2) * (x1-x2) - (y1-y2) * (x0-x2);
	param.dpdx = ((u1-u2) * (y0-y2) - (u0-u2) * (y1-y2))/denominator;
	param.dpdy = ((u1-u2) * (x0-x2) - (u0-u2) * (x1-x2))/-denominator;
	param.dqdx = ((v1-v2) * (y0-y2) - (v0-v2) * (y1-y2))/denominator;
	param.dqdy = ((v1-v2) * (x0-x2) - (v0-v2) * (x1-x2))/-denominator;

	param.u0 = u0;
	param.v0 = v0;
	param.tx0 = x0;
	param.ty0 = y0;

	return true;
}

void TSMesh::calculateShadowTemp()
{
	initShadowTemp = true;
	S32 firstVert  = 0;

	Vector<shadowParam> x;
	shadowParam param;
	for (int i=0; i<primitives.size(); i++)
	{
		TSDrawPrimitive & draw = primitives[i];
		U32 base = draw.start;

		shadowTemp.push_back( x );
		Vector<shadowParam> &temp = shadowTemp.last();

		// gonna depend on what kind of primitive it is...
		if ( (draw.matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles)
		{
			for (int j=0; j<draw.numElements; j+=3)
			{
				Point3F &v0 = verts[indices[base+j+0]+firstVert];
				Point3F &v1 = verts[indices[base+j+1]+firstVert];
				Point3F &v2 = verts[indices[base+j+2]+firstVert];

				Point2F &tv0 = tverts[indices[base+j+2]+firstVert];
				Point2F &tv1 = tverts[indices[base+j+2]+firstVert];
				Point2F &tv2 = tverts[indices[base+j+2]+firstVert];

				if(fillData(param,v0,v1,v2,tv0,tv1,tv2))
				{
					temp.push_back(param);
				}
			}
		}
		else
		{
			U32 idx0 = indices[base + 0] + firstVert;
			U32 idx1;
			U32 idx2 = indices[base + 1] + firstVert;
			U32 * nextIdx = &idx1;
			for (int j=2; j<draw.numElements; j++)
			{
				*nextIdx = idx2;
				//               nextIdx = (j%2)==0 ? &idx0 : &idx1;
				nextIdx = (U32*) ( (dsize_t)nextIdx ^ (dsize_t)&idx0 ^ (dsize_t)&idx1);
				idx2 = indices[base + j] + firstVert;

				Point3F &v0 = verts[idx0];
				Point3F &v1 = verts[idx1];
				Point3F &v2 = verts[idx2];

				Point2F &tv0 = tverts[idx0];
				Point2F &tv1 = tverts[idx1];
				Point2F &tv2 = tverts[idx2];

				if(fillData(param,v0,v1,v2,tv0,tv1,tv2))
				{
					temp.push_back(param);
				}
			}
		}
	}
}

bool TSMesh::LineTrianglesCollision(const Point3F & start, const Point3F & rayDelta,const shadowParam &param,
									GFXTexHandle &tex,GFXLockedRect *&pRect,RayInfo * rayInfo)
{
	//Ray: 这个算法参见《3D数学基础》 射线和三角形碰撞
	float minT = 1.0f;
	float dot = mDot(param.N,rayDelta);
	if(dot >= 0.0f)
		return false;

	float t = param.D - mDot(param.N,start);
	if(t>0.0f)
		return false;

	if(t<dot*minT)
		return false;

	t/=dot;

	Point3F p = start + rayDelta * t;
	float x0,y0;

	if(param.type == 0){
		x0 = p.y - param.p0.y;
		y0 = p.z - param.p0.z;
	}else if(param.type == 1){
		x0 = p.x - param.p0.x;
		y0 = p.z - param.p0.z;
	}else if(param.type == 2){
		x0 = p.x - param.p0.x;
		y0 = p.y - param.p0.y;
	}

	float temp = param.x1* param.y2 - param.y1 * param.x2;

	float alpha = (x0*param.y2 - y0*param.x2) / temp;
	if(alpha<0.0f)
		return false;

	float beta = (param.x1*y0 - param.y1*x0) / temp;
	if(beta<0.0f)
		return false;

	float gamma = 1.0f - alpha - beta;
	if(gamma<0.0f)
		return false;


	//如果要更加精确可以继续计算出这个点的插值颜色来
	if(tex->mFormat!=GFXFormatR8G8B8X8 && tex->mFormat!=GFXFormatR8G8B8A8 && tex->mFormat!=GFXFormatDXT5)
		return true;

	int Width = tex.getWidth();
	int Length = tex.getHeight();

	if(!pRect)
	{
		RectI rect(0,0,Width,Length);
		pRect = tex.lock(0,&rect);
		AssertFatal(pRect,"无法锁定纹理");
	}

	float x,y;

	if(param.type == 0){
		x = p.y;
		y = p.z;
	}else if(param.type == 1){
		x = p.x;
		y = p.z;
	}else if(param.type == 2){
		x = p.x;
		y = p.y;
	}

	float dx = x - param.tx0;
	float dy = y - param.ty0;

	float pu = (param.u0) + (param.dpdx * dx) + (param.dpdy * dy);
	float pv = (param.v0) + (param.dqdx * dx) + (param.dqdy * dy);
	U32 xpos = pu * Width;
	U32 ypos = pv * Length;
	xpos%=Width;
	ypos%=Length;

	U8 *pixel = pRect->bits + (ypos * Width/*pRect->pitch*/) + 4 * xpos;
	U8 A = pixel[3];
	if(!A)
		return false;

	return true;
}

bool TSMesh::castShadow(S32 frame, const Point3F & start, const Point3F & end, TSMaterialList *materials, RayInfo * rayInfo)
{
	if(frame!=0)
		return false;

	if(!initShadowTemp)
		calculateShadowTemp();

	Point3F rayDelta = end - start;
	for (int i=0; i<primitives.size(); i++)
	{
		TSDrawPrimitive & draw = primitives[i];
		U32 matIndex = draw.matIndex & TSDrawPrimitive::MaterialMask;
		GFXTexHandle &tex = materials->getMaterial(matIndex);
		GFXLockedRect *pRect = NULL;

		Vector<shadowParam> &temp = shadowTemp[i];
		for(int i=0;i<temp.size();i++)
		{
			if(LineTrianglesCollision(start,rayDelta,temp[i],tex,pRect,rayInfo))
			{
				if(pRect)
				{
					tex.unlock();
					pRect = NULL;
				}
				return true;
			}
		}

		if(pRect)
		{
			tex.unlock();
			pRect = NULL;
		}
	}

	return false;
}

//bool TSMesh::LineTrianglesCollision(const Point3F & start, const Point3F & rayDelta,
//									const Point3F &p0,const Point3F &p1,const Point3F &p2,
//									const Point2F &t0,const Point2F &t1,const Point2F &t2,
//									GFXTexHandle &tex,GFXLockedRect *&pRect,RayInfo * rayInfo)
//{
//	//Ray: 这个算法参见《3D数学基础》 射线和三角形碰撞
//	float minT = 1.0f;
//	Point3F e1 = p1 - p0;
//	Point3F e2 = p2 - p1;
//	Point3F n = mCross(e1,e2);
//	float dot = mDot(n,rayDelta);
//	if(dot >= 0.0f)
//		return false;
//
//	float d = mDot(n,p0);
//	float t = d - mDot(n,start);
//	if(t>0.0f)
//		return false;
//
//	if(t<dot*minT)
//		return false;
//
//	t/=dot;
//
//	Point3F p = start + rayDelta * t;
//	float x,x0,x1,x2;
//	float y,y0,y1,y2;
//	
//	float max_val = getMax(getMax(n.x,n.y),n.z);
//
//	if(n.x == max_val){
//		x0 = p.y - p0.y;
//		x1 = p1.y - p0.y;
//		x2 = p2.y - p0.y;
//		y0 = p.z - p0.z;
//		y1 = p1.z - p0.z;
//		y2 = p2.z - p0.z;
//
//	}else 
//	if(n.z == max_val){
//		x0 = p.x - p0.x;
//		x1 = p1.x - p0.x;
//		x2 = p2.x - p0.x;
//		y0 = p.y - p0.y;
//		y1 = p1.y - p0.y;
//		y2 = p2.y - p0.y;
//	}else
//	if(n.y == max_val){
//		x0 = p.x - p0.x;
//		x1 = p1.x - p0.x;
//		x2 = p2.x - p0.x;
//		y0 = p.z - p0.z;
//		y1 = p1.z - p0.z;
//		y2 = p2.z - p0.z;
//	}
//
//	float temp = x1* y2 - y1 * x2;
//	if(temp == 0.0f)
//		return false;
//
//	temp = 1.0f/temp;
//
//	float alpha = (x0*y2 - y0*x2) * temp;
//	if(alpha<0.0f)
//		return false;
//
//	float beta = (x1*y0 - y1*x0) * temp;
//	if(beta<0.0f)
//		return false;
//
//	float gamma = 1.0f - alpha - beta;
//	if(gamma<0.0f)
//		return false;
//
//
//	//如果要更加精确可以继续计算出这个点的插值颜色来
//	if(tex->mFormat!=GFXFormatR8G8B8X8 && tex->mFormat!=GFXFormatR8G8B8A8 && tex->mFormat!=GFXFormatDXT5)
//		return true;
//
//	int Width = tex.getWidth();
//	int Length = tex.getHeight();
//
//	if(!pRect)
//	{
//		RectI rect(0,0,Width,Length);
//		pRect = tex.lock(0,&rect);
//	}
//
//	Point2F a,b,c,o;
//
//	if (n.x == max_val)
//	{
//		a = Point2F(p0.y,p0.z);
//		b = Point2F(p1.y,p1.z);
//		c = Point2F(p2.y,p2.z);
//		o = Point2F(p.y,p.z);
//	}
//	else
//	if (n.y == max_val)
//	{
//		a = Point2F(p0.x,p0.z);
//		b = Point2F(p1.x,p1.z);
//		c = Point2F(p2.x,p2.z);
//		o = Point2F(p.x,p.z);
//	}
//	else
//	if (n.z == max_val)
//	{
//		a = Point2F(p0.x,p0.y);
//		b = Point2F(p1.x,p1.y);
//		c = Point2F(p2.x,p2.y);
//		o = Point2F(p.x,p.y);
//	}
//
//	x  = o.x;
//	y  = o.y;
//
//	x0 = a.x;
//	y0 = a.y;
//	x1 = b.x;
//	y1 = b.y;
//	x2 = c.x;
//	y2 = c.y;
//
//	float u0 = t0.x;
//	float v0 = t0.y;
//	float u1 = t1.x;
//	float v1 = t1.y;
//	float u2 = t2.x;
//	float v2 = t2.y;
//
//	float denominator = (y0-y2) * (x1-x2) - (y1-y2) * (x0-x2);
//
//	float dx = x - x0;
//	float dy = y - y0;
//
//	float dpdx = ((u1-u2) * (y0-y2) - (u0-u2) * (y1-y2))/denominator;
//	float dpdy = ((u1-u2) * (x0-x2) - (u0-u2) * (x1-x2))/-denominator;
//	float dqdx = ((v1-v2) * (y0-y2) - (v0-v2) * (y1-y2))/denominator;
//	float dqdy = ((v1-v2) * (x0-x2) - (v0-v2) * (x1-x2))/-denominator;
//
//	float pu = (u0) + (dpdx * dx) + (dpdy * dy);
//	float pv = (v0) + (dqdx * dx) + (dqdy * dy);
//	U32 xpos = pu * Width;
//	U32 ypos = pv * Length;
//	xpos%=Width;
//	ypos%=Length;
//
//	U8 *pixel = pRect->bits + (xpos * pRect->pitch) + 4 * ypos;
//	U8 A = pixel[3];
//	if(!A)
//		return false;
//
//	return true;
//}
//
//bool TSMesh::castShadow(S32 frame, const Point3F & start, const Point3F & end, TSMaterialList *materials, RayInfo * rayInfo)
//{
//	if(frame!=0)
//		return false;
//
//	//Ray: 修改自castRay，碰撞即返回，不必计算最小t
//
//	Point3F rayDelta = end - start;
//	S32 firstVert  = vertsPerFrame * frame;
//
//	for (int i=0; i<primitives.size(); i++)
//	{
//		TSDrawPrimitive & draw = primitives[i];
//		U32 base = draw.start;
//
//		U32 matIndex = draw.matIndex & TSDrawPrimitive::MaterialMask;
//		GFXTexHandle &tex = materials->getMaterial(matIndex);
//		GFXLockedRect *pRect = NULL;
//		// gonna depend on what kind of primitive it is...
//		if ( (draw.matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles)
//		{
//			for (int j=0; j<draw.numElements; j+=3)
//			{
//				Point3F &v0 = verts[indices[base+j+0]+firstVert];
//				Point3F &v1 = verts[indices[base+j+1]+firstVert];
//				Point3F &v2 = verts[indices[base+j+2]+firstVert];
//
//				Point2F &tv0 = tverts[indices[base+j+2]+firstVert];
//				Point2F &tv1 = tverts[indices[base+j+2]+firstVert];
//				Point2F &tv2 = tverts[indices[base+j+2]+firstVert];
//
//				if ( LineTrianglesCollision(start,rayDelta,v0,v1,v2,tv0,tv1,tv2,tex,pRect,rayInfo))
//				{
//					if(pRect)
//						tex.unlock();
//					return true;
//				}
//			}
//		}
//		else
//		{
//			U32 idx0 = indices[base + 0] + firstVert;
//			U32 idx1;
//			U32 idx2 = indices[base + 1] + firstVert;
//			U32 * nextIdx = &idx1;
//			for (int j=2; j<draw.numElements; j++)
//			{
//				*nextIdx = idx2;
//				//               nextIdx = (j%2)==0 ? &idx0 : &idx1;
//				nextIdx = (U32*) ( (dsize_t)nextIdx ^ (dsize_t)&idx0 ^ (dsize_t)&idx1);
//				idx2 = indices[base + j] + firstVert;
//
//				Point3F &v0 = verts[idx0];
//				Point3F &v1 = verts[idx1];
//				Point3F &v2 = verts[idx2];
//
//				Point2F &tv0 = tverts[idx0];
//				Point2F &tv1 = tverts[idx1];
//				Point2F &tv2 = tverts[idx2];
//
//				if ( LineTrianglesCollision(start,rayDelta,v0,v1,v2,tv0,tv1,tv2,tex,pRect,rayInfo))
//				{
//					if(pRect)
//						tex.unlock();
//					return true;
//				}
//			}
//		}
//
//		if(pRect)
//			tex.unlock();
//	}
//
//	return false;
//}

bool TSMesh::addToHull(U32 idx0, U32 idx1, U32 idx2)
{
   Point3F normal;
   mCross(verts[idx2]-verts[idx0],verts[idx1]-verts[idx0],&normal);
   if (mDot(normal,normal)<0.001f)
   {
      mCross(verts[idx0]-verts[idx1],verts[idx2]-verts[idx1],&normal);
      if (mDot(normal,normal)<0.001f)
      {
         mCross(verts[idx1]-verts[idx2],verts[idx0]-verts[idx2],&normal);
         if (mDot(normal,normal)<0.001f)
            return false;
      }
   }
   normal.normalize();
   F32 k = mDot(normal,verts[idx0]);
   for (S32 i=0; i<planeNormals.size(); i++)
   {
      if (mDot(planeNormals[i],normal)>0.99f && mFabs(k-planeConstants[i])<0.01f)
         // this is a repeat...
         return false;
   }
   // new plane, add it to the list...
   planeNormals.push_back(normal);
   planeConstants.push_back(k);
   return true;
}

bool TSMesh::buildConvexHull()
{
   // already done, return without error
   if (planeNormals.size())
      return true;

   bool error = false;

   // should probably only have 1 frame, but just in case...
   planesPerFrame = 0;
   S32 frame, i, j;
   for (frame=0; frame<numFrames; frame++)
   {
      S32 firstVert  = vertsPerFrame * frame;
      S32 firstPlane = planeNormals.size();
      for (i=0; i<primitives.size(); i++)
      {
         TSDrawPrimitive & draw = primitives[i];
         U32 start = draw.start;

         AssertFatal(draw.matIndex & TSDrawPrimitive::Indexed,"TSMesh::buildConvexHull (1)");

         // gonna depend on what kind of primitive it is...
         if ( (draw.matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles)
         {
            for (j=0; j<draw.numElements; j+=3)
               if (addToHull(indices[start+j+0]+firstVert,indices[start+j+1]+firstVert,indices[start+j+2]+firstVert) && frame==0)
                  planeMaterials.push_back(draw.matIndex & TSDrawPrimitive::MaterialMask);
         }
         else
         {
            AssertFatal((draw.matIndex&TSDrawPrimitive::Strip) == TSDrawPrimitive::Strip,"TSMesh::buildConvexHull (2)");

            U32 idx0 = indices[start + 0] + firstVert;
            U32 idx1;
            U32 idx2 = indices[start + 1] + firstVert;
            U32 * nextIdx = &idx1;
            for (j=2; j<draw.numElements; j++)
            {
               *nextIdx = idx2;
//               nextIdx = (j%2)==0 ? &idx0 : &idx1;
               nextIdx = (U32*) ( (dsize_t)nextIdx ^ (dsize_t)&idx0 ^ (dsize_t)&idx1);
               idx2 = indices[start + j] + firstVert;
               if (addToHull(idx0,idx1,idx2) && frame==0)
                  planeMaterials.push_back(draw.matIndex & TSDrawPrimitive::MaterialMask);
            }
         }
      }
      // make sure all the verts on this frame are inside all the planes
      for (i=0; i<vertsPerFrame; i++)
         for (j=firstPlane; j<planeNormals.size(); j++)
            if (mDot(verts[firstVert+i],planeNormals[j])-planeConstants[j]<0.01) // .01 == a little slack
               error = true;
      if (frame==0 || planesPerFrame==0)
         planesPerFrame = planeNormals.size();

      if ( (frame+1) * planesPerFrame != planeNormals.size() )
      {
         // eek, not all frames have same number of planes...
         while ( (frame+1) * planesPerFrame > planeNormals.size() )
         {
            // we're short, duplicate last plane till we match
            U32 sz = planeNormals.size();
            planeNormals.increment();
            planeNormals.last() = planeNormals[sz-1];
            planeConstants.increment();
            planeConstants.last() = planeConstants[sz-1];
         }
         while ( (frame+1) * planesPerFrame < planeNormals.size() )
         {
            // harsh -- last frame has more than other frames
            // duplicate last plane in each frame
            for (S32 k=frame-1; k>=0; k--)
            {
               planeNormals.insert(k*planesPerFrame+planesPerFrame);
               planeNormals[k*planesPerFrame+planesPerFrame] = planeNormals[k*planesPerFrame+planesPerFrame-1];
               planeConstants.insert(k*planesPerFrame+planesPerFrame);
               planeConstants[k*planesPerFrame+planesPerFrame] = planeConstants[k*planesPerFrame+planesPerFrame-1];
               if (k==0)
               {
                  planeMaterials.increment();
                  planeMaterials.last() = planeMaterials[planeMaterials.size()-2];
               }
            }
            planesPerFrame++;
         }
      }
      AssertFatal((frame+1) * planesPerFrame == planeNormals.size(),"TSMesh::buildConvexHull (3)");
   }
   return !error;
}

//-----------------------------------------------------
// TSMesh bounds methods
//-----------------------------------------------------

void TSMesh::computeBounds()
{
   MatrixF mat(true);
   computeBounds(mat,mBounds,-1,&mCenter,&mRadius);
}

void TSMesh::computeBounds(MatrixF & transform, Box3F & bounds, S32 frame, Point3F * center, F32 * radius)
{
   if (frame<0)
      computeBounds(verts.address(),verts.size(),transform,bounds,center,radius);
   else
      computeBounds(verts.address() + frame * vertsPerFrame,vertsPerFrame,transform,bounds,center,radius);
}

void TSMesh::computeBounds(Point3F * v, S32 numVerts, MatrixF & transform, Box3F & bounds, Point3F * center, F32 * radius)
{
   if (!numVerts)
   {
      bounds.min.set(0,0,0);
      bounds.max.set(0,0,0);
      if (center)
         center->set(0,0,0);
      if (radius)
         *radius = 0;
      return;
   }

   S32 i;
   Point3F p;
   transform.mulP(*v,&bounds.min);
   bounds.max = bounds.min;
   for (i=0; i<numVerts; i++)
   {
      transform.mulP(v[i],&p);
      bounds.max.setMax(p);
      bounds.min.setMin(p);
   }
   Point3F c;
   if (!center)
      center = &c;
   center->x = 0.5f * (bounds.min.x + bounds.max.x);
   center->y = 0.5f * (bounds.min.y + bounds.max.y);
   center->z = 0.5f * (bounds.min.z + bounds.max.z);
   if (radius)
   {
      *radius = 0.0f;
      for (i=0; i<numVerts; i++)
      {
         transform.mulP(v[i],&p);
         p -= *center;
         *radius = getMax(*radius,mDot(p,p));
      }
      *radius = mSqrt(*radius);
   }
}

//-----------------------------------------------------

S32 TSMesh::getNumPolys()
{
   S32 count = 0;
   for (S32 i=0; i<primitives.size(); i++)
   {
      if ((primitives[i].matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles)
         count += primitives[i].numElements / 3;
      else
         count += primitives[i].numElements - 2;
   }
   return count;
}

//-----------------------------------------------------
// TSMesh destructor
//-----------------------------------------------------

TSMesh::~TSMesh()
{
	if(mpVB)
	{
		delete mpVB;
		mpVB = NULL;
	}
}

//-----------------------------------------------------
// TSSkinMesh methods
//-----------------------------------------------------
GFXVertexBufferHandleBase *TSSkinMesh::getVertexBuffer()
{
	if(!TSShapeInstance::isNormalSkin && (TSShapeInstance::supportsVertTex || TSShapeInstance::supportsR2VB))
		return mpVB;
	else
	{
		// make sure we're playing with a full deck here...
		AssertFatal(TSShapeInstance::smRenderData.currentObjectInstance &&
			TSShapeInstance::smRenderData.currentObjectInstance->getMesh(TSShapeInstance::smRenderData.detailLevel) == this, 
			"doh, current mesh object instance is not this mesh");

		// use the vertex buffer from the object instance
		return TSShapeInstance::smRenderData.currentObjectInstance->mpVB;
	}
}

void TSSkinMesh::setVertexBuffer(GFXVertexBufferHandleBase *pVB)
{
	if(!TSShapeInstance::isNormalSkin &&  (TSShapeInstance::supportsVertTex || TSShapeInstance::supportsR2VB))
		mpVB = pVB;
	else
		TSShapeInstance::smRenderData.currentObjectInstance->mpVB = pVB;
}

GFXTextureObject *TSSkinMesh::getRVertexTexture(void *pObj)
{
	TSShapeInstance::MeshObjectInstance* pMeshObj = (TSShapeInstance::MeshObjectInstance*)pObj;
	if(pMeshObj)
		return pMeshObj->m_pRVertexTex->getPointer();
	else
		return TSShapeInstance::smRenderData.currentObjectInstance->m_pRVertexTex->getPointer();
}

void TSSkinMesh::setRVertexTexture(void* pObj,U32 width, U32 height, GFXFormat format, GFXTextureProfile *profile)
{
	TSShapeInstance::MeshObjectInstance* pMeshObj = (TSShapeInstance::MeshObjectInstance*)pObj;
	if(pMeshObj)
		pMeshObj->m_pRVertexTex->set(width,height,format,profile);
	else
		TSShapeInstance::smRenderData.currentObjectInstance->m_pRVertexTex->set(width,height,format,profile);
}

GFXTextureObject *TSSkinMesh::getCVertexTexture(void* pObj)
{
	TSShapeInstance::MeshObjectInstance* pMeshObj = (TSShapeInstance::MeshObjectInstance*)pObj;
	if(pMeshObj)
		return pMeshObj->m_pCVertexTex->getPointer();
	else
		return TSShapeInstance::smRenderData.currentObjectInstance->m_pCVertexTex->getPointer();
}

void TSSkinMesh::setCVertexTexture(void* pObj,U32 width, U32 height, GFXFormat format, GFXTextureProfile *profile)
{
	TSShapeInstance::MeshObjectInstance* pMeshObj = (TSShapeInstance::MeshObjectInstance*)pObj;
	if(pMeshObj)
		pMeshObj->m_pCVertexTex->set(width,height,format,profile);
	else
		TSShapeInstance::smRenderData.currentObjectInstance->m_pCVertexTex->set(width,height,format,profile);
}

void TSSkinMesh::switchVertexTexture(void * pObj)
{
	TSShapeInstance::MeshObjectInstance* pMeshObj = (TSShapeInstance::MeshObjectInstance*)pObj;
	if(!pMeshObj)
		pMeshObj = TSShapeInstance::smRenderData.currentObjectInstance;

	GFXTexHandle *pTmp = pMeshObj->m_pCVertexTex;
	pMeshObj->m_pCVertexTex = pMeshObj->m_pRVertexTex;
	pMeshObj->m_pRVertexTex = pTmp;
}


static void sZeroVertexArrays( Point3F * vtx, U32 vtxCount, Point3F *nrm, U32 nrmCount )
{
	dMemset(vtx,0,sizeof(Point3F)*vtxCount);
	dMemset(nrm,0,sizeof(Point3F)*nrmCount);

   //if( vtxCount == nrmCount )
   //{
   //   for( U32 i = 0; i < vtxCount; ++i )
   //   {
   //      Point3F& v = vtx[i];
   //      v.x = 0.0f; v.y = 0.0f; v.z = 0.0f;

   //      Point3F& n = nrm[i];
   //      n.x = 0.0f; n.y = 0.0f; n.z = 0.0f;
   //   }
   //}

   //else
   //{
   //   for( U32 i = 0; i < vtxCount; ++i )
   //   {
   //      Point3F& v = vtx[i];
   //      v.x = 0.0f; v.y = 0.0f; v.z = 0.0f;
   //   }

   //   for( U32 i = 0; i < nrmCount; ++i )
   //   {
   //      Point3F& n = nrm[i];
   //      n.x = 0.0f; n.y = 0.0f; n.z = 0.0f;
   //   }
   //}
}

void TSSkinMesh::skinCallback(void *param)
{
	SkinParam *pSkinData = (SkinParam *)param;
	pSkinData->pMesh->doUpdateSkin(pSkinData);
	delete pSkinData;
}

void TSSkinMesh::doUpdateSkin(SkinParam *pParam)
{
	int nodeSize = nodeIndex.size();
	Vector<MatrixF> tBoneTransforms;
	tBoneTransforms.setSize(nodeSize);
	S32 i;
	for (i=0; i<nodeIndex.size(); i++)
	{
		S32 node = nodeIndex[i];
		tBoneTransforms[i].mul(pParam->nodeTransforms[node],initialTransforms[i]);
	}

	if(TSShapeInstance::supportsVertTex && !pParam->isNormal)
	{
		if(!pParam->pMesh->getCVertexTexture(pParam->pMeshObj))
			pParam->pMesh->setCVertexTexture(pParam->pMeshObj,4*nodeSize,1,GFXFormatR32G32B32A32F,&GFXDefaultVertexMapProfile); 

		GFXTextureObject *pTex = pParam->pMesh->getCVertexTexture(pParam->pMeshObj);
		GFXLockedRect *pRect = pTex->lock();
		if(pRect->bits)
			memcpy(pRect->bits,(F32*)tBoneTransforms.address(),sizeof(F32)*4*4*nodeSize);
		pTex->unlock();
		pParam->pMesh->switchVertexTexture(pParam->pMeshObj);

		///测试 VertTex shader过程-------------------------------------------------------------------------------------------------------
		
		//GFXVertexBufferHandle<GFXVertexPNTTBS>* pvb = (GFXVertexBufferHandle<GFXVertexPNTTBS>*)getVertexBuffer();
		//GFXVertexBufferHandle<GFXVertexPNTTBS> &vb = *pvb;

		//GFXVertexPNTTBS *vbVerts = vb.lock();
		//GFXVertexPNTTBS *pEnd = vbVerts+vertsPerFrame;
		//register Point3F temp;

		//Point3F *pVerts = &initialVerts[0];
		//Point3F *pNorms = &initialNorms[0];

		//GFXTextureObject *pTex1 = pParam->pMesh->getRVertexTexture();
		//GFXLockedRect *pRect1 = pTex1->lock();

		//Vector<Point2F *>list;
		//MatrixF mat; 
		//Point3F tmp,pointTmp,normalTmp;
		//while(vbVerts!=pEnd)
		//{
		//	pointTmp.zero();
		//	normalTmp.zero();
		//	list.clear();

		//	list.push_back(&vbVerts->Bone0);
		//	list.push_back(&vbVerts->Bone1);
		//	list.push_back(&vbVerts->Bone2);
		//	list.push_back(&vbVerts->Bone3);

		//	for(int i=0;i<MAX_BONE_PER_VERTEX;i++)
		//	{
		//		S32 midx = (S32)list[i]->x;
		//		F32 w = list[i]->y;
		//		if(w==0)
		//			break;
		//		
		//		memcpy((F32*)&mat,&pRect1->bits[midx*4*4*4],sizeof(F32)*4*4);
		//		//mat = tBoneTransforms[int(list[i]->x)];
		//		mat.mulP( *pVerts, &tmp );
		//		pointTmp.x += tmp.x * w;
		//		pointTmp.y += tmp.y * w;
		//		pointTmp.z += tmp.z * w;
		//		mat.mulV( *pNorms, &tmp );
		//		normalTmp.x += tmp.x * w;
		//		normalTmp.y += tmp.y * w;
		//		normalTmp.z += tmp.z * w;
		//	}

		//	vbVerts->point = pointTmp;
		//	++pVerts;
		//	vbVerts->normal = normalTmp;
		//	++pNorms;

		//	vbVerts++;
		//}

		//vb.unlock();
		//pTex1->unlock();

		///////-------------------------------------------------------------------------------------------------------
		
		return;
	}
	else
	if(TSShapeInstance::supportsR2VB && !pParam->isNormal)
	{
		if(!pParam->pMesh->getCVertexTexture(pParam->pMeshObj))
			pParam->pMesh->setCVertexTexture(pParam->pMeshObj,4*nodeSize,1,GFXFormatR32G32B32A32F,&GFXDynamicDiffuseProfile); 

		if(!pParam->pMesh->getRVertexTexture(pParam->pMeshObj))
			pParam->pMesh->setRVertexTexture(pParam->pMeshObj,2*initialVerts.size(),1,GFXFormatR32G32B32A32F,&GFXDefaultDMapProfile); 

		GFXTextureObject *pTex = pParam->pMesh->getCVertexTexture(pParam->pMeshObj);
		GFXLockedRect *pRect = pTex->lock();
		if(pRect->bits)
			memcpy(pRect->bits,(F32*)tBoneTransforms.address(),sizeof(F32)*4*4*nodeSize);
		pTex->unlock();

		//static GFXTexHandle testBuf;
		//testBuf.set(2*initialVerts.size(),1,GFXFormatR32G32B32A32F,&GFXDynamicDiffuseProfile);

		RenderInst *coreRI = gRenderInstManager.allocInst();
		coreRI->type = RenderInstManager::RIT_First;
		coreRI->SortedIndex  = 0;  //计算类型
		coreRI->vertexTex	 = pParam->pMesh->getRVertexTexture(pParam->pMeshObj);
		coreRI->normLightmap = mSourceVNTexture.getPointer();
		coreRI->miscTex		 = mBoneIdxTexture.getPointer();
		coreRI->lightmap	 = mWeigthTexture.getPointer();
		coreRI->reflectTex	 = pTex;
		//coreRI->backBuffTex  = testBuf;
		coreRI->vertBuff	 = TSShapeInstance::smQuadVB;
		coreRI->boneNum		 = nodeSize*4;
		gRenderInstManager.addInst(coreRI);

		////测试 R2VB shader过程-------------------------------------------------------------------------------------------------------
		////测试用GFXDefaultDMapDebugProfile
		//GFXTextureObject *pVBT = pParam->pMesh->getRVertexTexture(pParam->pMeshObj);

		//GFXTextureObject *pVBT = testBuf.getPointer();
		//pRect = pVBT->lock();
		//Point4F *pVB = (Point4F *)pRect->bits;

		//GFXTextureObject *pBoneMxT = pParam->pMesh->getCVertexTexture(pParam->pMeshObj);
		//pRect = pBoneMxT->lock();
		//MatrixF *pBoneMx = (MatrixF *)pRect->bits;

		//pRect = mSourceVNTexture.lock();
		//Point4HF *pInitPN = (Point4HF *)pRect->bits;

		//pRect = mBoneIdxTexture.lock();
		//half *pBoneIdx = (half *)pRect->bits;

		//pRect = mWeigthTexture.lock();
		//half *pWeight = (half *)pRect->bits;

		//int NVSize = mSourceVNTexture.getWidth();
		//for(int i=0;i<NVSize;i++)
		//{
		//	//Point3F source = pInitPN[i].asPoint3F();
		//	Point4F source = pInitPN[i].asPoint4F();
		//	Point4F dest(0,0,0,0);
		//	Point4F temp;

		//	for(int j=0;j<MAX_BONE_PER_VERTEX;j++)
		//	{
		//		float BoneWeight = float(pWeight[i*MAX_BONE_PER_VERTEX+j]);
		//		if(BoneWeight==0.0f)
		//			break;

		//		float BoneIdx = (float)pBoneIdx[i*MAX_BONE_PER_VERTEX+j];
		//		temp = source;
		//		pBoneMx[int(BoneIdx/4)].mul(temp);
		//		dest += temp * BoneWeight;
		//	}

		//	dest.w = 1.0f;
		//	pVB[i] = dest;
		//}

		//pVBT->unlock();
		//pBoneMxT->unlock();
		//mSourceVNTexture.unlock();
		//mBoneIdxTexture.unlock();
		//mWeigthTexture.unlock();
		////-------------------------------------------------------------------------------------------------------

		return;
	}

	Vector<Point3F> tSkinVerts;
	Vector<Point3F> tSkinNorms;
	Vector<Point3F> tNormalStore;

	if (encodedNorms.size())
	{
		// we co-opt responsibility for updating encoded normals from mesh
		tNormalStore.setSize(vertsPerFrame);
		for (S32 i=0; i<vertsPerFrame; i++)
			tNormalStore[i]=decodeNormal(encodedNorms[i]);
		initialNorms.set(tNormalStore.address(),vertsPerFrame);
	}
	tSkinVerts.setSize(initialVerts.size());
	tSkinNorms.setSize(initialNorms.size());

	ToolVector<Point3F> tVerts,tNorms;
	tVerts.set(tSkinVerts.address(),tSkinVerts.size());
	tNorms.set(tSkinNorms.address(),tSkinNorms.size());

	const Point3F * inVerts = &initialVerts[0];
	const Point3F * inNorms = &initialNorms[0];

	const MatrixF * matrices = &tBoneTransforms[0];

	Point3F * outVerts = &tSkinVerts[0];
	Point3F * outNorms = &tSkinNorms[0];

	S32 * curVtx = &vertexIndex[0];
	S32 * curBone = &boneIndex[0];
	F32 * curWeight = &weight[0];
	S32   vertexIndexSize = vertexIndex.size();
	const S32 * endVtx = &vertexIndex[vertexIndexSize];

	Point3F srcVtx;
	Point3F srcNrm;

	sZeroVertexArrays( outVerts, verts.size (), outNorms, norms.size() );

	while( curVtx != endVtx )
	{
		const S32 vidx = *curVtx;
		++curVtx;

		const S32 midx = *curBone;
		++curBone;

		const F32 w = *curWeight;
		++curWeight;

		const MatrixF& deltaTransform = matrices[midx];

		deltaTransform.mulP( inVerts[vidx], &srcVtx );

		Point3F& dstVtx = outVerts[vidx];
		dstVtx.x = dstVtx.x + (srcVtx.x * w);
		dstVtx.y = dstVtx.y + (srcVtx.y * w);
		dstVtx.z = dstVtx.z + (srcVtx.z * w);

		deltaTransform.mulV( inNorms[vidx], &srcNrm );

		Point3F& dstNrm = outNorms[vidx];
		dstNrm.x = dstNrm.x + (srcNrm.x * w);
		dstNrm.y = dstNrm.y + (srcNrm.y * w);
		dstNrm.z = dstNrm.z + (srcNrm.z * w);
	}

	createVBIB(pParam->frame, pParam->matFrame, pParam->pVB,&tVerts,&tNorms,pParam->isNormal);

	bFirstUpdate = false;
}

void TSSkinMesh::updateSkin(S32 frame, S32 matFrame)
{
   PROFILE_SCOPE(UpdateSkin);

   TSShapeInstance::MeshObjectInstance* pObjectInstance = TSShapeInstance::smRenderData.currentObjectInstance;
   if(!pObjectInstance)
	   return;

   bool inited = pObjectInstance->inited;
   bool dirty = pObjectInstance->dirty && !smRefractPass;
   bool nullPoint = true;
   bool nullData = true; 

   pObjectInstance->inited = true;

   if(TSShapeInstance::supportsVertTex && !TSShapeInstance::isNormalSkin)
   {
		nullPoint = false;
		GFXTextureObject *pBone = getRVertexTexture(pObjectInstance);
		nullData = !inited || !pBone || pBone->setDirty(false);
   }
   else if(TSShapeInstance::supportsR2VB && !TSShapeInstance::isNormalSkin)
   {
	   //始终同步执行
   }
   else
   {
	   nullPoint = !pObjectInstance->mpVB;
	   if(!nullPoint)
		   nullData = pObjectInstance->mpVB->getPointer()->isNull();
   }

   if(dirty || nullPoint || nullData)
   {
	   SkinParam *pSkinData = new SkinParam;
	   pSkinData->isNormal = TSShapeInstance::isNormalSkin;
	   pSkinData->frame = frame;
	   pSkinData->matFrame = matFrame;
	   pSkinData->pVB = pObjectInstance->mpVB;
	   pSkinData->nodeTransforms = TSShapeInstance::ObjectInstance::smTransforms;
	   pSkinData->pMesh = this;
	   pSkinData->pMeshObj = pObjectInstance;

	   if(TSShapeInstance::isNormalSkin || nullPoint || nullData)
	   {
		   //Ray: 因为涉及到tangent计算和mPB的填充，为了不加锁同步处理顶点的初始操作.

		   if( !initialTangents.size() )
		   {
			   verts.set(initialVerts.address(),initialVerts.size());
			   norms.set(initialNorms.address(),initialNorms.size());
			   createTangents();
		   }

		   skinCallback(pSkinData);
	   }
	   else
	   {
#ifdef USE_MULTITHREAD_ANIMATE
		   if(g_MultThreadWorkMgr->isEnable())
		   {
			   pSkinData->nodeTransforms = TSShapeInstance::ObjectInstance::smRenderTransforms;

			   stThreadParam *pThreadData = new stThreadParam;
			   pThreadData->type = MeshType;
			   pThreadData->pShapeIns = smShapeIns;
			   pThreadData->pObj = smObject;
			   pThreadData->pParam = pSkinData;
			   pThreadData->pFunc = skinCallback;

			   g_MultThreadWorkMgr->addInProcessList(pThreadData);
		   }
		   else
			   skinCallback(pSkinData);
#else
		   skinCallback(pSkinData);
#endif
	   }

	   pObjectInstance->dirty = false;
   }
}

void TSSkinMesh::setVertTexParam(RenderInst *coreRI)
{
	if(!TSShapeInstance::isNormalSkin && (TSShapeInstance::supportsVertTex || TSShapeInstance::supportsR2VB))
	{
		coreRI->vertexTex = getRVertexTexture(NULL);
		coreRI->boneNum = nodeIndex.size()*4;
	}
}

void TSSkinMesh::render(S32 frame, S32 matFrame, TSMaterialList * materials)
{
   // update verts and normals...
   if(!smGlowPass)
      updateSkin(frame, matFrame);

   // render...
   Parent::render(frame,matFrame,materials);
}

//void TSSkinMesh::render(TSMaterialList * materials)
//{
//	if(bFirstUpdate)
//		updateSkin(0,0);
//
//	// render...
//	Parent::render(materials);
//}

//Ray: 这里都会使用碰撞体，而碰撞体在游戏中被设计成不运动的
//bool TSSkinMesh::buildPolyList(S32 frame, AbstractPolyList * polyList, U32 & surfaceKey)
//{
//   // update verts and normals...
//   if( !smGlowPass && !smRefractPass )
//      updateSkin(frame, 0);
//
//   // render...
//   return Parent::buildPolyList(frame,polyList,surfaceKey);
//}

bool TSSkinMesh::castRay(S32 frame, const Point3F & start, const Point3F & end, RayInfo * rayInfo)
{
   frame,start,end,rayInfo;
   return false;
}

bool TSSkinMesh::buildConvexHull()
{
   return false; // no error, but we don't do anything either...
}

void TSSkinMesh::computeBounds(MatrixF & transform, Box3F & bounds, S32 frame, Point3F * center, F32 * radius)
{
   frame;
   TSMesh::computeBounds(initialVerts.address(),initialVerts.size(),transform,bounds,center,radius);
}

//-----------------------------------------------------
// encoded normals
//-----------------------------------------------------

const Point3F TSMesh::smU8ToNormalTable[] =
{
      Point3F( 0.565061f, -0.270644f, -0.779396f ),
      Point3F( -0.309804f, -0.731114f, 0.607860f ),
      Point3F( -0.867412f, 0.472957f, 0.154619f ),
      Point3F( -0.757488f, 0.498188f, -0.421925f ),
      Point3F( 0.306834f, -0.915340f, 0.260778f ),
      Point3F( 0.098754f, 0.639153f, -0.762713f ),
      Point3F( 0.713706f, -0.558862f, -0.422252f ),
      Point3F( -0.890431f, -0.407603f, -0.202466f ),
      Point3F( 0.848050f, -0.487612f, -0.207475f ),
      Point3F( -0.232226f, 0.776855f, 0.585293f ),
      Point3F( -0.940195f, 0.304490f, -0.152706f ),
      Point3F( 0.602019f, -0.491878f, -0.628991f ),
      Point3F( -0.096835f, -0.494354f, -0.863850f ),
      Point3F( 0.026630f, -0.323659f, -0.945799f ),
      Point3F( 0.019208f, 0.909386f, 0.415510f ),
      Point3F( 0.854440f, 0.491730f, 0.167731f ),
      Point3F( -0.418835f, 0.866521f, -0.271512f ),
      Point3F( 0.465024f, 0.409667f, 0.784809f ),
      Point3F( -0.674391f, -0.691087f, -0.259992f ),
      Point3F( 0.303858f, -0.869270f, -0.389922f ),
      Point3F( 0.991333f, 0.090061f, -0.095640f ),
      Point3F( -0.275924f, -0.369550f, 0.887298f ),
      Point3F( 0.426545f, -0.465962f, 0.775202f ),
      Point3F( -0.482741f, -0.873278f, -0.065920f ),
      Point3F( 0.063616f, 0.932012f, -0.356800f ),
      Point3F( 0.624786f, -0.061315f, 0.778385f ),
      Point3F( -0.530300f, 0.416850f, 0.738253f ),
      Point3F( 0.312144f, -0.757028f, -0.573999f ),
      Point3F( 0.399288f, -0.587091f, -0.704197f ),
      Point3F( -0.132698f, 0.482877f, 0.865576f ),
      Point3F( 0.950966f, 0.306530f, 0.041268f ),
      Point3F( -0.015923f, -0.144300f, 0.989406f ),
      Point3F( -0.407522f, -0.854193f, 0.322925f ),
      Point3F( -0.932398f, 0.220464f, 0.286408f ),
      Point3F( 0.477509f, 0.876580f, 0.059936f ),
      Point3F( 0.337133f, 0.932606f, -0.128796f ),
      Point3F( -0.638117f, 0.199338f, 0.743687f ),
      Point3F( -0.677454f, 0.445349f, 0.585423f ),
      Point3F( -0.446715f, 0.889059f, -0.100099f ),
      Point3F( -0.410024f, 0.909168f, 0.072759f ),
      Point3F( 0.708462f, 0.702103f, -0.071641f ),
      Point3F( -0.048801f, -0.903683f, -0.425411f ),
      Point3F( -0.513681f, -0.646901f, 0.563606f ),
      Point3F( -0.080022f, 0.000676f, -0.996793f ),
      Point3F( 0.066966f, -0.991150f, -0.114615f ),
      Point3F( -0.245220f, 0.639318f, -0.728793f ),
      Point3F( 0.250978f, 0.855979f, 0.452006f ),
      Point3F( -0.123547f, 0.982443f, -0.139791f ),
      Point3F( -0.794825f, 0.030254f, -0.606084f ),
      Point3F( -0.772905f, 0.547941f, 0.319967f ),
      Point3F( 0.916347f, 0.369614f, -0.153928f ),
      Point3F( -0.388203f, 0.105395f, 0.915527f ),
      Point3F( -0.700468f, -0.709334f, 0.078677f ),
      Point3F( -0.816193f, 0.390455f, 0.425880f ),
      Point3F( -0.043007f, 0.769222f, -0.637533f ),
      Point3F( 0.911444f, 0.113150f, 0.395560f ),
      Point3F( 0.845801f, 0.156091f, -0.510153f ),
      Point3F( 0.829801f, -0.029340f, 0.557287f ),
      Point3F( 0.259529f, 0.416263f, 0.871418f ),
      Point3F( 0.231128f, -0.845982f, 0.480515f ),
      Point3F( -0.626203f, -0.646168f, 0.436277f ),
      Point3F( -0.197047f, -0.065791f, 0.978184f ),
      Point3F( -0.255692f, -0.637488f, -0.726794f ),
      Point3F( 0.530662f, -0.844385f, -0.073567f ),
      Point3F( -0.779887f, 0.617067f, -0.104899f ),
      Point3F( 0.739908f, 0.113984f, 0.662982f ),
      Point3F( -0.218801f, 0.930194f, -0.294729f ),
      Point3F( -0.374231f, 0.818666f, 0.435589f ),
      Point3F( -0.720250f, -0.028285f, 0.693137f ),
      Point3F( 0.075389f, 0.415049f, 0.906670f ),
      Point3F( -0.539724f, -0.106620f, 0.835063f ),
      Point3F( -0.452612f, -0.754669f, -0.474991f ),
      Point3F( 0.682822f, 0.581234f, -0.442629f ),
      Point3F( 0.002435f, -0.618462f, -0.785811f ),
      Point3F( -0.397631f, 0.110766f, -0.910835f ),
      Point3F( 0.133935f, -0.985438f, 0.104754f ),
      Point3F( 0.759098f, -0.608004f, 0.232595f ),
      Point3F( -0.825239f, -0.256087f, 0.503388f ),
      Point3F( 0.101693f, -0.565568f, 0.818408f ),
      Point3F( 0.386377f, 0.793546f, -0.470104f ),
      Point3F( -0.520516f, -0.840690f, 0.149346f ),
      Point3F( -0.784549f, -0.479672f, 0.392935f ),
      Point3F( -0.325322f, -0.927581f, -0.183735f ),
      Point3F( -0.069294f, -0.428541f, 0.900861f ),
      Point3F( 0.993354f, -0.115023f, -0.004288f ),
      Point3F( -0.123896f, -0.700568f, 0.702747f ),
      Point3F( -0.438031f, -0.120880f, -0.890795f ),
      Point3F( 0.063314f, 0.813233f, 0.578484f ),
      Point3F( 0.322045f, 0.889086f, -0.325289f ),
      Point3F( -0.133521f, 0.875063f, -0.465228f ),
      Point3F( 0.637155f, 0.564814f, 0.524422f ),
      Point3F( 0.260092f, -0.669353f, 0.695930f ),
      Point3F( 0.953195f, 0.040485f, -0.299634f ),
      Point3F( -0.840665f, -0.076509f, 0.536124f ),
      Point3F( -0.971350f, 0.202093f, 0.125047f ),
      Point3F( -0.804307f, -0.396312f, -0.442749f ),
      Point3F( -0.936746f, 0.069572f, 0.343027f ),
      Point3F( 0.426545f, -0.465962f, 0.775202f ),
      Point3F( 0.794542f, -0.227450f, 0.563000f ),
      Point3F( -0.892172f, 0.091169f, -0.442399f ),
      Point3F( -0.312654f, 0.541264f, 0.780564f ),
      Point3F( 0.590603f, -0.735618f, -0.331743f ),
      Point3F( -0.098040f, -0.986713f, 0.129558f ),
      Point3F( 0.569646f, 0.283078f, -0.771603f ),
      Point3F( 0.431051f, -0.407385f, -0.805129f ),
      Point3F( -0.162087f, -0.938749f, -0.304104f ),
      Point3F( 0.241533f, -0.359509f, 0.901341f ),
      Point3F( -0.576191f, 0.614939f, 0.538380f ),
      Point3F( -0.025110f, 0.085740f, 0.996001f ),
      Point3F( -0.352693f, -0.198168f, 0.914515f ),
      Point3F( -0.604577f, 0.700711f, 0.378802f ),
      Point3F( 0.465024f, 0.409667f, 0.784809f ),
      Point3F( -0.254684f, -0.030474f, -0.966544f ),
      Point3F( -0.604789f, 0.791809f, 0.085259f ),
      Point3F( -0.705147f, -0.399298f, 0.585943f ),
      Point3F( 0.185691f, 0.017236f, -0.982457f ),
      Point3F( 0.044588f, 0.973094f, 0.226052f ),
      Point3F( -0.405463f, 0.642367f, 0.650357f ),
      Point3F( -0.563959f, 0.599136f, -0.568319f ),
      Point3F( 0.367162f, -0.072253f, -0.927347f ),
      Point3F( 0.960429f, -0.213570f, -0.178783f ),
      Point3F( -0.192629f, 0.906005f, 0.376893f ),
      Point3F( -0.199718f, -0.359865f, -0.911378f ),
      Point3F( 0.485072f, 0.121233f, -0.866030f ),
      Point3F( 0.467163f, -0.874294f, 0.131792f ),
      Point3F( -0.638953f, -0.716603f, 0.279677f ),
      Point3F( -0.622710f, 0.047813f, -0.780990f ),
      Point3F( 0.828724f, -0.054433f, -0.557004f ),
      Point3F( 0.130241f, 0.991080f, 0.028245f ),
      Point3F( 0.310995f, -0.950076f, -0.025242f ),
      Point3F( 0.818118f, 0.275336f, 0.504850f ),
      Point3F( 0.676328f, 0.387023f, 0.626733f ),
      Point3F( -0.100433f, 0.495114f, -0.863004f ),
      Point3F( -0.949609f, -0.240681f, -0.200786f ),
      Point3F( -0.102610f, 0.261831f, -0.959644f ),
      Point3F( -0.845732f, -0.493136f, 0.203850f ),
      Point3F( 0.672617f, -0.738838f, 0.041290f ),
      Point3F( 0.380465f, 0.875938f, 0.296613f ),
      Point3F( -0.811223f, 0.262027f, -0.522742f ),
      Point3F( -0.074423f, -0.775670f, -0.626736f ),
      Point3F( -0.286499f, 0.755850f, -0.588735f ),
      Point3F( 0.291182f, -0.276189f, -0.915933f ),
      Point3F( -0.638117f, 0.199338f, 0.743687f ),
      Point3F( 0.439922f, -0.864433f, -0.243359f ),
      Point3F( 0.177649f, 0.206919f, 0.962094f ),
      Point3F( 0.277107f, 0.948521f, 0.153361f ),
      Point3F( 0.507629f, 0.661918f, -0.551523f ),
      Point3F( -0.503110f, -0.579308f, -0.641313f ),
      Point3F( 0.600522f, 0.736495f, -0.311364f ),
      Point3F( -0.691096f, -0.715301f, -0.103592f ),
      Point3F( -0.041083f, -0.858497f, 0.511171f ),
      Point3F( 0.207773f, -0.480062f, -0.852274f ),
      Point3F( 0.795719f, 0.464614f, 0.388543f ),
      Point3F( -0.100433f, 0.495114f, -0.863004f ),
      Point3F( 0.703249f, 0.065157f, -0.707951f ),
      Point3F( -0.324171f, -0.941112f, 0.096024f ),
      Point3F( -0.134933f, -0.940212f, 0.312722f ),
      Point3F( -0.438240f, 0.752088f, -0.492249f ),
      Point3F( 0.964762f, -0.198855f, 0.172311f ),
      Point3F( -0.831799f, 0.196807f, 0.519015f ),
      Point3F( -0.508008f, 0.819902f, 0.263986f ),
      Point3F( 0.471075f, -0.001146f, 0.882092f ),
      Point3F( 0.919512f, 0.246162f, -0.306435f ),
      Point3F( -0.960050f, 0.279828f, -0.001187f ),
      Point3F( 0.110232f, -0.847535f, -0.519165f ),
      Point3F( 0.208229f, 0.697360f, 0.685806f ),
      Point3F( -0.199680f, -0.560621f, 0.803637f ),
      Point3F( 0.170135f, -0.679985f, -0.713214f ),
      Point3F( 0.758371f, -0.494907f, 0.424195f ),
      Point3F( 0.077734f, -0.755978f, 0.649965f ),
      Point3F( 0.612831f, -0.672475f, 0.414987f ),
      Point3F( 0.142776f, 0.836698f, -0.528726f ),
      Point3F( -0.765185f, 0.635778f, 0.101382f ),
      Point3F( 0.669873f, -0.419737f, 0.612447f ),
      Point3F( 0.593549f, 0.194879f, 0.780847f ),
      Point3F( 0.646930f, 0.752173f, 0.125368f ),
      Point3F( 0.837721f, 0.545266f, -0.030127f ),
      Point3F( 0.541505f, 0.768070f, 0.341820f ),
      Point3F( 0.760679f, -0.365715f, -0.536301f ),
      Point3F( 0.381516f, 0.640377f, 0.666605f ),
      Point3F( 0.565794f, -0.072415f, -0.821361f ),
      Point3F( -0.466072f, -0.401588f, 0.788356f ),
      Point3F( 0.987146f, 0.096290f, 0.127560f ),
      Point3F( 0.509709f, -0.688886f, -0.515396f ),
      Point3F( -0.135132f, -0.988046f, -0.074192f ),
      Point3F( 0.600499f, 0.476471f, -0.642166f ),
      Point3F( -0.732326f, -0.275320f, -0.622815f ),
      Point3F( -0.881141f, -0.470404f, 0.048078f ),
      Point3F( 0.051548f, 0.601042f, 0.797553f ),
      Point3F( 0.402027f, -0.763183f, 0.505891f ),
      Point3F( 0.404233f, -0.208288f, 0.890624f ),
      Point3F( -0.311793f, 0.343843f, 0.885752f ),
      Point3F( 0.098132f, -0.937014f, 0.335223f ),
      Point3F( 0.537158f, 0.830585f, -0.146936f ),
      Point3F( 0.725277f, 0.298172f, -0.620538f ),
      Point3F( -0.882025f, 0.342976f, -0.323110f ),
      Point3F( -0.668829f, 0.424296f, -0.610443f ),
      Point3F( -0.408835f, -0.476442f, -0.778368f ),
      Point3F( 0.809472f, 0.397249f, -0.432375f ),
      Point3F( -0.909184f, -0.205938f, -0.361903f ),
      Point3F( 0.866930f, -0.347934f, -0.356895f ),
      Point3F( 0.911660f, -0.141281f, -0.385897f ),
      Point3F( -0.431404f, -0.844074f, -0.318480f ),
      Point3F( -0.950593f, -0.073496f, 0.301614f ),
      Point3F( -0.719716f, 0.626915f, -0.298305f ),
      Point3F( -0.779887f, 0.617067f, -0.104899f ),
      Point3F( -0.475899f, -0.542630f, 0.692151f ),
      Point3F( 0.081952f, -0.157248f, -0.984153f ),
      Point3F( 0.923990f, -0.381662f, -0.024025f ),
      Point3F( -0.957998f, 0.120979f, -0.260008f ),
      Point3F( 0.306601f, 0.227975f, -0.924134f ),
      Point3F( -0.141244f, 0.989182f, 0.039601f ),
      Point3F( 0.077097f, 0.186288f, -0.979466f ),
      Point3F( -0.630407f, -0.259801f, 0.731499f ),
      Point3F( 0.718150f, 0.637408f, 0.279233f ),
      Point3F( 0.340946f, 0.110494f, 0.933567f ),
      Point3F( -0.396671f, 0.503020f, -0.767869f ),
      Point3F( 0.636943f, -0.245005f, 0.730942f ),
      Point3F( -0.849605f, -0.518660f, -0.095724f ),
      Point3F( -0.388203f, 0.105395f, 0.915527f ),
      Point3F( -0.280671f, -0.776541f, -0.564099f ),
      Point3F( -0.601680f, 0.215451f, -0.769131f ),
      Point3F( -0.660112f, -0.632371f, -0.405412f ),
      Point3F( 0.921096f, 0.284072f, 0.266242f ),
      Point3F( 0.074850f, -0.300846f, 0.950731f ),
      Point3F( 0.943952f, -0.067062f, 0.323198f ),
      Point3F( -0.917838f, -0.254589f, 0.304561f ),
      Point3F( 0.889843f, -0.409008f, 0.202219f ),
      Point3F( -0.565849f, 0.753721f, -0.334246f ),
      Point3F( 0.791460f, 0.555918f, -0.254060f ),
      Point3F( 0.261936f, 0.703590f, -0.660568f ),
      Point3F( -0.234406f, 0.952084f, 0.196444f ),
      Point3F( 0.111205f, 0.979492f, -0.168014f ),
      Point3F( -0.869844f, -0.109095f, -0.481113f ),
      Point3F( -0.337728f, -0.269701f, -0.901777f ),
      Point3F( 0.366793f, 0.408875f, -0.835634f ),
      Point3F( -0.098749f, 0.261316f, 0.960189f ),
      Point3F( -0.272379f, -0.847100f, 0.456324f ),
      Point3F( -0.319506f, 0.287444f, -0.902935f ),
      Point3F( 0.873383f, -0.294109f, 0.388203f ),
      Point3F( -0.088950f, 0.710450f, 0.698104f ),
      Point3F( 0.551238f, -0.786552f, 0.278340f ),
      Point3F( 0.724436f, -0.663575f, -0.186712f ),
      Point3F( 0.529741f, -0.606539f, 0.592861f ),
      Point3F( -0.949743f, -0.282514f, 0.134809f ),
      Point3F( 0.155047f, 0.419442f, -0.894443f ),
      Point3F( -0.562653f, -0.329139f, -0.758346f ),
      Point3F( 0.816407f, -0.576953f, 0.024576f ),
      Point3F( 0.178550f, -0.950242f, -0.255266f ),
      Point3F( 0.479571f, 0.706691f, 0.520192f ),
      Point3F( 0.391687f, 0.559884f, -0.730145f ),
      Point3F( 0.724872f, -0.205570f, -0.657496f ),
      Point3F( -0.663196f, -0.517587f, -0.540624f ),
      Point3F( -0.660054f, -0.122486f, -0.741165f ),
      Point3F( -0.531989f, 0.374711f, -0.759328f ),
      Point3F( 0.194979f, -0.059120f, 0.979024f )
};

U8 TSMesh::encodeNormal(const Point3F & normal)
{
   U8 bestIndex=0;
   F32 bestDot=-10E30f;
   for (U32 i=0; i<256; i++)
   {
      F32 dot = mDot(normal,smU8ToNormalTable[i]);
      if (dot>bestDot)
      {
         bestIndex = i;
         bestDot = dot;
      }
   }
   return bestIndex;
}

//-----------------------------------------------------
// TSMesh assemble from/ dissemble to memory buffer
//-----------------------------------------------------

#define alloc TSShape::alloc

TSMesh * TSMesh::assembleMesh(U32 meshType, bool skip)
{
   CLockGuard guard(alloc.GetMutex());

   static TSMesh tempStandardMesh;
   static TSSkinMesh tempSkinMesh;
   static TSDecalMesh tempDecalMesh;
   static TSSortedMesh tempSortedMesh;

   bool justSize = skip || !alloc.allocShape32(0); // if this returns NULL, we're just sizing memory block

   // a little funny business because we pretend decals are derived from meshes
   S32 * ret = NULL;
   TSMesh * mesh = NULL;
   TSDecalMesh * decal = NULL;

   if (justSize)
   {
      switch (meshType)
      {
         case StandardMeshType :
         {
            ret = (S32*)&tempStandardMesh;
            mesh = &tempStandardMesh;
            alloc.allocShape32(sizeof(TSMesh)>>2);
            break;
         }
         case SkinMeshType     :
         {
            ret = (S32*)&tempSkinMesh;
            mesh = &tempSkinMesh;
            alloc.allocShape32(sizeof(TSSkinMesh)>>2);
            break;
         }
         case DecalMeshType    :
         {
            ret = (S32*)&tempDecalMesh;
            decal = &tempDecalMesh;
            alloc.allocShape32(sizeof(TSDecalMesh)>>2);
            break;
         }
         case SortedMeshType   :
         {
            ret = (S32*)&tempSortedMesh;
            mesh = &tempSortedMesh;
            alloc.allocShape32(sizeof(TSSortedMesh)>>2);
            break;
         }
      }
   }
   else
   {
      switch (meshType)
      {
         case StandardMeshType :
         {
            ret = alloc.allocShape32(sizeof(TSMesh)>>2);
            constructInPlace((TSMesh*)ret);
            mesh = (TSMesh*)ret;
            break;
         }
         case SkinMeshType     :
         {
            ret = alloc.allocShape32(sizeof(TSSkinMesh)>>2);
            constructInPlace((TSSkinMesh*)ret);
            mesh = (TSSkinMesh*)ret;
            break;
         }
         case DecalMeshType    :
         {
            ret = alloc.allocShape32(sizeof(TSDecalMesh)>>2);
            constructInPlace((TSDecalMesh*)ret);
            decal = (TSDecalMesh*)ret;
            break;
         }
         case SortedMeshType   :
         {
            ret = alloc.allocShape32(sizeof(TSSortedMesh)>>2);
            constructInPlace((TSSortedMesh*)ret);
            mesh = (TSSortedMesh*)ret;
            break;
         }
      }
   }

   alloc.setSkipMode(skip);

   if (mesh)
      mesh->assemble(skip);
   if (decal)
      decal->assemble(skip);

   alloc.setSkipMode(false);

   return (TSMesh*)ret;
}

void TSMesh::convertToTris(S16 * primitiveDataIn, S32 * primitiveMatIn, S16 * indicesIn,
                           S32 numPrimIn, S32 & numPrimOut, S32 & numIndicesOut,
                           S32 * primitivesOut, S16 * indicesOut)
{
   S32 prevMaterial = -99999;
   TSDrawPrimitive * newDraw = NULL;
   numPrimOut=0;
   numIndicesOut=0;
   for (S32 i=0; i<numPrimIn; i++)
   {
      S32 newMat = primitiveMatIn[i];
      newMat &= ~TSDrawPrimitive::TypeMask;
      if (newMat!=prevMaterial)
      {
         if (primitivesOut)
         {
            newDraw = (TSDrawPrimitive*) &primitivesOut[numPrimOut*2];
            newDraw->start = numIndicesOut;
            newDraw->numElements = 0;
            newDraw->matIndex = newMat | TSDrawPrimitive::Triangles;
         }
         numPrimOut++;
         prevMaterial = newMat;
      }
      U16 start = primitiveDataIn[i*2];
      U16 numElements = primitiveDataIn[i*2+1];

      // gonna depend on what kind of primitive it is...
      if ( (primitiveMatIn[i] & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles)
      {
         for (S32 j=0; j<numElements; j+=3)
         {
            if (indicesOut)
            {
               indicesOut[numIndicesOut+0] = indicesIn[start+j+0];
               indicesOut[numIndicesOut+1] = indicesIn[start+j+1];
               indicesOut[numIndicesOut+2] = indicesIn[start+j+2];
            }
            if (newDraw)
               newDraw->numElements += 3;
            numIndicesOut += 3;
         }
      }
      else
      {
         U32 idx0 = indicesIn[start + 0];
         U32 idx1;
         U32 idx2 = indicesIn[start + 1];
         U32 * nextIdx = &idx1;
         for (S32 j=2; j<numElements; j++)
         {
            *nextIdx = idx2;
            nextIdx = (U32*) ( (dsize_t)nextIdx ^ (dsize_t)&idx0 ^ (dsize_t)&idx1);
            idx2 = indicesIn[start + j];
            if (idx0==idx1 || idx1==idx2 || idx2==idx0)
               continue;
            if (indicesOut)
            {
               indicesOut[numIndicesOut+0] = idx0;
               indicesOut[numIndicesOut+1] = idx1;
               indicesOut[numIndicesOut+2] = idx2;
            }
            if (newDraw)
               newDraw->numElements += 3;
            numIndicesOut += 3;
         }
      }
   }
}

void unwindStrip(S16 * indices, S32 numElements, Vector<S16> & triIndices)
{
   U32 idx0 = indices[0];
   U32 idx1;
   U32 idx2 = indices[1];
   U32 * nextIdx = &idx1;
   for (S32 j=2; j<numElements; j++)
   {
      *nextIdx = idx2;
      nextIdx = (U32*) ( (dsize_t)nextIdx ^ (dsize_t)&idx0 ^ (dsize_t)&idx1);
      idx2 = indices[j];
      if (idx0==idx1 || idx1==idx2 || idx2==idx0)
         continue;
      triIndices.push_back(idx0);
      triIndices.push_back(idx1);
      triIndices.push_back(idx2);
   }
}

void TSMesh::convertToSingleStrip(S16 * primitiveDataIn, S32 * primitiveMatIn, S16 * indicesIn, S32 numPrimIn,
                                  S32 & numPrimOut, S32 & numIndicesOut,
                                  S32 * primitivesOut, S16 * indicesOut)
{
   S32 prevMaterial = -99999;
   TSDrawPrimitive * newDraw = NULL;
   TSDrawPrimitive * newTris = NULL;
   Vector<S16> triIndices;
   S32 curDrawOut = 0;
   numPrimOut=0;
   numIndicesOut=0;
   for (S32 i=0; i<numPrimIn; i++)
   {
      S32 newMat = primitiveMatIn[i];
      if (newMat!=prevMaterial)
      {
         // before adding the new primitive, transfer triangle indices
         if (triIndices.size())
         {
            if (newTris && indicesOut)
            {
               newTris->start = numIndicesOut;
               newTris->numElements = triIndices.size();
               dMemcpy(&indicesOut[numIndicesOut],triIndices.address(),triIndices.size()*sizeof(U16));
            }
            numIndicesOut += triIndices.size();
            triIndices.clear();
            newTris = NULL;
         }

         if (primitivesOut)
         {
            newDraw = (TSDrawPrimitive*) &primitivesOut[numPrimOut*2];
            newDraw->start = numIndicesOut;
            newDraw->numElements = 0;
            newDraw->matIndex = newMat;
         }
         numPrimOut++;
         curDrawOut = 0;
         prevMaterial = newMat;
      }
      U16 start = primitiveDataIn[i*2];
      U16 numElements = primitiveDataIn[i*2+1];

      // gonna depend on what kind of primitive it is...
      // from above we know it's the same kind as the one we're building...
      if ( (primitiveMatIn[i] & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles)
      {
         // triangles primitive...add to it
         for (S32 j=0; j<numElements; j+=3)
         {
            if (indicesOut)
            {
               indicesOut[numIndicesOut+0] = indicesIn[start+j+0];
               indicesOut[numIndicesOut+1] = indicesIn[start+j+1];
               indicesOut[numIndicesOut+2] = indicesIn[start+j+2];
            }
            if (newDraw)
               newDraw->numElements += 3;
            numIndicesOut += 3;
         }
      }
      else
      {
         // strip primitive...
         // if numElements less than smSmallestStripSize, add to triangles...
         if (numElements<smMinStripSize+2)
         {
            // put triangle indices aside until material changes...
            if (triIndices.empty())
            {
               // set up for new triangle primitive and add it if we are copying data right now
               if (primitivesOut)
               {
                  newTris = (TSDrawPrimitive*) &primitivesOut[numPrimOut*2];
                  newTris->matIndex  = newMat;
                  newTris->matIndex &= ~(TSDrawPrimitive::Triangles|TSDrawPrimitive::Strip);
                  newTris->matIndex |= TSDrawPrimitive::Triangles;
               }
               numPrimOut++;
            }
            unwindStrip(indicesIn+start,numElements,triIndices);
         }
         else
         {
            // strip primitive...add to it
            if (indicesOut)
            {
               if (curDrawOut&1)
               {
                  indicesOut[numIndicesOut+0] = indicesOut[numIndicesOut-1];
                  indicesOut[numIndicesOut+1] = indicesOut[numIndicesOut-1];
                  indicesOut[numIndicesOut+2] = indicesIn[start];
                  dMemcpy(indicesOut+numIndicesOut+3,indicesIn+start,2*numElements);
               }
               else if (curDrawOut)
               {
                  indicesOut[numIndicesOut+0] = indicesOut[numIndicesOut-1];
                  indicesOut[numIndicesOut+1] = indicesIn[start];
                  dMemcpy(indicesOut+numIndicesOut+2,indicesIn+start,2*numElements);
               }
               else
                  dMemcpy(indicesOut+numIndicesOut,indicesIn+start,2*numElements);
            }
            S32 added = numElements;
            added += curDrawOut ? (curDrawOut&1 ? 3 : 2) : 0;
            if (newDraw)
               newDraw->numElements += added;
            numIndicesOut += added;
            curDrawOut += added;
         }
      }
   }
   // spit out tris before leaving
   // before adding the new primitive, transfer triangle indices
   if (triIndices.size())
   {
      if (newTris && indicesOut)
      {
         newTris->start = numIndicesOut;
         newTris->numElements = triIndices.size();
         dMemcpy(&indicesOut[numIndicesOut],triIndices.address(),triIndices.size()*sizeof(U16));
      }
      numIndicesOut += triIndices.size();
      triIndices.clear();
      newTris = NULL;
   }
}

// this method does none of the converting that the above methods do, except that small strips are converted
// to triangle lists...
void TSMesh::leaveAsMultipleStrips(S16 * primitiveDataIn, S32 * primitiveMatIn, S16 * indicesIn, S32 numPrimIn,
                                   S32 & numPrimOut, S32 & numIndicesOut,
                                   S32 * primitivesOut, S16 * indicesOut)
{
   S32 prevMaterial = -99999;
   TSDrawPrimitive * newDraw = NULL;
   Vector<S16> triIndices;
   numPrimOut=0;
   numIndicesOut=0;
   for (S32 i=0; i<numPrimIn; i++)
   {
      S32 newMat = primitiveMatIn[i];

      U16 start = primitiveDataIn[i*2];
      U16 numElements = primitiveDataIn[i*2+1];

      if (newMat!=prevMaterial && triIndices.size())
      {
         // material just changed and we have triangles lying around
         // add primitive and indices for triangles and clear triIndices
         if (indicesOut)
         {
            TSDrawPrimitive * newTris = (TSDrawPrimitive*) &primitivesOut[numPrimOut*2];
            newTris->matIndex = prevMaterial;
            newTris->matIndex &= ~(TSDrawPrimitive::Triangles|TSDrawPrimitive::Strip);
            newTris->matIndex |= TSDrawPrimitive::Triangles;
            newTris->start = numIndicesOut;
            newTris->numElements = triIndices.size();
            dMemcpy(&indicesOut[numIndicesOut],triIndices.address(),triIndices.size()*sizeof(U16));
         }
         numPrimOut++;
         numIndicesOut += triIndices.size();
         triIndices.clear();
      }

      // this is a little convoluted because this code was adapted from convertToSingleStrip
      // but we will need a new primitive only if it is a triangle primitive coming in
      // or we have more elements than the min strip size...
      if ( (primitiveMatIn[i] & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles || numElements>=smMinStripSize+2)
      {
         if (primitivesOut)
         {
            newDraw = (TSDrawPrimitive*) &primitivesOut[numPrimOut*2];
            newDraw->start = numIndicesOut;
            newDraw->numElements = 0;
            newDraw->matIndex = newMat;
         }
         numPrimOut++;
      }
      prevMaterial = newMat;

      // gonna depend on what kind of primitive it is...
      // from above we know it's the same kind as the one we're building...
      if ( (primitiveMatIn[i] & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles)
      {
         // triangles primitive...add to it
         for (S32 j=0; j<numElements; j+=3)
         {
            if (indicesOut)
            {
               indicesOut[numIndicesOut+0] = indicesIn[start+j+0];
               indicesOut[numIndicesOut+1] = indicesIn[start+j+1];
               indicesOut[numIndicesOut+2] = indicesIn[start+j+2];
            }
            if (newDraw)
               newDraw->numElements += 3;
            numIndicesOut += 3;
         }
      }
      else
      {
         // strip primitive...
         // if numElements less than smSmallestStripSize, add to triangles...
         if (numElements<smMinStripSize+2)
            // put triangle indices aside until material changes...
            unwindStrip(indicesIn+start,numElements,triIndices);
         else
         {
            // strip primitive...add to it
            if (indicesOut)
               dMemcpy(indicesOut+numIndicesOut,indicesIn+start,2*numElements);
            if (newDraw)
               newDraw->numElements = numElements;
            numIndicesOut += numElements;
         }
      }
   }
   // spit out tris before leaving
   if (triIndices.size())
   {
      // material just changed and we have triangles lying around
      // add primitive and indices for triangles and clear triIndices
      if (indicesOut)
      {
         TSDrawPrimitive * newTris = (TSDrawPrimitive*) &primitivesOut[numPrimOut*2];
         newTris->matIndex = prevMaterial;
         newTris->matIndex &= ~(TSDrawPrimitive::Triangles|TSDrawPrimitive::Strip);
         newTris->matIndex |= TSDrawPrimitive::Triangles;
         newTris->start = numIndicesOut;
         newTris->numElements = triIndices.size();
         dMemcpy(&indicesOut[numIndicesOut],triIndices.address(),triIndices.size()*sizeof(U16));
      }
      numPrimOut++;
      numIndicesOut += triIndices.size();
      triIndices.clear();
   }
}

// This method retrieves data that is shared (or possibly shared) between different meshes.
// This adds an extra step to the copying of data from the memory buffer to the shape data buffer.
// If we have no parentMesh, then we either return a pointer to the data in the memory buffer
// (in the case that we skip this mesh) or copy the data into the shape data buffer and return
// that pointer (in the case that we don't skip this mesh).
// If we do have a parent mesh, then we return a pointer to the data in the shape buffer,
// copying the data in there ourselves if our parent didn't already do it (i.e., if it was skipped).
S32 * TSMesh::getSharedData32(S32 parentMesh, S32 size, S32 ** source, bool skip)
{
   CLockGuard guard(alloc.GetMutex());

   S32 * ptr;
   if(parentMesh<0)
      ptr = skip ? alloc.getPointer32(size) : alloc.copyToShape32(size);
   else
   {
      ptr = source[parentMesh];
      // if we skipped the previous mesh (and we're not skipping this one) then
      // we still need to copy points into the shape...
      if (!smDataCopied[parentMesh] && !skip)
      {
         S32 * tmp = ptr;
         ptr = alloc.allocShape32(size);
         if (ptr && tmp)
            dMemcpy(ptr,tmp,size*sizeof(S32));
      }
   }
   return ptr;
}

S8 * TSMesh::getSharedData8(S32 parentMesh, S32 size, S8 ** source, bool skip)
{
   CLockGuard guard(alloc.GetMutex());

   S8 * ptr;
   if(parentMesh<0)
      ptr = skip ? alloc.getPointer8(size) : alloc.copyToShape8(size);
   else
   {
      ptr = source[parentMesh];
      // if we skipped the previous mesh (and we're not skipping this one) then
      // we still need to copy points into the shape...
      if (!smDataCopied[parentMesh] && !skip)
      {
         S8 * tmp = ptr;
         ptr = alloc.allocShape8(size);
         if (ptr && tmp)
            dMemcpy(ptr,tmp,size*sizeof(S32));
      }
   }
   return ptr;
}

//-----------------------------------------------------------------------------
// Create vertex and index buffers
//-----------------------------------------------------------------------------
void TSMesh::createVBIB(S32 frame, S32 matFrame,GFXVertexBufferHandleBase *pVB,ToolVector<Point3F> *pVt,ToolVector<Point3F> *pNm,bool isNormal)	//Ray: 修改对老的特效制作方式的支持
{
   if(!pVt)
	   pVt = &verts;

   if(!pNm)
	   pNm = &norms;

   U32 numVerts =  pVt->size();
   if(numVerts>vertsPerFrame)
	   numVerts = vertsPerFrame;

   if( (numVerts == 0) || !GFXDevice::devicePresent() || !pVt->address())
      return;

   bool vertsChanged = false;
   if(!isNormal && meshType==SkinMeshType && TSShapeInstance::supportsVertTex)
   {
	   if(!vcol.size())
	   {
		   GFXVertexBufferHandle<GFXVertexPNTTBBBBT>* T = (GFXVertexBufferHandle<GFXVertexPNTTBBBBT> *)(pVB);
		   vertsChanged = setVertexData(T,frame,matFrame,numVerts,pVt,pNm);
	   }
	   else
	   {
		   GFXVertexBufferHandle<GFXVertexPCNTTBBBBT>* T = (GFXVertexBufferHandle<GFXVertexPCNTTBBBBT> *)(pVB);
		   vertsChanged = setVertexData(T,frame,matFrame,numVerts,pVt,pNm);
	   }
   }
   else if(!isNormal && meshType==SkinMeshType && TSShapeInstance::supportsR2VB)
   {
	   if(!vcol.size())
	   {
		   GFXVertexBufferHandle<GFXVertexTTT>* T = (GFXVertexBufferHandle<GFXVertexTTT> *)(pVB);
		   vertsChanged = setVertexData(T,frame,matFrame,numVerts,pVt,pNm);
		   T->declare(GFXDeclarePN4TTT,ELEMENTOF(GFXDeclarePN4TTT));
	   }
	   else
	   {
		   GFXVertexBufferHandle<GFXVertexCTTT>* T = (GFXVertexBufferHandle<GFXVertexCTTT> *)(pVB);
		   vertsChanged = setVertexData(T,frame,matFrame,numVerts,pVt,pNm);
		   T->declare(GFXDeclarePN4CTTT,ELEMENTOF(GFXDeclarePN4TTT));
	   }
   }
   else
   {
	   if(!vcol.size())
	   {
		   GFXVertexBufferHandle<GFXVertexPNTTT>* T = (GFXVertexBufferHandle<GFXVertexPNTTT> *)(pVB);
		   vertsChanged = setVertexData(T,frame,matFrame,numVerts,pVt,pNm);
	   }
	   else
	   {
		   GFXVertexBufferHandle<GFXVertexPCNTTT>* T = (GFXVertexBufferHandle<GFXVertexPCNTTT> *)(pVB);
		   vertsChanged = setVertexData(T,frame,matFrame,numVerts,pVt,pNm);
	   }
   }

   if( vertsChanged || mPB == NULL )
   {
      // go through and create PrimitiveInfo array
      Vector <GFXPrimitive> piArray;
      GFXPrimitive pInfo;

      U32   primitivesSize = primitives.size();
      for (U32 i=0; i<primitivesSize; i++)
      {
         const TSDrawPrimitive & draw = primitives[i];

         GFXPrimitiveType drawType = getDrawType(draw.matIndex>>30);

         switch( drawType )
         {
         case GFXTriangleList:
            pInfo.type = drawType;
            pInfo.numPrimitives = draw.numElements / 3;
            pInfo.startIndex = draw.start;
            pInfo.minIndex = 0;
            pInfo.numVertices = numVerts;
            break;

         case GFXTriangleStrip:
         case GFXTriangleFan:
            pInfo.type = drawType;
            pInfo.numPrimitives = draw.numElements - 2;
            pInfo.startIndex = draw.start;
            pInfo.minIndex = 0;
            pInfo.numVertices = numVerts;
            break;

         default:
            AssertFatal( false, "WTF?!" );
         }

         piArray.push_back( pInfo );
      }

      mPB.set(GFX, indices.size(), piArray.size(), GFXBufferTypeStatic );

      U16 *ibIndices = NULL;
      GFXPrimitive *piInput = NULL;
      mPB.lock(&ibIndices, &piInput);

      dMemcpy( ibIndices, indices.address(), indices.size() * sizeof(U16) );
      dMemcpy( piInput, piArray.address(), piArray.size() * sizeof(GFXPrimitive) );

      mPB.unlock();
   }
}


void TSMesh::assemble(bool skip)
{
   CLockGuard guard(alloc.GetMutex());

   alloc.checkGuard();

   numFrames = alloc.get32();
   numMatFrames = alloc.get32();
   parentMesh = alloc.get32();
   alloc.get32((S32*)&mBounds,6);
   alloc.get32((S32*)&mCenter,3);
   mRadius = (F32)alloc.get32();

   if (TSShape::smReadVersion>=32)
   {
	   sortOrder = alloc.get32();
   }

   if (TSShape::smReadVersion>=33)
   {
	   disableZ = alloc.get8();
   }

   S32 numVerts = alloc.get32();
   S32 * ptr32 = getSharedData32(parentMesh,3*numVerts,(S32**)smVertsList.address(),skip);
   verts.set((Point3F*)ptr32,numVerts);

   if(TSShape::smReadVersion>=30)
   {
	   S32 numvcol = alloc.get32();
	   ptr32 = getSharedData32(parentMesh,numvcol,(S32**)smVColList.address(),skip);
	   vcol.set((ColorI*)ptr32,numvcol);
   }
   else
	   vcol.set(NULL,0);

   S32 numTVerts = alloc.get32();
   ptr32 = getSharedData32(parentMesh,2*numTVerts,(S32**)smTVertsList.address(),skip);
   tverts.set((Point2F*)ptr32,numTVerts);

   S8 * ptr8;
   if (TSShape::smReadVersion>21 && TSMesh::smUseEncodedNormals)
   {
      // we have encoded normals and we want to use them...

      if (parentMesh<0)
         alloc.getPointer32(numVerts*3); // advance past norms, don't use
      norms.set(NULL,0);

      ptr8 = getSharedData8(parentMesh,numVerts,(S8**)smEncodedNormsList.address(),skip);
      encodedNorms.set(ptr8,numVerts);
   }
   else if (TSShape::smReadVersion>21)
   {
      // we have encoded normals but we don't want to use them...

      ptr32 = getSharedData32(parentMesh,3*numVerts,(S32**)smNormsList.address(),skip);
      norms.set((Point3F*)ptr32,numVerts);

      if (parentMesh<0)
         alloc.getPointer8(numVerts); // advance past encoded normls, don't use
      encodedNorms.set(NULL,0);
   }
   else
   {
      // no encoded normals...

      ptr32 = getSharedData32(parentMesh,3*numVerts,(S32**)smNormsList.address(),skip);
      norms.set((Point3F*)ptr32,numVerts);
      encodedNorms.set(NULL,0);
   }

   // copy the primitives and indices...how we do this depends on what
   // form we want them in when copied...just get pointers to data for now
   S32 szPrim = alloc.get32();
   S16 * prim16 = alloc.getPointer16(szPrim*2);
   S32 * prim32 = alloc.getPointer32(szPrim);
   S32 szInd = alloc.get32();
   S16 * ind16 = alloc.getPointer16(szInd);

   // count then copy...
   S32 cpyPrim = szPrim, cpyInd = szInd;
   if (smUseTriangles)
      convertToTris(prim16,prim32,ind16,szPrim,cpyPrim,cpyInd,NULL,NULL);
   else if (smUseOneStrip)
      convertToSingleStrip(prim16,prim32,ind16,szPrim,cpyPrim,cpyInd,NULL,NULL);
   else
      leaveAsMultipleStrips(prim16,prim32,ind16,szPrim,cpyPrim,cpyInd,NULL,NULL);
   ptr32 = alloc.allocShape32(2*cpyPrim);
   S16 * ptr16 = alloc.allocShape16(cpyInd);
   alloc.align32();
   S32 chkPrim = szPrim, chkInd = szInd;
   if (smUseTriangles)
      convertToTris(prim16,prim32,ind16,szPrim,chkPrim,chkInd,ptr32,ptr16);
   else if (smUseOneStrip)
      convertToSingleStrip(prim16,prim32,ind16,szPrim,chkPrim,chkInd,ptr32,ptr16);
   else
      leaveAsMultipleStrips(prim16,prim32,ind16,szPrim,chkPrim,chkInd,ptr32,ptr16);
   AssertFatal(chkPrim==cpyPrim && chkInd==cpyInd,"TSMesh::primitive conversion");
   primitives.set(ptr32,cpyPrim);
   indices.set(ptr16,cpyInd);

   S32 sz = alloc.get32();
   alloc.getPointer16(sz); // skip deprecated merge indices
   alloc.align32();

   vertsPerFrame = alloc.get32();
   U32 flags = (U32)alloc.get32();
   if (encodedNorms.size())
      flags |= UseEncodedNormals;
   setFlags(flags);

   alloc.checkGuard();

   if (alloc.allocShape32(0) && TSShape::smReadVersion<19)
      // only do this if we copied the data...
      computeBounds();

   if(numFrames>1 || numMatFrames>1)
	   mDynamic = true;

   if( meshType != SkinMeshType && !mDynamic)
   {
	   createTangents();
       createVBIB();
   }
}

void TSMesh::disassemble()
{
   CLockGuard guard(alloc.GetMutex());

   alloc.setGuard();

   alloc.set32(numFrames);
   alloc.set32(numMatFrames);
   alloc.set32(parentMesh);
   alloc.copyToBuffer32((S32*)&mBounds,6);
   alloc.copyToBuffer32((S32*)&mCenter,3);
   alloc.set32((S32)mRadius);
   
   if (TSShape::smVersion>=32)
   {
	   alloc.set32((S32)sortOrder);
   }

   if (TSShape::smVersion>=33)
   {
	   alloc.set8((S8)disableZ);
   }

   // verts...
   alloc.set32(verts.size());
   if (parentMesh<0)
      // if no parent mesh, then save off our verts
      alloc.copyToBuffer32((S32*)verts.address(),3*verts.size());

   // vcol...
   if(TSShape::smReadVersion>=30)
   {
	   alloc.set32(vcol.size());
	   if (parentMesh<0)
		   // if no parent mesh, then save off our verts
		   alloc.copyToBuffer32((S32*)vcol.address(),vcol.size());
   }

   // tverts...
   alloc.set32(tverts.size());
   if (parentMesh<0)
      // if no parent mesh, then save off our tverts
      alloc.copyToBuffer32((S32*)tverts.address(),2*tverts.size());

   // norms...
   if (parentMesh<0)
      // if no parent mesh, then save off our norms
      alloc.copyToBuffer32((S32*)norms.address(),3*norms.size()); // norms.size()==verts.size() or error...

   // encoded norms...
   if (parentMesh<0)
   {
      // if no parent mesh, compute encoded normals and copy over
      for (S32 i=0; i<norms.size(); i++)
      {
         U8 normIdx = encodedNorms.size() ? encodedNorms[i] : encodeNormal(norms[i]);
         alloc.copyToBuffer8((S8*)&normIdx,1);
      }
   }

   // primitives...
   alloc.set32(primitives.size());
   for (S32 i=0; i<primitives.size(); i++)
   {
      alloc.copyToBuffer16((S16*)&primitives[i],2);
      alloc.copyToBuffer32(((S32*)&primitives[i])+1,1);
   }

   // indices...
   alloc.set32(indices.size());
   alloc.copyToBuffer16((S16*)indices.address(),indices.size());

   // merge indices...DEPRECATED
   alloc.set32(0);

   // small stuff...
   alloc.set32(vertsPerFrame);
   alloc.set32(getFlags());

   alloc.setGuard();
}

//-----------------------------------------------------------------------------
// TSSkinMesh assemble from/ dissemble to memory buffer
//-----------------------------------------------------------------------------
void TSSkinMesh::assemble(bool skip)
{
   CLockGuard guard(alloc.GetMutex());

   // avoid a crash on computeBounds...
   initialVerts.set(NULL,0);

   TSMesh::assemble(skip);

   S32 sz = alloc.get32();
   S32 numVerts = sz;
   S32 * ptr32 = getSharedData32(parentMesh,3*numVerts,(S32**)smVertsList.address(),skip);
   initialVerts.set((Point3F*)ptr32,sz);

   S8 * ptr8;
   if (TSShape::smReadVersion>21 && TSMesh::smUseEncodedNormals)
   {
      // we have encoded normals and we want to use them...
      if (parentMesh<0)
         alloc.getPointer32(numVerts*3); // advance past norms, don't use
      initialNorms.set(NULL,0);

      ptr8 = getSharedData8(parentMesh,numVerts,(S8**)smEncodedNormsList.address(),skip);
      encodedNorms.set(ptr8,numVerts);
      // Note: we don't set the encoded normals flag because we handle them in updateSkin and
      //       hide the fact that we are using them from base class (TSMesh)
   }
   else if (TSShape::smReadVersion>21)
   {
      // we have encoded normals but we don't want to use them...
      ptr32 = getSharedData32(parentMesh,3*numVerts,(S32**)smNormsList.address(),skip);
      initialNorms.set((Point3F*)ptr32,numVerts);

      if (parentMesh<0)
         alloc.getPointer8(numVerts); // advance past encoded normls, don't use
      encodedNorms.set(NULL,0);
   }
   else
   {
      // no encoded normals...
      ptr32 = getSharedData32(parentMesh,3*numVerts,(S32**)smNormsList.address(),skip);
      initialNorms.set((Point3F*)ptr32,numVerts);
      encodedNorms.set(NULL,0);
   }

   sz = alloc.get32();
   ptr32 = getSharedData32(parentMesh,16*sz,(S32**)smInitTransformList.address(),skip);
   initialTransforms.set(ptr32,sz);

   sz = alloc.get32();
   ptr32 = getSharedData32(parentMesh,sz,(S32**)smVertexIndexList.address(),skip);
   vertexIndex.set(ptr32,sz);

   ptr32 = getSharedData32(parentMesh,sz,(S32**)smBoneIndexList.address(),skip);
   boneIndex.set(ptr32,sz);

   ptr32 = getSharedData32(parentMesh,sz,(S32**)smWeightList.address(),skip);
   weight.set((F32*)ptr32,sz);

   sz = alloc.get32();
   ptr32 = getSharedData32(parentMesh,sz,(S32**)smNodeIndexList.address(),skip);
   nodeIndex.set(ptr32,sz);

   alloc.checkGuard();

   if (alloc.allocShape32(0) && TSShape::smReadVersion<19)
      // only do this if we copied the data...
      TSMesh::computeBounds();

   if(!TSShapeInstance::isNormalSkin && (TSShapeInstance::supportsVertTex || TSShapeInstance::supportsR2VB))
   {
		//Ray:统计顶点骨骼索引
	    if(!initialVerts.address())
			return;

		int vertexNum = initialVerts.size();
		for(int i=0;i<MAX_BONE_PER_VERTEX;i++)
		{
			vertBoneIndices[i].boneIndices.setSize(vertexNum);
			vertBoneIndices[i].boneWeight.setSize(vertexNum);
			dMemset(vertBoneIndices[i].boneIndices.address(),0,sizeof(S32)*vertexNum);
			dMemset(vertBoneIndices[i].boneWeight.address(),0,sizeof(F32)*vertexNum);
		}

		const Point3F * inVerts = &initialVerts[0];
		const Point3F * inNorms = &initialNorms[0];
		S32 * curVtx = &vertexIndex[0];
		S32 * curBone = &boneIndex[0];
		F32 * curWeight = &weight[0];
		S32   vertexIndexSize = vertexIndex.size();
		const S32 * endVtx = &vertexIndex[vertexIndexSize];

		S32 vidx,midx;
		F32 w;
		while( curVtx != endVtx )
		{
			vidx = *curVtx;
			++curVtx;

			midx = *curBone;
			++curBone;

			w = *curWeight;
			++curWeight;

			int i;
			for( i=0;i<MAX_BONE_PER_VERTEX;i++)
			{
				if(!vertBoneIndices[i].boneWeight[vidx])
				{
					vertBoneIndices[i].boneIndices[vidx] = midx * 4;
					vertBoneIndices[i].boneWeight[vidx] = w;
					break;
				}
			}

			if(i==MAX_BONE_PER_VERTEX)
			{
				Con::errorf("顶点受到影响的骨骼数量超过4块了");

				int minI=0;
				F32 minWeight=2.0f;
				for( i=0;i<MAX_BONE_PER_VERTEX;i++)
				{
					if(minWeight > vertBoneIndices[i].boneWeight[vidx])
					{
						minWeight = vertBoneIndices[i].boneWeight[vidx];
						minI = i;
					}
				}

				if(w > minWeight)
				{
					vertBoneIndices[minI].boneIndices[vidx] = midx * 4;
					vertBoneIndices[minI].boneWeight[vidx] = w;
				}
			}
		}

		verts.set(initialVerts.address(),vertexNum);
		norms.set(initialNorms.address(),vertexNum);
		createTangents();

		createVBIB(0,0,NULL,&initialVerts,&initialNorms,TSShapeInstance::isNormalSkin);

		if(!TSShapeInstance::supportsVertTex)
		{
			int count =0;
			half *pData = NULL;

			mSourceVNTexture.set(vertexNum*2,1,GFXFormatR16G16B16A16F,&GFXDefaultPersistentProfile);
			GFXLockedRect *pRect = mSourceVNTexture.lock();
			pData = (half *)pRect->bits;
			for(int i=0;i<vertexNum;i++)
			{
				pData[count++] = half(initialVerts[i].x);
				pData[count++] = half(initialVerts[i].y);
				pData[count++] = half(initialVerts[i].z);
				pData[count++] = half(1.0f);
				pData[count++] = half(initialNorms[i].x);
				pData[count++] = half(initialNorms[i].y);
				pData[count++] = half(initialNorms[i].z);
				pData[count++] = half(1.0f);
			}
			mSourceVNTexture.unlock();

			count = 0;
			mBoneIdxTexture.set(vertexNum*2,1,GFXFormatR16G16B16A16F,&GFXDefaultPersistentProfile);
			pRect = mBoneIdxTexture.lock();
			pData = (half *)pRect->bits;
			for(int i=0;i<vertexNum;i++)
			{
				for(int k=0;k<2;k++)
				{
					for(int j=0;j<MAX_BONE_PER_VERTEX;j++)
					{
						pData[count++] = half((float)vertBoneIndices[j].boneIndices[i]);
					}
				}
			}
			mBoneIdxTexture.unlock();

			count = 0;
			mWeigthTexture.set(vertexNum*2,1,GFXFormatR16G16B16A16F,&GFXDefaultPersistentProfile);
			pRect = mWeigthTexture.lock();
			pData = (half *)pRect->bits;
			for(int i=0;i<vertexNum;i++)
			{
				for(int k=0;k<2;k++)
				{
					for(int j=0;j<MAX_BONE_PER_VERTEX;j++)
					{
						pData[count++] = half(vertBoneIndices[j].boneWeight[i]);
					}
				}
			}
			mWeigthTexture.unlock();
		}
   }
}

//-----------------------------------------------------------------------------
// disassemble
//-----------------------------------------------------------------------------
void TSSkinMesh::disassemble()
{
   CLockGuard guard(alloc.GetMutex());

   TSMesh::disassemble();

   alloc.set32(initialVerts.size());
   // if we have no parent mesh, then save off our verts & norms
   if (parentMesh<0)
   {
      alloc.copyToBuffer32((S32*)initialVerts.address(),3*initialVerts.size());

      // no longer do this here...let tsmesh handle this
      alloc.copyToBuffer32((S32*)initialNorms.address(),3*initialNorms.size());

      // if no parent mesh, compute encoded normals and copy over
      for (S32 i=0; i<initialNorms.size(); i++)
      {
         U8 normIdx = encodedNorms.size() ? encodedNorms[i] : encodeNormal(initialNorms[i]);
         alloc.copyToBuffer8((S8*)&normIdx,1);
      }
   }

   alloc.set32(initialTransforms.size());
   if (parentMesh<0)
      alloc.copyToBuffer32((S32*)initialTransforms.address(),initialTransforms.size()*16);

   alloc.set32(vertexIndex.size());
   if (parentMesh<0)
   {
      alloc.copyToBuffer32((S32*)vertexIndex.address(),vertexIndex.size());

      alloc.copyToBuffer32((S32*)boneIndex.address(),boneIndex.size());

      alloc.copyToBuffer32((S32*)weight.address(),weight.size());
   }

   alloc.set32(nodeIndex.size());
   if (parentMesh<0)
      alloc.copyToBuffer32((S32*)nodeIndex.address(),nodeIndex.size());

   alloc.setGuard();
}

void TSMesh::prepOpcodeCollision()
{
   // Make sure opcode is loaded!
   if(!gOpcodeInitialized)
   {
      Opcode::InitOpcode();
      gOpcodeInitialized = true;
   }

   // Don't re init if we already have something...
   if(mOptTree)
      return;

   // Ok, first set up a MeshInterface
   Opcode::MeshInterface *mi = new Opcode::MeshInterface();

   // Figure out how many triangles we have...
   U32 triCount = 0;
   const U32 base = 0;
   for (U32 i=0; i<primitives.size(); i++)
   {
      TSDrawPrimitive & draw = primitives[i];
      const U32 start = draw.start;

      AssertFatal(draw.matIndex & TSDrawPrimitive::Indexed,"TSMesh::buildPolyList (1)");

      // gonna depend on what kind of primitive it is...
      if ( (draw.matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles)
         triCount += draw.numElements / 3;
      else
      {
         // Have to walk the tristrip to get a count... may have degenerates
         U32 idx0 = base + indices[start + 0];
         U32 idx1;
         U32 idx2 = base + indices[start + 1];
         U32 * nextIdx = &idx1;
         for (S32 j=2; j<draw.numElements; j++)
         {
            *nextIdx = idx2;
            //            nextIdx = (j%2)==0 ? &idx0 : &idx1;
            nextIdx = (U32*) ( (dsize_t)nextIdx ^ (dsize_t)&idx0 ^ (dsize_t)&idx1);
            idx2 = base + indices[start + j];
            if (idx0 == idx1 || idx0 == idx2 || idx1 == idx2)
               continue;

            triCount++;
         }
      }
   }

   // Just do the first trilist for now.
   mi->SetNbVertices(verts.size());
   mi->SetNbTriangles(triCount);

   // Stuff everything into appropriate arrays.
   IceMaths::IndexedTriangle *its = new IceMaths::IndexedTriangle[mi->GetNbTriangles()], *curIts = its;
   IceMaths::Point           *pts = new IceMaths::Point[mi->GetNbVertices()];

   // add the polys...
   for (U32 i=0; i<primitives.size(); i++)
   {
      TSDrawPrimitive & draw = primitives[i];
      const U32 start = draw.start;

      AssertFatal(draw.matIndex & TSDrawPrimitive::Indexed,"TSMesh::buildPolyList (1)");

      // gonna depend on what kind of primitive it is...
      if ( (draw.matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Triangles)
      {
         for (S32 j=0; j<draw.numElements; )
         {
            curIts->mVRef[2] = base + indices[start + j + 0];
            curIts->mVRef[1] = base + indices[start + j + 1];
            curIts->mVRef[0] = base + indices[start + j + 2];
            curIts++;

            j += 3;
         }
      }
      else
      {
         AssertFatal((draw.matIndex & TSDrawPrimitive::TypeMask) == TSDrawPrimitive::Strip,"TSMesh::buildPolyList (2)");

         U32 idx0 = base + indices[start + 0];
         U32 idx1;
         U32 idx2 = base + indices[start + 1];
         U32 * nextIdx = &idx1;
         for (S32 j=2; j<draw.numElements; j++)
         {
            *nextIdx = idx2;
            //            nextIdx = (j%2)==0 ? &idx0 : &idx1;
            nextIdx = (U32*) ( (dsize_t)nextIdx ^ (dsize_t)&idx0 ^ (dsize_t)&idx1);
            idx2 = base + indices[start + j];
            if (idx0 == idx1 || idx0 == idx2 || idx1 == idx2)
               continue;

            curIts->mVRef[2] = idx0;
            curIts->mVRef[1] = idx1;
            curIts->mVRef[0] = idx2;
            curIts++;
         }
      }
   }

   AssertFatal((curIts - its) == mi->GetNbTriangles(), "Triangle count mismatch!");

   for(S32 i=0; i<mi->GetNbVertices(); i++)
      pts[i].Set(verts[i].x,verts[i].y,verts[i].z);

   mi->SetPointers(its, pts);

   // Ok, we've got a mesh interface populated, now let's build a thingy to collide against.
   mOptTree = new Opcode::Model();

   Opcode::OPCODECREATE opcc;

   opcc.mCanRemap = true;
   opcc.mIMesh = mi;
   opcc.mKeepOriginal = false;
   opcc.mNoLeaf = false;
   opcc.mQuantized = false;
   opcc.mSettings.mLimit = 1;

   mOptTree->Build(opcc);
}

bool TSMesh::castRayOpcode( const Point3F & s, const Point3F & e, RayInfo * info )
{
   Opcode::RayCollider ray;
   Opcode::CollisionFaces cfs;

   IceMaths::Point dir(e.x - s.x, e.y - s.y, e.z - s.z);
   const F32 rayLen = dir.Magnitude();
   IceMaths::Ray vec(Point(s.x, s.y, s.z), dir.Normalize() );

   ray.SetDestination(&cfs);
   ray.SetFirstContact(false);
   ray.SetClosestHit(true);
   ray.SetPrimitiveTests(true);
   ray.SetCulling(true);
   ray.SetMaxDist(rayLen);

   if (0 != ray.ValidateSettings())
       return false;

   //AssertFatal(ray.ValidateSettings() == NULL, "invalid ray settings");

   // Do collision.
   bool safety = ray.Collide(vec, *mOptTree);

   if (!safety)
       return false;

   //AssertFatal(safety, "TSMesh::castRayOpcode - no good ray collide!");

   // If no hit, just skip out.
   if(cfs.GetNbFaces()==0)
      return false;

   // Got a hit!
   //AssertFatal(cfs.GetNbFaces() == 1, "bad");

   const Opcode::CollisionFace &face = cfs.GetFaces()[0];

   // If the cast was successful let's check if the t value is less than what we had
   // and toggle the collision boolean
   // Stupid t... i prefer coffee
   const F32 t = face.mDistance / rayLen;

   if(t < 0.0f || t > 1.0f)
      return false;

   if(t <= info->t)
   {
      info->t = t;

      // Calculate the normal.
      Opcode::VertexPointers vp;
      mOptTree->GetMeshInterface()->GetTriangle(vp, face.mFaceID);

      // Get the two edges.
      const IceMaths::Point baseVert = *vp.Vertex[0];
      const IceMaths::Point a = *vp.Vertex[1] - baseVert;
      const IceMaths::Point b = *vp.Vertex[2] - baseVert;

      IceMaths::Point n;
      n.Cross(a,b);
      n.Normalize();

      info->normal.set(n.x, n.y, n.z);
      return true;
   }

   return false;
}

//-----------------------------------------------------------------------------
// find tangent vector
//-----------------------------------------------------------------------------
inline void TSMesh::findTangent( U32 index1, 
                                 U32 index2, 
                                 U32 index3, 
                                 Point3F *tan0, 
                                 Point3F *tan1 )
{
   const Point3F& v1 = verts[index1];
   const Point3F& v2 = verts[index2];
   const Point3F& v3 = verts[index3];

   const Point2F& w1 = tverts[index1];
   const Point2F& w2 = tverts[index2];
   const Point2F& w3 = tverts[index3];

   F32 x1 = v2.x - v1.x;
   F32 x2 = v3.x - v1.x;
   F32 y1 = v2.y - v1.y;
   F32 y2 = v3.y - v1.y;
   F32 z1 = v2.z - v1.z;
   F32 z2 = v3.z - v1.z;

   F32 s1 = w2.x - w1.x;
   F32 s2 = w3.x - w1.x;
   F32 t1 = w2.y - w1.y;
   F32 t2 = w3.y - w1.y;

   F32 denom = (s1 * t2 - s2 * t1);

   if( fabs(denom) < 0.0001f )
      return;  // handle degenerate triangles from strips

   F32 r = 1.0f / denom;

   Point3F sdir((t2 * x1 - t1 * x2) * r, 
      (t2 * y1 - t1 * y2) * r, 
      (t2 * z1 - t1 * z2) * r);

   Point3F tdir((s1 * x2 - s2 * x1) * r, 
      (s1 * y2 - s2 * y1) * r, 
      (s1 * z2 - s2 * z1) * r);


   tan0[index1]  += sdir;
   tan1[index1]  += tdir;

   tan0[index2]  += sdir;
   tan1[index2]  += tdir;

   tan0[index3]  += sdir;
   tan1[index3]  += tdir;
}

//-----------------------------------------------------------------------------
// create array of tangent vectors
//-----------------------------------------------------------------------------
void TSMesh::createTangents()
{
   U32   numVerts = verts.size();
 
   if( (numVerts == 0) || !&verts[0] )
      return;

   Vector<Point3F> tan0;
   tan0.setSize(numVerts * 2);

   Point3F *tan1 = tan0.address() + numVerts;
   dMemset( tan0.address(), 0, sizeof(Point3F) * 2 * numVerts );
   
   U32   numPrimatives = primitives.size();

   for (S32 i=0; i<numPrimatives; i++)
   {
      const TSDrawPrimitive & draw = primitives[i];
      GFXPrimitiveType drawType = getDrawType(draw.matIndex>>30);

      U32 p1Index = 0;
      U32 p2Index = 0;

      U16 *baseIdx = &indices[draw.start];

      const S16   numElements = draw.numElements;

      switch( drawType )
      {
      case GFXTriangleList:
         {
            for( U32 j=0; j<numElements; j+=3 )
            {
               findTangent( baseIdx[j], baseIdx[j+1], baseIdx[j+2], tan0.address(), tan1 );
            }
            break;
         }

      case GFXTriangleStrip:
         {
            p1Index = baseIdx[0];
            p2Index = baseIdx[1];
            for( U32 j=2; j<numElements; j++ )
            {
               findTangent( p1Index, p2Index, baseIdx[j], tan0.address(), tan1 );
               p1Index = p2Index;
               p2Index = baseIdx[j];
            }
            break;
         }
      case GFXTriangleFan:
         {
            p1Index = baseIdx[0];
            p2Index = baseIdx[1];
            for( U32 j=2; j<numElements; j++ )
            {
               findTangent( p1Index, p2Index, baseIdx[j], tan0.address(), tan1 );
               p2Index = baseIdx[j];
            }
            break;
         }

      default:
         AssertFatal( false, "TSMesh::createTangents: unknown primitive type!" );
      }
   }

   initialTangents.setSize( numVerts );
   tangents.setSize( numVerts );

   // fill out final info from accumulated basis data
   for( U32 i=0; i<numVerts; i++ )
   {
      const Point3F& n = norms[i];
      const Point3F& t = tan0[i];
      const Point3F& b = tan1[i];

      initialTangents[i] = t - n * mDot( n, t );
      initialTangents[i].normalize();
      tangents[i] = initialTangents[i];

      Point3F cp;
      mCross( n, t, &cp );
      
      tangents[i].w = (mDot( cp, b ) < 0.0f) ? -1.0f : 1.0f;
   }
}
