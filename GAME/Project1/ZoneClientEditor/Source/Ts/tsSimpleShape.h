//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _TSSIMPLESHAPE_H_
#define _TSSIMPLESHAPE_H_

#ifndef _RENDERABLESCENEOBJECT_H
#include "sceneGraph/RenderableSceneObject.h"
#endif
#ifndef _RESMANAGER_H_
#include "core/resManager.h"
#endif

#include "gfx/gfxTextureHandle.h"

class TSShape;
class TSShapeInstance;
class TSThread;
class GBitmap;

//客户端的一个简单模型显示，不是服务端的对象


//--------------------------------------------------------------------------
class TSSimpleShape : public RenderableSceneObject
{
   typedef RenderableSceneObject Parent;
   
protected:
   bool onAdd();
   void onRemove();

protected:

   StringTableEntry  mShapeName;
   Resource<TSShape> mShape;
   TSShapeInstance*  mShapeInstance;
   
   TSThread * mAmbientAnimation;
   U32  mLastAnimateRenderTime;
   U32	mLastRenderTime;
   F32	mLastRenderPos;
   S32	mInvisibleCycle;

public:
  void setTransform     ( const MatrixF &mat);
  void setAnimationPos	(F32 pos);
  F32  setInvisibleCycle(S32 cycle);

   // Rendering
protected:
   bool prepRenderImage  ( SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState=false);
   void renderObject     ( SceneState *state);

   void checkCycle       (F32 dur);
public:
   TSSimpleShape();
   ~TSSimpleShape();

#ifdef USE_MULTITHREAD_ANIMATE
   void deleteObject();
#endif

   DECLARE_CONOBJECT(TSSimpleShape);

   virtual bool loadRenderResource();
   virtual void freeRenderResource();

   void setShapeName(StringTableEntry shapeName) {mShapeName = shapeName;}
   TSShapeInstance* getShapeInstance(){return mShapeInstance;}
   void getNodeMatrix(StringTableEntry nodeName,MatrixF* mat);
   void getRenderNodeTransform(StringTableEntry nodeName,MatrixF* mat);
};

#endif // _H_TSSTATIC

