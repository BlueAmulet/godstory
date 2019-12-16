#include "console/consoleTypes.h"
#include "console/console.h"
#include "core/bitStream.h"
#include "T3D/gameFunctions.h"
#include "ts/tsShapeInstance.h"
#include "ts/TSShapeRepository.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/Data/readDataFile.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/Data/EventTriggerData.h"
#include "Gameplay/GameObjects/LivingSkill.h"
#include "Gameplay/Item/DropRule.h"
#include "Gameplay/Item/DropItem.h"
#include "Gameplay/GameObjects/CollectionObject.h"
#include "Gameplay/Item/GatherMaterial.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/GameEvents/ServerGameNetEvents.h"
#include "Gameplay/GameEvents/SimCallBackEvents.h"
#include "Gameplay/GameObjects/ScheduleManager.h"
#include "Gameplay/Mission/PlayerMission.h"
#include "Gameplay/Item/ItemPackage.h"
#include "Gameplay/Item/ItemBaseData.h"

#ifdef NTJ_CLIENT
#include "math/mRect.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "ui/dGuiMouseGamePlay.h"
#endif

#ifndef TickMs
#define TickMs      32
#define TickSec     (F32(TickMs) / 1000.0f)
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////               CollectionOjbect               //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_CO_NETOBJECT_V1(CollectionObject);

CollectionObject::CollectionObject()
{
	mNetFlags.set(Ghostable);
	//mTypeMask |= ItemObjectType;
	mItemMask |= CollectionObjectType;
	mDataBlock = NULL;
	mConvexList = new Convex;

	mActionAnimation.action = 0;
	mActionAnimation.delayTicks = 0;
	mActionAnimation.forward = true;
	mActionAnimation.thread = 0;
	mActionAnimation.atEnd = false;

#ifdef NTJ_SERVER
	m_DropItemList = NULL;
	mCurrTriggerTimes = 0;
	m_DropItemList = NULL;
	mElapseTime = 0;
	setMaskBits(PositionMask | TriggerTimesMask);
#endif

#ifndef NTJ_SERVER
	m_bRemove = false;
#endif
}

CollectionObject::~CollectionObject()
{
	delete mConvexList;
	mConvexList = NULL;
#ifdef NTJ_SERVER
	if (m_DropItemList)
		delete m_DropItemList;
	m_DropItemList = NULL;
#endif
}

//	potentialEnterTrigger中需要进行以下三个条件需要判断
//	（1）触发次数判断
//	（2）触发器条件判断
//	（3）生活技能判断
enWarnMessage CollectionObject::potentialEnterTrigger(Player *pPlayer)
{
	enWarnMessage msg = MSG_NONE;
	msg = pPlayer->isBusy(Player::INTERACTION_PICKUP);
	if(msg != MSG_NONE)
		return msg;

	if(pPlayer->isMounted())
		return MSG_PLAYER_RIDE;

	if ( IsTriggerCountExceed() )
	{
		//msg = MSG_COLLECTION_NOT_ENOUGTH;		//可采集次数为0
		return msg;
	}

	return enterTrigger(pPlayer);
}

