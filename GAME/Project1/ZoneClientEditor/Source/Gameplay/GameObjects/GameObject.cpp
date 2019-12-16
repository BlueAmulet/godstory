//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#pragma warning(disable: 4355) // “this”: 用于基成员初始值设定项列表

#include "Gameplay/GameObjects/GameObject.h"

#include "platform/platform.h"
#include "platform/profiler.h"
#include "math/mMath.h"
#include "core/stringTable.h"
#include "core/bitStream.h"
#include "core/dnet.h"
#include "core/resManager.h"
#include "console/simBase.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "collision/extrudedPolyList.h"
#include "collision/clippedPolyList.h"
#include "collision/earlyOutPolyList.h"
#include "ts/tsShapeInstance.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/detailManager.h"
#include "terrain/terrData.h"
#include "terrain/terrRender.h"
#include "terrain/waterBlock.h"
#include "app/game.h"
#include "T3D/gameConnection.h"
#include "T3D/trigger.h"
#include "T3D/physicalZone.h"
#include "T3D/item.h"
#include "T3D/missionArea.h"
#include "T3D/fx/particleEmitter.h"
#include "T3D/fx/cameraFXMgr.h"
#include "T3D/fx/splash.h"
#include "T3D/tsStatic.h"
#include "sceneGraph/decalManager.h"
#include "ts/TSShapeRepository.h"
#include "Gameplay/ai/NaviGrid/NavigationBlock.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "math/mathUtils.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "effects/EffectPacket.h"
#include "util/aniThread.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/ScheduleManager.h"
#include "sceneGraph/pathManager.h"
#include "Gameplay/GameObjects/PetMountOperation.h"
#include "Gameplay/GameObjects/NpcObject.h"

#ifdef NTJ_SOUND
#include "sfx/sfxSystem.h"
#endif

#ifdef NTJ_CLIENT

#include "ui/dGuiMouseGamePlay.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/LoadMgr/ObjectLoadManager.h"
#include "gui/core/guiTypes.h"
#include "Gameplay/Team/ClientTeam.h"
#include "gfx/gfxFontRenderBatcher.h"

#define REDNUM				    "gameres/gui/images/r_font.dds"
#define BLUENUM				    "gameres/gui/images/b_font.dds"
#define YELLOWNUM				"gameres/gui/images/y_font.dds"
#define GREENNUM				"gameres/gui/images/g_font.dds"
#define ORANGENUM				"gameres/gui/images/o_font.dds"
#define GREEN_PNUM				"gameres/gui/images/gp_font.dds"
#define PINK_PNUM				"gameres/gui/images/pp_font.dds"


#define ECHOTEX 				"gameres/gui/images/Font_fs.dds"
#define DODGETEX 				"gameres/gui/images/Font_sb.dds"
#define REBOUNDTEX 				"gameres/gui/images/Font_ft.dds"
#define IMMUNETEX 				"gameres/gui/images/Font_my.dds"
#define MISSTEX 				"gameres/gui/images/Font_wjz.dds"
#define ABSORBTEX 				"gameres/gui/images/Font_xs.dds"

#define EXPTEX 				    "gameres/gui/images/Font_jy.dds"
#define EXP_NTEX 				"gameres/gui/images/Font_jyn.dds"
#define CREDITTEX 			    "gameres/gui/images/Font_sw.dds"
#define CREDIT_NTEX 			"gameres/gui/images/Font_swn.dds"


#define DamageNumMAX 5
#define DamageTexMAX 1
#define OffsetY 45

GFXTexHandle GameObject::mNumTex[COLORMAX] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL};
GFXTexHandle GameObject::mTextTex[TEXTMAX] = {NULL,NULL,NULL,NULL,NULL,NULL};
GFXTexHandle GameObject::mGainTex[GAINMAX] = {NULL,NULL,NULL,NULL};
#endif

#ifdef NTJ_EDITOR
#include "ui/dWorldEditor.h"
#endif

//----------------------------------------------------------------------------

// Amount we try to stay out of walls by...
static F32 sWeaponPushBack = 0.03f;

// Amount of time if takes to transition to a new action sequence.
static F32 sAnimationTransitionTime = 0.25f;
static bool sUseAnimationTransitions = true;
static F32 sLandReverseScale = 0.25f;
static F32 sStandingJumpSpeed = 2.0f;
static F32 sJumpingThreshold = 4.0f;
static F32 sSlowStandThreshSquared = 1.69f;
static S32 sRenderMyPlayer = true;
static S32 sRenderMyItems = true;

// Chooses new action animations every n ticks.
static const F32 sNewAnimationTickTime = 4.0f;
static const F32 sMountPendingTickWait = 13.0f * F32(TickMs);

// Number of ticks before we pick non-contact animations
static const S32 sContactTickTime = 30;

// Downward velocity at which we consider the player falling
static const F32 sFallingThreshold = -10.0f;

// Movement constants
static F32 sVerticalStepDot = 0.173f;   // 80
static F32 sMinFaceDistance = 0.01f;
static F32 sTractionDistance = 0.03f;
static F32 sNormalElasticity = 0.01f;
static U32 sMoveRetryCount = 5;

// Client prediction
static F32 sMinWarpTicks = 0.5f;        // Fraction of tick at which instant warp occures
static S32 sMaxWarpTicks = 32;          // Max warp duration in ticks
static S32 sMaxWarpTicks_Rot = 3;       // Max warp duration in ticks
static S32 sMaxPredictionTicks = 30;   // Number of ticks to predict

// Anchor point compression
const F32 sAnchorMaxDistance = 32.0f;

//
static U32 sCollisionMoveMask = (AtlasObjectType        | TerrainObjectType    |
								 InteriorObjectType     |
								 WaterObjectType        | /*GameObjectType     |*/
								 StaticShapeObjectType  | VehicleObjectType    |
								 PhysicalZoneObjectType | StaticTSObjectType);

#ifdef NTJ_SERVER
static U32 sServerCollisionContactMask = (TerrainObjectType |
										TriggerObjectType);
#else
static U32 sServerCollisionContactMask = (sCollisionMoveMask |
										  (ItemObjectType    |
										  TriggerObjectType |
										  CorpseObjectType));
#endif

static U32 sClientCollisionContactMask = sServerCollisionContactMask | WaterObjectType;

enum PlayerConstants {
	JumpSkipContactsMax = 8
};

//----------------------------------------------------------------------------
// GameObject shape animation sequences:

// look     Used to contol the upper body arm motion.  Must animate
//          vertically +-80 deg.
GameObject::Range GameObject::mArmRange(mDegToRad(-80.0f),mDegToRad(+80.0f));

// head     Used to control the direction the head is looking.  Must
//          animated vertically +-80 deg .
GameObject::Range GameObject::mHeadVRange(mDegToRad(-80.0f),mDegToRad(+80.0f));


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(GameObject);
F32 GameObject::mGravity = -10;


//----------------------------------------------------------------------------

GameObject::GameObject() : m_SkillTable(this)
{
	mTypeMask |= GameObjectType;
	mGameObjectMask = 0;

	delta.pos = mAnchorPoint = Point3F(0,0,100);
	delta.rot = delta.head = Point3F(0,0,0);
	delta.rotOffset.set(0.0f,0.0f,0.0f);
	delta.warpOffset.set(0.0f,0.0f,0.0f);
	delta.posVec.set(0.0f,0.0f,0.0f);
	delta.rotVec.set(0.0f,0.0f,0.0f);
	delta.headVec.set(0.0f,0.0f,0.0f);
	delta.warpTicks = 0;
	delta.dt = 1.0f;
	delta.move = NullMove;
	mPredictionCount = sMaxPredictionTicks;
	mObjToWorld.setColumn(3,delta.pos);
	mRenderRot = 0;
	mRot = delta.rot;
	#pragma message(ENGINE(删除mHead变量))
	//mHead = delta.head;
	mVelocity.set(0.0f, 0.0f, 0.0f);
	mDataBlock = 0;
	mLowerActionAnimation.action = GameObjectData::NullAnimation;
	mLowerActionAnimation.thread = 0;
	mLowerActionAnimation.endTime = 0;
	mLowerActionAnimation.forward = true;
	mLowerActionAnimation.firstPerson = false;
	//mLowerActionAnimation.time = 1.0f; //ActionAnimation::Scale;
	mLowerActionAnimation.waitForEnd = false;
	mLowerActionAnimation.holdAtEnd = false;
	mLowerActionAnimation.animateOnServer = false;
	mLowerActionAnimation.atEnd = false;
	mUpperActionAnimation.action = GameObjectData::NullAnimation;
	mUpperActionAnimation.thread = 0;
	mUpperActionAnimation.endTime = 0;
	mUpperActionAnimation.forward = true;
	mUpperActionAnimation.firstPerson = false;
	//mUpperActionAnimation.time = 1.0f; //ActionAnimation::Scale;
	mUpperActionAnimation.waitForEnd = false;
	mUpperActionAnimation.holdAtEnd = false;
	mUpperActionAnimation.animateOnServer = false;
	mUpperActionAnimation.atEnd = false;
	mWoundThread = NULL;
	clearAnimateEP();
	mState = MoveState;
	mFalling = false;
	mContactTimer = 0;
	mJumpDelay = 0;
	mJumpSurfaceLastContact = 0;
	mJumpSurfaceNormal.set(0.0f, 0.0f, 1.0f);
	mControlObject = 0;
	dMemset( mSplashEmitter, 0, sizeof( mSplashEmitter ) );

	mImpactSound = 0;
	mRecoverTicks = 0;
	mReversePending = 0;

	mLastPos.set( 0.0f, 0.0f, 0.0f );
	mPrevRenderPos.set( 0.0f, 0.0f, 0.0f );
	mCurrRenderPos.set( 0.0f, 0.0f, 0.0f );
	interpolateFlag = false;
	debugFlag = false;

	mMoveBubbleSound = 0;
	mWaterBreathSound = 0;
	inLiquid = false;

	mConvex.init(this);
	mWorkingQueryBox.min.set(-1e9f, -1e9f, -1e9f);
	mWorkingQueryBox.max.set(-1e9f, -1e9f, -1e9f);

	mWeaponBackFraction = 0.0f;

	mInMissionArea = true;

	mBubbleEmitterTime = 10.0;
	mLastWaterPos.set( 0.0, 0.0, 0.0 );

	mMountPending = 0;

	mNSLinkMask = LinkSuperClassName | LinkClassName;

	mCheckVelocity_1 = 0;
	mServerOldSpeed = 0;
	mServerSpeedChange = false;
	mServerSynCount = 0;
	m_bMoving = false;

	m_BodyShape = NULL;
	m_FaceShape = NULL;
	m_HairShape = NULL;

	m_Sex = SEX_MALE;
	m_Race = Race_Ren;
	m_Family = Family_None;
	m_Influence = Influence_Npc;
	m_Level = 1;
	m_Exp = 0;
	for(S32 i=0; i<MaxClassesMasters; i++)
		m_Classes[i] = Class_Null;
	for(S32 i=0; i<MaxWuXingMasters; i++)
		m_WuXing[i] = 0;
	m_pTarget = NULL;

	m_pCurrentTerrain = NULL;
	m_ArmStatus = GameObjectData::Arm_A;
	m_MountAction = GameObjectData::Mount_a;
	m_Walking = false;
	m_Combative = false;
	m_IsDirty = true;
	mTriggerId = 0;
	clearJumpPath();
	mFlyPath.pathId = 0;
	mFlyPath.flyPos = 0;
	mFlyPath.atEnd = false;
	mFlyPath.mountId = 0;
	mBlink = false;
	mGameplayDeltaTime = 0;
	mLastGameplayTime = 0;
	mIsInCombat = false;
	mInvisibleTargetTick = 0;
	mInvisibleImmunityTick = 0;

	dMemset(&mChatInfo,0,sizeof(mChatInfo));

	m_Spell.SetSource(this);
	m_BuffTable.SetObj(this);
	//初始时清空
	mEnterTriggerID.clear();
	// AI
#ifndef NTJ_EDITOR
	m_pAI = NULL;
#endif

#ifdef NTJ_SERVER
	mHateList.init(this);
#endif

#ifdef NTJ_CLIENT
	mChatDirty = false;
	mChatLastUpdateTime = 0;   
#endif
}

GameObject::~GameObject()
{
#ifdef NTJ_CLIENT
    for (int i = 0 ; i < COLORMAX ; ++i)
    {
        mNumTex[i] = NULL;
    }
    for (int i = 0 ; i < TEXTMAX ; ++i)
    {
        mTextTex[i] = NULL;
    }
    for (int i = 0 ; i < GAINMAX ; ++i)
    {
        mGainTex[i] = NULL;
    }
#endif
	mTriggerId = 0;

#ifndef NTJ_EDITOR
	if (m_pAI)
		delete m_pAI;
#endif
}

void GameObject::initPersistFields()
{
	Parent::initPersistFields();

	removeField("datablock");  //Ray: 使用datablockname
}
//----------------------------------------------------------------------------

bool GameObject::onAdd()
{
	ActionAnimation serverAnim = mLowerActionAnimation;
	if(!Parent::onAdd() || !mDataBlock)
		return false;
	getTransform().getColumn(1,&mRot);
	mRot.set(0.0f,0.0f,-mAtan(-mRot.x,mRot.y));

	mWorkingQueryBox.min.set(-1e9f, -1e9f, -1e9f);
	mWorkingQueryBox.max.set(-1e9f, -1e9f, -1e9f);

	NetConnection* pConn = getNetConnection();
	if( !( pConn && pConn->isRobotConnection() ) )
		addToScene();

	// Make sure any state and animation passed from the server
	// in the initial update is set correctly.
	ActionState state = mState;
	mState = NullState;
	setState(state);
	if (serverAnim.action != GameObjectData::NullAnimation)
	{
		setActionThread(serverAnim.action, true, serverAnim.holdAtEnd, true, false, true);
		if (serverAnim.atEnd)
		{
			mShapeInstance->clearTransition(mLowerActionAnimation.thread);
			mShapeInstance->setPos(mLowerActionAnimation.thread,
				mLowerActionAnimation.forward ? 1.0f : 0.0f);
			if (inDeathAnim())
				mDeath.lastPos = 1.0f;
		}

		// We have to leave them sitting for a while since mounts don't come through right
		// away (and sometimes not for a while).  Still going to let this time out because
		// I'm not sure if we're guaranteed another anim will come through and cancel.
		if (!isServerObject() && inSittingAnim())
			mMountPending = (S32) sMountPendingTickWait;
		else
			mMountPending = 0;
	}

	//
	if (isServerObject())
	{
		scriptOnAdd();
	}
	else
	{
		U32 i;
		for( i=0; i<GameObjectData::NUM_SPLASH_EMITTERS; i++ )
		{
			if ( mDataBlock->splashEmitterList[i] ) 
			{
				mSplashEmitter[i] = new ParticleEmitter;
				mSplashEmitter[i]->onNewDataBlock( mDataBlock->splashEmitterList[i] );
				if( !mSplashEmitter[i]->registerObject() )
				{
					Con::warnf( ConsoleLogEntry::General, "Could not register splash emitter for class: %s", mDataBlock->getName() );
					delete mSplashEmitter[i];
					mSplashEmitter[i] = NULL;
				}
			}
		}
		mLastWaterPos = getPosition();

		// clear out all camera effects
		gCamFXMgr.clear();
	}

	m_Spell.SetSource(this);
	m_BuffTable.SetObj(this);

	return true;
}

void GameObject::onRemove()
{
	setControlObject(0);
	scriptOnRemove();
	removeFromScene();

	U32 i;
	for( i=0; i<GameObjectData::NUM_SPLASH_EMITTERS; i++ )
	{
		if( mSplashEmitter[i] )
		{
			mSplashEmitter[i]->deleteWhenEmpty();
			mSplashEmitter[i] = NULL;
		}
	}

	mWorkingQueryBox.min.set(-1e9f, -1e9f, -1e9f);
	mWorkingQueryBox.max.set(-1e9f, -1e9f, -1e9f);

	Parent::onRemove();
}

void GameObject::onScaleChanged()
{
	// 主动去碰撞其他物体的box
	mScaledBox.min.set(-0.1f,-0.1f,0);
	mScaledBox.max.set(0.1f,0.1f,1);
}


//----------------------------------------------------------------------------

bool GameObject::onNewDataBlock(GameBaseData* dptr)
{
	GameObjectData* prevData = mDataBlock;
	mDataBlock = dynamic_cast<GameObjectData*>(dptr);
	if (!mDataBlock || !Parent::onNewDataBlock(dptr))
		return false;

	if ( isGhost() )
	{
		// Create the sounds ahead of time.  This reduces runtime
		// costs and makes the system easier to understand.

		SFX_DELETE( mMoveBubbleSound );
		SFX_DELETE( mWaterBreathSound );

		if ( mDataBlock->sound[GameObjectData::MoveBubbles] )
			mMoveBubbleSound = SFX->createSource( mDataBlock->sound[GameObjectData::MoveBubbles] );

		if ( mDataBlock->sound[GameObjectData::WaterBreath] )
			mWaterBreathSound = SFX->createSource( mDataBlock->sound[GameObjectData::WaterBreath] );
	}
	else
	{
		// 缩放
		if (mDataBlock->objScale.x > POINT_EPSILON && mDataBlock->objScale.y > POINT_EPSILON && mDataBlock->objScale.z > POINT_EPSILON)
			mObjScale *= mDataBlock->objScale;
	}

	// 使用美术制作的box
	//mObjBox.max.x = mDataBlock->boxSize.x * 0.5f;
	//mObjBox.max.y = mDataBlock->boxSize.y * 0.5f;
	//mObjBox.max.z = mDataBlock->boxSize.z;
	//mObjBox.min.x = -mObjBox.max.x;
	//mObjBox.min.y = -mObjBox.max.y;
	//mObjBox.min.z = 0.0f;

	// Setup the box for our convex object...
	mObjBox.getCenter(&mConvex.mCenter);
	mConvex.mSize.x = mObjBox.len_x() / 2.0f;
	mConvex.mSize.y = mObjBox.len_y() / 2.0f;
	mConvex.mSize.z = mObjBox.len_z() / 2.0f;

	// Initialize our scaled attributes as well
	onScaleChanged();

	scriptOnNewDataBlock();
	return true;
}

bool GameObject::onChangeDataBlock(GameBaseData* dptr)
{
	// 记录动作信息
	F32 UpperPos = mShapeInstance->getSeqPos(mUpperActionAnimation.thread);
	F32 LowerPos = mShapeInstance->getSeqPos(mLowerActionAnimation.thread);
	//F32 WoundPos = mShapeInstance->getSeqPos(mWoundThread);

	mDataBlock = dynamic_cast<GameObjectData*>(dptr);
	if (!mDataBlock || !Parent::onChangeDataBlock(dptr))
		AssertRelease(false, "GameObject::onChangeDataBlock");

	// 保持原来的动作
	mUpperActionAnimation.thread = mShapeInstance->addThread();
	mShapeInstance->setEnableSubShape(mUpperActionAnimation.thread,TSThread::Upper);
	mLowerActionAnimation.thread = mShapeInstance->addThread();
	mShapeInstance->setEnableSubShape(mLowerActionAnimation.thread,TSThread::Lower);
	S32 seq = g_TSShapeRepository.GetSequence(mDataBlock->shapeName,mUpperActionAnimation.action);
	if(seq != -1)
		mShapeInstance->setSequence(mUpperActionAnimation.thread, seq, UpperPos);
	else
		mUpperActionAnimation.action = GameObjectData::NullAnimation;
	seq = g_TSShapeRepository.GetSequence(mDataBlock->shapeName,mLowerActionAnimation.action);
	if(seq != -1)
		mShapeInstance->setSequence(mLowerActionAnimation.thread, seq, UpperPos);
	else
		mLowerActionAnimation.action = GameObjectData::NullAnimation;
#ifdef NTJ_CLIENT
	// 受伤动作只在客户端作用
	seq = g_TSShapeRepository.GetSequence(mDataBlock->shapeName,GameObjectData::Wound);
	if(seq != -1)
	{
		mWoundThread = mShapeInstance->addThread();
		mShapeInstance->setEnableSubShape(mWoundThread,TSThread::Whole);
		mShapeInstance->setSequence(mWoundThread,seq,0);
		mShapeInstance->setTimeScale(mWoundThread,0);
	}
#endif
	updateAnimationTree(!isGhost());

	// Setup the box for our convex object...
	mObjBox.getCenter(&mConvex.mCenter);
	mConvex.mSize.x = mObjBox.len_x() / 2.0f;
	mConvex.mSize.y = mObjBox.len_y() / 2.0f;
	mConvex.mSize.z = mObjBox.len_z() / 2.0f;

	// Initialize our scaled attributes as well
	onScaleChanged();

	return true;
}

bool GameObject::updateShape()
{
	// 记录动作信息
	F32 UpperPos = mShapeInstance->getSeqPos(mUpperActionAnimation.thread);
	F32 LowerPos = mShapeInstance->getSeqPos(mLowerActionAnimation.thread);

	if(!Parent::updateShape())
		return false;

	if(isClientObject())
	{
		// 保持原来的动作
		mUpperActionAnimation.thread = mShapeInstance->addThread();
		mShapeInstance->setEnableSubShape(mUpperActionAnimation.thread,TSThread::Upper);
		mLowerActionAnimation.thread = mShapeInstance->addThread();
		mShapeInstance->setEnableSubShape(mLowerActionAnimation.thread,TSThread::Lower);
		S32 seq = g_TSShapeRepository.GetSequence(mTSSahpeInfo->m_TSShapeId,mUpperActionAnimation.action);
		if(seq != -1)
			mShapeInstance->setSequence(mUpperActionAnimation.thread, seq, UpperPos);
		else
			mUpperActionAnimation.action = GameObjectData::NullAnimation;
		seq = g_TSShapeRepository.GetSequence(mTSSahpeInfo->m_TSShapeId,mLowerActionAnimation.action);
		if(seq != -1)
			mShapeInstance->setSequence(mLowerActionAnimation.thread, seq, UpperPos);
		else
			mLowerActionAnimation.action = GameObjectData::NullAnimation;
#ifdef NTJ_CLIENT
		// 受伤动作只在客户端作用
		mWoundThread = NULL;
		seq = g_TSShapeRepository.GetSequence(mTSSahpeInfo->m_TSShapeId,GameObjectData::Wound);
		if(seq != -1)
		{
			mWoundThread = mShapeInstance->addThread();
			mShapeInstance->setEnableSubShape(mWoundThread,TSThread::Whole);
			mShapeInstance->setSequence(mWoundThread,seq,0);
			mShapeInstance->setTimeScale(mWoundThread,0);
		}
		if(getControllingClient()){
			g_ClientGameplayState->setPlayerModelView();
			g_ClientGameplayState->refreshPlayerModelView();
		}
#endif
	}
	updateAnimationTree(!isGhost());
	return true;
}

//----------------------------------------------------------------------------

void GameObject::setControllingClient(GameConnection* client)
{
	Parent::setControllingClient(client);
	if (mControlObject)
		mControlObject->setControllingClient(client);
}

void GameObject::setControlObject(ShapeBase* obj)
{
	if (mControlObject) {
		mControlObject->setControllingObject(0);
		mControlObject->setControllingClient(0);
	}
	if (obj == this || obj == 0)
		mControlObject = 0;
	else {
		if (ShapeBase* coo = obj->getControllingObject())
			coo->setControlObject(0);
		if (GameConnection* con = obj->getControllingClient())
			con->setControlObject(0);

		mControlObject = obj;
		mControlObject->setControllingObject(this);
		mControlObject->setControllingClient(getControllingClient());
	}
	setMaskBits(CtrlMask);
}

void GameObject::getCameraTransform(F32* pos,MatrixF* mat)
{
	//Ray: 测试用代码不要删除
	//if(interpolateFlag)
	//{
	//	if(mPrevRenderPos == mLastPos)
	//	{
	//		interpolateFlag = false;
	//		//if(debugFlag)
	//			//Con::printf("设置插值坐标(%0.3f,%0.3f,%0.3f) + (%0.3f,%0.3f,%0.3f)",mPrevRenderPos.x,mPrevRenderPos.y,mPrevRenderPos.z,mCurrRenderStep.x,mCurrRenderStep.y,mCurrRenderStep.z);
	//	}
	//	else
	//	{
	//		//Con::printf("设置插值坐标(%0.3f,%0.3f,%0.3f) + (%0.3f,%0.3f,%0.3f)",mPrevRenderPos.x,mPrevRenderPos.y,mPrevRenderPos.z,mCurrRenderStep.x,mCurrRenderStep.y,mCurrRenderStep.z);
	//		interpolateFlag = MathUtils::interpolateV3((F32 *)&mPrevRenderPos,(F32 *)&mCurrRenderPos,(F32 *)&mCurrRenderStep);
	//		MatrixF mat = getRenderTransform();
	//		mat.setPosition(mPrevRenderPos);
	//		setRenderTransform(mat);
	//		debugFlag = true;
	//	}
	//}

	if(!mControlObject.isNull())
		mControlObject->getCameraTransform(pos, mat);
	else if(mMount.object)
		mMount.object->getCameraTransform(pos, mat);
	else
		Parent::getCameraTransform(pos,mat);
}

void GameObject::onCameraScopeQuery(NetConnection *connection, CameraScopeQuery *query)
{
	// First, we are certainly in scope, and whatever we're riding is too...
	if(mControlObject.isNull() || mControlObject == mMount.object)
		Parent::onCameraScopeQuery(connection, query);
	else
	{
		connection->objectInScope(this);
		if (isMounted())
			connection->objectInScope(mMount.object);
		mControlObject->onCameraScopeQuery(connection, query);
	}
}

ShapeBase* GameObject::getControlObject()
{
	return mControlObject;
}

bool GameObject::isControllable()
{
	return (getObjectMount() && getObjectMount()->getControllingObject() != this);
}


//-----------------------------------------------------------------------------

void GameObject::processTick(const Move* move)
{
	PROFILE_SCOPE(Player_ProcessTick);

	interpolateFlag = true;

	// Manage the control object and filter moves for the player
	Move pMove,*tMove = (Move*)move;
	if (mControlObject) {
		if (!move)
			mControlObject->processTick(0);
		else {
			pMove = NullMove;
			if (isMounted()) {
				// Filter Jump trigger if mounted
				pMove.trigger[2] = move->trigger[2];
			}
			mControlObject->processTick((mDamageState == Enabled)? move: &NullMove);
			move = &pMove;
		}
	}

	Parent::processTick(move);

	// 大部分逻辑在这里处理 [5/6/2009 joy]
	processTick_Gameplay();

	if (mFlyPath.pathId && !getControllingClient())
	{
		updateFlyPath(mGameplayDeltaTime/1000.f);
	}
	// Warp to catch up to server
	else if (delta.warpTicks > 0) {
		delta.warpTicks--;

		// Set new pos.
		getTransform().getColumn(3,&delta.pos);
		delta.pos += delta.warpOffset;
		if(delta.warpRotTicks > 0)
		{
			delta.rot += delta.rotOffset;
			--delta.warpRotTicks;
		}
		if(delta.warpTicks > 0)
		{
			mVelocity = delta.warpOffset;
			mVelocity.normalize(delta.move.mVelocity.len());
		}
		else
		{
			mVelocity = delta.move.mVelocity;
			delta.rot = delta.head;
		}
		setPosition(delta.pos,delta.rot);
		setRenderPosition(delta.pos,delta.rot);
		//updateDeathOffsets();
		updateLookAnimation();

		// Backstepping
		delta.posVec.x = -delta.warpOffset.x;
		delta.posVec.y = -delta.warpOffset.y;
		delta.posVec.z = -delta.warpOffset.z;
		//if(delta.warpRotTicks > 0/*注意这里已经--了*/)
		//{
			delta.rotVec.x = -delta.rotOffset.x;
			delta.rotVec.y = -delta.rotOffset.y;
			delta.rotVec.z = -delta.rotOffset.z;
		//}
		//else
		//	delta.rotVec.zero();
	}
	else {
		// If there is no move, the player is either an
		// unattached player on the server, or a player's
		// client ghost.
		if (!move) {
			if (isGhost()) {
				// If we haven't run out of prediction time,
				// predict using the last known move.
				if (mPredictionCount-- <= 0)
				{
					delta.posVec.zero();
					delta.rotVec.zero();
					mVelocity.zero();
					return;
				}
				move = &delta.move;
			}
			else
				move = &NullMove;
		}
		if (!isGhost())
			updateAnimation(TickSec);

		bool needUpdate = true;
#ifdef NTJ_CLIENT
		needUpdate = (getControllingClient() || delta.move.x || delta.move.y || delta.move.z || delta.move.yaw || !mVelocity.isZero());
#endif
#ifdef NTJ_SERVER
		needUpdate = (getControllingClient() || delta.move.y || delta.move.yaw || move->y || move->yaw || !mVelocity.isZero());
#endif
		PROFILE_START(Player_PhysicsSection);
		if((isServerObject() || (didRenderLastRender() || getControllingClient())) && needUpdate)
		{
			updateWorkingCollisionSet();
			updateState();

			#pragma message(ENGINE(初步修改了玩家移动及其校验))
#ifdef NTJ_EDITOR
			updateMove(move);
			updatePos();
			SetPosToMove((Move*)move, getPosition());
#endif
#ifdef NTJ_SERVER
			if (CheckMoveSpeed(move,TickSec))
				NoSimulateMove(move);
			else
			{
				GameConnection* connection = getControllingClient();
				if(connection)
					connection->SetNoMove();
			}
#endif
#ifdef NTJ_CLIENT
			updateMove_Grid(move);
			updatePos_Grid();
			if(getControllingClient())
			{
				if(getControllingClient()->getControlObject() == this)
					gClientObjectLoadMgr.update(getPosition());
				SetPosToMove((Move*)move, getPosition());
			}
#endif

			//updateLookAnimation();
			//updateDeathOffsets();
#ifndef NTJ_SERVER
			UpdateCurrentTerrain();
#endif
		}
		else if(!needUpdate)
		{
			// 清除delta
			delta.posVec = mVelocity;
			delta.rotVec.zero();
		}
		PROFILE_END(Player_PhysicsSection);

		if (!isGhost())
		{
			// Animations are advanced based on frame rate on the
			// client and must be ticked on the server.
			updateActionThread();
			updateAnimationTree(true);
		}
	}
}

void GameObject::UpdateCurrentTerrain()
{
#ifndef NTJ_SERVER
	TerrainBlock* m_pTerrain = NavigationBlock::GetTerrain(getPosition());

	GameConnection* con = getControllingClient();
	if(con && (con->getControlObject() == this))
	{
		if(gClientSceneGraph->getCurrentTerrain() != m_pTerrain)
		{
			gClientSceneGraph->setCurrentTerrain(m_pTerrain);
		}
	}
#endif
}

void GameObject::beginAdvanceTime(F32 dt)
{
	mPrevRenderPos = mCurrRenderPos;
	if(debugFlag)
	{
		//Con::printf("坐标拉到目标点(%0.3f,%0.3f,%0.3f)",mPrevRenderPos.x,mPrevRenderPos.y,mPrevRenderPos.z);
		debugFlag = false;
	}
}

void GameObject::interpolateTick(F32 dt)
{
	if (mControlObject)
		mControlObject->interpolateTick(dt);

	// Client side interpolation
	Parent::interpolateTick(dt);

	Point3F pos = delta.pos + delta.posVec * dt;
	Point3F rot = delta.rot + delta.rotVec * dt;

	if(mFlyPath.pathId && isClientObject())
	{
		interpolateFlyPath(pos,rot,dt);
	}
	else
	{
		if(mJumpPath.jumping && isClientObject() && getControllingClient())
			interpolateJumpPath(pos,rot,dt);
		setRenderPosition(pos,rot,dt);
	}

	// apply camera effects - is this the best place? - bramage
	GameConnection* connection = GameConnection::getConnectionToServer();
	if(connection && connection->isFirstPerson() )
	{
		ShapeBase *obj = dynamic_cast<ShapeBase*>(connection->getControlObject());
		if( obj == this )
		{
			MatrixF curTrans = getRenderTransform();
			curTrans.mul( gCamFXMgr.getTrans() );
			Parent::setRenderTransform( curTrans );
		}
	}
}

