//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _BLOBSHADOW_H_
#define _BLOBSHADOW_H_

#include "collision/depthSortList.h"
#include "sceneGraph/sceneObject.h"
#include "ts/tsShapeInstance.h"
#include "lightingSystem/common/shadowBase.h"

class ShapeBase;

class BlobShadow : public ShadowBase
{
   F32 mRadius;
   F32 mInvShadowDistance;
   MatrixF mLightToWorld;
   MatrixF mWorldToLight;

   Vector<DepthSortList::Poly> mPartition;
   Vector<Point3F> mPartitionVerts;
   GFXVertexBufferHandle<GFXVertexPCT> mShadowBuffer;
   GFXPrimitiveBufferHandle mShadowPBuffer;
   size_t mPBTriangleNum;

   static U32 smShadowMask;

   static DepthSortList smDepthSortList;
   static GFXTexHandle smGenericShadowTexture;
   static F32 smGenericRadiusSkew;
   static S32 smGenericShadowDim;

   U32 mLastRenderTime;
   Point3F mLastPos;

   static void collisionCallback(SceneObject*,void *);

private:

   SceneObject* mParentObject;
   ShapeBase* mShapeBase;
   LightInfo* mParentLight;
   TSShapeInstance* mShapeInstance;
   void setLightMatrices(const Point3F & lightDir, const Point3F & pos);

   void buildPartition(const Point3F & p, const Point3F & lightDir, F32 radius, F32 shadowLen);
   void updatePartition();

public:
	S32 mTexCallbackHandle;
	static void texCB( GFXTexCallbackCode code, void *userData );

   BlobShadow(SceneObject* parentobject, LightInfo* light, TSShapeInstance* shapeinstance);
   ~BlobShadow();

   void setRadius(F32 radius);
   void setRadius(TSShapeInstance *, const Point3F & scale);

   bool prepare(const Point3F & pos, Point3F lightDir, F32 shadowLen);

   bool shouldRender(F32 camDist);
   void preRender(F32 camDist);
   void render(S32);
   U32 getLastRenderTime() { return mLastRenderTime; }
   void setRenderStatus(S32);
   void clearRenderStatus(S32);

   void freeResource();

   static void generateGenericShadowBitmap(S32 dim);
   static void deleteGenericShadowBitmap();

private:
	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mSetZBiasSB;
	static GFXStateBlock* mClearZBiasSB;
public:
	static void init();
	static void shutdown();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};

#endif