enWarnMessage CollectionObject::enterTrigger(Player *pPlayer)
{
#ifndef NTJ_EDITOR
	enWarnMessage msg = MSG_NONE;

	EventTriggerData *pEventTriggerData = gEventTriggerRepository.getEventTriggerData(mDataBlock->mEventTriggerId);
	if (!pEventTriggerData)
	{
		//msg = MSG_EVENT_TRIGGER_DATA_ERROR;	//事件触发数据错误
		return MSG_UNKOWNERROR;
	}

#ifdef NTJ_SERVER
	//服务端触发器前置脚本条件判断
	bool isConditionValid  = atoi(Con::executef("EventTriggerCondition", Con::getIntArg( pPlayer->getId()), Con::getIntArg(mDataBlock->mEventTriggerId) ));
	if (!isConditionValid)
		return MSG_COLLECTION_COND_INVALID;
#endif

	//生活技能判断
	msg = g_GatherMaterial.canGather(pPlayer,pEventTriggerData->mLivingSkillId);
	if(msg != MSG_NONE)
		return msg;

	SimTime nVocalTime = getVocalTime();

#ifdef NTJ_CLIENT
	//施放技能
	msg = g_GatherMaterial.spellLivingSkill(pPlayer,this);
	if(msg != MSG_NONE)
		return msg;

	clientSendStartMsg(pPlayer);

	if (nVocalTime != 0)
	{
		//客户端开始吟唱
		if ( g_ClientGameplayState->pVocalStatus->getStatusType() != VocalStatus::VOCALSTATUS_COLLECT &&
			 g_ClientGameplayState->pVocalStatus->getStatus() )
		{
			g_ClientGameplayState->pVocalStatus->cancelVocalEvent();
			return MSG_NONE; 
		}

		LivingSkillData* pData = g_LivingSkillRespository.getLivingSkillData(pEventTriggerData->mLivingSkillId);
		if(!pData)
			return MSG_LIVINGSKILL_DATAERROR;
		Con::executef("InitFetchTimeAnimProgress",Con::getIntArg(nVocalTime),Con::getIntArg((S32)VocalStatus::VOCALSTATUS_COLLECT), "", Con::getIntArg(pData->getCast().readyLoopAction));	
	}
#endif

#ifdef NTJ_SERVER
	if (nVocalTime == 0)
	{
		//不需要吟唱，则直接执行预定义事件
		ExecPredefinedEvent(pPlayer, pEventTriggerData->mEventType, pEventTriggerData->mEventId);
		//通知客户端触发开始
		//执行服务端事件脚本
	}
	else
	{
		if (pPlayer->pScheduleEvent->getEventID() == 0)
		{
			LivingSkillData* pData = g_LivingSkillRespository.getLivingSkillData(pEventTriggerData->mLivingSkillId);
			if(!pData)
				return MSG_LIVINGSKILL_DATAERROR;

			//需要吟唱，服务端进行同步吟唱
			simCallBackEvents* Event = new simCallBackEvents(INFO_COLLECTION_ITEM);
			Event->SetInt32ArgValues(3, getId(), pEventTriggerData->mEventType, pEventTriggerData->mEventId);
			U32 ret = Sim::postEvent(pPlayer, Event,Sim::getCurrentTime() + nVocalTime);
			pPlayer->pScheduleEvent->setEvent(ret, nVocalTime, pData->getCast().readyLoopAction); 
		}
		else
		{
			//有吟唱没有结束
			pPlayer->pScheduleEvent->cancelEvent(pPlayer);
			return MSG_NONE;
		}
	}
#endif

#endif
	return MSG_NONE;
}

void CollectionObject::leaveTrigger(Player *pPlayer)
{
#ifdef NTJ_SERVER
	if (mCurrTriggerTimes != -1)
		mCurrTriggerTimes--;
#endif
}

#ifdef NTJ_CLIENT
void CollectionObject::drawObjectInfo(Player *pMainPlayer, void *param1, void *param2)
{
	if (!checkTaskList(pMainPlayer))
		return;

	//画头顶信息
	GuiControlProfile** pStyleList = (GuiControlProfile **)param2;

	if(!pStyleList)
		return;

	char objectName[64]  = {0,};
	char titleName[COMMON_STRING_LENGTH]   = {0,};

	Point2I objectNameOffset;
	Point2I titleNameOffset;
	Point2I NpcIconOffset;

	Point3F screenPos;
	Point3F objectPos;
	ColorI objectNameColor(COLORI_NPC_FRIEND);
	ColorI titleNameColor(COLORI_NPC_TITLE);
	Point2I point;
	GuiControlProfile* pStyle = pStyleList[dGuiMouseGamePlay::StyleOne];
	GuiControlProfile* pStyle1 = pStyleList[dGuiMouseGamePlay::StyleTwo];

	objectPos = getRenderPosition(); 
	objectPos.z += getWorldBox().len_z();
	if (g_UIMouseGamePlay->project(objectPos,&screenPos))
	{
		if (dStrcmp(mDataBlock->mCollectionName, ""))
		{
			//以后还需修改
			point.x = screenPos.x - pStyle->mFont->getStrWidth(const_cast<char*>(mDataBlock->mCollectionName)) * 0.5f;
			point.y = screenPos.y - (pStyle->mFont->getHeight() + dGuiMouseGamePlay::HpOffsetY);
			GFX->getDrawUtil()->drawTextOutline(pStyle->mFont, point, mDataBlock->mCollectionName, &objectNameColor, true, pStyle->mFontOutline);
		}
	}
}

