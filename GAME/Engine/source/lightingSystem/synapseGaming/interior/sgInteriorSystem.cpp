//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

// This file contains the interface between the lighting kit and the interior rendering system.

#include "lightingSystem/common/sceneLighting.h"
#include "sceneGraph/shadowVolumeBSP.h"
#include "interior/interiorInstance.h"
#include "sceneGraph/lightingInterfaces.h"
#include "lightingSystem/common/sceneLighting.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/common/sceneLightingGlobals.h"

#include "lightingSystem/synapseGaming/sgLightingModel.h"
#include "lightingSystem/synapseGaming/interior/sgPlanarMap.h"


// not great but necessary due to reduced float precision
// caused by one of the linked libs...
#include <float.h>


//
// Lighting system interface
//
class sgInteriorSystem : public SceneLightingInterface
{
protected:   
public:
   sgInteriorSystem();
   static bool smUseVertexLighting;

   virtual SceneLighting::ObjectProxy* createObjectProxy(SceneObject* obj, SceneLighting::ObjectProxyList* sceneObjects);
   virtual PersistInfo::PersistChunk* createPersistChunk(const U32 chunkType);
   virtual bool createPersistChunkFromProxy(SceneLighting::ObjectProxy* objproxy, PersistInfo::PersistChunk **ret);

   virtual void init();
   virtual U32 addObjectType();
   virtual U32 addToClippingMask();

   virtual void processLightingBegin();
   virtual void processLightingCompleted(bool success);

   // Given a ray, this will return the color from the lightmap of this object, return true if handled
   virtual bool getColorFromRayInfo(RayInfo collision, ColorF& result);
};

bool sgInteriorSystem::smUseVertexLighting = false;

// 
// PersistChunk
//
struct InteriorChunk : public PersistInfo::PersistChunk
{
   typedef PersistChunk Parent;

   InteriorChunk();
   ~InteriorChunk();

   Vector<GBitmap*>     sgNormalLightMaps;

   Vector<U32>          mDetailLightmapCount;
   Vector<U32>          mDetailLightmapIndices;
   Vector<GBitmap*>     mLightmaps;

   bool                 mHasAlarmState;
   Vector<U32>          mDetailVertexCount;
   Vector<ColorI>       mVertexColorsNormal;
   Vector<ColorI>       mVertexColorsAlarm;

   bool read(Stream &);
   bool write(Stream &);
};

//------------------------------------------------------------------------------
// Class InteriorChunk
//------------------------------------------------------------------------------
InteriorChunk::InteriorChunk()
{
   mChunkType = PersistChunk::InteriorChunkType;
}

InteriorChunk::~InteriorChunk()
{
   for(U32 i = 0; i < mLightmaps.size(); i++)
      delete mLightmaps[i];

   for(U32 i = 0; i < sgNormalLightMaps.size(); i++)
   {
      if(sgNormalLightMaps[i])
         delete sgNormalLightMaps[i];
   }
}

//------------------------------------------------------------------------------
// - always read in vertex lighting, lightmaps may not be needed
bool InteriorChunk::read(Stream & stream)
{
   if(!Parent::read(stream))
      return(false);

   U32 size;
   U32 i;

   // size of this minichunk
   if(!stream.read(&size))
      return(false);

   // lightmaps
   stream.read(&size);
   mDetailLightmapCount.setSize(size);
   for(i = 0; i < size; i++)
      if(!stream.read(&mDetailLightmapCount[i]))
         return(false);

   stream.read(&size);
   mDetailLightmapIndices.setSize(size);
   for(i = 0; i < size; i++)
      if(!stream.read(&mDetailLightmapIndices[i]))
         return(false);

   if(!stream.read(&size))
      return(false);
   mLightmaps.setSize(size);

   for(i = 0; i < size; i++)
   {
      mLightmaps[i] = new GBitmap;
      if(sgLightManager::sgAllowFullLightMaps())
      {
         if(!mLightmaps[i]->readPNG(stream))
            return(false);
      }
      else
      {
         if(!mLightmaps[i]->read(stream))
            return(false);
      }
   }


   if(!stream.read(&size))
      return(false);
   sgNormalLightMaps.setSize(size);

   for(i = 0; i < size; i++)
   {
      bool isobj = false;
      sgNormalLightMaps[i] = NULL;
      stream.read(&isobj);
      if(isobj)
      {
         sgNormalLightMaps[i] = new GBitmap;
         if(!sgNormalLightMaps[i]->readPNG(stream))
            return(false);
      }
   }

   // size of the vertex lighting: need to reset stream position after zipStream reading
   U32 zipStreamEnd;
   if(!stream.read(&zipStreamEnd))
      return(false);
   zipStreamEnd += stream.getPosition();

   // since there is no resizeFilterStream the zipStream happily reads
   // off the end of the compressed block... reset the position
   stream.setPosition(zipStreamEnd);

   return(true);
}

bool InteriorChunk::write(Stream & stream)
{
   if(!Parent::write(stream))
      return(false);

   // lightmaps
   U32 startPos = stream.getPosition();
   if(!stream.write(U32(0)))
      return(false);

   U32 i;
   if(!stream.write(U32(mDetailLightmapCount.size())))
      return(false);
   for(i = 0; i < mDetailLightmapCount.size(); i++)
      if(!stream.write(mDetailLightmapCount[i]))
         return(false);

   if(!stream.write(U32(mDetailLightmapIndices.size())))
      return(false);
   for(i = 0; i < mDetailLightmapIndices.size(); i++)
      if(!stream.write(mDetailLightmapIndices[i]))
         return(false);

   if(!stream.write(U32(mLightmaps.size())))
      return(false);
   for(i = 0; i < mLightmaps.size(); i++)
   {
      AssertFatal(mLightmaps[i], "SceneLighting::SceneLighting::InteriorChunk::Write: Invalid bitmap!");
      if(sgLightManager::sgAllowFullLightMaps())
      {
         if(!mLightmaps[i]->writePNG(stream))
            return(false);
      }
      else
      {
         if(!mLightmaps[i]->write(stream))
            return(false);
      }
   }


   if(!stream.write(U32(sgNormalLightMaps.size())))
      return(false);
   for(i = 0; i < sgNormalLightMaps.size(); i++)
   {
      bool isobj = (sgNormalLightMaps[i] != NULL);
      stream.write(isobj);
      if(isobj)
      {
         if(!sgNormalLightMaps[i]->writePNG(stream))
            return(false);
      }
   }

   // write out the lightmap portions size
   U32 endPos = stream.getPosition();
   if(!stream.setPosition(startPos))
      return(false);

   // don't include the offset in the size
   if(!stream.write(U32(endPos - startPos - sizeof(U32))))
      return(false);
   if(!stream.setPosition(endPos))
      return(false);


   // vertex lighting: needs the size of the vertex info because the
   // zip stream may read off the end of the chunk
   startPos = stream.getPosition();
   if(!stream.write(U32(0)))
      return(false);

   // write out the vertex lighting portions size
   endPos = stream.getPosition();
   if(!stream.setPosition(startPos))
      return(false);

   // don't include the offset in the size
   if(!stream.write(U32(endPos - startPos - sizeof(U32))))
      return(false);
   if(!stream.setPosition(endPos))
      return(false);

   return(true);
}

//
// InteriorProxy (definition)
//
class InteriorProxy : public SceneLighting::ObjectProxy
{
private:
   typedef  ObjectProxy       Parent;

