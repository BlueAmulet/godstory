//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _LIGHTINGSYSTEM_SHADOWCASTER_H_
#define _LIGHTINGSYSTEM_SHADOWCASTER_H_

#include "lightingSystem/synapseGaming/sgHashMap.h"
#include "lightingSystem/common/shadowBase.h"
#include "core/fileStream.h"

class SceneObject;
class LightInfo;
class TSShapeInstance;

// Makes life easier
typedef hash_multimap<U32, ShadowBase*> ShadowMultimap;

class ShadowCaster
{
   bool mIsStaticObject;

   /// Multimap of all of our shadows
   ShadowMultimap mShadowMultimap;

   /// Gets the first entry in the shadow multimap for looping through the whole thing
   ShadowMultimap* getFirstShadowEntry();

   /// Deletes all shadows in our multimap
   void clearShadowMultimap();

   /// Finds a shadow for the given shape and light in our multimap, or creates one if necessary
   ShadowBase* lookupShadow(SceneObject* parentObject, LightInfo* light, TSShapeInstance* shapeInstance);

   /// Creates a new shadow (Factory)
   ShadowBase* createNewShadow(SceneObject* parentObject, LightInfo* light, TSShapeInstance* shapeInstance);

   /// Converts a LightInfo* to a hash lookup
   U32 lightToHash(LightInfo *light) { return U32(light); }

   /// Converts a hash lookup to a LightInfo*
   LightInfo* hashToLight(U32 hash) { return (LightInfo*)hash; }

   /// Have we registered with ShadowCasterManager
   bool mIsRegistered;

   /// When we last rendered a shadow
   U32 mLastRenderTime;

   ShadowBase* mSimpleShadow;	//Ray: 简单阴影，当设置为简单或者不知道SHADER2.0时候使用

public:
   ShadowCaster();
   ~ShadowCaster();

   /// Renders all shadows that need rendering
   void render(SceneObject* parentObject, TSShapeInstance* shapeInstance, F32 camDist, bool forceUpdate = false);

   /// Cleans up all shadows which haven't been used in a while
   void cleanupUnusedShadows(U32 time);

   /// Deletes all shadows
   void clearAllShadows();

   /// reset all shadows' shapeInstance
   void resetShapeInst(TSShapeInstance* shapeInst);

   void saveDynamicShadowData(SceneObject* parentObject, TSShapeInstance* shapeInstance,Stream *stream);
   void setDynamicShadowData(SceneObject* parentObject, TSShapeInstance* shapeInstance, void *,void *);
   void freeResource();
};

#endif