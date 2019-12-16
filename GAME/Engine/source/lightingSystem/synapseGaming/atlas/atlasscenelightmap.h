//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _ATLASSCENELIGHTMAP_H_
#define _ATLASSCENELIGHTMAP_H_

#include "atlas/editor/atlasExportInterfaces.h"
#include "atlas/resource/atlasResourceGeomTOC.h"

class LightInfo;
class sgLightingModel;

class AtlasInstanceColorSource : public IAtlasImageImportSource, public IGetSurfaceHelper
{
private:
   AtlasInstance* mAtlas;

   U32 mSourceSize;
   AtlasSurfaceQueryHelper* mSurfaceHelper;
   Vector<AtlasSurfaceQueryHelper*> mCurrHelpers;
   
   Vector<LightInfo*> mLights;   
   sgLightingModel& model;

   bool inRect(const RectF& r, const Point2F& tc);
   void handleLight(LightInfo* light, const Point3F& pos, const Point3F& normal, bool allowdiffuse, bool allowambient, F32& r, F32& g, F32& b);
public:        
   AtlasInstanceColorSource(AtlasInstance* atlas, const U32 sourceSize);
   ~AtlasInstanceColorSource();

   void addLight(LightInfo* light);

   // IAtlasImageImportSource
   virtual void getExtents(int &width, int &height);
   virtual void getColor(int x, int y, unsigned char &r, unsigned char &g, unsigned char &b, unsigned char &a);
   virtual int getNumberChannels();
   virtual void sampleChannels(int x, int y, unsigned char *out);
   
   // IGetSurfaceHelper
   virtual AtlasSurfaceQueryHelper* getSurfaceHelper(const Point2F& tc);
};
#endif
