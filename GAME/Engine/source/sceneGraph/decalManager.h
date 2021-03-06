//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _DECALMANAGER_H_
#define _DECALMANAGER_H_

#ifndef _RENDERABLESCENEOBJECT_H
#include "sceneGraph/RenderableSceneObject.h"
#endif

#include "gfx/gfxTextureHandle.h"
#include "T3D/gameBase.h"
#include "gfx/gfxVertexBuffer.h"
#include "lightingSystem/synapseGaming/vectorProjector.h"

class GameObject;
class GFXStateBlock;

/// DataBlock implementation for decals.
class DecalData : public GameBaseData
{
   typedef GameBaseData Parent;

   //-------------------------------------- Console set variables
  public:
   F32               sizeX;
   F32               sizeY;
   StringTableEntry  textureName;
   
   bool selfIlluminated;
   U32 lifeSpan;

   //-------------------------------------- load set variables
  public:
   GFXTexHandle textureHandle;

  public:
   DecalData();
   ~DecalData();
	
   void packData(BitStream*);
   void unpackData(BitStream*);
   bool preload(bool server, char errorBuffer[256]);

   DECLARE_CONOBJECT(DecalData);
   static void initPersistFields();
};

/// Store an instance of a decal.
struct DecalInstance
{
   DecalData* decalData;
   Point3F    point[4];
   
   U32 ownerId;

   U32            allocTime;
   F32            fade;
   DecalInstance* next;
};

/// Manage decals in the world.
class DecalManager : public RenderableSceneObject
{
   typedef RenderableSceneObject Parent;

   Vector<DecalInstance*> mDecalQueue;
   bool                   mQueueDirty;

public:
   void addDecal(const Point3F& pos,
                 const Point3F& rot,
                 Point3F normal,
                 const Point3F& scale,
                 DecalData*, U32);
   void ageDecal(U32);
   void findSpace();
   
   static U32             smMaxNumDecals;
   static U32             smDecalTimeout;

   static bool sgThisIsSelfIlluminated;
   static bool sgLastWasSelfIlluminated;

   static const U32          csmFreePoolBlockSize;
   Vector<DecalInstance*>    mFreePoolBlocks;
   DecalInstance*            mFreePool;

protected:
   bool prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState);
   void renderObject(SceneState *state, RenderInst *);

   DecalInstance* allocateDecalInstance();
   void freeDecalInstance(DecalInstance*);

  public:
   DecalManager();
   ~DecalManager();
   static void consoleInit();

   /// @name Decal Addition
   ///
   /// These functions allow you to add new decals to the world.
   /// @{
   void addDecal(const Point3F& pos,
                 const Point3F& rot,
                 Point3F normal,
                 const Point3F& scale,
                 DecalData*);
   void addDecal(const Point3F& pos,
                 const Point3F& rot,
                 Point3F normal,
                 DecalData*);
   void addDecal(const Point3F& pos,
                 Point3F normal,
                 DecalData*);
   /// @}

   void dataDeleted(DecalData *data);

   void renderDecal();
   DECLARE_CONOBJECT(DecalManager);

   static bool smDecalsOn;
private:
	static GFXStateBlock* mDisTexSB;
	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mSelfTrueSB;
	static GFXStateBlock* mSelfFalseSB;
	static GFXStateBlock* mClearSB;
public:
	static void init();
	static void shutdown();
	//
	//??????????????
	//
	static void releaseStateBlock();

	//
	//??????????????
	//
	static void resetStateBlock();
};

extern DecalManager* gDecalManager;


// ====================================================================================
//  ????????
//  ??????????????????????????????????????
//  ????????????????
//  ????SceneManager::getCurrentDecalManager()????????gDecalManager????????????gProjectDecalManager????????????????????
// ====================================================================================
#pragma message(ENGINE(????Decal????????))

//---------------------------------------------------------------------------------
// ProjectDecalData

class ProjectDecalData : public GameBaseData
{
	typedef GameBaseData Parent;

public:
	bool mInit;								// ??????????????
	U32 lifeSpan;							// ????????
	SimTime mInterval;						// ????????????
	StringTableEntry mTextureName;			// ??????????????????????????InitTexture()
	Vector<GFXTexHandle> mTextures;			// ????????
    F32 mRot;                               // ????????
	F32 mDiameter;							// ????????
	F32 mDiameterScale;                     // ????????????
	F32 mMaxDiameter;						// ????????????
	F32 mAlpha;								//????alpha
	F32 mAlphaScale;						//alpha????????
	F32 mMaxAlpha;							//????alpha
	bool mLoop;								// ????????????
	U32 mLoopTimeStep;						// ????????
	U8 mLoopType;							// ??????????????
public:
	ProjectDecalData();
	~ProjectDecalData();

