//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "lightingSystem/synapseGaming/sgSceneObjectLightingPlugin.h"
#include "util/journal/process.h"
#include "gfx/gfxDevice.h"
#include "sim/netConnection.h"
#include "core/bitStream.h"
#include "ts/tsMesh.h"
#include "T3D/shapeBase.h"
#include "lightingSystem/synapseGaming/sgLighting.h"
#include "lightingSystem/synapseGaming/sgLightInfo.h"
#include "lightingSystem/synapseGaming/sgLightObject.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"

Vector<sgSceneObjectLightingPlugin*> sgSceneObjectLightingPlugin::smPluginInstances;

sgSceneObjectLightingPlugin::LightingInfo::LightingInfo()
{
   mUseInfo = false;
   mInit = false;
   mHasLastColor = false;

   // set the colors to half white for invalid surfaces and all...
   mDefaultColor.set(0.5f, 0.5f, 0.5f);
   //mAlarmColor.set(0.5f, 0.5f, 0.5f);
   mLastColor.set(0.5f, 0.5f, 0.5f);

   mLastTime = 0;
   mLastPosition.set(0.0f, 0.0f, 0.0f);
   mAmbientLight = SGLM->createSGLightInfo();   
}

sgSceneObjectLightingPlugin::LightingInfo::~LightingInfo()
{
   SAFE_DELETE(mAmbientLight);
}

sgSceneObjectLightingPlugin::sgSceneObjectLightingPlugin(SceneObject* parent)
{
   smPluginInstances.push_back(this);
   mListIndex = smPluginInstances.size() - 1;
   mParentObject = parent;
}

sgSceneObjectLightingPlugin::~sgSceneObjectLightingPlugin()
{
   smPluginInstances[mListIndex] = NULL;
   mParentObject->setLightingPlugin(NULL);
}

void sgSceneObjectLightingPlugin::cleanupPluginInstances()
{
   for (U32 i = 0; i < smPluginInstances.size(); i++)
   {
      if (smPluginInstances[i])
         SAFE_DELETE(smPluginInstances[i])
   }
}

void sgSceneObjectLightingPlugin::findLights(const char *name, NetConnection *con)
{
   SimObject *object = Sim::findObject(name);
   if(!object)
      return;
   SimGroup *group = dynamic_cast<SimGroup *>(object);
   if(!group)
   {
      sgLightObject *light = dynamic_cast<sgLightObject*>(object);
      if(!light)
         return;

      // its a light object...
      S32 id = con->getGhostIndex(light);
      if(id > -1)
         lightIds.push_back(id);
   }
   else
   {
      // its a group object so get the contained light objects...
      for(SimObject **obj = group->begin(); obj != group->end(); obj++)
      {
         sgLightObject *light = dynamic_cast<sgLightObject*>(*obj);
         if(!light)
            continue;
         S32 id = con->getGhostIndex(light);
         if(id > -1)
            lightIds.push_back(id);
      }
   }
}

/// converts lightGroupName into a list of sgUniversalStaticLight objects.
void sgSceneObjectLightingPlugin::findLightGroup(SceneObject* obj, NetConnection *con)
{
   AssertFatal((obj->isServerObject()), "Client object called 'findClientLightGroup'.");

   lightIds.clear();

   if(obj->lightGroupName && (dStrlen(obj->lightGroupName) > 0))
   {
	   int length = dStrlen(obj->lightGroupName) + 1;
      char *lightname = new char[length];
      dStrcpy(lightname, length, obj->lightGroupName);
      char *currentname = lightname;
      char *delimiter = NULL;

      while((delimiter = dStrchr(currentname, ';')) != NULL)
      {
         delimiter[0] = 0;

         findLights(currentname, con);

         currentname = &delimiter[1];
      }

      findLights(currentname, con);

      delete[] lightname;
   }
}

U64 sgSceneObjectLightingPlugin::packUpdate(SceneObject* obj, U64 checkMask, NetConnection *conn, U64 mask, BitStream *stream)
{
   // these fields are static, so try to avoid transmit
   if(stream->writeFlag(mask & checkMask))
   {
      stream->writeFlag(obj->receiveSunLight);
      stream->writeFlag(obj->useAdaptiveSelfIllumination);
      stream->writeFlag(obj->useCustomAmbientLighting);
      stream->writeFlag(obj->customAmbientForSelfIllumination);
      stream->write(obj->customAmbientLighting);
      stream->writeFlag(obj->receiveLMLighting);
      if(obj->isServerObject())
      {
         lightIds.clear();
         findLightGroup(obj, conn);
      }

      U64 maxcount = getMin(lightIds.size(), SG_TSSTATIC_MAX_LIGHTS);
      stream->writeInt(maxcount, SG_TSSTATIC_MAX_LIGHT_SHIFT);
      for(U32 i=0; i<maxcount; i++)
      {
         stream->writeInt(lightIds[i], NetConnection::GhostIdBitSize);
      }      
   }
   return 0;
}