   bool isShadowedBy(InteriorProxy *);
   ShadowVolumeBSP::SVPoly * buildInteriorPoly(ShadowVolumeBSP * shadowVolumeBSP,
      Interior * detail, U32 surfaceIndex, LightInfo * light,
      bool createSurfaceInfo);
public:

   InteriorProxy(SceneObject * obj);
   ~InteriorProxy();
   InteriorInstance * operator->() {return(static_cast<InteriorInstance*>(static_cast<SceneObject*>(mObj)));}
   InteriorInstance * getObject() {return(static_cast<InteriorInstance*>(static_cast<SceneObject*>(mObj)));}

   // current light info
   ShadowVolumeBSP *                   mBoxShadowBSP;
   Vector<ShadowVolumeBSP::SVPoly*>    mLitBoxSurfaces;
   Vector<PlaneF>                      mOppositeBoxPlanes;
   Vector<PlaneF>                      mTerrainTestPlanes;


   struct sgSurfaceInfo
   {
      Interior *sgDetail;
      S32 sgSurfaceIndex;// SG_NULL_SURFACE == static mesh...
      InteriorSimpleMesh *sgStaticMesh;
      PlaneF sgSurfacePlane;
      bool sgSurfaceOutsideVisible;
      Vector<sgPlanarLightMap::sgSmoothingVert> sgTriStrip;
      Point3D sgWorldPos;
      Point3D sgSVector;
      Point3D sgTVector;
      Point2I sgLightMapExtent;
      Point2I sgLightMapOffset;
      U32 sgLightMapIndex;

      void sgDumpDebugData(int surfaceindex)
      {
         Con::printf("-----------------------------------------------");
         Con::printf("Debug data for surface: %d", surfaceindex);

         if(sgSurfaceIndex == SG_NULL_SURFACE)
            Con::printf("Static mesh surface.");
         else
            Con::printf("Interior surface: %d", sgSurfaceIndex);
         if(sgSurfaceOutsideVisible)
            Con::printf("Outside visible.");
         else
            Con::printf("Not outside visible.");

         Con::printf("Surface plane: %g, %g, %g, %g", sgSurfacePlane.x, sgSurfacePlane.y, sgSurfacePlane.z, sgSurfacePlane.d);

         Con::printf("Surface start position: %g, %g, %g", sgWorldPos.x, sgWorldPos.y, sgWorldPos.z);
         Con::printf("Surface vector s: %g, %g, %g", sgSVector.x, sgSVector.y, sgSVector.z);
         Con::printf("Surface vector t: %g, %g, %g", sgTVector.x, sgTVector.y, sgTVector.z);

         Con::printf("Light map index: %d", sgLightMapIndex);
         Con::printf("Light map extent: %d, %d", sgLightMapExtent.x, sgLightMapExtent.y);
         Con::printf("Light map offset: %d, %d", sgLightMapOffset.x, sgLightMapOffset.y);

         Con::printf("Vertex count: %d", sgTriStrip.size());
         for(int i=0; i<sgTriStrip.size(); i++)
         {
            sgPlanarLightMap::sgSmoothingVert &vert = sgTriStrip[i];
            Con::printf("Vertex %d position: %g, %g, %g", i, vert.sgVert.x, vert.sgVert.y, vert.sgVert.z);
            Con::printf("Vertex %d normal: %g, %g, %g", i, vert.sgNorm.x, vert.sgNorm.y, vert.sgNorm.z);
            Con::printf("Vertex %d texture: %g, %g", i, vert.sgText.x, vert.sgText.y);
         }

         Con::printf("");
      }
   };
   U32 sgCurrentSurfaceIndex;
   U32 sgSurfacesPerPass;
   InteriorInstance *sgInterior;
   Vector<LightInfo *> sgLights;
   Vector<sgSurfaceInfo *> sgSurfaces;

   void sgClearSurfaces()
   {
      for(U32 s=0; s<sgSurfaces.size(); s++)
      {
         if(sgSurfaces[s])
            delete sgSurfaces[s];
      }
      sgSurfaces.clear();
   }

   void sgAddLight(LightInfo *light, InteriorInstance *interior);
   void sgProcessSurface(sgSurfaceInfo &surfaceinfo);
   void sgConvertStaticMeshPrimitiveToSurfaceInfo(const InteriorSimpleMesh *staticmesh, U32 primitiveindex, Interior *detail, sgSurfaceInfo &surfaceinfo);
   void sgConvertInteriorSurfaceToSurfaceInfo(const Interior::Surface &surface, U32 i, Interior *detail, sgSurfaceInfo &surfaceinfo);
   void sgExtractLightingInformation(const Interior *detail, const PlaneF &lmEqX, const PlaneF &lmEqY,
      const PlaneF &surfplane, const Point2I &lmoff, const Point2I &lmext, const Point2I &lmsheetsize,
      Point3D &worldpos, Point3D &vectS, Point3D &vectT, Point2I &lmoffactual, Point2I &lmextactual);


   // lighting interface
   bool loadResources();
   void init();
   bool tgePreLight(LightInfo* light);
   bool preLight(LightInfo *);
   void light(LightInfo *);
   void postLight(bool lastLight);

   virtual void processLightingStart();
   virtual bool processStartObjectLightingEvent(SceneLighting::ObjectProxy* objproxy, U32 current, U32 max);
   virtual void processTGELightProcessEvent(U32 curr, U32 max, LightInfo*);

   virtual bool supportsShadowVolume();
   virtual void getClipPlanes(Vector<PlaneF>& planes);
   virtual void addToShadowVolume(ShadowVolumeBSP * shadowVolume, LightInfo * light, S32 level);

   // persist
   U32 getResourceCRC();
   bool setPersistInfo(PersistInfo::PersistChunk *);
   bool getPersistInfo(PersistInfo::PersistChunk *);
};

///
/// InteriorProxy (implementation)
///
InteriorProxy::InteriorProxy(SceneObject * obj) :
Parent(obj)
{
   mBoxShadowBSP = 0;

   sgCurrentSurfaceIndex = 0;
   sgSurfacesPerPass = 0;
}

InteriorProxy::~InteriorProxy()
{
   sgClearSurfaces();

   delete mBoxShadowBSP;
}

bool InteriorProxy::loadResources()
{
   InteriorInstance * interior = getObject();
   if(!interior)
      return(false);

   Resource<InteriorResource> & interiorRes = interior->getResource();
   if(!bool(interiorRes))
      return(false);

   return(true);
}

void InteriorProxy::init()
{
   InteriorInstance * interior = getObject();
   if(!interior)
      return;
}

/// reroutes InteriorProxy::preLight for point light and TSStatic support.
bool InteriorProxy::preLight(LightInfo * light)
{
   SG_CHECK_LIGHT(light);

   // create shadow volume of the bounding box of this object
   InteriorInstance * interior = getObject();
   if(!interior)
      return(false);

   if(!sgRelightFilter::sgAllowLighting(interior->getWorldBox(), false))
      return false;

   // build light list...
   sgAddLight(static_cast<LightInfo*>(light), interior);
   return(true);
}

