//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

// This file contains the interface between the lighting kit and the legacy terrain rendering system.

#include "core/bitVector.h"
#include "sceneGraph/shadowVolumeBSP.h"
#include "terrain/terrData.h"
#include "terrain/terrRender.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/common/sceneLighting.h"
#include "sceneGraph/lightingInterfaces.h"
#include "lightingSystem/synapseGaming/terrain/sgTerrainLightmap.h"
#include "lightingSystem/synapseGaming/terrain/sgTerrainProxy.h"

//
// Lighting system interface
//
class sgTerrainSystem : public SceneLightingInterface
{
public:
   sgTerrainSystem();
   virtual void init();
   virtual U32 addObjectType();
   virtual SceneLighting::ObjectProxy* createObjectProxy(SceneObject* obj, SceneLighting::ObjectProxyList* sceneObjects);
   virtual PersistInfo::PersistChunk* createPersistChunk(const U32 chunkType);
   virtual bool createPersistChunkFromProxy(SceneLighting::ObjectProxy* objproxy, PersistInfo::PersistChunk **ret);      

   // Given a ray, this will return the color from the lightmap of this object, return true if handled
   virtual bool getColorFromRayInfo(RayInfo collision, ColorF& result);
};

sgTerrainSystem::sgTerrainSystem()
{
   SGLM->getLightingInterfaces()->registerSystem(this);
}

void sgTerrainSystem::init()
{
}

U32 sgTerrainSystem::addObjectType()
{
   return TerrainObjectType;
}

SceneLighting::ObjectProxy* sgTerrainSystem::createObjectProxy(SceneObject* obj, SceneLighting::ObjectProxyList* sceneObjects)
{
   if ((obj->getTypeMask() & TerrainObjectType) != 0)
      return new TerrainProxy(obj);
   else
      return NULL;
}

PersistInfo::PersistChunk* sgTerrainSystem::createPersistChunk(const U32 chunkType)
{
   if (chunkType == PersistInfo::PersistChunk::TerrainChunkType)
      return new TerrainChunk();
   else
      return NULL;
}

bool sgTerrainSystem::createPersistChunkFromProxy(SceneLighting::ObjectProxy* objproxy, PersistInfo::PersistChunk **ret)
{
   if (dynamic_cast<TerrainProxy*>(objproxy) != NULL)
   {
      *ret = new TerrainChunk();
      return true;
   } else {
      return NULL;
   }
}

// Given a ray, this will return the color from the lightmap of this object, return true if handled
bool sgTerrainSystem::getColorFromRayInfo(RayInfo collision, ColorF& result)
{
   TerrainBlock *terrain = dynamic_cast<TerrainBlock *>(collision.object);
   if (!terrain)
      return false;

   Point2F uv;
   F32 terrainlength = (F32)(terrain->getSquareSize() * TerrainBlock::BlockSize);
   Point3F pos = terrain->getPosition();
   uv.x = (collision.point.x - pos.x) / terrainlength;
   uv.y = (collision.point.y - pos.y) / terrainlength;

   // similar to x = x & width...
   uv.x = uv.x - F32(U32(uv.x));
   uv.y = uv.y - F32(U32(uv.y));
   GBitmap* lightmap = terrain->lightMap;
   if (!lightmap)
      return false;

   result = lightmap->sampleTexel(uv.x, uv.y);
   // terrain lighting is dim - look into this (same thing done in shaders)...
   result *= 2.0;
   return true;
}

// File private static factory
static sgTerrainSystem p_sgTerrainSystem;