void CollectionObject::clientSendStartMsg(Player *pPlayer)
{
	GameConnection *conn = NULL;

	if (!pPlayer || (conn = pPlayer->getControllingClient()) == NULL)
		return;

	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_COLLECTION_ITEM);
	if (!ev)
		return;
	ev->SetInt32ArgValues(2, this->getServerId(), 1);
	conn->postNetEvent(ev);
}

void CollectionObject::clientSendEndMsg(Player *pPlayer)
{
	GameConnection *conn = NULL;

	if (!pPlayer || (conn = pPlayer->getControllingClient()) == NULL)
		return;

	ClientGameNetEvent *ev = new ClientGameNetEvent(INFO_COLLECTION_ITEM);
	if (!ev)
		return;
	ev->SetInt32ArgValues(2, this->getServerId(), 2);
	conn->postNetEvent(ev);
}
#endif

#ifdef NTJ_SERVER
void CollectionObject::serverSendStartMsg(Player *pPlayer, U32 nEventType)
{
	GameConnection *conn = NULL;

	if (!pPlayer || (conn = pPlayer->getControllingClient()) == NULL)
		return;

	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_COLLECTION_ITEM);
	if (!ev)
		return;
	ev->SetInt32ArgValues(2, 1, nEventType);
	conn->postNetEvent(ev);
}
void CollectionObject::serverSendEndMsg(Player *pPlayer)
{
	GameConnection *conn = NULL;

	if (!pPlayer || (conn = pPlayer->getControllingClient()) == NULL)
		return;

	ServerGameNetEvent *ev = new ServerGameNetEvent(INFO_COLLECTION_ITEM);
	if (!ev)
		return;
	ev->SetInt32ArgValues(1, 2);
	conn->postNetEvent(ev);
}

void CollectionObject::ExecPredefinedEvent(Player *pPlayer, U8 nEventType, U32 nEventId)
{
	if(!pPlayer || !pPlayer->pScheduleEvent)
		return;

	if (mCurrTriggerTimes != -1)
		mCurrTriggerTimes--;
	setMaskBits(TriggerTimesMask);

	switch(nEventType)
	{
	case 1:		//掉物事件类型
		{
			U32 nItemPackageId = 0;
			S32 nMaxDropNum = 0;
			DropRuleData *pDropRuleData = g_DropRuleRepository.getDropRuleData(nEventId);
			if (pDropRuleData == NULL)
				return;

			// 生成物品掉落列表
			DropItemList* droplist = new DropItemList;
			// 将玩家添加入物品掉物列表
			if (pDropRuleData->GetItemPackageID(nEventId, pPlayer->getLevel(), 0, nItemPackageId, nMaxDropNum, true))				
			{
				droplist->AddPlayer(pPlayer, nItemPackageId);
			}

			if (m_DropItemList != NULL)
				delete m_DropItemList;
			this->m_DropItemList = droplist;
		}
		break;

	case 2:		//技能对象
		{
			
		}
		break;

	case 3:		//脚本触发
		{			
			EventTriggerData *pEventTriggerData = gEventTriggerRepository.getEventTriggerData(mDataBlock->mEventTriggerId);
			if (pEventTriggerData)
			{
				enWarnMessage msg = g_GatherMaterial.gatherCast(pPlayer,pEventTriggerData->mLivingSkillId);
				if(msg == MSG_NONE)
				{
					if (pPlayer && mDataBlock)
					{
						Con::executef("OnServerTriggerEvent", Con::getIntArg(pPlayer->getId()), Con::getIntArg(mDataBlock->dataBlockId));
					}
					//pPlayer->addMoney(1);//测试用
				}
				else
				{
					MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR,msg);
					pPlayer->pScheduleEvent->cancelEvent(pPlayer);
					return;
				}
			}
			serverSendEndMsg(pPlayer);
			return;
		}
		break;
	default:
		return;
	}

	//通知客户端掉服务端预定义事件执行完毕,可以继续操作（例如拾取掉落的物品）
	serverSendStartMsg(pPlayer, nEventType);
}
#endif