void GameObject::advanceTime(F32 dt)
{
	// 先更新动作和动作速率
	updateActionThread();
	// Client side animations
	Parent::advanceTime(dt);
	updateAnimation(dt);
	updateActionTriggerState();
	updateSplash();
	updateFroth(dt);
	updateWaterSounds(dt);

	mLastPos = getPosition();
	mCurrRenderPos = getRenderTransform().getPosition();
	mCurrRenderStep = (mCurrRenderPos-mPrevRenderPos)/2;

	if (mImpactSound)
		playImpactSound();

	// update camera effects.  Definitely need to find better place for this - bramage
	if( isControlObject() )
	{
		if( isDisabled() )
		{
			// clear out all camera effects being applied to player if dead
			gCamFXMgr.clear();
		}
		gCamFXMgr.update( dt );
	}
}

bool GameObject::getAIMove(Move* move)
{
	return false;
}


//----------------------------------------------------------------------------

void GameObject::setState(ActionState state, U32 recoverTicks)
{
	if (state != mState) {
		// Skip initialization if there is no manager, the state
		// will get reset when the object is added to a manager.
		if (isProperlyAdded()) {
			switch (state) {
			case RecoverState: {
				// 类似硬直时间 取消
				//mRecoverTicks = recoverTicks;
				//mReversePending = U32(F32(mRecoverTicks) / sLandReverseScale);
				//setActionThread(GameObjectData::Root, true, false, true, true);
				break;
							   }
			}
		}

		mState = state;
	}
}

void GameObject::updateState()
{
	switch (mState)
	{
	case RecoverState:
		if (mRecoverTicks-- <= 0)
		{
			if (mReversePending && mLowerActionAnimation.action != GameObjectData::NullAnimation)
			{
				// this serves and counter, and direction state
				mRecoverTicks = mReversePending;
				mLowerActionAnimation.forward = false;

				S32 seq = g_TSShapeRepository.GetSequence(mTSSahpeInfo->m_TSShapeId,mLowerActionAnimation.action);
				F32 pos = mShapeInstance->getPos(mLowerActionAnimation.thread);

				mShapeInstance->setTimeScale(mLowerActionAnimation.thread, -sLandReverseScale);
				mShapeInstance->transitionToSequence(mLowerActionAnimation.thread,
					seq, pos, sAnimationTransitionTime, true);
				mReversePending = 0;
			}
			else
			{
				setState(MoveState);
			}
		}        // Stand back up slowly only if not moving much-
		else if (!mReversePending && mVelocity.lenSquared() > sSlowStandThreshSquared)
		{
			mLowerActionAnimation.waitForEnd = false;
			setState(MoveState);
		}
		break;
	}
}

const char* GameObject::getStateName()
{
	if (mDamageState != Enabled)
		return "Dead";
	if (isMounted())
		return "Mounted";
	if (mState == RecoverState)
		return "Recover";
	return "Move";
}

void GameObject::getDamageLocation(const Point3F& in_rPos, const char *&out_rpVert, const char *&out_rpQuad)
{
	Point3F newPoint;
	mWorldToObj.mulP(in_rPos, &newPoint);

	F32 zHeight = mDataBlock->boxSize.z;
	F32 zTorso  = mDataBlock->boxTorsoPercentage;
	F32 zHead   = mDataBlock->boxHeadPercentage;

	zTorso *= zHeight;
	zHead  *= zHeight;

	if (newPoint.z <= zTorso)
		out_rpVert = "legs";
	else if (newPoint.z <= zHead)
		out_rpVert = "torso";
	else
		out_rpVert = "head";

	if(dStrcmp(out_rpVert, "head") != 0)
	{
		if (newPoint.y >= 0.0f)
		{
			if (newPoint.x <= 0.0f)
				out_rpQuad = "front_left";
			else
				out_rpQuad = "front_right";
		}
		else
		{
			if (newPoint.x <= 0.0f)
				out_rpQuad = "back_left";
			else
				out_rpQuad = "back_right";
		}
	}
	else
	{
		F32 backToFront = mDataBlock->boxSize.x;
		F32 leftToRight = mDataBlock->boxSize.y;

		F32 backPoint  = backToFront * (mDataBlock->boxHeadBackPercentage  - 0.5f);
		F32 frontPoint = backToFront * (mDataBlock->boxHeadFrontPercentage - 0.5f);
		F32 leftPoint  = leftToRight * (mDataBlock->boxHeadLeftPercentage  - 0.5f);
		F32 rightPoint = leftToRight * (mDataBlock->boxHeadRightPercentage - 0.5f);

		S32 index = 0;
		if (newPoint.y < backPoint)
			index += 0;
		else if (newPoint.y <= frontPoint)
			index += 3;
		else
			index += 6;

		if (newPoint.x < leftPoint)
			index += 0;
		else if (newPoint.x <= rightPoint)
			index += 1;
		else
			index += 2;

		switch (index)
		{
		case 0:
			out_rpQuad = "left_back";
			break;

		case 1: out_rpQuad = "middle_back"; break;
		case 2: out_rpQuad = "right_back"; break;
		case 3: out_rpQuad = "left_middle";   break;
		case 4: out_rpQuad = "middle_middle"; break;
		case 5: out_rpQuad = "right_middle"; break;
		case 6: out_rpQuad = "left_front";   break;
		case 7: out_rpQuad = "middle_front"; break;
		case 8: out_rpQuad = "right_front"; break;

		default:
			AssertFatal(0, "Bad non-tant index");
		};
	}
}

//----------------------------------------------------------------------------

void GameObject::updateMove(const Move* move)
{
	mCheckVelocity_1 = 0;
	delta.move = *move;

	if(mFlyPath.pathId)
	{
		updateFlyPath(mGameplayDeltaTime/1000.f);
		return;
	}
	if(mJumpPath.jumping)
	{
		updateJumpPath(mGameplayDeltaTime/1000.f);
		return;
	}
	if(mBlink)
		return;
	// Update current orientation
	/*if (mDamageState == Enabled)*/ {
		F32 prevZRot = mRot.z;
		//delta.headVec = mHead;

		F32 p = move->pitch;
		if (p > M_PI_F) 
			p -= M_2PI_F;
		//mHead.x = mClampF(mHead.x + p,mDataBlock->minLookAngle,
		//                  mDataBlock->maxLookAngle);

		F32 y = move->yaw;
		if (y > M_PI_F)
			y -= M_2PI_F;

		GameConnection* con = getControllingClient();
		if (move->freeLook /*&& ((isMounted() && getMountNode() == 0) || (con && !con->isFirstPerson()))*/)
		{
			//mHead.z = mClampF(mHead.z + y,
			//                  -mDataBlock->maxFreelookAngle,
			//                  mDataBlock->maxFreelookAngle);
			mRot.z += y;
#ifdef NTJ_CLIENT
			#pragma message(ENGINE(此时摄像机不随人物转动))
			if(getControllingClient() && !getControlObject())
				dGuiMouseGamePlay::SetCameraYaw(dGuiMouseGamePlay::GetCameraYaw()-y);
#endif
#ifdef NTJ_EDITOR
			dWorldEditor::SetCameraYaw(dWorldEditor::GetCameraYaw()-y);
#endif
		}
		else
		{
			mRot.z += y;
			// Rotate the head back to the front, center horizontal
			// as well if we're controlling another object.
			//mHead.z *= 0.9f;
			//if (mControlObject)
			//   mHead.x *= 0.5f;
		}

		// constrain the range of mRot.z
		while (mRot.z < 0.0f)
			mRot.z += M_2PI_F;
		while (mRot.z > M_2PI_F)
			mRot.z -= M_2PI_F;

		delta.rot = mRot;
		delta.rotVec.x = delta.rotVec.y = 0.0f;
		delta.rotVec.z = prevZRot - mRot.z;
		while (delta.rotVec.z > M_PI_F)
			delta.rotVec.z -= M_2PI_F;
		while (delta.rotVec.z < -M_PI_F)
			delta.rotVec.z += M_2PI_F;

		//delta.head = mHead;
		//delta.headVec -= mHead;
	}
	MatrixF zRot;
	zRot.set(EulerF(0.0f, 0.0f, mRot.z));

	// Desired move direction & speed
	VectorF moveVec;
	F32 moveSpeed;
	if (mState == MoveState && mDamageState == Enabled)
	{
		zRot.getColumn(0,&moveVec);
		moveVec *= move->x;
		VectorF tv;
		zRot.getColumn(1,&tv);
		moveVec += tv * move->y;

		// 是否是步行速度
		moveSpeed = isWalking() ? mDataBlock->maxWalkSpeed : getMoveSpeed()/*mDataBlock->maxForwardSpeed*/;
		// Clamp water movement
		if (move->y > 0.0f)
		{
			if( mWaterCoverage >= 0.9f )
				moveSpeed = moveSpeed * move->y;
			else
				moveSpeed = moveSpeed * move->y;;
		}
		else
		{
			if( mWaterCoverage >= 0.9f )
				moveSpeed = moveSpeed * mFabs(move->y);
			else
				moveSpeed = moveSpeed * mFabs(move->y);
		}

		// Cancel any script driven animations if we are going to move.
		if (moveVec.x + moveVec.y + moveVec.z != 0.0f &&
			(mLowerActionAnimation.action >= GameObjectData::NumTableActionAnims))
			mLowerActionAnimation.action = GameObjectData::NullAnimation;
	}
	else
	{
		moveVec.set(0.0f, 0.0f, 0.0f);
		moveSpeed = 0.0f;
	}

	//验证值
	mCheckVelocity_1 = moveSpeed;

	// Acceleration due to gravity
	VectorF acc(0.0f, 0.0f, mGravity * mGravityMod * TickSec);

	// Determine ground contact normal. Only look for contacts if
	// we can move.
	VectorF contactNormal;
	bool jumpSurface = false, runSurface = false;
	if (!isMounted())
		findContact(&runSurface,&jumpSurface,&contactNormal);
	if (jumpSurface)
		mJumpSurfaceNormal = contactNormal;

	// Acceleration on run surface
	if (runSurface) {
		mContactTimer = 0;

		// Remove acc into contact surface (should only be gravity)
		// Clear out floating point acc errors, this will allow
		// the player to "rest" on the ground.
		F32 vd = -mDot(acc,contactNormal);
		if (vd > 0.0f) {
			VectorF dv = contactNormal * (vd + 0.002f);
			acc += dv;
			if (acc.len() < 0.0001f)
				acc.set(0.0f, 0.0f, 0.0f);
		}

		// Force a 0 move if there is no energy, and only drain
		// move energy if we're moving.
		VectorF pv;
		pv = moveVec;

		// Adjust the players's requested dir. to be parallel
		// to the contact surface.
		F32 pvl = pv.len();
		if(mJetting)
		{
			pvl = moveVec.len();
			if (pvl)
			{
				VectorF nn;
				mCross(pv,VectorF(0.0f, 0.0f, 0.0f),&nn);
				nn *= 1 / pvl;
				VectorF cv(0.0f, 0.0f, 0.0f);
				cv -= nn * mDot(nn,cv);
				pv -= cv * mDot(pv,cv);
				pvl = pv.len();
			}
		}
		else
		{
			if (pvl)
			{
				VectorF nn;
				mCross(pv,VectorF(0.0f, 0.0f, 1.0f),&nn);
				nn *= 1.0f / pvl;
				VectorF cv = contactNormal;
				cv -= nn * mDot(nn,cv);
				pv -= cv * mDot(pv,cv);
				pvl = pv.len();
			}
		}

		// Convert to acceleration
		if (pvl)
			pv *= moveSpeed / pvl;
		VectorF runAcc = pv - (mVelocity + acc);
		F32 runSpeed = runAcc.len();

		// Clamp acceleratin, player also accelerates faster when
		// in his hard landing recover state.
		F32 maxAcc = (mDataBlock->runForce / mMass) * TickSec;
		if (mState == RecoverState)
			maxAcc *= mDataBlock->recoverRunForceScale;
		if (runSpeed > maxAcc)
			runAcc *= maxAcc / runSpeed;
		acc += runAcc;

		// If we are running on the ground, then we're not jumping
		if (mDataBlock->isJumpAction(mLowerActionAnimation.action))
			mLowerActionAnimation.action = GameObjectData::NullAnimation;
	}
	else
	{
		mContactTimer++;

		if (!inLiquid && mDataBlock->airControl > 0.0f)
		{
			VectorF pv;
			pv = moveVec;
			F32 pvl = pv.len();

			if (pvl)
				pv *= moveSpeed / pvl;

			VectorF runAcc = pv - acc;
			runAcc.z = 0;
			runAcc.x = runAcc.x * mDataBlock->airControl;
			runAcc.y = runAcc.y * mDataBlock->airControl;
			F32 runSpeed = runAcc.len();
			F32 maxAcc = (mDataBlock->runForce / mMass) * TickSec * 0.3f;

			if (runSpeed > maxAcc)
				runAcc *= maxAcc / runSpeed;

			acc += runAcc;
		}
	}

	// Acceleration from Jumping
	if (move->trigger[2] && !isMounted() && canJump())
	{
		// Scale the jump impulse base on maxJumpSpeed
		F32 zSpeedScale = mVelocity.z;
		if (zSpeedScale <= mDataBlock->maxJumpSpeed)
		{
			zSpeedScale = (zSpeedScale <= mDataBlock->minJumpSpeed)? 1:
		1 - (zSpeedScale - mDataBlock->minJumpSpeed) /
			(mDataBlock->maxJumpSpeed - mDataBlock->minJumpSpeed);

		// Desired jump direction
		VectorF pv = moveVec;
		F32 len = pv.len();
		if (len > 0)
			pv *= 1 / len;

		// We want to scale the jump size by the player size, somewhat
		// in reduced ratio so a smaller player can jump higher in
		// proportion to his size, than a larger player.
		F32 scaleZ = (getScale().z * 0.25) + 0.75;

		// Calculate our jump impulse
		F32 impulse = mDataBlock->jumpForce / mMass;

		if (mDataBlock->jumpTowardsNormal)
		{
			// If we are facing into the surface jump up, otherwise
			// jump away from surface.
			F32 dot = mDot(pv,mJumpSurfaceNormal);
			if (dot <= 0)
				acc.z += mJumpSurfaceNormal.z * scaleZ * impulse * zSpeedScale;
			else
			{
				acc.x += pv.x * impulse * dot;
				acc.y += pv.y * impulse * dot;
				acc.z += mJumpSurfaceNormal.z * scaleZ * impulse * zSpeedScale;
			}
		}
		else
			acc.z += scaleZ * impulse * zSpeedScale;

		mJumpDelay = mDataBlock->jumpDelay;
		//mEnergy -= mDataBlock->jumpEnergyDrain;

		setActionThread(GameObjectData::Jump, true, false, false);
		mJumpSurfaceLastContact = JumpSkipContactsMax;
		}
	}
	else
	{
		if (jumpSurface) 
		{
			if (mJumpDelay > 0)
				mJumpDelay--;
			mJumpSurfaceLastContact = 0;
		}
		else
			mJumpSurfaceLastContact++;
	}

	if (move->trigger[1] && !isMounted() && canJetJump())
	{
		mJetting = true;

		// Scale the jump impulse base on maxJumpSpeed
		F32 zSpeedScale = mVelocity.z;

		if (zSpeedScale <= mDataBlock->jetMaxJumpSpeed)
		{
			zSpeedScale = (zSpeedScale <= mDataBlock->jetMinJumpSpeed)? 1:
		1 - (zSpeedScale - mDataBlock->jetMinJumpSpeed) / (mDataBlock->jetMaxJumpSpeed - mDataBlock->jetMinJumpSpeed);

		// Desired jump direction
		VectorF pv = moveVec;
		F32 len = pv.len();

		if (len > 0.0f)
			pv *= 1 / len;

		// If we are facing into the surface jump up, otherwise
		// jump away from surface.
		F32 dot = mDot(pv,mJumpSurfaceNormal);
		F32 impulse = mDataBlock->jetJumpForce / mMass;

		if (dot <= 0)
			acc.z += mJumpSurfaceNormal.z * impulse * zSpeedScale;
		else
		{
			acc.x += pv.x * impulse * dot;
			acc.y += pv.y * impulse * dot;
			acc.z += mJumpSurfaceNormal.z * impulse * zSpeedScale;
		}

		//mEnergy -= mDataBlock->jetJumpEnergyDrain;
		}
	}
	else
	{
		mJetting = false;
	}

	//if (mJetting)
	//{
	//	F32 newEnergy = mEnergy - mDataBlock->minJumpEnergy;

	//	if (newEnergy < 0)
	//	{
	//		newEnergy = 0;
	//		mJetting = false;
	//	}

	//	mEnergy = newEnergy;
	//}

	// Add in force from physical zones...
	acc += (mAppliedForce / getMass()) * TickSec;

	// Adjust velocity with all the move & gravity acceleration
	// TG: I forgot why doesn't the TickSec multiply happen here...
	mVelocity += acc;

	// apply horizontal air resistance

	F32 hvel = mSqrt(mVelocity.x * mVelocity.x + mVelocity.y * mVelocity.y);

	if(hvel > mDataBlock->horizResistSpeed)
	{
		F32 speedCap = hvel;
		if(speedCap > mDataBlock->horizMaxSpeed)
			speedCap = mDataBlock->horizMaxSpeed;
		speedCap -= mDataBlock->horizResistFactor * TickSec * (speedCap - mDataBlock->horizResistSpeed);
		F32 scale = speedCap / hvel;
		mVelocity.x *= scale;
		mVelocity.y *= scale;
	}
	if(mVelocity.z > mDataBlock->upResistSpeed)
	{
		if(mVelocity.z > mDataBlock->upMaxSpeed)
			mVelocity.z = mDataBlock->upMaxSpeed;
		mVelocity.z -= mDataBlock->upResistFactor * TickSec * (mVelocity.z - mDataBlock->upResistSpeed);
	}

	// Container buoyancy & drag
	if (mBuoyancy != 0)
	{     // Applying buoyancy when standing still causing some jitters-
		if (mBuoyancy > 1.0 || !mVelocity.isZero() || !runSurface)
			mVelocity.z -= mBuoyancy * mGravity * mGravityMod * TickSec;
	}
	mVelocity   -= mVelocity * mDrag * TickSec;

	// 不在空中时，可能需要人为设置速度 [3/17/2009 joy]
	if(runSurface && !move->mVelocity.isZero() && mVelocity.lenSquared() > move->mVelocity.lenSquared())
		mVelocity = move->mVelocity;

	// 速度太小时置0
	if(mVelocity.len() < 0.0001)
		mVelocity.set(0,0,0);

	// If we are not touching anything and have sufficient -z vel,
	// we are falling.
	if (runSurface)
		mFalling = false;
	else
	{
		VectorF vel;
		mWorldToObj.mulV(mVelocity,&vel);
		mFalling = vel.z < -4.0f/*sFallingThreshold*/;
	}

	if (!isGhost()) {
		// Vehicle Dismount
		if(move->trigger[2] && isMounted())
			Con::executef(mDataBlock, "doDismount",scriptThis());

		if(!inLiquid && mWaterCoverage != 0.0f) {
			Con::executef(mDataBlock, "onEnterLiquid",scriptThis(), Con::getFloatArg(mWaterCoverage), Con::getIntArg(mLiquidType));
			inLiquid = true;
		}
		else if(inLiquid && mWaterCoverage == 0.0f) {
			Con::executef(mDataBlock, "onLeaveLiquid",scriptThis(), Con::getIntArg(mLiquidType));
			inLiquid = false;
		}
	}
	else
	{
		if ( !inLiquid && mWaterCoverage >= 1.0f )
		{
			inLiquid = true;
		}
		else if ( inLiquid && mWaterCoverage < 0.8f )
		{
			if ( getVelocity().len() >= mDataBlock->exitSplashSoundVel && !isMounted() )
//				SFX->playOnce( mDataBlock->sound[GameObjectData::ExitWater], &getTransform() );

			inLiquid = false;
		}
	}
}

void GameObject::updateMove_Grid(const Move* move, bool updateDelta /* = true */)
{
	mCheckVelocity_1 = 0;
	if(updateDelta)
		delta.move = *move;
	if(mFlyPath.pathId)
	{
		updateFlyPath(mGameplayDeltaTime/1000.f);
		return;
	}
#ifdef NTJ_CLIENT
	if(mJumpPath.jumping)
#else
	if(move->jumping)
#endif
	{
		updateJumpPath(mGameplayDeltaTime/1000.f);
		return;
	}
	if(mBlink)
		return;
	// Update current orientation
	/*if (mDamageState == Enabled)*/ {
		F32 prevZRot = mRot.z;
		//delta.headVec = mHead;

		F32 p = move->pitch;
		if (p > M_PI_F) 
			p -= M_2PI_F;
		//mHead.x = mClampF(mHead.x + p,mDataBlock->minLookAngle,
		//                  mDataBlock->maxLookAngle);

		F32 y = move->yaw;
		if (y > M_PI_F)
			y -= M_2PI_F;

		GameConnection* con = getControllingClient();
		if (move->freeLook /*&& ((isMounted() && getMountNode() == 0) || (con && !con->isFirstPerson()))*/)
		{
			//mHead.z = mClampF(mHead.z + y,
			//                  -mDataBlock->maxFreelookAngle,
			//                  mDataBlock->maxFreelookAngle);
			mRot.z += y;
#ifdef NTJ_CLIENT
			#pragma message(ENGINE(此时摄像机不随人物转动))
			if(getControllingClient() && !getControlObject())
				dGuiMouseGamePlay::SetCameraYaw(dGuiMouseGamePlay::GetCameraYaw()-y);
#endif
#ifdef NTJ_EDITOR
			dWorldEditor::SetCameraYaw(dWorldEditor::GetCameraYaw()-y);
#endif
		}
		else
		{
			mRot.z += y;
			// Rotate the head back to the front, center horizontal
			// as well if we're controlling another object.
			//mHead.z *= 0.9f;
			//if (mControlObject)
			//   mHead.x *= 0.5f;
		}

		// constrain the range of mRot.z
		while (mRot.z < 0.0f)
			mRot.z += M_2PI_F;
		while (mRot.z > M_2PI_F)
			mRot.z -= M_2PI_F;

		delta.rot = mRot;
		delta.rotVec.x = delta.rotVec.y = 0.0f;
		delta.rotVec.z = prevZRot - mRot.z;
		while (delta.rotVec.z > M_PI_F)
			delta.rotVec.z -= M_2PI_F;
		while (delta.rotVec.z < -M_PI_F)
			delta.rotVec.z += M_2PI_F;

		//delta.head = mHead;
		//delta.headVec -= mHead;
	}
	MatrixF zRot;
	zRot.set(EulerF(0.0f, 0.0f, mRot.z));

	// Desired move direction & speed
	VectorF moveVec;
	F32 moveSpeed;
	if (mState == MoveState && mDamageState == Enabled)
	{
		zRot.getColumn(0,&moveVec);
		moveVec *= move->x;
		VectorF tv;
		zRot.getColumn(1,&tv);
		moveVec += tv * move->y;

		// 是否是步行速度
		moveSpeed = isWalking() ? mDataBlock->maxWalkSpeed : getMoveSpeed()/*mDataBlock->maxForwardSpeed*/;
		// Clamp water movement
		if (move->y > 0.0f)
		{
			if( mWaterCoverage >= 0.9f )
				moveSpeed = moveSpeed * move->y;
			else
				moveSpeed = moveSpeed * move->y;;
		}
		else
		{
			if( mWaterCoverage >= 0.9f )
				moveSpeed = moveSpeed * mFabs(move->y);
			else
				moveSpeed = moveSpeed * mFabs(move->y);
		}

		// Cancel any script driven animations if we are going to move.
		if (moveVec.x + moveVec.y + moveVec.z != 0.0f &&
			(mLowerActionAnimation.action >= GameObjectData::NumTableActionAnims))
			mLowerActionAnimation.action = GameObjectData::NullAnimation;
	}
	else
	{
		moveVec.set(0.0f, 0.0f, 0.0f);
		moveSpeed = 0.0f;
	}

	//验证值
	mCheckVelocity_1 = moveSpeed;

	// Acceleration due to gravity
	VectorF acc(0.0f, 0.0f, mGravity * mGravityMod * TickSec);

	// Determine ground contact normal. Only look for contacts if
	// we can move.
	VectorF contactNormal;
	bool jumpSurface = false, runSurface = false;
	if (!isMounted())
		findContact_Grid(&runSurface,&jumpSurface,&contactNormal);
	if (jumpSurface)
		mJumpSurfaceNormal = contactNormal;

	// Acceleration on run surface
	if (runSurface) {
		mContactTimer = 0;

		// Remove acc into contact surface (should only be gravity)
		// Clear out floating point acc errors, this will allow
		// the player to "rest" on the ground.
		F32 vd = -mDot(acc,contactNormal);
		if (vd > 0.0f) {
			VectorF dv = contactNormal * (vd + 0.002f);
			acc += dv;
			if (acc.len() < 0.0001f)
				acc.set(0.0f, 0.0f, 0.0f);
		}

		// Force a 0 move if there is no energy, and only drain
		// move energy if we're moving.
		VectorF pv;
		pv = moveVec;

		// Adjust the players's requested dir. to be parallel
		// to the contact surface.
		F32 pvl = pv.len();
		if(mJetting)
		{
			pvl = moveVec.len();
			if (pvl)
			{
				VectorF nn;
				mCross(pv,VectorF(0.0f, 0.0f, 0.0f),&nn);
				nn *= 1 / pvl;
				VectorF cv(0.0f, 0.0f, 0.0f);
				cv -= nn * mDot(nn,cv);
				pv -= cv * mDot(pv,cv);
				pvl = pv.len();
			}
		}
		else
		{
			if (pvl)
			{
				VectorF nn;
				mCross(pv,VectorF(0.0f, 0.0f, 1.0f),&nn);
				nn *= 1.0f / pvl;
				VectorF cv = contactNormal;
				cv -= nn * mDot(nn,cv);
				pv -= cv * mDot(pv,cv);
				pvl = pv.len();
			}
		}

		// Convert to acceleration
		if (pvl)
			pv *= moveSpeed / pvl;
		VectorF runAcc = pv - (mVelocity + acc);
		F32 runSpeed = runAcc.len();

		// Clamp acceleratin, player also accelerates faster when
		// in his hard landing recover state.
		F32 maxAcc = (mDataBlock->runForce / mMass) * TickSec;
		if (mState == RecoverState)
			maxAcc *= mDataBlock->recoverRunForceScale;
		if (runSpeed > maxAcc)
			runAcc *= maxAcc / runSpeed;
		acc += runAcc;

		// If we are running on the ground, then we're not jumping
		if (mDataBlock->isJumpAction(mLowerActionAnimation.action))
			mLowerActionAnimation.action = GameObjectData::NullAnimation;
	}
	else
	{
		mContactTimer++;

		if (!inLiquid && mDataBlock->airControl > 0.0f)
		{
			VectorF pv;
			pv = moveVec;
			F32 pvl = pv.len();

			if (pvl)
				pv *= moveSpeed / pvl;

			VectorF runAcc = pv - acc;
			runAcc.z = 0;
			runAcc.x = runAcc.x * mDataBlock->airControl;
			runAcc.y = runAcc.y * mDataBlock->airControl;
			F32 runSpeed = runAcc.len();
			F32 maxAcc = (mDataBlock->runForce / mMass) * TickSec * 0.3f;

			if (runSpeed > maxAcc)
				runAcc *= maxAcc / runSpeed;

			acc += runAcc;
		}
	}

	// Acceleration from Jumping
	if (move->trigger[2] && !isMounted() && canJump())
	{
		// Scale the jump impulse base on maxJumpSpeed
		F32 zSpeedScale = mVelocity.z;
		if (zSpeedScale <= mDataBlock->maxJumpSpeed)
		{
			zSpeedScale = (zSpeedScale <= mDataBlock->minJumpSpeed)? 1:
		1 - (zSpeedScale - mDataBlock->minJumpSpeed) /
			(mDataBlock->maxJumpSpeed - mDataBlock->minJumpSpeed);

		// Desired jump direction
		VectorF pv = moveVec;
		F32 len = pv.len();
		if (len > 0)
			pv *= 1 / len;

		// We want to scale the jump size by the player size, somewhat
		// in reduced ratio so a smaller player can jump higher in
		// proportion to his size, than a larger player.
		F32 scaleZ = (getScale().z * 0.25) + 0.75;

		// Calculate our jump impulse
		F32 impulse = mDataBlock->jumpForce / mMass;

		if (mDataBlock->jumpTowardsNormal)
		{
			// If we are facing into the surface jump up, otherwise
			// jump away from surface.
			F32 dot = mDot(pv,mJumpSurfaceNormal);
			if (dot <= 0)
				acc.z += mJumpSurfaceNormal.z * scaleZ * impulse * zSpeedScale;
			else
			{
				acc.x += pv.x * impulse * dot;
				acc.y += pv.y * impulse * dot;
				acc.z += mJumpSurfaceNormal.z * scaleZ * impulse * zSpeedScale;
			}
		}
		else
			acc.z += scaleZ * impulse * zSpeedScale;

		mJumpDelay = mDataBlock->jumpDelay;
		//mEnergy -= mDataBlock->jumpEnergyDrain;

		setActionThread(GameObjectData::Jump, true, false, false);
		mJumpSurfaceLastContact = JumpSkipContactsMax;
		}
	}
	else
	{
		if (jumpSurface) 
		{
			if (mJumpDelay > 0)
				mJumpDelay--;
			mJumpSurfaceLastContact = 0;
		}
		else
			mJumpSurfaceLastContact++;
	}

	if (move->trigger[1] && !isMounted() && canJetJump())
	{
		mJetting = true;

		// Scale the jump impulse base on maxJumpSpeed
		F32 zSpeedScale = mVelocity.z;

		if (zSpeedScale <= mDataBlock->jetMaxJumpSpeed)
		{
			zSpeedScale = (zSpeedScale <= mDataBlock->jetMinJumpSpeed)? 1:
		1 - (zSpeedScale - mDataBlock->jetMinJumpSpeed) / (mDataBlock->jetMaxJumpSpeed - mDataBlock->jetMinJumpSpeed);

		// Desired jump direction
		VectorF pv = moveVec;
		F32 len = pv.len();

		if (len > 0.0f)
			pv *= 1 / len;

		// If we are facing into the surface jump up, otherwise
		// jump away from surface.
		F32 dot = mDot(pv,mJumpSurfaceNormal);
		F32 impulse = mDataBlock->jetJumpForce / mMass;

		if (dot <= 0)
			acc.z += mJumpSurfaceNormal.z * impulse * zSpeedScale;
		else
		{
			acc.x += pv.x * impulse * dot;
			acc.y += pv.y * impulse * dot;
			acc.z += mJumpSurfaceNormal.z * impulse * zSpeedScale;
		}

		//mEnergy -= mDataBlock->jetJumpEnergyDrain;
		}
	}
	else
	{
		mJetting = false;
	}

	//if (mJetting)
	//{
	//	F32 newEnergy = mEnergy - mDataBlock->minJumpEnergy;

	//	if (newEnergy < 0)
	//	{
	//		newEnergy = 0;
	//		mJetting = false;
	//	}

	//	mEnergy = newEnergy;
	//}

	// Add in force from physical zones...
	acc += (mAppliedForce / getMass()) * TickSec;

	// Adjust velocity with all the move & gravity acceleration
	// TG: I forgot why doesn't the TickSec multiply happen here...
	mVelocity += acc;

	// apply horizontal air resistance

	F32 hvel = mSqrt(mVelocity.x * mVelocity.x + mVelocity.y * mVelocity.y);

	if(hvel > mDataBlock->horizResistSpeed)
	{
		F32 speedCap = hvel;
		if(speedCap > mDataBlock->horizMaxSpeed)
			speedCap = mDataBlock->horizMaxSpeed;
		speedCap -= mDataBlock->horizResistFactor * TickSec * (speedCap - mDataBlock->horizResistSpeed);
		F32 scale = speedCap / hvel;
		mVelocity.x *= scale;
		mVelocity.y *= scale;
	}
	if(mVelocity.z > mDataBlock->upResistSpeed)
	{
		if(mVelocity.z > mDataBlock->upMaxSpeed)
			mVelocity.z = mDataBlock->upMaxSpeed;
		mVelocity.z -= mDataBlock->upResistFactor * TickSec * (mVelocity.z - mDataBlock->upResistSpeed);
	}

	// Container buoyancy & drag
	if (mBuoyancy != 0)
	{     // Applying buoyancy when standing still causing some jitters-
		if (mBuoyancy > 1.0 || !mVelocity.isZero() || !runSurface)
			mVelocity.z -= mBuoyancy * mGravity * mGravityMod * TickSec;
	}
	mVelocity   -= mVelocity * mDrag * TickSec;

	// 不在空中时，可能需要人为设置速度 [3/17/2009 joy]
	if(runSurface && !move->mVelocity.isZero() && mVelocity.lenSquared() > move->mVelocity.lenSquared())
		mVelocity = move->mVelocity;

	// 速度太小时置0
	if(mVelocity.len() < 0.0001)
		mVelocity.set(0,0,0);

	// If we are not touching anything and have sufficient -z vel,
	// we are falling.
	if (runSurface)
		mFalling = false;
	else
	{
		VectorF vel;
		mWorldToObj.mulV(mVelocity,&vel);
		mFalling = vel.z < -0.1f/*sFallingThreshold*/;
	}

	if (!isGhost()) {
		// Vehicle Dismount
		if(move->trigger[2] && isMounted())
			Con::executef(mDataBlock, "doDismount",scriptThis());

		if(!inLiquid && mWaterCoverage != 0.0f) {
			Con::executef(mDataBlock, "onEnterLiquid",scriptThis(), Con::getFloatArg(mWaterCoverage), Con::getIntArg(mLiquidType));
			inLiquid = true;
		}
		else if(inLiquid && mWaterCoverage == 0.0f) {
			Con::executef(mDataBlock, "onLeaveLiquid",scriptThis(), Con::getIntArg(mLiquidType));
			inLiquid = false;
		}
	}
	else
	{
		if ( !inLiquid && mWaterCoverage >= 1.0f )
		{
			inLiquid = true;
		}
		else if ( inLiquid && mWaterCoverage < 0.8f )
		{
			if ( getVelocity().len() >= mDataBlock->exitSplashSoundVel && !isMounted() )
//				SFX->playOnce( mDataBlock->sound[GameObjectData::ExitWater], &getTransform() );

			inLiquid = false;
		}
	}
}

