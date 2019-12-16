//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _BASICLIGHTMANAGER_H_
#define _BASICLIGHTMANAGER_H_

// Uncomment to force basic lighting to work only with basicLights.  Otherwise, we can work
// with anything.  Useful for debugging cleanup issues.
//#define BASIC_LIGHTS_ONLY

#ifndef _LIGHTMANAGER_H_
#include "sceneGraph/lightManager.h"
#endif 

#ifndef _UTIL_SIGNAL_H_
#include "util/tSignal.h"
#endif

class BasicLightInfo;
class AvailableSLInterfaces;

class BasicLightManager : public LightManager
{
private:
   // Singleton support
   static BasicLightManager* smBasicLightManagerSingleton;
   LightInfo* mLastLightSentToShader;

   BasicLightInfo* mDefaultLight;
   BasicLightInfo* mSpecialLights[LightManager::slSpecialLightTypesCount];
   BasicLightInfo* mTranslatedSun;   
   AvailableSLInterfaces* mLightingInterfaces;
public:
   BasicLightManager();
	virtual ~BasicLightManager();

   // Gets the singleton
   static BasicLightManager* get()
   {
      if (smBasicLightManagerSingleton)
         return smBasicLightManagerSingleton;
      smBasicLightManagerSingleton = new BasicLightManager();
      return smBasicLightManagerSingleton;
   }

   static void cleanup();

   Signal<bool> mActivate;
   virtual bool canActivate();
   virtual void activate();
   virtual void deactivate();
   virtual const char* getId() const;

   // Returns the active scene lighting interface for this light manager.  
   virtual AvailableSLInterfaces* getSceneLightingInterface();

	// Returns a LightInfo
	virtual LightInfo* createLightInfo();
	// Returns a "default" light info that callers should not free.  Used for instances where we don't actually care about 
	// the light (for example, setting default data for SceneGraphData)
	virtual LightInfo* getDefaultLight();
   virtual BasicLightInfo* createBasicLightInfo();

   // "Special lights" currently this is the sun and the "translated" sun (used by terrain and interior).
   virtual LightInfo *getSpecialLight(LightManager::SpecialLightTypesEnum type);
   virtual void setSpecialLight(LightManager::SpecialLightTypesEnum type, LightInfo *light);

   // registered before scene traversal...
   virtual void registerGlobalLight(LightInfo *light, SimObject *obj, bool zonealreadyset) { }
   virtual void unregisterGlobalLight(LightInfo *light) { }
   // registered per object...
   virtual void registerLocalLight(LightInfo *light) { }
   virtual void unregisterLocalLight(LightInfo *light) { }

   virtual void registerGlobalLights(bool staticlighting);
   virtual void unregisterAllLights() { }

   /// Returns all unsorted and un-scored lights (both global and local).
   virtual void getAllUnsortedLights(LightInfoList &list);

   /// For DTS objects.  Finds the best lights
   /// including a composite based on the environmental
   /// ambient lighting amount *and installs them in OpenGL*.
   virtual void setupLights(SceneObject *obj) { }
   /// For the terrain and Atlas.  Finds the best
   /// lights in the viewing area based on distance to camera.
   virtual void setupLights(SceneObject *obj, const Point3F &camerapos,
      const Point3F &cameradir, F32 viewdist, S32 maxlights) { }
   /// Finds the best lights that overlap with the bounding box
   /// based on the box center.
   virtual void setupLights(SceneObject *obj, const Box3F &box, S32 maxlights) { }
   /// Copies the best lights list - this DOESN'T find the lights!  Call
   /// setupLights to populate the list.
   virtual void getBestLights(LightInfoList &list);
   /// Reset the best lights list and all associated data.
   virtual void resetLights() { }

   /// Sets shader constants / textures for light infos
   virtual void setLightInfo(ProcessedMaterial* pmat, const Material* mat, const SceneGraphData& sgData, U32 pass);

   /// Sets the blend state for a lighting pass
   virtual void setLightingBlendFunc() { }

   /// Allows us to set textures during the Material::setTextureStage call, return true if we've done work.
   virtual bool setTextureStage(const SceneGraphData& sgData, const U32 currTexFlag, const U32 textureSlot) { return false; }

   // Do the static scene lighting.
   bool lightScene(const char* callback, const char* param);

   virtual void hdrPrepare(const Point2I &offset, const Point2I &extent) {}
   virtual void hdrRender() {}
};

#define BLM BasicLightManager::get()

#ifdef BASIC_LIGHTS_ONLY
#define BASIC_CHECK_LIGHTINFO(x) AssertFatal(dynamic_cast<BasicLightInfo*>(x), "Incorrect light type!")
#else
#define BASIC_CHECK_LIGHTINFO(x)
#endif

#endif
