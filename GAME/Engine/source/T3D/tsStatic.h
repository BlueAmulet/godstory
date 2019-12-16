//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _TSSTATIC_H_
#define _TSSTATIC_H_

#ifndef _RENDERABLESCENEOBJECT_H
#include "sceneGraph/RenderableSceneObject.h"
#endif
#ifndef _CONVEX_H_
#include "collision/convex.h"
#endif
#ifndef _RESMANAGER_H_
#include "core/resManager.h"
#endif

#include "gfx/gfxTextureHandle.h"

class TSShape;
class TSShapeInstance;
class TSThread;
class TSStatic;
class GBitmap;
class GFXStateBlock;

//--------------------------------------------------------------------------
class TSStaticConvex : public Convex
{
   typedef Convex Parent;
   friend class TSStatic;

  protected:
   TSStatic* pStatic;
   MatrixF*  nodeTransform;

  public:
   U32       hullId;
   Box3F     box;

  public:
   TSStaticConvex() { mType = TSStaticConvexType; nodeTransform = 0; }
   TSStaticConvex(const TSStaticConvex& cv) {
      mType      = TSStaticConvexType;
      mObject    = cv.mObject;
      pStatic    = cv.pStatic;
      nodeTransform = cv.nodeTransform;
      hullId     = cv.hullId;
      box        = box;
   }

   void findNodeTransform();
   const MatrixF& getTransform() const;
   Box3F getBoundingBox() const;
   Box3F getBoundingBox(const MatrixF& mat, const Point3F& scale) const;
   Point3F      support(const VectorF& v) const;
   void         getFeatures(const MatrixF& mat,const VectorF& n, ConvexFeature* cf);
   void         getPolyList(AbstractPolyList* list);
};

class TSStaticPolysoupConvex : public Convex
{
   typedef Convex Parent;
   friend class TSMesh;

public:
   TSStaticPolysoupConvex();
   ~TSStaticPolysoupConvex() {};

public:
   Box3F                box;
   Point3F              verts[4];
   PlaneF               normal;
   S32                  idx;
   TSMesh               *mesh;

   static SceneObject* smCurObject;

public:

   // Returns the bounding box in world coordinates
   Box3F getBoundingBox() const;
   Box3F getBoundingBox(const MatrixF& mat, const Point3F& scale) const;

   void getFeatures(const MatrixF& mat,const VectorF& n, ConvexFeature* cf);

   // This returns a list of convex faces to collide against
   void getPolyList(AbstractPolyList* list);

   // This returns the furthest point from the input vector
   Point3F support(const VectorF& v) const;
};

//--------------------------------------------------------------------------
class TSStatic : public RenderableSceneObject
{
   typedef RenderableSceneObject Parent;
   friend class TSStaticConvex;
   // test
   friend class dGuiMouseGamePlay;

   static U32 smUniqueIdentifier;

   enum Constants {
      MaxCollisionShapes = 8
   };

   enum MaskBits {
	  advancedStaticOptionsMask = Parent::NextFreeMask,
	  NextFreeMask = Parent::NextFreeMask << 1
   };
   
  protected:
   bool onAdd();
   void onRemove();

   // Collision
   bool isFunctionEnabled(U32 type)
   { 
	   if(type==CAST_RAY_FADE_TYPE && (!mAllowTransparency || mIsTransparency) )
		   return false;
	   else if(type == CAST_SHADOW_TYPE && !mAllowStaticShadowOnTerrain)
		   return false;

	   return true;
   }
   void prepCollision();
   bool castShadow(const Point3F &start, const Point3F &end, RayInfo* info);
   bool castRay(const Point3F &start, const Point3F &end, RayInfo* info);
   bool buildPolyList(AbstractPolyList* polyList, const Box3F &box, const SphereF& sphere);
   void buildConvex(const Box3F& box, Convex* convex);
  protected:
   Convex* mConvexList;

   StringTableEntry  mShapeName;
   StringTableEntry  mShapeFullPath;
   bool              mIsLoadFromTpk;

   Resource<TSShape> mShape;
   TSShapeInstance *mShapeInstance;
   
   TSThread * mAmbientAnimation;
   U32  mLastRenderKey;
   U32	mLastAnimateRenderTime;

   bool mAllowStaticShadowOnTerrain;
   bool mAllowDynamicShadowOnTerrain;
   bool mIsTransparency;
   bool mAllowTransparency;
   bool mAllowReceiveShadow;
   bool mAllowPlayerStep;
   bool mAllowReflection;
   S32  mTinyStaticLevel;
   F32  mVisibility;
   static F32  mFadePerMilliSec;

   bool   mAsField;

   Vector<S32>            mCollisionDetails;
   Vector<S32>            mLOSDetails;

   Vector<S32>            mMeshDetails;

   U32	mCrc;
  public:
    GBitmap*			  mLightMap;
	GFXTexHandle		  mLightMapTex;
	Box3F	mShadowBox;
	bool bShadowBoxDirty;

#ifdef USE_MULTITHREAD_ANIMATE
	void deleteObject();
#endif

   // Rendering
  protected:
   bool prepRenderImage  ( SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState=false);
   void renderObject     ( bool bQuery,SceneState *state);
   void prepRenderQuery	 ( SceneState* state);
   void prepRenderShadowQuery();
   void renderQuery		 ();
   void renderShadowQuery();
   void renderShadow	 (SceneState *state);
   void setTransform     ( const MatrixF &mat);

   void PrepVisibility();

  public:
   TSStatic();
   ~TSStatic();

   DECLARE_CONOBJECT(TSStatic);
   static void initPersistFields();

   void OnSerialize( CTinyStream& stream );
   void OnUnserialize( CTinyStream& stream );


	virtual bool loadRenderResource();
   virtual void freeRenderResource();


   U64  packUpdate  (NetConnection *conn, U64 mask, BitStream *stream);
   void unpackUpdate(NetConnection *conn,           BitStream *stream);

   void setShadowVariables(sgShadowProjector *pShadow );
   void renderShadowBuff(sgShadowProjector *pShadow ,MatrixF &proj,MatrixF &lightspace,MatrixF &sgWorldToLightY,MatrixF &shadowscale);
   bool shadowTest(SceneState *);
	void calShadowBox();

   void inspectPostApply();

   bool receiveShadow()  {return mAllowReceiveShadow;}
   bool allowPlayerStep() {return mAllowPlayerStep;}
   void setTransparency(bool flag) {mIsTransparency = flag;}

   U32  getCRC(){return mCrc;}
   void GetLightMapSize(U32 &,U32 &);
   TSShapeInstance* getShapeInstance(){return mShapeInstance;}
   void LightingCompleted();

   // <Edit> [7/14/2009 joy] 可视距离等级
   inline S32 getVisibleDistanceLevel() { return mTinyStaticLevel;}

   bool haveDynamicShadow();
   void saveDynamicShadowData(Stream *stream);
   void setDynamicShadowData(void *,void *);

   StringTableEntry getShapeName(void) {return mShapeName;}
  
private:
	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mClearSB;
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

#endif // _H_TSSTATIC

