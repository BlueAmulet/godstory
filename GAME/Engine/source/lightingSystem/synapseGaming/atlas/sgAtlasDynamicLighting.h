//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "atlas/runtime/atlasClipMapBatcher.h"

//#include "atlas/resource/atlasResourceGeomTOC.h"
//#include "materials/matInstance.h"
//#include "math/mathUtils.h"
//#include "lightingSystem/synapseGaming/sgLightManager.h"
//#include "lightingSystem/synapseGaming/sgLightInfo.h"
class MatInstance;
class AtlasLightingPlugin : public AtlasRenderPlugin
{
private:
   MatInstance* mLightMatInst;
   MatInstance* mLightMaskMatInst;
   bool mMatInit;
   bool mMatOk;

   Vector<AtlasClipMapBatcher::RenderNote*> mRenderNotes;
public:
   AtlasLightingPlugin();
   ~AtlasLightingPlugin();

   virtual void init();
   virtual void queue(AtlasClipMapBatcher::RenderNote* rn, SceneObject* mObject, AtlasResourceGeomStub *args);  
   virtual void render(SceneGraphData& sgData, SceneObject* obj);
private:
	static GFXStateBlock* mSetSB;
public:
	static void initsb();
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
