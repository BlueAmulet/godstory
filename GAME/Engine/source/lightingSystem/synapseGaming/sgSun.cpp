//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "terrain/sun.h"
#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "core/bitStream.h"

class sgSun : public Sun
{
private:
   typedef Sun Parent;
protected:
   sgLightInfo* mSgLight;     // This will be the casted Sun::mLight   
   sgLightInfo* mSgRegisteredLight; // This will be the casted Sun::mRegisteredLight
   bool mLightCastsShadows;
   bool useBloom;
   bool useToneMapping;
   bool useDynamicRangeLighting;   

   bool DRLHighDynamicRange;
   F32 DRLTarget;
   F32 DRLMax;
   F32 DRLMin;
   F32 DRLMultiplier;

   F32 bloomCutOff;
   F32 bloomAmount;
   F32 bloomSeedAmount;

   virtual void conformLight();   
public:
   sgSun();
   
   virtual bool onAdd();
   virtual void onRemove();
   virtual void registerLights(LightManager *lightManager, bool relight);

   U64 packUpdate(NetConnection* conn, U64 mask, BitStream * stream);
   void unpackUpdate(NetConnection* conn, BitStream * stream);

   static void initPersistFields();

   DECLARE_CONOBJECT(sgSun);
};

IMPLEMENT_CO_NETOBJECT_V1(sgSun);

sgSun::sgSun() : Parent()
{
   mLightCastsShadows = true;
   useBloom = false;
   useToneMapping = false;
   useDynamicRangeLighting = false;

   DRLHighDynamicRange = false;
   DRLTarget = 0.5f;
   DRLMax = 1.4f;
   DRLMin = 0.5f;
   DRLMultiplier = 1.1f;

   bloomCutOff = 0.8f;
   bloomAmount = 0.25f;
   bloomSeedAmount = 1.0f;
}

void sgSun::conformLight()
{   
   Parent::conformLight();
   mSgLight = dynamic_cast<sgLightInfo*>(mLight);
   mSgRegisteredLight = dynamic_cast<sgLightInfo*>(mRegisteredLight);
   AssertFatal(mSgLight, "Incorrect lightinfo type, or not initialized!");
   if (!mSgLight)
      return;
   mSgLight->sgCastsShadows = mLightCastsShadows;
}

bool sgSun::onAdd()
{
   if (!Parent::onAdd())
      return false;

   if ((!mSgLight) || (!mSgRegisteredLight))
      return false;

   mSgLight->sgDoubleSidedAmbient = true;
   mSgLight->sgUseNormals = true;
   mSgLight->sgZone[0] = 0;

   return true;
}

void sgSun::onRemove()
{
   if (isClientObject())
   {
      bool lastSun = true;
      bool lastDRL = true;
      bool lastHDRL = true;
      bool lastBloom = true;
      bool lastToneMapping = true;

      for(SimGroupIterator itr(Sim::getRootGroup());  *itr; ++itr)
	   {
		   const char* className = (*itr)->getClassName();
		   if (!dStricmp(className, "sgSun"))
		   {
   			
			   sgSun *sun = dynamic_cast<sgSun*>((*itr));
			   if (sun && sun != this && sun->isClientObject())
            {
				   lastSun = false;
               
               if (sun->useDynamicRangeLighting)
                  lastDRL = false;
               if (sun->DRLHighDynamicRange)
                  lastHDRL = false;
               if (sun->useBloom)
                  lastBloom = false;
               if (sun->useToneMapping)
                  lastToneMapping = false;
            }
		   }
	   }

      // If we are the last sun then be sure to turn out the lights
      if (lastSun)
      {
         sgLightManager::sgSetAllowDynamicRangeLighting(false);
         sgLightManager::sgSetAllowHighDynamicRangeLighting(false);
         sgLightManager::sgSetAllowDRLBloom(false);
         sgLightManager::sgSetAllowDRLToneMapping(false);
      }
      else
      {
         // Turn off anything we used that the others aren't using
         if (lastDRL)
            sgLightManager::sgSetAllowDynamicRangeLighting(false);
         if (lastHDRL)
            sgLightManager::sgSetAllowHighDynamicRangeLighting(false);
         if (lastBloom)
            sgLightManager::sgSetAllowDRLBloom(false);
         if (lastToneMapping)
            sgLightManager::sgSetAllowDRLToneMapping(false);
      }
   }

   // And let the rest of cleanup happen.
   Parent::onRemove();
}

