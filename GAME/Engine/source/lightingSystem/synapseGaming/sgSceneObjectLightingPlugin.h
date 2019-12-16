//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "sceneGraph/sceneObject.h"
#include "lightingSystem/synapseGaming/shadowCaster.h"

class sgLightInfo;

class sgSceneObjectLightingPlugin : public SceneObjectLightingPlugin
{
private:
   Vector<S32> lightIds;
   ShadowCaster mShadowCaster;
   SceneObject* mParentObject;

   void findLightGroup(SceneObject* obj, NetConnection *con);
   void findLights(const char *name, NetConnection *con);   
private:
   static Vector<sgSceneObjectLightingPlugin*> smPluginInstances;
   U32 mListIndex;
public:   
   static void cleanupPluginInstances();

   sgSceneObjectLightingPlugin(SceneObject* parent);
   ~sgSceneObjectLightingPlugin();

   U64  packUpdate(SceneObject* obj, U64 checkMask, NetConnection *conn, U64 mask, BitStream *stream);
   void unpackUpdate(SceneObject* obj, NetConnection *conn, BitStream *stream);        
   void renderShadow(SceneObject* obj, SceneState *state);
   void cleanupShadowCaster() { mShadowCaster.clearAllShadows();}
   void resetShapeInst(TSShapeInstance* shapeInst);

   struct LightingInfo
   {
      LightingInfo();
      ~LightingInfo();

      bool                       mUseInfo;
      bool                       mInit;
      ColorF                     mDefaultColor;
      bool                       mHasLastColor;
      ColorF                     mLastColor;
      U32                        mLastTime;
      Point3F                    mLastPosition;
      sgLightInfo*               mAmbientLight;
   };
   LightingInfo      mLightingInfo; ///< Lighting info for this object   

   void registerLocalLights(NetConnection* connection);
   void saveDynamicShadowData(SceneObject* obj,Stream *stream);
   void setDynamicShadowData(SceneObject* obj, void *,void *);
   void loadResource();
   void freeResource();
};
