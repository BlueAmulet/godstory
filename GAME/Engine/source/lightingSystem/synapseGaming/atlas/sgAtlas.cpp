//-----------------------------------------------------------------------------
// PowerEngine Game Engine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "platform/platform.h"
#include "math/mMatrix.h"
#include "math/mathUtils.h"
#include "lightingSystem/common/sceneLighting.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "sceneGraph/lightingInterfaces.h"
#include "lightingSystem/common/sceneLighting.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"
#include "lightingSystem/synapseGaming/sgLightMap.h"
#include "atlas/runtime/atlasInstance2.h"

#include "lightingSystem/synapseGaming/atlas/atlasSceneLightmap.h"
#include "atlas/resource/atlasResourceConfigTOC.h"
#include "atlas/resource/atlasResourceTexTOC.h"
#include "atlas/resource/atlasResourceGeomTOC.h"
#include "atlas/editor/atlasImageImport.h"

#include "lightingSystem/common/sceneLighting.h"

//
// PersistChunk (sgScenePersist.h)
//
struct AtlasLightMapChunk : public PersistInfo::PersistChunk
{
	typedef PersistInfo::PersistChunk Parent;

	AtlasLightMapChunk()
	{
		mChunkType = PersistInfo::PersistChunk::AtlasLightMapChunkType;
	}
	~AtlasLightMapChunk()
	{
	}
};

class AtlasLightMapProxy : public SceneLighting::ObjectProxy
{
private:
   typedef ObjectProxy Parent;
   AtlasInstanceColorSource* aics;
   U32 mSourceWidth;
public:
   AtlasInstance *sgAtlas;
   AtlasLightMapProxy(SceneObject *obj);
   ~AtlasLightMapProxy();
   virtual void light(LightInfo *light);
   virtual void postLight(bool lastlight);
   virtual U32 getResourceCRC();
   virtual bool setPersistInfo(PersistInfo::PersistChunk *);
   virtual bool getPersistInfo(PersistInfo::PersistChunk *);
};

AtlasLightMapProxy::AtlasLightMapProxy(SceneObject *obj) : ObjectProxy(obj)
{
   sgAtlas = dynamic_cast<AtlasInstance *>((SceneObject *)obj);
   AssertFatal((sgAtlas), "Invalid atlas or stub instance.");   
   mSourceWidth = sgAtlas->getLightMapDimension();
   if (sgAtlas->getLightMapChunkSize() > (S32)mSourceWidth)
   {
      mSourceWidth = sgAtlas->getLightMapChunkSize();
      Con::errorf("Minimum lightmap size for this terrain is: %d", mSourceWidth);
   }
   AssertFatal(mSourceWidth > 0, "Invalid lightmap size!");
   AssertFatal(isPow2(mSourceWidth), "Lightmap size must be a power of 2!");

   aics = new AtlasInstanceColorSource(sgAtlas, mSourceWidth);
}

AtlasLightMapProxy::~AtlasLightMapProxy()
{
   delete aics;
}

// Called to set lightmap from cache.
bool AtlasLightMapProxy::setPersistInfo(PersistInfo::PersistChunk * info)
{
   if(!Parent::setPersistInfo(info))
      return(false);

   AtlasLightMapChunk *chunk = dynamic_cast<AtlasLightMapChunk *>(info);
   AssertFatal(chunk, "SceneLighting::AtlasLightMapChunk::setPersistInfo: invalid info chunk!");

   if(!sgAtlas)
      return false;

   char baseAtlas[256];
   dSprintf(baseAtlas, sizeof(baseAtlas), "%s", sgAtlas->getAtlasFilename());
   char * dot = dStrstr((const char*)baseAtlas, ".atlas");
   if(dot)
      *dot = '\0';

   char cachedLighting[256];
   dSprintf(cachedLighting, sizeof(cachedLighting), "%s_%x.atlas", baseAtlas, gLighting->calcMissionCRC());
   if (Platform::isFile(cachedLighting))
   {
      sgAtlas->loadAlternateLightMap(cachedLighting);
      return true;
   } else {
      return false;
   }
}

void AtlasLightMapProxy::light(LightInfo *light)
{ 
   aics->addLight(light);
}

