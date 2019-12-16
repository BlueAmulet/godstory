//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#ifndef _SHAPEBASE_H_
#include "T3D/shapeBase.h"
#endif
#ifndef _BOXCONVEX_H_
#include "collision/boxConvex.h"
#endif

#include "T3D/gameProcess.h"
#include "Gameplay/GameplayCommon.h"
#include "Gameplay/GameObjects/GameObjectData.h"
#include "Gameplay/GameObjects/Buff.h"
#include "Gameplay/GameObjects/Spell.h"
#include "Gameplay/GameObjects/Cooldown.h"
#include "Gameplay/GameObjects/Skill.h"

#ifndef NTJ_EDITOR
#include "Gameplay/ai/AIFsm.h"
#endif

//颜色定义
#define COLORI_NPC_FRIEND           242,228,12                // 友好NPC
#define COLORI_NPC_ENEMY            241,14,14                 // 敌对NPC
#define COLORI_NPC_TITLE            222,144,101               // NPC称号
#define COLORI_BORDER				0,0,0					  // 描边黑色

#define COLORI_PLAYER_ME            255,255,255               // 自己
#define COLORI_PLAYER_BODY          107,107,107               // 尸体
#define COLORI_PLAYER_TEAMMATE      107,255,64                // 队友
#define COLORI_PLAYER_ENEMY         COLORI_NPC_ENEMY          // 敌对
#define COLORI_PLAYER_TITLE         COLORI_NPC_FRIEND         // 称号
#define COLORI_PLAYER_DAMAGE        COLORI_NPC_ENEMY          // 伤害
#define COLORI_PLAYER_ADDHPMP       0,176,80                  // 加HP,MP

#define COLORI_FACTION_ME           COLORI_PLAYER_TEAMMATE    // 普通帮会
#define COLORI_FACTION_ENEMY        COLORI_NPC_ENEMY          // 敌对帮会

#define COLORI_PET_ME               COLORI_PLAYER_TEAMMATE    // 自己
#define COLORI_PET_OTHERS           COLORI_PLAYER_ME          // 别人
#define COLORI_PET_ENEMY            COLORI_PLAYER_ENEMY       // 敌对 
#define COLORI_PET_TITLE            COLORI_PLAYER_TITLE       // 称号

#define COLORI_MONSTER_PASSIVE      COLORI_NPC_FRIEND         // 被动怪
#define COLORI_MONSTER_AGGRESSIVE   COLORI_NPC_ENEMY          // 主动怪
#define COLORI_MONSTER_TITLE        COLORI_NPC_TITLE          // 称号
#define COLORI_MONSTER_DAMAGE       COLORI_NPC_FRIEND         // 伤害

#define CHAT_WORD_MAX_LENGTH        256                       // 头顶冒泡最大长度

class ParticleEmitter;
class ParticleEmitterData;
class DecalData;
class SplashData;
class Player;
class EffectObject;

//----------------------------------------------------------------------------

class GameObject: public ShapeBase
{
	typedef ShapeBase Parent;

	friend class Player;
	friend class NpcObject;
	friend class PetObject;
	friend class AIBase;
	friend class Spell;
	friend class SkillTable;

public:
	enum ImageSlot
	{
		Slot_Face			= 0,		// 脸部
		Slot_Hair,						// 头发
		Slot_LeftHand,					// 左手武器
		Slot_RightHand,					// 右手武器
		Slot_Ex1,						// 装饰1
		Slot_Ex2,						// 装饰2
		Slot_Ex3,						// 装饰3
		Slot_Talisman,					// 法宝
	};

	enum transformBits
	{
		RaceBits = 5,
		FamilyBits = 4,
		LevelBits = 8,
		ClassBits = 3,
		ArmStatusBits = 4,
		TargetObjEPBits = 4,
	};

	enum
	{
		InvisibleTargetTick = 2,
		InvisibleImmunityTick = 32,
		SetTargetToClient = BIT(31),
	};

