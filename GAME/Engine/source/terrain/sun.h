//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SUN_H_
#define _SUN_H_

#ifndef _NETOBJECT_H_
#include "sim/netObject.h"
#endif
#ifndef _COLOR_H_
#include "core/color.h"
#endif

class LightInfo;

/// This Sun object should support different types of lighting systems because of the
/// lighting system createLightInfo factory method.  So other lighting systems should
/// only need to derive from here to add more information to the lightinfo.
class Sun : public NetObject
{
private:
   typedef NetObject Parent;
public:
   ColorF mLightColor;
   ColorF mLightAmbient;
   ColorF mLightObjAmbient;
	F32 mDynShadowMod;//动态影子颜色深度系数

protected:

   LightInfo* mLight;
   LightInfo* mRegisteredLight;

   virtual void createLightIfNeeded();
   virtual void conformLight();   
   
   bool lineColor(F32 * pColor,F32 * pLastColor,F32 step)
   {
	   bool ret = true;

	   for(U32 i = 0; i < 3; i++)
	   {
		   if(pColor[i] > pLastColor[i])
			   pLastColor[i] = mClampF(pLastColor[i] + step, 0.f, pColor[i]);
		   else if(pColor[i] < pLastColor[i])
			   pLastColor[i] = mClampF(pLastColor[i] - step, pColor[i], 1.f);

		   if(pLastColor[i] != pColor[i])
			   ret = false;
	   }

	   return ret;
   }
public:

   Sun();
   ~Sun();

   void OnSerialize( CTinyStream& stream );
   void OnUnserialize( CTinyStream& stream );

   // SimObject
   virtual bool onAdd();
   virtual void onRemove();
   virtual void registerLights(LightManager *lm, bool lightingScene);

   void setAmbientColor(ColorF ambientColor){mLightAmbient=ambientColor; mDirty = true; mLastTime = Platform::getVirtualMilliseconds();}
   void setLightColor(ColorF lightColor){mLightColor=lightColor;mDirty = true; mLastTime = Platform::getVirtualMilliseconds();}
   void processLight();

   //
   void inspectPostApply();

   static void initPersistFields();

   // NetObject
   enum NetMaskBits {
      UpdateMask     = BIT(0)
   };

   U64  packUpdate  (NetConnection *conn, U64 mask, BitStream * stream);
   void unpackUpdate(NetConnection *conn,           BitStream * stream);

   F32		mSunAzimuth;
   F32		mSunElevation;
   S32		mLastTime;
   bool		mDirty;

   bool		mReceiveLMLighting;
   ColorF   mBackColor;
   F32      mColorScale;
	F32		mTerrColorScale;

   DECLARE_CONOBJECT(Sun);
};

#endif
