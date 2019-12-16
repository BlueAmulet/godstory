//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

//Includes
#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _MRECT_H_
#include "math/mRect.h"
#endif
#ifndef _COLOR_H_
#include "core/color.h"
#endif
#ifndef _SCENEOBJECT_H_
#include "sceneGraph/sceneObject.h"
#endif
#ifndef _GFXTEXTUREHANDLE_H_
#include "gfx/gfxTextureHandle.h"
#endif

struct stThreadParam;
class LightManager;
class GlowBuffer;
class SceneState;
class NetConnection;

class Sun;
class Sky;
class TerrainBlock;
class DecalManager;
class sgDecalProjector;
class TSShapeInstance;

class GFXD3D9OcclusionQuery;

struct FogVolume
{
   float    visibleDistance;
   float    minHeight;
   float    maxHeight;
   float    percentage;
   ColorF   color;
};

enum FogConstants
{
   MaxFogVolumes = 3,
   FogTextureDistSize = 64,
   FogTextureHeightSize = 64
};

//--------------------------------------------------------------------------
//-------------------------------------- SceneGraph
//
class SceneGraph
{
private:
#pragma message(ENGINE(添加当前场景状态变量及获取方法))
	SceneState *mCurSceneState;
public:
    inline SceneState* getCurSceneState(){return mCurSceneState; }
  public:
   SceneGraph(bool isClient);
   ~SceneGraph();

   /// @name SceneObject Management
   /// @{

   ///
   bool addObjectToScene(SceneObject*);
   void removeObjectFromScene(SceneObject*);
   void zoneInsert(SceneObject*);
   void zoneRemove(SceneObject*);
   /// @}

 
   /// @name Zone management
   /// @{

   ///
   void registerZones(SceneObject*, U32 numZones);
   void unregisterZones(SceneObject*);
   SceneObject* getZoneOwner(const U32 zone);
   /// @}

   /// @name Rendering and Scope Management
   /// @{

   ///
   void updateScene(const U32 objectMask = 0xffffffff);
   void doRenderScene();
   void renderScene(const U32 objectMask = 0xffffffff);
   void scopeScene(const Point3F& scopePosition,
                   const F32      scopeDistance,
                   NetConnection* netConnection);
   /// @}

   /// @name Camera
   /// For objects, valid only during the rendering cycle
   /// @{

   ///
   const Point3F& getBaseCameraPosition() const;
   const Point3F& getCurrCameraPosition() const;
   /// @}

   /// @name Fog/Visibility Management
   /// @{

   ///
   ColorF getFogColor();
   F32 getFogDistance();
   F32 getVisibleDistance();
   F32 getStaticObjDistance();
   F32 getObjSenseDistance();
   F32 getObjSenseDistanceMod();
   F32 getStaticObjDistanceMod();
   F32 getFogDistanceMod();
   F32 getVisibleDistanceMod();

   void setFogDistance(F32 dist);
   void setVisibleDistance(F32 dist);
   void setStaticObjDistance(F32 dist);
   void setObjSenseDistance(F32 dist);
   void setFogColor(ColorF fogColor);
   void setFogVolumes(U32 numFogVolumes, FogVolume *fogVolumes);
   void getFogVolumes(U32 &numFogVolumes, FogVolume * &fogVolumes);
   void buildFogTexture( SceneState *pState );

   void getFogCoordPair(F32 dist, F32 z, F32 &x, F32 &y);
   F32  getFogCoord(F32 dist, F32 z);
   /// @}

   void setSunColor(ColorF *ambientColor,ColorF *lightColor,ColorF *shadowColor);

   inline U32 getStateKey() { return(smStateKey); }

  private:
   void setBaseCameraPosition(const Point3F&);
   void setCurrCameraPosition(const Point3F&);

   //-------------------------------------- Private interface and data
  protected:
   GFXTexHandle mSfxBBCopy;    // special fx back buffer copy


   bool mFogDirty;
   SceneState *mLastState;
   GFXTexHandle mFogTexture;
   GFXTexHandle mBlackFogTexture;
   GFXTexHandle mFogTextureIntensity;
   static const U32 csmMaxTraversalDepth;
   static U32 smStateKey;
  