SimTime CollectionObject::getVocalTime()
{
	SimTime nVocalTime = 0;
#ifndef NTJ_EDITOR
	EventTriggerData *pEventTriggerData = gEventTriggerRepository.getEventTriggerData(mDataBlock->mEventTriggerId);
	if (!pEventTriggerData)
		return nVocalTime;
	LivingSkillData *pData = g_LivingSkillRespository.getLivingSkillData(pEventTriggerData->mLivingSkillId);
	if (pData)
		nVocalTime = pData->getCast().readyTime;
#endif
	return nVocalTime;
}

void CollectionObject::processTick(const Move * move)
{
	Parent::processTick(move);

#ifdef NTJ_CLIENT
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	F32 len = (getPosition() - pPlayer->getPosition()).len();
	if ( len > mDataBlock->mSenseDistance )
	{
		pPlayer->deleteFromCollectionList(this);
	}
	else
	{
		pPlayer->addToCollectionList(this);
	}
#endif

#ifdef NTJ_SERVER
	if (mCurrTriggerTimes == 0 && mElapseTime == 0)
	{
		mElapseTime = Platform::getRealMilliseconds();
	}
	else if (mElapseTime > 0)
	{
		//是否刷新
		SimTime nCurrentTime = Platform::getRealMilliseconds();
		if ( (nCurrentTime - mElapseTime) >= mDataBlock->mRefreshTime )
		{
			mElapseTime = 0;
			mCurrTriggerTimes = mDataBlock->mTriggeredTimes;
			setMaskBits(TriggerTimesMask);
		}
	}
#endif

#ifndef NTJ_SERVER
	if (mCurrTriggerTimes != 0)
	{
		m_bRemove = false;
	}
#endif

	updateAnimations();
}

bool CollectionObject::onAdd()
{
	// 直接从CollectionRepository中读取dataBlock
	if(!GameBase::mDataBlock)
		GameBase::mDataBlock = dynamic_cast<GameBaseData*>(gCollectionRepository.GetCollectionObjectData(mDataBlockId));

	if(!Parent::onAdd())
		return false;

	CollectionObjectData *pCollectionObjectData = gCollectionRepository.GetCollectionObjectData(mDataBlockId);
	setScale(pCollectionObjectData->mObjScale);

	addToScene();

#ifdef NTJ_SERVER
	mCurrTriggerTimes = mDataBlock->mTriggeredTimes;
#endif
	return true;
}

void CollectionObject::onRemove()
{
	scriptOnRemove();
	removeFromScene();
	Parent::onRemove();
}

bool CollectionObject::prepRenderImage(SceneState* state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState)
{
#ifndef NTJ_SERVER
	if (m_bRemove)
		return false;
#endif

	return Parent::prepRenderImage(state, stateKey, startZone, modifyBaseZoneState);
}

bool CollectionObject::canAddToScope(NetConnection *con)
{
#ifndef NTJ_SERVER
	if (m_bRemove)
		return false;
#endif

	return true;
}

U64	CollectionObject::packUpdate(NetConnection *conn, U64 mask, BitStream *stream)
{
	U64 retMask = Parent::packUpdate(conn, mask, stream);

	if ( stream->writeFlag(mask & PositionMask) )
	{
		Point3F pos;
		getTransform().getColumn(3,&pos);
		stream->writeCompressedPoint(pos);
	}
	if ( stream->writeFlag(mask & TriggerTimesMask) )
	{
		stream->writeInt(mCurrTriggerTimes, Base::Bit32);
	}
	return retMask;
}

void CollectionObject::unpackUpdate(NetConnection *conn, BitStream *stream)
{
	Parent::unpackUpdate(conn, stream);

	if (stream->readFlag())
	{
		Point3F pos;
		stream->readCompressedPoint(&pos);
		setPosition(pos);
	}

	if (stream->readFlag())
	{
		mCurrTriggerTimes = stream->readInt(Base::Bit32);
	}
}

