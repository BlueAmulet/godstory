//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "core/resManager.h"
#include "core/fileStream.h"
#include "math/mRect.h"
#include "math/mMathFn.h"
#include "util/fourcc.h"
#include "atlas/core/atlasFile.h"
#include "atlas/resource/atlasResourceConfigTOC.h"
#include "atlas/resource/atlasResourceTexTOC.h"
#include "atlas/resource/atlasResourceGeomTOC.h"

#include "lightingSystem/synapseGaming/atlas/atlasSceneLightmap.h"
#include "atlas/editor/atlasExportInterfaces.h"
#include "atlas/editor/atlasImageImport.h"
#include "atlas/runtime/atlasInstance2.h"

#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"
#include "lightingSystem/synapseGaming/sgLighting.h"

AtlasInstanceColorSource::AtlasInstanceColorSource(AtlasInstance* atlas, const U32 sourceSize)
: model(sgLightingModelManager::sgGetLightingModel())
{
   AssertFatal(mAtlas, "Must pass an AtlasInstance!");
   AssertFatal(sourceSize > 0, "Source size invalid!");
   AssertFatal(isPow2(sourceSize), "Source size must be a power of two!");

   mAtlas = atlas;   
   mSourceSize = sourceSize; 
   mSurfaceHelper = NULL;   
}

AtlasInstanceColorSource::~AtlasInstanceColorSource()
{
   for (U32 i = 0; i < mCurrHelpers.size(); i++)
   {
      SAFE_DELETE(mCurrHelpers[i]);
   }
}

void AtlasInstanceColorSource::getExtents(int &width, int &height)
{
   height = width = mSourceSize;
}

void AtlasInstanceColorSource::addLight(LightInfo* light)
{
   mLights.push_back(light);
}

// This is here because RectF::contains does a min <= point <= min+extent and we want min <= point < extent (non inclusive end)
bool AtlasInstanceColorSource::inRect(const RectF& r, const Point2F& tc)
{   
   return ((tc.x >= r.point.x) && (tc.y >= r.point.y) && (tc.x < r.point.x + r.extent.x) && (tc.y < r.point.y + r.extent.y));
}

// This is the spot to modify the caching behavior of the lightmapper.  Right now we cache everything,
// but for larger maps that may not be the way to go.
AtlasSurfaceQueryHelper* AtlasInstanceColorSource::getSurfaceHelper(const Point2F& tc)
{
   // Stupidly search for our chunk for now           
   if ((!mSurfaceHelper) || (!inRect(mSurfaceHelper->mChunk->mTCBounds, tc)))
   {
      AtlasResourceGeomTOC *toc = mAtlas->getGeomTOC()->getResourceTOC();
      U32 treeDepth = toc->getTreeDepth();
      bool bFound = false;
      
      for (U32 y = 0; y < BIT(treeDepth-1) && (!bFound); y++)
      {
         for (U32 x = 0; x < BIT(treeDepth-1) && (!bFound); x++)
         {            
            AtlasResourceGeomStub* stub = toc->getStub(treeDepth-1, Point2I(x, y));
            if (inRect(stub->mChunk->mTCBounds, tc))            
            {
               U32 index = BIT(treeDepth-1)*y+x;
               while (index >= mCurrHelpers.size())
               {
                  mCurrHelpers.increment();
                  mCurrHelpers.last() = NULL;
               }
               if (mCurrHelpers[index] == NULL)
               {                  
                  toc->immediateLoad(stub, AtlasTOC::RootLoad);
                  AtlasSurfaceQueryHelper* sqh = new AtlasSurfaceQueryHelper(this);
                  sqh->mChunk = stub->mChunk;
                  sqh->mTexBounds = stub->mChunk->mTCBounds;
                  sqh->generateLookupTables();
                  mCurrHelpers[index] = sqh;
                  return sqh;
               } else {
                  return mCurrHelpers[index];
               }
            }
         }
      }
   } 
   return mSurfaceHelper;
}