  public:
   static U32 smFrameKey;

  public:
   static F32 smVisibleDistanceMod;

   static bool useSpecial;
   static bool renderFog;

   /// This var is for cases where you need the "normal" camera position if you are
   /// in a reflection pass.  Used for correct fog calculations in reflections.
   Point3F mNormCamPos;

   U32 staticNum;

  protected:
   bool mIsClient;
   bool mReflectPass;

   // NonClipProjection is the projection matrix without oblique frustum clipping
   // applied to it (in reflections)
   MatrixF mNonClipProj;

   MatrixF mLastView2WorldMX;
   MatrixF mLastWorld2ViewMX;

   bool mHazeArrayDirty;
   static F32 mHazeArray[FogTextureDistSize];
   static U32 mHazeArrayi[FogTextureDistSize];
   static F32 mDistArray[FogTextureDistSize];

   F32 mInvVisibleDistance;
   F32 mHeightRangeBase;
   F32 mHeightOffset;
   F32 mInvHeightRange;

   U32  mCurrZoneEnd;
   U32  mNumActiveZones;

   Point3F  mBaseCameraPosition;
   Point3F  mCurrCameraPosition;

   U32       mNumFogVolumes;
   FogVolume mFogVolumes[MaxFogVolumes];
   F32       mFogDistance;
   F32       mVisibleDistance;
   F32       mStaticObjDistance;
   F32       mObjSenseDistance;
   ColorF    mFogColor;
   bool      mFogColorDirty;

   LightManager* mLightManager;

   Sun*          mCurrSun;
   Sky*          mCurrSky;
   TerrainBlock* mCurrTerrain;
   DecalManager* mCurrDecalManager;

   Vector< sgDecalProjector* > mDecalList;
   Vector<TerrainBlock*> mTerrainList;
   Vector<SceneObject*> mWaterList;

   Vector<SceneObject*> mInCameraObjects;
   Vector<SceneObject*> mInCameraStatics;
   Vector<SceneObject*> mInCameraShadows;
   U32 mDPtimes;
   U32 mTrDPtimes;
   U32 mSdDPtimes;
   U32 mSaDPtimes;
   U32 mSkDPtimes;
   U32 mQueryTimes;
   U32 mSkipObjects;
   U32 clippedRectCount;

   GlowBuffer  *mGlowBuffer;

   void            addRefPoolBlock();
   SceneObjectRef* allocateObjectRef();
   void            freeObjectRef(SceneObjectRef*);
   SceneObjectRef*         mFreeRefPool;
   Vector<SceneObjectRef*> mRefPoolBlocks;
   static const U32        csmRefPoolBlockSize;

   /// @see setDisplayTargetResolution
   Point2I mDisplayTargetResolution;

  public:
	  GFXD3D9OcclusionQuery *m_pQuery;

   /// @name dtr Display Target Resolution
   ///
   /// Some rendering must be targeted at a specific display resolution.
   /// This display resolution is distinct from the current RT's size
   /// (such as when rendering a reflection to a texture, for instance)
   /// so we store the size at which we're going to display the results of
   /// the current render.
   ///
   /// @{

   ///
   void onZombify();
   void setDisplayTargetResolution(const Point2I &size);
   const Point2I &getDisplayTargetResolution() const;

   /// @}

   // Gets the current 
   LightManager * getLightManager();
   // Sets the client and server scene graph lighting managers.
   static bool setLightingSystem(const char* sName);

   inline void setFogDirty(){mFogDirty = true;}
   inline F32 getFogHeightOffset(){ return mHeightOffset; }
   inline F32 getFogInvHeightRange(){ return mInvHeightRange; }

   inline Sun*          getCurrentSun()          { return mCurrSun; }
   inline Sky*          getCurrentSky()          { return mCurrSky; }
   inline TerrainBlock* getCurrentTerrain()      { return mCurrTerrain; }
   void setCurrentTerrain(TerrainBlock* ter);
   inline DecalManager* getCurrentDecalManager() { return mCurrDecalManager; }

