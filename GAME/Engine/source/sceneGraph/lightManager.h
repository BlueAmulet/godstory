//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _LIGHTMANAGER_H_
#define _LIGHTMANAGER_H_

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

class SceneObject;
class SimObject;
class LightInfo;
class LightInfoList;
class Material;
class ProcessedMaterial;
struct SceneGraphData;
class Point3F;
class Box3F;
class AvailableSLInterfaces;

class LightManager
{
public:
   enum SpecialLightTypesEnum
   {
      slSunLightType,
      slTranslatedSunlightType,
      slSpecialLightTypesCount
   };

	virtual ~LightManager() { }

   // Called when the lighting manager should become active
   virtual void activate() { }
   // Called when we don't want the light manager active (should clean up)
   virtual void deactivate() { }
   // Return an "id" that other systems can use to load different versions of assets (custom shaders, etc.)
   // Should be short and contain no spaces and safe for filename use.
   virtual const char* getId() const = 0;

   // Returns the active scene lighting interface for this light manager.  
   virtual AvailableSLInterfaces* getSceneLightingInterface() = 0;

	// Returns a LightInfo
	virtual LightInfo* createLightInfo() = 0;
	// Returns a "default" light info that callers should not free.  Used for instances where we don't actually care about 
	// the light (for example, setting default data for SceneGraphData)
	virtual LightInfo* getDefaultLight() = 0;

   // "Special lights" currently this is the sun and the "translated" sun (used by terrain and interior).
   virtual LightInfo *getSpecialLight(SpecialLightTypesEnum type) = 0;
   virtual void setSpecialLight(SpecialLightTypesEnum type, LightInfo *light) = 0;

   // registered before scene traversal...
   virtual void registerGlobalLight(LightInfo *light, SimObject *obj, bool zonealreadyset) = 0;
   virtual void unregisterGlobalLight(LightInfo *light) = 0;
   // registered per object...
   virtual void registerLocalLight(LightInfo *light) = 0;
   virtual void unregisterLocalLight(LightInfo *light) = 0;

   virtual void registerGlobalLights(bool staticlighting) = 0;
   virtual void unregisterAllLights() = 0;

   /// Returns all unsorted and un-scored lights (both global and local).
   virtual void getAllUnsortedLights(LightInfoList &list) = 0;

   /// For DTS objects.  Finds the best lights
   /// including a composite based on the environmental
   /// ambient lighting amount *and installs them in OpenGL*.
   virtual void setupLights(SceneObject *obj) = 0;
   /// For the terrain and Atlas.  Finds the best
   /// lights in the viewing area based on distance to camera.
   virtual void setupLights(SceneObject *obj, const Point3F &camerapos,
      const Point3F &cameradir, F32 viewdist, S32 maxlights) = 0;
   /// Finds the best lights that overlap with the bounding box
   /// based on the box center.
   virtual void setupLights(SceneObject *obj, const Box3F &box, S32 maxlights) = 0;
   /// Copies the best lights list - this DOESN'T find the lights!  Call
   /// setupLights to populate the list.
   virtual void getBestLights(LightInfoList &list) = 0;   
   /// Reset the best lights list and all associated data.
   virtual void resetLights() = 0;

   /// Sets shader constants / textures for light infos
   virtual void setLightInfo(ProcessedMaterial* pmat, const Material* mat, const SceneGraphData& sgData, U32 pass) = 0;

   /// Sets the blend state for a lighting pass
   virtual void setLightingBlendFunc() = 0;

   /// Allows us to set textures during the Material::setTextureStage call, return true if we've done work.
   virtual bool setTextureStage(const SceneGraphData& sgData, const U32 currTexFlag, const U32 textureSlot) = 0;

   // Called when the scene lighting should be computed.
   virtual bool lightScene(const char* callback, const char* param);

   // Called to find out if it is valid to activate this lighting system.  If not, we should print out
   // a console warning explaining why.
   virtual bool canActivate();

   virtual void hdrPrepare(const Point2I &offset, const Point2I &extent) = 0;
   virtual void hdrRender() = 0;
};

class LightManagerFactory;
typedef Vector<LightManagerFactory*> LightManagerFactoryList;

class AvailableLightManagers
{
private:   
   LightManagerFactoryList mFactoryList;
   S32 mCurrentLMIndex;
   LightManager* mCurrentLM;   

   static AvailableLightManagers* smSingleton;
   AvailableLightManagers();
   ~AvailableLightManagers();
public:
   inline static AvailableLightManagers* get() { if (smSingleton == NULL) { smSingleton = new AvailableLightManagers(); } return smSingleton; }
   static void destroyManagers() { if (smSingleton) { delete smSingleton; smSingleton = NULL;} }

   void registerFactory(LightManagerFactory* lmf);

   LightManager* getDefaultLM();
   LightManager* getLMByName(const char* lmName);
   
   U32 getLMCount() const;
   LightManager* getLM(const U32 index);
   const char* getLMName(const U32 index) const;

   inline LightManager* getCurrentLM() { return mCurrentLM; };
   inline S32 getCurrentLMIndex() { return mCurrentLMIndex; };
   inline const char* getCurrentLMName() { return getLMName(mCurrentLMIndex); };

   void cleanupCurrentLM();   
};

/// This class's main purpose is to describe and create a light manager.
class LightManagerFactory
{
public:
   inline LightManagerFactory() { AvailableLightManagers::get()->registerFactory(this); }
   virtual LightManager* createLightManager() = 0;
   virtual void cleanupLightManager() = 0;
   virtual const char* getName() const = 0;    
};

class TestHulingyun
{
public:
	static U32 smTestId;
	static int MAX_SIZE;

	struct Test_Struct
	{
		U32 nTestId;
		void *nTestObj;

		Test_Struct() : nTestId(0), nTestObj(NULL)  {  }
	};

	void Insert(void *);
	void Delete(void *);
	void Print();
	int  GetCount()		{ return m_count; };

	TestHulingyun();
	~TestHulingyun();

private:
	Test_Struct *list;
	int m_count;
};

extern TestHulingyun g_TestHulingyun;

#endif
