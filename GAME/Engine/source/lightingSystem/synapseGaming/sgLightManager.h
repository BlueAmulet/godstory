//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _SGLIGHTMANAGER_H_
#define _SGLIGHTMANAGER_H_

#ifndef _LIGHTMANAGER_H_
#include "sceneGraph/lightManager.h"
#endif
#ifndef _MMATH_H_
#include "math/mMath.h"
#endif
#ifndef _COLOR_H_
#include "core/color.h"
#endif
#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif
#ifndef _DATACHUNKER_H_
#include "core/dataChunker.h"
#endif
#include "core/stringTable.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "lightingSystem/synapseGaming/sgDynamicRangeLighting.h"
#ifndef _SGLIGHTINFO_H_
#include "lightingSystem/synapseGaming/sgLightInfo.h"
#endif
#ifndef _GFXDEVICE_H_
#include "gfx/gfxDevice.h"
#endif
#ifndef _SG_SYSTEM_INTERFACE_H
#include "sceneGraph/lightingInterfaces.h"
#endif

class SceneObject;
class MatInstance;
class sgMatInstance;
class SceneLighting;
class GFXStateBlock;

extern void sgFindObjectsCallback(SceneObject* obj, void *val);
extern bool gEditingMission;

class sgLightManager : public LightManager
{
private:
	static sgLightManager* smsgLightManagerSingleton;
   SceneLighting* mSceneLighting;
   AvailableSLInterfaces* mLightingInterfaces;
public:
   sgLightManager();
   virtual ~sgLightManager();

   AvailableSLInterfaces* getLightingInterfaces();

   // Gets the singleton
   inline static sgLightManager* get()
   {
	   if (smsgLightManagerSingleton)
		   return smsgLightManagerSingleton;
	   smsgLightManagerSingleton = new sgLightManager();
	   return smsgLightManagerSingleton;
   }

   inline static void cleanup()
   {
	   if (smsgLightManagerSingleton)
	   {
		   SAFE_DELETE(smsgLightManagerSingleton);
	   }
   }
   
   virtual void activate();
   // Called when we don't want the light manager active (should clean up)
   virtual void deactivate();

   virtual const char* getId() const;

   // Returns the active scene lighting interface for this light manager.  
   virtual AvailableSLInterfaces* getSceneLightingInterface();

   // Used to allow plug and play systems within the lighting manager to register for init and shutdown (like interiors)
   Signal<bool> mSgActivate;

   // Returns a LightInfo
   virtual LightInfo* createLightInfo();
   // Returns a "default" light info that callers should not free.  Used for instances where we don't actually care about 
   // the light
   virtual LightInfo* getDefaultLight();

   // This will be moved to the derived LightManager
   sgLightInfo* createSGLightInfo();

	// registered before scene traversal...
	void registerGlobalLight(LightInfo *light, SimObject *obj, bool zonealreadyset);
	inline void unregisterGlobalLight(LightInfo *light) {sgRegisteredGlobalLights.sgUnregisterLight(light);}
	// registered per object...
	inline void registerLocalLight(LightInfo *light) {sgRegisteredLocalLights.sgRegisterLight(light);}
	inline void unregisterLocalLight(LightInfo *light) {sgRegisteredLocalLights.sgUnregisterLight(light);}

	void registerGlobalLights(bool staticlighting);
	void unregisterAllLights();

	/// Returns all unsorted and un-scored lights (both global and local).
	void getAllUnsortedLights(LightInfoList &list);
	/// Copies the best lights list - this DOESN'T find the lights!  Call
	/// setupLights to populate the list.
	inline void getBestLights(LightInfoList &list)
	{
		list.clear();
		list.merge(sgBestLights);
	}
	/// Accepts a pre-filtered list instead of using the best lights list
	/// so interiors can filter lights against zones and then build the
	/// the dual list...
	void sgBuildDualLightLists(const LightInfoList &list, LightInfoDualList &listdual);

	/// For DST objects.  Finds the best lights
	/// including a composite based on the environmental
	/// ambient lighting amount *and installs them in OpenGL*.
	void setupLights(SceneObject *obj);
	/// For the terrain and Atlas.  Finds the best
	/// lights in the viewing area based on distance to camera.
	void setupLights(SceneObject *obj, const Point3F &camerapos,
		const Point3F &cameradir, F32 viewdist, S32 maxlights);
	/// Finds the best lights that overlap with the bounding box
	/// based on the box center.
	void setupLights(SceneObject *obj, const Box3F &box, S32 maxlights);
	/// Reset the best lights list and all associated data.
	void resetLights();

