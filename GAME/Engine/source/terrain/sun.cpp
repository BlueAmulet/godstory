//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gfx/gBitmap.h"
#include "math/mathIO.h"
#include "core/bitStream.h"
#include "console/consoleTypes.h"
#include "sceneGraph/sceneGraph.h"
#include "terrain/terrData.h"
#include "math/mathUtils.h"
#include "terrain/sun.h"
#include "sceneGraph/lightInfo.h"
#include "sceneGraph/lightManager.h"

IMPLEMENT_CO_NETOBJECT_V1(Sun);

DECLARE_SERIALIBLE(Sun);
//-----------------------------------------------------------------------------

Sun::Sun()
{
   mNetFlags.set(Ghostable | ScopeAlways);
   mTypeMask = EnvironmentObjectType;

   // Light properties
   mLightColor.set(0.7f, 0.7f, 0.7f);
   mLightAmbient.set(0.3f, 0.3f, 0.3f);
   mLightObjAmbient.set(0.3f, 0.3f, 0.3f);
	mDynShadowMod = 0.5;
   
   mSunAzimuth = 0.0f;
   mSunElevation = 35.0f;
   mLight = NULL;
   mRegisteredLight = NULL;

   mReceiveLMLighting = false;
   mBackColor.set(0.0f, 0.0f, 0.0f);
   mColorScale = 1.0f;
	mTerrColorScale = 1.0f;
}

Sun::~Sun()
{
   SAFE_DELETE(mLight);
   SAFE_DELETE(mRegisteredLight);
}

void Sun::createLightIfNeeded()
{
   if (!mLight)
   {
      LightManager* lm = gClientSceneGraph->getLightManager();
      mLight = lm->createLightInfo();
      mRegisteredLight = lm->createLightInfo();

      mLight->mType = LightInfo::Vector;   
      mLight->mDirection.set(0.f, 0.707f, -0.707f);
      conformLight();
   }
}

//-----------------------------------------------------------------------------

void Sun::conformLight()
{
   // Update light properties from field properties
   mLight->mColor = mLightColor * mColorScale;
   mLight->mAmbient = mLightAmbient * mColorScale;
   mLight->mBackColor = mBackColor * mColorScale;
   mLight->mReceiveLMLighting = mReceiveLMLighting;
   mLight->mObjAmbient = mLightObjAmbient * mColorScale;

   // Normalize
   mLight->mDirection.normalize();
}

void Sun::processLight()
{
#ifdef NTJ_CLIENT
	if(mDirty)
	{
		const F32 cColorStep = 0.05f;              // amount to add per 100ms
		S32 time = Platform::getVirtualMilliseconds();

		F32 step = (F32(time - mLastTime) / 1000.f) * cColorStep;
		if(step==0)
			return;

        ColorF color = mLightColor * mColorScale;
        ColorF ambient = mLightAmbient * mColorScale;
        ColorF backColor = mBackColor * mColorScale;
		bool ret1 = MathUtils::interpolateColor((F32 *)&mLight->mColor,(F32 *)&color,step);
		bool ret2 = MathUtils::interpolateColor((F32 *)&mLight->mAmbient,(F32 *)&ambient,step);
        bool ret4 = MathUtils::interpolateColor((F32 *)&mLight->mBackColor,(F32 *)&backColor,step);

		if(!ret1 && !ret2 && !ret4)
			mDirty = false;

		gClientSceneGraph->refreshTerrainLight();
	}
#endif
}

//-----------------------------------------------------------------------------

bool Sun::onAdd()
{
   if(!Parent::onAdd())
      return(false);

   createLightIfNeeded();

   if(isScopeLocal())
   {
	   //Ray: 本地预载的物件只有在这里有机会计算
	   // Calculate Light Direction.
	   F32 Yaw = mDegToRad(mClampF(mSunAzimuth,0,359));
	   F32 Pitch = mDegToRad(mClampF(mSunElevation,-360,+360));
	   VectorF sunvec;
	   MathUtils::getVectorFromAngles(sunvec, Yaw, Pitch);
	   mLight->mDirection = -sunvec;
   }

   // Used to copy mLightColor, etc to light
   conformLight();

   if(isClientObject())
   {
      Sim::getLightSet()->addObject(this);
	  gClientSceneGraph->registSunObject(this);
   }

   return(true);
}

void Sun::onRemove()
{
   Parent::onRemove();
   SAFE_DELETE(mLight);   
   SAFE_DELETE(mRegisteredLight);
}

void Sun::registerLights(LightManager *lightManager, bool relight)
{
   mRegisteredLight->set(mLight);   

	if(relight)
	{
		// static lighting not affected by this option when using the sun...		
		lightManager->registerGlobalLight(mRegisteredLight, this, true);
	}	
   // Always do this now, allows the lighting system to manipulate the sun light if needed.
	lightManager->setSpecialLight(LightManager::slSunLightType, mRegisteredLight);
}

