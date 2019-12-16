//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "sceneGraph/sceneObject.h"
#include "lightingSystem/basicLighting/basicLightManager.h"
#include "lightingSystem/common/blobShadow.h"
#include "T3D/shapeBase.h"
#include "gfx/gfxDevice.h"
#include "gfx/gfxTransformSaver.h"

class BasicSceneObjectLightingPlugin : public SceneObjectLightingPlugin
{
private:
   ShadowBase* shadow;
   SceneObject* mParentObject;
   U32 mListIndex;
   static Vector<BasicSceneObjectLightingPlugin*> smPluginInstances;
public:
   BasicSceneObjectLightingPlugin(SceneObject* parent);
   ~BasicSceneObjectLightingPlugin();

   static void cleanupPluginInstances();

   // Called by Shapebase and descendents 
   virtual void renderShadow(SceneObject* obj, SceneState *state);
   // Called by statics
   virtual U64  packUpdate(SceneObject* obj, U64 checkMask, NetConnection *conn, U64 mask, BitStream *stream) { return 0; }
   virtual void unpackUpdate(SceneObject* obj, NetConnection *conn, BitStream *stream) { }
};

Vector<BasicSceneObjectLightingPlugin*> BasicSceneObjectLightingPlugin::smPluginInstances;

BasicSceneObjectLightingPlugin::BasicSceneObjectLightingPlugin(SceneObject* parent)
{
   shadow = NULL;
   smPluginInstances.push_back(this);
   mListIndex = smPluginInstances.size() - 1;
   mParentObject = parent;
}

BasicSceneObjectLightingPlugin::~BasicSceneObjectLightingPlugin()
{
   SAFE_DELETE(shadow);
   smPluginInstances[mListIndex] = NULL;
   mParentObject->setLightingPlugin(NULL);
}

void BasicSceneObjectLightingPlugin::cleanupPluginInstances()
{
   for (U32 i = 0; i < smPluginInstances.size(); i++)
   {
      if (smPluginInstances[i])
         SAFE_DELETE(smPluginInstances[i])
   }

   BlobShadow::deleteGenericShadowBitmap();
}


void BasicSceneObjectLightingPlugin::renderShadow( SceneObject* obj, SceneState *state)
{
   ShapeBase* sb = dynamic_cast<ShapeBase*>(obj);
   if (!sb)
      return;
   if (!shadow)
   {
      shadow = new BlobShadow(obj, BLM->getSpecialLight(LightManager::slSunLightType), sb->getShapeInstance());
   }
   
   // hack until new scenegraph in place
   GFXTransformSaver ts;
   
   MatrixF world = GFX->getWorldMatrix();
   TSMesh::setCamTrans(world);
   TSMesh::setSceneState(state);
   TSMesh::setCubemap(sb->mDynamicCubemap);
   TSMesh::setGlow(false);
   //TSMesh::setRefract(false);

   F32 camDist = (state->getCameraPosition() - obj->getRenderPosition()).len();
   
   // Make sure the shadow wants to be rendered
   if(shadow->shouldRender(camDist))
   {
      // Render! (and note the time)      
      shadow->preRender(camDist);
   }      
}

class BasicSceneObjectPluginFactory
{
protected:
   void activate(bool enable);
   void addLightPlugin(SceneObject* obj);
   void addToExistingObjects();
public:
   BasicSceneObjectPluginFactory();
};

BasicSceneObjectPluginFactory p_BasicSceneObjectPluginFactory;

BasicSceneObjectPluginFactory::BasicSceneObjectPluginFactory()
{
   BLM->mActivate.notify(this, &BasicSceneObjectPluginFactory::activate);
}

void BasicSceneObjectPluginFactory::activate(bool enable)
{
   if (enable)
   {
      SceneObject::smSceneObjectAdd.notify(this, &BasicSceneObjectPluginFactory::addLightPlugin);
      addToExistingObjects();
   } else {
      SceneObject::smSceneObjectAdd.remove(this, &BasicSceneObjectPluginFactory::addLightPlugin);
      BasicSceneObjectLightingPlugin::cleanupPluginInstances();
   }
}

void BasicSceneObjectPluginFactory::addLightPlugin(SceneObject* obj)
{
   obj->setLightingPlugin(new BasicSceneObjectLightingPlugin(obj));
}

// Some objects may not get cleaned up during mission load/free, so add our
// plugin to existing scene objects
void BasicSceneObjectPluginFactory::addToExistingObjects()
{
   SimpleQueryList sql;  
   gClientContainer.findObjects(0xFFFFFFFF, SimpleQueryList::insertionCallback, &sql);
   for (SceneObject** i = sql.mList.begin(); i != sql.mList.end(); i++)
      addLightPlugin(*i);
}
                                                                                                         