   /// Sets shader constants / textures for light infos
   virtual void setLightInfo(ProcessedMaterial* pmat, const Material* mat, const SceneGraphData& sgData, U32 pass);

   /// Sets the blend state for a lighting pass
   virtual void setLightingBlendFunc();

   /// Allows us to set textures during the Material::setTextureStage call
   virtual bool setTextureStage(const SceneGraphData& sgData, const U32 currTexFlag, const U32 textureSlot);

   /// Gets the dynamic lighting material for root+light combo.
   virtual sgMatInstance* getDynamicLightingMaterial(MatInstance *root, sgLightInfo *light, bool tryfordual);

   // Called when the scene lighting should be computed.
   virtual bool lightScene(const char* callback, const char* param);
private:
	sgLightInfo *sgSpecialLights[slSpecialLightTypesCount];
    sgLightInfo *sgTranslatedSun;   
public:
    inline LightInfo *sgGetDefaultLight() { return sgDefaultLight; }
	LightInfo *getSpecialLight(SpecialLightTypesEnum type);
	void setSpecialLight(SpecialLightTypesEnum type, LightInfo *light);
private:
	// registered before scene traversal...
	LightInfoList sgRegisteredGlobalLights;
	// registered per object...
	LightInfoList sgRegisteredLocalLights;

	LightInfoList sgCameraBaseLights;

	// best lights per object...
	LightInfoList sgBestLights;
	void sgFindBestLights(const Box3F &box, S32 maxlights, const Point3F &viewdir, bool camerabased,bool caseStatic);

	// used in DTS lighting...
	void sgScoreLight(sgLightInfo *light, const Box3F &box, const SphereF &sphere, bool camerabased);

public:
	enum lightingProfileQualityType
	{
		// highest quality - for in-game and final tweaks...
		lpqtProduction = 0,
		// medium quality - for lighting layout...
		lpqtDesign = 1,
		// low quality - for object placement...
		lpqtDraft = 2
	};
	enum sgLightingPropertiesEnum
	{
		sgReceiveSunLightProp,
		sgAdaptiveSelfIlluminationProp,
		sgCustomAmbientLightingProp,
		sgCustomAmbientForSelfIlluminationProp,

		sgPropertyCount
	};
	inline static bool sgGetProperty(U32 prop)
	{
		AssertFatal((prop < sgPropertyCount), "Invalid property type!");
		return sgLightingProperties[prop];
	}
	inline static void sgSetProperty(U32 prop, bool val)
	{
		AssertFatal((prop < sgPropertyCount), "Invalid property type!");
		sgLightingProperties[prop] = val;
	}
	inline static bool sgAllowDiffuseCustomAmbient() {return sgLightingProperties[sgCustomAmbientLightingProp] && (!sgLightingProperties[sgCustomAmbientForSelfIlluminationProp]);}
	inline static bool sgAllowAdaptiveSelfIllumination() {return sgLightingProperties[sgAdaptiveSelfIlluminationProp];}
	inline static bool sgAllowCollectSelfIlluminationColor() {return !sgLightingProperties[sgCustomAmbientLightingProp];}
	inline static bool sgAllowReceiveSunLight() {return sgLightingProperties[sgReceiveSunLightProp] && (!sgAllowDiffuseCustomAmbient());}
private:
	static bool sgLightingProperties[sgPropertyCount];
	static U32 sgLightingProfileQuality;
	static bool sgLightingProfileAllowShadows;
	static LightInfo* sgDefaultLight;
	//static bool sgDetailMaps;
	static bool sgUseDynamicShadows;
	static U32 sgDynamicShadowQuality;
	static bool sgUseDynamicLightingDualOptimization;
	//static bool sgUseDynamicShadowSelfShadowing;
	//static bool sgUseDynamicShadowSelfShadowingOnPS_2_0;

	static bool sgUseSelfIlluminationColor;
	static ColorF sgSelfIlluminationColor;
	//static bool sgDynamicDTSVectorLighting;
	//static bool sgDynamicParticleSystemLighting;
	static bool sgFilterZones;
	static S32 sgZones[2];
	//static S32 sgShadowDetailSize;
	static S32 sgMaxBestLights;
	static bool sgInGUIEditor;

public:
	// user prefs...
	static bool sgUseDynamicRangeLighting;
	static bool sgUseDRLHighDynamicRange;
	static bool sgUseScreenBloom;