bool InteriorProxy::tgePreLight(LightInfo* light) 
{
   InteriorInstance * interior = dynamic_cast<InteriorInstance*>(getObject());
   if(!interior)
      return(false);

   if(light->mType != LightInfo::Vector)
      return(false);

   // Create shadow volume of the bounding box of this object, this is used
   // to figure out which pieces of the terrain need to consider the interior
   // during lighting.   
   mLitBoxSurfaces.clear();
   mTerrainTestPlanes.clear();

   const Box3F & objBox = interior->getObjBox();
   const MatrixF & objTransform = interior->getTransform();
   const VectorF & objScale = interior->getScale();

   // grab the surfaces which form the shadow volume
   U32 numPlanes = 0;
   PlaneF testPlanes[3];
   U32 planeIndices[3];

   // grab the bounding planes which face the light
   U32 i;
   for(i = 0; (i < 6) && (numPlanes < 3); i++)
   {
      PlaneF plane;
      plane.x = BoxNormals[i].x;
      plane.y = BoxNormals[i].y;
      plane.z = BoxNormals[i].z;

      if(i&1)
         plane.d = (((const float*)objBox.min)[(i-1)>>1]);
      else
         plane.d = -(((const float*)objBox.max)[i>>1]);

      // project
      mTransformPlane(objTransform, objScale, plane, &testPlanes[numPlanes]);

      planeIndices[numPlanes] = i;

      if(mDot(testPlanes[numPlanes], light->mDirection) < gParellelVectorThresh)
         numPlanes++;
   }
   AssertFatal(numPlanes, "SceneLighting::InteriorProxy::preLight: no planes found");

   // project the points
   Point3F projPnts[8];
   for(i = 0; i < 8; i++)
   {
      Point3F pnt;
      pnt.set(BoxPnts[i].x ? objBox.max.x : objBox.min.x,
         BoxPnts[i].y ? objBox.max.y : objBox.min.y,
         BoxPnts[i].z ? objBox.max.z : objBox.min.z);

      // scale it
      pnt.convolve(objScale);
      objTransform.mulP(pnt, &projPnts[i]);
   }

   mBoxShadowBSP = new ShadowVolumeBSP;

   // insert the shadow volumes for the surfaces
   for(i = 0; i < numPlanes; i++)
   {
      ShadowVolumeBSP::SVPoly * poly = mBoxShadowBSP->createPoly();
      poly->mWindingCount = 4;

      U32 j;
      for(j = 0; j < 4; j++)
         poly->mWinding[j] = projPnts[BoxVerts[planeIndices[i]][j]];

      testPlanes[i].neg();
      poly->mPlane = testPlanes[i];

      mBoxShadowBSP->buildPolyVolume(poly, light);
      mLitBoxSurfaces.push_back(mBoxShadowBSP->copyPoly(poly));
      mBoxShadowBSP->insertPoly(poly);

      // create the opposite planes for testing against terrain
      Point3F pnts[3];
      for(j = 0; j < 3; j++)
         pnts[j] = projPnts[BoxVerts[planeIndices[i]^1][j]];
      PlaneF plane(pnts[2], pnts[1], pnts[0]);
      mOppositeBoxPlanes.push_back(plane);
   }

   // grab the unique planes for terrain checks
   for(i = 0; i < numPlanes; i++)
   {
      U32 mask = 0;
      for(U32 j = 0; j < numPlanes; j++)
         mask |= BoxSharedEdgeMask[planeIndices[i]][planeIndices[j]];

      ShadowVolumeBSP::SVNode * traverse = mBoxShadowBSP->getShadowVolume(mLitBoxSurfaces[i]->mShadowVolume);
      while(traverse->mFront)
      {
         if(!(mask & 1))
            mTerrainTestPlanes.push_back(mBoxShadowBSP->getPlane(traverse->mPlaneIndex));

         mask >>= 1;
         traverse = traverse->mFront;
      }
   }

   // there will be 2 duplicate node planes if there were only 2 planes lit
   if(numPlanes == 2)
   {
      for(S32 i = 0; i < mTerrainTestPlanes.size(); i++)
         for(U32 j = 0; j < mTerrainTestPlanes.size(); j++)
         {
            if(i == j)
               continue;

            if((mDot(mTerrainTestPlanes[i], mTerrainTestPlanes[j]) > gPlaneNormThresh) &&
               (mFabs(mTerrainTestPlanes[i].d - mTerrainTestPlanes[j].d) < gPlaneDistThresh))
            {
               mTerrainTestPlanes.erase(i);
               i--;
               break;
            }
         }
   }
   return(true);
}

bool InteriorProxy::isShadowedBy(InteriorProxy * test)
{
   // add if overlapping world box
   if((*this)->getWorldBox().isOverlapped((*test)->getWorldBox()))
      return(true);

   // test the box shadow volume
   for(U32 i = 0; i < mLitBoxSurfaces.size(); i++)
   {
      ShadowVolumeBSP::SVPoly * poly = mBoxShadowBSP->copyPoly(mLitBoxSurfaces[i]);
      if(test->mBoxShadowBSP->testPoly(poly))
         return(true);
   }

   return(false);
}

void InteriorProxy::postLight(bool lastLight)
{
   delete mBoxShadowBSP;
   mBoxShadowBSP = 0;

   InteriorInstance * interior = getObject();
   if(!interior)
      return;
}

void InteriorProxy::processLightingStart()
{
   InteriorInstance *interior = dynamic_cast<InteriorInstance *>(getObject());
   AssertFatal(interior, "InteriorProxy getObject should be an InteriorInstance!");
   if(!interior)
      return;

   for(U32 d=0; d<interior->getResource()->getNumDetailLevels(); d++)
   {
      Interior *detail = interior->getResource()->getDetailLevel(d);
      gInteriorLMManager.clearLightmaps(detail->getLMHandle(), interior->getLMHandle());
   }
}

void InteriorProxy::processTGELightProcessEvent(U32 curr, U32 max, LightInfo*)
{
   InteriorInstance *interior = dynamic_cast<InteriorInstance *>(getObject());
   AssertFatal(interior, "InteriorProxy getObject should be an InteriorInstance!");
   if(!interior)
      return;

   if (interior->getInteriorFileName())
      Con::printf("      Lighting interior object %d of %d (%s)...", (curr+1), max, interior->getInteriorFileName());
   else
      Con::printf("      Lighting interior object %d of %d ...", (curr+1), max);
}

//------------------------------------------------------------------------------
U32 InteriorProxy::getResourceCRC()
{
   InteriorInstance * interior = getObject();
   if(!interior)
      return(0);
   return(interior->getCRC());
}