	/// Bit masks for different types of events
	// UpdataMask
	const static U64 ActionMask   = Parent::NextFreeMask << 0;
	const static U64 MoveMask     = Parent::NextFreeMask << 1;
	const static U64 ImpactMask   = Parent::NextFreeMask << 2;
	const static U64 BuffMask     = Parent::NextFreeMask << 3;
	const static U64 SpellMask    = Parent::NextFreeMask << 4;
	const static U64 TargetMask   = Parent::NextFreeMask << 5;
	const static U64 HPMask       = Parent::NextFreeMask << 6;
	const static U64 MPMask       = Parent::NextFreeMask << 7;
	const static U64 PPMask       = Parent::NextFreeMask << 8;
	const static U64 StatsMask    = Parent::NextFreeMask << 9;
	const static U64 BaseInfoMask = Parent::NextFreeMask << 10;
	const static U64 WoundMask    = Parent::NextFreeMask << 11;
	const static U64 ChainEPMask  = Parent::NextFreeMask << 12;
	const static U64 SpeedMask    = Parent::NextFreeMask << 13;
	const static U64 CtrlMask     = Parent::NextFreeMask << 14;
	const static U64 ChatInfoMask = Parent::NextFreeMask << 15;
	const static U64 NextFreeMask = Parent::NextFreeMask << 16;

private:
	struct Range {
		Range(F32 _min,F32 _max) {
			min = _min;
			max = _max;
			delta = _max - _min;
		};
		F32 min,max;
		F32 delta;
	};

	SimObjectPtr<ParticleEmitter> mSplashEmitter[GameObjectData::NUM_SPLASH_EMITTERS];
	F32 mBubbleEmitterTime;

	/// Client interpolation/warp data
	struct StateDelta {
		Move move;                    ///< Last move from server
		F32 dt;                       ///< Last interpolation time
		/// @name Interpolation data
		/// @{

		Point3F pos;
		Point3F rot;
		Point3F head;
		VectorF posVec;
		VectorF rotVec;
		VectorF headVec;
		/// @}

		/// @name Warp data
		/// @{

		S32 warpTicks;
		S32 warpRotTicks;
		Point3F warpOffset;
		Point3F rotOffset;
		/// @}
	};
	StateDelta delta;                ///< Used for interpolation on the client.  @see StateDelta
	S32 mPredictionCount;            ///< Number of ticks to predict

	// Current pos, vel etc.
	#pragma message(ENGINE(删除mHead变量))
	//Point3F mHead;                   ///< Head rotation, uses only x & z
	#pragma message(ENGINE(为平滑摄像机新增变量))
	F32 mRenderRot;
	Point3F mRot;                    ///< Body rotation, uses only z
	VectorF mVelocity;               ///< Velocity
	Point3F mAnchorPoint;            ///< Pos compression anchor
	static F32 mGravity;             ///< Gravity
	S32 mImpactSound;

	// <Edit> [9/18/2009 joy] JumpPath
	struct JumpPath
	{
		Point3F begin;
		Point3F end;
		F32 a;
		F32 b;
		F32 lenH;
		F32 t;
		F32 advTime;
		F32 rotz;
		bool jumping;
		bool fall;
	};
	JumpPath mJumpPath;

	struct FlyPath
	{
		U32 pathId;
		SimTime flyPos;
		bool atEnd;
		U32 mountId;
	};
	FlyPath mFlyPath;

	bool mBlink;

	S32 mMountPending;               ///< mMountPending suppresses tickDelay countdown so players will sit until
	///< their mount, or another animation, comes through (or 13 seconds elapses).

	/// Main player state
	enum ActionState {
		NullState,
		MoveState,
		RecoverState,
		NumStateBits = 3
	};
	ActionState mState;              ///< What is the player doing? @see ActionState
	bool mFalling;                   ///< Falling in mid-air?
	S32  mJumpDelay;                 ///< Delay till next jump
	S32  mContactTimer;              ///< Ticks since last contact

	Point3F mJumpSurfaceNormal;      ///< Normal of the surface the player last jumped on
	U32 mJumpSurfaceLastContact;     ///< How long it's been since the player landed (ticks)
	F32  mWeaponBackFraction;        ///< Amount to slide the weapon back (if it's up against something)

	SFXSource* mMoveBubbleSound;   ///< Sound for moving bubbles
	SFXSource* mWaterBreathSound;  ///< Sound for underwater breath

	SimObjectPtr<ShapeBase> mControlObject; ///< Controlling object

