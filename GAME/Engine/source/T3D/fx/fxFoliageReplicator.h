//-----------------------------------------------------------------------------
// PowerEngine
// Written by Melvyn May, 4th August 2002.
//-----------------------------------------------------------------------------

#ifndef _FOLIAGEREPLICATOR_H_
#define _FOLIAGEREPLICATOR_H_

#ifndef _RENDERABLESCENEOBJECT_H
#include "sceneGraph/RenderableSceneObject.h"
#endif

#ifndef _GFXTEXTUREHANDLE_H_
#include "gfx/gfxTextureHandle.h"
#endif

#pragma warning( push, 4 )

#define AREA_ANIMATION_ARC         (1.0f / 360.0f)

#define FXFOLIAGEREPLICATOR_COLLISION_MASK   (   TerrainObjectType     |   \
                                                 AtlasObjectType       |   \
                                                 InteriorObjectType    |   \
                                                 StaticObjectType      |   \
                                                 WaterObjectType      )

#define FXFOLIAGEREPLICATOR_NOWATER_COLLISION_MASK   (   TerrainObjectType      |   \
                                          InteriorObjectType      |   \
                                          StaticObjectType   )


#define FXFOLIAGE_ALPHA_EPSILON            1e-4



//------------------------------------------------------------------------------
// Class: fxFoliageItem
//------------------------------------------------------------------------------
class fxFoliageItem
{
public:
	MatrixF     Transform;		
	F32         Width;			
	F32         Height;			
	Box3F	    FoliageBox;		
	bool		Flipped;			
	F32         SwayPhase;     
	F32         SwayTimeRatio; 
	F32         LightPhase;		
	F32         LightTimeRatio; 
	U32         LastFrameSerialID; 
	S32         randnum;
	S32         randnum1;
	F32         randnum2;
	F32         mViewZ;
};

//------------------------------------------------------------------------------
// Class: fxFoliageCulledList
//------------------------------------------------------------------------------
class fxFoliageCulledList
{
public:
   fxFoliageCulledList() {};
   fxFoliageCulledList(Box3F SearchBox, fxFoliageCulledList* InVec);
   ~fxFoliageCulledList() {};

   void FindCandidates(Box3F SearchBox, fxFoliageCulledList* InVec);

   U32 GetListCount(void) { return mCulledObjectSet.size(); };
   fxFoliageItem* GetElement(U32 index) { return mCulledObjectSet[index]; };

   Vector<fxFoliageItem*>   mCulledObjectSet;      // Culled Object Set.
};


//------------------------------------------------------------------------------
// Class: fxFoliageQuadNode
//------------------------------------------------------------------------------
class fxFoliageQuadrantNode {
public:
   U32                  Level;
   Box3F               QuadrantBox;
   fxFoliageQuadrantNode*   QuadrantChildNode[4];
   Vector<fxFoliageItem*>   RenderList;
	// Used in DrawIndexPrimitive call.
	U32							 startIndex;
	U32							 primitiveCount;
};


//------------------------------------------------------------------------------
// Class: fxFoliageRenderList
//------------------------------------------------------------------------------
class fxFoliageRenderList
{
public:
   Point3F               FarPosLeftUp;      // View Frustum.
   Point3F               FarPosLeftDown;
   Point3F               FarPosRightUp;
   Point3F               FarPosRightDown;
   Point3F               CameraPosition;      // Camera Position.
   Box3F               mBox;            // Clipping Box.
   PlaneF               ViewPlanes[5];      // Clipping View-Planes.

   Vector<fxFoliageItem*>   mVisObjectSet;      // Visible Object Set.
   F32                  mHeightLerp;      // Height Lerp.

public:
   bool IsQuadrantVisible(const Box3F VisBox, const MatrixF& RenderTransform);
   void SetupClipPlanes(SceneState* state, const F32 FarClipPlane);
   void DrawQuadBox(const Box3F& QuadBox, const ColorF Colour);
};

#pragma warning(disable : 4100)

// Define a vertex 
DEFINE_VERT( GFXVertexFoliage,
             GFXVertexFlagXYZ | GFXVertexFlagNormal | GFXVertexFlagTextureCount5 | 
             GFXVertexFlagUV0 | GFXVertexFlagUV1 | GFXVertexFlagUV2 | GFXVertexFlagUVW3| GFXVertexFlagUV4)
{
   Point3F point;
   Point3F normal;
   Point2F texCoord;
   Point2F texCoord2;
   Point2F texCoord3;
   Point3F texCoord4;
   Point2F texCoord5;
};

#pragma warning(default : 4100)

//------------------------------------------------------------------------------
// Class: fxFoliageReplicator
//------------------------------------------------------------------------------
class fxFoliageReplicator : public RenderableSceneObject
{
private:
   typedef RenderableSceneObject      Parent;

protected:

   void CreateFoliage(void);
   void DestroyFoliage(void);
	void DestroyFoliageItems();