bool CollectionObject::onNewDataBlock(GameBaseData* dptr)
{
	mDataBlock = dynamic_cast<CollectionObjectData*>(dptr);
	if (!mDataBlock)
		return false;

	if (!Parent::onNewDataBlock(mDataBlock))
		return false;

#ifdef NTJ_CLIENT
	//初始化CollectionOjbect的待机动画
	mActionAnimation.thread = mShapeInstance->addThread();
	mActionAnimation.action = GameObjectData::Idle_a0;
	mActionAnimation.atEnd = false;
	S32 nSequence = g_TSShapeRepository.GetSequence(mDataBlock->shapeName, mActionAnimation.action);
	mShapeInstance->setSequence(mActionAnimation.thread, nSequence, 0);
	mShapeInstance->setPos(mActionAnimation.thread, 0);
	mShapeInstance->setTimeScale(mActionAnimation.thread, 1);
#endif
	scriptOnNewDataBlock();
	return true;
}

bool CollectionObject::updateShape()
{
	if(!Parent::updateShape())
		return false;

	if(isClientObject())
	{
#ifdef NTJ_CLIENT
		//初始化CollectionOjbect的待机动画
		mActionAnimation.thread = mShapeInstance->addThread();
		mActionAnimation.action = GameObjectData::Idle_a0;
		mActionAnimation.atEnd = false;
		S32 nSequence = g_TSShapeRepository.GetSequence(mDataBlock->shapeName, mActionAnimation.action);
		mShapeInstance->setSequence(mActionAnimation.thread, nSequence, 0);
		mShapeInstance->setPos(mActionAnimation.thread, 0);
		mShapeInstance->setTimeScale(mActionAnimation.thread, 1);
#endif
	}
	return true;
}

void CollectionObject::updateAnimations()
{
#ifdef NTJ_CLIENT
	if (m_bRemove)
		return;

	if (mCurrTriggerTimes == 0 && mActionAnimation.action != GameObjectData::Death)
	{
		//待机动画转到死亡动画
		mActionAnimation.action = GameObjectData::Death;
		mActionAnimation.atEnd = false;
		S32 nSequence = g_TSShapeRepository.GetSequence(mDataBlock->shapeName, mActionAnimation.action);
		mShapeInstance->setSequence(mActionAnimation.thread, nSequence, 0);
		mShapeInstance->setPos(mActionAnimation.thread, 0);
		mShapeInstance->setTimeScale(mActionAnimation.thread, 1);
		return;
	}

	if (mCurrTriggerTimes != 0  && mActionAnimation.action == GameObjectData::Death)
	{
		//死亡动画转到待机动画
		mActionAnimation.action = GameObjectData::Idle_a0;
		mActionAnimation.atEnd = false;
		S32 nSequence = g_TSShapeRepository.GetSequence(mDataBlock->shapeName, mActionAnimation.action);
		mShapeInstance->setSequence(mActionAnimation.thread, nSequence, 0);
		mShapeInstance->setPos(mActionAnimation.thread, 0);
		mShapeInstance->setTimeScale(mActionAnimation.thread, 1);
		return;
	}

	mShapeInstance->advanceTime(TickSec, mActionAnimation.thread);		//动画时间推进
	mActionAnimation.atEnd = mShapeInstance->getPos(mActionAnimation.thread) == 1;
	if (mActionAnimation.action == GameObjectData::Death && mActionAnimation.atEnd)
		m_bRemove = true;
#endif
}