	// mission properties...
	static bool sgAllowDynamicRangeLighting;
	static bool sgAllowDRLHighDynamicRange;
	static bool sgAllowDRLBloom;
	static bool sgAllowDRLToneMapping;

//-----------------------------------
	static U32 sgDynamicShadowDetailSize;
	static bool sgMultipleDynamicShadows;
	static bool sgShowCacheStats;

	static F32 sgDRLTarget;
	static F32 sgDRLMax;
	static F32 sgDRLMin;
	static F32 sgDRLMultiplier;
	static F32 sgBloomCutOff;
	static F32 sgBloomAmount;
	static F32 sgBloomSeedAmount;
	static U32 sgAtlasMaxDynamicLights;

	inline static bool sgAllowDynamicShadows() {return sgUseDynamicShadows;}
	inline static U32 sgGetDynamicShadowQuality()
	{
		F32 psversion = GFX->getPixelShaderVersion();
		if((psversion >= 2.0) && (sgDynamicShadowQuality < 1))
			return 0;
		if((psversion >= 2.0) && (sgDynamicShadowQuality < 2))
			return 1;
		return 2;

		//F32 psversion = GFX->getPixelShaderVersion();
		//if((psversion >= 3.0) || ((psversion >= 2.0) && sgUseDynamicShadowSelfShadowingOnPS_2_0))
		//	return sgUseDynamicShadowSelfShadowing;
		//return false;
	}
	inline static bool sgAllowDynamicLightingDualOptimization()
	{
		if(GFX->getPixelShaderVersion() >= 2.0)
			return sgUseDynamicLightingDualOptimization;
		return false;
	}
	inline static bool sgAllowDRLSystem()
	{
		return sgUseDynamicRangeLighting && (GFX->getPixelShaderVersion() >= 2.0) &&
         (sgAllowDynamicRangeLighting || sgAllowDRLBloom) && !sgInGUIEditor && !gEditingMission;
	}
	inline static bool UseScreenBloom()
	{
		return sgUseScreenBloom;
	}
	inline static bool sgAllowFullDynamicRangeLighting() {return sgAllowDRLSystem() && sgAllowDynamicRangeLighting;}
	inline static bool sgAllowFullHighDynamicRangeLighting() {return sgAllowDRLSystem() && sgUseDRLHighDynamicRange && sgAllowDRLHighDynamicRange;}
	inline static bool sgAllowBloom() {return sgAllowDRLSystem() && sgAllowDRLBloom;}
	inline static bool sgAllowToneMapping() {return sgAllowDRLSystem() && sgAllowDRLToneMapping;}
	inline static ColorF sgGetSelfIlluminationColor(ColorF defaultcol)
	{
		if(sgUseSelfIlluminationColor)
			return sgSelfIlluminationColor;
		return defaultcol;
	}
	inline static void sgSetAllowDynamicRangeLighting(bool enable) {sgAllowDynamicRangeLighting = enable;}
	inline static void sgSetAllowHighDynamicRangeLighting(bool enable) {sgAllowDRLHighDynamicRange = enable;}
	inline static void sgSetAllowDRLBloom(bool enable) {sgAllowDRLBloom = enable;}
	inline static void sgSetAllowDRLToneMapping(bool enable) {sgAllowDRLToneMapping = enable;}
	inline static void sgSetBrightnessState(bool data, int Version) 
	{
		sgUseDRLHighDynamicRange = sgAllowDynamicRangeLighting = sgAllowDRLHighDynamicRange = data; 
		if(data)
		{
			sgDRLSurfaceChain::ResetTexture(true);
			if(Version)
			{
				sgAllowDRLBloom = sgAllowDRLToneMapping = false;
				sgUseScreenBloom = true;
			}
			else
			{
				sgAllowDRLBloom = sgAllowDRLToneMapping = true;
				sgUseScreenBloom = false;
			}
		}
		else
		{
			sgAllowDRLBloom = sgAllowDRLToneMapping = sgUseScreenBloom = false;
		}
	}