void GameObject::findContact_Grid(bool* run,bool* jump,VectorF* contactNormal)
{
	bool contacted;
	Point3F pos = getPosition();

	//S32 index = g_NavigationManager->GetBlock().GetIndex(pos);
	//if(index != -1 && g_NavigationManager->GetBlock().CanReach(index))
	if (g_NavigationManager->CanReach(pos))
	{
		contacted = true;
		//*contactNormal = g_NavigationManager->GetBlock().SlerpNormal(pos);
		*contactNormal = g_NavigationManager->SlerpNormal(pos);
		*run = contactNormal->z > mDataBlock->runSurfaceCos;

		mContactInfo.clear();
		mContactInfo.contacted = contacted;
		mContactInfo.contactNormal = *contactNormal;
		mContactInfo.run = *run;
#ifdef NTJ_CLIENT
		if( isClientObject() )
			updateTrigger();
#endif
	}
	else
	{
		if (isClientObject())
			findContact(run,jump,contactNormal);
		else
		{
			run = false;
			mContactInfo.clear();
			*contactNormal = mContactInfo.contactNormal;
		}
	}
}

bool GameObject::updatePos_Grid(const F32 travelTime)
{
	PROFILE_SCOPE(Player_UpdatePos_Simple);

	// When mounted to another object, only Z rotation used.
	if (isMounted()) {
		mVelocity = mMount.object->getVelocity();
		setPosition(Point3F(0.0f, 0.0f, 0.0f), ((GameObject*)mMount.object)->getRotation());
		//setMaskBits(MoveMask);
		return true;
	}
#ifdef NTJ_CLIENT
	if(getControllingClient() && (mJumpPath.jumping || mBlink || mFlyPath.pathId))
		return true;
#endif

	getTransform().getColumn(3,&delta.posVec);
	static Point3F prePos;
	static Point3F tmpPos;
	prePos = getPosition();
	tmpPos = prePos + mVelocity * travelTime;

	//S32 index = g_NavigationManager->GetBlock().GetIndex(tmpPos);
	//if(index == -1 || !g_NavigationManager->GetBlock().CanReach(index))
	if (!g_NavigationManager->CanReach(tmpPos))
	{
        Point3F idVec = mVelocity;
        idVec.normalizeSafe();
        tmpPos += idVec * NaviGridSize *0.9f;

        if (!g_NavigationManager->CanReach(tmpPos))
        {
            tmpPos = getPosition();
            mVelocity.x = 0;
            mVelocity.y = 0;
        }
        else
        {
            tmpPos = g_NavigationManager->SlerpPosition(tmpPos);

            //检测高度是否差值太大
            if (fabs(tmpPos.z - prePos.z) >= NaviGridSize)
            {
                tmpPos = getPosition();
                mVelocity.x = 0;
                mVelocity.y = 0;
            }
        }
	}
	else
	{
		tmpPos = g_NavigationManager->SlerpPosition(tmpPos);
	}

	//tmpPos = g_NavigationManager->GetBlock().SlerpPosition(tmpPos);
	
	if (isClientObject())
	{
		delta.pos = tmpPos;
		delta.posVec = delta.posVec - delta.pos;
		delta.dt = 1.0f;
	}

	setPosition(tmpPos,mRot);
	if(!prePos.equal(tmpPos))
		setMaskBits(MoveMask);

	if (mVelocity.isZero())
		return true;
	else
		return false;
}

bool GameObject::calculateJumpPath(Point3F& end)
{
	if(mJumpPath.jumping/*mJumpPath.stepH >= mJumpPath.lenH*/)
		return false;

	static Point3F temp,begin;

#ifdef NTJ_CLIENT
	static Vector<DepthSortList::Poly> sPartition;
	static Vector<Point3F> sPartitionVerts;
	end.z += 1.5f;
	gVectorProjector.buildPartition(end,Point3F(0,0,-1),0.2f,3,dGuiMouseGamePlay::m_MouseFloorCollisionMask,sPartition,sPartitionVerts,NULL,false);
	F32 newPos = 100.0f;
	if(sPartitionVerts.size() > 0)
	{
		for (S32 i=0; i<sPartitionVerts.size(); i++)
			newPos = getMin(sPartitionVerts[i].y, newPos);
		end.z -= getMin(newPos, 1.6f);
	}
	else
		end.z -= 1.5f;
#endif
	begin = getPosition();
	temp = end - begin;
	if(temp.lenSquared() > 400.0f || temp.lenSquared() < 1.0f)
		return false;

	Point2F _begin(0,0), _end(mSqrt(temp.x*temp.x + temp.y*temp.y), temp.z);
	F32 tMin = _end.x/MAX_FORWARD_SPEED/2;
	Point2F _top(0, getMax(0.0f, _end.y) + (mFabs(mGravity)*0.5f*tMin*tMin));

	// 先求出b的两种情况
	F32 a_t = _end.x*_end.x/(-4.0f * _top.y);
	F32 b_t = _end.x;
	F32 c_t = -_end.y;
	F32 delta_t = b_t*b_t - 4*a_t*c_t;
	if(delta_t < 0)
		return false;

	mJumpPath.b = getMax((-b_t+sqrt(delta_t))/(2*a_t), (-b_t-sqrt(delta_t))/(2*a_t)); // 取较大者
	mJumpPath.a = -(mJumpPath.b*mJumpPath.b)/(4*_top.y);
	if(mJumpPath.a > 0 || mJumpPath.b < 0)
		return false;

	mJumpPath.t = mSqrt(2*_top.y/mFabs(mGravity)) + mSqrt(2*(_top.y - _end.y)/mFabs(mGravity));
	mJumpPath.begin = begin;
	mJumpPath.end = end;
	mJumpPath.lenH = _end.x;

	mJumpPath.rotz = -mAtan(-temp.x,temp.y);
	mJumpPath.advTime = 0;
	mJumpPath.jumping = true;

	return true;
}

bool GameObject::setJumpPath(Point3F& end)
{
	if(mJumpPath.jumping)
		return false;

	if(!calculateJumpPath(end))
	{
		clearJumpPath();
		return false;
	}

	return true;
}

void GameObject::clearJumpPath()
{
	mJumpPath.begin.zero();
	mJumpPath.end.zero();
	mJumpPath.a = 0;
	mJumpPath.b = 0;
	mJumpPath.lenH = 0;
	mJumpPath.t = 0;
	mJumpPath.advTime = 0;
	mJumpPath.rotz = 0;
	mJumpPath.jumping = false;
	mJumpPath.fall = false;
}

void GameObject::updateJumpPath(F32 dt)
{
	Point3F pos;
	mJumpPath.advTime += dt;
	if(mJumpPath.advTime >= mJumpPath.t)
	{
		pos = mJumpPath.end;
		mJumpPath.jumping = false;
	}
	else
	{
		F32 percent = mJumpPath.advTime/mJumpPath.t;
		F32 x = percent * mJumpPath.lenH;
		F32 y = mJumpPath.a * x * x + mJumpPath.b * x;
		pos = mJumpPath.end - mJumpPath.begin;
		pos.z = 0;
		pos *= percent;
		pos.z = y;
		pos += mJumpPath.begin;
	}
	mJumpPath.fall = (mJumpPath.advTime * 2 >= mJumpPath.t);
#ifdef NTJ_CLIENT
	if(getControllingClient() && !getControlObject())
		dGuiMouseGamePlay::SetCameraYaw(dGuiMouseGamePlay::GetCameraYaw()-(mJumpPath.rotz - getRotation().z)/*dGuiMouseGamePlay::GetCameraYaw()-y*/);
#endif
	setPosition(pos,Point3F(0,0,mJumpPath.rotz));
	setMaskBits(MoveMask);
	updateContainer();
}

void GameObject::interpolateJumpPath(Point3F& pos, Point3F& rot, F32 dt)
{
	//Point3F pos;
	F32 time = (1.0f-dt) * TickSec + mJumpPath.advTime;
	if(time >= mJumpPath.t)
	{
		pos = mJumpPath.end;
		//mJumpPath.jumping = false;
	}
	else
	{
		F32 percent = time/mJumpPath.t;
		F32 x = percent * mJumpPath.lenH;
		F32 y = mJumpPath.a * x * x + mJumpPath.b * x;
		pos = mJumpPath.end - mJumpPath.begin;
		pos.z = 0;
		pos *= percent;
		pos.z = y;
		pos += mJumpPath.begin;
	}
	rot.z = mJumpPath.rotz;
}

bool GameObject::setFlyPath(U32 pathId, U32 mountId, U32 flyPos /* = 0 */)
{
	if(!pathId)
	{
		clearFlyPath();
		return true;
	}
	if(isClientObject())
	{
		if(!gClientPathManager->isValidPath(pathId))
		{
			clearFlyPath();
			return false;
		}
		mFlyPath.pathId = pathId;
		mFlyPath.flyPos = flyPos;
		mFlyPath.atEnd = false;
		mFlyPath.mountId = mountId;
	}
	else
	{
		if(!gServerPathManager->isValidPath(pathId))
		{
			clearFlyPath();
			return false;
		}

		// 如果是玩家 需要骑乘上去
		if(getGameObjectMask() & PlayerObjectType)
		{
			S32 slot = ((Player*)this)->getPetTable().getMountedSlot();
			if(slot != -1)
				PetMountOperationManager::DisbandMountPet((Player*)this, slot);
			PetMountOperationManager::Mount((Player*)this, mountId);
			GameObject* pObj = (GameObject*)getObjectMount();
			// 骑乘成功且给骑乘设定飞行路径成功
			if(!pObj || !pObj->setFlyPath(pathId, mountId, flyPos))
			{
				clearFlyPath();
				return false;
			}
		}

		mFlyPath.pathId = pathId;
		mFlyPath.flyPos = flyPos;
		mFlyPath.atEnd = false;
		mFlyPath.mountId = mountId;

		if(getControllingClient() && (getGameObjectMask() & PlayerObjectType))
		{
			ServerGameNetEvent* pEvent = new ServerGameNetEvent(INFO_FLY);
			pEvent->SetInt32ArgValues(3, pathId, mountId, flyPos);
			getControllingClient()->postNetEvent(pEvent);
		}
	}
#ifndef NTJ_EDITOR
	if(m_pAI)
	{
		// 清除AI操作
		m_pAI->ClearMove();
		m_pAI->ClearAIPath();
	}
#endif

	return true;
}

void GameObject::clearFlyPath()
{
	if(mFlyPath.pathId)
	{
		if(isClientObject())
		{
			mFlyPath.flyPos = 0x7FFFFFFF;
			updateFlyPath(0);
		}
		if(isServerObject())
		{
			if(getGameObjectMask() & PlayerObjectType)
				PetMountOperationManager::Unmount((Player*)this, mFlyPath.mountId);
			// 飞行坐骑只能乘坐1人
			if(getMountedObject(0))
				((GameObject*)getMountedObject(0))->clearFlyPath();
		}
	}
	if(getGameObjectMask() & PlayerObjectType)
	{
		mFlyPath.pathId = 0;
		mFlyPath.flyPos = 0;
		mFlyPath.atEnd = false;
		mFlyPath.mountId = 0;

		if(isServerObject() && getControllingClient())
		{
			ServerGameNetEvent* pEvent = new ServerGameNetEvent(INFO_FLY);
			pEvent->SetInt32ArgValues(3, mFlyPath.pathId, mFlyPath.mountId, mFlyPath.flyPos);
			getControllingClient()->postNetEvent(pEvent);
		}
	}
}

void GameObject::updateFlyPath(F32 dt)
{
	if(!mFlyPath.pathId)
		return;

	mFlyPath.flyPos += dt * 1000;
	Point3F pos;
	QuatF q;
	if(isClientObject())
		mFlyPath.atEnd = gClientPathManager->getPathPosition(mFlyPath.pathId, mFlyPath.flyPos, pos, q);
	else
		mFlyPath.atEnd = gServerPathManager->getPathPosition(mFlyPath.pathId, mFlyPath.flyPos, pos, q);

#ifdef NTJ_CLIENT
	Point3F rot;
	getTransform().getColumn(1, &rot);
	F32 rotz = -mAtan(-rot.x, rot.y);
#endif

	setTransform(pos,q);

#ifdef NTJ_CLIENT
	updateTrigger();
	getTransform().getColumn(1, &rot);
	rot.z = -mAtan(-rot.x, rot.y);

	if(getControllingClient() && !getControlObject())
		dGuiMouseGamePlay::SetCameraYaw(dGuiMouseGamePlay::GetCameraYaw()-(rot.z - rotz));
#endif
#ifdef NTJ_SERVER
	VectorF contactNormal;
	bool jumpSurface = false, runSurface = false;
	if (!isMounted())
		findContact(&runSurface,&jumpSurface,&contactNormal);
#endif

	setMaskBits(MoveMask);
	updateContainer();
}

void GameObject::interpolateFlyPath(Point3F& pos, Point3F& rot, F32 dt)
{
	SimTime time = (1.0f-dt) * TickMs + mFlyPath.flyPos;
	QuatF q;
	gClientPathManager->getPathPosition(mFlyPath.pathId, time, pos, q);
	setRenderTransform(pos, q);
}

bool GameObject::setBlink(Point3F& end)
{
#ifdef NTJ_CLIENT
	if(m_pAI)
	{
		// 清除AI操作
		m_pAI->ClearMove();
		m_pAI->ClearAIPath();
	}
#endif
	//Point3F rot(0.0f,0.0f,-mAtan(-vec.x,vec.y));
	setPosition(end, getRotation());
	mBlink = true;
	return true;
}

bool GameObject::isPackWholeStats(NetConnection* conn)
{
	if(getControllingClient() == conn)
		return true;
	if(getGameObjectMask() & PetObjectType)
	{
		if (((PetObject*)this)->getMaster() && ((PetObject*)this)->getMaster()->getControllingClient() == conn)
			return true;
	}
	return false;
}

bool GameObject::findTriggerID(U32 triggerID)
{
	if(mEnterTriggerID.size() == 0)
		return true;
	for(int i=0; i<mEnterTriggerID.size(); ++i)
	{
		if(mEnterTriggerID[i] == triggerID)
			return false;
	}
	return true;
}
//----------------------------------------------------------------------------

bool GameObject::checkDismountPosition(const MatrixF& oldMat, const MatrixF& mat)
{
	AssertFatal(getContainer() != NULL, "Error, must have a container!");
	AssertFatal(getObjectMount() != NULL, "Error, must be mounted!");

	Point3F pos;
	Point3F oldPos;
	mat.getColumn(3, &pos);
	oldMat.getColumn(3, &oldPos);
	RayInfo info;
	disableCollision();
	getObjectMount()->disableCollision();
	if (getContainer()->castRay(oldPos, pos, sCollisionMoveMask, &info))
	{
		enableCollision();
		getObjectMount()->enableCollision();
		return false;
	}

	Box3F wBox = mObjBox;
	wBox.min += pos;
	wBox.max += pos;

	EarlyOutPolyList polyList;
	polyList.mNormal.set(0.0f, 0.0f, 0.0f);
	polyList.mPlaneList.clear();
	polyList.mPlaneList.setSize(6);
	polyList.mPlaneList[0].set(wBox.min,VectorF(-1.0f, 0.0f, 0.0f));
	polyList.mPlaneList[1].set(wBox.max,VectorF(0.0f, 1.0f, 0.0f));
	polyList.mPlaneList[2].set(wBox.max,VectorF(1.0f, 0.0f, 0.0f));
	polyList.mPlaneList[3].set(wBox.min,VectorF(0.0f, -1.0f, 0.0f));
	polyList.mPlaneList[4].set(wBox.min,VectorF(0.0f, 0.0f, -1.0f));
	polyList.mPlaneList[5].set(wBox.max,VectorF(0.0f, 0.0f, 1.0f));

	if (getContainer()->buildPolyList(wBox, sCollisionMoveMask, &polyList))
	{
		enableCollision();
		getObjectMount()->enableCollision();
		return false;
	}

	enableCollision();
	getObjectMount()->enableCollision();
	return true;
}


//----------------------------------------------------------------------------

bool GameObject::canJump()
{
	return mState == MoveState && mDamageState == Enabled && !isMounted() && !mJumpDelay && /*mEnergy >= mDataBlock->minJumpEnergy && */mJumpSurfaceLastContact < JumpSkipContactsMax;
}

bool GameObject::canJetJump()
{
	return mState == MoveState && mDamageState == Enabled && !isMounted() && /*mEnergy >= mDataBlock->jetMinJumpEnergy && */mDataBlock->jetJumpForce != 0.0f;
}

//----------------------------------------------------------------------------

void GameObject::updateLookAnimation(F32 dT)
{
	// Calculate our interpolated head position.
	Point3F renderHead = delta.head + delta.headVec * dT;
}


//----------------------------------------------------------------------------
// Methods to get delta (as amount to affect velocity by)

bool GameObject::inDeathAnim()
{
	// 死亡上下部的动作应该是统一的，所以判断一处即可
	if (mLowerActionAnimation.thread && mLowerActionAnimation.action >= 0)
		if (mLowerActionAnimation.action < mDataBlock->actionCount)
			return mLowerActionAnimation.action == GameObjectData::Death;

	return false;
}

// Get change from mLastDeathPos - return current pos.  Assumes we're in death anim.
F32 GameObject::deathDelta(Point3F & delta)
{
	// Get ground delta from the last time we offset this.
	MatrixF  mat;
	F32 pos = mShapeInstance->getPos(mLowerActionAnimation.thread);
	mShapeInstance->deltaGround1(mLowerActionAnimation.thread, mDeath.lastPos, pos, mat);
	mat.getColumn(3, & delta);
	return pos;
}

// Called before updatePos() to prepare it's needed change to velocity, which
// must roll over.  Should be updated on tick, this is where we remember last
// position of animation that was used to roll into velocity.
void GameObject::updateDeathOffsets()
{
	if (inDeathAnim())
		// Get ground delta from the last time we offset this.
		mDeath.lastPos = deathDelta(mDeath.posAdd);
	else
		mDeath.clear();
}


//----------------------------------------------------------------------------

static const U32 sPlayerConformMask =  InteriorObjectType|StaticShapeObjectType|
StaticObjectType|TerrainObjectType|
AtlasObjectType;

static void accel(F32& from, F32 to, F32 rate)
{
	if (from < to)
		from = getMin(from += rate, to);
	else
		from = getMax(from -= rate, to);
}

// if (dt == -1)
//    normal tick, so we advance.
// else
//    interpolate with dt as % of tick, don't advance
//
MatrixF * GameObject::Death::fallToGround(F32 dt, const Point3F& loc, F32 curZ, F32 boxRad)
{
	static const F32 sConformCheckDown = 4.0f;
	RayInfo     coll;
	bool        conformToStairs = false;
	Point3F     pos(loc.x, loc.y, loc.z + 0.1f);
	Point3F     below(pos.x, pos.y, loc.z - sConformCheckDown);
	MatrixF  *  retVal = NULL;

	PROFILE_START(ConformToGround);

	if (gClientContainer.castRay(pos, below, sPlayerConformMask, &coll))
	{
		F32      adjust, height = (loc.z - coll.point.z), sink = curSink;
		VectorF  desNormal = coll.normal;
		VectorF  normal = curNormal;

		// dt >= 0 means we're interpolating and don't accel the numbers
		if (dt >= 0.0f)
			adjust = dt * TickSec;
		else
			adjust = TickSec;

		// Try to get them to conform to stairs by doing several LOS calls.  We do this if
		// normal is within about 5 deg. of vertical.
		if (desNormal.z > 0.995f)
		{
			Point3F  corners[3], downpts[3];
			S32      c;

			for (c = 0; c < 3; c++) {    // Build 3 corners to cast down from-
				corners[c].set(loc.x - boxRad, loc.y - boxRad, loc.z + 1.0f);
				if (c)      // add (0,boxWidth) and (boxWidth,0)
					corners[c][c - 1] += (boxRad * 2.0f);
				downpts[c].set(corners[c].x, corners[c].y, loc.z - sConformCheckDown);
			}

			// Do the three casts-
			for (c = 0; c < 3; c++)
				if (gClientContainer.castRay(corners[c], downpts[c], sPlayerConformMask, &coll))
					downpts[c] = coll.point;
				else
					break;

			// Do the math if everything hit below-
			if (c == 3) {
				mCross(downpts[1] - downpts[0], downpts[2] - downpts[1], &desNormal);
				AssertFatal(desNormal.z > 0, "Abnormality in GameObject::Death::fallToGround()");
				downpts[2] = downpts[2] - downpts[1];
				downpts[1] = downpts[1] - downpts[0];
				desNormal.normalize();
				conformToStairs = true;
			}
		}

		// Move normal in direction we want-
		F32   * cur = normal, * des = desNormal;
		for (S32 i = 0; i < 3; i++)
			accel(*cur++, *des++, adjust * 0.25f);

		if (mFabs(height) < 2.2f && !normal.isZero() && desNormal.z > 0.01f)
		{
			VectorF  upY(0.0f, 1.0f, 0.0f), ahead;
			VectorF  sideVec;
			MatrixF  mat(true);

			normal.normalize();
			mat.set(EulerF (0.0f, 0.0f, curZ));
			mat.mulV(upY, & ahead);
			mCross(ahead, normal, &sideVec);
			sideVec.normalize();
			mCross(normal, sideVec, &ahead);

			static MatrixF resMat(true);
			resMat.setColumn(0, sideVec);
			resMat.setColumn(1, ahead);
			resMat.setColumn(2, normal);

			// Adjust Z down to account for box offset on slope.  Figure out how
			// much we want to sink, and gradually accel to this amount.  Don't do if
			// we're conforming to stairs though
			F32   xy = mSqrt(desNormal.x * desNormal.x + desNormal.y * desNormal.y);
			F32   desiredSink = (boxRad * xy / desNormal.z);
			if (conformToStairs)
				desiredSink *= 0.5f;

			accel(sink, desiredSink, adjust * 0.15f);

			Point3F  position(pos);
			position.z -= sink;
			resMat.setColumn(3, position);

			if (dt < 0.0f)
			{  // we're moving, so update normal and sink amount
				curNormal = normal;
				curSink = sink;
			}

			retVal = &resMat;
		}
	}
	PROFILE_END(ConformToGround);
	return retVal;
}


//-------------------------------------------------------------------------------------

// This is called ::onAdd() to see if we're in a sitting animation.  These then
// can use a longer tick delay for the mount to get across.
bool GameObject::inSittingAnim()
{
	U32   action = mLowerActionAnimation.action;
	if (mLowerActionAnimation.thread && action < mDataBlock->actionCount) {
		if(action == GameObjectData::Sit)
			return true;
	}
	return false;
}


//----------------------------------------------------------------------------

void GameObject::refreshImageAction(U32 imageSlot)
{
	if(mMountedImageList[imageSlot].shapeInstance && mMountedImageList[imageSlot].animThread)
	{
		S32 TempSeq = g_TSShapeRepository.GetSequence(mMountedImageList[imageSlot].shapeName,mUpperActionAnimation.action);
		if(TempSeq == -1)
			return;
		F32 UpperPos = mShapeInstance->getSeqPos(mUpperActionAnimation.thread);
		mMountedImageList[imageSlot].shapeInstance->setTimeScale(mMountedImageList[imageSlot].animThread,mUpperActionAnimation.forward ? 1.0f : -1.0f);
		mMountedImageList[imageSlot].shapeInstance->setSequence(mMountedImageList[imageSlot].animThread,TempSeq,mUpperActionAnimation.forward ? UpperPos : 1.0f - UpperPos);
	}
}

bool GameObject::setActionThread(const char* sequence,bool hold,bool wait,bool fsp)
{
	for (U32 i = 1; i < mDataBlock->actionCount; i++)
	{
		if (!dStricmp(GameObjectData::ActionAnimationList[i].name,sequence))
		{
			setActionThread(i,true,hold,wait,fsp);
			setMaskBits(ActionMask);
			return true;
		}
	}
	return false;
}

void GameObject::setActionThread(U32 action,bool forward,bool hold,bool wait,bool fsp, bool forceSet)
{
	// 以Arm_A为标准投送动作，此时会根据手持武器自动选择相应动作
	action = getAnim((GameObjectData::Animations)action);
	if (mUpperActionAnimation.action == action && mLowerActionAnimation.action == action && !GameObjectData::getActionFlags(action,GameObjectData::Flags_Repeat))
		return;

	if (action >= GameObjectData::NumActionAnims)
	{
		Con::errorf("GameObject::setActionThread(%d): GameObject action out of range", action);
		return;
	}

	U32 UpperAction = action;
	U32 LowerAction = action;
	F32 UpperPos = 0;
	F32 LowerPos = 0;
	if(!forceSet)
	{
		// 上下部的动作都结束，无条件使用新动作
		if(mUpperActionAnimation.atEnd && !mUpperActionAnimation.holdAtEnd && mLowerActionAnimation.atEnd && !mLowerActionAnimation.holdAtEnd)
		{
			UpperAction = action;
			LowerAction = action;
		}
		// 上下部的动作有结束的情况，先进行同步，再选择使用新动作
		else if(mUpperActionAnimation.atEnd && !mUpperActionAnimation.holdAtEnd)
		{
			UpperAction = mLowerActionAnimation.action;
			UpperPos = mShapeInstance->getSeqPos(mLowerActionAnimation.thread);
			GameObjectData::selectAction(action, UpperAction, mLowerActionAnimation.action, UpperAction, LowerAction);
		}
		else if(mLowerActionAnimation.atEnd && !mLowerActionAnimation.holdAtEnd)
		{
			LowerAction = mUpperActionAnimation.action;
			LowerPos = mShapeInstance->getSeqPos(mUpperActionAnimation.thread);
			GameObjectData::selectAction(action, mUpperActionAnimation.action, LowerAction, UpperAction, LowerAction);
		}
		else
		{
			// 选择新动作
			GameObjectData::selectAction(action, mUpperActionAnimation.action, mLowerActionAnimation.action, UpperAction, LowerAction);
			if(UpperAction != LowerAction)
			{
				// 上下部动作不一致，尝试同步
				GameObjectData::selectAction(UpperAction, UpperAction, LowerAction, UpperAction, LowerAction);
				if(UpperAction == LowerAction)
					LowerPos = mShapeInstance->getSeqPos(mUpperActionAnimation.thread);
			}
		}
	}

	S32 Useq = g_TSShapeRepository.GetSequence(mTSSahpeInfo->m_TSShapeId,UpperAction);
	if ((mUpperActionAnimation.action != UpperAction || (UpperAction == action && GameObjectData::getActionFlags(UpperAction,GameObjectData::Flags_Repeat))) && Useq != -1)
	{
		mUpperActionAnimation.action          = UpperAction;
		mUpperActionAnimation.forward         = forward;
		mUpperActionAnimation.firstPerson     = fsp;
		mUpperActionAnimation.holdAtEnd       = hold;
		mUpperActionAnimation.waitForEnd      = hold? true: wait;
		mUpperActionAnimation.animateOnServer = fsp;
		mUpperActionAnimation.atEnd           = false;
		mUpperActionAnimation.endTime         = GameObjectData::ActionAnimationList[UpperAction].delayTicks + Platform::getVirtualMilliseconds();

		if (sUseAnimationTransitions && (isGhost()/* || mLowerActionAnimation.animateOnServer*/))
		{
			//if(IsRunAnim(UpperAction) && (mShapeInstance->getOldSequence(mLowerActionAnimation.thread) == Useq) && UpperAction == LowerAction)
			//	UpperPos = mShapeInstance->getOldPos(mLowerActionAnimation.thread);
			// The transition code needs the timeScale to be set in the
			// right direction to know which way to go.
			F32   transTime = GameObjectData::ActionAnimationList[UpperAction].transTime;

			mShapeInstance->transitionToSequence(mUpperActionAnimation.thread,Useq,	mUpperActionAnimation.forward ? UpperPos : 1.0f - UpperPos, transTime, true);
			mShapeInstance->setTimeScale(mUpperActionAnimation.thread, mUpperActionAnimation.forward ? 1.0f : -1.0f);
			MountedImage *pImage = NULL;
			// image部分，应该根据链接点分别放到上下半身
			for(S32 i=0; i<MaxMountedImages; i++)
			{
				pImage = &mMountedImageList[i];
				if(pImage->shapeInstance && pImage->animThread && pImage->isUpper)
				{
					S32 TempSeq = g_TSShapeRepository.GetSequence(pImage->shapeName,UpperAction);
					if(TempSeq == -1)
						continue;
					pImage->shapeInstance->transitionToSequence(pImage->animThread,TempSeq,mUpperActionAnimation.forward ? UpperPos : 1.0f - UpperPos, transTime, true);
					pImage->shapeInstance->setTimeScale(pImage->animThread,mUpperActionAnimation.forward ? 1.0f : -1.0f);
				}
			}
		}
		else if(isGhost())
		{
			mShapeInstance->setSequence(mUpperActionAnimation.thread,Useq,mUpperActionAnimation.forward ? 0.0f : 1.0f);
		}

		applyAnimateEP();
		// 在上下分截的情况下，一般只需要更新上半部的动作 [6/8/2009 joy]
		setMaskBits(ActionMask);
	}

	S32 Lseq = g_TSShapeRepository.GetSequence(mTSSahpeInfo->m_TSShapeId,LowerAction);
	if ((mLowerActionAnimation.action != LowerAction || (LowerAction == action && GameObjectData::getActionFlags(LowerAction,GameObjectData::Flags_Repeat))) && Lseq != -1)
	{
		mLowerActionAnimation.action          = LowerAction;
		mLowerActionAnimation.forward         = forward;
		mLowerActionAnimation.firstPerson     = fsp;
		mLowerActionAnimation.holdAtEnd       = hold;
		mLowerActionAnimation.waitForEnd      = hold? true: wait;
		mLowerActionAnimation.animateOnServer = fsp;
		mLowerActionAnimation.atEnd           = false;
		mLowerActionAnimation.endTime         = GameObjectData::ActionAnimationList[LowerAction].delayTicks + Platform::getVirtualMilliseconds();

		if (sUseAnimationTransitions && (isGhost()/* || mLowerActionAnimation.animateOnServer*/))
		{
			//if(IsRunAnim(LowerAction) && (mShapeInstance->getOldSequence(mLowerActionAnimation.thread) == Lseq))
			//{
			//	LowerPos = mShapeInstance->getOldPos(mLowerActionAnimation.thread);
			//	if(LowerAction == UpperAction && LowerPos != UpperPos)
			//		int cv=0;
			//}
			// The transition code needs the timeScale to be set in the
			// right direction to know which way to go.
			F32   transTime = GameObjectData::ActionAnimationList[LowerAction].transTime;

			mShapeInstance->transitionToSequence(mLowerActionAnimation.thread,Lseq,mLowerActionAnimation.forward ? LowerPos : 1.0f - LowerPos, transTime, true);
			mShapeInstance->setTimeScale(mLowerActionAnimation.thread, mLowerActionAnimation.forward ? 1.0f : -1.0f);
			MountedImage *pImage = NULL;
			// image部分，应该根据链接点分别放到上下半身
			for(S32 i=0; i<MaxMountedImages; i++)
			{
				pImage = &mMountedImageList[i];
				if(pImage->shapeInstance && pImage->animThread && !pImage->isUpper)
				{
					S32 TempSeq = g_TSShapeRepository.GetSequence(pImage->shapeName,LowerAction);
					if(TempSeq == -1)
						continue;
					pImage->shapeInstance->transitionToSequence(pImage->animThread,TempSeq,mLowerActionAnimation.forward ? UpperPos : 1.0f - UpperPos, transTime, true);
					pImage->shapeInstance->setTimeScale(pImage->animThread,mLowerActionAnimation.forward ? 1.0f : -1.0f);
				}
			}
		}
		else if(isGhost())
		{
			mShapeInstance->setSequence(mLowerActionAnimation.thread,Lseq,mLowerActionAnimation.forward ? 0.0f : 1.0f);
		}
	}
}