bool CollectionObject::castRay(const Point3F &start, const Point3F &end, RayInfo* info)
{
	// Collide against bounding box
	F32 st,et,fst = 0,fet = 1;
	F32 *bmin = &mObjBox.min.x;
	F32 *bmax = &mObjBox.max.x;
	F32 const *si = &start.x;
	F32 const *ei = &end.x;

	for (int i = 0; i < 3; i++) 
	{
		if (*si < *ei) 
		{
			if (*si > *bmax || *ei < *bmin)
				return false;
			F32 di = *ei - *si;
			st = (*si < *bmin)? (*bmin - *si) / di: 0;
			et = (*ei > *bmax)? (*bmax - *si) / di: 1;
		}
		else 
		{
			if (*ei > *bmax || *si < *bmin)
				return false;
			F32 di = *ei - *si;
			st = (*si > *bmax)? (*bmax - *si) / di: 0;
			et = (*ei < *bmin)? (*bmin - *si) / di: 1;
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

bool CollectionObject::collideBox(const Point3F &start, const Point3F &end, RayInfo* info)
{
	if  (!Parent::collideBox(start, end, info))
		return false;

#ifdef NTJ_CLIENT
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return false;

	return checkTaskList(pPlayer);
#endif

	return true;
}

bool CollectionObject::checkTaskList(Player *pPlayer)
{
	if (mDataBlock->mArrTaskIds.size() == 0)
		return true;

	for (S32 i = 0; i < mDataBlock->mArrTaskIds.size(); i++)
	{
		U32 nTaskId = mDataBlock->mArrTaskIds[i];
		S32 iPos = 0;
		if ( !g_MissionManager->IsAcceptedMission(pPlayer, nTaskId, iPos) )		
			return false;
	}

#ifndef NTJ_EDITOR
	//判断任务旗标，以及掉落任务物品
	EventTriggerData *pEventData = gEventTriggerRepository.getEventTriggerData(mDataBlock->mEventTriggerId);
	if (pEventData && pEventData->mEventType == 1)	//事件类型为掉落事件
	{
		DropRuleData *pDropRuleData = g_DropRuleRepository.getDropRuleData(pEventData->mEventId);
		if (!pDropRuleData)
			return true;
		S32 nMaxNum = 0;
		U32 nItemPackageId = 0;
		if (pDropRuleData->GetItemPackageID(pDropRuleData->m_nDropEventId, pPlayer->getLevel(), 0, nItemPackageId, nMaxNum, true))
			return true;
		ItemPackage *pItemPackage = g_ItemPackageRepository.getPackage(nItemPackageId);
		if (!pItemPackage)
			return true;

		Vector<ItemProbobilityInfo>::iterator it = pItemPackage->m_itemsInfo.begin();
		while(it != pItemPackage->m_itemsInfo.end())
		{
			ItemBaseData *pItemBaseData = g_ItemRepository->getItemData((*it).m_nId);
			if (pItemBaseData && pItemBaseData->isTaskItem())
			{
				if (!g_MissionManager->IsNeedTaskItem(pPlayer, (*it).m_nId))
					return false;
			}
			++it;
		}
	}
#endif
	return true;
}

void CollectionObject::buildConvex(const Box3F& box, Convex* convex)
{
	// These should really come out of a pool
	mConvexList->collectGarbage();

	Box3F realBox = box;
	mWorldToObj.mul(realBox);
	realBox.min.convolveInverse(mObjScale);
	realBox.max.convolveInverse(mObjScale);

	if (realBox.isOverlapped(getObjBox()) == false)
		return;

	// Just return a box convex for the entire shape...
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
	BoxConvex* cp = new BoxConvex;
	mConvexList->registerObject(cp);
	convex->addToWorkingList(cp);
	cp->init(this);

	mObjBox.getCenter(&cp->mCenter);
	cp->mSize.x = mObjBox.len_x() / 2.0f;
	cp->mSize.y = mObjBox.len_y() / 2.0f;
	cp->mSize.z = mObjBox.len_z() / 2.0f;
}

bool CollectionObject::IsTriggerCountExceed()
{
	if (mCurrTriggerTimes == 0)
	{
		return true;
	}

	return false;
}

bool CollectionObject::IsLoopTrigger()
{
	return mDataBlock->mIsLoopTrigger;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////              CollectionRepository            //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
CollectionRepository gCollectionRepository;

CollectionRepository::CollectionRepository()
{

}

CollectionRepository::~CollectionRepository()
{
	Clear();
}

CollectionObjectData *CollectionRepository::GetCollectionObjectData(U32 dataBlockId)
{
	CollectionDataMap::iterator it = mMap.find(dataBlockId);

	if (it == mMap.end())
		return NULL;

	return it->second;
}

void CollectionRepository::Read()
{
	CDataFile op;
	RData tempdata;
	char filename[256];
	CollectionObjectData* pCollectionObjData = NULL;

	Platform::makeFullPathName(GAME_COLLECTIONDATA_FILE, filename, sizeof(filename));
	op.readDataFile(filename);
	//U32					mCollectionId;			//物件ID
	//StringTableEntry		mCollectionName;		//物件名称
	//U32					mModel1;				//模型编号
	//U32					mModel2;				//触发后模型编号
	//U32					mType;					//物件类别
	//U32					mSubType;				//物件子类
	//U32					mRefreshTime;			//刷新时间
	//U32					mReserveTime;			//触发后保留时间
	//U32					mEventTriggerId			//物件类触发器ID
	//U32					mRouteId;				//路径ID
	//bool					mIsLoopTrigger;			//可循环触发
	//StringTableEntry		mTriggerSound;			//触发后音效
	//bool					mIsSensible;			//是否可感知
	//F32					mSenseDistance;			//感知范围
	//F32					mTriggerDistance;		//触发范围
	//U32					mFadeTime;				//淡入淡出时间
	//U32					mTriggerTimes;			//可触发次数

	for(int i=0; i<op.RecordNum; ++i)
	{
		pCollectionObjData = new CollectionObjectData;

		//物件ID
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "CollectionRepository.dat::Read - Failed to read mCollectionId", i);
		pCollectionObjData->dataBlockId = tempdata.m_U32;

		//物件名称
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "CollectionRepository.dat::Read - Failed to read mCollectionName", i);
		pCollectionObjData->mCollectionName = StringTable->insert(tempdata.m_string);

		//模型编号
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "CollectionRepository.dat::Read - Failed to read shapeName", i);
		pCollectionObjData->shapesSetId = tempdata.m_U32;

		//触发后模型编号
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "CollectionRepository.dat::Read - Failed to read mTriggeredShape", i);
		pCollectionObjData->mTriggeredShapesSetId = tempdata.m_U32;

		//物件类别
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "CollectionRepository.dat::Read - Failed to read mType", i);
		pCollectionObjData->mType = tempdata.m_U32;

		//物件子类
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "CollectionRepository.dat::Read - Failed to read mSubType", i);
		pCollectionObjData->mSubType = tempdata.m_U32;

		//刷新时间
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "CollectionRepository.dat::Read - Failed to read mRefreshTime", i);
		pCollectionObjData->mRefreshTime = tempdata.m_U32;

		//触发后保留时间
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "CollectionRepository.dat::Read - Failed to read mReserveTime", i);
		pCollectionObjData->mReserveTime = tempdata.m_U32;

		//事件ID
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "CollectionRepository.dat::Read - Failed to read mEventTriggerId", i);
		pCollectionObjData->mEventTriggerId = tempdata.m_U32;

		//路径ID
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "CollectionRepository.dat::Read - Failed to read mRouteId", i);
		pCollectionObjData->mRouteId = tempdata.m_U32;

		//可循环触发
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_S8, "CollectionRepository.dat::Read - Failed to read mIsLoopTrigger", i);
		pCollectionObjData->mIsLoopTrigger = (tempdata.m_S8 != 0);

		//触发后音效
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "CollectionRepository.dat::Read - Failed to read mTriggerSound", i);
		pCollectionObjData->mTriggerSound = tempdata.m_string;

		//是否可感知
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_S8, "CollectionRepository.dat::Read - Failed to read mIsSensible", i);
		pCollectionObjData->mIsSensible = (tempdata.m_S8 != 0);

		//感知范围
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_F32, "CollectionRepository.dat::Read - Failed to read mSenseDistance", i);
		pCollectionObjData->mSenseDistance = tempdata.m_F32;

		//触发范围
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_F32, "CollectionRepository.dat::Read - Failed to read mTriggerDistance", i);
		pCollectionObjData->mTriggerDistance = tempdata.m_F32;

		//淡入淡出时间
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "CollectionRepository.dat::Read - Failed to read mFadeTime", i);
		pCollectionObjData->mFadeTime = tempdata.m_U32;

		//可触发次数
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "CollectionRepository.dat::Read - Failed to read mTriggerTimes", i);
		pCollectionObjData->mTriggeredTimes = tempdata.m_U32;

		//任务ID字符串:  101,102,103,104...
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "CollectionRepository.dat::Read - Failed to read mTriggerTimes", i);
		pCollectionObjData->FillTaskIds(tempdata.m_string);

		//缩放x
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_F32, "CollectionRepository.dat::Read - Failed to read mObjScale.x", i);
		pCollectionObjData->mObjScale.x = tempdata.m_F32;
		
		//缩放y
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_F32, "CollectionRepository.dat::Read - Failed to read mObjScale.y", i);
		pCollectionObjData->mObjScale.y = tempdata.m_F32;

		//缩放z
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_F32, "CollectionRepository.dat::Read - Failed to read mObjScale.z", i);
		pCollectionObjData->mObjScale.z = tempdata.m_F32;

		addToUserDataBlockGroup(pCollectionObjData);

		if ( !Insert(pCollectionObjData) )
			break;
	}
}