//------------------------------------------------------------------------------
bool InteriorProxy::setPersistInfo(PersistInfo::PersistChunk * info)
{

   if(!Parent::setPersistInfo(info))
      return(false);

   InteriorChunk * chunk = dynamic_cast<InteriorChunk*>(info);
   AssertFatal(chunk, "InteriorProxy::setPersistInfo: invalid info chunk!");

   InteriorInstance * interior = getObject();
   if(!interior)
      return(false);

   U32 numDetails = interior->getNumDetailLevels();

   // check the lighting method
   AssertFatal(sgInteriorSystem::smUseVertexLighting == Interior::smUseVertexLighting, "InteriorProxy::setPersistInfo: invalid vertex lighting state");
   if(sgInteriorSystem::smUseVertexLighting != Interior::smUseVertexLighting)
      return(false);

   // need lightmaps?
   if(!sgInteriorSystem::smUseVertexLighting)
   {
      if(chunk->mDetailLightmapCount.size() != numDetails)
         return(false);

      LM_HANDLE instanceHandle = interior->getLMHandle();
      U32 idx = 0;

      for(U32 i = 0; i < numDetails; i++)
      {
         Interior * detail = interior->getDetailLevel(i);

         LM_HANDLE interiorHandle = detail->getLMHandle();
         Vector<GFXTexHandle> & baseHandles = gInteriorLMManager.getHandles(interiorHandle, 0);

         if(chunk->mDetailLightmapCount[i] > baseHandles.size())
            return(false);

         for(U32 j = 0; j < chunk->mDetailLightmapCount[i]; j++)
         {
            U32 baseIndex = chunk->mDetailLightmapIndices[idx];
            if(baseIndex >= baseHandles.size())
               return(false);

            AssertFatal(chunk->mLightmaps[idx], "InteriorProxy::setPersistInfo: bunk bitmap!");
            if(chunk->mLightmaps[idx]->getWidth() != baseHandles[baseIndex]->getWidth() ||
               chunk->mLightmaps[idx]->getHeight() != baseHandles[baseIndex]->getHeight())
               return(false);

            GFXTexHandle tHandle = gInteriorLMManager.duplicateBaseLightmap(interiorHandle, instanceHandle, baseIndex);

            // create the diff bitmap
            tHandle->getBitmap()->combine( baseHandles[baseIndex]->getBitmap(), 
               chunk->mLightmaps[idx],
               GFXTOPAdd );
#if 0
            // CodeReview: Left this code in for now so if there is problems with
            // the new combine code this can be used as a reference [5/11/2007 Pat]
            U8 * pDiff = chunk->mLightmaps[idx]->getAddress(0,0);
            U8 * pBase = baseHandles[baseIndex]->getBitmap()->getAddress(0,0);
            U8 * pDest = tHandle->getBitmap()->getAddress(0,0);

            Point2I extent(tHandle->getWidth(), tHandle->getHeight());
            for(U32 y = 0; y < extent.y; y++)
            {
               for(U32 x = 0; x < extent.x; x++)
               {
                  *pDest++ = *pBase++ + *pDiff++;
                  *pDest++ = *pBase++ + *pDiff++;
                  *pDest++ = *pBase++ + *pDiff++;
               }
            }
#endif

            if(chunk->sgNormalLightMaps[idx])
            {
               GFXTexHandle nlmhandle = gInteriorLMManager.duplicateBaseNormalmap(interiorHandle, instanceHandle, baseIndex);
               GBitmap *tempnlm = chunk->sgNormalLightMaps[idx];

               // CodeReview: Deal with the format changes [5/14/2007 Pat]
               tempnlm->setFormat( nlmhandle->getBitmap()->getFormat() ); 

               dMemcpy(nlmhandle->getBitmap()->getWritableBits(), tempnlm->getBits(), tempnlm->byteSize);
            }

            idx++;
         }
      }
   }

   return(true);
}

bool InteriorProxy::getPersistInfo(PersistInfo::PersistChunk * info)
{
   if(!Parent::getPersistInfo(info))
      return(false);

   InteriorChunk * chunk = dynamic_cast<InteriorChunk*>(info);
   AssertFatal(chunk, "InteriorProxy::getPersistInfo: invalid info chunk!");

   InteriorInstance * interior = getObject();
   if(!interior)
      return(false);

   LM_HANDLE instanceHandle = interior->getLMHandle();

   AssertFatal(!chunk->mDetailLightmapCount.size(), "InteriorProxy::getPersistInfo: invalid array!");
   AssertFatal(!chunk->mDetailLightmapIndices.size(), "InteriorProxy::getPersistInfo: invalid array!");
   AssertFatal(!chunk->mLightmaps.size(), "InteriorProxy::getPersistInfo: invalid array!");

   U32 numDetails = interior->getNumDetailLevels();
   U32 i;
   for(i = 0; i < numDetails; i++)
   {
      Interior * detail = interior->getDetailLevel(i);
      LM_HANDLE interiorHandle = detail->getLMHandle();

      Vector<GFXTexHandle> & baseHandles = gInteriorLMManager.getHandles(interiorHandle, 0);
      Vector<GFXTexHandle> & instanceHandles = gInteriorLMManager.getHandles(interiorHandle, instanceHandle);
      Vector<GFXTexHandle> &sgNLMHandles = gInteriorLMManager.getNormalHandles(interiorHandle, instanceHandle);

      U32 litCount = 0;

      // walk all the instance lightmaps and grab diff lighting from them
      for(U32 j = 0; j < instanceHandles.size(); j++)
      {
         if(!instanceHandles[j])
            continue;

         litCount++;
         chunk->mDetailLightmapIndices.push_back(j);

         GBitmap * baseBitmap = baseHandles[j]->getBitmap();
         GBitmap * instanceBitmap = instanceHandles[j]->getBitmap();

         Point2I extent(baseBitmap->getWidth(), baseBitmap->getHeight());

         GBitmap * diffLightmap = new GBitmap(extent.x, extent.y, false);

         // diffLightmap = instanceBitmap - baseBitmap
         diffLightmap->combine( instanceBitmap, baseBitmap, GFXTOPSubtract );

#if 0
         // CodeReview: Left this code in for now so if there is problems with
         // the new combine code this can be used as a reference [5/11/2007 Pat]
         U8 * pBase = baseBitmap->getAddress(0,0);
         U8 * pInstance = instanceBitmap->getAddress(0,0);
         U8 * pDest = diffLightmap->getAddress(0,0);

         // fill the diff lightmap
         for(U32 y = 0; y < extent.y; y++)
         {
            for(U32 x = 0; x < extent.x; x++)
            {
               *pDest++ = *pInstance++ - *pBase++;
               *pDest++ = *pInstance++ - *pBase++;
               *pDest++ = *pInstance++ - *pBase++;
            }
         }
#endif
         chunk->mLightmaps.push_back(diffLightmap);



         // create a new object...
         if(sgNLMHandles[j])
         {
            GBitmap *nlm = sgNLMHandles[j]->getBitmap();
            GBitmap *tempnlm = new GBitmap(*nlm); // new GBitmap(nlm->getWidth(), nlm->getHeight(), false);
            //dMemcpy(tempnlm->getWritableBits(), nlm->getBits(), nlm->byteSize);
            chunk->sgNormalLightMaps.push_back(tempnlm);

         }
         else
         {
            chunk->sgNormalLightMaps.push_back(NULL);
         }
      }

      chunk->mDetailLightmapCount.push_back(litCount);
   }

   // process the vertex lighting...
   AssertFatal(!chunk->mDetailVertexCount.size(), "InteriorProxy::getPersistInfo: invalid chunk info");
   AssertFatal(!chunk->mVertexColorsNormal.size(), "InteriorProxy::getPersistInfo: invalid chunk info");
   AssertFatal(!chunk->mVertexColorsAlarm.size(), "InteriorProxy::getPersistInfo: invalid chunk info");

   chunk->mHasAlarmState = interior->getDetailLevel(0)->hasAlarmState();
   chunk->mDetailVertexCount.setSize(numDetails);

   U32 size = 0;
   for(i = 0; i < numDetails; i++)
   {
      Interior * detail = interior->getDetailLevel(i);

      U32 count = detail->getWindingCount();
      chunk->mDetailVertexCount[i] = count;
      size += count;
   }

   return(true);
}