void GameObject::updateActionThread()
{
	PROFILE_START(UpdateActionThread);

	// Select an action animation sequence, this assumes that
	// this function is called once per tick.
	if(isGhost())
	{
		if(mLowerActionAnimation.action != GameObjectData::NullAnimation)
			if (mLowerActionAnimation.forward)
				mLowerActionAnimation.atEnd = mShapeInstance->getPos(mLowerActionAnimation.thread) == 1;
			else
				mLowerActionAnimation.atEnd = mShapeInstance->getPos(mLowerActionAnimation.thread) == 0;
		if(mUpperActionAnimation.action != GameObjectData::NullAnimation)
			if (mUpperActionAnimation.forward)
				mUpperActionAnimation.atEnd = mShapeInstance->getPos(mUpperActionAnimation.thread) == 1;
			else
				mUpperActionAnimation.atEnd = mShapeInstance->getPos(mUpperActionAnimation.thread) == 0;
	}
	// 服务端需要通过时间去判断动作是否结束
	else
	{
		if(mUpperActionAnimation.action != GameObjectData::NullAnimation)
		{
			mUpperActionAnimation.atEnd = GameObjectData::ActionAnimationList[mUpperActionAnimation.action].delayTicks ? (mUpperActionAnimation.endTime <= Platform::getVirtualMilliseconds()) : false;
			// 一个动作结束，清除相关的特效记录
			if(mUpperActionAnimation.atEnd)
				clearAnimateEP();
		}
		if(mLowerActionAnimation.action != GameObjectData::NullAnimation)
			mLowerActionAnimation.atEnd = GameObjectData::ActionAnimationList[mLowerActionAnimation.action].delayTicks ? (mLowerActionAnimation.endTime <= Platform::getVirtualMilliseconds()) : false;
	}


	// Mount pending variable puts a hold on the delayTicks below so players don't
	// inadvertently stand up because their mount has not come over yet.
	if (mMountPending)
		mMountPending = (isMounted() ? 0 : (mMountPending - 1));

	if (mLowerActionAnimation.action == GameObjectData::NullAnimation ||
		((!mLowerActionAnimation.waitForEnd || mLowerActionAnimation.atEnd)) &&
		!mLowerActionAnimation.holdAtEnd /*&& (mLowerActionAnimation.delayTicks -= !mMountPending) <= 0 有待仔细看*/)
	{
		//The scripting language will get a call back when a script animation has finished...
		//  example: When the chat menu animations are done playing...
		//if ( isServerObject() && mLowerActionAnimation.action >= GameObjectData::NumTableActionAnims )
			//Con::executef(mDataBlock, "animationDone",scriptThis());
		pickActionAnimation();
	}

	if (mLowerActionAnimation.action != GameObjectData::NullAnimation && isClientObject())
	{
		// Update action animation time scale to match ground velocity
		F32 UpperScale = 1;
		F32 LowerScale = 1;
		// TODO: 需要调整动画的速度
#ifndef NTJ_EDITOR
		if(GameObjectData::getActionFlags(mUpperActionAnimation.action, GameObjectData::Flags_ScaleAttack))
			UpperScale = mClampF((GetStats().AttackSpeed_gPc)/GAMEPLAY_PERCENT_SCALE, MIN_GAMEPLAY_PERCENT, MAX_GAMEPLAY_PERCENT);
		else if(GameObjectData::getActionFlags(mUpperActionAnimation.action, GameObjectData::Flags_ScaleVelocity))
		{
			if (getGameObjectMask() & MountObjectType)
			{
				GameObject* obj = (GameObject*)getMountedObject(0);
				UpperScale = obj ? (obj->GetStats().MountedVelocity_gPc/GAMEPLAY_PERCENT_SCALE) : (GetStats().Velocity_gPc/GAMEPLAY_PERCENT_SCALE);
			}
			else
				UpperScale = (isMounted() ? GetStats().MountedVelocity_gPc : GetStats().Velocity_gPc)/GAMEPLAY_PERCENT_SCALE;
			UpperScale = mClampF(UpperScale/getScale().y, MIN_GAMEPLAY_PERCENT, MAX_GAMEPLAY_PERCENT);
		}

		if(GameObjectData::getActionFlags(mLowerActionAnimation.action, GameObjectData::Flags_ScaleAttack))
			LowerScale = mClampF((GetStats().AttackSpeed_gPc)/GAMEPLAY_PERCENT_SCALE, MIN_GAMEPLAY_PERCENT, MAX_GAMEPLAY_PERCENT);
		else if(GameObjectData::getActionFlags(mLowerActionAnimation.action, GameObjectData::Flags_ScaleVelocity))
		{
			if (getGameObjectMask() & MountObjectType)
			{
				GameObject* obj = (GameObject*)getMountedObject(0);
				LowerScale = obj ? (obj->GetStats().MountedVelocity_gPc/GAMEPLAY_PERCENT_SCALE) : (GetStats().Velocity_gPc/GAMEPLAY_PERCENT_SCALE);
			}
			else
				LowerScale = (isMounted() ? GetStats().MountedVelocity_gPc : GetStats().Velocity_gPc)/GAMEPLAY_PERCENT_SCALE;
			LowerScale = mClampF(LowerScale/getScale().y, MIN_GAMEPLAY_PERCENT, MAX_GAMEPLAY_PERCENT);
		}
#endif

		UpperScale = mUpperActionAnimation.forward? UpperScale: -UpperScale;
		LowerScale = mLowerActionAnimation.forward? LowerScale: -LowerScale;
		mShapeInstance->setTimeScale(mUpperActionAnimation.thread,UpperScale);
		mShapeInstance->setTimeScale(mLowerActionAnimation.thread,LowerScale);
		for (S32 i=0; i<ShapeBase::MaxMountedImages; ++i)
		{
			if (!mMountedImageList[i].shapeName || !mMountedImageList[i].shapeName[0])
				continue;
			MountedImage& image = mMountedImageList[i];
			if (image.animThread && image.isAnimated && image.shapeInstance)
				image.shapeInstance->setTimeScale(image.animThread, image.isUpper ? UpperScale : LowerScale);
		}
	}
	PROFILE_END(UpdateActionThread);
}

void GameObject::updateActionTriggerState()
{
	// Only need to deal with triggers on the client
	if (!isGhost())
		return;

	//if(getControllingClient()) {
	//	for (S32 i=1; i<32; ++i)
	//	{
	//		if(mShapeInstance->getTriggerState(i,false))
	//			Con::printf("updateActionTriggerState : %d, UpperAction : %d, LowerAction : %d", i, mUpperActionAnimation.action, mLowerActionAnimation.action);
	//	}
	//}

	bool triggeredLeft = false;
	bool triggeredRight = false;
	F32 offset = 0.0f;
	if(mShapeInstance->getTriggerState(1)) {
		triggeredLeft = true;
		offset = -mDataBlock->decalOffset;
	}
	else if(mShapeInstance->getTriggerState(2)) {
		triggeredRight = true;
		offset = mDataBlock->decalOffset;
	}

	if (triggeredLeft || triggeredRight)
	{
		Point3F rot, pos;
		RayInfo rInfo;
		MatrixF mat = getRenderTransform();
		mat.getColumn(1, &rot);
		mat.mulP(Point3F(offset,0.0f,0.0f), &pos);
		if (gClientContainer.castRay(Point3F(pos.x, pos.y, pos.z + 0.01f),
			Point3F(pos.x, pos.y, pos.z - 2.0f ),
			TerrainObjectType | InteriorObjectType | VehicleObjectType, &rInfo))
		{
			S32 sound = -1;
			// Only put footpuffs and prints on the terrain
			if( rInfo.object->getTypeMask() & TerrainObjectType)
			{
				//TerrainBlock* tBlock = static_cast<TerrainBlock*>(rInfo.object);

				// Footpuffs, if we can get the material color...
				/*S32 mapIndex = tBlock->mMPMIndex[0];
				if (mapIndex != -1) {
				MaterialPropertyMap* pMatMap = MaterialPropertyMap::get();
				const MaterialPropertyMap::MapEntry* pEntry = pMatMap->getMapEntryFromIndex(mapIndex);
				if(pEntry)
				{
				sound = pEntry->sound;
				if( rInfo.t <= 0.5 && mWaterCoverage == 0.0f)
				{
				// New emitter every time for visibility reasons
				ParticleEmitter * emitter = new ParticleEmitter;
				emitter->onNewDataBlock( mDataBlock->footPuffEmitter );

				S32 x;
				ColorF colorList[ParticleEngine::PC_COLOR_KEYS];

				for(x = 0; x < 2; ++x)
				colorList[x].set( pEntry->puffColor[x].red, pEntry->puffColor[x].green, pEntry->puffColor[x].blue, pEntry->puffColor[x].alpha );
				for(x = 2; x < ParticleEngine::PC_COLOR_KEYS; ++x)
				colorList[x].set( 1.0, 1.0, 1.0, 0.0 );

				emitter->setColors( colorList );
				if( !emitter->registerObject() )
				{
				Con::warnf( ConsoleLogEntry::General, "Could not register emitter for particle of class: %s", mDataBlock->getName() );
				delete emitter;
				emitter = NULL;
				}
				else
				{
				emitter->emitParticles( pos, Point3F( 0.0, 0.0, 1.0 ), mDataBlock->footPuffRadius,
				Point3F(0, 0, 0), mDataBlock->footPuffNumParts );
				emitter->deleteWhenEmpty();
				}
				}
				}
				}*/

				// Footprint...
				if (mDataBlock->decalData != NULL)
					mSceneManager->getCurrentDecalManager()->addDecal(rInfo.point, rot,
					Point3F(rInfo.normal), getScale(), mDataBlock->decalData);
			}
			else
				if ( rInfo.object->getTypeMask() & VehicleObjectType)
					sound = 2; // Play metal sound

			// Play footstep sounds
			playFootstepSound(triggeredLeft, sound);
		}
	}

	if (mShapeInstance->getTriggerState(TSShape::Trigger::State_AttackNormal))
		applyAnimateEP();
	if (mShapeInstance->getTriggerState(TSShape::Trigger::State_AttackSpell))
		applyAnimateEP();
}

void GameObject::pickActionAnimation(bool force /* = false */)
{
	// Only select animations in our normal move state.
	if (mState != MoveState || mDamageState != Enabled)
		return;

	// 默认root
	bool forward = true;
	U32 action = getRootAnim();

	// 战斗待机
	if(IsAutoCastEnable() && GetTarget())
		action = GameObjectData::Ready_a;

	if (!mVelocity.isZero())
	{
		// 移动速度较慢时用步行动作
		action = (mVelocity.lenSquared() < 4) ? GameObjectData::Walk : GameObjectData::Run_a;
		forward = true;
	}
	// 轻功跳跃
	if(mJumpPath.jumping)
	{
		if(mJumpPath.fall)
			action = GameObjectData::Fall;
		else
			action = GameObjectData::Jump;
	}
	// 操作
	getScheduleAnim(action);
	// 骑乘
	getMountAnim(action);
	// 状态决定的动作
	getBuffAnim(action);
	// 坐骑在飞行中
	if(mFlyPath.pathId && !isMounted())
		action = GameObjectData::Run_a;
	// 死亡
	if(isDisabled())
		action = GameObjectData::Death;
	// idle
#ifdef NTJ_SERVER
	if(action == GameObjectData::Root_a && IsRootAnim(mUpperActionAnimation.action) && gRandGen.randF() > 0.985f)
		action = GameObjectData::Idle_a0 + (Platform::getVirtualMilliseconds() % 3);
#endif
	setActionThread(action,forward,false,false,false,force);
}

void GameObject::onMount(ShapeBase* obj,StringTableEntry node)
{
	if(obj->getMountList() == this && (obj->getTypeMask() & GameObjectType))
	{
		GameObject* gb = (GameObject*)obj;
		gb->setRotation(mRot.z);
	}
	// Parent function will call script
	Parent::onMount(obj,node);
}

void GameObject::onUnmount(ShapeBase* obj,StringTableEntry node)
{
	// Reset back to root position during dismount.
	setActionThread(getRootAnim(),true,false,false);

	// Re-orient the player straight up
	Point3F pos,vec;
	obj->getTransform().getColumn(1,&vec);
	obj->getTransform().getColumn(3,&pos);
	Point3F rot(0.0f,0.0f,-mAtan(-vec.x,vec.y));
	setPosition(pos,rot);

	delta.pos = pos;
	delta.rot = rot;

	// Parent function will call script
	Parent::onUnmount(obj,node);
}


//----------------------------------------------------------------------------

void GameObject::updateAnimation(F32 dt)
{
	if ((isGhost() || mUpperActionAnimation.animateOnServer) && mUpperActionAnimation.thread)
		mShapeInstance->advanceTime(dt,mUpperActionAnimation.thread);
	if ((isGhost() || mLowerActionAnimation.animateOnServer) && mLowerActionAnimation.thread)
		mShapeInstance->advanceTime(dt,mLowerActionAnimation.thread);
#ifdef NTJ_CLIENT
	if (mWoundThread)
		mShapeInstance->advanceTime(dt,mWoundThread);
#endif

	// If we are the client's player on this machine, then we need
	// to make sure the transforms are up to date as they are used
	// to setup the camera.
	if (isGhost())
	{
		if (getControllingClient())
		{
			updateAnimationTree(isFirstPerson());
			//mShapeInstance->animate();
		}
		else
		{
			updateAnimationTree(false);
		}
	}
}

void GameObject::updateAnimationTree(bool firstPerson)
{
    if(!mShapeInstance || !mShapeInstance->getShape())
        return;

	S32 mode = 0;
	if (firstPerson)
		if (mLowerActionAnimation.firstPerson)
			mode = 0;
	//            TSShapeInstance::MaskNodeRotation;
	//            TSShapeInstance::MaskNodePosX |
	//            TSShapeInstance::MaskNodePosY;
		else
			mode = TSShapeInstance::MaskNodeAllButBlend;

	//for (U32 i = 0; i < GameObjectData::NumSpineNodes; i++)
	//{
	//	if (mDataBlock->spineNode[i] != -1)
	//	{
	//		mShapeInstance->setNodeAnimationState(mDataBlock->spineNode[i],mode);
	//	}
	//}
}


//----------------------------------------------------------------------------

bool GameObject::step(Point3F *pos,F32 *maxStep,F32 time)
{
	const Point3F& scale = getScale();
	Box3F box;
	VectorF offset = mVelocity * time;
	// 碰撞体统一用mScaledBox
	box.min = mScaledBox.min + offset + *pos;
	box.max = mScaledBox.max + offset + *pos;
	box.max.z += mDataBlock->maxStepHeight * scale.z + sMinFaceDistance;

	SphereF sphere;
	sphere.center = (box.min + box.max) * 0.5f;
	VectorF bv = box.max - sphere.center;
	sphere.radius = bv.len();

	ClippedPolyList polyList;
	polyList.mPlaneList.clear();
	polyList.mNormal.set(0.0f, 0.0f, 0.0f);
	polyList.mPlaneList.setSize(6);
	polyList.mPlaneList[0].set(box.min,VectorF(-1.0f, 0.0f, 0.0f));
	polyList.mPlaneList[1].set(box.max,VectorF(0.0f, 1.0f, 0.0f));
	polyList.mPlaneList[2].set(box.max,VectorF(1.0f, 0.0f, 0.0f));
	polyList.mPlaneList[3].set(box.min,VectorF(0.0f, -1.0f, 0.0f));
	polyList.mPlaneList[4].set(box.min,VectorF(0.0f, 0.0f, -1.0f));
	polyList.mPlaneList[5].set(box.max,VectorF(0.0f, 0.0f, 1.0f));

	CollisionWorkingList& rList = mConvex.getWorkingList();
	CollisionWorkingList* pList = rList.wLink.mNext;
	while (pList != &rList) {
		Convex* pConvex = pList->mConvex;

		// Alright, here's the deal... a polysoup mesh really needs to be 
		// designed with stepping in mind.  If there are too many smallish polygons
		// the stepping system here gets confused and allows you to run up walls 
		// or on the edges/seams of meshes.

		TSStatic *st = dynamic_cast<TSStatic *> (pConvex->getObject());
		bool skip = false;
		if (st && !st->allowPlayerStep())
			skip = true;

		if ((pConvex->getObject()->getType() & StaticObjectType) != 0 && !skip)
		{
			Box3F convexBox = pConvex->getBoundingBox();
			if (box.isOverlapped(convexBox))
				pConvex->getPolyList(&polyList);
		}
		pList = pList->wLink.mNext;
	}

	// Find max step height
	F32 stepHeight = pos->z - sMinFaceDistance;
	U32* vp = polyList.mIndexList.begin();
	U32* ep = polyList.mIndexList.end();
	for (; vp != ep; vp++) {
		F32 h = polyList.mVertexList[*vp].point.z + sMinFaceDistance;
		if (h > stepHeight)
			stepHeight = h;
	}

	F32 step = stepHeight - pos->z;
	if (stepHeight > pos->z && step < *maxStep) {
		// Go ahead and step
		pos->z = stepHeight;
		*maxStep -= step;
		return true;
	}

	return false;
}


//----------------------------------------------------------------------------
inline Point3F createInterpPos(const Point3F& s, const Point3F& e, const F32 t, const F32 d)
{
	Point3F ret;
	ret.interpolate(s, e, t/d);
	return ret;
}


bool GameObject::updatePos(const F32 travelTime)
{
	PROFILE_SCOPE(Player_UpdatePos);

	// When mounted to another object, only Z rotation used.
	if (isMounted()) {
		mVelocity = mMount.object->getVelocity();
		setPosition(Point3F(0.0f, 0.0f, 0.0f), ((GameObject*)mMount.object)->getRotation());
		//setMaskBits(MoveMask);
		return true;
	}

	if(mJumpPath.jumping || mBlink)
		return true;

	getTransform().getColumn(3,&delta.posVec);
	// Try and move to new pos
	F32 totalMotion  = 0.0f;
	F32 initialSpeed = mVelocity.len();

	Point3F start;
	Point3F initialPosition;
	getTransform().getColumn(3,&start);
	initialPosition = start;

	static CollisionList collisionList;
	static CollisionList physZoneCollisionList;

	collisionList.clear();
	physZoneCollisionList.clear();

	MatrixF collisionMatrix(true);
	collisionMatrix.setColumn(3, start);

	VectorF firstNormal;
	F32 maxStep = mDataBlock->maxStepHeight;
	F32 time = travelTime;
	U32 count = 0;

	const Point3F& scale = getScale();

	static Polyhedron sBoxPolyhedron;
	static ExtrudedPolyList sExtrudedPolyList;
	static ExtrudedPolyList sPhysZonePolyList;

	for (; count < sMoveRetryCount; count++) {
		F32 speed = mVelocity.len();
		if (!speed && !mDeath.haveVelocity())
			break;

		Point3F end = start + mVelocity * time;
		if (mDeath.haveVelocity()) {
			// Add in death movement-
			VectorF  deathVel = mDeath.getPosAdd();
			VectorF  resVel;
			getTransform().mulV(deathVel, & resVel);
			end += resVel;
		}
		Point3F distance = end - start;

		// 物理碰撞统一用mScaledBox [5/22/2009 joy]
		if (mFabs(distance.x) < mScaledBox.len_x() &&
			mFabs(distance.y) < mScaledBox.len_y() &&
			mFabs(distance.z) < mScaledBox.len_z())
		{
			// We can potentially early out of this.  If there are no polys in the clipped polylist at our
			//  end position, then we can bail, and just set start = end;
			Box3F wBox = mScaledBox;
			wBox.min += end;
			wBox.max += end;

			static EarlyOutPolyList eaPolyList;
			eaPolyList.clear();
			eaPolyList.mNormal.set(0.0f, 0.0f, 0.0f);
			eaPolyList.mPlaneList.clear();
			eaPolyList.mPlaneList.setSize(6);
			eaPolyList.mPlaneList[0].set(wBox.min,VectorF(-1.0f, 0.0f, 0.0f));
			eaPolyList.mPlaneList[1].set(wBox.max,VectorF(0.0f, 1.0f, 0.0f));
			eaPolyList.mPlaneList[2].set(wBox.max,VectorF(1.0f, 0.0f, 0.0f));
			eaPolyList.mPlaneList[3].set(wBox.min,VectorF(0.0f, -1.0f, 0.0f));
			eaPolyList.mPlaneList[4].set(wBox.min,VectorF(0.0f, 0.0f, -1.0f));
			eaPolyList.mPlaneList[5].set(wBox.max,VectorF(0.0f, 0.0f, 1.0f));

			// Build list from convex states here...
			CollisionWorkingList& rList = mConvex.getWorkingList();
			CollisionWorkingList* pList = rList.wLink.mNext;
			while (pList != &rList) {
				Convex* pConvex = pList->mConvex;
				if (pConvex->getObject()->getTypeMask() & sCollisionMoveMask) {
					Box3F convexBox = pConvex->getBoundingBox();
					if (wBox.isOverlapped(convexBox))
					{
						// No need to seperate out the physical zones here, we want those
						//  to cause a fallthrough as well...
						pConvex->getPolyList(&eaPolyList);
					}
				}
				pList = pList->wLink.mNext;
			}

			if (eaPolyList.isEmpty())
			{
				totalMotion += (end - start).len();
				start = end;
				break;
			}
		}

		collisionMatrix.setColumn(3, start);
		sBoxPolyhedron.buildBox(collisionMatrix, mScaledBox);

		// Setup the bounding box for the extrudedPolyList
		Box3F plistBox = mScaledBox;
		collisionMatrix.mul(plistBox);
		Point3F oldMin = plistBox.min;
		Point3F oldMax = plistBox.max;
		plistBox.min.setMin(oldMin + (mVelocity * time) - Point3F(0.1f, 0.1f, 0.1f));
		plistBox.max.setMax(oldMax + (mVelocity * time) + Point3F(0.1f, 0.1f, 0.1f));

		// Build extruded polyList...
		VectorF vector = end - start;
		sExtrudedPolyList.extrude(sBoxPolyhedron,vector);
		sExtrudedPolyList.setVelocity(mVelocity);
		sExtrudedPolyList.setCollisionList(&collisionList);

		sPhysZonePolyList.extrude(sBoxPolyhedron,vector);
		sPhysZonePolyList.setVelocity(mVelocity);
		sPhysZonePolyList.setCollisionList(&physZoneCollisionList);

		// Build list from convex states here...
		CollisionWorkingList& rList = mConvex.getWorkingList();
		CollisionWorkingList* pList = rList.wLink.mNext;
		while (pList != &rList) {
			Convex* pConvex = pList->mConvex;
			if (pConvex->getObject()->getTypeMask() & sCollisionMoveMask) {
				Box3F convexBox = pConvex->getBoundingBox();
				if (plistBox.isOverlapped(convexBox))
				{
					if (pConvex->getObject()->getTypeMask() & PhysicalZoneObjectType)
						pConvex->getPolyList(&sPhysZonePolyList);
					else
						pConvex->getPolyList(&sExtrudedPolyList);
				}
			}
			pList = pList->wLink.mNext;
		}

		// Take into account any physical zones...
		for (U32 j = 0; j < physZoneCollisionList.getCount(); j++) 
		{
			AssertFatal(dynamic_cast<PhysicalZone*>(physZoneCollisionList[j].object), "Bad phys zone!");
			const PhysicalZone* pZone = (PhysicalZone*)physZoneCollisionList[j].object;
			if (pZone->isActive())
				mVelocity *= pZone->getVelocityMod();
		}

		if (collisionList.getCount() != 0 && collisionList.getTime() < 1.0f) 
		{
			// Set to collision point
			F32 velLen = mVelocity.len();

			F32 dt = time * getMin(collisionList.getTime(), 1.0f);
			start += mVelocity * dt;
			time -= dt;

			totalMotion += velLen * dt;

			mFalling = false;

			// Back off...
			if ( velLen > 0.f ) {
				F32 newT = getMin(0.01f / velLen, dt);
				start -= mVelocity * newT;
				totalMotion -= velLen * newT;
			}

			// Try stepping if there is a vertical surface
			if (collisionList.getMaxHeight() < start.z + mDataBlock->maxStepHeight * scale.z) 
			{
				bool stepped = false;
				for (U32 c = 0; c < collisionList.getCount(); c++) 
				{
					const Collision& cp = collisionList[c];
					// if (mFabs(mDot(cp.normal,VectorF(0,0,1))) < sVerticalStepDot)
					//    Dot with (0,0,1) just extracts Z component [lh]-
					if (mFabs(cp.normal.z) < sVerticalStepDot)
					{
						stepped = step(&start,&maxStep,time);
						break;
					}
				}
				if (stepped)
				{
					continue;
				}
			}

			// Pick the surface most parallel to the face that was hit.
			const Collision *collision = &collisionList[0];
			const Collision *cp = collision + 1;
			const Collision *ep = collision + collisionList.getCount();
			for (; cp != ep; cp++)
			{
				if (cp->faceDot > collision->faceDot)
					collision = cp;
			}

			F32 bd = -mDot(mVelocity,collision->normal);

			// shake camera on ground impact
			if( bd > mDataBlock->groundImpactMinSpeed && isControlObject() )
			{
				if (mDataBlock->shakeId != 0)
				{
					gCamFXMgr.addFX( mDataBlock->shakeId );
				}
			}

			if (bd > mDataBlock->minImpactSpeed && !mMountPending) {
				if (!isGhost())
					onImpact(collision->object, collision->normal*bd);

				if (mDamageState == Enabled && mState != RecoverState) {
					// Scale how long we're down for
					F32   value = (bd - mDataBlock->minImpactSpeed);
					F32   range = (mDataBlock->minImpactSpeed * 0.9f);
					U32   recover = mDataBlock->recoverDelay;
					if (value < range)
						recover = 1 + S32(mFloor( F32(recover) * value / range) );
					// Con::printf("Used %d recover ticks", recover);
					// Con::printf("  minImpact = %g, this one = %g", mDataBlock->minImpactSpeed, bd);
					setState(RecoverState, recover);
				}
			}
			if (isServerObject() && bd > (mDataBlock->minImpactSpeed / 3.0f)) {
				mImpactSound = GameObjectData::ImpactNormal;
				setMaskBits(ImpactMask);
			}

			// Subtract out velocity
			VectorF dv = collision->normal * (bd + sNormalElasticity);
			mVelocity += dv;
			if (count == 0)
			{
				firstNormal = collision->normal;
			}
			else
			{
				if (count == 1)
				{
					// Re-orient velocity along the crease.
					if (mDot(dv,firstNormal) < 0.0f &&
						mDot(collision->normal,firstNormal) < 0.0f)
					{
						VectorF nv;
						mCross(collision->normal,firstNormal,&nv);
						F32 nvl = nv.len();
						if (nvl)
						{
							if (mDot(nv,mVelocity) < 0.0f)
								nvl = -nvl;
							nv *= mVelocity.len() / nvl;
							mVelocity = nv;
						}
					}
				}
			}

			// Track collisions
			if (!isGhost() && collision->object->getTypeMask() & ShapeBaseObjectType) {
				ShapeBase* col = static_cast<ShapeBase*>(collision->object);
				queueCollision(col,mVelocity - col->getVelocity());
			}
		}
		else
		{
			totalMotion += (end - start).len();
			start = end;
			break;
		}
	}

	if (count == sMoveRetryCount)
	{
		// Failed to move
		start        = initialPosition;
		initialSpeed = 0;
		mVelocity.set(0.0f, 0.0f, 0.0f);
	}

#ifndef NTJ_EDITOR
	if (!g_NavigationManager->CanReach(start))
	{
		// Failed to move
		start        = initialPosition;
		initialSpeed = 0;
		mVelocity.set(0.0f, 0.0f, 0.0f);
	}
#endif

	// Set new position
	// If on the client, calc delta for backstepping
	if (isClientObject())
	{
		delta.pos = start;
		delta.posVec = delta.posVec - delta.pos;
		delta.dt = 1.0f;
	}

	setPosition(start,mRot);
	setMaskBits(MoveMask);
	updateContainer();

	if (!isGhost())  {
		// Collisions are only queued on the server and can be
		// generated by either updateMove or updatePos
		notifyCollision();

		// Do mission area callbacks on the server as well
		//checkMissionArea();
	}

	// Check the totaldistance moved.  If it is more than 1000th of the velocity, then
	//  we moved a fair amount...
	if (totalMotion >= (0.001f * initialSpeed))
		return true;
	else
		return false;
}


//----------------------------------------------------------------------------