void AtlasInstanceColorSource::handleLight(LightInfo* light, const Point3F& pos, const Point3F& normal, bool allowdiffuse, bool allowambient, F32& r, F32& g, F32& b)
{
   SG_CHECK_LIGHT(light);
   sgLightInfo* sgLight = static_cast<sgLightInfo*>(light);

   ColorF ambient(0.0f, 0.0f, 0.0f);
   ColorF diffuse(0.0f, 0.0f, 0.0f);
   VectorF lightingnormal(0.0f, 0.0f, 0.0f);
   model.sgLightingLM(pos, normal, diffuse, ambient, lightingnormal);

   if(allowdiffuse && ((diffuse.red > SG_MIN_LEXEL_INTENSITY) ||
      (diffuse.green > SG_MIN_LEXEL_INTENSITY) || (diffuse.blue > SG_MIN_LEXEL_INTENSITY)))
   {
      // Check for shadows
      F32 dot = mDot(normal, -light->mDirection);                  
      bool shadowed = (dot < 0);
      if (!shadowed)
      {      
         RayInfo info;
         if(sgLight->sgCastsShadows && sgLightManager::sgAllowShadows())
         {
            // set light pos for shadows...
            Point3F lightpos = light->mPos;
            if(light->mType == LightInfo::Vector)
            {
               lightpos = SG_STATIC_LIGHT_VECTOR_DIST * light->mDirection * -1;            
               lightpos = pos + lightpos;
            }
            
            RayInfo info;         
            shadowed = mAtlas->getContainer()->castRay((pos + (normal * 0.1f)), lightpos, ShadowCasterObjectType, &info);
         } 
      } 

      if(!shadowed)
      {
         r += diffuse.red * dot;
         g += diffuse.green * dot;
         b += diffuse.blue * dot;
      }
   }

   if(allowambient && ((ambient.red > 0.0f) || (ambient.green > 0.0f) || (ambient.blue > 0.0f)))
   {
      r += ambient.red;
      g += ambient.green;
      b += ambient.blue;      
   }   
}

void AtlasInstanceColorSource::getColor(int x, int y, unsigned char &r, unsigned char &g, unsigned char &b, unsigned char &a)
{        
   // Set some defaults
   F32 lr, lg, lb;
   lr = lg = lb = 0;
   r = g = b = 0;
   a = 255;

   // Figure out our texture coordinate.  (The swap is intentional to deal with texture space vs. pixel space)
   Point2F tc(y / (F32) mSourceSize, x / (F32) mSourceSize);   
   mSurfaceHelper = getSurfaceHelper(tc);
   AssertFatal(mSurfaceHelper, "Surface helper not found!");
   if (!mSurfaceHelper)   
      return;   

   U16 triIndex;
   Point3F outPos, outNorm;   
   if (mSurfaceHelper->lookup(tc, true, triIndex, outPos, outNorm))
   {
      mAtlas->getRenderTransform().mulP(outPos);
      mAtlas->getRenderTransform().mulV(outNorm);
      for (U32 i = 0; i < mLights.size(); i++)
      {
         LightInfo* light = mLights[i];
         SG_CHECK_LIGHT(light);
         sgLightInfo* sgLight = static_cast<sgLightInfo*>(light);
         // setup zone info...
         bool isinzone = (sgLight->sgZone[0] == 0) || (sgLight->sgZone[1] == 0);

         // allow what?
         bool allowdiffuse = (!sgLight->sgDiffuseRestrictZone) || isinzone;
         bool allowambient = (!sgLight->sgAmbientRestrictZone) || isinzone;

         // should I bother?
         if((allowdiffuse) || (allowambient))
         {          
            model = sgLightingModelManager::sgGetLightingModel(sgLight->sgLightingModelName);
            model.sgSetState(sgLight);
            model.sgInitStateLM();

            handleLight(mLights[i], outPos, outNorm, allowdiffuse, allowambient, lr, lg, lb);

            model.sgResetState();
         }
      }
      r = (mClampF(lr, 0.0f, 1.0f) * 255);
      g = (mClampF(lg, 0.0f, 1.0f) * 255);
      b = (mClampF(lb, 0.0f, 1.0f) * 255);

   } else {
      AssertFatal(false, "Surface helper query failed!");
   }
}

int AtlasInstanceColorSource::getNumberChannels()
{
   // Indicate we're not a multi-plane image - just one layer.
   return 0;         
}

void AtlasInstanceColorSource::sampleChannels(int x, int y, unsigned char *out)
{
   // Shouldn't get called because we are not a multi-plane image.
}