/// adds the ability to bake point lights into interior light maps.
void InteriorProxy::sgAddLight(LightInfo *light, InteriorInstance *interior)
{
   // need this...
   sgInterior = interior;

   sgLightInfo *sglight = dynamic_cast<sgLightInfo *>(light);
   AssertFatal((sglight), "Light not a Synapse Gaming light object!");
   sgLightingModel &model = sgLightingModelManager::sgGetLightingModel(
      sglight->sgLightingModelName);
   model.sgSetState(sglight);

   // test for early out...
   if(!model.sgCanIlluminate(interior->getWorldBox()))
   {
      model.sgResetState();
      return;
   }

   model.sgResetState();
   sgLights.push_back(light);

   // on first light build surface list...
   if(sgLights.size() == 1)
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


      // stats...
      sgStatistics::sgInteriorObjectIncludedCount++;


      // get the global shadow casters...
      sgShadowObjects::sgGetObjects(interior);

      sgClearSurfaces();

      sgCurrentSurfaceIndex = 0;
      InteriorResource *res = sgInterior->getResource();
      U32 countd = res->getNumDetailLevels();
      for(U32 d=0; d<countd; d++)
      {
         Interior *detail = res->getDetailLevel(d);
         U32 counti = detail->getSurfaceCount();
         U32 offset = sgSurfaces.size();
         sgSurfaces.increment(counti);
         for(U32 i=0; i<counti; i++)
         {
            sgSurfaceInfo *info = new sgSurfaceInfo();
            sgSurfaces[i + offset] = info;
            sgConvertInteriorSurfaceToSurfaceInfo(detail->getSurface(i), i, detail, *info);

            //info->sgDumpDebugData(i + offset);
         }

         U32 countsm = detail->getStaticMeshCount();
         for(U32 sm=0; sm<countsm; sm++)
         {
            const InteriorSimpleMesh *mesh = detail->getStaticMesh(sm);
            if(!mesh)
               continue;

            counti = mesh->primitives.size();
            offset = sgSurfaces.size();
            sgSurfaces.increment(counti);
            for(U32 i=0; i<counti; i++)
            {
               sgSurfaceInfo *info = new sgSurfaceInfo();
               sgSurfaces[i + offset] = info;
               sgConvertStaticMeshPrimitiveToSurfaceInfo(mesh, i, detail, *info);
            }
         }
      }

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

   // recalc number of surfaces per pass based on new light count...
   sgSurfacesPerPass = sgSurfaces.size() / sgLights.size();
}

void InteriorProxy::light(LightInfo *light)
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


   U32 i;
   U32 countthispass = 0;


   // stats...
   sgStatistics::sgInteriorObjectIlluminationCount++;


   for(i=sgCurrentSurfaceIndex; i<sgSurfaces.size(); i++)
   {
      sgProcessSurface(*sgSurfaces[i]);
      countthispass++;
      sgCurrentSurfaceIndex++;
      if((countthispass >= sgSurfacesPerPass) && (sgLights.last() != light))
         break;
   }


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

void InteriorProxy::sgConvertStaticMeshPrimitiveToSurfaceInfo(const InteriorSimpleMesh *staticmesh, U32 primitiveindex,
                                                              Interior *detail, sgSurfaceInfo &surfaceinfo)
{
   const InteriorSimpleMesh::primitive &prim = staticmesh->primitives[primitiveindex];
   const MatrixF &transform = sgInterior->getTransform();
   const Point3F &scale = sgInterior->getScale();

   // need to generate the plane...
   AssertFatal((prim.count >= 3), "Primitive with only 2 verts?!?");
   Point3F p0 = staticmesh->verts[prim.start];
   Point3F p1 = staticmesh->verts[prim.start+1];
   Point3F p2 = staticmesh->verts[prim.start+2];

   // and it needs to be in interior/object space, not static mesh space...
   p0.convolve(staticmesh->scale);
   p1.convolve(staticmesh->scale);
   p2.convolve(staticmesh->scale);
   staticmesh->transform.mulP(p0);
   staticmesh->transform.mulP(p1);
   staticmesh->transform.mulP(p2);

   // generate the plane...
   PlaneF plane = PlaneF(p0, p1, p2);

   // also need a world space version...
   PlaneF projPlane;
   mTransformPlane(transform, scale, plane, &projPlane);

   //-----------------------------
   // get the generic instance 0 lm...
   GBitmap *lm = gInteriorLMManager.getBitmap(detail->getLMHandle(), 0, prim.lightMapIndex);
   AssertFatal((lm), "Why was there no base light map??");

   sgExtractLightingInformation(detail, prim.lightMapEquationX, prim.lightMapEquationY, plane,
      Point2I(prim.lightMapOffset.x, prim.lightMapOffset.y), Point2I(prim.lightMapSize.x, prim.lightMapSize.y), Point2I(lm->getWidth(), lm->getHeight()),
      surfaceinfo.sgWorldPos,
      surfaceinfo.sgSVector,
      surfaceinfo.sgTVector,
      surfaceinfo.sgLightMapOffset,
      surfaceinfo.sgLightMapExtent);

   surfaceinfo.sgDetail = detail;
   surfaceinfo.sgSurfaceIndex = SG_NULL_SURFACE;
   surfaceinfo.sgStaticMesh = (InteriorSimpleMesh *)staticmesh;
   surfaceinfo.sgSurfacePlane = projPlane;
   // currently ctor export has no zones...
   // when it does this needs to be fixed - also static mesh zone lighting...
   surfaceinfo.sgSurfaceOutsideVisible = true;
   surfaceinfo.sgLightMapIndex = prim.lightMapIndex;

   surfaceinfo.sgTriStrip.clear();
   surfaceinfo.sgTriStrip.increment(prim.count);

   for(U32 v=0; v<prim.count; v++)
   {
      U32 index = prim.start + v;
      surfaceinfo.sgTriStrip[v].sgVert = staticmesh->verts[index];
      surfaceinfo.sgTriStrip[v].sgNorm = staticmesh->norms[index];
      surfaceinfo.sgTriStrip[v].sgText = staticmesh->diffuseUVs[index];

      surfaceinfo.sgTriStrip[v].sgVert.convolve(staticmesh->scale);
      staticmesh->transform.mulP(surfaceinfo.sgTriStrip[v].sgVert);
      staticmesh->transform.mulV(surfaceinfo.sgTriStrip[v].sgNorm);

      surfaceinfo.sgTriStrip[v].sgVert.convolve(scale);
      transform.mulP(surfaceinfo.sgTriStrip[v].sgVert);
      transform.mulV(surfaceinfo.sgTriStrip[v].sgNorm);
   }
}