void GameObject::findContact(bool* run,bool* jump,VectorF* contactNormal)
{
	Point3F pos;
	getTransform().getColumn(3,&pos);

	Box3F wBox;
	Point3F exp(0,0,sTractionDistance);
	wBox.min = pos + mScaledBox.min - exp;
	wBox.max.x = pos.x + mScaledBox.max.x;
	wBox.max.y = pos.y + mScaledBox.max.y;
	wBox.max.z = pos.z + mScaledBox.min.z + sTractionDistance;

	static ClippedPolyList polyList;
	polyList.clear();
	polyList.doConstruct();
	polyList.mNormal.set(0.0f, 0.0f, 0.0f);
	polyList.setInterestNormal(Point3F(0.0f, 0.0f, -1.0f));

	polyList.mPlaneList.setSize(6);
	polyList.mPlaneList[0].setYZ(wBox.min, -1.0f);
	polyList.mPlaneList[1].setXZ(wBox.max, 1.0f);
	polyList.mPlaneList[2].setYZ(wBox.max, 1.0f);
	polyList.mPlaneList[3].setXZ(wBox.min, -1.0f);
	polyList.mPlaneList[4].setXY(wBox.min, -1.0f);
	polyList.mPlaneList[5].setXY(wBox.max, 1.0f);
	Box3F plistBox = wBox;

	// Expand build box as it will be used to collide with items.
	// PickupRadius will be at least the size of the box.
	F32 pd = (F32)mDataBlock->pickupDelta;
	wBox.min.x -= pd; wBox.min.y -= pd;
	wBox.max.x += pd; wBox.max.y += pd;
	wBox.max.z = pos.z + mScaledBox.max.z;

	GameObject * serverParent = NULL;
	if (bool(mServerObject))
	{
		serverParent = dynamic_cast<GameObject* >((NetObject *)mServerObject);
		GameConnection * con = serverParent->getControllingClient();
		if (con && !con->isAIControlled())
			serverParent = NULL;
	}

	// Build list from convex states here...
	CollisionWorkingList& rList = mConvex.getWorkingList();
	CollisionWorkingList* pList = rList.wLink.mNext;
	U32 mask = isGhost() ? sClientCollisionContactMask : sServerCollisionContactMask;
	while (pList != &rList)
	{
		Convex* pConvex = pList->mConvex;

		U32 objectMask = pConvex->getObject()->getTypeMask();

		// Check: triggers, corpses and items...
		//
		if (objectMask & TriggerObjectType)
		{
			Trigger* pTrigger = dynamic_cast<Trigger*>(pConvex->getObject());
			if (pTrigger)
			{
				pTrigger->potentialEnterObject(this);
				/*if(mTriggerId != pTrigger->getTriggerData()->mTriggerID)
					setTriggerID(pTrigger->getTriggerData()->mTriggerID);*/
			}
		}
		else if (objectMask & CorpseObjectType)
		{
			// If we've overlapped the worldbounding boxes, then that's it...
			if (getWorldBox().isOverlapped(pConvex->getObject()->getWorldBox()))
			{
				ShapeBase* col = static_cast<ShapeBase*>(pConvex->getObject());
				queueCollision(col,getVelocity() - col->getVelocity());
			}
		}
		//else if (objectMask & ItemObjectType)
		//{
		//	// If we've overlapped the worldbounding boxes, then that's it...
		//	Item* item = static_cast<Item*>(pConvex->getObject());
		//	if (getWorldBox().isOverlapped(item->getWorldBox()))
		//		if (this != item->getCollisionObject())
		//			queueCollision(item,getVelocity() - item->getVelocity());
		//}
		else if ((objectMask & mask) && !(objectMask & PhysicalZoneObjectType))
		{
			Box3F convexBox = pConvex->getBoundingBox();
			if (plistBox.isOverlapped(convexBox) && serverParent == NULL)
				pConvex->getPolyList(&polyList);
		}

		pList = pList->wLink.mNext;
	}

	if (serverParent)
	{
		// Just grab the info-
		const ContactInfo & info = serverParent->mContactInfo;
		*jump = info.jump;
		*run = info.run;
		if (info.contacted)
			*contactNormal = info.contactNormal;
		return;
	}

	if (!polyList.isEmpty())
	{
		// Pick flattest surface
		F32 bestVd = -1.0f;
		ClippedPolyList::Poly* poly = polyList.mPolyList.begin();
		ClippedPolyList::Poly* end = polyList.mPolyList.end();
		for (; poly != end; poly++)
		{
			F32 vd = poly->plane.z;       // i.e.  mDot(Point3F(0,0,1), poly->plane);
			if (vd > bestVd)
			{
				bestVd = vd;
				*contactNormal = poly->plane;
			}
		}
		*run  = bestVd > mDataBlock->runSurfaceCos;
		*jump = bestVd > mDataBlock->jumpSurfaceCos;
	}
	else
		*jump = *run = false;

	// Save the info for client peeking hack-
	mContactInfo.clear();
	mContactInfo.contacted = !polyList.isEmpty();
	if (mContactInfo.contacted)
		mContactInfo.contactNormal = *contactNormal;
	mContactInfo.run = *run;
	mContactInfo.jump = *jump;
}

//----------------------------------------------------------------------------

void GameObject::checkMissionArea()
{
	// Checks to see if the player is in the Mission Area...
	Point3F pos;
	MissionArea * obj = dynamic_cast<MissionArea*>(Sim::findObject("GlobalMissionArea"));

	if(!obj)
		return;

	const RectI &area = obj->getArea();
	getTransform().getColumn(3, &pos);

	if ((pos.x < area.point.x || pos.x > area.point.x + area.extent.x ||
		pos.y < area.point.y || pos.y > area.point.y + area.extent.y)) {
			if(mInMissionArea) {
				mInMissionArea = false;
				Con::executef(mDataBlock, "onLeaveMissionArea",scriptThis());
			}
		}
	else if(!mInMissionArea)
	{
		mInMissionArea = true;
		Con::executef(mDataBlock, "onEnterMissionArea",scriptThis());
	}
}


//----------------------------------------------------------------------------

bool GameObject::isDisplacable() const
{
	return true;
}

Point3F GameObject::getMomentum() const
{
	return mVelocity * getMass();
}

void GameObject::setMomentum(const Point3F& newMomentum)
{
	Point3F newVelocity = newMomentum / getMass();
	mVelocity = newVelocity;
}

F32 GameObject::getMass() const
{
	if( mDataBlock )
		return mDataBlock->mass;

	// AFAICT the value of mMass never gets assigned from the player datablock. [5/24/2007 Pat]
	return mMass;
}

#define  LH_HACK   1
// Hack for short-term soln to Training crash -
#if   LH_HACK
static U32  sBalance;

bool GameObject::displaceObject(const Point3F& displacement)
{
	F32 vellen = mVelocity.len();
	if (vellen < 0.001f || sBalance > 16) {
		mVelocity.set(0.0f, 0.0f, 0.0f);
		return false;
	}

	F32 dt = displacement.len() / vellen;

	sBalance++;

	bool result = updatePos(dt);

	sBalance--;

	getTransform().getColumn(3, &delta.pos);
	delta.posVec.set(0.0f, 0.0f, 0.0f);

	return result;
}

#else

bool GameObject::displaceObject(const Point3F& displacement)
{
	F32 vellen = mVelocity.len();
	if (vellen < 0.001f) {
		mVelocity.set(0.0f, 0.0f, 0.0f);
		return false;
	}

	F32 dt = displacement.len() / vellen;

	bool result = updatePos(dt);

	mObjToWorld.getColumn(3, &delta.pos);
	delta.posVec.set(0.0f, 0.0f, 0.0f);

	return result;
}

#endif

//----------------------------------------------------------------------------

void GameObject::setPosition(const Point3F& pos,const Point3F& rot)
{
	MatrixF mat;
	if (isMounted()) {
		// Use transform from mounted object
		MatrixF nmat,tmat;
		mMount.object->getMountTransform(mMount.node,&nmat);
		getLocalMountTransform(mMount.myNode,&tmat);
		tmat.inverse();
		mat.mul(nmat, tmat);
	}
	else {
		mat.set(EulerF(0.0f, 0.0f, rot.z));
		mat.setColumn(3,pos);
	}
	Parent::setTransform(mat);
	mRot = rot;
}


void GameObject::setRenderPosition(const Point3F& pos, const Point3F& rot, F32 dt)
{
	MatrixF mat;
	if (isMounted()) {
		// Use transform from mounted object
		MatrixF nmat,tmat;
		mMount.object->getRenderMountTransform_(mMount.node,&nmat);
		getLocalMountTransform(mMount.myNode,&tmat);
		tmat.inverse();
		mat.mul(nmat, tmat);
		//mat.mul(mMount.object->getRenderTransform());
	}
	else {
		EulerF   orient(0.0f, 0.0f, rot.z);

		mat.set(orient);
		mat.setColumn(3, pos);

		// 会有抖动，暂时屏蔽 [6/11/2009 joy]
		//if (inDeathAnim()) {
		//	F32   boxRad = (mDataBlock->boxSize.x * 0.5f);
		//	if (MatrixF * fallMat = mDeath.fallToGround(dt, pos, rot.z, boxRad))
		//		mat = * fallMat;
		//}
		//else
		//	mDeath.initFall();
	}
	#pragma message(ENGINE(记录平滑变量))
	if(dt >= 0)
		mRenderRot = rot.z - mRot.z;
	Parent::setRenderTransform(mat);
}

//----------------------------------------------------------------------------
#pragma message(ENGINE(设置人物和摄像机方向))
void GameObject::setRotation(F32 rotZ, bool bCameraDragged /* = false */)
{
	MatrixF mat;
	if (isMounted()) {
		// Use transform from mounted object
		MatrixF nmat,zrot;
		mMount.object->getMountTransform(mMount.node,&nmat);
		zrot.set(EulerF(0.0f, 0.0f, rotZ));
		mat.mul(nmat,zrot);
	}
	else {
		mat.set(EulerF(0.0f, 0.0f, rotZ));
		mat.setColumn(3,getPosition());
	}
	setTransform(mat);
	mRot.z = rotZ;

	if(bCameraDragged)
	{
		delta.rot = mRot;
		delta.rotVec.z = 0;
		setRenderPosition(getPosition(),mRot);
	}
}
//----------------------------------------------------------------------------

void GameObject::setTransform(const MatrixF& mat)
{
	// This method should never be called on the client.

	// This currently converts all rotation in the mat into
	// rotations around the z axis.
	Point3F pos,vec;
	mat.getColumn(1,&vec);
	mat.getColumn(3,&pos);
	Point3F rot(0.0f, 0.0f, -mAtan(-vec.x,vec.y));
	setPosition(pos,rot);
	setMaskBits(MoveMask | NoWarpMask);
}

void GameObject::setTransform(const Point3F& pos,const QuatF& q)
{
	static MatrixF mat;
	if (isMounted()) {
		// Use transform from mounted object
		MatrixF nmat,tmat;
		mMount.object->getMountTransform(mMount.node,&nmat);
		getLocalMountTransform(mMount.myNode,&tmat);
		tmat.inverse();
		mat.mul(nmat, tmat);
	}
	else {
		q.setMatrix(&mat);
		mat.setPosition(pos);
	}
	Parent::setTransform(mat);
	mat.getColumn(1, &mRot);
	mRot.z = -mAtan(-mRot.x, mRot.y);
	mRot.x = mRot.y = 0;

	setMaskBits(MoveMask | NoWarpMask);
}

void GameObject::setRenderTransform(const Point3F& pos,const QuatF& q)
{
	static MatrixF mat;
	if (isMounted()) {
		// Use transform from mounted object
		MatrixF nmat,tmat;
		mMount.object->getRenderMountTransform_(mMount.node,&nmat);
		getLocalMountTransform(mMount.myNode,&tmat);
		tmat.inverse();
		mat.mul(nmat, tmat);
	}
	else {
		q.setMatrix(&mat);
		mat.setPosition(pos);
	}

	if(getControllingClient() && !getControlObject())
	{
		Point3F rot;
		mat.getColumn(1, &rot);
		rot.z = -mAtan(-rot.x, rot.y);
		mRenderRot = rot.z - mRot.z;
	}

	Parent::setRenderTransform(mat);
}

void GameObject::getEyeTransform(MatrixF* mat)
{
	// Eye transform in world space.  We only use the eye position
	// from the animation and supply our own rotation.
	MatrixF pmat,xmat,zmat;
	xmat.set(EulerF(0.0f/*mHead.x*/, 0.0f, 0.0f));
	zmat.set(EulerF(0.0f, 0.0f, 0.0f/*mHead.z*/));
	pmat.mul(zmat,xmat);

	F32 *dp = pmat;

	F32* sp;
	MatrixF eyeMat(true);
	if (mDataBlock->eyeNode != -1)
	{
		sp = mShapeInstance->getNodeTransforms()[mDataBlock->eyeNode];
	}
	else
	{
		Point3F center;
		mObjBox.getCenter(&center);
		eyeMat.setPosition(center);
		sp = eyeMat;
	}

	const Point3F& scale = getScale();
	dp[3] = sp[3] * scale.x;
	dp[7] = sp[7] * scale.y;
	dp[11] = sp[11] * scale.z;
	mat->mul(getTransform(),pmat);
}

void GameObject::getRenderEyeTransform(MatrixF* mat)
{
	// Eye transform in world space.  We only use the eye position
	// from the animation and supply our own rotation.
	MatrixF pmat,xmat,zmat;
	#pragma message(ENGINE(取得摄像机的偏移，注意需平滑处理))
#ifdef NTJ_CLIENT
	xmat.set(EulerF(dGuiMouseGamePlay::GetCameraPitch(), 0.0f, 0.0f));
	zmat.set(EulerF(0.0f, 0.0f, MoveManager::mFreeLook ? dGuiMouseGamePlay::GetCameraYaw()-mRenderRot : dGuiMouseGamePlay::GetCameraYaw()));
#endif
#ifdef NTJ_EDITOR
	xmat.set(EulerF(dWorldEditor::GetCameraPitch(), 0.0f, 0.0f));
	zmat.set(EulerF(0.0f, 0.0f, MoveManager::mFreeLook ? dWorldEditor::GetCameraYaw()-mRenderRot : dWorldEditor::GetCameraYaw()));
#endif
#ifdef NTJ_SERVER
	xmat.set(EulerF(0.0f, 0.0f, 0.0f));
	zmat.set(EulerF(0.0f, 0.0f, 0.0f));
#endif
	pmat.mul(zmat,xmat);

	F32 *dp = pmat;

	F32* sp;
	MatrixF eyeMat(true);
	if (mDataBlock->eyeNode != -1)
	{
		sp = mShapeInstance->getNodeTransforms()[mDataBlock->eyeNode];
	}
	else
	{
		Point3F center;
		mObjBox.getCenter(&center);
		eyeMat.setPosition(center);
		sp = eyeMat;
	}

	const Point3F& scale = getScale();
	dp[3] = sp[3] * scale.x;
	dp[7] = sp[7] * scale.y;
	dp[11] = sp[11] * scale.z;

	// disable pitch & roll
	Point3F pos, rot;
	*mat = getRenderTransform();
	mat->getColumn(1, &rot);
	rot.z = -mAtan(-rot.x, rot.y);
	pos = mat->getPosition();
	mat->set(EulerF(0.0f, 0.0f, rot.z));
	mat->setColumn(3,pos);

	mat->mul(pmat);
}

void GameObject::getCameraParameters(F32 *min,F32* max,Point3F* off,MatrixF* rot)
{
	if (!mControlObject.isNull() && mControlObject == getObjectMount()) {
		mControlObject->getCameraParameters(min,max,off,rot);
		return;
	}
	const Point3F& scale = getScale();
	*min = mDataBlock->cameraMinDist * scale.y;
	*max = mDataBlock->cameraMaxDist * scale.y;
	off->set(0.0f, 0.0f, 0.0f);
	rot->identity();
}


//----------------------------------------------------------------------------

Point3F GameObject::getVelocity() const
{
	return mVelocity;
}

void GameObject::setVelocity(const VectorF& vel)
{
	mVelocity = vel;
	setMaskBits(MoveMask);
}

void GameObject::applyImpulse(const Point3F&,const VectorF& vec)
{
	// Players ignore angular velocity
	VectorF vel;
	vel.x = vec.x / getMass();
	vel.y = vec.y / getMass();
	vel.z = vec.z / getMass();
	setVelocity(mVelocity + vel);
}

//----------------------------------------------------------------------------

bool GameObject::castRay(const Point3F &start, const Point3F &end, RayInfo* info)
{
	if (isDisabled())
		return false;

	// Collide against bounding box. Need at least this for the editor.
	F32 st,et,fst = 0.0f,fet = 1.0f;
	F32 *bmin = &mObjBox.min.x;
	F32 *bmax = &mObjBox.max.x;
	F32 const *si = &start.x;
	F32 const *ei = &end.x;

	for (int i = 0; i < 3; i++) {
		if (*si < *ei) {
			if (*si > *bmax || *ei < *bmin)
				return false;
			F32 di = *ei - *si;
			st = (*si < *bmin)? (*bmin - *si) / di: 0.0f;
			et = (*ei > *bmax)? (*bmax - *si) / di: 1.0f;
		}
		else {
			if (*ei > *bmax || *si < *bmin)
				return false;
			F32 di = *ei - *si;
			st = (*si > *bmax)? (*bmax - *si) / di: 0.0f;
			et = (*ei < *bmin)? (*bmin - *si) / di: 1.0f;
		}
		if (st > fst) fst = st;
		if (et < fet) fet = et;
		if (fet < fst)
			return false;
		bmin++; bmax++;
		si++; ei++;
	}

	info->normal = start - end;
	info->normal.normalizeSafe();
	getTransform().mulV( info->normal );

	info->t = fst;
	info->object = this;
	info->point.interpolate(start,end,fst);
	info->material = 0;
	return true;
}


//----------------------------------------------------------------------------

static MatrixF IMat(1);

bool GameObject::buildPolyList(AbstractPolyList* polyList, const Box3F&, const SphereF&)
{
	// Collision with the player is always against the player's object
	// space bounding box axis aligned in world space.
	Point3F pos;
	getTransform().getColumn(3,&pos);
	IMat.setColumn(3,pos);
	polyList->setTransform(&IMat, Point3F(1.0f,1.0f,1.0f));
	polyList->setObject(this);
	polyList->addBox(mObjBox);
	return true;
}


void GameObject::buildConvex(const Box3F& box, Convex* convex)
{
	if (mShapeInstance == NULL)
		return;

	// These should really come out of a pool
	mConvexList->collectGarbage();

	Box3F realBox = box;
	mWorldToObj.mul(realBox);
	realBox.min.convolveInverse(mObjScale);
	realBox.max.convolveInverse(mObjScale);

	if (realBox.isOverlapped(getObjBox()) == false)
		return;

	Convex* cc = 0;
	CollisionWorkingList& wl = convex->getWorkingList();
	for (CollisionWorkingList* itr = wl.wLink.mNext; itr != &wl; itr = itr->wLink.mNext) {
		if (itr->mConvex->getType() == BoxConvexType &&
			itr->mConvex->getObject() == this) {
				cc = itr->mConvex;
				break;
			}
	}
	if (cc)
		return;

	// Create a new convex.
	// OrthoBoxConvex 没有旋转，但是BoxConvex可能引起精度等问题
	BoxConvex* cp = new BoxConvex;
	mConvexList->registerObject(cp);
	convex->addToWorkingList(cp);
	cp->init(this);

	mObjBox.getCenter(&cp->mCenter);
	cp->mSize.x = mObjBox.len_x() / 2.0f;
	cp->mSize.y = mObjBox.len_y() / 2.0f;
	cp->mSize.z = mObjBox.len_z() / 2.0f;
}


//----------------------------------------------------------------------------

bool GameObject::collideBox(const Point3F &start, const Point3F &end, RayInfo* info)
{
#ifdef NTJ_CLIENT
	if(mVisibility < 0.01f)
		return false;
#endif

	return Parent::collideBox(start, end, info);
}

void GameObject::updateWorkingCollisionSet()
{
	// First, we need to adjust our velocity for possible acceleration.  It is assumed
	// that we will never accelerate more than 20 m/s for gravity, plus 10 m/s for
	// jetting, and an equivalent 10 m/s for jumping.  We also assume that the
	// working list is updated on a Tick basis, which means we only expand our
	// box by the possible movement in that tick.
	Point3F scaledVelocity = mVelocity * TickSec;
	F32 len    = scaledVelocity.len();
	F32 newLen = len + (10.0f * TickSec);

	// Check to see if it is actually necessary to construct the new working list,
	// or if we can use the cached version from the last query.  We use the x
	// component of the min member of the mWorkingQueryBox, which is lame, but
	// it works ok.
	bool updateSet = false;

	Box3F convexBox = mConvex.getBoundingBox(getTransform(), getScale());
	F32 l = (newLen * 1.1f) + 0.1f;  // from Convex::updateWorkingList
	const Point3F  lPoint( l, l, l );
	convexBox.min -= lPoint;
	convexBox.max += lPoint;

	// Check containment
	if (mWorkingQueryBox.min.x != -1e9f)
	{
		if (mWorkingQueryBox.isContained(convexBox) == false)
			// Needed region is outside the cached region.  Update it.
			updateSet = true;
	}
	else
	{
		// Must update
		updateSet = true;
	}
	// Actually perform the query, if necessary
	if (updateSet == true) {
		const Point3F  twolPoint( 2.0f * l, 2.0f * l, 2.0f * l );
		mWorkingQueryBox = convexBox;
		mWorkingQueryBox.min -= twolPoint;
		mWorkingQueryBox.max += twolPoint;

		disableCollision();
		NetConnection* pConn = getNetConnection();
		if( !( pConn && pConn->isRobotConnection() ) )
		mConvex.updateWorkingList(mWorkingQueryBox,
			isGhost() ? sClientCollisionContactMask : sServerCollisionContactMask);
		enableCollision();
	}
}


//----------------------------------------------------------------------------

void GameObject::writePacketData(GameConnection *connection, BitStream *stream)
{
	Parent::writePacketData(connection, stream);

	stream->writeInt(mState,NumStateBits);
	if (stream->writeFlag(mState == RecoverState))
		stream->writeInt(mRecoverTicks,GameObjectData::RecoverDelayBits);
	if (stream->writeFlag(mJumpDelay > 0))
		stream->writeInt(mJumpDelay,GameObjectData::JumpDelayBits);

	Point3F pos;
	getTransform().getColumn(3,&pos);
	//if (stream->writeFlag(!isMounted())) {
		// Will get position from mount
		stream->setCompressionPoint(pos);
		stream->write(pos.x);
		stream->write(pos.y);
		stream->write(pos.z);
		stream->write(mVelocity.x);
		stream->write(mVelocity.y);
		stream->write(mVelocity.z);
		stream->writeInt(mJumpSurfaceLastContact > 15 ? 15 : mJumpSurfaceLastContact, 4);
	//}
	//stream->write(mHead.x);
	//stream->write(mHead.z);
	stream->write(mRot.z);
	#pragma message(ENGINE(旋转角速度也需要更新，这样人物转动会更平滑))
	stream->write(delta.rotVec.z);
	stream->write(mFlyPath.pathId);
	stream->write(mFlyPath.flyPos);
	stream->writeFlag(mFlyPath.atEnd);

	if (mControlObject) {
		S32 gIndex = connection->getGhostIndex(mControlObject);
		if (stream->writeFlag(gIndex != -1)) {
			stream->writeInt(gIndex,NetConnection::GhostIdBitSize);
			mControlObject->writePacketData(connection, stream);
		}
	}
	else
		stream->writeFlag(false);
}


void GameObject::readPacketData(GameConnection *connection, BitStream *stream)
{
	Parent::readPacketData(connection, stream);

	mState = (ActionState)stream->readInt(NumStateBits);
	if (stream->readFlag())
		mRecoverTicks = stream->readInt(GameObjectData::RecoverDelayBits);
	if (stream->readFlag())
		mJumpDelay = stream->readInt(GameObjectData::JumpDelayBits);
	else
		mJumpDelay = 0;

	Point3F pos,rot;
	//if (stream->readFlag()) {
		// Only written if we are not mounted
		stream->read(&pos.x);
		stream->read(&pos.y);
		stream->read(&pos.z);
		stream->read(&mVelocity.x);
		stream->read(&mVelocity.y);
		stream->read(&mVelocity.z);
		stream->setCompressionPoint(pos);
		delta.pos = pos;
		mJumpSurfaceLastContact = stream->readInt(4);
	//}
	//else
	//	pos = delta.pos;
	//stream->read(&mHead.x);
	//stream->read(&mHead.z);
	stream->read(&rot.z);
	rot.x = rot.y = 0;
	setPosition(pos,rot);
	//delta.head = mHead;
	delta.rot = rot;
	stream->read(&delta.rotVec.z);
	stream->read(&mFlyPath.pathId);
	stream->read(&mFlyPath.flyPos);
	mFlyPath.atEnd = stream->readFlag();

	if (stream->readFlag()) {
		S32 gIndex = stream->readInt(NetConnection::GhostIdBitSize);
		ShapeBase* obj = static_cast<ShapeBase*>(connection->resolveGhost(gIndex));
		setControlObject(obj);
		obj->readPacketData(connection, stream);
	}
	else
		setControlObject(0);
}

U64 GameObject::packUpdate(NetConnection *con, U64 mask, BitStream *stream)
{
	U64 retMask = Parent::packUpdate(con, mask, stream);

	if (stream->writeFlag((mask & ImpactMask) && !(mask & InitialUpdateMask)))
		stream->writeInt(mImpactSound, GameObjectData::ImpactBits);

	// 发送手持武器的状态，这决定了动作
	if (stream->writeFlag(mask & ImageMask))
		stream->writeInt(m_ArmStatus, ArmStatusBits);

	// 这里只需要更新动作的上半部，因为下部的动作会跟人物的运动状态关联
	if (stream->writeFlag(mask & ActionMask &&
		mUpperActionAnimation.action != GameObjectData::NullAnimation && !IsReadyAnim(mUpperActionAnimation.action) &&
		(mUpperActionAnimation.action >= GameObjectData::Skip || IsIdleAnim(mUpperActionAnimation.action))))
	{
		stream->writeInt(mUpperActionAnimation.action,GameObjectData::ActionAnimBits);
		stream->writeFlag(mUpperActionAnimation.holdAtEnd);
		stream->writeFlag(mUpperActionAnimation.firstPerson);
		// 对于已经死亡的对象，需要发一个标志下去，因为服务器不会推进动作，所以atEnd的值是不准确的 [6/11/2009 joy]
		stream->writeFlag(mUpperActionAnimation.action == GameObjectData::Death && (mask & InitialUpdateMask));

		packAnimateEP(con, mask, stream);
	}

	if (mask & CtrlMask)
	{
		if (mControlObject)
		{
			S32 gIndex = con->getGhostIndex(mControlObject);
			if (stream->writeFlag(gIndex != -1)) {
				stream->writeFlag(true);
				stream->writeInt(gIndex,NetConnection::GhostIdBitSize);
			}
			else
				// Will have to try again later
				retMask |= CtrlMask;
		}
		// Unmount if this isn't the initial packet
		else if (stream->writeFlag(!(mask & InitialUpdateMask)))
			stream->writeFlag(false);
	}
	else
		stream->writeFlag(false);

	if (stream->writeFlag(mask & BaseInfoMask))
	{
		stream->writeFlag(m_Sex == SEX_MALE);
		stream->writeInt(m_Race, RaceBits);
		stream->writeInt(m_Family, FamilyBits);
		stream->writeInt(m_Influence, Base::Bit8);
		stream->writeInt(m_Level, LevelBits);
		stream->writeInt(m_Classes[_1st], ClassBits);
		stream->writeInt(m_Classes[_2nd], ClassBits);
		stream->writeInt(m_Classes[_3rd], ClassBits);
		stream->writeInt(m_Classes[_4th], ClassBits);
		stream->writeInt(m_Classes[_5th], ClassBits);
		stream->writeFlag(isCombative());
	}
	if (stream->writeFlag(mask & StatsMask))
	{
		if (stream->writeFlag(isPackWholeStats(con)))
			m_BuffTable.packStats(stream);
		else
			m_BuffTable.packMaxStats(stream);
	}
	if (stream->writeFlag(mask & SpeedMask))
		m_BuffTable.packSpeed(stream);
	if (stream->writeFlag(mask & BuffMask))
	{
		m_BuffTable.packBuff(stream);
	}
	if (stream->writeFlag(mask & SpellMask))
	{
		m_Spell.Pack(con, stream);
	}
	if (stream->writeFlag(mask & ChainEPMask))
	{
		m_Spell.PackChainEP(con, stream);
	}
	if (stream->writeFlag(getControllingClient() != con && (mask & TargetMask)))
	{
		if(stream->writeFlag(GetTarget()))
		{
			stream->writeInt(con->getGhostIndex(GetTarget()), NetConnection::GhostIdBitSize);
			stream->writeFlag(IsAutoCastEnable());
		}
	}
	if (stream->writeFlag(mask & HPMask))
	{
		stream->writeInt(GetStats().HP, BuffTable::StatsBits);
		stream->writeFlag(isInCombat());
	}
	if (stream->writeFlag(mask & MPMask))
		stream->writeInt(GetStats().MP, BuffTable::StatsBits);
	if (stream->writeFlag(mask & PPMask))
		stream->writeInt(GetStats().PP, BuffTable::StatsBits);

	// 处理头顶冒泡文字
	if( stream->writeFlag( mask & ChatInfoMask ) )
		stream->writeString( mChatInfo );

	// The rest of the data is part of the control object packet update.
	// If we're controlled by this client, we don't need to send it.
	// we only need to send it if this is the initial update - in that case,
	// the client won't know this is the control object yet.
	if(stream->writeFlag(getControllingClient() == con && !(mask & InitialUpdateMask)))
		return(retMask);

	if (stream->writeFlag(mask & MoveMask))
	{
		stream->writeFlag(mFalling);

		stream->writeInt(mState,NumStateBits);
		if (stream->writeFlag(mState == RecoverState))
			stream->writeInt(mRecoverTicks,GameObjectData::RecoverDelayBits);

		Point3F pos;
		getTransform().getColumn(3,&pos);
		stream->writeCompressedPoint(pos);
		F32 len = mVelocity.len();
		if(stream->writeFlag(len > 0.02f))
		{
			Point3F outVel = mVelocity;
			outVel *= 1.0f/len;
			stream->writeNormalVector(outVel, 10);
			len *= 32.0f;  // 5 bits of fraction
			if(len > 8191)
				len = 8191;
			stream->writeInt((S32)len, 13);
		}
		stream->writeFloat(mRot.z / M_2PI_F, 10);
		//stream->writeSignedFloat(mHead.x / mDataBlock->maxLookAngle, 6);
		//stream->writeSignedFloat(mHead.z / mDataBlock->maxLookAngle, 6);

		//Ray: 下面两句同时需要
		delta.move.pack(stream);
		Move::mCompressPoint = NULL;

		stream->writeFlag(!(mask & NoWarpMask));
	}



	return retMask;
}