//-----------------------------------------------------------------------------

void Sun::inspectPostApply()
{
   conformLight();
   setMaskBits(UpdateMask);
}

void Sun::OnSerialize( CTinyStream& stream )
{
	__super::OnSerialize( stream );

	stream << mLightColor;
	stream << mLightAmbient;
	stream << mLightObjAmbient;
	stream << mDynShadowMod;
	stream << mReceiveLMLighting;
	stream << mBackColor;
	stream << mColorScale;
	stream << mTerrColorScale;
	stream << mSunAzimuth;
	stream << mSunElevation;
}

void Sun::OnUnserialize( CTinyStream& stream )
{
	__super::OnUnserialize( stream );

	stream >> mLightColor;
	stream >> mLightAmbient;
	stream >> mLightObjAmbient;
	stream >> mDynShadowMod;
	stream >> mReceiveLMLighting;
	stream >> mBackColor;
	stream >> mColorScale;
	stream >> mTerrColorScale;
	stream >> mSunAzimuth;
	stream >> mSunElevation;
}

ConsoleMethod(Sun, apply, void, 2, 2, "")
{
   object->inspectPostApply();
}

void Sun::unpackUpdate(NetConnection *, BitStream * stream)
{
   createLightIfNeeded();
   if(stream->readFlag())
   {
      // direction -> color -> ambient
      mathRead(*stream, &mLight->mDirection);

      stream->read(&mLightColor.red);
      stream->read(&mLightColor.green);
      stream->read(&mLightColor.blue);
      stream->read(&mLightColor.alpha);

      stream->read(&mLightAmbient.red);
      stream->read(&mLightAmbient.green);
      stream->read(&mLightAmbient.blue);
      stream->read(&mLightAmbient.alpha);

	  stream->read(&mLightObjAmbient.red);
	  stream->read(&mLightObjAmbient.green);
	  stream->read(&mLightObjAmbient.blue);
	  stream->read(&mLightObjAmbient.alpha);
	  
	  stream->read(&mDynShadowMod);

      stream->read(&mReceiveLMLighting);
      stream->read(&mBackColor);
      stream->read(&mColorScale);
		stream->read(&mTerrColorScale);
   }
   conformLight();
}

U64 Sun::packUpdate(NetConnection *, U64 mask, BitStream * stream)
{
   if(stream->writeFlag(mask & UpdateMask))
   {
		// Calculate Light Direction.
		F32 Yaw = mDegToRad(mClampF(mSunAzimuth,0,359));
		F32 Pitch = mDegToRad(mClampF(mSunElevation,-360,+360));
		VectorF sunvec;
		MathUtils::getVectorFromAngles(sunvec, Yaw, Pitch);
		mLight->mDirection = -sunvec;

      // direction -> color -> ambient
      mathWrite(*stream, mLight->mDirection);

      stream->write(mLightColor.red);
      stream->write(mLightColor.green);
      stream->write(mLightColor.blue);
      stream->write(mLightColor.alpha);

      stream->write(mLightAmbient.red);
      stream->write(mLightAmbient.green);
      stream->write(mLightAmbient.blue);
      stream->write(mLightAmbient.alpha);

	  stream->write(mLightObjAmbient.red);
	  stream->write(mLightObjAmbient.green);
	  stream->write(mLightObjAmbient.blue);
	  stream->write(mLightObjAmbient.alpha);

	  stream->write(mDynShadowMod);

	  stream->write(mReceiveLMLighting);
      stream->write(mBackColor);
      stream->write(mColorScale);
		stream->write(mTerrColorScale);
   }
   return(0);
}

//-----------------------------------------------------------------------------

void Sun::initPersistFields()
{
   Parent::initPersistFields();
   addGroup("Misc");	
   addField("azimuth",     TypeF32,       Offset( mSunAzimuth, Sun));
   addField("elevation",   TypeF32,       Offset( mSunElevation, Sun));
   //addField("direction", TypePoint3F,   Offset(mLight->mDirection, Sun));
   addField("frontColor",       TypeColorF,    Offset(mLightColor, Sun));
   addField("backColor",		TypeColorF,    Offset(mBackColor, Sun));
   addField("staticBaseColor",  TypeColorF,    Offset(mLightAmbient, Sun));
   addField("npcBaseColor",     TypeColorF,    Offset(mLightObjAmbient, Sun));
	addField("DynShadowMod",		TypeF32,    Offset(mDynShadowMod, Sun));
   addField("colorScale",		TypeF32,	   Offset(mColorScale, Sun));
	addField("TerrColorScale",		TypeF32,	   Offset(mTerrColorScale, Sun));
   addField("npcLMLighting",     TypeBool,    Offset(mReceiveLMLighting, Sun));
   endGroup("Misc");	
}