	#pragma message(ENGINE(初步修改了玩家移动及其校验))
	// 速度的验证
	F32 mCheckVelocity_1;
	F32 mServerOldSpeed;												// 变化速度前的旧速度
	bool mServerSpeedChange;											// 是否速度有变化
	F32 mServerSynCount;												// 当前同步次数
	bool m_bMoving;														// 是否正在移动

	/// @name Animation threads & data
	/// @{
	#pragma message(ENGINE(修改了上下部动作的叠加))
	// 动作分为上下两部分，这两部分能同时推进不同的动作

	struct ActionAnimation {
		U32 action;
		TSThread* thread;
		SimTime endTime;               // before picking another.
		bool forward;
		bool firstPerson;
		bool waitForEnd;
		bool holdAtEnd;
		bool animateOnServer;
		bool atEnd;
	} mUpperActionAnimation, mLowerActionAnimation;
	TSThread* mWoundThread;

	static Range mArmRange;
	static Range mHeadVRange;
	static Range mHeadHRange;
	/// @}

	// 跟随动作的特效，主要用于瞬发技能特效方面
	U32 mSkillDataEP;
	SkillData::Selectable mSelectableEP;
	bool mTargetIsObj;
	Vector<U32> mTargetObjEP;
	Point3F mDestEP;

public:
	char mChatInfo[CHAT_WORD_MAX_LENGTH];
private:
	bool mInMissionArea;          ///< Are we in the mission area?
	//
	S32 mRecoverTicks;            ///< same as recoverTicks in the player datablock
	U32 mReversePending;

	bool inLiquid;                ///< Are we in liquid?
	//
	GameObjectData* mDataBlock;       ///< MMmmmmm...datablock...

	bool debugFlag;
	bool interpolateFlag;
	Point3F mPrevRenderPos;             ///< Ray: 上一次的渲染坐标，和mCurrRenderPos 会做帧插值
	Point3F mCurrRenderPos;             ///< Ray: 本次次的渲染坐标
	Point3F mCurrRenderStep;             ///< Ray: 本次次的渲染坐标
	Point3F mLastPos;             ///< Holds the last position for physics updates
	Point3F mLastWaterPos;        ///< Same as mLastPos, but for water

	struct ContactInfo {
		bool     contacted, jump, run;
		VectorF  contactNormal;
		void clear()   {contacted=jump=run=false; contactNormal.set(1,1,1);}
		ContactInfo()  {clear();}
	} mContactInfo;

	struct Death {
		F32      lastPos;
		Point3F  posAdd;
		VectorF  rotate;
		VectorF  curNormal;
		F32      curSink;
		void     clear()           {dMemset(this, 0, sizeof(*this)); initFall();}
		VectorF  getPosAdd()       {VectorF ret(posAdd); posAdd.set(0,0,0); return ret;}
		bool     haveVelocity()    {return posAdd.x != 0 || posAdd.y != 0;}
		void     initFall()        {curNormal.set(0,0,1); curSink = 0;}
		Death()                    {clear();}
		MatrixF* fallToGround(F32 adjust, const Point3F& pos, F32 zrot, F32 boxRad);
	} mDeath;

	
protected:
	U32 mGameObjectMask;

	StringTableEntry				m_BodyShape;			// 当前身体模型
	StringTableEntry				m_FaceShape;			// 当前脸部模型
	StringTableEntry				m_HairShape;			// 当前发型模型

	enSex							m_Sex;					// 性别
	enRace							m_Race;					// 种族
	enFamily						m_Family;				// 门宗
	enInfluence						m_Influence;			// 势力(玩家只能是Influence_None)
	U32								m_Level;				// 等级
	U32								m_Exp;					// 经验
	U32								m_Classes[MaxClassesMasters];	// 职业
	S32								m_WuXing[MaxWuXingMasters];		// 五行加成
	GameObjectData::ArmStatus		m_ArmStatus;			// 手持武器状态
	GameObjectData::Animations		m_MountAction;			// 骑乘动作
	bool							m_Walking;				// 是否处于步行状态
	bool							m_Combative;			// 是否参与战斗

	SimObjectPtr<GameObject>		m_pTarget;				// 目标