void GameObject::unpackUpdate(NetConnection *con, BitStream *stream)
{
	Parent::unpackUpdate(con,stream);

	if (stream->readFlag())
		mImpactSound = stream->readInt(GameObjectData::ImpactBits);

	if (stream->readFlag())
	{
		m_ArmStatus = (GameObjectData::ArmStatus)stream->readInt(ArmStatusBits);
#ifdef NTJ_CLIENT
		g_ClientGameplayState->refreshPlayerModelAction();
#endif
	}

	// Server specified action animation
	if (stream->readFlag()) {
		U32 action = stream->readInt(GameObjectData::ActionAnimBits);
		bool hold = stream->readFlag();
		bool fsp = stream->readFlag();
		// 已经死亡的对象，设置死亡动作的位置 [6/11/2009 joy]
		// 为什么这里的isProperlyAdded()会为false
		if(stream->readFlag() && !isProperlyAdded() && mUpperActionAnimation.thread && mLowerActionAnimation.thread && inDeathAnim())
		{
			mShapeInstance->clearTransition(mUpperActionAnimation.thread);
			mShapeInstance->clearTransition(mLowerActionAnimation.thread);
			mShapeInstance->setPos(mUpperActionAnimation.thread, 1.0f);
			mShapeInstance->setPos(mLowerActionAnimation.thread, 1.0f);
		}

		if (isProperlyAdded()) {
			setActionThread(action,true,hold,false,fsp,false);
			// mMountPending suppresses tickDelay countdown so players will sit until
			// their mount, or another animation, comes through (or 13 seconds elapses).
			mMountPending = (S32) (inSittingAnim() ? sMountPendingTickWait : 0);
		}
		unpackAnimateEP(con, stream);
	}

	if (stream->readFlag())
	{
		if (stream->readFlag())
		{
			S32 gIndex = stream->readInt(NetConnection::GhostIdBitSize);
			ShapeBase* obj = static_cast<ShapeBase*>(con->resolveGhost(gIndex));
			if(!obj)
			{
				con->setLastError("Invalid packet from server.");
				return;
			}
			setControlObject(obj);
		}
		else
			setControlObject(0);
	}

	if (stream->readFlag())
	{
		m_Sex = stream->readFlag() ? SEX_MALE : SEX_FEMALE;
		m_Race = (enRace)stream->readInt(RaceBits);
		m_Family = (enFamily)stream->readInt(FamilyBits);
		setInfluence((enInfluence)stream->readInt(Base::Bit8));
		U32 nNewLevel = mClamp(stream->readInt(LevelBits), 1, MaxPlayerLevel);
#ifdef NTJ_CLIENT
		Player* obj = dynamic_cast<Player*>(this);
		if( nNewLevel != m_Level && this == (GameObject*)g_ClientGameplayState->GetControlPlayer() )
		{
			Con::executef("ShowPlayerExp",Con::getIntArg(getExp()),Con::getIntArg(g_LevelExp[nNewLevel]));
			Con::executef("PlayPlayerLeverUpSound");

			//角色升级事件
			if(obj)
				Con::executef("ClientEventLevelUp",Con::getIntArg(obj->getPlayerID()));
		}	
		
		if (obj && CLIENT_TEAM->IsTeammate(obj->getPlayerID()))
		{
			Con::executef("GetTeammateListInfo");
		}

#endif
 		m_Level = nNewLevel;
		m_Classes[_1st] = stream->readInt(ClassBits);
		m_Classes[_2nd] = stream->readInt(ClassBits);
		m_Classes[_3rd] = stream->readInt(ClassBits);
		m_Classes[_4th] = stream->readInt(ClassBits);
		m_Classes[_5th] = stream->readInt(ClassBits);
		setCombative(stream->readFlag());

		if(getControllingClient())
			SetRefreshGui(true);
	}
	if (stream->readFlag())
	{
		if(stream->readFlag())
		{
			m_BuffTable.unpackStats(stream);
			OnStatsChanged();
		}
		else
			m_BuffTable.unpackMaxStats(stream);
		SetRefreshGui(true);
	}
	if (stream->readFlag())
		m_BuffTable.unpackSpeed(stream);
	if (stream->readFlag())
	{
		m_BuffTable.unpackBuff(stream);
	}
	if (stream->readFlag())
	{
		m_Spell.Unpack(con, stream);
	}
	if (stream->readFlag())
	{
		m_Spell.UnpackChainEP(con, stream);
	}
	if (stream->readFlag())
	{
		if(stream->readFlag())
		{
			S32 id = stream->readInt(NetConnection::GhostIdBitSize);
			GameObject* obj = dynamic_cast<GameObject*>(con->resolveGhost(id));
			SetTarget(obj);
			SetAutoCastEnable(stream->readFlag());
		}
		else
			SetTarget(NULL);
	}
	if (stream->readFlag())
	{
		setHP(stream->readInt(BuffTable::StatsBits));
		setInCombat(stream->readFlag());
		if(getControllingClient() == con)
			SetRefreshGui(true);
	}
	if (stream->readFlag())
	{
		setMP(stream->readInt(BuffTable::StatsBits));
		if(getControllingClient() == con)
			SetRefreshGui(true);
	}
	if (stream->readFlag())
	{
		setPP(stream->readInt(BuffTable::StatsBits));
		if(getControllingClient() == con)
			SetRefreshGui(true);
	}

	if( stream->readFlag() )
	{
		char szOldChatInfo[CHAT_WORD_MAX_LENGTH] = {0,};
		dMemcpy(szOldChatInfo,mChatInfo,CHAT_WORD_MAX_LENGTH);

		stream->readString( mChatInfo ,256);
#ifdef NTJ_CLIENT
        if(dStrcmp(szOldChatInfo,mChatInfo) && mChatInfo[0] != '\0')
        {
            bool BanorNot = 0;
            for (std::list<std::string>::iterator it = g_ClientGameplayState->m_BanNamelist.begin(); it != g_ClientGameplayState->m_BanNamelist.end(); ++it)
            {
                std::string temp = getObjectName();
                if (temp == (*it))
                {
                    BanorNot = 1;
                    break;
                }
            }
            if (BanorNot == 0)
            {
                mChatDirty = true;
                mChatLastUpdateTime = Platform::getVirtualMilliseconds();
                Con::executef( "AddChatMessage", "10", mChatInfo, "0", getObjectName() );
            }
        }
#endif
    }

	// controlled by the client?
	if(stream->readFlag())
		return;

	if (stream->readFlag()) {
		mPredictionCount = sMaxPredictionTicks;
		mFalling = stream->readFlag();

		ActionState actionState = (ActionState)stream->readInt(NumStateBits);
		if (stream->readFlag()) {
			mRecoverTicks = stream->readInt(GameObjectData::RecoverDelayBits);
			setState(actionState, mRecoverTicks);
		}
		else
			setState(actionState);

		Point3F pos,rot;
		stream->readCompressedPoint(&pos);
		F32 speed = mVelocity.len();
		if(stream->readFlag())
		{
			stream->readNormalVector(&mVelocity, 10);
			mVelocity *= stream->readInt(13) / 32.0f;
		}
		else
		{
			mVelocity.set(0.0f, 0.0f, 0.0f);
		}

		rot.y = rot.x = 0.0f;
		rot.z = stream->readFloat(10) * M_2PI_F;
		//mHead.x = stream->readSignedFloat(6) * mDataBlock->maxLookAngle;
		//mHead.z = stream->readSignedFloat(6) * mDataBlock->maxLookAngle;
		delta.move.unpack(stream);
		//rot.z = delta.move.mRotationZ;

		//delta.head = mHead;
		delta.headVec.set(0.0f, 0.0f, 0.0f);

		// 
		mJumpPath.jumping = delta.move.jumping;
		mJumpPath.fall = delta.move.fall;
		if(delta.move.flyPathId != mFlyPath.pathId)
		{
			mFlyPath.pathId = delta.move.flyPathId;
			mFlyPath.flyPos = delta.move.flyPos;
			mFlyPath.atEnd = delta.move.flyAtEnd;
		}
		S32 tMaxWarpTicks = (getGameObjectMask() & PlayerObjectType) ? sMaxWarpTicks_Rot : sMaxWarpTicks;
		if (stream->readFlag() && isProperlyAdded() && mFadeVal > 0.01f)
		{
			// Determine number of ticks to warp based on the average
			// of the client and server velocities.
			delta.warpOffset = pos - delta.pos;
			F32 as = (speed + mVelocity.len()) * 0.5f * TickSec;
			F32 dt = (as > 0.00001f) ? delta.warpOffset.len() / as: tMaxWarpTicks;
			delta.warpTicks = (S32)((dt > sMinWarpTicks) ? getMax(mFloor(dt + 0.5f), 1.0f) : 0.0f);

			if(delta.warpOffset.lenSquared() < 0.01f)
				delta.warpTicks = 0;
			if (delta.warpTicks)
			{
				// Setup the warp to start on the next tick.
				//if (delta.warpTicks > tMaxWarpTicks)
				//	delta.warpTicks = tMaxWarpTicks;
				delta.warpTicks = mClamp(delta.warpTicks, sMaxWarpTicks_Rot, tMaxWarpTicks);
				delta.warpOffset /= (F32)delta.warpTicks;

				// head保存最新的朝向数据
				delta.head = rot;
				delta.warpRotTicks = getMin(delta.warpTicks, sMaxWarpTicks_Rot);
				F32 fRot = (getGameObjectMask() & PlayerObjectType) ? rot.z : mAtan(delta.warpOffset.x, delta.warpOffset.y);
				while(fRot < 0)
					fRot += M_2PI_F;
				while(fRot >= M_2PI_F)
					fRot -= M_2PI_F;
				delta.rotOffset.set(0,0, fRot - delta.rot.z);
				if(delta.rotOffset.z < - M_PI_F)
					delta.rotOffset.z += M_2PI_F;
				else if(delta.rotOffset.z > M_PI_F)
					delta.rotOffset.z -= M_2PI_F;
				delta.rotOffset /= (F32)delta.warpRotTicks;
			}
			else
			{
				// Going to skip the warp, server and client are real close.
				// Adjust the frame interpolation to move smoothly to the
				// new position within the current tick.
				Point3F cp = delta.pos + delta.posVec * delta.dt;
				if (delta.dt == 0) 
				{
					delta.posVec.set(0.0f, 0.0f, 0.0f);
					delta.rotVec.set(0.0f, 0.0f, 0.0f);
				}
				else
				{
					F32 dti = 1.0f / delta.dt;
					delta.posVec = (cp - pos) * dti;
					delta.rotVec.z = mRot.z - rot.z;

					if(delta.rotVec.z > M_PI_F)
						delta.rotVec.z -= M_2PI_F;
					else if(delta.rotVec.z < -M_PI_F)
						delta.rotVec.z += M_2PI_F;

					delta.rotVec.z *= dti;
				}
				delta.pos = pos;
				delta.rot = rot;
				setPosition(pos,rot);
			}
		}
		else 
		{
			// Set the player to the server position
			delta.pos = pos;
			delta.rot = rot;
			delta.posVec.set(0.0f, 0.0f, 0.0f);
			delta.rotVec.set(0.0f, 0.0f, 0.0f);
			delta.warpTicks = 0;
			delta.dt = 0.0f;
			setPosition(pos,rot);
		}
		//if(mFlyPath.pathId)
		//{
		//	Point3F pos;
		//	QuatF q;
		//	gClientPathManager->getPathPosition(mFlyPath.pathId, mFlyPath.flyPos, pos, q);
		//	setTransform(pos,q);
		//	//updateContainer();
		//}
	}
}

//----------------------------------------------------------------------------
void GameObject::consoleInit()
{
	Con::addVariable("pref::GameObject::renderMyPlayer",TypeBool, &sRenderMyPlayer);
	Con::addVariable("pref::GameObject::renderMyItems",TypeBool, &sRenderMyItems);

	Con::addVariable("GameObject::minWarpTicks",TypeF32,&sMinWarpTicks);
	Con::addVariable("GameObject::maxWarpTicks",TypeS32,&sMaxWarpTicks);
	Con::addVariable("GameObject::maxPredictionTicks",TypeS32,&sMaxPredictionTicks);
}

//--------------------------------------------------------------------------
void GameObject::calcClassRenderData()
{
	Parent::calcClassRenderData();

	disableCollision();
	MatrixF nmat;
	MatrixF smat;
	//Parent::getRetractionTransform(0,&nmat);
	Parent::getImageTransform(0, &nmat);
	Parent::getImageTransform(0, &smat);

	// See if we are pushed into a wall...
	Point3F start, end;
	smat.getColumn(3, &start);
	nmat.getColumn(3, &end);

	RayInfo rinfo;
	if (getContainer()->castRay(start, end, 0xFFFFFFFF & ~(WaterObjectType|DefaultObjectType), &rinfo, getLayerID())) {
		if (rinfo.t < 1.0f)
			mWeaponBackFraction = 1.0f - rinfo.t;
		else
			mWeaponBackFraction = 0.0f;
	} else {
		mWeaponBackFraction = 0.0f;
	}
	enableCollision();
}


void GameObject::playFootstepSound(bool triggeredLeft, S32 sound)
{
	MatrixF footMat = getTransform();

	if ( mWaterCoverage == 0.0f )
	{
		switch ( sound )
		{
		case 0: // Soft
//			SFX->playOnce( mDataBlock->sound[GameObjectData::FootSoft], &footMat );
			break;
		case 1: // Hard
//			SFX->playOnce( mDataBlock->sound[GameObjectData::FootHard], &footMat );
			break;
		case 2: // Metal
//			SFX->playOnce( mDataBlock->sound[GameObjectData::FootMetal], &footMat );
			break;
		case 3: // Snow
//			SFX->playOnce( mDataBlock->sound[GameObjectData::FootSnow], &footMat );
			break;
		default: //Hard
//			SFX->playOnce( mDataBlock->sound[GameObjectData::FootHard], &footMat );
			break;
		}
	}
	else
	{
		/*if ( mWaterCoverage < mDataBlock->footSplashHeight )
			SFX->playOnce( mDataBlock->sound[GameObjectData::FootShallowSplash], &footMat );
		else
		{
			if ( mWaterCoverage < 1.0 )
				SFX->playOnce( mDataBlock->sound[GameObjectData::FootWading], &footMat );
			else
			{
				if ( triggeredLeft )
				{
					SFX->playOnce( mDataBlock->sound[GameObjectData::FootUnderWater], &footMat );
					SFX->playOnce( mDataBlock->sound[GameObjectData::FootBubbles], &footMat );
				}
			}
		}*/
	}
}

void GameObject:: playImpactSound()
{
	/*
	if(mWaterCoverage == 0.0f)
	{
	Point3F pos;
	RayInfo rInfo;
	MatrixF mat = getTransform();
	mat.mulP(Point3F(mDataBlock->decalOffset,0.0f,0.0f), &pos);
	if(gClientContainer.castRay(Point3F(pos.x, pos.y, pos.z + 0.01f),
	Point3F(pos.x, pos.y, pos.z - 2.0f ), TerrainObjectType | InteriorObjectType | VehicleObjectType, &rInfo))
	{
	S32 sound = -1;
	if( rInfo.object->getTypeMask() & TerrainObjectType)
	{
	TerrainBlock* tBlock = static_cast<TerrainBlock*>(rInfo.object);
	S32 mapIndex = tBlock->mMPMIndex[0];
	if (mapIndex != -1) {
	MaterialPropertyMap* pMatMap = MaterialPropertyMap::get();
	const MaterialPropertyMap::MapEntry* pEntry = pMatMap->getMapEntryFromIndex(mapIndex);
	if(pEntry)
	sound = pEntry->sound;
	}
	}
	else if( rInfo.object->getTypeMask() & VehicleObjectType)
	sound = 2; // Play metal sound

	switch(sound) {
	case 0:
	//Soft
	alxPlay(mDataBlock->sound[GameObjectData::ImpactSoft], &getTransform());
	break;
	case 1:
	//Hard
	alxPlay(mDataBlock->sound[GameObjectData::ImpactHard], &getTransform());
	break;
	case 2:
	//Metal
	alxPlay(mDataBlock->sound[GameObjectData::ImpactMetal], &getTransform());
	break;
	case 3:
	//Snow
	alxPlay(mDataBlock->sound[GameObjectData::ImpactSnow], &getTransform());
	break;
	default:
	//Hard
	alxPlay(mDataBlock->sound[GameObjectData::ImpactHard], &getTransform());
	break;
	}
	}
	}
	mImpactSound = 0;
	*/
}

//--------------------------------------------------------------------------
// Update splash
//--------------------------------------------------------------------------

void GameObject::updateSplash()
{
	F32 speed = getVelocity().len();
	if( speed < mDataBlock->splashVelocity || isMounted() ) return;

	Point3F curPos = getPosition();

	if ( curPos.equal( mLastPos ) )
		return;

	if (pointInWater( curPos )) {
		if (!pointInWater( mLastPos )) {
			Point3F norm = getVelocity();
			norm.normalize();

			// make sure player is moving vertically at good pace before playing splash
			F32 splashAng = mDataBlock->splashAngle / 360.0;
			if( mDot( norm, Point3F(0.0, 0.0, -1.0) ) < splashAng )
				return;


			RayInfo rInfo;
			if (gClientContainer.castRay(mLastPos, curPos,
				WaterObjectType, &rInfo)) {
					createSplash( rInfo.point, speed );
					mBubbleEmitterTime = 0.0;
				}

		}
	}
}


//--------------------------------------------------------------------------

void GameObject::updateFroth( F32 dt )
{
	// update bubbles
	Point3F moveDir = getVelocity();
	mBubbleEmitterTime += dt;

	if (mBubbleEmitterTime < mDataBlock->bubbleEmitTime) {
		if (mSplashEmitter[GameObjectData::BUBBLE_EMITTER]) {
			Point3F emissionPoint = getRenderPosition();
			U32 emitNum = GameObjectData::BUBBLE_EMITTER;
			mSplashEmitter[emitNum]->emitParticles(mLastPos, emissionPoint,
				Point3F( 0.0, 0.0, 1.0 ), moveDir, (U32)(dt * 1000.0));
		}
	}

	Point3F contactPoint;
	if (!collidingWithWater(contactPoint)) {
		mLastWaterPos = mLastPos;
		return;
	}

	F32 speed = moveDir.len();
	if( speed < mDataBlock->splashVelEpsilon ) speed = 0.0;
	U32 emitRate = (U32) (speed * mDataBlock->splashFreqMod * dt);

	U32 i;
	for ( i=0; i<GameObjectData::BUBBLE_EMITTER; i++ ) {
		if (mSplashEmitter[i] )
			mSplashEmitter[i]->emitParticles( mLastWaterPos,
			contactPoint, Point3F( 0.0, 0.0, 1.0 ),
			moveDir, emitRate );
	}
	mLastWaterPos = contactPoint;
}

void GameObject::updateWaterSounds(F32 dt)
{
	if ( mWaterCoverage < 1.0f || mDamageState != Enabled )
	{
		// Stop everything
		if ( mMoveBubbleSound )
			mMoveBubbleSound->stop();
		if ( mWaterBreathSound )
			mWaterBreathSound->stop();
		return;
	}

	if ( mMoveBubbleSound )
	{
		// We're under water and still alive, so let's play something
		if ( mVelocity.len() > 1.0f )
		{
			if ( !mMoveBubbleSound->isPlaying() )
				mMoveBubbleSound->play();

			mMoveBubbleSound->setTransform( getTransform() );
		}
		else
			mMoveBubbleSound->stop();
	}

	if ( mWaterBreathSound )
	{
		if ( !mWaterBreathSound->isPlaying() )
			mWaterBreathSound->play();

		mWaterBreathSound->setTransform( getTransform() );
	}
}


//--------------------------------------------------------------------------
// Returns true if player is intersecting a water surface
//--------------------------------------------------------------------------
bool GameObject::collidingWithWater( Point3F &waterHeight )
{

	Point3F curPos = getPosition();

	F32 height = mFabs( mObjBox.max.z - mObjBox.min.z );

	RayInfo rInfo;
	if( gClientContainer.castRay( curPos + Point3F(0.0, 0.0, height), curPos, WaterObjectType, &rInfo) )
	{
		WaterBlock* pBlock = dynamic_cast<WaterBlock*>(rInfo.object);

		if( !pBlock )
			return false;

		//if( !pBlock->isWater( pBlock->getLiquidType() ))
		//   return false;

		waterHeight = rInfo.point;
		return true;
	}


	return false;
}

//--------------------------------------------------------------------------

void GameObject::createSplash( Point3F &pos, F32 speed )
{
	/*if ( speed >= mDataBlock->hardSplashSoundVel )
		SFX->playOnce( mDataBlock->sound[GameObjectData::ImpactWaterHard], &getTransform() );
	else if ( speed >= mDataBlock->medSplashSoundVel )
		SFX->playOnce( mDataBlock->sound[GameObjectData::ImpactWaterMedium], &getTransform() );
	else
		SFX->playOnce( mDataBlock->sound[GameObjectData::ImpactWaterEasy], &getTransform() );*/

	if( mDataBlock->splash )
	{
		MatrixF trans = getTransform();
		trans.setPosition( pos );
		Splash *splash = new Splash;
		splash->onNewDataBlock( mDataBlock->splash );
		splash->setTransform( trans );
		splash->setInitialState( trans.getPosition(), Point3F( 0.0, 0.0, 1.0 ) );
		if (!splash->registerObject())
			delete splash;
	}
}


bool GameObject::isControlObject()
{
	GameConnection* connection = GameConnection::getConnectionToServer();
	if( !connection ) return false;
	ShapeBase *obj = dynamic_cast<ShapeBase*>(connection->getControlObject());
	return ( obj == this );
}

#pragma message(ENGINE(初步修改了玩家移动及其校验))

void GameObject::SetPosToMove(Move* move, Point3F& delta)
{
	if(move == &NullMove)		//Ray: NullMove不能被修改，检查此类危险！
		return;

	move->flyPathId = mFlyPath.pathId;
	move->flyPos = mFlyPath.flyPos;
	move->flyAtEnd = mFlyPath.atEnd;
	move->jumping = mJumpPath.jumping;
	if(move->jumping)
		move->fall = mJumpPath.fall;
	move->blink = mBlink;
	if(move->blink)
		mBlink = false;
	move->mJumpDelay = mJumpDelay;
	move->isMounted = (getGameObjectMask() & MountObjectType);
	move->mPos = delta;
	move->mVelocity = mVelocity;
	move->mJumpSurfaceLastContact = mJumpSurfaceLastContact;
	move->mRotationZ = mRot.z;

	move->mCheck_1 = mCheckVelocity_1;
}

bool GameObject::CheckMoveSpeed(const Move* move, const F32 dt)
{
	// 飞行
	if(move->flyPathId)
		return true;
	// 没有初始化或者没有改变
	if(move == &NullMove || (move->mPos.len() == 0 && move->mRotationZ == 0))
		return true;

	// 没有改变
	if(!m_bMoving && !move->mJumpDelay && move->mVelocity.len() < 0.00001f)
	{
		const Point3F &rot = getRotation();
		if(rot.z == move->mRotationZ)
			return true;
	}
	if(move->blink)
		return true;

	F32 tempSprintSpeedMult = 1;

	F32 moveSpeed;
	if (mDamageState == Enabled && !isImmobilityBuff())
	{
		if (move->y > 0)
		{
			//moveSpeed = getMax(mDataBlock->maxForwardSpeed * tempSprintSpeedMult * move->y,
			//	mDataBlock->maxSideSpeed * tempSprintSpeedMult * mFabs(move->x));
			moveSpeed = mDataBlock->maxSideSpeed * tempSprintSpeedMult * move->y * MAX_GAMEPLAY_PERCENT;
		}
		else
		{
			F32 tempSideSpeed = 0.0f;
			F32 tempBackSpeed = 0.0f;
			tempBackSpeed = mDataBlock->maxBackwardSpeed * tempSprintSpeedMult * mFabs(move->y);
			tempSideSpeed = mDataBlock->maxSideSpeed * tempSprintSpeedMult * mFabs(move->x);				

			if (tempSideSpeed > 0 && tempBackSpeed > 0)
				moveSpeed = tempBackSpeed;
			else
				moveSpeed = getMax(tempSideSpeed,tempBackSpeed);
		}
	}
	else
	{
		moveSpeed = 0;
		mVelocity.set(0, 0, 0);
	}

	Point3F diff = move->mVelocity * TickSec * 32;	// 最长32个tick
	F32 lenS = (move->mPos - getPosition()).lenSquared();
	if(diff.lenSquared() < lenS && lenS > 1.0f)
		return false;

	if (mServerOldSpeed == 0)
		mServerOldSpeed = moveSpeed;	

	if (mFabs(mServerOldSpeed - moveSpeed) > 0.1)
		mServerSpeedChange = true;
	else
		mServerSpeedChange = false;

	if (mServerSpeedChange)
	{
		if (mFabs(move->mCheck_1 - moveSpeed) > 0.1)
		{
			//速度有变化，并且客户端还没有收到服务器速度的更新，就暂时以客户端为准（玩家有可能在这里占到一点小便宜:),类似魔兽骑马进入室内速度不会立刻变慢）
			if (/*mFabs*/(move->mCheck_1 - mServerOldSpeed) <= 0.1 || mServerSynCount < 64) //在2秒钟内允许
			{
				mServerSynCount++;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			mServerOldSpeed = moveSpeed;
			mServerSynCount = 0;
			return true;
		}		
	}
	else
	{
		mServerSynCount = 0;
	}

	//if ((moveSpeed < move->mCheck_1) && mFabs(move->mCheck_1 - moveSpeed) > 0.1) 
	//	return false;	

	return true;
}

void GameObject::NoSimulateMove(const Move* move)
{
#ifdef NTJ_SERVER
	if(mFlyPath.pathId)
	{
		delta.move.mVelocity = getPosition();

		updateFlyPath(mGameplayDeltaTime/1000.f);

		// 飞行结束了
		if(mFlyPath.atEnd && move->flyAtEnd)
			clearFlyPath();

		delta.move.mPos = getPosition();
		delta.move.mRotationZ = mRot.z;
		delta.move.mVelocity = delta.move.mPos - delta.move.mVelocity;
		delta.move.flyPathId = mFlyPath.pathId;
		delta.move.flyPos = mFlyPath.flyPos;
		delta.move.flyAtEnd = mFlyPath.atEnd;
		m_bMoving = true;
		mVelocity = delta.move.mVelocity;
		delta.pos = delta.move.mPos;
		delta.rot.z = delta.move.mRotationZ;
		delta.rot.x = delta.rot.y = 0;
		if(move && move != &NullMove)
			*(Move*)move = delta.move;

		if (isMounted())
		{
			mVelocity = mMount.object->getVelocity();
			setPosition(Point3F(0.0f, 0.0f, 0.0f), ((GameObject*)mMount.object)->getRotation());
		}
		setMaskBits(MoveMask);
		updateContainer();
		return;
	}
	if (isMounted()) {
		mVelocity = mMount.object->getVelocity();
		setPosition(Point3F(0.0f, 0.0f, 0.0f), ((GameObject*)mMount.object)->getRotation());
		setMaskBits(MoveMask);
		updateContainer();
		return;
	}
	// 没有初始化或者没有改变
	if(move == &NullMove || (move->mPos.isZero() && move->mRotationZ == 0))
	{
		VectorF contactNormal;
		bool jumpSurface = false, runSurface = false;
		if (!isMounted())
			findContact(&runSurface,&jumpSurface,&contactNormal);
		if(move != &NullMove)
			mVelocity.zero();
		return;
	}
	//if (move->isMounted != bool(getGameObjectMask() & MountObjectType))
	//	return;

	delta.move = *move;
	m_bMoving = false;

	// 检查是否已经同步
	Point3F pos,rot;
	pos = move->mPos;

	//if (!m_bSynchronous)
	//{
	//	if (NTJfabs(m_ptServerPosition.x - pos.x) < 2.0f && 
	//		NTJfabs(m_ptServerPosition.y - pos.y) < 2.0f && NTJfabs(m_ptServerPosition.z - pos.z) < 2.0f)
	//	{	
	//		m_bSynchronous = true;
	//	}
	//	else
	//		return;
	//}

	if (move->mJumpDelay)
		mJumpDelay = move->mJumpDelay;
	else
		mJumpDelay = 0;

	mVelocity = move->mVelocity;
	delta.pos = pos;
	mJumpSurfaceLastContact = move->mJumpSurfaceLastContact;

	rot.z = move->mRotationZ;	
	delta.rot = rot;	

	// 旋转也需更新
	const Point3F &PlayerRot = getRotation();
	if(PlayerRot.z != move->mRotationZ)
		m_bMoving = true;

	VectorF contactNormal;
	bool jumpSurface = false, runSurface = false;
	if (!isMounted())
		findContact(&runSurface,&jumpSurface,&contactNormal);

	setPosition(pos,rot);
	setMaskBits(MoveMask);

	F32 speed = mVelocity.len();
	mJumpPath.jumping = move->jumping;
	mJumpPath.fall = move->fall;

	// CVehicleObject Dismount
	//if(move->trigger[2] && isMounted())
	//	Con::executef(m_pDataBlock,2,"doDismount",scriptThis());

	//if(!inLiquid && m_fWaterCoverage != 0.0f) 
	//{
	//	//这里可以做环境伤害。。。
	//	//Con::executef(m_pDataBlock,4,"onEnterLiquid",scriptThis(), Con::getFloatArg(m_fWaterCoverage), Con::getIntArg(m_nLiquidType));
	//	inLiquid = true;
	//}
	//else if(inLiquid && m_fWaterCoverage == 0.0f) 
	//{
	//	//Con::executef(m_pDataBlock,3,"onLeaveLiquid",scriptThis(), Con::getIntArg(m_nLiquidType));
	//	inLiquid = false;
	//}

	if (speed > 0.1f)
	{
		//if (m_ActiveSpell.IsSpellRuning())
		//	m_ActiveSpell.CancelCurrentSpell();

		////走动取消所有的吟唱事件(包括镶嵌,打造)
		//if(GetScheduleEventID())
		//{
		//	WarnMessageEvent::AddScreenMessage(this, MSG_BREAKACTION_CANCEL);		
		//	Sim::cancelEvent(GetScheduleEventID());

		//	if (IsGathering())
		//		SetGatherState(false);

		//	ClearScheduleEvent();
		//}

		m_bMoving = true;
	}
	else if (speed == 0)
	{
		//if (CanSpecIdleAction())
		//{
		//	setActionThread(m_pDataBlock->PickAnimation(GameObjectData::ACTION_SPECIDLE, m_ActionEquipment,HasRideItem()), true, false, true, true, true);
		//	setMaskBits(ActionMask);
		//}
		//else if (GetAttackTarget() && GetAttackTarget()->IsEnabled() && IsValidDistance(GetAttackTarget()) && IsFaceObj(GetAttackTarget()) && bAttackEnd())
		//{
		//	//(move->trigger[GAME_MOVE_TRIGGER_ATTACK] == 1) && GetAttackTarget() && IsEnabled() && bAttackEnd()
		//	DoAttack();
		//}
	}

	//if (move->bCanJump) 
	//{
	//	//setActionThread(m_pDataBlock->PickAnimation(GameObjectData::ACTION_JUMP, m_ActionEquipment,HasRideItem()), true, false, true);
	//	//setMaskBits(MaskEx::ActionMaskEx);
	//	// 这里为什么m_bMoving = true;？？？？？？
	//	// m_bMoving = true;

	//	//if (HasRideItem())
	//	//	setRideActionThread(m_pRideDataBlock->PickAnimation(CMonsterObjectData::MONSTER_ANIMATION_JUMP, 0), true, false, true);
	//}

	updateContainer();

#endif
}

bool GameObject::CanSetTarget(SceneObject* obj)
{
	if(!obj || !(obj->getType() & GameObjectType))
		return true;

	GameObject* pGameObject = (GameObject*)obj;
	if((pGameObject->getGameObjectMask() & NpcObjectType) && pGameObject->isDisabled())
		return false;
	if(getGameObjectMask() & PlayerObjectType)
	{
		if(pGameObject->mInvisibleTargetTick && CanAttackTarget(pGameObject))
			return false;
		if((pGameObject->GetBuffTable().GetEffect() & BuffData::Effect_Invisibility) && getDistance(obj) > 3.0f)
			return false;
	}
	return true;
}

bool GameObject::SetTarget(SceneObject* obj, bool atk /* = true */, U32 mask /* = 0 */)
{
	if(!CanSetTarget(obj))
		return false;
#ifdef NTJ_CLIENT
	if(getControllingClient() && atk && obj)
		g_ClientGameplayState->clearPath();
#endif
#ifndef NTJ_EDITOR
	if(m_pAI)
		m_pAI->ClearAICast();
#endif
	if(m_pTarget == obj)
	{
		SetAutoCastEnable(atk);
#ifndef NTJ_EDITOR
		if(m_pAI)
			m_pAI->SetFollowObject(atk ? obj : NULL, IsAutoCastEnable() ? m_SkillTable.GetDefaultRange() : INTERACTIONDISTANCE, mask);
#endif
		return true;
	}

	if(obj && !(obj->getType() & GameObjectType))
	{
		SetAutoCastEnable(false);
#ifndef NTJ_EDITOR
		if(m_pAI)
			m_pAI->SetFollowObject(atk ? obj : NULL, INTERACTIONDISTANCE, mask);
#endif
		return false;
	}
	m_pTarget = (GameObject*)obj;
	setMaskBits(GameObject::TargetMask);

	// 更新AutoCastEnable
	SetAutoCastEnable(atk);

#ifndef NTJ_EDITOR
	if(m_pAI)
		m_pAI->SetFollowObject(atk ? obj : NULL, IsAutoCastEnable() ? m_SkillTable.GetDefaultRange() : INTERACTIONDISTANCE, mask);
#endif

#ifdef NTJ_SERVER
	if(mask == SetTargetToClient && getControllingClient())
	{
		ServerGameNetEvent* pEvent = new ServerGameNetEvent(INFO_SET_TARGET_TOCLIENT);
		pEvent->SetIntArgValues(2, (bool)(m_pTarget.getObject()) ,m_pTarget ? getControllingClient()->getGhostIndex(m_pTarget) : 0);
		getControllingClient()->postNetEvent(pEvent);
	}
#endif
#ifdef NTJ_CLIENT
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(mask != SetTargetToClient && conn && conn->getControlObject() == this)
	{
		ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_SET_TARGET);
		pEvent->SetInt32ArgValues(2,m_pTarget ? m_pTarget->getServerId() : 0,atk);
		conn->postNetEvent(pEvent);
	}
	g_UIMouseGamePlay->RefreshTargetDecal();
#endif

	return true;
}

bool GameObject::SetTarget(Point3F& pos, U32 mask /* = 0 */)
{
	if (mBlink || mJumpPath.jumping || mFlyPath.pathId)
		return false;

#ifndef NTJ_EDITOR
	if(m_pAI)
	{
		m_pAI->ClearAICast();
		m_pAI->SetMove(pos, mask);
#ifdef NTJ_CLIENT
		g_ClientGameplayState->clearPath();
		g_UIMouseGamePlay->DisableAutoMove();
#endif
	}
#endif
	return true;
}