void InteriorProxy::sgConvertInteriorSurfaceToSurfaceInfo(const Interior::Surface &surface, U32 i,
                                                          Interior *detail, sgSurfaceInfo &surfaceinfo)
{
   // points right way?
   PlaneF plane = detail->getPlane(surface.planeIndex);
   if(Interior::planeIsFlipped(surface.planeIndex))
      plane.neg();

   const MatrixF &transform = sgInterior->getTransform();
   const Point3F &scale = sgInterior->getScale();

   PlaneF projPlane;
   mTransformPlane(transform, scale, plane, &projPlane);

   const Interior::TexGenPlanes &lmTexGenEQ = detail->getLMTexGenEQ(i);
   const Interior::TexGenPlanes &texgen = detail->getTexGenEQ(surface.texGenIndex);

   // get the generic instance 0 lm...
   U32 lmindex = detail->getNormalLMapIndex(i);
   GBitmap *lm = gInteriorLMManager.getBitmap(detail->getLMHandle(), 0, lmindex);
   AssertFatal((lm), "Why was there no base light map??");

   sgExtractLightingInformation(detail, lmTexGenEQ.planeX, lmTexGenEQ.planeY, plane,
      Point2I(surface.mapOffsetX, surface.mapOffsetY), Point2I(surface.mapSizeX, surface.mapSizeY), Point2I(lm->getWidth(), lm->getHeight()),
      surfaceinfo.sgWorldPos,
      surfaceinfo.sgSVector,
      surfaceinfo.sgTVector,
      surfaceinfo.sgLightMapOffset,
      surfaceinfo.sgLightMapExtent);

   surfaceinfo.sgDetail = detail;
   surfaceinfo.sgSurfaceIndex = i;
   surfaceinfo.sgStaticMesh = NULL;
   surfaceinfo.sgSurfacePlane = projPlane;
   surfaceinfo.sgSurfaceOutsideVisible = (surface.surfaceFlags & Interior::SurfaceOutsideVisible);
   surfaceinfo.sgLightMapIndex = lmindex;

   surfaceinfo.sgTriStrip.clear();
   surfaceinfo.sgTriStrip.increment(surface.windingCount);

   for(U32 v=0; v<surface.windingCount; v++)
   {
      Point3F &vert = surfaceinfo.sgTriStrip[v].sgVert;
      Point3F &norm = surfaceinfo.sgTriStrip[v].sgNorm;
      Point2F &text = surfaceinfo.sgTriStrip[v].sgText;
      Point3F &tang   = surfaceinfo.sgTriStrip[v].sgTang;
      Point3F &binorm = surfaceinfo.sgTriStrip[v].sgBinorm;

      U32 index = detail->getWinding(surface.windingStart + v);
      vert = detail->getPoint(index);

      detail->getTexMat(i, v, tang, norm, binorm);

      // before the transform...
      text.x = texgen.planeX.x * vert.x +
         texgen.planeX.y * vert.y +
         texgen.planeX.z * vert.z +
         texgen.planeX.d;
      text.y = texgen.planeY.x * vert.x +
         texgen.planeY.y * vert.y +
         texgen.planeY.z * vert.z +
         texgen.planeY.d;

      vert.convolve(scale);
      transform.mulP(vert);
      transform.mulV(norm);
      transform.mulV(tang);
      transform.mulV(binorm);
   }
}

void InteriorProxy::sgExtractLightingInformation(const Interior *detail, const PlaneF &lmEqX, const PlaneF &lmEqY, const PlaneF &surfplane,
                                                 const Point2I &lmoff, const Point2I &lmext, const Point2I &lmsheetsize,
                                                 Point3D &worldpos, Point3D &vectS, Point3D &vectT, Point2I &lmoffactual, Point2I &lmextactual)
{
   const MatrixF &transform = sgInterior->getTransform();
   const Point3F &scale = sgInterior->getScale();

   S32 xlen, ylen, xoff, yoff;
   S32 lmborder = detail->getLightMapBorderSize();
   xlen = lmext.x + (lmborder * 2);
   ylen = lmext.y + (lmborder * 2);
   xoff = lmoff.x - lmborder;
   yoff = lmoff.y - lmborder;

   // very important check!!!
   AssertFatal((
      ((xoff >= 0) && ((xlen + xoff) < lmsheetsize.x)) &&
      ((yoff >= 0) && ((ylen + yoff) < lmsheetsize.y))), "Light map extents exceeded bitmap size!");

   lmoffactual = Point2I(xoff, yoff);
   lmextactual = Point2I(xlen, ylen);


   const F32 * const lGenX = lmEqX;
   const F32 * const lGenY = lmEqY;

   AssertFatal((lGenX[0] * lGenX[1] == 0.f) && 
      (lGenX[0] * lGenX[2] == 0.f) &&
      (lGenX[1] * lGenX[2] == 0.f), "Bad lmTexGen!");
   AssertFatal((lGenY[0] * lGenY[1] == 0.f) && 
      (lGenY[0] * lGenY[2] == 0.f) &&
      (lGenY[1] * lGenY[2] == 0.f), "Bad lmTexGen!");

   // get the axis index for the texgens (could be swapped)
   S32 si = -1;
   S32 ti = -1;
   S32 axis = -1;

   if(lGenX[0] == 0.f && lGenY[0] == 0.f)          // YZ
   {
      axis = 0;
      if(lGenX[1] == 0.f) { // swapped?
         si = 2;
         ti = 1;
      } else {
         si = 1;
         ti = 2;
      }
   }
   else if(lGenX[1] == 0.f && lGenY[1] == 0.f)     // XZ
   {
      axis = 1;
      if(lGenX[0] == 0.f) { // swapped?
         si = 2;
         ti = 0;
      } else {
         si = 0;
         ti = 2;
      }
   }
   else if(lGenX[2] == 0.f && lGenY[2] == 0.f)     // XY
   {
      axis = 2;
      if(lGenX[0] == 0.f) { // swapped?
         si = 1;
         ti = 0;
      } else {
         si = 0;
         ti = 1;
      }
   }

   AssertFatal(!(axis == -1), "SceneLighting::lightInterior: bad TexGen!");

   const F32 * pNormal = ((const F32*)surfplane);

   Point3F start;
   F32 *pStart = (F32 *)start;

   // get the start point on the lightmap
   F32 lumelScale = 1 / (lGenX[si] * lmsheetsize.x);
   pStart[si] = (((xoff * lumelScale) / (1 / lGenX[si])) - lGenX[3]) / lGenX[si];
   pStart[ti] = (((yoff * lumelScale) / (1 / lGenY[ti])) - lGenY[3]) / lGenY[ti];
   pStart[axis] = ((pNormal[si] * pStart[si]) + (pNormal[ti] * pStart[ti]) + surfplane.d) / -pNormal[axis];

   start.convolve(scale);
   transform.mulP(start);

   worldpos = Point3D(start.x, start.y, start.z);

   // get the s/t vecs oriented on the surface
   Point3F vS, vT;
   F32 * pSVec = ((F32*)vS);
   F32 * pTVec = ((F32*)vT);

   // s
   pSVec[si] = 1.f;
   pSVec[ti] = 0.f;

   F32 angle;
   Point3F planeNormal = surfplane;
   ((F32*)planeNormal)[ti] = 0.f;
   planeNormal.normalize();

   angle = mAcos(mClampF(((F32*)planeNormal)[axis], -1.f, 1.f));
   pSVec[axis] = (((F32*)planeNormal)[si] < 0.f) ? mTan(angle) : -mTan(angle);

   // t
   pTVec[ti] = 1.f;
   pTVec[si] = 0.f;

   planeNormal = surfplane;
   ((F32*)planeNormal)[si] = 0.f;
   planeNormal.normalize();

   angle = mAcos(mClampF(((F32*)planeNormal)[axis], -1.f, 1.f));
   pTVec[axis] = (((F32*)planeNormal)[ti] < 0.f) ? mTan(angle) : -mTan(angle);

   Point3D vS64 = Point3D(vS.x, vS.y, vS.z);
   Point3D vT64 = Point3D(vT.x, vT.y, vT.z);

   // scale the vectors
   vS64 *= lumelScale;
   vT64 *= lumelScale;

   Point3D m0 = Point3D(transform[0], transform[1], transform[2]);
   Point3D m1 = Point3D(transform[4], transform[5], transform[6]);
   Point3D m2 = Point3D(transform[8], transform[9], transform[10]);

   Point3D scale64 = Point3D(scale.x, scale.y, scale.z);

   vectS.x = mDot(vS64, m0);
   vectS.y = mDot(vS64, m1);
   vectS.z = mDot(vS64, m2);
   vectS.convolve(scale64);

   vectT.x = mDot(vT64, m0);
   vectT.y = mDot(vT64, m1);
   vectT.z = mDot(vT64, m2);
   vectT.convolve(scale64);

   // project vecs
   //transform.mulV(vectS);
   //vectS.convolve(scale);

   //transform.mulV(vectT);
   //vectT.convolve(scale);
}

