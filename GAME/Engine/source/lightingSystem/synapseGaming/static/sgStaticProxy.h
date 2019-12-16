
#ifndef _SCENELIGHTING_H_
#include "lightingSystem/common/sceneLighting.h"
#endif

#ifndef _BITVECTOR_H_
#include "core/bitVector.h"
#endif


class GBitmap;
class sgLightInfo;
class TSStatic;
class CLightmap;

struct StaticChunk : public PersistInfo::PersistChunk
{
   typedef PersistInfo::PersistChunk Parent;

   StaticChunk();
   ~StaticChunk();

   GBitmap *mLightmap;

   bool read(Stream &);
   bool write(Stream &);
};

class StaticProxy : public SceneLighting::ObjectProxy
{
private:
   typedef  ObjectProxy    Parent;

   bool					   mHaveLightmap;
   ColorF *				   mLightmap;
   ColorF *				   sgBakedLightmap;

   CLightmap*			   mLightMapObj;
   U32					   mLightMapWidth;
   U32					   mLightMapHeight;

   Vector<sgLightInfo *> sgLights;
   void sgAddUniversalPoint(sgLightInfo *light);
   void postLight(bool lastLight);

public:

   StaticProxy(SceneObject * obj);
   ~StaticProxy();

   TSStatic* getObject() {return(TSStatic*)(static_cast<SceneObject*>(mObj));}

   // lighting
   void init();
   bool preLight(LightInfo *);
   void light(LightInfo *);

   void BakedLightmapping(LightInfo * light, ColorF *pBuffer);
   void lightVector(LightInfo* light, ColorF *pBuffer);

   // persist
   U32 getResourceCRC();
   bool setPersistInfo(PersistInfo::PersistChunk *);
   bool getPersistInfo(PersistInfo::PersistChunk *);

   // events
   virtual void processTGELightProcessEvent(U32 curr, U32 max, LightInfo* currlight); 
   virtual void processSGObjectProcessEvent(LightInfo* currLight);
};