bool GameObject::CanAttackTarget(GameObject* obj)
{
	if(!obj)
		return false;
	if(obj->getGameObjectMask() & PetObjectType)
		obj = ((PetObject*)obj)->getMaster();

	if(!obj || obj == this || !obj->isCombative() || !isCombative() || obj->isDisabled() || isDisabled())
		return false;
	// Npc独占判断
	if((obj->getGameObjectMask() & NpcObjectType) && !((NpcObject*)obj)->checkExclusive(this))
		return false;
	if((getGameObjectMask() & NpcObjectType) && !((NpcObject*)this)->checkExclusive(obj))
		return false;
	// 区域属性判断
	if(PlayerObjectType & getGameObjectMask() & obj->getGameObjectMask())
	{
		if(!isCombatArea(true) || !obj->isCombatArea(true))
			return false;
	}
	else if(!isCombatArea(false) || !obj->isCombatArea(false))
		return false;
	return true;
}

bool GameObject::isCombatArea(bool pk)
{
	TriggerData* pTri = g_TriggerDataManager->getTriggerData(mTriggerId);

	if(!pTri)
		return true;

	return pk ? pTri->getBaseLimit(TriggerData::Limit_PK) : pTri->getBaseLimit(TriggerData::Limit_AtkMonster);
}

F32 GameObject::GetFollowRange()
{
	if(IsAutoCastEnable())
		return m_SkillTable.GetDefaultRange();
	return 3.0f;
}

F32 GameObject::GetRangeMin(U32 skillId)
{
	SkillData* pData = g_SkillRepository.GetSkill(skillId);
	if(pData)
		return pData->GetRangeMin();
	return 0.0f;
}

F32 GameObject::GetRangeMax(U32 skillId)
{
	SkillData* pData = g_SkillRepository.GetSkill(skillId);
	if(pData)
		return pData->GetRangeMax();
	return 3.0;
}

bool GameObject::isRenderImage(U32 imageSlot)
{
	//if(mUpperActionAnimation.action >= GameObjectData::NumTableActionAnims)
	//	return false;
	if(imageSlot == GameObject::Slot_LeftHand || imageSlot == GameObject::Slot_RightHand)
	{
		if(isDisabled() || (GameObjectData::ActionAnimationList[mUpperActionAnimation.action].flags & GameObjectData::Flags_HideWeapon))
			return false;
	}
	return true;
}

void GameObject::PrepVisibility()
{
#ifdef NTJ_CLIENT
	F32 dt = Platform::getVirtualMilliseconds() - mLastRenderTime;
	if(mFading)
	{
		mFadeElapsedTime += dt;
		if(mFadeElapsedTime > mFadeTime + mFadeDelay)
		{
			mFadeVal = F32(!mFadeOut);
			mFading = false;
		}
		else if(mFadeElapsedTime > mFadeDelay)
		{
			mFadeVal = (mFadeElapsedTime - mFadeDelay) / mFadeTime;
			if(mFadeOut)
				mFadeVal = 1 - mFadeVal;
		}
	}
	// 隐身
	static GameObject* observer;
	observer = g_ClientGameplayState->GetControlPlayer();
	if((GetBuffTable().GetEffect() & BuffData::Effect_Invisibility) && observer)
	{
		if(!observer->CanAttackTarget(this))
			mVisibility = 0.4f;
		else
		{
			F32 len = getDistance(observer);
			mVisibility = 0.4f * mClampF((3.0f - len)/3.0f, 0.0f, 1.0f);
		}
	}
	else
		mVisibility = 1.0f;

	mFinalVisibility = mFadeVal * mVisibility;
	mLastRenderTime = Platform::getVirtualMilliseconds();
#endif
}

bool GameObject::isEdgeBlurPause()
{
	return (!isRenderImage(GameObject::Slot_LeftHand)) || !(IsAttackAnim(mUpperActionAnimation.action) || IsSkillAttackAnim(mUpperActionAnimation.action) || IsCriticalAnim(mUpperActionAnimation.action) || IsFatalAnim(mUpperActionAnimation.action));
}

void GameObject::setArmStatus(GameObjectData::ArmStatus arm)
{
	if(m_ArmStatus != arm)
	{
		m_ArmStatus = arm;
#ifdef NTJ_SERVER
		// 同时改变普通攻击
		AddSkill(Macro_GetSkillId(DefaultSkill_Min + m_ArmStatus, 1), isProperlyAdded());
#endif
	}
}

GameObjectData::Animations GameObject::getAnim(GameObjectData::Animations action)
{
	if(getArmStatus() == GameObjectData::Arm_A)
		return action;

	switch (action)
	{
	case GameObjectData::Idle_a0:
	case GameObjectData::Idle_a1:
	case GameObjectData::Idle_a2:
	case GameObjectData::Attack_a0:
	case GameObjectData::Attack_a1:
	case GameObjectData::Attack_a2:
	case GameObjectData::SkillAttack_a0:
	case GameObjectData::SkillAttack_a1:
	case GameObjectData::SkillAttack_a2:
		{
			return (GameObjectData::Animations)(action + getArmStatus() * 3);
		}
	case GameObjectData::Root_a:
	case GameObjectData::Run_a:
	case GameObjectData::Ready_a:
	case GameObjectData::Critical_a:
	case GameObjectData::Fatal_a:
		{
			return (GameObjectData::Animations)(action + getArmStatus());
		}
	case GameObjectData::ReadySpell_b0:
	case GameObjectData::SpellCast_b0:
	case GameObjectData::ChannelCast_b0:
		{
			return (GameObjectData::Animations)(action + (getArmStatus() - 1));
		}
	case GameObjectData::Mount_a:
		{
			return getMountAction();
		}
	}

	return action;
}

void GameObject::getMountAnim(U32& action)
{
	if(!isMounted())
		return;
	ShapeBase* pBase = getObjectMount();
	if(pBase->getTypeMask() & GameObjectType)
	{
		if(((GameObject*)pBase)->getGameObjectMask() & MountObjectType)
		{
			MountObjectData* pData = (MountObjectData*)pBase->getDataBlock();
			if(pData)
			{
				S32 idx = getMountIndex();
				if(idx >= 0 && idx < MountObjectData::MaxMounted)
					action = pData->info[idx].action;
				else
				{
					AssertFatal(false, "mount error !");
					action = GameObjectData::Mount_a;
				}
			}
		}
	}
}

void GameObject::getScheduleAnim(U32& action)
{
	if(getGameObjectMask() & PlayerObjectType)
	{
#ifdef NTJ_SERVER
		if(((Player*)this)->pScheduleEvent && ((Player*)this)->pScheduleEvent->getAnimation())
			action = ((Player*)this)->pScheduleEvent->getAnimation();
#endif
#ifdef NTJ_CLIENT
		if(getControllingClient() && g_ClientGameplayState->pVocalStatus && g_ClientGameplayState->pVocalStatus->getAnimation())
			action = g_ClientGameplayState->pVocalStatus->getAnimation();
#endif
	}
}

void GameObject::onWound()
{
#ifdef NTJ_CLIENT
	if (mWoundThread)
	{
		mShapeInstance->setPos(mWoundThread,0);
		mShapeInstance->setTimeScale(mWoundThread,1);
	}
#else
	setMaskBits(WoundMask);
#endif
}

bool GameObject::cullCastAnim()
{
	if(GameObjectData::ActionAnimationList[mUpperActionAnimation.action].group == GameObjectData::Group_SpellCast)
	{
		if(mShapeInstance->getSeqPos(mUpperActionAnimation.thread) > 0.8f)
			return true;
		return false;
	}
	return true;
}

void GameObject::packAnimateEP(NetConnection *conn, U64 mask, BitStream *stream)
{
#ifdef NTJ_SERVER
	if(stream->writeFlag((mask & InitialUpdateMask) || !mSkillDataEP || (mTargetIsObj ? mTargetObjEP.empty() : mDestEP.isZero())))
		return;
	stream->writeInt(mSkillDataEP, Base::Bit32);
	stream->writeFlag(mSelectableEP == SkillData::Selectable_A);

	if(stream->writeFlag(mTargetIsObj))
	{
		// 目标是对象
		if(mTargetObjEP.size() > BIT(TargetObjEPBits))
			mTargetObjEP.setSize(BIT(TargetObjEPBits));
		stream->writeInt(mTargetObjEP.size(), TargetObjEPBits);

		for (S32 i=0; i<mTargetObjEP.size(); ++i)
		{
			SimObject* pSim = Sim::findObject(mTargetObjEP[i]);
			if(stream->writeFlag(!pSim || !(pSim->getTypeMask() & GameObjectType)))
				continue;
			stream->writeInt(conn->getGhostIndex((GameObject*)pSim), NetConnection::GhostIdBitSize);
		}
	}
	else
	{
		// 目标是地点坐标
		stream->writeRangedF32(mDestEP.x, -1024.0f, 1024.0f, 16);
		stream->writeRangedF32(mDestEP.y, -1024.0f, 1024.0f, 16);
		stream->writeRangedF32(mDestEP.z, -1024.0f, 1024.0f, 16);
	}
#endif
}

void GameObject::unpackAnimateEP(NetConnection *conn, BitStream *stream)
{
#ifdef NTJ_CLIENT
	if(stream->readFlag())
		return;
	// 先把特效放出去
	applyAnimateEP();
	mSkillDataEP = stream->readInt(Base::Bit32);
	mSelectableEP = stream->readFlag() ? SkillData::Selectable_A : SkillData::Selectable_B;
	SkillData* pData = g_SkillRepository.GetSkill(mSkillDataEP);
	mTargetIsObj = stream->readFlag();
	if(mTargetIsObj)
	{
		U32 size = stream->readInt(TargetObjEPBits);
		for (S32 i=0; i<size; ++i)
		{
			if(stream->readFlag())
				continue;
			NetObject* pNet = conn->resolveGhost(stream->readInt(NetConnection::GhostIdBitSize));
			if (pData && pNet && (pNet->getTypeMask() & GameObjectType))
			{
				GameObject* pObj = (GameObject*)pNet;
				addTargetObjEP(pObj->getId());
				if(pData->IsFlags(SkillData::Flags_ScheduleSpell))
				{
					// 确定作用次数及排定时间
					SimTime time = 1000;
					S32 times = 1;
					Point3F dist = pObj->getPosition() - getPosition();
					time = dist.len() / getMax(pData->m_Cast.projectileVelocity, 1.0f) * 1000;
					time = time * 3 + 2000 + (1000 * mShapeInstance->getCommonDuration(mUpperActionAnimation.thread)/mShapeInstance->getTimeScale(mUpperActionAnimation.thread));
					// 加入列表
					U32 id = g_EffectPacketContainer.addEffectPacket(pData->m_Cast.sourceEP, this, getRenderTransform(), pObj, pObj->getPosition());
					ScheduleSpell* pScheduleSpell = new ScheduleSpell(time, times, this, true, pObj, dist, pData, g_Stats, id, SkillData::Selectable_A);
					if(pScheduleSpell)
						m_Spell.m_ScheduleSpell.push_back(pScheduleSpell);
				}
			}
		}
	}
	else
	{
		mDestEP.x = stream->readRangedF32(-1024.0f, 1024.0f, 16);
		mDestEP.y = stream->readRangedF32(-1024.0f, 1024.0f, 16);
		mDestEP.z = stream->readRangedF32(-1024.0f, 1024.0f, 16);
	}
	if(pData && pData->m_Cast.sourceDelayEP)
		g_EffectPacketContainer.addEffectPacket(pData->m_Cast.sourceDelayEP, this, getRenderTransform(), this, getPosition());
#endif
}

void GameObject::applyAnimateEP()
{
#ifdef NTJ_CLIENT
	if(!mSkillDataEP || (mTargetIsObj ? mTargetObjEP.empty() : mDestEP.isZero()))
		return;
	SkillData* pData = g_SkillRepository.GetSkill(mSkillDataEP);
	if(!pData)
	{
		clearAnimateEP();
		return;
	}

	if(mTargetIsObj)
	{
		for (S32 i=mTargetObjEP.size()-1; i>=0; --i)
		{
			if(pData->IsFlags(SkillData::Flags_ScheduleSpell))
			{
				g_EffectPacketContainer.setEffectPacketsStatus(pData->m_Cast.sourceEP, EffectPacketItem::Status_B);
			}
			else
			{
				GameObject* pObj = (GameObject*)Sim::findObject(mTargetObjEP[i]);
				if(pObj)
				{
					g_EffectPacketContainer.addEffectPacket(pData->m_Cast.targetDelayEP, pObj, pObj->getRenderTransform(), pObj, pObj->getPosition());
					pObj->onWound();
				}
			}
		}
	}
#endif
	clearAnimateEP();
}

void GameObject::processTick_Gameplay()
{
	static SimTime delta;
	// 计算时间间隔，并设置伐值1024ms
	delta = mLastGameplayTime ? getMin(Platform::getVirtualMilliseconds() - mLastGameplayTime, (SimTime)1024) : 0;
	mGameplayDeltaTime = delta;

#ifdef NTJ_SERVER
	// 某些情况下需要清除目标
	if(GetTarget() && !CanSetTarget(GetTarget()))
		SetTarget(NULL, false, SetTargetToClient);

	m_BuffTable.Process(delta);
	CalcStats();
	ProcessCast(delta);
#endif

#ifdef NTJ_CLIENT
	if(getControllingClient())
	{
		RefreshGui();
	}
	ProcessCast(delta);

	//头顶冒泡显示5秒钟
	if((Platform::getVirtualMilliseconds() - mChatLastUpdateTime) >= 5000 && NULL != getChatInfo())
	{
		mChatDirty = false;
		clearChatInfo();
	}
#endif
	// 隐身特殊处理
	if(mInvisibleTargetTick)
		--mInvisibleTargetTick;
	if(mInvisibleImmunityTick)
		--mInvisibleImmunityTick;

	mLastGameplayTime = Platform::getVirtualMilliseconds();
}

// 是否朝向目标，以是否在前方180度范围内为标准
bool GameObject::IsFaceTo(Point3F& pos)
{
	Point3F Diff = pos - getPosition();
	if (!isZero(Diff.x) || !isZero(Diff.y))
	{
		F32 yawDiff = mAtan( Diff.x, Diff.y ) - getRotation().z;
		if( yawDiff > M_PI_F )
			yawDiff -= M_2PI_F;
		else if( yawDiff < -M_PI_F )
			yawDiff += M_2PI_F;
		if(mFabs(yawDiff)*2.0f < M_PI_F)
			return true;
	}
	return false;
}

Point3F& GameObject::getPetPos()
{
	static Point3F objPos(1.2f, -1.2f, 0);
	static Point3F worldPos;
	getTransform().mulP(objPos, &worldPos);
	if(!g_NavigationManager->CanReach(worldPos))
		worldPos = getPosition();
	return worldPos;
}

void GameObject::onEnabled()
{
	Parent::onEnabled();

	//mTypeMask &= ~CorpseObjectType;
	//mTypeMask |= GameObjectType;

	// 设置复活动作
	setActionThread(GameObjectData::Root_a, true, false, false, false, true);
	onBuffTrigger(BuffData::Trigger_Rise, NULL); // 统一状态触发<复活>
}

void GameObject::onDisabled()
{
	onBuffTrigger(BuffData::Trigger_Death, NULL); // 统一状态触发<死亡>

	Parent::onDisabled();

	//mTypeMask &= ~GameObjectType;
	//mTypeMask |= CorpseObjectType;

	unmount();
	BreakCast();
	m_AutoCastTimer.SetEnable(false);

	// 设置死亡动作
	if(canTame())
		setActionThread(GameObjectData::Idle_a0, true, true, true, false, true);
	else
		setActionThread(GameObjectData::Death, true, true, true, false, true);
}

void GameObject::onInvisible()
{
	mInvisibleTargetTick = InvisibleTargetTick;
	mInvisibleImmunityTick = InvisibleImmunityTick;
}

bool GameObject::isImmobilityBuff()
{
	if(getGameObjectMask() & PlayerObjectType)
	{
		Player* pPlayer = (Player*)(this);
		if(pPlayer && pPlayer->getInteractionState() == Player::INTERACTION_STALLTRADE)
			return true;
	}
	return m_BuffTable.GetEffect() & BuffData::Buff_Immobility;
}

void GameObject::setClasses(U32 val, U32 newClass, bool force)
{
	AssertFatal(val < MaxClassesMasters && newClass < MaxClasses,"");
	// 当已经选择了技能时，不能轻易改变
	if(m_Classes[val] != Class_Null && !force)
		return;

	m_Classes[val] = newClass;

#ifdef NTJ_SERVER
	setMaskBits(BaseInfoMask);
#endif
}

void GameObject::setInfluence(enInfluence infl)
{
	// 玩家不可以改变势力
	if(getGameObjectMask() & PlayerObjectType)
		return;
	if(infl < TotalInfluence)
		m_Influence = infl;
#ifdef NTJ_SERVER
	setMaskBits(BaseInfoMask);
#endif
}

//----------------------------------------------------------------------------
// 战斗相关

void GameObject::CalcStats()
{
	if(m_BuffTable.CalculateStats(m_Family))
	{
		m_SkillTable.Update();
		setMaskBits(StatsMask | SpeedMask | HPMask | MPMask | PPMask);
	}
}

void GameObject::RefreshGui()
{
#ifdef NTJ_CLIENT
	if(m_BuffTable.GetRefresh())
	{
		m_SkillTable.Update();
		Con::executef("RefreshPlayerInfoGui", avar("%d", this->getId()));
		SetRefreshGui(false);
	}
#endif
}

Skill* GameObject::AddSkill(U32 skillId, bool updateToClient)
{
	SkillData* pData = g_SkillRepository.GetSkill(skillId);
	if(!pData)
		return NULL;

	// 已经有相同的技能了
	Skill* pSkill = m_SkillTable.GetSkill(pData->GetSeriesID());
	if(pSkill && (skillId == pSkill->GetBaseSkillId()))
		return pSkill;

	if(pSkill && pSkill->GetData())
	{
		// 被动技能的buff
		if(pSkill->GetData()->IsFlags(SkillData::Flags_Passive))
			m_BuffTable.RemoveBuff(pSkill->GetData()->m_Operation[0].buffId);
	}

	pSkill = m_SkillTable.AddSkill(pData);
	if(pSkill && pSkill->GetData())
	{
		// 被动技能的buff
		if(pData->IsFlags(SkillData::Flags_Passive))
			m_BuffTable.AddBuff(Buff::Origin_PassiveSkill, pData->m_Operation[0].buffId, this);

		// 发送更新给客户端
#ifdef NTJ_SERVER
		if(getControllingClient() && (getGameObjectMask() & PlayerObjectType) && ((Player*)this)->hasInitializEvent()){
			SkillEvent* event = new SkillEvent(skillId, true);
			getControllingClient()->postNetEvent(event);
		}
#endif
		return pSkill;
	}
	return NULL;
}

void GameObject::RemoveSkill(U32 skillId)
{
	// 先删除被动技能的buff
	if(skillId != 0)
	{
		SkillData* pData = g_SkillRepository.GetSkill(skillId);
		if(pData)
			m_BuffTable.RemoveBuff(pData->m_Operation[0].buffId);
	}
	// 删除技能
	m_SkillTable.RemoveSkill(Macro_GetSkillSeriesId(skillId));

	// 发送更新给客户端
#ifdef NTJ_SERVER
	if(getControllingClient() && (getGameObjectMask() & PlayerObjectType) && ((Player*)this)->hasInitializEvent()){
		SkillEvent* event = new SkillEvent(skillId, false);
		getControllingClient()->postNetEvent(event);
	}
#endif
}

bool GameObject::AddBuff(Buff::Origin _origin, U32 _buffId, GameObject* _src, S32 _count)
{
	return m_BuffTable.AddBuff(_origin, _buffId, _src, _count);
}

bool GameObject::RemoveBuff(U32 _buffId)
{
	return m_BuffTable.RemoveBuff(_buffId);
}

bool GameObject::RemoveBuffById(U32 _buffId, U32 _count /* = 1 */, U32 mask /* = Buff::Mask_Remove_Sys */, bool seriesId /* = false */)
{
	return m_BuffTable.RemoveBuffById(_buffId, _count, mask, seriesId);
}

bool GameObject::RemoveBuffBySrc(U32 _buffId, GameObject* src, U32 _count /* = 1 */, U32 mask /* = Buff::Mask_Remove_Sys */, bool seriesId /* = false */)
{
	return m_BuffTable.RemoveBuffBySrc(_buffId, src, _count, mask, seriesId);
}

bool GameObject::HandleRemoveRequest(U32 _buffId, U32 index)
{
	return m_BuffTable.HandleRemoveRequest(_buffId, index);
}

enWarnMessage GameObject::CanCast(U32 skillId, GameObject* obj, const Point3F* pos, bool clientAI /* = false */)
{
	SkillData* pData = g_SkillRepository.GetSkill(skillId);
	if(pData)
		return CanCast(pData, obj, pos, clientAI);
	return MSG_COMBAT_ERROR;
}

enWarnMessage GameObject::CanCast(SkillData* pData, GameObject* obj, const Point3F* pos, bool clientAI /* = false */)
{
	static F32 delta;
	if(!pData)
		return MSG_COMBAT_ERROR;

	// 客户端无法使用自动攻击
#ifdef NTJ_CLIENT
	if(pData->IsFlags(SkillData::Flags_AutoCast))
		return MSG_SKILL_CASTFAILED;
#endif

	// 被动技能
	if(pData->IsFlags(SkillData::Flags_Passive))
		return MSG_SKILL_CASTFAILED;

	// armStatus
	if(!pData->IsArmStatus(getArmStatus()))
		return MSG_SKILL_CASTFAILED;

	// 施放目标限制
	switch(pData->m_CastLimit.object)
	{
	case SkillData::Object_Destine:
		{
			obj = NULL;
			if(!pos)
				return MSG_COMBAT_ERROR;
		}
		break;
	case SkillData::Object_Self:
		{
			pos = NULL;
			if(this != obj)
				return MSG_SKILL_CANNOTDOTHAT;
		}
		break;
	case SkillData::Object_Pet:
		{
			pos = NULL;
			if(!(getGameObjectMask() & PlayerObjectType))
				return MSG_COMBAT_ERROR;
			//Player* player = (Player*)this;
			//if(player->getPet(0) != obj)
			//	return MSG_SKILL_CANNOTDOTHAT;
		}
		break;
	case SkillData::Object_Target:
		{
			pos = NULL;
			if(!obj)
				return MSG_SKILL_NEEDTARGET;
			bool canAtk = CanAttackTarget(obj);
			if(canAtk && !(pData->m_CastLimit.target & SkillData::Target_Enemy))
				return MSG_SKILL_INVALIDTARGET;
			if(!canAtk && !(pData->m_CastLimit.target & SkillData::Target_Friend))
				return MSG_SKILL_INVALIDTARGET;
			// 目标是自身，但技能不能对自身起作用
			if(obj == this && (pData->m_CastLimit.target & SkillData::Target_DisableSelf))
				return MSG_SKILL_INVALIDTARGET;
		}
	}

	// 特殊效果的检查
	if(pData->IsEffect(SkillData::Effect_Jump | SkillData::Effect_Blink))
	{
		if(!pos || !g_NavigationManager->CanReach(*(Point3F*)pos))
			return MSG_SKILL_INVALIDTARGET;
	}

	// 检查冷却（clientAI不做检查）
	if(!clientAI && !CheckCastCooldown(pData))
		return MSG_SKILL_NOTREADY;

	// 技能消耗（待补充）
	if(pData->m_Cost.HP >= GetStats().HP)
		return MSG_SKILL_OUTOFHP;
	if(pData->m_Cost.MP > GetStats().MP)
		return MSG_SKILL_OUTOFMP;
	if(pData->m_Cost.PP > GetStats().PP)
		return MSG_SKILL_OUTOFPP;

	// 目标、自身前置状态限制
	bool PreA = pData->GetCastLimit().srcPreBuff_A ? m_BuffTable.checkBuff(pData->GetCastLimit().srcPreBuff_A, pData->GetCastLimit().srcPreBuffCount_A, true) : true;
	bool PreB = pData->GetCastLimit().srcPreBuff_B ? m_BuffTable.checkBuff(pData->GetCastLimit().srcPreBuff_B, pData->GetCastLimit().srcPreBuffCount_B, true) : true;
	if(pData->GetCastLimit().srcPreBuffOp & SkillData::Op_NOT_0) PreA = !PreA;
	if(pData->GetCastLimit().srcPreBuffOp & SkillData::Op_NOT_1) PreB = !PreB;
	if(!((pData->GetCastLimit().srcPreBuffOp & SkillData::Op_OR_0) ? (PreA || PreB) : (PreA && PreB)))
		return MSG_SKILL_CANNOTDOTHAT;
	if(obj)
	{
		PreA = pData->GetCastLimit().tgtPreBuff_A ? obj->GetBuffTable().checkBuff(pData->GetCastLimit().tgtPreBuff_A, pData->GetCastLimit().tgtPreBuffCount_A, true) : true;
		PreB = pData->GetCastLimit().tgtPreBuff_B ? obj->GetBuffTable().checkBuff(pData->GetCastLimit().tgtPreBuff_B, pData->GetCastLimit().tgtPreBuffCount_B, true) : true;
		if(pData->GetCastLimit().srcPreBuffOp & SkillData::Op_NOT_0) PreA = !PreA;
		if(pData->GetCastLimit().srcPreBuffOp & SkillData::Op_NOT_1) PreB = !PreB;
		if(!((pData->GetCastLimit().tgtPreBuffOp & SkillData::Op_OR_0) ? (PreA || PreB) : (PreA && PreB)))
			return MSG_SKILL_CANNOTDOTHAT;
	}

	// 距离限制（clientAI不做检查）
	if(!clientAI)
	{
		F32 delta = obj ? getDistance(obj) : getDistance(pos);
		if(delta < pData->m_CastLimit.rangeMin || delta > pData->m_CastLimit.rangeMax)
			return MSG_SKILL_OUTRANGE;
	}

	return MSG_NONE;
}

bool GameObject::CastSpell(U32 skillId, GameObject& obj, U32 itemType /* = 0 */, U32 itemIdx /* = 0 */, U64 UID /* = 0 */)
{
	if(!m_Spell.Cast(skillId, obj, itemType, itemIdx, UID))
		return false;

#ifdef NTJ_CLIENT
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(conn && conn->getControlObject() == this)
	{
		CastSpellEvent* pEvent = new CastSpellEvent(skillId, obj.getServerId(), itemType, itemIdx, UID);
		conn->postNetEvent(pEvent);
	}
#endif

	return true;
}

bool GameObject::CastSpell(U32 skillId, Point3F& pos, U32 itemType /* = 0 */, U32 itemIdx /* = 0 */, U64 UID /* = 0 */)
{
	if(!m_Spell.Cast(skillId, pos, itemType, itemIdx, UID))
		return false;

#ifdef NTJ_CLIENT
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(conn && conn->getControlObject() == this)
	{
		CastSpellEvent* pEvent = new CastSpellEvent(skillId, pos, itemType, itemIdx, UID);
		conn->postNetEvent(pEvent);
	}
#endif

	return true;
}

bool GameObject::AI_CastSpell(U32 skillId, GameObject* obj, const Point3F* pos, U32 itemType /* = 0 */, U32 itemIdx /* = 0 */, U64 UID /* = 0 */)
{
#ifndef NTJ_EDITOR
	SkillData* pSkillData = g_SkillRepository.GetSkill(skillId);
	if(!pSkillData)
		return false;

#ifdef NTJ_CLIENT
	if(m_pAI && pSkillData->IsFlags(SkillData::Flags_AutoCast) && (obj != GetTarget() || !m_pAI->CheckChaseState()) && CanAttackTarget(obj))
	{
		SetTarget(obj, true);
		return true;
	}
#endif

	if(m_Spell.IsSpellRunning() || !m_pAI)
		return false;

	bool targetSelf = (pSkillData->m_CastLimit.object == SkillData::Object_Self);
	if(targetSelf)
		obj = this;
	if(obj)
		pos = NULL;

	enWarnMessage msg = CanCast(pSkillData, obj, pos, isClientObject());
	if(MSG_NONE != msg)
	{
#ifdef NTJ_CLIENT
		MessageEvent::show(SHOWTYPE_WARN, msg, SHOWPOS_SCREEN);
#endif
		return false;
	}

	if(obj)
	{
		if(!targetSelf)
		{
			SetTarget(obj);
			m_pAI->SetFollowObject(obj, pSkillData->m_CastLimit.rangeMax);
		}
	}
	else
		SetTarget((Point3F)*pos);

	static Spell::PreSpell pre;
	pre.pSkillData = pSkillData;
	pre.itemInter.type = itemType;
	pre.itemInter.index = itemIdx;
	pre.itemInter.UID = UID;
	m_pAI->SetAICast(pre, pSkillData->m_CastLimit.rangeMin, pSkillData->m_CastLimit.rangeMax, targetSelf, pos);
#endif
	return true;
}

bool GameObject::PreCastSpell(U32 skillId, GameObject* obj, const Point3F* pos, U32 itemType /* = 0 */, U32 itemIdx /* = 0 */, U64 UID /* = 0 */)
{
#ifndef NTJ_EDITOR
	// to do:天赋加成
	if(!m_Spell.SetPreSpell(skillId, itemType, itemIdx, UID))
		return AI_CastSpell(skillId, obj, pos, itemType, itemIdx, UID);
#endif
	return true;
}

bool GameObject::PreCastSpellDone(GameObject* obj, const Point3F* pos)
{
#ifndef NTJ_EDITOR
	if(!m_Spell.GetPreSpell().pSkillData)
		return false;

	if(m_Spell.GetPreSpell().pSkillData->m_CastLimit.object == SkillData::Object_Destine)
		obj = NULL;
	else
		pos = NULL;

	// to do:天赋加成
	AI_CastSpell(m_Spell.GetPreSpell().pSkillData->GetID(), obj, pos, m_Spell.GetPreSpell().itemInter.type, m_Spell.GetPreSpell().itemInter.index, m_Spell.GetPreSpell().itemInter.UID);
	m_Spell.ResetPreSpell();
#endif
	return true;
}

void GameObject::BreakCast()
{
	if(m_Spell.IsSpellRunning())
		m_Spell.BreakSpell();
}

void GameObject::ProcessCast(S32 dt)
{
	// 延时法术
	m_Spell.AdvanceScheduleSpell(dt);

	// 携带的触发法术
	m_Spell.AdvanceChainEP(dt);

	if(isDisabled())
		return;

#ifdef NTJ_SERVER
	// 目标死亡
	if(GetTarget() && GetTarget()->isDisabled())
	{
		SetTarget(NULL, false, SetTargetToClient);
		return;
	}
	// 自动攻击
	else if(GetTarget() && m_SkillTable.IsDefaultValid() && m_AutoCastTimer.CanAutoCast() && !isCanNotAttackBuff() && GetTarget()->isEnabled() && IsFaceTo(GetTarget()->getPosition()))
	{
		if(m_SkillTable.InDefaultRange(GetDistance(GetTarget())))
			CastSpell(m_SkillTable.GetDefaultSkillId(), *GetTarget());
	}
#endif

	// 法术施放
	if(m_Spell.IsSpellRunning())
		m_Spell.AdvanceSpell(dt);
}

void GameObject::UpdateAutoCast(GameObjectData::Animations action)
{
	SimTime dur = (isClientObject() && action == mUpperActionAnimation.action) ? mShapeInstance->getCommonDuration(mUpperActionAnimation.thread) * 1000 : GetStats().AttackSpeed/*攻击速度*/;
	m_AutoCastTimer.UpdateTimer(GetStats().AttackSpeed /*攻击速度*/, dur);
}

void GameObject::SetAutoCastDelay(SimTime delay)
{
	m_AutoCastTimer.SetDelay(Platform::getVirtualMilliseconds() + delay);
}

// 开启/关闭自动攻击
void GameObject::SetAutoCastEnable(bool enable)
{
	if(enable)
		enable = CanAttackTarget(GetTarget());
	if(IsAutoCastEnable() == enable)
		return;
	m_AutoCastTimer.SetEnable(enable);
	setMaskBits(GameObject::TargetMask);

#ifdef NTJ_CLIENT
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(conn && conn->getControlObject() == this)
	{
		ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_SET_AUTOCAST);
		pEvent->SetIntArgValues(1, IsAutoCastEnable());
		conn->postNetEvent(pEvent);
	}