bool CollectionRepository::Insert(CollectionObjectData *pData /*= NULL*/)
{
	if (!pData)
		return false;

	mMap.insert(CollectionDataMap::value_type(pData->dataBlockId, pData));
	return true;
}

U32 CollectionRepository::GetCollectionIdByIndex(S32 index)
{
	AssertRelease( (index >= 0) &&(index < mMap.size()) , "CollectionRepository.dat::invalid index");
	CollectionDataMap::const_iterator it = mMap.begin();
	int temp = 0;
	for(;it != mMap.end(); ++ it, ++ temp )
	{
		if(temp == index)
		{
			return it->first;
		}
	}
	return 0;
}

S32 CollectionRepository::GetDataCount()
{
	return (S32)mMap.size();
}

void CollectionRepository::Clear()
{
	CollectionDataMap::iterator it = mMap.begin();

	while(it != mMap.end())
	{
		if (it->second)
		{
			delete it->second;
		}

		it++;
	}

	mMap.clear();
}

void CollectionObjectData::FillTaskIds(StringTableEntry sId)
{
	if (!sId || !sId[0])
		return;

	// Ids format:		"101,102,103,104"
	//sIds = "101:102:103:104";		//for test
	char intBuf[16];
	StringTableEntry pStart = sId;
	StringTableEntry pEnd = sId;
	while(*pEnd != 0)
	{
		while(*pEnd != 0 && *pEnd != ',' && *pEnd != ' ')
		{
			AssertFatal((*pEnd >= '0' || *pEnd <= '9'), "CollectionObjectData::FillTaskIds: invald Task id string");
			pEnd++;
		}
		dMemcpy(intBuf, pStart, pEnd - pStart);
		intBuf[pEnd - pStart] = 0;
		mArrTaskIds.push_back(atoi(intBuf));
		if (*pEnd != 0)
		{
			pEnd++;
			pStart = pEnd;
		}
	}
}