void AtlasLightMapProxy::postLight(bool lastlight)
{
   if (lastlight)
   {

      sgAtlas->releaseLightmap();

      AtlasFile af;

      // Initialize our TOCs...
      //    - Config TOC
      AtlasResourceConfigTOC *arctoc = new AtlasResourceConfigTOC();
      arctoc->initialize(2);
      af.registerTOC(arctoc);

      U32 texTocDepth = getBinLog2(mSourceWidth) - getBinLog2(sgAtlas->getLightMapChunkSize()) + 1; 
      AtlasImageImporter aii(&af, aics, texTocDepth);
      aii.registerTOC();

      char baseAtlas[256];
      dSprintf(baseAtlas, sizeof(baseAtlas), "%s", sgAtlas->getAtlasFilename());
      char * dot = dStrstr((const char*)baseAtlas, ".atlas");
      if(dot)
         *dot = '\0';

      char cachedLighting[256];
      dSprintf(cachedLighting, sizeof(cachedLighting), "%s_%x.atlas", baseAtlas, gLighting->calcMissionCRC());

      // Create atlas file and kick off the threads...
      af.createNew(cachedLighting);   
      af.startLoaderThreads();

      // First, create a config TOC for a straight up diffuse mapped terrain.
      AtlasConfigChunk *acc = new AtlasConfigChunk;

      acc->setEntry("type", "blender");
      acc->setEntry("shadowMapSlot", "0");
      acc->setEntry("virtualTexSize", "32768");

      arctoc->addConfig("schema", acc);

      aii.populateChunks();

      delete acc;
      delete arctoc;
   }
}

// Checks to see if the cached data is good / load it newly computed stuff (?)
bool AtlasLightMapProxy::getPersistInfo(PersistInfo::PersistChunk * info)
{
   if(!Parent::getPersistInfo(info))
      return(false);

   // Load the calculated lighting out, (this may move)
   char baseAtlas[256];
   dSprintf(baseAtlas, sizeof(baseAtlas), "%s", sgAtlas->getAtlasFilename());
   char * dot = dStrstr((const char*)baseAtlas, ".atlas");
   if(dot)
      *dot = '\0';

   char cachedLighting[256];
   dSprintf(cachedLighting, sizeof(cachedLighting), "%s_%x.atlas", baseAtlas, gLighting->calcMissionCRC());

   sgAtlas->loadAlternateLightMap(cachedLighting);

   return true;
}

U32 AtlasLightMapProxy::getResourceCRC()
{
   return sgAtlas->getCRC();
}

//
// Lighting system 
//
class sgAtlasSystem : public SceneLightingInterface
{
public:
   sgAtlasSystem();
   virtual SceneLighting::ObjectProxy* createObjectProxy(SceneObject* obj, SceneLighting::ObjectProxyList* sceneObjects);
   virtual PersistInfo::PersistChunk* createPersistChunk(const U32 chunkType);
   virtual bool createPersistChunkFromProxy(SceneLighting::ObjectProxy* objproxy, PersistInfo::PersistChunk **ret);
   virtual bool postProcessLoad(PersistInfo* pi, SceneLighting::ObjectProxyList* sceneObjects);
   virtual U32 addObjectType();
   // Given a ray, this will return the color from the lightmap of this object, return true if handled
   virtual bool getColorFromRayInfo(RayInfo collision, ColorF& result);
};

sgAtlasSystem::sgAtlasSystem()
{
   SGLM->getLightingInterfaces()->registerSystem(this);
}

SceneLighting::ObjectProxy* sgAtlasSystem::createObjectProxy(SceneObject* obj, SceneLighting::ObjectProxyList* sceneObjects)
{
	AtlasInstance *atlas = dynamic_cast<AtlasInstance *>(obj);
	// don't error out this could be atlas1...   
   if (!atlas)
      return NULL;

   SceneLighting::ObjectProxy* proxy = NULL;

   if (atlas->isBlended() && atlas->getLightMapDimension() > 0)
   {
      if (isPow2(atlas->getLightMapDimension()))
      {
	      proxy = new AtlasLightMapProxy(atlas);
	      proxy->calcValidation();
	      proxy->loadResources();		   
      }
      else
         Con::errorf("Lightmap dimension must be a power of 2!");
   }
   else
      atlas->releaseLightmap();     // Just in case we had one, but we want to get rid of it with dim = 0

   return proxy;
}