void sgSceneObjectLightingPlugin::unpackUpdate(SceneObject* obj, NetConnection *conn, BitStream *stream)
{
   if(stream->readFlag())
   {
      obj->receiveSunLight = stream->readFlag();
      obj->useAdaptiveSelfIllumination = stream->readFlag();
      obj->useCustomAmbientLighting = stream->readFlag();
      obj->customAmbientForSelfIllumination = stream->readFlag();
      stream->read(&obj->customAmbientLighting);
      obj->receiveLMLighting = stream->readFlag();

      U32 count = stream->readInt(SG_TSSTATIC_MAX_LIGHT_SHIFT);
      lightIds.clear();
      for(U32 i=0; i<count; i++)
      {
         S32 id = stream->readInt(NetConnection::GhostIdBitSize);
         lightIds.push_back(id);
      }
   }
}

void sgSceneObjectLightingPlugin::renderShadow(SceneObject* obj, SceneState *state)
{
#ifdef POWER_OS_XENON
	return;
#endif

   // We only support ShapeBase right now.  In the future, more stuff!
   RenderableSceneObject* sb = dynamic_cast<RenderableSceneObject*>(obj);
   if (sb)
   {
      // hack until new scenegraph in place
      GFX->pushWorldMatrix();
      //MatrixF world = GFX->getWorldMatrix();
      //TSMesh::setCamTrans(world);
      //TSMesh::setSceneState(state);
      //TSMesh::setCubemap(sb->getDynamicCubemap());
      //TSMesh::setGlow(false);
      //TSMesh::setRefract(false);

      Point3F cam = state->getCameraPosition() - obj->getRenderPosition();
      mShadowCaster.render(obj, sb->getShapeInstance(), cam.len());

      GFX->popWorldMatrix();
   }
}

void sgSceneObjectLightingPlugin::resetShapeInst(TSShapeInstance* shapeInst)
{
	mShadowCaster.resetShapeInst(shapeInst);
}

void sgSceneObjectLightingPlugin::registerLocalLights(NetConnection* connection)
{
   for(U32 i=0; i<lightIds.size(); i++)
   {
      SimObject *sim = connection->resolveGhost(lightIds[i]);
      if(!sim)
         continue;

      sgLightObject *light = dynamic_cast<sgLightObject*>(sim);
      if(!light)
         continue;

      sgLightObjectData *data = static_cast<sgLightObjectData *>(light->getDataBlock());
      if((data) && (data->sgStatic) && (data->mLightOn) && (light->mEnable))
      {
         light->mLight->sgAssignedToTSObject = true;
         SGLM->registerLocalLight(light->mLight);
      }
   }
}

class sgSceneObjectPluginFactory
{
protected:
   void activate(bool enable);
   void addLightPlugin(SceneObject* obj);
   void addToExistingObjects();
public:
   sgSceneObjectPluginFactory();
};

sgSceneObjectPluginFactory p_sgSceneObjectPluginFactory;

sgSceneObjectPluginFactory::sgSceneObjectPluginFactory()
{
   SGLM->mSgActivate.notify(this, &sgSceneObjectPluginFactory::activate);
}

void sgSceneObjectPluginFactory::activate(bool enable)
{
   if (enable)
   {
      SceneObject::smSceneObjectAdd.notify(this, &sgSceneObjectPluginFactory::addLightPlugin);
      addToExistingObjects();
   } else {
      SceneObject::smSceneObjectAdd.remove(this, &sgSceneObjectPluginFactory::addLightPlugin);
      sgSceneObjectLightingPlugin::cleanupPluginInstances();
   }
}

void sgSceneObjectPluginFactory::addLightPlugin(SceneObject* obj)
{
   AssertFatal( obj->getLightingPlugin() == NULL, "Memory leak here." );

   obj->setLightingPlugin(new sgSceneObjectLightingPlugin(obj));
}

// Some objects may not get cleaned up during mission load/free, so add our
// plugin to existing scene objects
void sgSceneObjectPluginFactory::addToExistingObjects()
{
   Vector<SceneObject *> objects;
   gClientContainer.findObjects(0xFFFFFFFF, sgFindObjectsCallback, &objects);
   for (SceneObject** i = objects.begin(); i != objects.end(); i++)
      addLightPlugin(*i);
}

void sgSceneObjectLightingPlugin::saveDynamicShadowData(SceneObject* obj, Stream *stream)
{
	RenderableSceneObject* sb = dynamic_cast<RenderableSceneObject*>(obj);
	mShadowCaster.saveDynamicShadowData(obj, sb->getShapeInstance(),stream);
}

void sgSceneObjectLightingPlugin::setDynamicShadowData(SceneObject* obj,void *p1,void *p2)
{
	RenderableSceneObject* sb = dynamic_cast<RenderableSceneObject*>(obj);
	mShadowCaster.setDynamicShadowData(obj, sb->getShapeInstance(), p1,p2);
}

void sgSceneObjectLightingPlugin::loadResource()
{
}

void sgSceneObjectLightingPlugin::freeResource()
{
	mShadowCaster.freeResource();
}