   inline S32 getTerrainCount()						{ return mTerrainList.size();}
   //TerrainBlock* getTerrain(F32 x,F32 y);
   inline Vector<TerrainBlock*>& getTerrainList()      { return mTerrainList; }
   void getWaterObjectList(SimpleQueryList&);

   inline GFXTexHandle& getFogTexture()          { return mFogTexture; }
   inline GFXTexHandle& getBlackFogTexture()     { return mBlackFogTexture; }
   inline GFXTexHandle getFogTextureIntensity() { return mFogTextureIntensity; }

   inline void setReflectPass( bool isReflecting ) { mReflectPass = isReflecting; }
   inline bool isReflectPass(){ return mReflectPass; }

   // NonClipProjection is the projection matrix without oblique frustum clipping
   // applied to it (in reflections)
   inline void setNonClipProjection( MatrixF &proj ){ mNonClipProj = proj; }
   inline MatrixF getNonClipProjection(){ return mNonClipProj; }

   inline MatrixF &getLastView2WorldMX(){ return mLastView2WorldMX; }
   inline MatrixF &getLastWorld2ViewMX(){ return mLastWorld2ViewMX; }
   inline void setLastView2WorldMX(const MatrixF &mx){ mLastWorld2ViewMX=mLastView2WorldMX = mx; mLastWorld2ViewMX.inverse();}

   inline GFXTexHandle &getSfxBBCopy(){ return mSfxBBCopy; }
   inline GlowBuffer *getGlowBuff(){ return mGlowBuffer; }

   void addInCameraObject(SceneObject *obj){mInCameraObjects.push_back(obj);}
   Vector<SceneObject*> &getInCameraObjects(){return mInCameraObjects;}
   void addInCameraStatic(SceneObject *obj){mInCameraStatics.push_back(obj);}
   Vector<SceneObject*> &getInCameraStatics(){return mInCameraStatics;}
   void addInCameraShadow(SceneObject *obj){mInCameraShadows.push_back(obj);}
   Vector<SceneObject*> &getInCameraShadows(){return mInCameraShadows;}
   void setDPtimes(U32 times){mDPtimes=times;}
   U32 getDPtimes(){return mDPtimes;}
   void setTrDPtimes(U32 times){mTrDPtimes=times;}
   U32 getTrDPtimes(){return mTrDPtimes;}
   void setSdDPtimes(U32 times){mSdDPtimes=times;}
   U32 getSdDPtimes(){return mSdDPtimes;}
   void setSaDPtimes(U32 times){mSaDPtimes+=times;}
   U32 getSaDPtimes(){return mSaDPtimes;}
   void setSkDPtimes(U32 times){mSkDPtimes+=times;}
   U32 getSkDPtimes(){return mSkDPtimes;}
   void setQueryTimes(U32 times){mQueryTimes+=times;}
   U32 getQueryTimes(){return mQueryTimes;}
   void setSkipTimes(U32 times){mSkipObjects+=times;}
   U32 getSkipTimes(){return mSkipObjects;}
	void setRectCount(U32 count){clippedRectCount = count;}
	U32 getRectCount(){return clippedRectCount;}

   inline void registSunObject(Sun *pSun) {mCurrSun = pSun;}
   void processSunLight();
   void refreshTerrainLight();
   // Object database for zone managers
  protected:
   struct ZoneManager {
      SceneObject* obj;
      U32          zoneRangeStart;
      U32          numZones;
   };
   Vector<ZoneManager>     mZoneManagers;

   /// Zone Lists
   ///
   /// @note The object refs in this are somewhat singular in that the object pointer does not
   ///  point to a referenced object, but the owner of that zone...
   Vector<SceneObjectRef*> mZoneLists;

  protected:
   void buildSceneTree(SceneState*, SceneObject*, const U32, const U32, const U32);
   void traverseSceneTree(SceneState* pState);
   void treeTraverseVisit(SceneObject*, SceneState*, const U32);

   void compactZonesCheck();
   bool alreadyManagingZones(SceneObject*) const;

public:
   void findZone(const Point3F&, SceneObject*&, U32&);
protected:

   void rezoneObject(SceneObject*);

   void addToTerrainList(SceneObject* obj);
   void removeFromTerrainList(SceneObject* obj);