void CollectionRepository::AssertErrorMsg(bool bAssert, const char *msg, int rowNum)
{
	char buf[256];

	if (!bAssert)
	{
		dSprintf(buf, sizeof(buf), "RowNum: %d  ", rowNum);
		dStrcat(buf, sizeof(buf), msg);
		AssertRelease(false, buf);
	}
}

#ifdef NTJ_SERVER
ConsoleFunction(CreateCollectionObject, bool, 5, 5, "")
{
	//CreateCollectionObject("55000000", "-234.793 -192.556 106.092", "0 0 1 224.21", "1 1 1");
	//position = "-236.793 -190.556 106.092";
	//dataBlockId = "55000000";

	if (argc < 5)
		return false;

	U32 nDataBlockId = atoi(argv[1]);
	Point3F position;
	AngAxisF aa(Point3F(0,0,0),0);
	Point3F scale;

	dSscanf(argv[2], "%f %f %f", &position.x, &position.y, &position.z);
	dSscanf(argv[3], "%f %f %f %f", &aa.axis.x, &aa.axis.y, &aa.axis.z, &aa.angle);
	aa.angle = mDegToRad(aa.angle);
	dSscanf(argv[4], "%f %f %f", &scale.x, &scale.y, &scale.z);

	CollectionObject *pCollectionObject = new CollectionObject;
	pCollectionObject->setDataID(nDataBlockId);

	MatrixF temp;
	aa.setMatrix(&temp);

	pCollectionObject->setTransform(temp);
	pCollectionObject->setPosition(position);
	pCollectionObject->setScale(scale);
	pCollectionObject->registerObject();

	return true;
}
#endif

#ifdef NTJ_CLIENT
ConsoleFunction(SetCollectionTarget, void, 3, 3, "SetCollectionTarget(player, collectionObj)")
{
	if (argc != 3)
		return;
	Player *pPlayer = dynamic_cast<Player *>( Sim::findObject(argv[1]) );
	CollectionObject *pCollectionObj = dynamic_cast<CollectionObject *>( Sim::findObject(argv[2]) );
	if (pPlayer)
		pPlayer->setCollectionTarget(pCollectionObj);
}
#endif