#endif
}

S32 GameObject::DamageReduce(const Stats& stats, U32 damType, U32 buffEff, F32 scale, S32& reduce)
{
	static S32 dam, damPhy, damSpl, damPnm, heal, _dam, _damPhy, _damSpl, _damPnm;
	// 计算初始伤害
	damPhy = damType & BuffData::Damage_Phy ? stats.PhyDamage : 0;
	damSpl = damType & BuffData::Damage_Mu ? stats.MuDamage : 0;
	damSpl += damType & BuffData::Damage_Huo ? stats.HuoDamage : 0;
	damSpl += damType & BuffData::Damage_Tu ? stats.TuDamage : 0;
	damSpl += damType & BuffData::Damage_Jin ? stats.JinDamage : 0;
	damSpl += damType & BuffData::Damage_Shui ? stats.ShuiDamage : 0;
	damPnm = damType & BuffData::Damage_Pneuma ? stats.PneumaDamage : 0;
	heal = damType & BuffData::Damage_Heal ? stats.Heal : 0;
	dam = damPhy + damSpl + damPnm - heal;

	// 护甲减免（考虑极限护甲的情况）
	if(m_BuffTable.GetEffect() & BuffData::Effect_PhyDefMax)
		_damPhy = 1;
	else
		_damPhy = (damPhy && !(buffEff & BuffData::Effect_IgnorePhyDef)) ? (damPhy * (1.0f - getDamageReducePhy(GetStats().PhyDefence))) : damPhy;
	if(m_BuffTable.GetEffect() & BuffData::Effect_PhyDefMax)
		_damSpl = 1;
	else
		_damSpl = (damSpl && !(buffEff & BuffData::Effect_IgnoreSplDef)) ? (damSpl * (1.0f - getDamageReduceSpl(GetStats().MuDefence))) : damSpl;
	_damPnm = damPnm ? (damPnm * (1.0f - getDamageReducePneuma(GetStats().PneumaDefence))) : damPnm;

	// 伤害和治疗调整
	_damPhy *= mClampF(1.0f - GetStats().PhyScale_gPc/GAMEPLAY_PERCENT_SCALE, 0.0f, 2.0f);
	_damSpl *= mClampF(1.0f - GetStats().SplScale_gPc/GAMEPLAY_PERCENT_SCALE, 0.0f, 2.0f);
	heal *= mClampF(1.0f - GetStats().HealScale_gPc/GAMEPLAY_PERCENT_SCALE, 0.0f, 2.0f);
	_dam = _damPhy + _damSpl + _damPnm - heal;

	reduce = (dam - _dam) * scale;
	dam = _dam * scale;
	// hp 直接起效，无视其他任何减免与增强
	dam += mClampF(stats.MaxHP_gPc/GAMEPLAY_PERCENT_SCALE, -1.0f, 1.0f) * GetStats().MaxHP + stats.HP;

	return dam;
}

void GameObject::ApplyDamage(const Stats& stats, GameObject* pSource, U32 buffId)
{
#ifdef NTJ_SERVER
	static U32 damType, combatMask;
	static S32 dam, temDam, reduce;
	static bool exDam;
	static F32 scale;

	BuffData* pData = g_BuffRepository.GetBuff(buffId);
	if(!pData || isDisabled())
		return;

	// ------------------------------------- 是否受到伤害 ------------------------------------- //
	if(!isDamageable())
		return;

	// ------------------------------------- 独占检查 ------------------------------------- //
	if(pSource)
	{
		if((getGameObjectMask() & NpcObjectType) && !((NpcObject*)this)->checkExclusive(pSource))
			return;
		if((pSource->getGameObjectMask() & NpcObjectType) && !((NpcObject*)pSource)->checkExclusive(this))
			return;
	}

	// ------------------------------------- 捕捉 ------------------------------------- //
	if(canTame() != pData->IsFlags(BuffData::Flags_Tame))
		return;

	// ------------------------------------- 免疫伤害 ------------------------------------- //
	combatMask = 0;
	damType = pData->GetDamageType();
	if(GetBuffTable().GetEffect() & BuffData::Effect_ImmunityPhy)
		damType &= ~BuffData::Damage_Phy;
	if(GetBuffTable().GetEffect() & BuffData::Effect_ImmunitySpl)
		damType &= ~BuffData::Damage_Spl;
	// 发送免疫消息
	if(pData->GetDamageType() != damType)
		combatMask |= CombatLogEvent::CombatMask_Immunity;
	// 已完全免疫，直接退出
	if(0 == damType)
	{
		CombatLogEvent::show(CombatLogEvent::CombatInfo_Immunity, pSource, this, 0,0,0,0);
		return;
	}

	// ------------------------------------- 受到攻击 ------------------------------------- //
	if(!pData->IsFlags(BuffData::Flags_TgtTriDisabled) && (pData->GetDamageType() & BuffData::Damage_Phy))
		onBuffTrigger(BuffData::Trigger_UnderPhyAttack, pSource); // 统一状态触发<被物理攻击>
	if(!pData->IsFlags(BuffData::Flags_TgtTriDisabled) && (pData->GetDamageType() & BuffData::Damage_Spl))
		onBuffTrigger(BuffData::Trigger_UnderSplAttack, pSource); // 统一状态触发<被法术攻击>

	// ------------------------------------- 计算命中闪避 ------------------------------------- //
	if(!HitFilter(pSource, stats))
	{
		if(!pData->IsFlags(BuffData::Flags_TgtTriDisabled))
		{
			onBuffTrigger(BuffData::Trigger_Dodge, pSource); // 统一状态触发<闪避>
			if(pSource)
				pSource->onBuffTrigger(BuffData::Trigger_Miss, this); // 统一状态触发<未命中>
		}
		return;
	}
	if(!pData->IsFlags(BuffData::Flags_TgtTriDisabled))
	{
		onBuffTrigger(BuffData::Trigger_Wound, pSource); // 统一状态触发<未闪避>
		if(pSource)
			pSource->onBuffTrigger(BuffData::Trigger_Hit, this); // 统一状态触发<命中>
	}

	// ------------------------------------- 伤害随机 ------------------------------------- //
	scale = gRandGen.randF(mClampF((pSource ? pSource->GetStats().DamageHandle_gPc : 8000) / GAMEPLAY_PERCENT_SCALE, 0.0f, 1.0f), 1.0f);

	// ------------------------------------- 计算暴击 ------------------------------------- //
	if(CriticalFilter(pSource, stats))
	{
		scale = mClampF((GetStats().CriticalTimes + stats.CriticalTimes)/GAMEPLAY_PERCENT_SCALE, 1.0f, 5.0f);
		combatMask |= CombatLogEvent::CombatMask_Critical;

		if(!pData->IsFlags(BuffData::Flags_TgtTriDisabled))
		{
			onBuffTrigger(BuffData::Trigger_BeCriticalHit, pSource); // 统一状态触发<被暴击>
			if(pSource)
				pSource->onBuffTrigger(BuffData::Trigger_CriticalHit, this); // 统一状态触发<暴击>
		}
	}

	// ------------------------------------- 计算减免 ------------------------------------- //
	dam = DamageReduce(stats, damType, pData->GetEffect(), scale, reduce);
	onReduce(dam, reduce, pSource);
	exDam = (bool)stats.MP || (bool)stats.PP;

	// ------------------------------------- 伤害钳制 ------------------------------------- //
	if(0 == dam && 0 != damType && !pData->IsFlags(BuffData::Flags_Zero))
	{
		if(damType & (BuffData::Damage_Phy | BuffData::Damage_Spl))
			dam = (Platform::getVirtualMilliseconds() & 0x1) + 1;
		else if(damType & BuffData::Damage_Heal)
			dam = (Platform::getVirtualMilliseconds() & 0x1) - 2;
	}

	// ------------------------------------- 仇恨系统 ------------------------------------- //
	mHateList.addHate(pSource, dam);

	// ------------------------------------- 状态相关触发 ------------------------------------- //
	if(m_BuffTable.onTriggerGroup(pData, pSource))
		return;
	temDam = dam;
	if(!m_BuffTable.onTriggerShield(dam) && !exDam)
    {
        // 完全吸收
        CombatLogEvent::show(CombatLogEvent::CombatInfo_Absorb, pSource, this, 0,0,0,0);
		return;
    }
	if(temDam != dam)
		combatMask |= CombatLogEvent::CombatMask_Absorb;

	// ------------------------------------- 可否致死 ------------------------------------- //
	if(m_BuffTable.GetStats().HP <= dam && pData->IsFlags(BuffData::Flags_CanNotKill))
		dam = m_BuffTable.GetStats().HP - 1;

	// ------------------------------------- 应用伤害 ------------------------------------- //
	addHP(-dam);
	addMP(stats.MP);
	addPP(stats.PP);
	onDamage(dam,pSource);

	pSource->addHP(dam * mClampF(stats.DeriveHP_gPc/GAMEPLAY_PERCENT_SCALE, 0.0f, 2.0f));
	pSource->addMP(stats.MP * mClampF((pData->IsFlags(BuffData::Flags_DamDeriveMP) ? dam : -stats.DeriveMP_gPc)/GAMEPLAY_PERCENT_SCALE, 0.0f, 2.0f));

	if(dam > 0 && !pData->IsFlags(BuffData::Flags_TgtTriDisabled))
	{
		onBuffTrigger(BuffData::Trigger_Hurt, pSource); // 统一状态触发<受到伤害>
		if(pSource)
			pSource->onBuffTrigger(BuffData::Trigger_Damage, this); // 统一状态触发<造成伤害>
	}
	else if(!pData->IsFlags(BuffData::Flags_TgtTriDisabled))
	{
		onBuffTrigger(BuffData::Trigger_Healed, pSource); // 统一状态触发<受到治疗>
		if(pSource)
			pSource->onBuffTrigger(BuffData::Trigger_Heal, this); // 统一状态触发<治疗>
	}

	// ------------------------------------- 检查死亡 ------------------------------------- //
	if(m_BuffTable.GetStats().HP <= 0)
	{
		CombatLogEvent::show(CombatLogEvent::CombatInfo_Kill, pSource, this, 0,0,0,0);
		setDamageState(Disabled);
	}
	//else if(dam > 0)
	//	onWound();

	// ------------------------------------- 战斗日志 ------------------------------------- //
	if(!pData->IsFlags(BuffData::Flags_DisableCombatLog))
		CombatLogEvent::show(CombatLogEvent::CombatInfo_Damage, pSource, this, 0,-dam,stats.MP,stats.PP,combatMask);
#endif
}

bool GameObject::HitFilter(GameObject* pSource, const Stats& stats, bool sendLog /* = false */)
{
	// to do : 发送战斗信息
	if(pSource)
	{
		// 必中时忽略闪避
		if(pSource->GetBuffTable().GetEffect() & BuffData::Effect_GuaranteeHit)
			return true;
		if(stats.Hit_gPc + pSource->GetStats().Hit_gPc < gRandGen.randI(1,10000))
		{
			CombatLogEvent::show(CombatLogEvent::CombatInfo_Miss, pSource, this, 0,0,0,0);
			return false;
		}
	}
	// 禁止闪避
	if(m_BuffTable.GetEffect() & BuffData::Effect_AntiDodge)
		return true;
	if(stats.Dodge_gPc >= gRandGen.randI(1,10000))
	{
		CombatLogEvent::show(CombatLogEvent::CombatInfo_Dodge, pSource, this, 0,0,0,0);
		return false;
	}
	return true;
}

bool GameObject::CriticalFilter(GameObject* pSource, const Stats& stats)
{
	if(pSource)
	{
		// 必暴时忽略防暴
		if(pSource->GetBuffTable().GetEffect() & BuffData::Effect_GuaranteeCritical)
			return true;
		if(stats.Critical_gPc + GetStats().Critical_gPc - pSource->GetStats().AntiCritical_gPc >= gRandGen.randI(1,10000))
			return true;
	}
	return false;
}

bool GameObject::isDamageable() const
{
	 return isCombative() && !mInvisibleImmunityTick;
}

void GameObject::onDamage(S32 dam,GameObject* pSource)
{
#ifdef NTJ_SERVER
#endif
}

void GameObject::onReduce(S32 dam, S32 reduce, GameObject* pSource)
{
#ifdef NTJ_SERVER
	// 伤害或减免为负，说明可能是加血情况，忽视
	// 若目标对象是玩家
	if(dam <= 0 && pSource && pSource->getGameObjectMask() & PlayerObjectType)
	{
		((Player*)pSource)->reduceWear(1, dam);
	}
	
	// 若当前对象是玩家
	if(reduce <= 0 && getGameObjectMask() & PlayerObjectType)
	{
		((Player*)this)->reduceWear(2, reduce);
	}
#endif
}

void GameObject::onCombatBegin(GameObject* pObj)
{
}

void GameObject::onCombatEnd(GameObject* pObj)
{
}

F32 GameObject::GetDistance(GameObject* obj)
{
	return getDistance(obj);
}

void GameObject::say(StringTableEntry str)
{
#ifdef NTJ_SERVER
	if(str && str[0])
	{
		dStrcpy( mChatInfo, sizeof( mChatInfo ), str);
		setMaskBits( ChatInfoMask );
	}
#endif
}

void GameObject::setTriggerID(S32 triggerID)
{
	 mTriggerId = triggerID; 
}

void GameObject::drawHpBar(GameObject *mainPlayer,void *pParam1,void *pParam2)
{
#ifdef NTJ_CLIENT 
	if(!mainPlayer || mainPlayer->isDisabled())
		return;

	Vector<GFXTexHandle> *pTexList = (Vector<GFXTexHandle> *)pParam1;

	GFXTexHandle &HpBarFillTexture = (*pTexList)[dGuiMouseGamePlay::HpBarFillTexture];
	GFXTexHandle &HpBarBackgroundTexture = (*pTexList)[dGuiMouseGamePlay::HpBarBackgroundTexture];
	GFXTexHandle &HpBarbottomTexture = (*pTexList)[dGuiMouseGamePlay::HpBarbottomTexture];

	Point3F screenPos;
	Point3F objectPos;
	objectPos = getRenderPosition(); 
	objectPos.z += getWorldBox().len_z();

	if (g_UIMouseGamePlay->project(objectPos,&screenPos))
	{
		Point2I offset;
		offset.x = screenPos.x - HpBarBackgroundTexture.getWidth()*0.5;
		offset.y = screenPos.y - dGuiMouseGamePlay::HpOffsetY;
		//血条
		F32 HpBarScale = F32(GetStats().HP)/GetStats().MaxHP;

		RectI BottomRect(offset.x+7,offset.y+3,HpBarbottomTexture.getWidth(),HpBarbottomTexture.getHeight());
		RectI backgroundRect(offset.x,offset.y,HpBarBackgroundTexture.getWidth(),HpBarBackgroundTexture.getHeight());
		RectI fillRect(offset.x+7,offset.y+3,HpBarFillTexture.getWidth(),HpBarFillTexture.getHeight());
		fillRect.extent.x = fillRect.extent.x * HpBarScale;

		GFX->getDrawUtil()->clearBitmapModulation();
		GFX->getDrawUtil()->drawBitmapStretch(HpBarbottomTexture,BottomRect);
		GFX->getDrawUtil()->drawBitmapStretch(HpBarFillTexture,fillRect);
		GFX->getDrawUtil()->drawBitmapStretch(HpBarBackgroundTexture,backgroundRect);		 
	}
#endif
}


#ifdef NTJ_CLIENT
void GameObject::LoadImagetoTex()
{
    //伤害数字贴图
    if (!mNumTex[RED])
        mNumTex[RED]= GFXTexHandle(REDNUM, &GFXDefaultGUIProfile);
    if (!mNumTex[BLUE])
        mNumTex[BLUE]= GFXTexHandle(BLUENUM, &GFXDefaultGUIProfile);
    if (!mNumTex[YELLOW])
        mNumTex[YELLOW]= GFXTexHandle(YELLOWNUM, &GFXDefaultGUIProfile);
    if (!mNumTex[GREEN])
        mNumTex[GREEN]= GFXTexHandle(GREENNUM, &GFXDefaultGUIProfile);
    if (!mNumTex[ORANGE])
        mNumTex[ORANGE]= GFXTexHandle(ORANGENUM, &GFXDefaultGUIProfile);
    if (!mNumTex[PINK_P])
        mNumTex[PINK_P]= GFXTexHandle(PINK_PNUM, &GFXDefaultGUIProfile);
    if (!mNumTex[GREEN_P])
        mNumTex[GREEN_P]= GFXTexHandle(GREEN_PNUM, &GFXDefaultGUIProfile);

    //战斗文字贴图
    if (!mTextTex[ECHO])
        mTextTex[ECHO]= GFXTexHandle(ECHOTEX, &GFXDefaultGUIProfile);
    if (!mTextTex[DODGE])
        mTextTex[DODGE]= GFXTexHandle(DODGETEX, &GFXDefaultGUIProfile);
    if (!mTextTex[REBOUND])
        mTextTex[REBOUND]= GFXTexHandle(REBOUNDTEX, &GFXDefaultGUIProfile);
    if (!mTextTex[IMMUNE])
        mTextTex[IMMUNE]= GFXTexHandle(IMMUNETEX, &GFXDefaultGUIProfile);
    if (!mTextTex[MISS])
        mTextTex[MISS]= GFXTexHandle(MISSTEX, &GFXDefaultGUIProfile);
    if (!mTextTex[ABSORB])
        mTextTex[ABSORB]= GFXTexHandle(ABSORBTEX, &GFXDefaultGUIProfile);

    //经验荣誉贴图
    if (!mGainTex[EXP])
        mGainTex[EXP]= GFXTexHandle(EXPTEX, &GFXDefaultGUIProfile);
    if (!mGainTex[EXP_N])
        mGainTex[EXP_N]= GFXTexHandle(EXP_NTEX, &GFXDefaultGUIProfile);
    if (!mGainTex[CREDIT])
        mGainTex[CREDIT]= GFXTexHandle(CREDITTEX, &GFXDefaultGUIProfile);
    if (!mGainTex[CREDIT_N])
        mGainTex[CREDIT_N]= GFXTexHandle(CREDIT_NTEX, &GFXDefaultGUIProfile);
}

void GameObject::addDamageNumber(S32 number, U32 colorEnum, U32 pos, bool zoom)
{
    stDamageNum* info = new stDamageNum;
    char* szNumber	= new char[12];
    dSprintf(szNumber, 12, "%+d", number);
    info->text		= szNumber;    
    info->zoom		= zoom;
    info->id		= 0;

    //NPC只在中间冒数字，玩家在中间冒减益数字，右边冒增益数字
    if(getGameObjectMask() & NpcObjectType)
        info->pos   = 2;
    else
        info->pos   = number > 0?  1 : 2;

    info->Alpha = 255;
    info->eColor = (FontColor)colorEnum;

    mDamageNumList.push_front(info);
    if (mDamageNumList.size() > DamageNumMAX)
        mDamageNumList.pop_back();
}

void GameObject::addDamageText(GFXTexHandle* Tex)
{
	stDamageText info;
    info.id = 0;
    info.Alpha = 255;
    info.ptextText = Tex;
	
	mDamageTextList.push_back(info);
    if (mDamageTextList.size() > DamageTexMAX)
        mDamageTextList.pop_front();
}

void GameObject::addExpText(bool flag, U32 number)
{
    mExpOrCreText[flag].Alpha =255;
    mExpOrCreText[flag].id = 0;
    mExpOrCreText[flag].Num = number;
}

static struct {S32 x,y;} sPNumber_Center[20] =
{
	{16,3},{16,10},{16,23},{16,48},
	{16,95},{16,95},{16,85},{16,85},
	{16,94},{16,93},{16,86},{16,87},
	{16,92},{16,92},{16,88},{16,88},
	{16,91},{16,90},{16,89},{16,90}
};

static struct {S32 x,y;} sPNumber_Left[20] =
{
	{11,11},{14,15},{23,22},{39,31},
	{112,55},{112,55},{102,55},{102,55},
	{111,55},{110,55},{103,55},{104,55},
	{109,55},{109,55},{105,55},{105,55},
	{108,55},{107,55},{106,55},{107,55},
};

static struct {S32 x,y;} sPNumber_Right[20] = 
{
	{26,11},{23,15},{14,22},{-2,31},
	{-75,55},{-75,55},{-65,55},{-65,55},
	{-74,55},{-73,55},{-66,55},{-67,55},
	{-72,55},{-72,55},{-68,55},{-68,55},
	{-71,55},{-70,55},{-69,55},{-70,55},
};

void GameObject::drawNumTex(char* text ,Point2I Pos,FontColor ColorEnum , bool Zoom , int Alpha)
{
    S32 NumTexWidth	 = mNumTex[RED].getWidth() / 4;
    S32 NumTexHeight = mNumTex[RED].getHeight() / 4;    
    S32 TextLen = dStrlen(text);
    S32 Num;   
    S32 xModify = (NumTexWidth - 25)*(TextLen - 1)/2;
    Pos.x -= xModify;

    GFX->getDrawUtil()->setBitmapModulation(ColorI(255,255,255,Alpha));
    if (text[0] == '+')
        GFX->getDrawUtil()->drawBitmapStretchSR(mNumTex[ColorEnum], RectI(Pos.x , Pos.y, NumTexWidth / (2-Zoom), NumTexHeight / (2-Zoom)), RectI(0,0,NumTexWidth,NumTexHeight));
    else
        GFX->getDrawUtil()->drawBitmapStretchSR(mNumTex[ColorEnum], RectI(Pos.x , Pos.y, NumTexWidth / (2-Zoom), NumTexHeight / (2-Zoom)), RectI(NumTexWidth,0,NumTexWidth,NumTexHeight));

    for( U32 nCount = 1; nCount < TextLen; nCount++)
    {
        Num = text[nCount] - '0';
        RectI renderPos(Pos.x + (NumTexWidth - 25) / (2-Zoom) * (nCount), Pos.y, NumTexWidth / (2-Zoom), NumTexHeight / (2-Zoom));           
        RectI bmpOffset(NumTexWidth * ((Num + 2) %4), NumTexHeight * ((Num + 2) / 4), NumTexWidth, NumTexHeight);        
        GFX->getDrawUtil()->drawBitmapStretchSR(mNumTex[ColorEnum], renderPos, bmpOffset);        
    }
    GFX->getDrawUtil()->clearBitmapModulation();
}

void GameObject::drawTextTex(GFXTexHandle* pTex , Point2I Pos , U32 Alpha ,F32 ZoomRate)
{  
    GFX->getDrawUtil()->setBitmapModulation(ColorI(255,255,255,Alpha));

    RectI renderPos(Pos.x , Pos.y,  pTex->getWidth()*ZoomRate, pTex->getHeight()*ZoomRate);           
           
    GFX->getDrawUtil()->drawBitmapStretch((*pTex), renderPos);        

    GFX->getDrawUtil()->clearBitmapModulation();
}

void GameObject::drawExpTex(bool flag , U32 Number , Point2I Pos , U32 Alpha ,F32 ZoomRate)
{
    S32 NumTexWidth	 = mNumTex[RED].getWidth() / 4;
    S32 NumTexHeight = mNumTex[RED].getHeight() / 4;    
    char cNum[10];
    U32  Num = 0;
    sprintf_s(cNum , "%d", Number);
    S32 TextLen = dStrlen(cNum);
    GFXTexHandle *tempTex,*tempTexN;
    if (!flag)
    {
        tempTex = &GameObject::mGainTex[EXP];
        tempTexN = &GameObject::mGainTex[EXP_N];
    }       
    else
    {
        tempTex = &GameObject::mGainTex[CREDIT];
        tempTexN = &GameObject::mGainTex[CREDIT_N];
    }   
    
    S32 xModify = ((NumTexWidth - 25)*(TextLen - 1) + tempTex->getWidth() )/2;
    Pos.x -= xModify;

    GFX->getDrawUtil()->setBitmapModulation(ColorI(255,255,255,Alpha));

    RectI renderPos1(Pos.x , Pos.y - 32,  tempTex->getWidth()*ZoomRate, tempTex->getHeight()*ZoomRate);           

    GFX->getDrawUtil()->drawBitmapStretch((*tempTex), renderPos1);     

    for( U32 nCount = 0; nCount < TextLen; nCount++)
    {
        Num = cNum[nCount] - '0';
        RectI renderPos2(Pos.x + (tempTex->getWidth() + (NumTexWidth - 25) * (nCount))  * ZoomRate, Pos.y, NumTexWidth * ZoomRate, NumTexHeight * ZoomRate);           
        RectI bmpOffset(NumTexWidth * ((Num + 2) %4), NumTexHeight * ((Num + 2) / 4), NumTexWidth, NumTexHeight);        
        GFX->getDrawUtil()->drawBitmapStretchSR((*tempTexN), renderPos2, bmpOffset);        
    }

    GFX->getDrawUtil()->clearBitmapModulation();

}

void GameObject::drawDamageInfo()
{ 
    LoadImagetoTex();   
	Point3F screenPos;
	Point3F objectPos;
	Point2I renderPos, Pos;
	objectPos = getRenderPosition(); 
	objectPos.z += getWorldBox().len_z();
	if (!g_UIMouseGamePlay->project(objectPos,&screenPos))
	{
		renderPos.x = 400;
		renderPos.y = 300;
	}
	else
	{
		renderPos.x = screenPos.x;
		renderPos.y = screenPos.y + OffsetY;
	}   

//伤害数字
	for(S32 i = 0 , nCount = 0; i < mDamageNumList.size() ; ++i)
	{
		stDamageNum* info = mDamageNumList[i];
		if(!info)
		{
			mDamageNumList.erase_fast(i--);
			continue;
		}		

		if(info->id > 120)
		{
			delete info;
            mDamageNumList.erase_fast(i--);
            continue;
        }	
        if(info->id <= 19)
        {
            if(info->pos == 2)
            {
                Pos.x = renderPos.x - sPNumber_Center[info->id].x;
                Pos.y = renderPos.y - sPNumber_Center[info->id].y;
            }
            else if(info->pos == 0)
            {
                Pos.x = renderPos.x - sPNumber_Left[info->id].x;
                Pos.y = renderPos.y - sPNumber_Left[info->id].y;
            }
            else
            {
                Pos.x = renderPos.x - sPNumber_Right[info->id].x;
                Pos.y = renderPos.y - sPNumber_Right[info->id].y;
            }
        }
        else
        {
            if(info->pos == 2)
            {
                Pos.x = renderPos.x - sPNumber_Center[19].x;
                Pos.y = renderPos.y - sPNumber_Center[19].y;
            }
            else if(info->pos == 0)
            {
                Pos.x = renderPos.x - sPNumber_Left[19].x;
                Pos.y = renderPos.y - sPNumber_Left[19].y;
            }
            else
            {
                Pos.x = renderPos.x - sPNumber_Right[19].x;
                Pos.y = renderPos.y - sPNumber_Right[19].y;
            }                
            if(info->id >= 50 && info->Alpha > 4)
                info->Alpha -= 4;
        }

        Pos.y -= nCount*35;
        if (info->Alpha > 190 && nCount > 0)
            info->Alpha = 190;
        drawNumTex(info->text , Pos, info->eColor , info->zoom ,info->Alpha);
        ++nCount;
        info->id ++;
    }
//伤害状态文字
    for (std::list<stDamageText>::iterator it = mDamageTextList.begin();it != mDamageTextList.end();)
    {       
        Pos.x = renderPos.x;
        Pos.y = renderPos.y;
        if((*it).id >= 30 && (*it).Alpha > 5)
            (*it).Alpha -= 5;
        F32 ZoomRate = (float)((*it).id) / 10.0f;
        if (ZoomRate>1)ZoomRate = 1;
        

        drawTextTex((*it).ptextText,Pos,(*it).Alpha ,ZoomRate);

        (*it).id++;
        if((*it).id > 120)                  
            it =  mDamageTextList.erase(it);
        else
            it++;        	
    }
//经验值和荣誉值
    for (int i = 0 ;i < 2 ; ++i)
    {
        Pos.x = screenPos.x;
        Pos.y = screenPos.y - 15;
        if(mExpOrCreText[i].id >= 30 && mExpOrCreText[i].Alpha > 5)
            mExpOrCreText[i].Alpha -= 5;
        F32 ZoomRate = (float)(mExpOrCreText[i].id) / 10.0f;
        if (ZoomRate>1)ZoomRate = 1;

        if (mExpOrCreText[i].Alpha > 0)
            drawExpTex(mExpOrCreText[i].flag , mExpOrCreText[i].Num, Pos,mExpOrCreText[i].Alpha ,ZoomRate);
        
        mExpOrCreText[i].id++;
        if(mExpOrCreText[i].id > 120)                  
            mExpOrCreText[i].Alpha = 0;          	
    }
}

void GameObject::updateTrigger()
{
	CollisionWorkingList& rList = mConvex.getWorkingList();
	CollisionWorkingList* pList = rList.wLink.mNext;
	while (pList != &rList)
	{
		Convex* pConvex = pList->mConvex;

		U32 objectMask = pConvex->getObject()->getTypeMask();

		// Check: triggers, corpses and items...
		//
		if (objectMask & TriggerObjectType)
		{
			Trigger* pTrigger = dynamic_cast<Trigger*>(pConvex->getObject());
			if (pTrigger)
			{
				pTrigger->potentialEnterObject(this);
				/*if(mTriggerId != pTrigger->getTriggerData()->mTriggerID)
					setTriggerID(pTrigger->getTriggerData()->mTriggerID);*/
			}
		}

		pList = pList->wLink.mNext;
	}
}
#endif//NTJ_CLIENT

//----------------------------------------------------------------------------
// 脚本函数放到最后，否则VAX会不正常

#ifdef NTJ_CLIENT

ConsoleFunction(CastSpell, void, 2,2,"")
{
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(player && player->GetTarget())
		player->CastSpell(dAtoi(argv[1]), *player->GetTarget());
}

#endif

ConsoleMethod( GameObject, getState, const char*, 2, 2, "Return the current state name.")
{
	return object->getStateName();
}

ConsoleMethod( GameObject, getDamageLocation, const char*, 3, 3, "(Point3F pos)")
{
	const char *buffer1;
	const char *buffer2;
	char *buff = Con::getReturnBuffer(128);

	Point3F pos(0.0f, 0.0f, 0.0f);
	dSscanf(argv[2], "%g %g %g", &pos.x, &pos.y, &pos.z);
	object->getDamageLocation(pos, buffer1, buffer2);

	dSprintf(buff, 128, "%s %s", buffer1, buffer2);
	return buff;
}

ConsoleMethod( GameObject, setActionThread, bool, 3, 5, "(string sequenceName, bool hold, bool fsp)")
{
	bool hold = (argc > 3)? dAtob(argv[3]): false;
	bool fsp  = (argc > 4)? dAtob(argv[4]): true;
	return object->setActionThread(argv[2],hold,true,fsp);
}

ConsoleMethod( GameObject, setControlObject, bool, 3, 3, "(ShapeBase obj)")
{
	ShapeBase* controlObject;
	if (Sim::findObject(argv[2],controlObject)) {
		object->setControlObject(controlObject);
		return true;
	}
	else
		object->setControlObject(0);
	return false;
}

ConsoleMethod( GameObject, getControlObject, S32, 2, 2, "Get the current control object.")
{
	ShapeBase* controlObject = object->getControlObject();
	return controlObject ? controlObject->getId(): 0;
}

ConsoleMethod( GameObject, clearControlObject, void, 2, 2, "")
{
	object->setControlObject(0);
}

ConsoleMethod( GameObject, checkDismountPoint, bool, 4, 4, "(Point3F oldPos, Point3F pos)")
{
	Point3F oldPos(0.0f, 0.0f, 0.0f);
	Point3F pos(0.0f, 0.0f, 0.0f);
	dSscanf(argv[2], "%g %g %g",
		&oldPos.x,
		&oldPos.y,
		&oldPos.z);
	dSscanf(argv[3], "%g %g %g",
		&pos.x,
		&pos.y,
		&pos.z);
	MatrixF oldPosMat(true);
	oldPosMat.setColumn(3, oldPos);
	MatrixF posMat(true);
	posMat.setColumn(3, pos);
	return object->checkDismountPosition(oldPosMat, posMat);
}