	TerrainBlock*					m_pCurrentTerrain;		// 当前所在的地图

	// 战斗部分
	BuffTable						m_BuffTable;			// 数值属性 状态列表
	SkillTable						m_SkillTable;			// 技能列表
	Spell							m_Spell;				// 施放技能
	AutoCastTimer					m_AutoCastTimer;		// 施法计时器
	CooldownTable					m_CooldownTable;		// 冷却列表
	bool                            mIsInCombat;			// 是否在战斗状态
	U32								mInvisibleTargetTick;	// 隐身后一定时间内不能被选为目标
	U32								mInvisibleImmunityTick;	// 隐身后一定时间内无敌

	GFXTexHandle					m_BuffTexture;			//头顶文字
	bool                            m_IsDirty;
	S32								mTriggerId;				//触发区域ID
	SimTime							mGameplayDeltaTime;		//处理逻辑的时间间隔
	SimTime							mLastGameplayTime;		//上次处理逻辑的时间

	// New collision
public:
	OrthoBoxConvex mConvex;
	Box3F          mWorkingQueryBox;
	Vector<U32>						mEnterTriggerID;		//记录进入的触发器
#ifdef NTJ_CLIENT
	bool                            mChatDirty;             // 头顶冒泡信息标记
	SimTime                         mChatLastUpdateTime;    // 头顶信息更新时间
	GFXTexHandle					mBuffChatTexture;	    // 头顶冒泡

    enum FontColor
    {
        RED = 0,
        BLUE,
        YELLOW,
        GREEN,       
        ORANGE,
        PINK_P,
        GREEN_P,
        COLORMAX,
    };
    static GFXTexHandle	  mNumTex[COLORMAX];	   //伤害数字贴图

    enum FontText
    {
        ECHO = 0,
        DODGE,
        REBOUND,
        IMMUNE,       
        MISS,
        ABSORB,
        TEXTMAX,
    };
    static GFXTexHandle	  mTextTex[TEXTMAX];	   //战斗文字贴图

    enum FontGain
    {
        EXP = 0,
        EXP_N,
        CREDIT,
        CREDIT_N,       
        GAINMAX,
    };
    static GFXTexHandle	  mGainTex[GAINMAX];	   //经验荣誉贴图


	// 渲染伤害内容数据
	struct stDamageNum
	{
		U32	                        id;		
		char*                     text;			// 数字值或文本值
		bool                      zoom;			// 是否扩大显示
		U32	                       pos;			// 0-左边显示 1-右边显示 2-中央显示 3-经验荣誉值专用
		U32                      Alpha;			// 字体Alpha
        FontColor               eColor;         // 数字字体颜色枚举
		~stDamageNum() { if(text) delete[] text;}
	};

    struct stDamageText
    {
       GFXTexHandle *ptextText;
       U32	                id;
       U32               Alpha;	
    };

    struct stExpText
    {        
        U32	                id;
        U32              Alpha;
        bool              flag;               //0-Exp 1-Credit
        U32                Num;
        stExpText(){Alpha = 0;}
    };

	U32								mDelayShowTime; //延迟时间
    Vector<stDamageNum*>			mDamageNumList;
    std::list<stDamageText>		    mDamageTextList;
    stExpText                       mExpOrCreText[2];
	
    void LoadImagetoTex();
	void addDamageNumber(S32 number, U32 colorEnum, U32 pos, bool zoom = false);
	void addDamageText(GFXTexHandle* Tex);
    void addExpText(bool flag, U32 number);
    void drawDamageInfo();
    void drawNumTex(char* text ,Point2I Pos,FontColor ColorEnum , bool Zoom , int Alpha);
    void drawTextTex(GFXTexHandle* pTex , Point2I Pos , U32 Alpha ,F32 ZoomRate);
    void drawExpTex(bool flag , U32 Number , Point2I Pos , U32 Alpha ,F32 ZoomRate);

	//头顶冒泡
	inline bool    getChatDirty        ();
	inline void    setChatDirty        (bool value);
	inline char*   getChatInfo         ();
	inline void    clearChatInfo       ();
	virtual void   drawObjectWord      (void *){};
#endif//NTJ_CLIENT