   void SyncFoliageReplicators(void);

   Box3F FetchQuadrant(Box3F Box, U32 Quadrant);
   void ProcessQuadrant(fxFoliageQuadrantNode* pParentNode, fxFoliageCulledList* pCullList, U32 Quadrant);
   void ProcessNodeChildren(fxFoliageQuadrantNode* pParentNode, fxFoliageCulledList* pCullList);

   enum {   FoliageReplicationMask   = (1 << 0) };


   U32   mCreationAreaAngle;
   bool  mClientReplicationStarted;
   bool  mAddedToScene;
   U32   mCurrentFoliageCount;

   Vector<fxFoliageQuadrantNode*>   mFoliageQuadTree;
   Vector<fxFoliageItem*>           mReplicatedFoliage;
   fxFoliageRenderList              mFrustumRenderSet;
   Vector<fxFoliageItem*>           mSortFoliageItem;

	GFXVertexBufferHandle<GFXVertexFoliage> mVertexBuffer;
	GFXPrimitiveBufferHandle	mPrimBuffer;
	GFXShader*						mShader;
	GBitmap*							mAlphaLookup;

   MRandomLCG                 RandomGen;
   F32                        mFadeInGradient;
   F32                        mFadeOutGradient;
   S32                        mLastRenderTime;
   F32                        mGlobalSwayPhase;
   F32                        mGlobalSwayTimeRatio;
   F32                        mGlobalLightPhase;
   F32                        mGlobalLightTimeRatio;
   U32                        mFrameSerialID;

   U32                        mQuadTreeLevels;            // Quad-Tree Levels.
   U32                        mPotentialFoliageNodes;     // Potential Foliage Nodes.
   U32                        mNextAllocatedNodeIdx;      // Next Allocated Node Index.
   U32                        mBillboardsAcquired;        // Billboards Acquired.

	// Used for alpha lookup in the pixel shader
	GFXTexHandle					mAlphaTexture;
	S32             mWidth;
	S32             mHeight;
	Point2F* mTextureCoordUV1;
	Point2F* mTextureCoordUV2;
	Point2F* mTextureCoordUV3;
	Point2F* mTextureCoordUV4;
	
	void SetupBuffers(); 
	void renderBuffers(SceneState* state);
	void renderArc(const F32 fRadiusX, const F32 fRadiusY);
	void renderPlacementArea(const F32 ElapsedTime);
	void renderQuad(fxFoliageQuadrantNode* quadNode, const MatrixF& RenderTransform, const bool UseDebug);
	void computeAlphaTex();
public:
   fxFoliageReplicator();
   virtual ~fxFoliageReplicator();

   virtual void StartUp(void);
   void ShowReplication(void);
   void HideReplication(void);

