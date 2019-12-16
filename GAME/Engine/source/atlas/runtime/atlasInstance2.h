//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _ATLASINSTANCE2_H_
#define _ATLASINSTANCE2_H_

#include "platform/threads/thread.h"
#include "platform/threads/mutex.h"
#include "core/stream.h"
#include "core/resManager.h"
#include "util/safeDelete.h"
#include "gfx/gfxDevice.h"
#include "gfx/gfxStructs.h"
#include "sceneGraph/RenderableSceneObject.h"
#include "materials/matInstance.h"
#include "atlas/core/atlasFile.h"
#include "atlas/runtime/atlasInstanceGeomTOC.h"
#include "atlas/runtime/atlasInstanceTexTOC.h"
#include "atlas/runtime/atlasClipMapBatcher.h"

class ClipMap;
class ClipMapBlenderCache;
class AtlasClipMapImageSource;

/// @defgroup AtlasRuntime Atlas Runtime
///
/// Functionality that relates to rendering and interacting with an Atlas
/// terrain in a running game instance.
///
/// @ingroup Atlas

/// An instance of an Atlas terrain.
///
/// Provides glue code to bring an Atlas terrain into the game world!
///
/// @ingroup AtlasRuntime
class AtlasInstance : public RenderableSceneObject
{
   friend class AtlasClipMapBatcher;
   typedef RenderableSceneObject Parent;

   Resource<AtlasFile> mAtlasFile;
   AtlasInstanceGeomTOC *mGeomTOC;

   Resource<AtlasFile> mLightMapFile;

   GFXTexHandle mDetailTex;

   StringTableEntry mDetailTexFileName;
   StringTableEntry mAtlasFileName;

   ClipMap *mClipMap;
   AtlasClipMapBatcher mBatcher;

   // Blended atlas specific stuff
   bool mIsBlended;
   ClipMapBlenderCache* mAcmic_b;   
   AtlasResourceTexTOC* mOpacityToc;
   U32 mLightmapDimension;
   S32 mLMChunkSize;

   void syncThreads();
public:
   AtlasInstance();
   ~AtlasInstance();
   DECLARE_CONOBJECT(AtlasInstance);

   AtlasInstanceGeomTOC *getGeomTOC()
   {
      return mGeomTOC;
   }

   AtlasFile* getAtlasFile()
   {
	   return mAtlasFile;
   }
   AtlasFile* getLightMapFile()
   {
	   return mLightMapFile;
   }

   void findDeepestStubs(Vector<AtlasResourceGeomTOC::StubType *> &stubs);

   bool onAdd();
   void onRemove();
   virtual void inspectPostApply();

   static void consoleInit();
   static void initPersistFields();

   bool prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState);
   void renderObject( SceneState *state, RenderInst *ri );

   bool castRay(const Point3F &start, const Point3F &end, RayInfo* info);
   void buildConvex(const Box3F& box, Convex* convex);
   bool buildPolyList(AbstractPolyList* polyList, const Box3F &box, const SphereF &sphere);

   U64 packUpdate(NetConnection * conn, U64 mask, BitStream *stream);
   void unpackUpdate(NetConnection * conn, BitStream *stream);

   void refillClipMap();

   enum RayCollisionDebugLevel
   {
      RayCollisionDebugToTriangles = 0,
      RayCollisionDebugToColTree   = 1,
      RayCollisionDebugToChunkBox  = 2,
      RayCollisionDebugToObjectBox = 3,
      RayCollisionDebugToMesh      = 4, ///< Let raycasting test against raw geometry data rather than the collision mesh.
   };

   enum ChunkBoundsDebugMode
   {
      ChunkBoundsDebugNone          = 0,
      ChunkBoundsDebugLODThreshold  = 1,
      ChunkBoundsDebugLODHolistic   = 2,
      ChunkBoundsDebugHeat          = 3,
   };

   static S32  smRayCollisionDebugLevel;
   static S32  smRenderChunkBoundsDebugLevel;
   static bool smLockFrustrum;
   static bool smRenderWireframe;
   static bool smNoUpdate;

   // Lightmap support
   void loadAlternateLightMap(const StringTableEntry sLightmapFile);
   void releaseLightmap();
   U32 getCRC();
   S32 getLightMapChunkSize();
   const StringTableEntry getAtlasFilename() { return mAtlasFileName; }
   bool isBlended() { return mIsBlended; }
   U32 getLightMapDimension() { return mLightmapDimension; }
private:
	static GFXStateBlock* mFillWireSB;
	static GFXStateBlock* mFillSolidSB;
	static GFXStateBlock* mCullCWSB;
	static GFXStateBlock* mCullCCWSB;

public:
	static void init();
	static void shutdown();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};

#endif