	//----------------------------------------------------------------------------
	// AI
#ifndef NTJ_EDITOR
	AIBase*			m_pAI;
#endif
#ifdef NTJ_SERVER
	HateList		mHateList;
#endif

protected:
	void setState(ActionState state, U32 ticks=0);
	void updateState();

	// Jetting
	bool mJetting;

	///Update the movement
	void updateMove(const Move *move);
	///Interpolate movement
	bool updatePos(const F32 travelTime = TickSec);
	///Update head animation
	void updateLookAnimation(F32 dT = 0.f);
	///Update other animations
	void updateAnimation(F32 dt);
	void updateAnimationTree(bool firstPerson);
	bool step(Point3F *pos,F32 *maxStep,F32 time);
	///See if the player is still in the mission area
	void checkMissionArea();

	virtual void refreshImageAction(U32 imageSlot);
	virtual void setActionThread(U32 action,bool forward,bool hold = false,bool wait = false,bool fsp = false, bool forceSet = false);
	virtual void updateActionThread();
	virtual void updateActionTriggerState();
	virtual void pickActionAnimation(bool force = false);
	void onMount(ShapeBase* obj,StringTableEntry node);
	void onUnmount(ShapeBase* obj,StringTableEntry node);

	void setRenderPosition(const Point3F& pos,const Point3F& viewRot,F32 dt=-1);
	void findContact(bool* run,bool* jump,VectorF* contactNormal);
	/// Set which client is controlling this player
	void setControllingClient(GameConnection* client);

	void calcClassRenderData();
	/// Play a footstep sound
	void playFootstepSound(bool triggeredLeft, S32 sound);
	/// Play an impact sound
	void playImpactSound();

	/// Are we in the process of dying?
	bool inDeathAnim();
	F32  deathDelta(Point3F &delta);
	void updateDeathOffsets();
	bool inSittingAnim();

	/// @name Water
	/// @{

	void updateSplash();                             ///< Update the splash effect
	void updateFroth( F32 dt );                      ///< Update any froth
	void updateWaterSounds( F32 dt );                ///< Update water sounds
	void createSplash( Point3F &pos, F32 speed );    ///< Creates a splash
	bool collidingWithWater( Point3F &waterHeight ); ///< Are we collising with water?
	/// @}


public:
	DECLARE_CONOBJECT(GameObject);

	GameObject();
	~GameObject();
	static void consoleInit();

	/// @name Transforms
	/// Transforms are all in object space
	/// @{
	void setPosition(const Point3F& pos,const Point3F& viewRot);
	void setRotation(F32 rotZ, bool bCameraDragged = false);
	void setTransform(const MatrixF &mat);
	void setTransform(const Point3F& pos,const QuatF& q);
	void setRenderTransform(const Point3F& pos,const QuatF& q);
	void getEyeTransform(MatrixF* mat);
	void getRenderEyeTransform(MatrixF* mat);
	void getCameraParameters(F32 *min, F32 *max, Point3F *offset, MatrixF *rot);
	void getCameraTransform(F32* pos,MatrixF* mat);
	/// @}

	inline U32 getGameObjectMask();
	Point3F getVelocity() const;
	void setVelocity(const VectorF& vel);
	/// Apply an impulse at the given point, with magnitude/direction of vec
	void applyImpulse(const Point3F& pos,const VectorF& vec);
	/// Get the rotation of the player
	const Point3F& getRotation() { return mRot; }
	F32 getRenderRotationZ() { return mRot.z + mRenderRot; }
	/// Get the rotation of the head of the player
	//const Point3F& getHeadRotation() { return mHead; }
	void getDamageLocation(const Point3F& in_rPos, const char *&out_rpVert, const char *&out_rpQuad);

	bool  canJump();                                         ///< Can the player jump?
	bool  canJetJump();                                       ///< Can the player jet?
	bool  haveContact()     {return !mContactTimer;}         ///< Is it in contact with something
	/// duh
	float getMaxForwardVelocity() { return (mDataBlock != NULL ? mDataBlock->maxForwardSpeed : 0); }

	virtual bool    isDisplacable() const;
	virtual Point3F getMomentum() const;
	virtual void    setMomentum(const Point3F &momentum);
	virtual F32     getMass() const;
	virtual bool    displaceObject(const Point3F& displaceVector);
	virtual bool    getAIMove(Move*);