   // RenderableSceneObject
   virtual void renderObject(SceneState *state, RenderInst *ri);
   virtual bool prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone,
                        const bool modifyBaseZoneState = false);

   // SimObject
   virtual bool onAdd();
   void onRemove();
   void onEditorEnable();
   void onEditorDisable();
   void inspectPostApply();

   // NetObject
   virtual U64 packUpdate(NetConnection *conn, U64 mask, BitStream *stream);
   virtual void unpackUpdate(NetConnection *conn, BitStream *stream);


   void OnSerialize( CTinyStream& stream );
   void OnUnserialize( CTinyStream& stream );

   // ConObject.
   static void initPersistFields();

   // Field Data.
   class tagFieldData
   {
      public:

      bool              mUseDebugInfo;
      F32               mDebugBoxHeight;
      U32               mSeed;
      StringTableEntry  mFoliageFile;
      GFXTexHandle      mFoliageTexture;
      U32               mFoliageCount;
      U32               mFoliageRetries;

      U32               mInnerRadiusX;
      U32               mInnerRadiusY;
      U32               mOuterRadiusX;
      U32               mOuterRadiusY;

      F32               mMinWidth;
      F32               mMaxWidth;
      F32               mMinHeight;
      F32               mMaxHeight;
      bool              mFixAspectRatio;
      bool              mFixSizeToMax;
      F32               mOffsetZ;
      bool              mRandomFlip;

      bool              mUseCulling;
      U32               mCullResolution;
      F32               mViewDistance;
      F32               mViewClosest;
      F32               mFadeInRegion;
      F32               mFadeOutRegion;
      F32               mAlphaCutoff;
      F32               mGroundAlpha;

      bool              mSwayOn;
      bool              mSwaySync;
      F32               mSwayMagnitudeSide;
      F32               mSwayMagnitudeFront;
      F32               mMinSwayTime;
      F32               mMaxSwayTime;

      bool              mLightOn;
      bool              mLightSync;
      F32               mMinLuminance;
      F32               mMaxLuminance;
      F32               mLightTime;

      bool            mAllowOnTerrain;
      bool            mAllowOnInteriors;
      bool            mAllowStatics;
      bool            mAllowOnWater;
      bool            mAllowWaterSurface;
      S32             mAllowedTerrainSlope;

      bool            mHideFoliage;
      bool            mShowPlacementArea;
      U32             mPlacementBandHeight;
      ColorF          mPlaceAreaColour;

      tagFieldData()
      {
         // Set Defaults.
         mUseDebugInfo         = false;
         mDebugBoxHeight       = 1.0f;
         mSeed                 = 1376312589;
         mFoliageFile          = StringTable->insert("");
         mFoliageTexture       = GFXTexHandle();
         mFoliageCount         = 10;
         mFoliageRetries       = 100;

         mInnerRadiusX         = 0;
         mInnerRadiusY         = 0;
         mOuterRadiusX         = 128;
         mOuterRadiusY         = 128;

         mMinWidth             = 1;
         mMaxWidth             = 3;
         mMinHeight            = 1;
         mMaxHeight            = 5;
         mFixAspectRatio       = true;
         mFixSizeToMax         = false;
         mOffsetZ              = 0;
         mRandomFlip           = true;

         mUseCulling           = true;
         mCullResolution       = 64;
         mViewDistance         = 50.0f;
         mViewClosest          = 1.0f;
         mFadeInRegion         = 10.0f;
         mFadeOutRegion        = 1.0f;
         mAlphaCutoff          = 0.2f;
         mGroundAlpha          = 1.0f;

         mSwayOn               = false;
         mSwaySync             = false;
         mSwayMagnitudeSide    = 0.1f;
         mSwayMagnitudeFront   = 0.2f;
         mMinSwayTime          = 3.0f;
         mMaxSwayTime          = 10.0f;

         mLightOn              = false;
         mLightSync            = false;
         mMinLuminance         = 0.7f;
         mMaxLuminance         = 1.0f;
         mLightTime            = 5.0f;

         mAllowOnTerrain       = true;
         mAllowOnInteriors     = true;
         mAllowStatics         = true;
         mAllowOnWater         = false;
         mAllowWaterSurface    = false;
         mAllowedTerrainSlope  = 90;

         mHideFoliage          = false;
         mShowPlacementArea    = true;
         mPlacementBandHeight  = 25;
         mPlaceAreaColour      .set(0.4f, 0, 0.8f);
      }

   } mFieldData;

   // Declare Console Object.
   DECLARE_CONOBJECT(fxFoliageReplicator);
private:
	static GFXStateBlock* mAreaSB;
	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mDebugSetSB;
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

class fxFireReplicator : public fxFoliageReplicator
{
	typedef fxFoliageReplicator      Parent;
public:
	fxFireReplicator();
	~fxFireReplicator();
	bool     onAdd();
	void     renderObject(SceneState *state, RenderInst *ri);
    static void     initPersistFields();
	void     packData(BitStream* stream);
	void     unpackData(BitStream* stream);
	void unpackUpdate(NetConnection *conn, BitStream *stream);
	U64 packUpdate(NetConnection *conn, U64 mask, BitStream *stream);
	DECLARE_CONOBJECT(fxFireReplicator);
protected:
	void    MySort();
private:
    Point4F layer_speed;
	float   distortion_amount2;
	float   distortion_amount1;
	float   distortion_amount0;
	ColorF  myColor;
	float   bumpscale;
	GFXTexHandle mTexture;
	StringTableEntry      m_TextureName;
	GFXTexHandle mTexture2;
	StringTableEntry      m_Texture2Name;
    ShaderData *    fireshader;
};

#include "ts/tsShapeInstance.h"
#include "ts/tsShape.h"

class VolumeFog  : public RenderableSceneObject
{
	typedef RenderableSceneObject      Parent;
public:
	DECLARE_CONOBJECT(VolumeFog);


	VolumeFog();
	~VolumeFog();

	static void     initPersistFields();
	bool onAdd();
	void     onRemove();
	static void shutdown();
	void Render(RenderInst *ri);
	bool prepRenderImage( SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState=false);
	void unpackUpdate(NetConnection *conn, BitStream *stream);
	U64 packUpdate(NetConnection *conn, U64 mask, BitStream *stream);
	void     packData(BitStream* stream);
	void     unpackData(BitStream* stream);
	void    renderObject( SceneState *state, RenderInst *ri );
	void OnSerialize( CTinyStream& stream );
	void OnUnserialize( CTinyStream& stream );
protected:
private:
	GFXTexHandle		                    m_pTexDepth;
	ShaderData*                             m_pShader;
	Point4F                                 color_amount;
	ColorF                                  m_FogColor;
    StringTableEntry                        m_TexName;
	U32                                     m_dwNowTime;
    float                                   m_fTime;
	Point3F                                 m_vSpeed;
	float                                   m_fRadius;
	float                                   m_fAttenuationStart;
};

#pragma warning( pop ) 
#endif // _FOLIAGEREPLICATOR_H_
