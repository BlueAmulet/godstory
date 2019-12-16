//-----------------------------------------------------------------------------
// PowerEngine Shader Engine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _LIGHTINGSYSTEM_SHADOWBASE_H_
#define _LIGHTINGSYSTEM_SHADOWBASE_H_

#include "platform/platform.h"
#include "core/fileStream.h"

class TSShapeInstance;

class ShadowBase
{
public:
   virtual ~ShadowBase() {}
   virtual bool shouldRender(F32 camDist) = 0;
   virtual void preRender(F32 camDist) = 0;
   virtual void render(S32 key) = 0;
   virtual U32 getLastRenderTime() = 0;
   virtual void resetShapeInst(TSShapeInstance* shapeInst) {}
   virtual void saveDynamicShadowData(Stream *stream){};
   virtual void setDynamicShadowData(void *,void *){};
   virtual void freeResource(){};
   virtual void setRenderStatus(S32 key){};
   virtual void clearRenderStatus(S32 key){};
};

#endif