	//static GFXTexHandle sgDLightMap;
	static bool sgInit();
	//static bool sgAllowDetailMaps();
	inline static bool sgAllowShadows() {return sgLightingProfileAllowShadows;}
	inline static bool sgAllowFullLightMaps() {return (sgLightingProfileQuality == lpqtProduction);}
	inline static U32 sgGetLightMapScale()
	{
		if(sgLightingProfileQuality == lpqtDesign) return 2;
		if(sgLightingProfileQuality == lpqtDraft) return 4;
		return 1;
	}
	static void sgGetFilteredLightColor(ColorF &color, ColorF &ambient, S32 objectzone);
	// adds proper support for self-illumination...
	//static void sgSetAmbientSelfIllumination(LightInfo *lightinfo, F32 *lightColor,
	//		F32 *ambientColor);
	
   void sgSetupZoneLighting(bool enable, SimObject *sobj);

   sgDRLSystem drlSystem;

   virtual void hdrPrepare(const Point2I &offset, const Point2I &extent) {drlSystem.sgPrepSystem(offset, extent);}
   virtual void hdrRender() {drlSystem.sgRenderSystem();}

private:
	static S32 QSORT_CALLBACK sgSortLightsByAddress(const void *, const void *);
	static S32 QSORT_CALLBACK sgSortLightsByScore(const void *, const void *);

   bool getLightingAmbientColor(SceneObject* obj, ColorF * col, sgLightInfo** light);

private:
	//static member
	static GFXStateBlock* mBlendSB;
	//static GFXStateBlock* mAddrClampSB;
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

class sgRelightFilter
{
public:
	static bool sgFilterRelight;
	static bool sgFilterRelightVisible;
	static bool sgFilterRelightByDistance;
	static F32 sgFilterRelightByDistanceRadius;
	static Point3F sgFilterRelightByDistancePosition;
	static void sgInit();
	static bool sgAllowLighting(const Box3F &box, bool forcefilter);
	static void sgRenderAllowedObjects(void *worldeditor);
};

class sgStatistics
{
public:
	static U32 sgInteriorLexelCount;
	static U32 sgInteriorLexelIlluminationCount;
	static U32 sgInteriorLexelIlluminationTime;
   static U32 sgInteriorLexelDiffuseCount;
	static U32 sgInteriorObjectCount;
	static U32 sgInteriorObjectIncludedCount;
	static U32 sgInteriorObjectIlluminationCount;
	static U32 sgInteriorSurfaceIncludedCount;
	static U32 sgInteriorSurfaceIlluminationCount;
	static U32 sgInteriorSurfaceIlluminatedCount;
   static U32 sgInteriorSurfaceSmoothedCount;
   static U32 sgInteriorSurfaceSmoothedLexelCount;
   static U32 sgInteriorSurfaceSetupTime;
   static U32 sgInteriorSurfaceSetupCount;
   static U32 sgInteriorSurfaceMergeTime;
   static U32 sgInteriorSurfaceMergeCount;
	static U32 sgStaticMeshSurfaceOccluderCount;
   static U32 sgStaticMeshBVPTPotentialOccluderCount;
   static U32 sgStaticMeshCastRayCount;
	static U32 sgTerrainLexelCount;
	static U32 sgTerrainLexelTime;

	static void sgClear();
	static void sgPrint();
};


//----------------------------------------------
// performance testing

class elapsedTimeAggregate
{
private:
   U32 time;
	U32 *resultVar;
public:
	elapsedTimeAggregate(U32 &timeresultvar)
	{
		resultVar = &timeresultvar;
		time = Platform::getRealMilliseconds();
	}
	~elapsedTimeAggregate()
	{
		*resultVar += (Platform::getRealMilliseconds() - time);
	}
};

#define SG_PERFORMANCE_TESTING_OUTPUT
#ifdef SG_PERFORMANCE_TESTING_OUTPUT
class elapsedTime
{
private:
	char info[256];
	U32 time;
public:
	elapsedTime(char *infostr)
	{
		dStrcpy(info, sizeof(info), infostr);
		time = Platform::getRealMilliseconds();
	}
	~elapsedTime()
	{
		Con::printf(info, (Platform::getRealMilliseconds() - time));
	}
};
#else
class elapsedTime
{
public:
	elapsedTime(char *infostr) {}
};
#endif

#define SGLM sgLightManager::get()

#endif//_SGLIGHTMANAGER_H_
