//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _WATERBLOCK_H_
#define _WATERBLOCK_H_

#ifndef _GAMEBASE_H_
#include "T3D/gameBase.h"
#endif
#ifndef _GFXDEVICE_H_
#include "gfx/gfxDevice.h"
#endif
#ifndef _SCENEDATA_H_
#include "materials/sceneData.h"
#endif
#ifndef _REFLECTPLANE_H_
#include "gfx/reflectPlane.h"
#endif
#ifndef _MATINSTANCE_H_
#include "materials/matInstance.h"
#endif
#ifndef _RENDERABLESCENEOBJECT_H
#include "sceneGraph/renderablesceneobject.h"
#endif
#include "terrain/shoreSurf.h"

class AudioEnvironment;
class GFXD3D9OcclusionQuery;
class GFXStateBlock;

//*****************************************************************************
// WaterBlock
//*****************************************************************************
class WaterBlock : public RenderableSceneObject
{
   typedef RenderableSceneObject Parent;

public:

   // LEGACY support
   enum EWaterType
   {
      eWater            = 0,
      eOceanWater       = 1,
      eRiverWater       = 2,
      eStagnantWater    = 3,
      eLava             = 4,
      eHotLava          = 5,
      eCrustyLava       = 6,
      eQuicksand        = 7,
   };

   enum MaterialType
   {
      BASE_PASS = 0,
      UNDERWATER_PASS = 1,
      FOG_PASS = 2,
      BLEND = 3,
      NO_REFLECT = 4,
      NUM_MAT_TYPES
   };

private:

   enum MaskBits {
      InitialUpdateMask = Parent::NextFreeMask,
      UpdateMask =   InitialUpdateMask << 1,
      NextFreeMask = UpdateMask << 1
   };

   enum consts
   {
      MAX_WAVES = 4,
      MAX_SHORESURFS = 4,
      NUM_ANIM_FRAMES = 32,
   };

   // wave parameters   
   Point2F  mWaveDir[ MAX_WAVES ];
   F32      mWaveSpeed[ MAX_WAVES ];
   Point2F  mWaveTexScale[ MAX_WAVES ];
   
   // vertex / index buffers
   Vector< GFXVertexBufferHandle<GFXVertexPCTT> * > mVertBuffList;
   Vector< GFXPrimitiveBufferHandle * >          mPrimBuffList;
   GFXVertexBufferHandle<GFXVertexPC>  mRadialVertBuff;
   GFXPrimitiveBufferHandle           mRadialPrimBuff;
   GFXD3D9OcclusionQuery *			m_pQuery;


   bool bInitialize;
   bool isInited() { return bInitialize;}

   // misc
   bool           mFullReflect;
   F32            mGridElementSize;
   U32            mWidth;
   U32            mHeight;
   F32            mElapsedTime;
   ColorI         mBaseColor;
   ColorI         mUnderwaterColor;
   F32            mClarity;
   F32            mFresnelBias;
   F32            mFresnelPower;
   F32            mVisibilityDepth;
   bool           mRenderFogMesh;
   Point3F        mPrevScale;
   GFXTexHandle   mBumpTex;
   EWaterType     mLiquidType;            ///< Type of liquid: Water?  Lava?  What?

   // reflect plane
   ReflectPlane_SingleZ   mReflectPlane;
   U32            mReflectTexSize;
   U32            mCurReflectTexSize;

   // materials
   const char     *  mSurfMatName[NUM_MAT_TYPES];
   MatInstance    *  mMatInstances[NUM_MAT_TYPES];

   // for reflection update interval
   U32 mReflectUpdateTicks;

   // 海岸线浪花效果
   bool mShoreSurfEnable[MAX_SHORESURFS];
   F32 mShoreHeight[MAX_SHORESURFS];
   S32 mSurfRandSeed[MAX_SHORESURFS];
   F32 mSurfWidth[MAX_SHORESURFS], mSurfWidthRand[MAX_SHORESURFS];
   F32 mSurfHeight[MAX_SHORESURFS], mSurfHeightRand[MAX_SHORESURFS];
   F32 mSurfPos[MAX_SHORESURFS], mSurfPosRand[MAX_SHORESURFS];
   F32 mSurfSpeed[MAX_SHORESURFS], mSurfSpeedRand[MAX_SHORESURFS];
   F32 mSurfAlpha[MAX_SHORESURFS], mSurfAlphaRand[MAX_SHORESURFS];
   F32 mSurfMiddle[MAX_SHORESURFS], mSurfMiddleRand[MAX_SHORESURFS];
   StringTableEntry mSurfTexFilename[MAX_SHORESURFS];
   ShoreSurf mShoreSurfs[MAX_SHORESURFS];
   void defaultShoreSurfData();
   void setupShoreSurfData();
   void renderShoreSurf();

   SceneGraphData setupSceneGraphInfo( SceneState *state );
   void setShaderParams();
   void setupVBIB();
   void setupVertexBlock( U32 width, U32 height, U32 rowOffset );
   void setupPrimitiveBlock( U32 width, U32 height );
   void drawUnderwaterFilter();
   void render1_1( SceneGraphData &sgData, const Point3F &camPosition );
   void render2_0( SceneGraphData &sgData, const Point3F &camPosition );
   void animBumpTex( SceneState *state );
   void setupRadialVBIB();
   void setMultiPassProjection();
   void clearVertBuffers();

   
   void cleanupMaterials();
protected:

   //-------------------------------------------------------
   // Standard engine functions
   //-------------------------------------------------------
   bool onAdd();
   void onRemove();
   bool prepRenderImage ( SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState=false);\
   void renderQuery		();
   void renderObject    (SceneState *state, RenderInst *ri );
   U64  packUpdate		(NetConnection *conn, U64 mask, BitStream *stream);
   void unpackUpdate	(NetConnection *conn,           BitStream *stream);

   void onGhostAlwaysDone();

   bool castRay(const Point3F &start, const Point3F &end, RayInfo* info);
public:
   WaterBlock();
   virtual ~WaterBlock();

   bool isPointSubmerged ( const Point3F &pos, bool worldSpace = true ) const{ return true; }
   AudioEnvironment * getAudioEnvironment(){ return NULL; }

   static void initPersistFields();

   void OnSerialize( CTinyStream& stream );
   void OnUnserialize( CTinyStream& stream );

   EWaterType getLiquidType() const                 { return mLiquidType; }
   bool isUnderwater( const Point3F &pnt );

   virtual void updateReflection();
   virtual void updateWaterQuery();
   virtual void inspectPostApply();

   void triggerTerrainChange(SceneObject *pTerrain);

   DECLARE_CONOBJECT(WaterBlock);
private:
	static GFXStateBlock* mSetQuerySB;
	static GFXStateBlock* mColorWriteTSB;
	static GFXStateBlock* mSetFilterSB;
	static GFXStateBlock* mSetAddr1_1SB;
	static GFXStateBlock* mUnderWater1_1SB;
	static GFXStateBlock* mFogMeshSB;
	static GFXStateBlock* mAlphaVColorSB;
	static GFXStateBlock* mFogMesh2SB;
	static GFXStateBlock* mClear2SB;
	static GFXStateBlock* mClearSB;
	static GFXStateBlock* mZTrueSB;
	static GFXStateBlock* mSetUnderWater;
	static GFXStateBlock* mClearUnderWaterSB;
	static GFXStateBlock* mSetAnimSB;
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