	void packData(BitStream*);
	void unpackData(BitStream*);
	bool preload(bool server, char errorBuffer[256]);

	void InitTexture();						// ????????????

	DECLARE_CONOBJECT(ProjectDecalData);
	static void initPersistFields();
};

//---------------------------------------------------------------------------------
// ProjectDecalInstance
class ProjectDecalManager;

struct ProjectDecalInstance 
{
	friend class ProjectDecalManager;
	enum Mode
	{
		Rotation	= BIT(0),		// ????
		Permanent	= BIT(1),		// ??????????????????????lifeSpan????
	};

protected:
	U32 ownerId;					// ??????sgDecalProjector????????????
	U32 mDecalMode;

	F32 fade;						// ????
	U32 curIndex;					// ??????????????
	U32 allocTime;					// ????????????????
	U32 lastTime1; //????????????
	U32 lastTime2; //????????????
	U32 lastTime3; //ALPHA????????
	U32 lastTimeAnimate; //????????????
	ColorI mColor;					// ????

	bool mRefresh;					// ????????????
	bool mRender;					// ??????????????????????????????
	bool mNeedRender;				// ??????????????????????
	Box3F mObjBox;					//????????

	Point3F mPos;
	F32 mRotZ;
	F32 mRadius;
	F32 mProjectLen;				// ??????????

	F32 mMinDiameter;
	F32 mMaxDiameter;
	bool mDiameterFinished;

	F32 mAlpha;
	F32 mMinAlpha;
	F32 mMaxAlpha;
	bool mAlphaFinished;

	bool timeResetFlag;				//????????
	bool alphaLoopDir;	
	bool scaleLoopDir;

    bool mByHand; 

	ProjectDecalInstance* next;

	MatrixF mLightToWorld;
	Vector<DepthSortList::Poly> mPartition;
	Vector<Point3F> mPartitionVerts;
	GFXVertexBufferHandle<GFXVertexPCT> mVBuffer;

public:
	ProjectDecalData* mProjectDecalData;
	const static U32 smProjectMask;

	ProjectDecalInstance();
	~ProjectDecalInstance();
	void		setColor(ColorI& color);
	void		setPos(Point3F& pos);
	void		setRotZ(F32 rotZ);
	void		setRadius(F32 radius);
	void		setProjectLen(F32 len);
	void		setFade(F32 val);
	void		setNeedRender(bool val);
	bool		prepare();
	void		renderProjectDecal();
};

//---------------------------------------------------------------------------------
// ProjectDecalManager

class ProjectDecalManager : public RenderableSceneObject
{
	typedef RenderableSceneObject Parent;

	Vector<ProjectDecalInstance*> mProjectDecalQueue;
	bool                          mQueueDirty;
	bool                          mRenderQueue;			// ??????????????????????

public:
	void ageDecal(U32);
	void findSpace();

	static bool            smDecalsOn;
	static U32             smMaxNumDecals;
	static U32             smDecalTimeout;

	static bool sgThisIsSelfIlluminated;
	static bool sgLastWasSelfIlluminated;

	static const U32              csmFreePoolBlockSize;
	Vector<ProjectDecalInstance*> mFreePoolBlocks;
	ProjectDecalInstance*         mFreePool;

	static U32                    mGlobeId;

	void doPrepRender(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState)	{prepRenderImage(state,stateKey,startZone,modifyBaseZoneState);}

protected:
	bool prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState);
	void renderObject(SceneState *state, RenderInst *);

	ProjectDecalInstance* allocateDecalInstance();
	void freeDecalInstance(ProjectDecalInstance*);

public:
	ProjectDecalManager();
	~ProjectDecalManager();

	static void consoleInit();

	ProjectDecalInstance* getDecal( U32 id);
	ProjectDecalInstance* addDecal( U32 mode,
		ProjectDecalData* pData,
		const Point3F& pos, U32& id,
		F32 rotZ = 0,
		F32 radius = 0,
		F32 shadowLen = 0,
		F32 fadeVal = 0, bool byhand = false);

	void dataDeleted(ProjectDecalData *data);

	DECLARE_CONOBJECT(ProjectDecalManager);
private:
	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mZWriteSB;
	static GFXStateBlock* mClearSB;
public:
	static void init();
	static void shutdown();
	//
	//??????????????
	//
	static void releaseStateBlock();

	//
	//??????????????
	//
	static void resetStateBlock();
};

extern ProjectDecalManager* gProjectDecalManager;

#endif // _H_DecalManager