   void addToWaterList(SceneObject* obj);
   void removeFromWaterList(SceneObject* obj);
};

extern SceneGraph* gClientSceneGraph;
extern SceneGraph* gServerSceneGraph;


inline LightManager * SceneGraph::getLightManager()
{
   return mLightManager;
}


inline const Point3F& SceneGraph::getBaseCameraPosition() const
{
   return mBaseCameraPosition;
}

inline const Point3F& SceneGraph::getCurrCameraPosition() const
{
   return mCurrCameraPosition;
}

inline void SceneGraph::setBaseCameraPosition(const Point3F& p)
{
   mBaseCameraPosition = p;
}

inline void SceneGraph::getFogCoordPair(F32 dist, F32 z, F32 &x, F32 &y)
{
   x = (getVisibleDistanceMod() - dist) * mInvVisibleDistance;
   y = (z - mHeightOffset) * mInvHeightRange;
}

/*
inline F32 SceneGraph::getFogCoord(F32 dist, F32 z)
{
   GBitmap* pBitmap = mFogTexture.getBitmap();
   AssertFatal(pBitmap->getFormat() == GBitmap::RGBA, "Error, wrong format for this query");
   AssertFatal(pBitmap->getWidth() == 64 && pBitmap->getHeight() == 64, "Error, fog texture wrong dimensions");

   S32 x = S32(((getVisibleDistanceMod() - dist) * mInvVisibleDistance) * 63.0f);
   S32 y = S32(((z - mHeightOffset) * mInvHeightRange) * 63.0f);
   U32 samplex = mClamp(x, 0, 63);
   U32 sampley = mClamp(y, 0, 63);

   return F32(pBitmap->pBits[(((sampley * 64) + samplex) * 4) + 3]) / 255.0f;
}
*/

inline ColorF SceneGraph::getFogColor()
{
   return(mFogColor);
}

inline F32 SceneGraph::getFogDistance()
{
   return(mFogDistance);
}

inline F32 SceneGraph::getFogDistanceMod()
{
   return(mFogDistance * smVisibleDistanceMod);
}

inline F32 SceneGraph::getVisibleDistance()
{
   return(mVisibleDistance);
}

inline F32 SceneGraph::getStaticObjDistance()
{
	return(mStaticObjDistance);
}

inline F32 SceneGraph::getStaticObjDistanceMod()
{
	return(mStaticObjDistance * smVisibleDistanceMod);
}

inline F32 SceneGraph::getObjSenseDistance()
{
	return mObjSenseDistance;
}

inline F32 SceneGraph::getObjSenseDistanceMod()
{
	return(mObjSenseDistance * smVisibleDistanceMod);
}

inline F32 SceneGraph::getVisibleDistanceMod()
{
   return(mVisibleDistance * smVisibleDistanceMod);
}

inline void SceneGraph::setCurrCameraPosition(const Point3F& p)
{
   mCurrCameraPosition = p;
}

inline void SceneGraph::getFogVolumes(U32 &numFogVolumes, FogVolume * &fogVolumes)
{
   numFogVolumes = mNumFogVolumes;
   fogVolumes = mFogVolumes;
}

//--------------------------------------------------------------------------
inline SceneObjectRef* SceneGraph::allocateObjectRef()
{
   if (mFreeRefPool == NULL) {
      addRefPoolBlock();
   }
   AssertFatal(mFreeRefPool!=NULL, "Error, should always have a free reference here!");

   SceneObjectRef* ret = mFreeRefPool;
   mFreeRefPool = mFreeRefPool->nextInObj;

   ret->nextInObj = NULL;
   return ret;
}

inline void SceneGraph::freeObjectRef(SceneObjectRef* trash)
{
   trash->nextInBin = NULL;
   trash->prevInBin = NULL;
   trash->nextInObj = mFreeRefPool;
   mFreeRefPool     = trash;
}

inline SceneObject* SceneGraph::getZoneOwner(const U32 zone)
{
   AssertFatal(zone < mCurrZoneEnd, "Error, out of bounds zone selected!");

   return mZoneLists[zone]->object;
}


#endif //_SCENEGRAPH_H_


