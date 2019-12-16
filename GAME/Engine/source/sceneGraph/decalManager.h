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
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};

extern DecalManager* gDecalManager;


// ====================================================================================
//  投影模式
//  与阴影的方法相近，不会被其他物件所遮掩
//  支持图片序列动画
//  注：SceneManager::getCurrentDecalManager()只可取的gDecalManager，而不会取得gProjectDecalManager，故不会影响原有代码
// ====================================================================================
#pragma message(ENGINE(新加Decal投影模式))

//---------------------------------------------------------------------------------
// ProjectDecalData

class ProjectDecalData : public GameBaseData
{
	typedef GameBaseData Parent;

public:
	bool mInit;								// 图片是否初始化
	U32 lifeSpan;							// 存在期限
	SimTime mInterval;						// 动画更新间隔
	StringTableEntry mTextureName;			// 图片序列名称，读取过程详见InitTexture()
	Vector<GFXTexHandle> mTextures;			// 图片序列
    F32 mRot;                               // 旋转速率
	F32 mDiameter;							// 贴图直径
	F32 mDiameterScale;                     // 直径缩放速率
	F32 mMaxDiameter;						// 最大缩放尺寸
	F32 mAlpha;								//初始alpha
	F32 mAlphaScale;						//alpha变换速度
	F32 mMaxAlpha;							//最大alpha
	bool mLoop;								// 动画是否循环
	U32 mLoopTimeStep;						// 动画间隔
	U8 mLoopType;							// 动画的循环类型
public:
	ProjectDecalData();
	~ProjectDecalData();

	void packData(BitStream*);
	void unpackData(BitStream*);
	bool preload(bool server, char errorBuffer[256]);

	void InitTexture();						// 读取图片序列

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
		Rotation	= BIT(0),		// 旋转
		Permanent	= BIT(1),		// 永久性存在的对象，不受lifeSpan影响
	};

protected:
	U32 ownerId;					// 作为与sgDecalProjector相关联的依据
	U32 mDecalMode;

	F32 fade;						// 淡出
	U32 curIndex;					// 当前使用的图片
	U32 allocTime;					// 当前实例初始时间
	U32 lastTime1; //贴图动画时间
	U32 lastTime2; //缩放动画时间
	U32 lastTime3; //ALPHA动画时间
	U32 lastTimeAnimate; //动画间隔时间
	ColorI mColor;					// 颜色

	bool mRefresh;					// 是否需要刷新
	bool mRender;					// 是否需要渲染，决定最终是否渲染
	bool mNeedRender;				// 是否需要渲染，请求渲染
	Box3F mObjBox;					//对象盒子

	Point3F mPos;
	F32 mRotZ;
	F32 mRadius;
	F32 mProjectLen;				// 投影的距离

	F32 mMinDiameter;
	F32 mMaxDiameter;
	bool mDiameterFinished;

	F32 mAlpha;
	F32 mMinAlpha;
	F32 mMaxAlpha;
	bool mAlphaFinished;

	bool timeResetFlag;				//动画标记
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
	bool                          mRenderQueue;			// 队列中是否有实例需渲染

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
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};

extern ProjectDecalManager* gProjectDecalManager;

#endif // _H_DecalManager
