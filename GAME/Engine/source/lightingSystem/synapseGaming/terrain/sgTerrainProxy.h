//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _SCENELIGHTING_H_
#include "lightingSystem/common/sceneLighting.h"
#endif

#ifndef _BITVECTOR_H_
#include "core/bitVector.h"
#endif

#ifndef _TERRDATA_H_
#include "terrain/terrData.h"
#endif

class ShadowVolumeBSP;
class TerrainBlock;
class GBitmap;
class sgLightInfo;

struct TerrainChunk : public PersistInfo::PersistChunk
{
   typedef PersistInfo::PersistChunk Parent;

   TerrainChunk();
   ~TerrainChunk();

	GBitmap *mShine;
	GBitmap *bakedBmp;
   bool read(Stream &);
   bool write(Stream &);
};

class TerrainProxy : public SceneLighting::ObjectProxy
{
private:
   typedef  ObjectProxy    Parent;

   BitVector               mShadowMask;
   ShadowVolumeBSP *       mShadowVolume;
   ColorF *                mLightmap;


   ColorF *sgBakedLightmap;
   Vector<sgLightInfo *> sgLights;
   void sgAddUniversalPoint(sgLightInfo *light);
   void sgLightUniversalPoint(LightInfo *light, TerrainBlock * terrain);
   bool sgMarkStaticShadow(void *terrainproxy, SceneObject *sceneobject, LightInfo *light);
   void postLight(bool lastLight);
   
   void lightVector(LightInfo *);

   struct SquareStackNode
   {
      U8          mLevel;
      U16         mClipFlags;
      Point2I     mPos;
   };

   S32 testSquare(const Point3F &, const Point3F &, S32, F32, const Vector<PlaneF> &);
   bool markObjectShadow(ObjectProxy *);
   bool sgIsCorrectStaticObjectType(SceneObject *obj);
   bool checkShadowed(const Point2F& lmPos, F32 lexelDim, const Point2F& worldOffset, const LightInfo* light, const Point3F& normal);
public:

   TerrainProxy(SceneObject * obj);
   ~TerrainProxy();
   TerrainBlock * operator->() {return(static_cast<TerrainBlock*>(static_cast<SceneObject*>(mObj)));}
   TerrainBlock * getObject() {return(static_cast<TerrainBlock*>(static_cast<SceneObject*>(mObj)));}

   bool getShadowedSquares(const Vector<PlaneF> &, Vector<U16> &);

   // lighting
   void init();
   bool preLight(LightInfo *);
   void light(LightInfo *);

   // persist
   U32 getResourceCRC();
   bool setPersistInfo(PersistInfo::PersistChunk *);
   bool getPersistInfo(PersistInfo::PersistChunk *);

   // events
   virtual void processTGELightProcessEvent(U32 curr, U32 max, LightInfo* currlight); 
   virtual void processSGObjectProcessEvent(LightInfo* currLight);
};