void InteriorProxy::sgProcessSurface(sgSurfaceInfo &surfaceinfo)
{
   sgPlanarLightMap *lightmap = new sgPlanarLightMap(surfaceinfo.sgLightMapExtent.x, surfaceinfo.sgLightMapExtent.y,
      sgInterior, surfaceinfo.sgDetail, surfaceinfo.sgSurfaceIndex, surfaceinfo.sgStaticMesh, surfaceinfo.sgSurfacePlane, surfaceinfo.sgTriStrip);

   lightmap->sgWorldPosition = surfaceinfo.sgWorldPos;
   lightmap->sgLightMapSVector = surfaceinfo.sgSVector;
   lightmap->sgLightMapTVector = surfaceinfo.sgTVector;
   lightmap->sgSetupLighting();

   for(U32 ii=0; ii<sgLights.size(); ii++)
   {
      // should we even bother?
      sgLightInfo *sglight = dynamic_cast<sgLightInfo *>(sgLights[ii]);
      AssertFatal((sglight), "Light not a Synapse Gaming light object!");

      if(sglight->mType != LightInfo::Vector)
      {
         bool valid = false;
         if((sglight->sgLocalAmbientAmount > SG_MIN_LEXEL_INTENSITY) && sglight->sgDoubleSidedAmbient)
            valid = true;
         if(surfaceinfo.sgSurfacePlane.distToPlane(sglight->mPos) > 0)
            valid = true;
         for(U32 v=0; v<surfaceinfo.sgTriStrip.size(); v++)
         {
            sgPlanarLightMap::sgSmoothingVert &vert = surfaceinfo.sgTriStrip[v];
            if(mDot(vert.sgNorm, (sglight->mPos - vert.sgVert)) > 0)
               valid = true;
         }

         if(!valid)
            continue;
      }
      else
      {
         if(!surfaceinfo.sgSurfaceOutsideVisible)
            continue;
      }

      lightmap->sgCalculateLighting(sglight);
   }

   if(lightmap->sgIsDirty())
   {
      GFXTexHandle &lmh = gInteriorLMManager.duplicateBaseLightmap(
         surfaceinfo.sgDetail->getLMHandle(), sgInterior->getLMHandle(), surfaceinfo.sgLightMapIndex);
      GBitmap *lm = lmh.getBitmap();

      GFXTexHandle &lnmh = gInteriorLMManager.duplicateBaseNormalmap(
         surfaceinfo.sgDetail->getLMHandle(), sgInterior->getLMHandle(), surfaceinfo.sgLightMapIndex);
      GBitmap *lnm = lnmh.getBitmap();

      lightmap->sgMergeLighting(lm, lnm, surfaceinfo.sgLightMapOffset.x, surfaceinfo.sgLightMapOffset.y);
   }

   delete lightmap;
}

bool InteriorProxy::supportsShadowVolume()
{
   return true;
}

void InteriorProxy::getClipPlanes(Vector<PlaneF>& planes)
{
   for(U32 i = 0; i < mLitBoxSurfaces.size(); i++)
      planes.push_back(mLitBoxSurfaces[i]->mPlane);
}

void InteriorProxy::addToShadowVolume(ShadowVolumeBSP * shadowVolume, LightInfo * light, S32 level)
{
   if(light->mType != LightInfo::Vector)
      return;

   InteriorInstance* interior = dynamic_cast<InteriorInstance*>(getObject());
   if (!interior)
      return;
   Resource<InteriorResource> mInteriorRes = interior->getResource();

   ColorF ambient = light->mAmbient;

   bool shadowedTree = true;

   // check if just getting shadow detail
   if(level == SceneLighting::SHADOW_DETAIL)
   {
      shadowedTree = false;
      level = mInteriorRes->getNumDetailLevels() - 1;
   }

   Interior * detail = mInteriorRes->getDetailLevel(level);

   // make sure surfaces do not get processed more than once
   BitVector surfaceProcessed;
   surfaceProcessed.setSize(detail->mSurfaces.size());
   surfaceProcessed.clear();

   bool isoutside = false;
   for(U32 zone=0; zone<interior->getNumCurrZones(); zone++)
   {
      if(interior->getCurrZone(zone) == 0)
      {
         isoutside = true;
         break;
      }
   }
   if(!isoutside)
      return;

   for(U32 i = 0; i < detail->getNumZones(); i++)
   {
      Interior::Zone & zone = detail->mZones[i];
      for(U32 j = 0; j < zone.surfaceCount; j++)
      {
         U32 surfaceIndex = detail->mZoneSurfaces[zone.surfaceStart + j];

         // dont reprocess a surface
         if(surfaceProcessed.test(surfaceIndex))
            continue;
         surfaceProcessed.set(surfaceIndex);

         Interior::Surface & surface = detail->mSurfaces[surfaceIndex];

         // outside visible?
         if(!(surface.surfaceFlags & Interior::SurfaceOutsideVisible))
            continue;

         // good surface?
         PlaneF plane = detail->getPlane(surface.planeIndex);
         if(Interior::planeIsFlipped(surface.planeIndex))
            plane.neg();

         // project the plane
         PlaneF projPlane;
         mTransformPlane(interior->getTransform(), interior->getScale(), plane, &projPlane);

         // fill with ambient? (need to do here, because surface will not be
         // added to the SVBSP tree)
         F32 dot = mDot(projPlane, light->mDirection);
         if(dot > -gParellelVectorThresh && !(GFX->getPixelShaderVersion() > 0.0) )
            continue;

         ShadowVolumeBSP::SVPoly * poly = buildInteriorPoly(shadowVolume, detail,
            surfaceIndex, light, shadowedTree);

         // insert it into the SVBSP tree
         shadowVolume->insertPoly(poly);
      }
   }
}