	bool checkDismountPosition(const MatrixF& oldPos, const MatrixF& newPos);  ///< Is it safe to dismount here?

	static void initPersistFields();

	//
	bool onAdd();
	void onRemove();
	bool onNewDataBlock(GameBaseData* dptr);
	bool onChangeDataBlock(GameBaseData* dptr);
	void onScaleChanged();
	Box3F mScaledBox;

	// chat
	void say(StringTableEntry str);

	// 单独处理触发器
#ifdef NTJ_CLIENT
	void updateTrigger();
#endif

	// Animation
	const char* getStateName();
	bool setActionThread(const char* sequence,bool hold,bool wait,bool fsp = false);

	// Object control
	void setControlObject(ShapeBase *obj);
	ShapeBase* getControlObject();
	bool isControllable();

	//绘制
	virtual void drawObjectInfo(GameObject *,void *,void *){};
	virtual void drawHpBar(GameObject *,void *,void *);
	void setDirty(bool isDirty){ m_IsDirty = isDirty; }

	void PrepVisibility();

	//
	virtual bool collideBox(const Point3F &start, const Point3F &end, RayInfo* info);
	void updateWorkingCollisionSet();
	void processTick(const Move *move);
	void interpolateTick(F32 delta);
	void advanceTime(F32 dt);
	void beginAdvanceTime(F32 dt);
	bool castRay(const Point3F &start, const Point3F &end, RayInfo* info);
	bool buildPolyList(AbstractPolyList* polyList, const Box3F &box, const SphereF &sphere);
	void buildConvex(const Box3F& box, Convex* convex);
	bool isControlObject();

	void onCameraScopeQuery(NetConnection *cr, CameraScopeQuery *);
	void writePacketData(GameConnection *conn, BitStream *stream);
	void readPacketData (GameConnection *conn, BitStream *stream);
	U64  packUpdate  (NetConnection *conn, U64 mask, BitStream *stream);
	void unpackUpdate(NetConnection *conn,           BitStream *stream);

	virtual void OnStatsChanged() {}

	void findContact_Grid(bool* run,bool* jump,VectorF* contactNormal);
	void updateMove_Grid(const Move* move, bool updateDelta = true);					//只根据网格移动
	bool updatePos_Grid(const F32 travelTime = TickSec);	//只根据网格移动

	void SetPosToMove(Move* move, Point3F& delta);
	void NoSimulateMove (const Move* move);					//没有同步模拟的移动
	bool CheckMoveSpeed (const Move* move, const F32 dt);	//服务器端检测移动速度
	void UpdateCurrentTerrain();		
	TerrainBlock* GetCurrentTerrain() { return m_pCurrentTerrain;}

	bool			CanSetTarget(SceneObject* obj);
	GameObject*		GetTarget()						{ return m_pTarget; }
	bool			SetTarget(SceneObject* obj, bool atk = true, U32 mask = 0);
	bool			SetTarget(Point3F& pos, U32 mask = 0);

	virtual bool	CanAttackTarget(GameObject* obj);
	bool			isCombatArea(bool pk);

	F32				GetFollowRange();
	F32				GetRangeMin(U32 skillId);
	F32				GetRangeMax(U32 skillId);

	// 动作相关函数
	virtual bool updateShape();
	virtual bool isRenderImage(U32 imageSlot);
	virtual bool isEdgeBlurPause();
	virtual void setArmStatus(GameObjectData::ArmStatus arm);
	inline GameObjectData::ArmStatus getArmStatus();
	inline void setMountAction(GameObjectData::Animations mountAction);
	inline GameObjectData::Animations getMountAction();
	inline U32 getRootAnim();
	GameObjectData::Animations getAnim(GameObjectData::Animations action);
	inline void getBuffAnim(U32& action);
	void getMountAnim(U32& action);
	void getScheduleAnim(U32& action);
	void onWound();
	inline bool isWalking() const { return m_Walking;}
	inline void setWalking(bool val) { m_Walking = val;}
	inline bool isCombative() const { return m_Combative;}
	inline void setCombative(bool val) { m_Combative = val;}
	bool cullCastAnim();
	inline U32 getUpperAction() { return mUpperActionAnimation.action;}
	inline bool isUpperAtEnd() { return mUpperActionAnimation.atEnd;}
	inline bool isSameUpper(U32 action) { return mUpperActionAnimation.action == getAnim((GameObjectData::Animations)action);}