void sgSun::registerLights( LightManager *lightManager, bool relight )
{
   Parent::registerLights(lightManager, relight);
   sgLightManager::sgGetFilteredLightColor(mRegisteredLight->mColor, mRegisteredLight->mAmbient, 0);
   if (relight)
   {
      mSgRegisteredLight->sgCastsShadows = true;
   }
   
   sgLightManager::sgSetAllowDynamicRangeLighting(useDynamicRangeLighting);
   sgLightManager::sgSetAllowHighDynamicRangeLighting(DRLHighDynamicRange);
   sgLightManager::sgSetAllowDRLBloom(useBloom);
   sgLightManager::sgSetAllowDRLToneMapping(useToneMapping);

   sgLightManager::sgDRLTarget = DRLTarget;
   sgLightManager::sgDRLMax = DRLMax;
   sgLightManager::sgDRLMin = DRLMin;
   sgLightManager::sgDRLMultiplier = DRLMultiplier;

   sgLightManager::sgBloomCutOff = bloomCutOff;
   sgLightManager::sgBloomAmount = bloomAmount;
   sgLightManager::sgBloomSeedAmount = bloomSeedAmount; 
}

U64 sgSun::packUpdate(NetConnection* conn, U64 mask, BitStream * stream)
{
   U64 retMask = Parent::packUpdate(conn, mask, stream);
   // BTR:  We end up writing the flag for update twice now, once in the parent and once here.
   // But this is a rarely updated object, so I think it's ok. 
   if(stream->writeFlag(mask & UpdateMask))
   {
      stream->writeFlag(mSgLight->sgCastsShadows);

      stream->write(useBloom);
      stream->write(useToneMapping);
      stream->write(useDynamicRangeLighting);

      stream->write(DRLHighDynamicRange);
      stream->write(DRLTarget);
      stream->write(DRLMax);
      stream->write(DRLMin);
      stream->write(DRLMultiplier);

      stream->write(bloomCutOff);
      stream->write(bloomAmount);
      stream->write(bloomSeedAmount);
   }
   return retMask;
}

void sgSun::unpackUpdate( NetConnection* conn, BitStream * stream )
{
   Parent::unpackUpdate(conn, stream);
   if(stream->readFlag())
   {
      mSgLight->sgCastsShadows = stream->readFlag();

      stream->read(&useBloom);
      stream->read(&useToneMapping);
      stream->read(&useDynamicRangeLighting);

      stream->read(&DRLHighDynamicRange);
      stream->read(&DRLTarget);
      stream->read(&DRLMax);
      stream->read(&DRLMin);
      stream->read(&DRLMultiplier);

      stream->read(&bloomCutOff);
      stream->read(&bloomAmount);
      stream->read(&bloomSeedAmount);
   }
}

void sgSun::initPersistFields()
{
   Parent::initPersistFields();
   addGroup("Misc");	
   addField("castsShadows",	TypeBool,	Offset(mLightCastsShadows,sgSun));
   endGroup("Misc");	

   addGroup("Scene Lighting");
   addField("useBloom", TypeBool, Offset(useBloom, sgSun));
   addField("useToneMapping", TypeBool, Offset(useToneMapping, sgSun));
   addField("useDynamicRangeLighting", TypeBool, Offset(useDynamicRangeLighting, sgSun));

   addField("DRLHighDynamicRange", TypeBool, Offset(DRLHighDynamicRange, sgSun));
   addField("DRLTarget", TypeF32, Offset(DRLTarget, sgSun));
   addField("DRLMax", TypeF32, Offset(DRLMax, sgSun));
   addField("DRLMin", TypeF32, Offset(DRLMin, sgSun));
   addField("DRLMultiplier", TypeF32, Offset(DRLMultiplier, sgSun));

   addField("bloomCutOff", TypeF32, Offset(bloomCutOff, sgSun));
   addField("bloomAmount", TypeF32, Offset(bloomAmount, sgSun));
   addField("bloomSeedAmount", TypeF32, Offset(bloomSeedAmount, sgSun));
   endGroup("Scene Lighting");
}