//------------------------------------------------------------------------------
ShadowVolumeBSP::SVPoly * InteriorProxy::buildInteriorPoly(ShadowVolumeBSP * shadowVolumeBSP,
                                                           Interior * detail, U32 surfaceIndex, LightInfo * light,
                                                           bool createSurfaceInfo)
{
   InteriorInstance* interior = dynamic_cast<InteriorInstance*>(getObject());
   if (!interior)
      return NULL;

   // transform and add the points...
   const MatrixF & transform = interior->getTransform();
   const VectorF & scale = interior->getScale();

   const Interior::Surface & surface = detail->mSurfaces[surfaceIndex];

   ShadowVolumeBSP::SVPoly * poly = shadowVolumeBSP->createPoly();

   poly->mWindingCount = surface.windingCount;

   // project these points
   for(U32 j = 0; j < poly->mWindingCount; j++)
   {
      Point3F iPnt = detail->mPoints[detail->mWindings[surface.windingStart + j]].point;
      Point3F tPnt;
      iPnt.convolve(scale);
      transform.mulP(iPnt, &tPnt);
      poly->mWinding[j] = tPnt;
   }

   // convert from fan
   U32 tmpIndices[ShadowVolumeBSP::SVPoly::MaxWinding];
   Point3F fanIndices[ShadowVolumeBSP::SVPoly::MaxWinding];

   tmpIndices[0] = 0;

   U32 idx = 1;
   U32 i;
   for(i = 1; i < poly->mWindingCount; i += 2)
      tmpIndices[idx++] = i;
   for(i = ((poly->mWindingCount - 1) & (~0x1)); i > 0; i -= 2)
      tmpIndices[idx++] = i;

   idx = 0;
   for(i = 0; i < poly->mWindingCount; i++)
      if(surface.fanMask & (1 << i))
         fanIndices[idx++] = poly->mWinding[tmpIndices[i]];

   // set the data
   poly->mWindingCount = idx;
   for(i = 0; i < poly->mWindingCount; i++)
      poly->mWinding[i] = fanIndices[i];

   // flip the plane - shadow volumes face inwards
   PlaneF plane = detail->getPlane(surface.planeIndex);
   if(!Interior::planeIsFlipped(surface.planeIndex))
      plane.neg();

   // transform the plane
   mTransformPlane(transform, scale, plane, &poly->mPlane);
   shadowVolumeBSP->buildPolyVolume(poly, light);

   // do surface info?
   if(createSurfaceInfo)
   {
      ShadowVolumeBSP::SurfaceInfo * surfaceInfo = new ShadowVolumeBSP::SurfaceInfo;
      shadowVolumeBSP->mSurfaces.push_back(surfaceInfo);

      // fill it
      surfaceInfo->mSurfaceIndex = surfaceIndex;
      surfaceInfo->mShadowVolume = shadowVolumeBSP->getShadowVolume(poly->mShadowVolume);

      // POLY and POLY node gets it too
      ShadowVolumeBSP::SVNode * traverse = shadowVolumeBSP->getShadowVolume(poly->mShadowVolume);
      while(traverse->mFront)
      {
         traverse->mSurfaceInfo = surfaceInfo;
         traverse = traverse->mFront;
      }

      // get some info from the poly node
      poly->mSurfaceInfo = traverse->mSurfaceInfo = surfaceInfo;
      surfaceInfo->mPlaneIndex = traverse->mPlaneIndex;
   }

   return(poly);
}

// Called before the obj is going to be lit
bool InteriorProxy::processStartObjectLightingEvent(SceneLighting::ObjectProxy* objproxy, U32 current, U32 max)
{
   InteriorInstance *interior = dynamic_cast<InteriorInstance *>(objproxy->getObject());
   if((interior)) // && (interior->mInteriorFileName))
   {
      Con::printf("    Lighting interior object %d of %d (%s)...", (current+1), max, interior->getInteriorFileName());
      return true;
   } else {
      return false;
   }
}

//
//
//

sgInteriorSystem::sgInteriorSystem()
{
   SGLM->getLightingInterfaces()->registerSystem(this);
}

SceneLighting::ObjectProxy* sgInteriorSystem::createObjectProxy(SceneObject* obj, SceneLighting::ObjectProxyList* sceneObjects)
{
   if ((obj->getTypeMask() & InteriorObjectType) != 0)
   {
      return new InteriorProxy(obj);
   } else {
      return NULL;
   }
}

PersistInfo::PersistChunk* sgInteriorSystem::createPersistChunk(const U32 chunkType) 
{
   if (chunkType == PersistInfo::PersistChunk::InteriorChunkType)
   {
      return new InteriorChunk;
   } else {
      return NULL;
   }
}

bool sgInteriorSystem::createPersistChunkFromProxy(SceneLighting::ObjectProxy* objproxy, PersistInfo::PersistChunk **ret)
{
   if ((objproxy->mObj->getTypeMask() & InteriorObjectType) != 0)
   {
      *ret = new InteriorChunk;
      return true;
   } else {
      return false;
   }
}

void sgInteriorSystem::init()
{

}

U32 sgInteriorSystem::addObjectType()
{
   return InteriorObjectType;
}

U32 sgInteriorSystem::addToClippingMask()
{
   return InteriorObjectType;
}

void sgInteriorSystem::processLightingBegin()
{
   // Store the vertex lighting state when we being lighting, we compare this when we finish lighting 
   smUseVertexLighting = Interior::smUseVertexLighting;
}

void sgInteriorSystem::processLightingCompleted(bool success)
{
   if(success)
   {
      AssertFatal(smUseVertexLighting == Interior::smUseVertexLighting, "SceneLighting::completed: vertex lighting state changed during scene light");

      // cannot do anything if vertex state has changed (since we only load in what is needed)
      if(smUseVertexLighting == Interior::smUseVertexLighting)
      {
         if(!smUseVertexLighting)
         {
            gInteriorLMManager.downloadGLTextures();
            gInteriorLMManager.destroyBitmaps();
         }
         else
            gInteriorLMManager.destroyTextures();
      }
   }
}

// Given a ray, this will return the color from the lightmap of this object, return true if handled
bool sgInteriorSystem::getColorFromRayInfo(RayInfo collision, ColorF& result)
{
   InteriorInstance* interior = dynamic_cast<InteriorInstance*>(collision.object);
   if (interior == NULL)
      return false;

   interior->getRenderWorldTransform().mulP(collision.point);
   Interior *detail = interior->getDetailLevel(0);
   AssertFatal((detail), "SceneObject::getLightingAmbientColor: invalid interior");
   if(collision.face < detail->getSurfaceCount())
   {
      const Interior::Surface &surface = detail->getSurface(collision.face);
      const Interior::TexGenPlanes &texgen = detail->getLMTexGenEQ(collision.face);

      GBitmap* lightmap = gInteriorLMManager.getHandle(detail->getLMHandle(),
         interior->getLMHandle(), detail->getNormalLMapIndex(collision.face)).getBitmap();
      if (!lightmap)
         return false;

      Point2F uv;
      uv.x = mDot(texgen.planeX, collision.point) + texgen.planeX.d;
      uv.y = mDot(texgen.planeY, collision.point) + texgen.planeY.d;

      // We do width/height - 1 because sampleTexel maps 0..1 to 0..maxPixels-1, if
      // we don't also adjust here we'll be off by a pixel, which is important because
      // interior lightmaps will have junk in unused parts of the lightmap.
      F32 maxWidth = lightmap->getWidth() - 1;
      U32 size = (U32)(uv.x * maxWidth);
      size = mClamp(size, surface.mapOffsetX, (surface.mapOffsetX + surface.mapSizeX));
      uv.x = F32(size) / maxWidth;

      F32 maxHeight = lightmap->getHeight() - 1;
      size = (U32)(uv.y * maxHeight);
      size = mClamp(size, surface.mapOffsetY, (surface.mapOffsetY + surface.mapSizeY));
      uv.y = F32(size) / maxHeight;

      result = lightmap->sampleTexel(uv.x, uv.y);
      return true;
   }
   return false;
}


// Static factory object registers with lighting system plug-in system
static sgInteriorSystem p_sgInteriorSystem;