	inline void clearAnimateEP() { mSkillDataEP = 0; mSelectableEP = SkillData::Selectable_A; mTargetObjEP.clear(); mDestEP.zero(); mTargetIsObj = true;}
	inline void setSkillDataEP(U32 skillId, SkillData::Selectable sel) { mSkillDataEP = skillId; mSelectableEP = sel;}
	inline void addTargetObjEP(U32 simId) { mTargetObjEP.push_back(simId); mTargetIsObj = true;}
	inline void setDestEP(Point3F& pos) { mDestEP.set(pos); mTargetIsObj = false;}
	void packAnimateEP(NetConnection *conn, U64 mask, BitStream *stream);
	void unpackAnimateEP(NetConnection *conn, BitStream *stream);
	void applyAnimateEP();

	// 逻辑相关
	void processTick_Gameplay();
	bool IsFaceTo(Point3F& pos);
	Point3F& getPetPos();
	void onEnabled();
	void onDisabled();
	void onInvisible();

	inline U32 getSex() { return m_Sex;}
	inline U32 getRace() { return m_Race;}
	inline U32 getFamily() { return m_Family;}
	inline U32 getLevel() { return m_Level;}
	inline U32 getExp() { return m_Exp;}
	inline U32 getClasses(U32 val) { AssertFatal(val < MaxClassesMasters,""); return m_Classes[val];}
	inline S32 getWuXing(U32 val) { if(val >= MaxWuXingMasters) return 0; return *((&(m_BuffTable.GetStats().Mu)) + val);}
	inline enInfluence getInfluence() { return m_Influence;}

	inline void setSex(enSex val) { m_Sex = val;}
	inline void setRace(enRace val) { m_Race = val;setMaskBits(BaseInfoMask);}
	inline void setFamily(enFamily val);
	virtual void addLevel(S32 level) {}
	virtual void addExp(S32 exp) {}
	void setClasses(U32 val, U32 newClass, bool force = false);
	void setInfluence(enInfluence infl);

	inline F32 getMoveSpeed();
	inline bool uncontrollableMove();
	bool isImmobilityBuff();
	inline bool isSilenceBuff();
	inline bool isCanNotAttackBuff();

	bool calculateJumpPath(Point3F& end);
	bool setJumpPath(Point3F& end);
	void clearJumpPath();
	void updateJumpPath(F32 dt);
	void interpolateJumpPath(Point3F& pos, Point3F& rot, F32 dt);

	bool setFlyPath(U32 pathId, U32 mountId, U32 flyPos = 0);
	void clearFlyPath();
	void updateFlyPath(F32 dt);
	void interpolateFlyPath(Point3F& pos, Point3F& rot, F32 dt);

	bool setBlink(Point3F& end);
	bool isPackWholeStats(NetConnection* conn);
	//在触发器记录里查找，是否已经进入过
	bool findTriggerID(U32 triggerID);

	virtual StringTableEntry getObjectIcon() { return "";}			//获取对象头像图标

	//----------------------------------------------------------------------------
	// 战斗相关

	virtual void CalcStats();
	void RefreshGui();
	inline void SetRefreshGui(bool val);
	inline void SetRefreshStats(bool val);
	inline const BuffTable& GetBuffTable() { return m_BuffTable;}
	inline const Stats& GetStats() { return m_BuffTable.GetStats();}
	inline void onBuffTrigger(U32 mask, GameObject* trigger) { m_BuffTable.onTrigger(mask, trigger);}

	bool AddBuff(Buff::Origin _origin, U32 _buffId, GameObject* _src, S32 _count = 1);
	bool RemoveBuff(U32 _buffId);
	bool RemoveBuffById(U32 _buffId, U32 _count = 1, U32 mask = Buff::Mask_Remove_Sys, bool seriesId = false);
	bool RemoveBuffBySrc(U32 _buffId, GameObject* src, U32 _count = 1, U32 mask = Buff::Mask_Remove_Sys, bool seriesId = false);
	bool HandleRemoveRequest(U32 _buffId, U32 index);