PersistInfo::PersistChunk* sgAtlasSystem::createPersistChunk(const U32 chunkType) 
{
   if (chunkType == PersistInfo::PersistChunk::AtlasLightMapChunkType)
      return new AtlasLightMapChunk;
   else
      return NULL;
}

bool sgAtlasSystem::createPersistChunkFromProxy(SceneLighting::ObjectProxy* objproxy, PersistInfo::PersistChunk **ret)
{
   if (dynamic_cast<AtlasLightMapProxy*> (objproxy))
   {
      *ret = new AtlasLightMapChunk;
      return true;
   }
   *ret = NULL;
   return ((objproxy->mObj->getTypeMask() & AtlasObjectType) != 0);
}

bool sgAtlasSystem::postProcessLoad(PersistInfo* pi, SceneLighting::ObjectProxyList* sceneObjects)
{
   return true;
}

U32 sgAtlasSystem::addObjectType()
{
   return AtlasObjectType;
}

// Given a ray, this will return the color from the lightmap of this object, return true if handled
bool sgAtlasSystem::getColorFromRayInfo(RayInfo collision, ColorF& result)
{
	if( !dynamic_cast< AtlasInstance* >( collision.object ) )
		return false;

	// Get to the lightmap file and retrieve the texture TOC.
	// Note that this code only works with the lightmaps produced by this module.
	// It does not work with lightmaps packaged with the main map (suppose there
	// is no need it does).

	AtlasInstance* atlas = ( AtlasInstance* ) collision.object;
	AtlasFile* lightMapFile = atlas->getLightMapFile();
	if( !lightMapFile )
		return false;
	AtlasResourceTexTOC* arttoc;
	if( !lightMapFile->getTocBySlot( 0, arttoc ) )
		return false;

	// Get some key figures.

	U32 treeDepth = arttoc->getTreeDepth();
	U32 chunkSize = arttoc->getTextureChunkSize();
	U32 lightmapSize = BIT( treeDepth - 1 ) * chunkSize;

	// Scale the texcoords to lightmap coords.

	Point2I lightmapCoords( collision.texCoord.x * F32( lightmapSize ),
		collision.texCoord.y * F32( lightmapSize ) );

	// Scale the lightmap coords into chunk coords.

	Point2I chunkXY( lightmapCoords.x / chunkSize, lightmapCoords.y / chunkSize );

	// Walk the TOC tree bottom up and stop at the first matching
	// piece of data that is available.

	for( int i = treeDepth - 1; i >= 0; -- i )
	{
		// Get the stub and, if its data is online, query its texture.

		AtlasResourceTexStub* stub = arttoc->getStub( i, chunkXY );
		if( stub->hasResource() )
		{
			// Data is there.  Grab the bitmap and read out the color information.

			if( !stub->mChunk->isBitmapTexFormat( stub->mChunk->mFormat ) )
				return false;
			
			GBitmap* bitmap = stub->mChunk->bitmap;

			U32 shift = treeDepth - i - 1;
			U32 x = ( lightmapCoords.x >> shift ) - chunkXY.x * chunkSize;
			U32 y = ( lightmapCoords.y >> shift ) - chunkXY.y * chunkSize;

			ColorI color;
			bitmap->getColor( x, y, color );

			result = ColorF( F32( color.red ) / 255.0 * 2.0, F32( color.green ) / 255 * 2.0,
				F32( color.blue ) / 255.0 * 2.0, 1.0 );

			return true;
		}
		else
		{
			// No data.

			//TODO: This is the place to request the data to be loaded.  However, as long
			//  as there isn't also a place where we kick out chunks no longer needed, this
			//  is not a reasonable thing to do.
			//arttoc->requestLoad( stub, AtlasTOC::NormalPriority,
			//	F32( treeDepth - i ) / F32( treeDepth + 1 ) );

			// When going up a level, we need to scale our texcoords
			// to the new level dimensions.

			chunkXY.x >>= 1;
			chunkXY.y >>= 1;
		}
	}

	return false;
}


// Static factory object registers with lighting system plug-in system
static sgAtlasSystem p_sgAtlasSystem;