	inline void setHP(S32 val) { if(GetStats().HP != val) {m_BuffTable.setHP(val); setMaskBits(HPMask);}}
	inline void setMP(S32 val) { if(GetStats().MP != val) {m_BuffTable.setMP(val); setMaskBits(MPMask);}}
	inline void setPP(S32 val) { if(GetStats().PP != val) {m_BuffTable.setPP(val); setMaskBits(PPMask);}}
	inline void addHP(S32 val) { if(val != 0) {m_BuffTable.addHP(val); setMaskBits(HPMask);}}
	inline void addMP(S32 val) { if(val != 0) {m_BuffTable.addMP(val); setMaskBits(MPMask);}}
	inline void addPP(S32 val) { if(val != 0) {m_BuffTable.addPP(val); setMaskBits(PPMask);}}

	virtual Skill* AddSkill(U32 skillId, bool updateToClient = true);
	virtual void RemoveSkill(U32 skillId);
	inline SkillTable& GetSkillTable() { return m_SkillTable;}

	void UpdateAutoCast(GameObjectData::Animations action);
	void SetAutoCastDelay(SimTime delay);
	void SetAutoCastEnable(bool enable);
	inline bool IsAutoCastEnable() { return m_AutoCastTimer.GetEnable();}
	inline bool isInCombat() { return mIsInCombat;}
	inline void setInCombat(bool val, GameObject* pObj = NULL);

	Spell& GetSpell() {return m_Spell;}
	enWarnMessage CanCast(U32 skillId, GameObject* obj, const Point3F* pos, bool clientAI = false);
	enWarnMessage CanCast(SkillData* pData, GameObject* obj, const Point3F* pos, bool clientAI = false);
	bool CheckCastCooldown(SkillData* pData) { return pData ? !(bool)m_CooldownTable.GetCooldown(pData->m_Cast.cdGroup, pData->IsFlags(SkillData::Flags_IgnoreCDGlobal)) : false;}
	bool CastSpell(U32 skillId, GameObject& obj, U32 itemType = 0, U32 itemIdx = 0, U64 UID = 0);
	bool CastSpell(U32 skillId, Point3F& pos, U32 itemType = 0, U32 itemIdx = 0, U64 UID = 0);
	bool AI_CastSpell(U32 skillId, GameObject* obj, const Point3F* pos, U32 itemType = 0, U32 itemIdx = 0, U64 UID = 0);
	bool PreCastSpell(U32 skillId, GameObject* obj, const Point3F* pos, U32 itemType = 0, U32 itemIdx = 0, U64 UID = 0);
	bool PreCastSpellDone(GameObject* obj, const Point3F* pos);
	void BreakCast();
	void ProcessCast(S32 dt);

	S32 DamageReduce(const Stats& stats, U32 damType, U32 buffEff, F32 scale, S32& reduce);
	void ApplyDamage(const Stats& stats, GameObject* pSource, U32 buffId);
	bool HitFilter(GameObject* pSource, const Stats& stats, bool sendLog = false);
	bool CriticalFilter(GameObject* pSource, const Stats& stats);
	virtual void onDamage(S32 dam,GameObject* pSource);
	virtual bool canTame() const { return false;}
	virtual bool isDamageable() const;
	void onReduce(S32 dam, S32 reduce, GameObject* pSource);
	virtual void onCombatBegin(GameObject* pObj);
	virtual void onCombatEnd(GameObject* pObj);

	inline void SetCooldown_Global() { m_CooldownTable.SetCooldown_Global();}
	inline void SetCooldown_Group(S32 cdGroup, U32 cdTime, U32 totalTime);
	inline bool GetCooldown(S32 cdGroup, U32& cdTime, U32& totalTime, bool ignoreGlobal = false);
	F32 GetDistance(GameObject* obj);
	
	void setTriggerID(S32 triggerID);			
	S32	 getTriggerId(){ return mTriggerId; }

	//----------------------------------------------------------------------------
	// AI
#ifndef NTJ_EDITOR
	virtual void AddHate	(GameObject* pObject, S32 nHate) { }
#endif

};


#include "Gameplay/GameObjects/GameObject_inline.h"
#endif