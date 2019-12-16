//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "T3D/gameConnection.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "base/bitStream.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/Mission/Conversation.h"
#include "Gameplay/Mission/PlayerMission.h"
#include "Gameplay/Item/DropRule.h"
#include "Effects/EffectPacket.h"
#include "Gameplay/Item/Player_EquipIdentify.h"
#include "Gameplay/GameObjects/LivingSkill.h"
#include "gameplay/Item/Player_Bank.h"
#include "Gameplay/Item/Prescription.h"
#include "Gameplay/GameObjects/CollectionObject.h" 
#include "Gameplay/item/res.h"
#include "Gameplay/item/PrizeBox.h"
#include "T3D/trigger.h"
#include "Gameplay/GameObjects/MPManager.h"
#include "Gameplay/GameObjects/PetOperationManager.h"
#include "Gameplay/GameObjects/SpiritOperationManager.h"

#ifdef NTJ_EDITOR
#include <windows.h>
#include "UI/EditorGuiGamePlay.h"
#endif

#ifdef NTJ_CLIENT
#include "gui/core/guiDefaultControlRender.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/ai/PlayerAISimple.h"
#include "Gameplay/ClientPlayerManager.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "gui/core/guiTypes.h"
#include "UI/dGuiMouseGamePlay.h"
#include "Gameplay/Team/ClientTeam.h"
#include "Gameplay/Item/AutoNpcSell.h"
#include "Gameplay/GlobalEvent/ClientGlobalEvent.h"
#endif
 
#ifdef NTJ_SERVER
#include "Gameplay/ServerGameplayState.h"
#include "Common/PlayerStruct.h"
#include "NetWork/ServerPacketProcess.h"
#include "Common/DataAgentHelper.h"
#include "Gameplay/Team/ZoneTeamManager.h"
#include "Gameplay/item/DropItem.h"
#include "Gameplay/GameObjects/ScheduleManager.h"
#include <list>
#endif

#include "Gameplay/GameObjects/TimerTrigger.h"

#include "../MaskEx.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "../Copymap/ZoneCopymap.h"

IMPLEMENT_CO_NETOBJECT_V1(Player);

#ifdef NTJ_SERVER
U16 Player::mRecoverVigorVelocity = 1;

#endif//NTJ_SERVER

Player::Player()
{
	mGameObjectMask |= PlayerObjectType;
	mPlayerId	= 0;

	mBody = 1000;
	mFace = 10012;
	mHair = 10011;
	mHairCol = 0;

	mLastSaveTime = 0;
	mIsSaveDirty = false;

	mMoney = mBankMoney = mBindMoney = 0;
	mInteractionState = INTERACTION_NONE;
	mOtherFlag = 0;

	missionInfo.InitData();

	mSwitchState = 0;
	m_Combative = true;
	mShowTriggerID	= 0;		//����ʾ������ID
	mLivingSkillStudyID = 0;

	mTradeLockFlag = 0;
	mBankLock	= true;
	mVigor    = 0;
	mMaxVigor = 0;
	mLivingLevel = 0;
	mCurrentLivingExp = 0;

	mTimerTriggerMgr = new TimerTriggerManager;
	mTimerTriggerMgr->SetPlayer(this);
	mTimerPetStudyMgr = new TimerTriggerManager;
	mTimerPetStudyMgr->SetPlayer(this);

	petList.Initialize();
	mountPetList.Initialize();
	mTalentTable.init(this);

	mCumulateAttack = 0;
	mCumulateDamage = 0;
	m_Influence = Influence_None;

#ifndef NTJ_EDITOR
	pLivingSkill    = new LivingSkillTable;
	pPrescription   = new Prescription;
	pMPManagerInfo  = new MPManager;
#endif

#ifdef NTJ_SERVER
	mIsInitializeEvent = false;
	mEventInitialized = false;
	mUID			= 0;
	mTransporting = false;		//���ͱ�־
	mTriggerId		= 0;			//��������ID
	mSecondPassword = NULL;
	pScheduleEvent = new ScheduleEvent;
	mSocialCount = 0;
	mIsRecoverVigor = true;
	mLastLotRequestTime = 0;
	memset( mSocialItems, 0, sizeof( stSocialItem ) * SOCAIL_ITEM_MAX );
	mTimeItemVec.clear();
#endif

#ifdef NTJ_CLIENT
	mPrizeBox = NULL;
	mLivingSkillList = new LivingSkillList;
	mPrescriptionList = new PrescriptionList;
	mSpiritList.Initialize();

	texRect.point.set(0,0);
	texRect.extent.set(254,64);
#endif
}

Player::~Player()
{
#ifdef NTJ_SERVER
	if( g_ServerGameplayState->GetPlayer( this->mPlayerId ) != NULL )
		g_ServerGameplayState->DeleteFromPlayerManager(this);
	SAFE_DELETE(pScheduleEvent);
	for(int i=0; i<mTimeItemVec.size(); ++i)
	{
		if(mTimeItemVec[i])
			SAFE_DELETE(mTimeItemVec[i]);
	}
	mTimeItemVec.clear();
#endif

#ifndef NTJ_EDITOR
	SAFE_DELETE(m_pAI);
	SAFE_DELETE(pLivingSkill);
	SAFE_DELETE(pPrescription);
	SAFE_DELETE(pMPManagerInfo);
#endif 

#ifdef NTJ_CLIENT
	SAFE_DELETE(mLivingSkillList);
	SAFE_DELETE(mPrescriptionList);
	missionItemList.Clear();
#endif
	SAFE_DELETE(mTimerPetStudyMgr);
	SAFE_DELETE(mTimerTriggerMgr);
	missionShareList.clear();
}

bool Player::onAdd()
{
	// ֱ��ȡ��PlayerData
	if(!GameBase::mDataBlock)
		GameBase::mDataBlock = dynamic_cast<GameBaseData*>(g_PlayerRepository.GetPlayerData(mBody));

	if(!Parent::onAdd())
		return false;
	mDataBlockId = mBody;

	// to do: ��ʾװ����ģ��
#ifdef NTJ_SERVER
	mShapeStack.add(ShapeStack::Stack_Base, dAtoi(Con::executef("getCreateFace",Con::getIntArg(m_Sex),Con::getIntArg(mFace))));
	mShapeStack.add(ShapeStack::Stack_Base, dAtoi(Con::executef("getCreateHair",Con::getIntArg(m_Sex),Con::getIntArg(mHair))));
	updateFashionFlags();
	g_ItemManager->mountAllEquipModel(this);

	if( getLayerID() != 1 )
	{
		ZONE_COPYMAP->onPlayerEnter( this );
	}
#endif

#ifdef NTJ_EDITOR
	gEditorGuiGamePlay->setControlPlayer(this);
#endif

	return true;
}

void Player::onRemove()
{
#ifdef NTJ_EDITOR
	gEditorGuiGamePlay->setControlPlayer(NULL);
#endif

#ifdef NTJ_SERVER
	if( getLayerID() != 1 )
	{
		ZONE_COPYMAP->onPlayerLeave( this );
	}
#endif

	Parent::onRemove();
}

bool Player::isSelfPlayer()
{
#ifndef NTJ_SERVER
	if(getControllingClient())
		return true;
#endif
	return false;
}

bool Player::onNewDataBlock(GameBaseData* dptr)
{
	mDataBlock = dynamic_cast<PlayerData*>(dptr);
	if (!mDataBlock || !Parent::onNewDataBlock(mDataBlock))
		return false;

	return true;
}

bool Player::onChangeDataBlock(GameBaseData* dptr)
{
	mDataBlock = dynamic_cast<PlayerData*>(dptr);
	if(!mDataBlock || !Parent::onChangeDataBlock(dptr))
		AssertRelease(false, "Player::onChangeDataBlock");

	return true;
}

void Player::onEnabled()
{
	Parent::onEnabled();

#ifdef NTJ_CLIENT
	if(m_pAI)
		m_pAI->SetNextAIState(PlayerAISimple::PLAYER_AI_IDLE);
#endif
#ifdef NTJ_SERVER
	// ��ɫ�����¼�
	Con::executef("SvrEventOnEnabled", Con::getIntArg(getId()));
#endif
}

void Player::onDisabled()
{
	Parent::onDisabled();

#ifdef NTJ_CLIENT
	if(this == g_ClientGameplayState->GetControlObject())	// ���ַ����������©�����������״̬������Ϸ���ܲ���ִ��
	{
		// ������ʾ����
		Con::executef("OpenResurgenceWnd");
	}
	if(m_pAI)
		m_pAI->SetNextAIState(PlayerAISimple::PLAYER_AI_DEATH);
#endif
#ifdef NTJ_SERVER
	//�ջ����г���
	mPetTable.disbandAllPet(this);

	reduceAllWear();
	// ��ɫ�����¼�
	Con::executef("SvrEventOnDisabled", Con::getIntArg(getId()));
#endif
}

void Player::setArmStatus(GameObjectData::ArmStatus arm)
{
	if(arm != m_ArmStatus)
	{
		mTalentTable.onUnarm(m_ArmStatus);
		Parent::setArmStatus(arm);
		mTalentTable.onArm(m_ArmStatus);
	}
}

// -------------------------------------------------------------------------
void Player::initPersistFields()
{
	Parent::initPersistFields();
#ifdef NTJ_SERVER
	Con::addVariable("Player::RecoverVigorVelocity", TypeS32, &Player::mRecoverVigorVelocity);
#endif
}

bool Player::setCollectionTarget(CollectionObject *pCollectionObj)
{
	if (!pCollectionObj)
		return false;

	Box3F box = pCollectionObj->getWorldBox();
	F32 len = ( pCollectionObj->getPosition() - pCollectionObj->getPosition()).len();
	if (len > (1.0f + (box.max - box.min).len() / 2))
		return false;

	setInteraction(pCollectionObj, INTERACTION_PICKUP);

	enWarnMessage msg = pCollectionObj->potentialEnterTrigger(this);
	if (msg != MSG_NONE)
	{
#ifdef NTJ_SERVER
		MessageEvent::send(getControllingClient(), SHOWTYPE_ERROR,msg);
#endif
#ifdef NTJ_CLIENT
		MessageEvent::show(SHOWTYPE_ERROR,msg);
#endif
		return false;
	}

	return true;
}

bool Player::CanAttackTarget(GameObject* obj)
{
	if(!Parent::CanAttackTarget(obj))
		return false;

	// �����NPC֮��������ж�
	if(obj->getGameObjectMask() & NpcObjectType)
	{
		//NpcObject* npc = (NpcObject*)obj;
		//AssertFatal(npc->getInfluence() != Influence_None, "npc Influence error!")
		//if(mFame[npc->getInfluence()].fame > Fame_Neutrality)
		//	return false;
	}
	// ���֮��Ķ����ж�
	else if(obj->getGameObjectMask() & PlayerObjectType)
	{
		Player* player = (Player*)obj;
#ifdef NTJ_SERVER
		CZoneTeam* pTeam = ZONE_TEAM_MGR->GetTeamByPlayerId( getPlayerID() );
		if( pTeam )
		{
			if( pTeam->IsTeammate( player->getPlayerID() ) )
				return false;
		}
#endif
#ifdef NTJ_CLIENT
		if( CLIENT_TEAM->IsTeammate( player->getPlayerID() ) )
			return false;
#endif
	}
	return true;
}

bool Player::isPKArea()
{
	//TriggerData* pTri = g_TriggerDataManager->getTriggerData(mTriggerId);

	//if(pTri && pTri->getBaseLimit(TriggerData::Limit_PK))
	//	return true;
	return false;
}

void Player::setUpdateTeam()
{
	setMaskBits( TeamMask );
}

#ifndef NTJ_EDITOR
void Player::addHate(GameObject* pObject, S32 nHate)
{
	//m_pAI->getHateList().AddHate(pObject, nHate);
}
#endif


void Player::addLevel(S32 level)
{
	U32 nOld = m_Level;
	m_Level = mClamp(m_Level + level, 1, EnablePlayerLevel); 

#ifdef NTJ_SERVER
	//�Զ���ʦ
	if (getMPInfo() && m_Level == 50)
		getMPInfo()->autoLeaveMaster(this);
#endif

	if (m_Level != nOld)
	{
		m_BuffTable.RemoveBuff(Macro_GetBuffId(g_FamilyBuff[m_Family], nOld));
		m_BuffTable.AddBuff(Buff::Origin_Base, Macro_GetBuffId(g_FamilyBuff[m_Family], m_Level), this);
#ifdef NTJ_SERVER
		// ������Ч [7/23/2009 joy]
		static MatrixF mat;
		mat.identity();
		mat.setPosition(getPosition());
		g_EffectPacketContainer.addEffectPacket(EP_LevelUp, this, mat, NULL, getPosition());

		// ���10���̶�����6�����вֿ��λ
		if(m_Level == 10)
		{
			bankList.SetMaxSlot(bankList.GetMaxSlots() + 6);
			bankList.UpdateMaxSlots(getControllingClient());
		}

		// ǿ�Ƽ���
		CalcStats();
		setHP(0xFFFFFF);
		setMP(0xFFFFFF);
		setPP(0xFFFFFF);
		setMaskBits(GameObject::BaseInfoMask);

		// ��ɫ�����¼�
		Con::executef("SvrEventLevelUp", Con::getIntArg(getId()));
#endif
	}
}

void Player::addExp(S32 exp)
{
	m_Exp = mClamp(m_Exp + exp, 0, g_LevelExp[m_Level]*3);
#ifdef NTJ_SERVER
	sendPlayerEvent(EventExperienceMask);	
#endif
}

void Player::onDamage(S32 dam,GameObject* pSource)
{
	if(!pSource)
		return;
	Parent::onDamage(dam,pSource);

#ifdef NTJ_SERVER
	
	if (dam>0 && pScheduleEvent)
		pScheduleEvent->cancelEvent(this);
#endif
}

bool Player::setShapeShifting(bool val)
{
	if(!Parent::setShapeShifting(val))
		return false;

#ifdef NTJ_SERVER
	if(getShapeShifting())
	{
		// ������ʱ���Ǳ�ɽ��̹���
		setArmStatus(GameObjectData::Arm_A);
		setEdgeBlurTexture(NULL);
	}
	else
	{
		if(equipList.GetSlot(Res::EQUIPPLACEFLAG_WEAPON) && equipList.GetSlot(Res::EQUIPPLACEFLAG_WEAPON)->isItemObject())
		{
			ItemShortcut* pItem = (ItemShortcut*)equipList.GetSlot(Res::EQUIPPLACEFLAG_WEAPON);
			setArmStatus((GameObjectData::ArmStatus)(GETSUB(pItem->getRes()->getSubCategory())));
			setEdgeBlurTexture(pItem->getRes()->getTrailEffectTexture());
		}
		else
		{
			setArmStatus(GameObjectData::Arm_A);
			setEdgeBlurTexture(NULL);
		}
	}
#endif
	return true;
}

void Player::addFavour(U32 infl, S32 favour)
{
	if(infl >= MaxInfluence)
		return;

	// ���fame
	if(mFame[infl].fame > Fame_Adoration || mFame[infl].fame < Fame_Enmity)
	{
		AssertFatal(false, "overflow : fame !");
		mFame[infl].fame = mClamp(mFame[infl].fame, Fame_Enmity, Fame_Adoration);
	}

	mFame[infl].favour += favour;
	// ����
	while(mFame[infl].favour >= g_FameMax[mFame[infl].fame])
	{
		if(mFame[infl].fame >= Fame_Adoration)
		{
			mFame[infl].favour = g_FameMax[mFame[infl].fame];
			break;
		}
		mFame[infl].favour -= g_FameMax[mFame[infl].fame];
		++mFame[infl].fame;
	}
	// ����
	while(mFame[infl].favour < 0)
	{
		if(mFame[infl].fame <= Fame_Enmity)
		{
			mFame[infl].favour = 0;
			break;
		}
		mFame[infl].favour += g_FameMax[mFame[infl].fame];
		--mFame[infl].fame;
	}
	// ������Ϣ���ͻ���
#ifdef NTJ_SERVER
	if(getControllingClient())
	{
		ServerGameNetEvent* pEvent = new ServerGameNetEvent(INFO_FAME);
		pEvent->SetIntArgValues(2, infl, mFame[infl].fame);
		pEvent->SetInt32ArgValues(1, mFame[infl].favour);
		getControllingClient()->postNetEvent(pEvent);
	}
#endif

	// to do : �����͵ж������ĺøжȹ���
	//for (S32 i=0; i<TotalInfluence; ++i)
	//{
	//	if(g_FameRelationship[infl][i] == Fame_Top)
	//		addFavour(i, favour);
	//	else if(g_FameRelationship[infl][i] == Fame_Bottom)
	//		addFavour(i, -favour);
	//}
}

S32 Player::getFavour(U32 infl)
{
	if(infl >= MaxInfluence)
		return 0;
	return mFame[infl].favour;
}

void Player::setFame(U32 infl, U32 fame, S32 favour /* = 0 */)
{
	if(infl >= MaxInfluence)
		return;

	mFame[infl].fame = mClamp(fame, Fame_Enmity, Fame_Adoration);
	mFame[infl].favour = mClamp(favour, 0, g_FameMax[fame]);
#ifdef NTJ_SERVER
	if(getControllingClient())
	{
		ServerGameNetEvent* pEvent = new ServerGameNetEvent(INFO_FAME);
		pEvent->SetIntArgValues(2, infl, mFame[infl].fame);
		pEvent->SetInt32ArgValues(1, mFame[infl].favour);
		getControllingClient()->postNetEvent(pEvent);
	}
#endif
}

S32 Player::getFame(U32 infl)
{
	if(infl >= MaxInfluence)
		return Fame_Bottom;
	return mFame[infl].fame;
}

void Player::processTick(const Move *move)
{
	mTimerTriggerMgr->ProcessTick();
	mTimerPetStudyMgr->ProcessTick();

	mPetTable.ProcessTick(this);

	

#ifdef NTJ_CLIENT
	if(m_pAI)
		m_pAI->Update();
	if(getInteractionState() == Player::INTERACTION_NPCTRADE)
		this->npcShopList.UpdateData();

	Vector< SimObjectPtr<CollectionObject> >::iterator it = mCollectionList.begin();
	while (it != mCollectionList.end())
	{
		if ( (*it).isNull() )
			mCollectionList.erase(it);
		else
			++it;
	}
	g_CClientGlobalEvent.queryCallboardMessage(this);
#endif

	Parent::processTick(move);
#ifdef NTJ_SERVER
	// ���������������ҹ�Զ������Ҫ���
	if(!canInteraction(getInteraction()))
		setInteraction(NULL, INTERACTION_NONE);

	if((pScheduleEvent && !getVelocity().isZero()) || isDisabled())
		pScheduleEvent->cancelEvent(this);

	recoverVigor();
	updateTimeItem();
	if( ::GetTickCount() - mLastSaveTime > 1000 || mIsSaveDirty )
	{
		// ÿһ����������һ��
		try
		{
			saveToWorld();
		}
		catch(...)
		{

		}
	
		mLastSaveTime = ::GetTickCount();
	}

	//// ÿ30���Ӽ���һ���Ŷ�
	//if( ::GetTickCount() - mLastLotRequestTime > 30 * 60 * 1000 )
	//{
	//	Con::executef( "AddLotRequest", Con::getIntArg( mPlayerId ) );

	//	mLastLotRequestTime = ::GetTickCount();
	//}

#endif
	//��鳬ʱ���߼��ӳ��¼�
	checkOverTimeLogicEvent();
}

void Player::writePacketData(GameConnection *connection, BitStream *stream)
{
	Parent::writePacketData(connection, stream);
#ifdef NTJ_SERVER
	if(!mEventInitialized)
	{
		//��mTeamId ������ϢȥWorldServer���߱��ز�ѯ�������Ȼ���͵��ͻ���ȥ
		mEventInitialized = true;
	}
#endif
}

U64 Player::packUpdate(NetConnection *conn, U64 mask, BitStream *stream)
{
	U64 retMask = Parent::packUpdate(conn, mask, stream);
	bool bSelf = (getControllingClient() == conn);

	// ��ʼ��
	if(stream->writeFlag(mask & InitialUpdateMask))
	{
		stream->writeSignedInt(mPlayerId, Base::Bit32);
		stream->writeInt(m_Sex, 8);
		stream->writeInt(mBody, 16);
		stream->writeInt(mFace, 16);
		stream->writeInt(mHair, 16);
		stream->writeInt(mHairCol, 16);
		stream->writeInt(mLastLoginTime, 32);
#ifdef NTJ_SERVER
		stream->writeInt(_time32(NULL),32);
#endif
	}

	if( stream->writeFlag( mask & TeamMask ) )
	{
		mTeamInfo.WritePacket( stream );
	}

	if (stream->writeFlag( mask & SwitchMask))
	{
		stream->writeInt(mSwitchState, 8);
	}

	// ������ҵĳ�������
	if(stream->writeFlag(mask & PetMask))
	{
		mPetTable.packPetObject(conn, stream);
	}
	return retMask;
}

void Player::unpackUpdate(NetConnection *conn, BitStream *stream)
{
	Parent::unpackUpdate(conn,stream);

	// ��ʼ��
	if(stream->readFlag())
	{
		mPlayerId = stream->readSignedInt(Base::Bit32);
		m_Sex = (enSex)(stream->readInt(8));
		mBody = stream->readInt(16);
		mFace = stream->readInt(16);
		mHair = stream->readInt(16);
		mHairCol = stream->readInt(16);
		mLastLoginTime = stream->readInt( 32 );

#ifdef NTJ_CLIENT
		if(mPlayerId == this->getPlayerID())
		{
			ClientGameplayState::mServerInitTime = stream->readInt(32);
			ClientGameplayState::mClinetUpdateTime = Platform::getVirtualMilliseconds();
		}
		PLAYER_MGR->AddLocalPlayer( mPlayerId, this );
#endif
	}

	// �������
	if( stream->readFlag())
	{
		mTeamInfo.ReadPacket( stream );
		setDirty(true);
	}

	if (stream->readFlag())
	{
		mSwitchState = stream->readInt(8);
		setDirty(true);
	}

	if (stream->readFlag())
	{
		mPetTable.unpackPetObject(conn, stream);
	}
}

// ----------------------------------------------------------------------------
// ���ÿ���״̬
void Player::setSwitchState(U32 flag, bool isOpen)
{
	if(isOpen)
		mSwitchState |= flag;
	else
		mSwitchState &= ~flag;	
	setMaskBits(Player::SwitchMask);
}

// ----------------------------------------------------------------------------
// ������ҽ��������״̬
// ע:���ڱ�������������ֹ����������:
// ServerCall----->(setInteraction)----->ServerBreak
//		   ��				 |
//(Send Net|Message)  (Player|BaseProEvent)
//         |				 ��
// ClientCall      (setInteraction)----->ClientBreak
bool Player::setInteraction(SceneObject* obj, U32 state)
{
	if(obj == mInteractionObject && state == mInteractionState)
		return false;

	breakInteraction();	// ��ֹ��ǰ������Ϊ

	mInteractionObject = obj;
	mInteractionState = state;
#ifdef NTJ_SERVER
	sendPlayerEvent(EventInteractionMask);
#endif
	return true;
}

// ----------------------------------------------------------------------------
// ��ֹ��ǰ������Ϊ
bool Player::breakInteraction()
{
	if(mInteractionObject.isNull() || mInteractionState == INTERACTION_NONE)
		return false;
	switch(mInteractionState)
	{
	case INTERACTION_PLAYERTRADE:		// ��Ҽ佻��
		{
			// �˳���Ҽ佻��
			g_ItemManager->closePlayerTrade(this);
			Player* pTarget = dynamic_cast<Player*>(mInteractionObject.getObject());
			mInteractionObject = NULL;
			if(pTarget)
			{
#ifdef NTJ_SERVER
				pTarget->setInteraction(NULL, INTERACTION_NONE);
#endif//NTJ_SERVER
#ifdef NTJ_CLIENT
				pTarget->tradeList.Reset();
				pTarget->tradeList.SetTradeState(TRADE_NONE);
#endif//NTJ_CLIENT
			}
		}
		break;
	case INTERACTION_NPCTALK:			// NPC�Ի�״̬
		{
			//���������ֹ������ֻ��Ҫ�����������Ϳ�
#ifdef NTJ_CLIENT
			// �˳�NPC�Ի�
			Conversation::setCurrentConversation(NULL);
			Con::executef("CloseNpcDialog");
#endif//NTJ_CLIENT
		}
		break;
	case INTERACTION_NPCTRADE:			// NPC�̵꽻��
		{
			// �ر�NPCϵͳ�̵�
			g_ItemManager->closeNpcShop(this);
		}
		break;
	case INTERACTION_PICKUP:			// ʰȡ��Ʒ״̬
		{

		}
		break;
	case INTERACTION_IDENTIFY:
		{
			gIdentifyManager->closeIdentify(this);
		}
		break;
	case INTERACTION_STRENGTHEN:
		{
			EquipStrengthen::closeStrengthen(this);
		}
		break;
	case INTERACTION_PUNCHHOLE:
		{
			EquipPunchHole::closePunchHole(this);
		}
		break;
	case INTERACTION_PETIDENTIFY:
		{
			PetOperationManager::ClosePetIdentify(this);
		}
		break;
	case INTERACTION_PETINSIGHT:
		{
			PetOperationManager::ClosePetInsight(this);
		}
		break;
	case INTERACTION_PETLIANHUA:
		{
			PetOperationManager::ClosePetLianHua(this);
		}
		break;
	case INTERACTION_PETHUANTONG:
		{
			PetOperationManager::ClosePetHuanTong(this);
		}
		break;
	case INTERACTION_SPIRITSKILL:
		{
			SpiritOperationManager::CloseSpiritSkillWnd(this);
		}
		break;
	case INTERACTION_LEARNSKILL:       // ����ѧϰ
		{
#ifdef NTJ_CLIENT
			Con::executef("CloseSkillStudy");
#endif //NTJ_CLIENT			
		}
		break;
	case INTERACTION_SELECTSKILL:  //����ϵѡ��
		{
#ifdef NTJ_CLIENT
			Con::executef("CloseSkillSelect");
#endif //NTJ_CLIENT
		}
		break;
	case INTERACTION_STUDYLIVESKILL:
		{
#ifdef NTJ_CLIENT
			Con::executef("CloseStudyLivingSkillWnd");
#endif //NTJ_CLIENT	
		}
		break;
	case INTERACTION_BANKTRADE:
		{
			g_PlayerBank->closeBank(this);
		}
		break;
	case INTERACTION_STALLBUY:
		{
#ifdef NTJ_CLIENT
			Con::executef("CloseStallSeeWnd");
#endif
		}
		break;
	case INTERACTION_REPAIR:
		{
			g_ItemManager->closeRepair(this);
		}
		break;
	}

	return true;
}

// ----------------------------------------------------------------------------
// ����һ���߼��ӳ��¼�
U32 Player::insertLogicEvent(stLogicEvent* logicEvent)
{
	static U32 id = 0;
	logicEvent->start = Platform::getVirtualMilliseconds();
	logicEvent->id = id ++;
	mLogicEventQueue.push_back(logicEvent);
	return logicEvent->id;
}

// ----------------------------------------------------------------------------
// �������߼��ӳ��¼�
void Player::checkOverTimeLogicEvent()
{
	static U32 lastCheckTime = Platform::getVirtualMilliseconds();
	U32 current = Platform::getVirtualMilliseconds();
	if(current - lastCheckTime < 20000) // ÿ��20����һ��
		return;
	bool bRefreshSys = false;		// �Ƿ�ˢ��ϵͳ�߼��ӳ��¼�
	bool bRefreshIndividual = false;// �Ƿ�ˢ�¸����߼��ӳ��¼�
	for(S32 i = 0; i < mLogicEventQueue.size(); ++i)
	{
		stLogicEvent* ev = mLogicEventQueue[i];
		if(ev->start + MAX_LOGIC_REPLYTIME <= current)
		{
			//ע��ϵͳ�߼��ӳ��¼��Ǵ�10001��ʼ��ŵ�
			if(ev->type > 10000)
				bRefreshSys = true;
			else
				bRefreshIndividual = true;
			if(ev->data)
				delete[] ev->data;
			delete ev;
			mLogicEventQueue.erase_fast(i);
			i--;
			
		}
	}
	if(bRefreshSys)
		Con::executef("RefreshLogicEvent", Con::getIntArg(1));
	if(bRefreshIndividual)
		Con::executef("RefreshLogicEvent", Con::getIntArg(0));
	lastCheckTime = current;
}

// ----------------------------------------------------------------------------
// �Ƴ�һ���߼��ӳ��¼�
void Player::removeLogicEvent(U32 id)
{
	for(int i = 0; i < mLogicEventQueue.size(); ++i)
	{
		stLogicEvent* ev = mLogicEventQueue[i];
		if(ev->id == id)
		{
			if(ev->data)
				delete[] ev->data;
			delete ev;
			mLogicEventQueue.erase_fast(i);
			return;
		}
	}
}

// ----------------------------------------------------------------------------
// ����������ȡ�߼��ӳ��¼�
Player::stLogicEvent* Player::getLogicEvent(S32 index)
{
	if(index < 0 || index >= mLogicEventQueue.size())
		return NULL;
	return mLogicEventQueue[index];
}

// ----------------------------------------------------------------------------
// ����һ���߼��ӳ��¼�
Player::stLogicEvent* Player::findLogicEvent(U32 id)
{
	for(int i = 0; i < mLogicEventQueue.size(); ++i)
	{
		if(mLogicEventQueue[i]->id == id)
			return mLogicEventQueue[i];
	}
	return NULL;
}

// ----------------------------------------------------------------------------
// �жϵ�ǰ����Ƿ���æ���޷�����������
enWarnMessage Player::isBusy(enInteractionState state/*=INTERACTION_NONE*/)
{
	// �ж�����Ƿ�������
	if(isDisabled())
		return MSG_PLAYER_ISDEAD;
#ifdef NTJ_SERVER
	// �ж�����Ƿ����ڴ�����
	if(isTransporting())
		return MSG_PLAYER_TRANSFERING;
#endif
	// �ж�����Ƿ�����ʹ�ü���
	if(GetSpell().IsSpellRunning())
		return MSG_PLAYER_SPELLRUNNING;
	// �ж������״̬BUFF����ֹ��������Ϊ
	//if(GetBuffTable()->
		//return MSG_PLAYER_BUFFDISABLED;
	// ����ǰ���н���״̬�����齻��״̬�Ƿ���ͬ
	if(getInteractionState() != INTERACTION_NONE && getInteractionState() != state)
		return MSG_PLAYER_BUSY;
	return MSG_NONE;
}

// ----------------------------------------------------------------------------
// ���ӽ�ɫ��Ǯ/�󶨽�Ǯ/���н�Ǯ
bool Player::addMoney(S32 money, U32 type /*= Currentcy_Money */)
{
	if(money <= 0)
		return false;

	if(type == Currentcy_Money)				//��Ǯ
	{
		S32 temp = mMoney + money;
		if(temp <0 || temp >= Player::MAX_MONEY_NUM)
			return false;

		mMoney = temp;
#ifdef NTJ_SERVER
		sendPlayerEvent(EventMoneyMask);	
#endif
		return true;
	}
	else if(type == Currentcy_BindMoney)	//�󶨽�Ǯ
	{
		S32 temp = mBindMoney + money;
		if(temp <0 || temp >= Player::MAX_MONEY_NUM)
			return false;

		mBindMoney = temp;
#ifdef NTJ_SERVER
		sendPlayerEvent(EventBindMoneyMask);	
#endif
		return true;
	}
	else if(type == Currentcy_BankMoney)	//���н�Ǯ
	{
		if(mBankLock)
			return false;
		S32 temp = mBankMoney + money;
		if(temp <0 || temp >= Player::MAX_MONEY_NUM)
			return false;

		mBankMoney = temp;
#ifdef NTJ_SERVER
		sendPlayerEvent(EventBankMoneyMask);	
#endif
		return true;
	}
	return false;
}

// ----------------------------------------------------------------------------
// �ж��Ƿ��ܿۼ�����
bool Player::canReduceMoney(S32 money, U32 type)
{
	if(money <= 0)
		return true;
	char szType[12];
	dSprintf(szType,12, "%d", type);
	U32 ulen = dStrlen(szType);
	S32 haveMoney = -money;
	for(S32 i = 0; i < ulen; i++)
	{
		U32 num = szType[i] - '0';
		if(num == Currentcy_Money)
			haveMoney += mMoney;
		else if(num == Currentcy_BindMoney)
			haveMoney += mBindMoney;
		else if(num == Currentcy_BankMoney)
			haveMoney += mBankMoney;
	}
	return (haveMoney >= 0);
}

// ----------------------------------------------------------------------------
// �ж��Ƿ�����Է����ཻ�׽�Ǯ
bool Player::canTradeMoney(Player* target, S32 givemoney, S32 gotmoney)
{
	if(!target || givemoney > MAX_TRADEMONEY_NUM || gotmoney > MAX_TRADEMONEY_NUM)
		return false;
	S32 money = getMoney(Currentcy_Money);
	if(money < givemoney)
		return false;
	money = money - givemoney + gotmoney;
	if(money < 0 || money > MAX_MONEY_NUM)
		return false;
	money = target->getMoney(Currentcy_Money);
	if(money < gotmoney)
		return false;
	money = money - gotmoney + givemoney;
	if(money < 0 || money > MAX_MONEY_NUM)
		return false;
	return true;
}

// ----------------------------------------------------------------------------
// ���ٽ�ɫ��Ǯ/�󶨽�Ǯ/���н�Ǯ
bool Player::reduceMoney(S32 money, U32 type )
{
	if(money <= 0)
		return false;
	char szType[12];
	dSprintf(szType,12, "%d", type);
	U32 ulen = dStrlen(szType);
	bool IsFailed = true;
	S32 lefts = money;
	for(U32 i = 0; i < ulen; i++)
	{
		U32 num = szType[i] - '0';
		if(num == Currentcy_Money)				//��Ǯ
		{
			if(lefts <= mMoney)
			{
				IsFailed = false;
				break;
			}
			else
			{
				lefts -= mMoney;
			}
		}
		else if(num == Currentcy_BindMoney)		//�󶨽�Ǯ
		{
			if(lefts <= mBindMoney)
			{
				IsFailed = false;
				break;
			}
			else
			{
				lefts -= mBindMoney;
			}
		}
		else if(num == Currentcy_BankMoney)		//���н�Ǯ
		{
			if(!mBankLock)
			{
				if(lefts <= mBankMoney)
				{
					IsFailed = false;
					break;
				}
				else
				{
					lefts -= mBankMoney;
				}
			}			
		}
	}

	if(IsFailed)
		return false;
	lefts = money;
	for(U32 i = 0; i < ulen; i++)
	{
		U32 num = szType[i] - '0';
		if(num == Currentcy_Money)
		{
			if(lefts <= mMoney)
			{
				mMoney -= lefts;
#ifdef NTJ_SERVER
				sendPlayerEvent(EventMoneyMask);	
#endif
				return true;
			}
			else
			{
				lefts -= mMoney;
				mMoney = 0;
#ifdef NTJ_SERVER
				sendPlayerEvent(EventMoneyMask);	
#endif
			}
		}
		else if(num == Currentcy_BindMoney)
		{
			if(lefts <= mBindMoney)
			{
				mBindMoney -= lefts;
#ifdef NTJ_SERVER
				sendPlayerEvent(EventBindMoneyMask);
#endif
				return true;
			}
			else
			{
				lefts -= mBindMoney;
				mBindMoney = 0;
#ifdef NTJ_SERVER
				sendPlayerEvent(EventBindMoneyMask);
#endif
			}
		}
		else if(num == Currentcy_BankMoney)
		{
			if(!mBankLock)
			{
				if(lefts <= mBankMoney)
				{
					mBankMoney -= lefts;
#ifdef NTJ_SERVER
					sendPlayerEvent(EventBankMoneyMask);
#endif
					return true;
				}
				else
				{
					lefts -= mBankMoney;
					mBankMoney = 0;
#ifdef NTJ_SERVER
					sendPlayerEvent(EventBankMoneyMask);
#endif
				}
			}			
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
// ��ȡ��ɫ��Ǯ/�󶨽�Ǯ/���н�Ǯ
S32 Player::getMoney(U32 type)
{
	switch(type)
	{
	case Currentcy_Money :		return mMoney;
	case Currentcy_BindMoney :	return mBindMoney;
	case Currentcy_BankMoney :	return mBankMoney;
	}
	return 0;
}


// ----------------------------------------------------------------------------
// ���ý�ɫ����
void Player::setPlayerName(StringTableEntry name)
{
	setShapeName(name);
	// ������"P_��ɫ����"�����������,����simObject::findObject
	char szName[COMMON_STRING_LENGTH + 2];
	dSprintf(szName, COMMON_STRING_LENGTH + 2, "P_%s", name);
	assignName(szName);
}

// ----------------------------------------------------------------------------
// ���ý�ɫ���ɫ����ֵ
void Player::setMaxVigor(S32 vigor)
{
	mMaxVigor = vigor;

#ifdef NTJ_SERVER
	sendPlayerEvent(EventVigorMask);
#endif
}
//-----------------------------------------------------------------------------
// ���ӽ�ɫ����ֵ
void Player::addVigor(S32 vigor)
{
	if(vigor > 0 && mVigor == mMaxVigor)
		return;

	S32 tempVigor = mVigor;
	mVigor = mClamp(mVigor+vigor,0,mMaxVigor);

	tempVigor = tempVigor -  mVigor;
	if(tempVigor >0)
		addLivingExp(tempVigor);

#ifdef NTJ_SERVER
	sendPlayerEvent(EventVigorMask);
#endif	
}
//-------------------------------------------------------------------------------
// ���������ֵ
void Player::addLivingExp(S32 exp)
{
	if(exp <= 0)
		return;

	S32 tempExp = mCurrentLivingExp;
	mCurrentLivingExp = mClamp(mCurrentLivingExp+exp,0,g_LivingLevelExp[mLivingLevel]*2);
	tempExp = mCurrentLivingExp - tempExp;

	if(mCurrentLivingExp >= g_LivingLevelExp[mLivingLevel])
	{
		mCurrentLivingExp -= g_LivingLevelExp[mLivingLevel];
		addLivingLevel(1);
	}
#ifdef NTJ_CLIENT
	MessageEvent::show(SHOWTYPE_ERROR,avar(GetLocalStr(MSG_PLAYER_ADDLIVINGEXP),tempExp),SHOWPOS_CHAT);
#endif
}
// ----------------------------------------------------------------------------
// ��������ȼ�
void Player::addLivingLevel(S32 level)
{
	S32 tempLevel = mLivingLevel;
	mLivingLevel = mClamp(mLivingLevel+level,0,EnablePlayerLevel);

#ifdef NTJ_CLIENT
	if((mLivingLevel - tempLevel) >0)
		MessageEvent::show(SHOWTYPE_ERROR,avar(GetLocalStr(MSG_PLAYER_ADDLIVINGLEVEL),mLivingLevel),SHOWPOS_CHAT);
#endif
}
//-------------------------------------------------------------------------------
//����ר������
void Player::setLivingSkillPro(U32 skillId)
{
#ifdef NTJ_SERVER
	if(!pLivingSkill)
		return;

	if(GETSUBCATEGORY(mLivingSkillStudyID) != GETSUBCATEGORY(skillId))
	{
		LivingSkillTable::LivingSkillTableMap& skillTab = pLivingSkill->getLivingSkillTable();
		LivingSkillTable::LivingSkillTableMap::iterator it = skillTab.find(GETSUBCATEGORY(mLivingSkillStudyID));
		if (it != skillTab.end())
		{
			LivingSkill* pSkill = it->second;
			if(!pSkill || !pSkill->getData()->getRipe())
				return;

			//ר�������Ƿ�Ϊ����
			LivingSkillData* pData = g_LivingSkillRespository.getLivingSkillData(mLivingSkillStudyID+1);
			if(pData || (pSkill->getRipe() != pSkill->getData()->getRipe()))
				return;
		}
	}
#endif
	mLivingSkillStudyID = skillId;

#ifdef NTJ_SERVER
	sendPlayerEvent(EventSkillMask);
#endif
}

U32 Player::getLivingSkillRipe(U32 skillId)
{
#ifndef NTJ_EDITOR
	if(!pLivingSkill)
		return 0;

	LivingSkillTable::LivingSkillTableMap& skillTab = pLivingSkill->getLivingSkillTable();
	LivingSkillTable::LivingSkillTableMap::iterator it = skillTab.find(GETSUBCATEGORY(skillId));
	if(it != skillTab.end())
	{
		LivingSkill* pSkill = it->second;
		if(!pSkill)
			return 0;
		else
			return pSkill->getRipe();
	}
#endif
	return 0;
}

void Player::setLivingSkillGuerdon(U32 skillId,U32 extRipe)
{
#ifndef NTJ_EDITOR
	GameConnection*	conn = getControllingClient();
	if (!conn)
		return;

	LivingSkillData* pData = g_LivingSkillRespository.getLivingSkillData(skillId);
	if(!pData)
		return;

	LivingSkillTable::LivingSkillTableMap& skillTab = pLivingSkill->getLivingSkillTable();
	LivingSkillTable::LivingSkillTableMap::iterator it = skillTab.find(GETSUBCATEGORY(skillId));
	if(it == skillTab.end())
		return;

	LivingSkill* pSkill = it->second;
	if(!pSkill)
		return;

	LivingSkillData::Guerdon& stGuerdon = pData->getGuerdon();
	if(stGuerdon.type == LivingSkillData::GUERDON_RIPE)
	{
		U32 iCurRipe = pSkill->getRipe();
		U32 tempRipe = iCurRipe;
		if(iCurRipe < pData->getRipe())
		{
			if(stGuerdon.num > 0)
			{
				iCurRipe += stGuerdon.num;
				if(iCurRipe > pData->getRipe())
				{
#ifdef NTJ_CLIENT
					U32 delta = pData->getRipe() - pSkill->getRipe();
					MessageEvent::show(SHOWTYPE_NOTIFY,avar(GetLocalStr(MSG_LIVINGSKILL_USESKILL),delta),SHOWPOS_CHAT);
#endif
					iCurRipe = pData->getRipe();
					pSkill->setRipe(iCurRipe);
				}
				else
				{
#ifdef NTJ_CLIENT
					MessageEvent::show(SHOWTYPE_NOTIFY,avar(GetLocalStr(MSG_LIVINGSKILL_USESKILL),stGuerdon.num),SHOWPOS_CHAT);
#endif
					pSkill->setRipe(iCurRipe);
				}
			}

			if (extRipe > 0)
			{
				iCurRipe += extRipe;
				if(iCurRipe > pData->getRipe())
				{
#ifdef NTJ_CLIENT
					U32 delta = pData->getRipe() - pSkill->getRipe();
					MessageEvent::show(SHOWTYPE_NOTIFY,avar(GetLocalStr(MSG_LIVINGSKILL_USEPRODUCE),delta),SHOWPOS_CHAT);
#endif
					iCurRipe = pData->getRipe();
					pSkill->setRipe(iCurRipe);
				}
				else
				{

#ifdef NTJ_CLIENT
					MessageEvent::show(SHOWTYPE_NOTIFY,avar(GetLocalStr(MSG_LIVINGSKILL_USEPRODUCE),extRipe),SHOWPOS_CHAT);
#endif
					pSkill->setRipe(iCurRipe);
				}
			}

			//�������
			tempRipe = iCurRipe - tempRipe;
			if(tempRipe >0)
				addLivingExp(tempRipe);

#ifdef  NTJ_SERVER
			LivingSkillGuerdonEvent* event = new LivingSkillGuerdonEvent(skillId,extRipe);
			conn->postNetEvent(event);
#endif
		}
	}
#endif
}
////////////////////////////////////////////////////////////����˴���////////////////////////////////////////////////////////////����˴���
////////////////////////////////////////////////////////////����˴���////////////////////////////////////////////////////////////����˴���
#ifdef NTJ_SERVER

// ----------------------------------------------------------------------------
// ��ʼ����ҽ�ɫ����
void Player::initializeData(GameConnection *pConn,T_UID UID,stPlayerStruct *pPlayerData)
{
	//mTeamId		= ... ����Ҫ��TeamId����������ʼ��
	memcpy( &mTeamInfo, g_ServerGameplayState->GetPlayerTeam( UID, pPlayerData->BaseData.PlayerId ), sizeof( stTeamInfo ) );

	setLayerID( pPlayerData->DispData.CopyMapInstId );

	mCopymapInstId = pPlayerData->DispData.CopyMapInstId;

	//��ʼ��������ݣ�Ȼ��PACKUPDATE���ͻ���
	mUID		= UID;
	mIsAdult    = pPlayerData->BaseData.isAdult;
	mPointNum   = pPlayerData->BaseData.PointNum;
	dTotalOnlineTime = pPlayerData->BaseData.dTotalOnlineTime;
	mPlayerId   = pPlayerData->BaseData.PlayerId;
	mAccountId  = pPlayerData->BaseData.AccountId;
	mTriggerId	= pPlayerData->DispData.TriggerNo;
	mBody		= pPlayerData->DispData.Body;
	mFace		= pPlayerData->DispData.Face;
	mHair		= pPlayerData->DispData.Hair;
	mHairCol	= pPlayerData->DispData.HairCol;
	mOtherFlag	= pPlayerData->DispData.OtherFlag;
	mMoney		= pPlayerData->MainData.Money;
	mBindMoney	= pPlayerData->MainData.BindMoney;
	mBankMoney	= pPlayerData->MainData.BankMoney;
	m_Level		= pPlayerData->DispData.Level;
	m_Exp		= pPlayerData->MainData.CurrentExp;
	mTradeLockFlag = pPlayerData->MainData.TradeLockFlag;
	mSecondPassword = StringTable->insert(pPlayerData->MainData.SecondPassword);
	mLivingSkillStudyID = pPlayerData->MainData.LivingSkillStudyID;
	mLastLoginTime = pPlayerData->BaseData.lastLoginTime;
	mTimerTriggerMgr->Initialize(pPlayerData);
	mTimerPetStudyMgr->InitializePetStudy(pPlayerData);
	pMPManagerInfo->initialize(pPlayerData);
	mBeginPlayerTime = Platform::getTime();
	if(gServerPathManager->isValidPath(pPlayerData->DispData.flyPath))
	{
		mFlyPath.pathId = pPlayerData->DispData.flyPath;
		mFlyPath.flyPos = pPlayerData->DispData.flyPos;
		mFlyPath.mountId = pPlayerData->DispData.flyMount;
	}

	dMemcpy(mPlayerDataFlags, pPlayerData->MainData.PlayerDataFlag, MAX_PLAYERDATAFLAGS);

	mSocialCount = pPlayerData->MainData.SocialItemCount;
	dMemcpy( mSocialItems, pPlayerData->MainData.SocialItem, sizeof( stSocialItem ) * mSocialCount );

	if(mSecondPassword[0] == '\0')
		mBankLock = false;
	else
		mBankLock = true;
	// ���ý�ɫ����
	setPlayerName(pPlayerData->BaseData.PlayerName);
	// ����DataBlock������
	mDataBlockId = mBody;

	m_Sex = (enSex)(pPlayerData->DispData.Sex);
	m_Race = (enRace)(pPlayerData->DispData.Race);
	m_Family = (enFamily)(pPlayerData->DispData.Family);
	for(S32 i=0; i<MaxClassesMasters; i++)
		m_Classes[i] = pPlayerData->DispData.Classes[i];

	//������Һ����ӿ���
	setControllingClient(pConn);
	pConn->setControlObject(this);

	// ��ʼ��ҳ�������
	if( dStrcmp( pPlayerData->DispData.LeavePos, "0" ) == 0 )
	{
		dStrcpy( pPlayerData->DispData.LeavePos, COMMON_STRING_LENGTH, Con::executef( "SvrGetNewbiePosition" ) );
	}

	MatrixF mat = pConn->getCameraObject()->getTransform();
	if((!dStricmp(pPlayerData->DispData.LeavePos,"") && !dStricmp( pPlayerData->DispData.fLeavePos, "" ) )					//��һ�ε�½
		|| ( pPlayerData->DispData.ZoneId!=g_ServerGameplayState->getZoneId() && pPlayerData->DispData.fZoneId != g_ServerGameplayState->getZoneId()))		//��һ���뿪�ĵ�ͼ�ͱ��εĲ�ͬ
	{
		//��ʱ��������������꣬����Ӧ��Ϊ�߻��趨��������ߴ�����������õ�����
		setTransform(mat);
	}
	else
	{
		Con::printf( "LOAD PLAYER DATA, POS:%s", pPlayerData->DispData.LeavePos );

		//������һ���뿪��ͼ������
		Point3F pos;
		if( dStricmp( pPlayerData->DispData.fLeavePos, "" ) && pPlayerData->DispData.CopyMapInstId != 1 )
		{
			dSscanf(pPlayerData->DispData.fLeavePos,"%f %f %f",&pos.x, &pos.y, &pos.z);
		}
		else
		{
			dSscanf(pPlayerData->DispData.LeavePos,"%f %f %f",&pos.x, &pos.y, &pos.z);
		}
		// ��Ϊ����������ģ����Բ���Ҫ�����߶�
		mat.setColumn(3,pos);
		setTransform(mat);
	}

	if(mTriggerId)
	{
		//���򴥷��������
	}


	// ���������������
	dMemcpy(&missionInfo, &pPlayerData->MainData.MissionInfo, sizeof(stMission));

	// ��ʼ����ȴʱ��
	initializeCooldown(pPlayerData);
	// ��ʼ������
	initializeSkill(pPlayerData);
	// ������ͨ������װ����Ӧ������������
	AddSkill(Macro_GetSkillId(DefaultSkill_Min + m_ArmStatus, 1), false);
	// ��ʼ�������
	initLivingSkill(pPlayerData);
	// ��ʼ���䷽
	initPrescription(pPlayerData);
	// ��ʼ��״̬
	m_BuffTable.SetObj(this);
	initializeBuff(pPlayerData);

	// ����װ�����洢����
	equipList.LoadData(pPlayerData);
	// ������Ʒ���洢����
	S32 maxBag1 = equipList.GetBagSize(Res::EQUIPPLACEFLAG_PACK1);
	S32 maxBag2 = equipList.GetBagSize(Res::EQUIPPLACEFLAG_PACK2);
	inventoryList.SetMaxSlot(ItemManager::EXTBAG_BEGINSLOT + maxBag1 + maxBag2);
	inventoryList.LoadData(pPlayerData);
	// �������вֿ����洢����
	maxBag1 = equipList.GetBagSize(Res::EQUIPPLACEFLAG_BANKPACK);
	maxBag2 = pPlayerData->MainData.BankBagSize;
	S32 maxBag3 = m_Level >= 10 ? 6 : 0;
	bankList.SetMaxSlot(ItemManager::EXTBAG_BANK_BEGINSLOT + maxBag1 + maxBag2 + maxBag3);
	bankList.LoadData(pPlayerData);
	// ���ؿ�����洢����
	panelList.LoadData(pPlayerData,this);
	// ���س��Ｐ�������
	mPetTable.loadData(pPlayerData);
	// ����Ԫ������
	mSpiritTable.LoadData(pPlayerData);
	// ������Ʒ�ֽ�����
	mItemSplitList.LoadData(pPlayerData);
	//��������Ʒ�ϳ�����
	mItemComposeList.LoadData(pPlayerData);
	// ��ʼʵЧ����
	LoadTimeItem();

	// ��ֵ����
	m_BuffTable.AddBuff(Buff::Origin_Base, Macro_GetBuffId(g_FamilyBuff[m_Family], m_Level), this);
	m_BuffTable.SetRefresh(true);
	g_ItemManager->mountAllEquipAttribute(this);
	CalcStats();
	setHP(pPlayerData->MainData.CurrentHP);
	setMP(pPlayerData->MainData.CurrentMP);
	setPP(pPlayerData->MainData.CurrentPP);

	mVigor            = pPlayerData->MainData.CurrentVigor;
	mMaxVigor         = pPlayerData->MainData.MaxVigor;
	mCurrentLivingExp = pPlayerData->MainData.CurrentLivingExp;
	mLivingLevel      = pPlayerData->MainData.LivingLevel;

	// �Ƿ����ֽ�ɫ
	if(IsOtherFlag(OTHERFLAG_NEWBIE))
	{
		//.....
		// �ⲿ��ע��߻��� NewbieInit �ű�
		setOtherFlag(OTHERFLAG_USEFASHION);
		setOtherFlag(OTHERFLAG_FAMILYFASHION);
	}	
}

// ----------------------------------------------------------------------------
// �����ҽ�ɫ���ݵ����ݴ洢�ṹ
stPlayerStruct *Player::buildPlayerData()
{
	static stPlayerStruct PlayerData;
	dMemset(&PlayerData,0,sizeof(stPlayerStruct));

	//�����ҵĽṹ
	PlayerData.BaseData.AccountId	= mAccountId;
	PlayerData.BaseData.PlayerId	= mPlayerId;

	// �������SID
	PlayerData.BaseData.SaveUID = g_ServerGameplayState->GetPlayerSID( mPlayerId ) + 1;
	g_ServerGameplayState->SetPlayerSID( mPlayerId, PlayerData.BaseData.SaveUID );

	stPlayerStruct* pPlayer = g_ServerGameplayState->GetPlayerLoginData( mPlayerId );
	if( !pPlayer )
	{
		throw "";
	}

	const char* szName = getPlayerName();
	if(szName != NULL)
		dStrcpy( PlayerData.BaseData.PlayerName, sizeof(PlayerData.BaseData.PlayerName), szName);

	if( getLayerID() != 1 )
	{
		PlayerData.DispData.fZoneId = g_ServerGameplayState->getZoneId();
		PlayerData.DispData.ZoneId = pPlayer->DispData.ZoneId;
	}
	else
	{
		PlayerData.DispData.ZoneId	= g_ServerGameplayState->getZoneId();
		PlayerData.DispData.fZoneId = pPlayer->DispData.fZoneId;
	}

	PlayerData.DispData.Sex			= m_Sex;
	PlayerData.DispData.Race		= m_Race;
	PlayerData.DispData.Family		= m_Family;
	PlayerData.DispData.Level		= m_Level;
	PlayerData.MainData.Money       = mMoney;
	PlayerData.MainData.BindMoney   = mBindMoney;
	PlayerData.MainData.BankMoney   = mBankMoney;
	PlayerData.DispData.Body		= mBody;
	PlayerData.DispData.Face		= mFace;
	PlayerData.DispData.Hair		= mHair;
	PlayerData.DispData.HairCol		= mHairCol;

	PlayerData.DispData.CopyMapInstId = mCopymapInstId;

	// �������·��
	PlayerData.DispData.flyPath = mFlyPath.pathId;
	PlayerData.DispData.flyPos = mFlyPath.flyPos;
	PlayerData.DispData.flyMount = mFlyPath.mountId;

	// �����ɫʱ������ֽ�ɫ��־
	// mOtherFlag &= ~OTHERFLAG_NEWBIE;
	PlayerData.DispData.OtherFlag	= mOtherFlag;

	for(S32 i=0; i<MaxClassesMasters; i++)
		PlayerData.DispData.Classes[i] = m_Classes[i];

	PlayerData.MainData.CurrentHP = GetStats().HP;
	PlayerData.MainData.CurrentMP = GetStats().MP;
	PlayerData.MainData.CurrentPP = GetStats().PP;

	PlayerData.MainData.CurrentVigor     = mVigor;
	PlayerData.MainData.MaxVigor         = mMaxVigor;
	PlayerData.MainData.CurrentLivingExp = mCurrentLivingExp;
	PlayerData.MainData.LivingLevel      = mLivingLevel;
	PlayerData.MainData.CurrentExp = m_Exp;
	PlayerData.MainData.LivingSkillStudyID = mLivingSkillStudyID;

	dMemcpy(PlayerData.MainData.PlayerDataFlag, mPlayerDataFlags, MAX_PLAYERDATAFLAGS);

	Point3F pos;
	getTransform().getColumn(3,&pos);

	// if the player is not in the copy map
	if( getLayerID() == 1 )
	{
		dSprintf(PlayerData.DispData.LeavePos,sizeof(PlayerData.DispData.LeavePos),"%.2f %.2f %.2f",pos.x,pos.y,pos.z);

		dStrcpy( PlayerData.DispData.fLeavePos,sizeof(PlayerData.DispData.LeavePos), mCopyMapPosition.c_str() );
	}
	else
	{
		dSprintf(PlayerData.DispData.fLeavePos,sizeof(PlayerData.DispData.fLeavePos),"%.2f %.2f %.2f",pos.x,pos.y,pos.z);

		dStrcpy(PlayerData.DispData.LeavePos, sizeof(PlayerData.DispData.LeavePos), pPlayer->DispData.LeavePos );
	}

	// �����ɫ��������
	g_MissionManager->PauseTime(this);
	dMemcpy(&PlayerData.MainData.MissionInfo, &missionInfo, sizeof(stMission));

	// ��������ϵ
	PlayerData.MainData.SocialItemCount = mSocialCount;
	for( int i = 0; i< mSocialCount; i++ )
	{
		PlayerData.MainData.SocialItem[i].friendValue = mSocialItems[i].friendValue;
		PlayerData.MainData.SocialItem[i].playerId = mSocialItems[i].playerId;
		PlayerData.MainData.SocialItem[i].type = mSocialItems[i].type;
	}

	// ������ȴ����
	m_CooldownTable.SaveData(&PlayerData);
	// ���漼������
	m_SkillTable.SaveData(&PlayerData);
	// ���漼������
	m_BuffTable.SaveData(&PlayerData);
	// �����ɫװ��������Ʒ�����������Ϳ��������
	equipList.SaveData(&PlayerData);
	inventoryList.SaveData(&PlayerData);
	bankList.SaveData(&PlayerData);
	panelList.SaveData(&PlayerData);
	//���������
	pLivingSkill->saveData(&PlayerData);
	pPrescription->saveData(&PlayerData);
	//�����������
	mPetTable.saveData(&PlayerData);
	//����Ԫ������
	mSpiritTable.SaveData(&PlayerData);
	//������Ʒ�ֽ�����
	mItemSplitList.SaveData(&PlayerData);
	//������Ʒ�ϳ�����
	mItemComposeList.SaveData(&PlayerData);
	//����ʵЧ
	SaveTimeItem();

	//ɾ������ʱ�䴥��������
	dMemset(PlayerData.MainData.TimerTriggerInfo, 0, sizeof(stTimerTriggerInfo) * MAX_TIMER_SIZE);
	dMemset(PlayerData.MainData.TimerPetStudyInfo, 0, sizeof(stTimerTriggerInfo) * PET_MAXSLOTS);

	// ��ʱ����������
	mTimerTriggerMgr->SaveData(&PlayerData);
	//�����ʱ�������ṹ
	mTimerPetStudyMgr->SavePetStudyData(&PlayerData);
	pMPManagerInfo->saveData(&PlayerData);
	return &PlayerData;
}

// ----------------------------------------------------------------------------
// ɱ��������ݹ����߷��侭�顢����������á���Ʒ����
void Player::killNpc(NpcObject* npc)
{
	// ��ȡ��������
	NpcObjectData* npcdata = g_NpcRepository.GetNpcData(npc->getDataID());
	if(npcdata == NULL)
		return;

	U32 nItemPackageId = 0;
	S32 nMaxDropNum = 0;

	std::list<Player*> playerList;

	// �жϵ�ǰ�������Ƿ��ڶ�����
	if(mTeamInfo.m_bInTeam)
	{
		CZoneTeam* pTeam = g_ZoneTeamManager.GetTeam(mTeamInfo.m_nId);
		if(pTeam)
		{
			Player* player = NULL;
			for(int i = 0; i < pTeam->GetTeammateCount(); i++)
			{
				player = g_ServerGameplayState->GetPlayer( pTeam->GetPlayerId( i ) );
				if( !player )
					continue;

				//�ж���������ľ����Ƿ��ڹ����������֮��
				if(player->getDistance(npc) <= npcdata->expField)
				{
					playerList.push_back( player );
				}
			}

			Con::printf( "ɱ��������棬��%d��", playerList.size() );

			for( std::list<Player*>::iterator it = playerList.begin(); it != playerList.end(); it++ )
			{
				player = *it;

				//�ӳ�����ʦ��
				if(player->getMPInfo() && player->getTeamInfo().m_bIsCaption)
					player->getMPInfo()->calcMasterExp(player,npc,npcdata->expField);

				int maxType = 0;
				
				// ���������ع�ϵ
				for( std::list<Player*>::iterator itt = playerList.begin(); itt != playerList.end(); itt++ )
				{
					if( *itt == player )
						continue;

					stSocialItem* socialItem = player->getPlayerSocialItem( (*itt)->getPlayerID() );
					if( !socialItem )
						continue;

					if( maxType == 0 )
						maxType = socialItem->type;
					else
					{
						if( socialItem->type > 0 && socialItem->type < maxType )
						{
							maxType = socialItem->type;
						}
					}
				}

				Con::executef(player, "KilledMonster", Con::getIntArg(npc->getId()), Con::getIntArg( playerList.size() ), Con::getIntArg( maxType ) );

				// ��ȡ���������
				DropRuleData *pDropRuleData = g_DropRuleRepository.getDropRuleData(npcdata->eventID);
				if (pDropRuleData == NULL)
					return;

				// ������Ʒ�����б�
				DropItemList* droplist = new DropItemList;
				bool bCreatePrizeBox = false;
				// ������������Ʒ�����б�
				if (pDropRuleData->GetItemPackageID(npcdata->eventID, player->getLevel(), npc->getLevel(), nItemPackageId, nMaxDropNum))		
				{
					bCreatePrizeBox = droplist->AddPlayer(player, nItemPackageId, nMaxDropNum);
				}
				// ����PrizeBox����
				if (bCreatePrizeBox)
					PrizeBox::CreatePrizeBox(npc, droplist);
				else
					SAFE_DELETE(droplist);
			}
		}
	}
	else
	{
		// �ж���������ľ����Ƿ��ڹ����������֮��
		if(getDistance(npc) <= npcdata->expField)
		{
			// ��������������
			Con::executef(this, "KilledMonster", Con::getIntArg(npc->getId()));

			// ��ȡ���������
			DropRuleData *pDropRuleData = g_DropRuleRepository.getDropRuleData(npcdata->eventID);
			if (pDropRuleData == NULL)
				return;

			// ������Ʒ�����б�
			DropItemList* droplist = new DropItemList;
			bool bCreatePrizeBox = false;
			// ������������Ʒ�����б�
			if (pDropRuleData->GetItemPackageID(npcdata->eventID, this->getLevel(), npc->getLevel(), nItemPackageId, nMaxDropNum))			
			{
				bCreatePrizeBox = droplist->AddPlayer(this, nItemPackageId, nMaxDropNum);
			}
			// ����PrizeBox����
			if (bCreatePrizeBox)
				PrizeBox::CreatePrizeBox(npc, droplist);
			else
				SAFE_DELETE(droplist);		
		}		
	}	
}

bool Player::initializeSkill(stPlayerStruct* playerInfo)
{
	if(!playerInfo || m_SkillTable.GetSkill().size() > SKILL_MAXSLOTS)
	{
		AssertFatal(false, "SkillTable::LoadData error!!");
		return false;
	}

	for(int i=0; i<SKILL_MAXSLOTS; ++i)
	{
		stSkillInfo& stTemp = playerInfo->MainData.SkillInfo[i];
		if(stTemp.SkillId == 0)
			continue;
		AddSkill(stTemp.SkillId, false);
	}
	return true;
}

bool Player::initLivingSkill(stPlayerStruct* playerInfo)
{
	if (!playerInfo || !pLivingSkill || pLivingSkill->getLivingSkillTable().size() > LIVINGSKILL_MAXSLOTS)
	{
		AssertFatal(false, "LivingSkillTable::LoadData error!!");
		return false;
	}

	for (int i=0; i< LIVINGSKILL_MAXSLOTS; i++)
	{
		stLivingSkillInfo& stTab = playerInfo->MainData.LivingSkillInfo[i];
		if(stTab.SkillID == 0)
			continue;
		pLivingSkill->addLivingSkill(stTab.SkillID,stTab.Ripe);
	}

	return true;
}

bool Player::initializeCooldown(stPlayerStruct* playerInfo)
{
	if(!playerInfo)
	{
		AssertFatal(false, "SkillTable::LoadData error!!");
		return false;
	}

	for(int i=0; i<COOLDOWN_MAX; ++i)
	{
		m_CooldownTable.SetCooldown_Group(i, playerInfo->MainData.CooldownInfo[i].Cooldown * 1000, 0);
	}
	return true;
}

bool Player::initPrescription(stPlayerStruct* playerInfo)
{
	if (!playerInfo || !pPrescription || pPrescription->getPrescriptionTab().size() > PRESCRIPTION_MAX)
	{
		AssertFatal(false, "Prescription::LoadData error!!");
		return false;
	}

	for (int i=0;i<playerInfo->MainData.PrescriptionCount; i++) 
	{
		pPrescription->addPrescription(playerInfo->MainData.PrescriptionInfo[i]);
	}
	return true;
}

bool Player::initializeBuff(stPlayerStruct* playerInfo)
{
	if(!playerInfo)
	{
		AssertFatal(false, "Player::initializeBuff error!!");
		return false;
	}

	for(int i=0; i<BUFF_MAXSLOTS; ++i)
	{
		stBuffInfo& stTemp = playerInfo->MainData.BuffInfo[i];
		if(stTemp.buffId == 0)
			break;
		m_BuffTable.AddBuff(Buff::Origin_Buff, stTemp.buffId, stTemp.self ? this : NULL, stTemp.count, stTemp.time);
	}
	for(int i=0; i<BUFF_MAXSLOTS; ++i)
	{
		stBuffInfo& stTemp = playerInfo->MainData.DebuffInfo[i];
		if(stTemp.buffId == 0)
			break;
		m_BuffTable.AddBuff(Buff::Origin_Buff, stTemp.buffId, stTemp.self ? this : NULL, stTemp.count, stTemp.time);
	}
	for(int i=0; i<BUFF_MAXSLOTS; ++i)
	{
		stBuffInfo& stTemp = playerInfo->MainData.SysBuffInfo[i];
		if(stTemp.buffId == 0)
			break;
		m_BuffTable.AddBuff(Buff::Origin_Buff, stTemp.buffId, stTemp.self ? this : NULL, stTemp.count, stTemp.time);
	}
	return true;
}

// ----------------------------------------------------------------------------
// �ۼ����߻��������;�(�ܻ��򹥻���)
void Player::reduceWear(U32 type, U32 damage)
{
	GameConnection* pConn = getControllingClient();
	if(!pConn)
		return;	
	if(type == Res::CATEGORY_WEAPON)	// �����ۼ��;�
	{
		S32 weaponPos[3][2] = {
			Res::EQUIPPLACEFLAG_WEAPON,		0,
			Res::EQUIPPLACEFLAG_HEAD,		0,
			Res::EQUIPPLACEFLAG_SHOULDER,	0
		};
		U32 iAttack = getCumulateAttack() + damage;
		setCumulateAttack(iAttack);
		S32 iReduce = iAttack / 15000;
		if(iReduce > 0)
		{
			U32 uSumWear = 0;
			ItemShortcut* pItem = NULL;
			for(S32 i = 0; i < 3; i++)
			{
				pItem = (ItemShortcut*)equipList.GetSlot(weaponPos[i][0]);
				if(pItem)
					weaponPos[i][1] = pItem->getRes()->getCurrentMaxWear();
				uSumWear += weaponPos[i][1];
			}

			if(uSumWear >= 1)
			{
				for(S32 i = 0; i < iReduce; ++i)
				{
					U32 iRandom = Platform::getRandomI(1, uSumWear);
					U32 uWears = 0;
					for(S32 k = 0; k < 3; ++k)
					{
						uWears += weaponPos[k][1];
						if(iRandom <= uWears)
						{
							pItem = (ItemShortcut*)equipList.GetSlot(weaponPos[k][0]);
							if(pItem)
							{
								if(pItem->getRes()->getCurrentWear() == 0)
									break;
								S32 iDistance = pItem->getRes()->getCurrentWear() - iReduce;
								if(iDistance <= 0)
								{
									iDistance = 0;
									g_ItemManager->unmountEquipAttribute(this, pItem);
								}
								pItem->getRes()->setCurrentWear(iDistance);
								equipList.SetSlot(weaponPos[k][0], pItem, false);
								equipList.UpdateToClient(pConn, weaponPos[k][0], ITEM_NOSHOW);
							}
							break;
						}
						uSumWear += weaponPos[k][1];
					}
				}
				setCumulateAttack(iAttack % 15000);
			}			
		}
	}
	else if(type == Res::CATEGORY_EQUIPMENT)	//���߿ۼ��;�
	{
		S32 equipPos[5][2] = {
			Res::EQUIPPLACEFLAG_BACK,		0,
			Res::EQUIPPLACEFLAG_BODY,		0,
			Res::EQUIPPLACEFLAG_HAND,		0,
			Res::EQUIPPLACEFLAG_WAIST,		0,
			Res::EQUIPPLACEFLAG_FOOT,		0
		};
		U32 iDamage = getCumulateDamage() + damage;
		setCumulateDamage(iDamage);
		S32 iReduce = iDamage / 2000;
		if(iReduce > 0)
		{
			U32 uSumWear = 0;
			ItemShortcut* pItem = NULL;
			for(S32 i = 0; i < 5; i++)
			{
				pItem = (ItemShortcut*)equipList.GetSlot(equipPos[i][0]);
				if(pItem)
					equipPos[i][1] = pItem->getRes()->getCurrentMaxWear();
				uSumWear += equipPos[i][1];
			}

			if(uSumWear >= 1)
			{
				for(S32 i = 0; i < iReduce; ++i)
				{
					U32 iRandom = Platform::getRandomI(1, uSumWear);
					U32 uWears = 0;
					for(S32 k = 0; k < 5; ++k)
					{
						uWears += equipPos[k][1];
						if(iRandom <= uWears)
						{
							pItem = (ItemShortcut*)equipList.GetSlot(equipPos[k][0]);
							if(pItem)
							{
								if(pItem->getRes()->getCurrentWear() == 0)
									break;
								S32 iDistance = pItem->getRes()->getCurrentWear() - iReduce;
								if(iDistance <= 0)
								{
									iDistance = 0;
									g_ItemManager->unmountEquipAttribute(this, pItem);
								}
								pItem->getRes()->setCurrentWear(iDistance);
								equipList.SetSlot(equipPos[k][0], pItem, false);
								equipList.UpdateToClient(pConn, equipPos[k][0], ITEM_NOSHOW);
							}
							break;
						}
					}
				}
				setCumulateDamage(iDamage % 2000);
			}			
		}
	}
}

// ----------------------------------------------------------------------------
// �ۼ�ȫ��װ���;�(�����ͷ���)
void Player::reduceAllWear()
{
	GameConnection* pConn = getControllingClient();
	if(!pConn)
		return;
	ItemShortcut* pItem = NULL;
	for(S32 i=Res::EQUIPPLACEFLAG_HEAD; i < Res::EQUIPPLACEFLAG_TRUMP; ++i)
	{
		if(i != Res::EQUIPPLACEFLAG_NECK || i != Res::EQUIPPLACEFLAG_RING || i != Res::EQUIPPLACEFLAG_AMULET)
		{
			U32 iReduce = 0;
			pItem = (ItemShortcut*)(equipList.GetSlot(i));
			if(!pItem)
				continue;
			iReduce = mCeil(pItem->getRes()->getCurrentMaxWear() / 10.0f);
			if(pItem->getRes()->getCurrentWear() == 0)
				continue;
			S32 iDistance = pItem->getRes()->getCurrentWear() - iReduce;
			if(iDistance <= 0)
			{
				iDistance = 0;
				g_ItemManager->unmountEquipAttribute(this, pItem);
			}
			pItem->getRes()->setCurrentWear(iDistance);
			equipList.SetSlot(i, pItem, false);
			equipList.UpdateToClient(pConn, i, ITEM_NOSHOW);
		}
	}
}

// ----------------------------------------------------------------------------
// �ָ�����ֵ,5���ӻָ�һ��,�����ָ�10��.
void Player::recoverVigor()
{
	SimTime recoverTime =  (Platform::getTime() - mLastLoginTime) % 300;
	if(recoverTime == 0 && mIsRecoverVigor)
	{
		addVigor(10*mRecoverVigorVelocity);
		mIsRecoverVigor = false;
	}
	
	if(recoverTime != 0 && !mIsRecoverVigor)
		mIsRecoverVigor = true;
}
// ----------------------------------------------------------------------------
// ���������Ŀ���������ϵ
stSocialItem* Player::getPlayerSocialItem(U32 playerID)
{
	if(mSocialCount>0)
	{
		for (S32 i=0; i<mSocialCount; i++)
		{
			if(mSocialItems[i].playerId == playerID)
			{
				return &mSocialItems[i];
			}
		}
	}

	return NULL;
}
// ----------------------------------------------------------------------------
// ˢ��ʱװ��־
void Player::updateFashionFlags()
{
	if(getOtherFlag() & Player::OTHERFLAG_USEFASHION)
	{
		if(getOtherFlag() & Player::OTHERFLAG_FAMILYFASHION)
		{
			mShapeStack.setFlags(ShapeStack::Stack_Fashion, ShapeStack::Flags_Enabled, false);
			mShapeStack.setFlags(ShapeStack::Stack_FamilyFashion, ShapeStack::Flags_Enabled, true);
		}
		else
		{
			mShapeStack.setFlags(ShapeStack::Stack_Fashion, ShapeStack::Flags_Enabled, true);
			mShapeStack.setFlags(ShapeStack::Stack_FamilyFashion, ShapeStack::Flags_Enabled, false);
		}
	}
	else
	{
		mShapeStack.setFlags(ShapeStack::Stack_Fashion, ShapeStack::Flags_Enabled, false);
		mShapeStack.setFlags(ShapeStack::Stack_FamilyFashion, ShapeStack::Flags_Enabled, false);
	}
}
//����ʱЧ����
void Player::SaveTimeItem()
{
	ItemShortcut* pItem = NULL;
	Res* pRes = NULL;
	U32 disTime = 0;
	U32 leaveTime = 0;
	for(int i=0; i<inventoryList.GetMaxSlots(); ++i)
	{
		pItem = (ItemShortcut*)inventoryList.GetSlot(i);
		if(!pItem)
			continue;
		pRes = pItem->getRes();
		if(!pRes)
			continue;
		switch (pRes->getTimeMode())
		{
		case Res::TIMEMODE_DOWNONLY:	//�����߿�ʱ
			{
				pRes->setLapseTime(Sim::getCurrentTime());
			}
			break;
		case Res::TIMEMODE_ONLINEGET:	//��ȡ�����߿�ʱ
			{
				disTime = mLastSaveTime - mBeginPlayerTime;
				leaveTime = pRes->getLapseTime() - disTime;
				if(leaveTime < 0)
					continue;
				pRes->setLapseTime(leaveTime);
			}
			break;
		case Res::TIMEMODE_ONLINEUSE:	//ʹ�ú����߿�ʱ
			{
				disTime = mLastSaveTime - mBeginPlayerTime;
				leaveTime = pRes->getLapseTime() - disTime;
				if(leaveTime < 0)
					continue;
				pRes->setLapseTime(leaveTime);
			}
			break;
		case Res::TIMEMODE_ALLGET:		//��ȡ�������߶���ʱ
			{
				disTime = mLastSaveTime - mBeginPlayerTime;
				leaveTime = pRes->getLapseTime() - disTime;
				if(leaveTime < 0)
					continue;
				pRes->setLapseTime(leaveTime);
			}
			break;
		case Res::TIMEMODE_ALL:			//ʹ�ú��������߶���ʱ
			{
				disTime = mLastSaveTime - mBeginPlayerTime;
				leaveTime = pRes->getLapseTime() - disTime;
				if(leaveTime < 0)
					continue;
				pRes->setLapseTime(leaveTime);
			}
			break;
		default:
			continue;
		}
		//inventoryList.UpdateToClient(getControllingClient(), i, ITEM_NOSHOW);
	}

	for(int i=0; i<equipList.GetMaxSlots(); ++i)
	{
		pItem = (ItemShortcut*)equipList.GetSlot(i);
		if(!pItem)
			continue;
		pRes = pItem->getRes();
		if(!pRes)
			continue;
		switch (pRes->getTimeMode())
		{
		case Res::TIMEMODE_DOWNONLY:	//�����߿�ʱ
			{
				pRes->setLapseTime(mLastSaveTime);
			}
			break;
		case Res::TIMEMODE_ONLINEGET:	//��ȡ�����߿�ʱ
			{
				disTime = mLastSaveTime - mBeginPlayerTime;
				leaveTime = pRes->getLapseTime() - disTime;
				if(leaveTime < 0)
					continue;
				pRes->setLapseTime(leaveTime);
			}
			break;
		case Res::TIMEMODE_ONLINEUSE:	//ʹ�ú����߿�ʱ
			{
				disTime = mLastSaveTime - mBeginPlayerTime;
				leaveTime = pRes->getLapseTime() - disTime;
				if(leaveTime < 0)
					continue;
				pRes->setLapseTime(leaveTime);
			}
			break;
		case Res::TIMEMODE_ALLGET:		//��ȡ�������߶���ʱ
			{
				disTime = mLastSaveTime - mBeginPlayerTime;
				leaveTime = pRes->getLapseTime() - disTime;
				if(leaveTime < 0)
					continue;
				pRes->setLapseTime(leaveTime);
			}
			break;
		case Res::TIMEMODE_ALL:			//ʹ�ú��������߶���ʱ
			{
				disTime = mLastSaveTime - mBeginPlayerTime;
				leaveTime = pRes->getLapseTime() - disTime;
				if(leaveTime < 0)
					continue;
				pRes->setLapseTime(leaveTime);
			}
			break;
		default:
			continue;
		}
		//equipList.UpdateToClient(getControllingClient(), i, ITEM_NOSHOW);
	}

	for(int i=0; i<bankList.GetMaxSlots(); ++i)
	{
		pItem = (ItemShortcut*)bankList.GetSlot(i);
		if(!pItem)
			continue;
		pRes = pItem->getRes();
		if(!pRes)
			continue;
		switch (pRes->getTimeMode())
		{
		case Res::TIMEMODE_DOWNONLY:	//�����߿�ʱ
			{
				pRes->setLapseTime(mLastSaveTime);
			}
			break;
		case Res::TIMEMODE_ONLINEGET:	//��ȡ�����߿�ʱ
			{
				disTime = mLastSaveTime - mBeginPlayerTime;
				leaveTime = pRes->getLapseTime() - disTime;
				if(leaveTime < 0)
					continue;
				pRes->setLapseTime(leaveTime);
			}
			break;
		case Res::TIMEMODE_ONLINEUSE:	//ʹ�ú����߿�ʱ
			{
				disTime = mLastSaveTime - mBeginPlayerTime;
				leaveTime = pRes->getLapseTime() - disTime;
				if(leaveTime < 0)
					continue;
				pRes->setLapseTime(leaveTime);
			}
			break;
		case Res::TIMEMODE_ALLGET:		//��ȡ�������߶���ʱ
			{
				disTime = mLastSaveTime - mBeginPlayerTime;
				leaveTime = pRes->getLapseTime() - disTime;
				if(leaveTime < 0)
					continue;
				pRes->setLapseTime(leaveTime);
			}
			break;
		case Res::TIMEMODE_ALL:			//ʹ�ú��������߶���ʱ
			{
				disTime = mLastSaveTime - mBeginPlayerTime;
				leaveTime = pRes->getLapseTime() - disTime;
				if(leaveTime < 0)
					continue;
				pRes->setLapseTime(leaveTime);
			}
			break;
		default:
			continue;
		}
		//bankList.UpdateToClient(getControllingClient(), i, ITEM_NOSHOW);
	}
}
//����ʱЧ����
void Player::LoadTimeItem()
{
	ItemShortcut* pItem = NULL;
	Res* pRes = NULL;
	//ʣ��ʱ��
	U32 uLapseTime = 0;
	//���ʱ�䣨�ϴ����ߵ��������ߵļ����
	U32 uDisTime = 0;
	//��Ʒ��
	for(int i=0; i<inventoryList.GetMaxSlots(); ++i)
	{
		pItem = (ItemShortcut*)inventoryList.GetSlot(i);
		if(!pItem)
			continue;
		pRes = pItem->getRes();
		if(!pRes)
			continue;
		uLapseTime = pRes->getLapseTime();
		if(uLapseTime == 0)
			continue;
		
		switch (pRes->getTimeMode())
		{
		case Res::TIMEMODE_DOWNONLY:	//�����߿�ʱ
			{
				uDisTime = mBeginPlayerTime - mLastLoginTime;
				if(uDisTime >= uLapseTime)
				{
					//������Ϣ�����䣬��ʾĳ��ƷʵЧ����
					inventoryList.SetSlot(i, NULL);
					inventoryList.UpdateToClient(getControllingClient(), i, ITEM_NOSHOW);
					continue;
				}
				pRes->setLapseTime(uLapseTime - uDisTime);
			}
			break;
		case Res::TIMEMODE_ONLINEGET:	//��ȡ�����߿�ʱ
			break;
		case Res::TIMEMODE_ONLINEUSE:	//ʹ�ú����߿�ʱ
			break;
		case Res::TIMEMODE_ALLGET:		//��ȡ�������߶���ʱ
			{
				uDisTime = mBeginPlayerTime - mLastLoginTime;
				if(uDisTime >= uLapseTime)
				{
					//������Ϣ�����䣬��ʾĳ��ƷʵЧ����
					inventoryList.SetSlot(i, NULL);
					inventoryList.UpdateToClient(getControllingClient(), i, ITEM_LOSE);
					continue;
				}
				pRes->setLapseTime(uLapseTime - uDisTime);
				
			}break;
		case Res::TIMEMODE_ALL:			//ʹ�ú��������߶���ʱ
			{
				uDisTime = mBeginPlayerTime - mLastLoginTime;
				if(uDisTime >= uLapseTime)
				{
					//������Ϣ�����䣬��ʾĳ��ƷʵЧ����
					inventoryList.SetSlot(i, NULL);
					inventoryList.UpdateToClient(getControllingClient(), i, ITEM_LOSE);
					continue;
				}
				pRes->setLapseTime(uLapseTime - uDisTime);
				
			}break;
		default:
			continue;
		}
		inventoryList.UpdateToClient(getControllingClient(), i, ITEM_NOSHOW);

		stTimeItem* timeItem = new stTimeItem;
		timeItem->uID = pRes->getUID();
		timeItem->uLeaveTime = uLapseTime - uDisTime;
		timeItem->uStartTime = mBeginPlayerTime;
		pushToTimeItemVec(timeItem);
	}

	//װ����
	for(int i=0; i<equipList.GetMaxSlots(); ++i)
	{
		pItem = (ItemShortcut*)equipList.GetSlot(i);
		if(!pItem)
			continue;
		pRes = pItem->getRes();
		if(!pRes)
			continue;
		uLapseTime = pRes->getLapseTime();
		if(uLapseTime == 0)
			continue;

		switch (pRes->getTimeMode())
		{
		case Res::TIMEMODE_DOWNONLY:	//�����߿�ʱ
			break;
		case Res::TIMEMODE_ONLINEGET:	//��ȡ�����߿�ʱ
			break;
		case Res::TIMEMODE_ONLINEUSE:	//ʹ�ú����߿�ʱ
			break;
		case Res::TIMEMODE_ALLGET:		//��ȡ�������߶���ʱ
			{
				uDisTime = mBeginPlayerTime - mLastLoginTime;
				if(uDisTime >= uLapseTime)
				{
					//������Ϣ�����䣬��ʾĳ��ƷʵЧ����
					g_ItemManager->unmountEquipAttribute(this, pItem);
					g_ItemManager->unmountEquipModel(this, pItem);
					equipList.SetSlot(i, NULL);
					equipList.UpdateToClient(getControllingClient(), i, ITEM_LOSE);
					continue;
				}
				pRes->setLapseTime(uLapseTime - uDisTime);
				
			}break;
		case Res::TIMEMODE_ALL:			//ʹ�ú��������߶���ʱ
			{
				uDisTime = mBeginPlayerTime - mLastLoginTime;
				if(uDisTime >= uLapseTime)
				{
					//������Ϣ�����䣬��ʾĳ��ƷʵЧ����
					g_ItemManager->unmountEquipAttribute(this, pItem);
					g_ItemManager->unmountEquipModel(this, pItem);
					equipList.SetSlot(i, NULL);
					equipList.UpdateToClient(getControllingClient(), i, ITEM_LOSE);
					continue;
				}
				pRes->setLapseTime(uLapseTime - uDisTime);
				
			}break;
		default:
			continue;
		}
		equipList.UpdateToClient(getControllingClient(), i, ITEM_LOSE);

		stTimeItem* timeItem = new stTimeItem;
		timeItem->uID = pRes->getUID();
		timeItem->uLeaveTime = uLapseTime - uDisTime;
		timeItem->uStartTime = mBeginPlayerTime;
		pushToTimeItemVec(timeItem);
	}
	//�ֿ�
	for(int i=0; i<bankList.GetMaxSlots(); ++i)
	{
		pItem = (ItemShortcut*)bankList.GetSlot(i);
		if(!pItem)
			continue;
		pRes = pItem->getRes();
		if(!pRes)
			continue;
		uLapseTime = pRes->getLapseTime();
		if(uLapseTime == 0)
			continue;

		switch (pRes->getTimeMode())
		{
		case Res::TIMEMODE_DOWNONLY:	//�����߿�ʱ
			break;
		case Res::TIMEMODE_ONLINEGET:	//��ȡ�����߿�ʱ
			break;
		case Res::TIMEMODE_ONLINEUSE:	//ʹ�ú����߿�ʱ
			break;
		case Res::TIMEMODE_ALLGET:		//��ȡ�������߶���ʱ
			{
				uDisTime = mBeginPlayerTime - mLastLoginTime;
				if(uDisTime >= uLapseTime)
				{
					//������Ϣ�����䣬��ʾĳ��ƷʵЧ����
					bankList.SetSlot(i, NULL);
					bankList.UpdateToClient(getControllingClient(), i, ITEM_LOSE);
					continue;
				}
				pRes->setLapseTime(uLapseTime - uDisTime);
				
			}break;
		case Res::TIMEMODE_ALL:			//ʹ�ú��������߶���ʱ
			{
				uDisTime = mBeginPlayerTime - mLastLoginTime;
				if(uDisTime >= uLapseTime)
				{
					//������Ϣ�����䣬��ʾĳ��ƷʵЧ����
					bankList.SetSlot(i, NULL);
					bankList.UpdateToClient(getControllingClient(), i, ITEM_LOSE);
					continue;
				}
				pRes->setLapseTime(uLapseTime - uDisTime);
				
			}break;
		default:
			continue;
		}
		bankList.UpdateToClient(getControllingClient(), i, ITEM_NOSHOW);

		stTimeItem* timeItem = new stTimeItem;
		timeItem->uID = pRes->getUID();
		timeItem->uLeaveTime = uLapseTime - uDisTime;
		timeItem->uStartTime = mBeginPlayerTime;
		pushToTimeItemVec(timeItem);
	}

}

//����ʱЧ�б�
void Player::updateTimeItem()
{
	stTimeItem* timeItem = NULL;
	U32 disTime = 0;
	for(int i=0; i<mTimeItemVec.size(); ++i)
	{
		timeItem = mTimeItemVec[i];
		if(!timeItem)
			continue;
		disTime = Platform::getTime() - timeItem->uStartTime;
		if(disTime >= timeItem->uLeaveTime)
		{
			S32 iIndex = -1;
			iIndex = inventoryList.findSlot(timeItem->uID);
			if(iIndex != -1)
			{
				inventoryList.SetSlot(iIndex, NULL);
				inventoryList.UpdateToClient(getControllingClient(), iIndex, ITEM_LOSE);
				removeFromTimeItemVec(timeItem->uID);
				continue;
			}

			iIndex = equipList.findSlot(timeItem->uID);
			if(iIndex != -1)
			{
				ItemShortcut* pItem = (ItemShortcut*)equipList.GetSlot(iIndex);
				if(!pItem)
					continue;
				g_ItemManager->unmountEquipAttribute(this, pItem);
				g_ItemManager->unmountEquipModel(this, pItem);
				equipList.SetSlot(iIndex, NULL);
				equipList.UpdateToClient(getControllingClient(), iIndex, ITEM_LOSE);
				removeFromTimeItemVec(timeItem->uID);
				continue;
			}

			iIndex = bankList.findSlot(timeItem->uID);
			if(iIndex != -1)
			{
				bankList.SetSlot(iIndex, NULL);
				bankList.UpdateToClient(getControllingClient(), iIndex, ITEM_LOSE);
				removeFromTimeItemVec(timeItem->uID);
				continue;
			}

			if(iIndex == -1)
			{
				MessageEvent::send(getControllingClient(), SHOWTYPE_NOTIFY, MSG_ITEM_NOFINDITEM);
				continue;
			}
		}
	}
}

void Player::removeFromTimeItemVec(U64 uId)
{
	for(int i=0; i<mTimeItemVec.size(); ++i)
	{
		if(mTimeItemVec[i]->uID == uId)
		{
			mTimeItemVec.erase_fast(i);
			break;
		}
	}
}

void Player::pushToTimeItemVec(stTimeItem* item)
{
	if(item)
		mTimeItemVec.push_back(item);
}
// ----------------------------------------------------------------------------
// �⴫��
void Player::doTransportObject(int ErrorCode,T_UID UID,int PlayerId,int LineId,int ZoneId,int Ip,int Port)
{
	GameConnection*	pClient = getControllingClient();
	if(!pClient)
		return;

	//// ������Ҵ��ͺ���ܵ��µ�崻�����
	//Player* pPlayer = dynamic_cast<Player*>( pClient->getControlObject() );
	//if( pPlayer ) 
	//	pPlayer->deleteObject();

	//���ʹ�����Ϣ���ͻ���׼������
	TransportEvent* event = new TransportEvent;
	event->setParam(ErrorCode,false,UID,PlayerId,LineId,ZoneId,Ip,Port);
	pClient->postNetEvent(event);

}

void Player::preTransportObject(int LineId,int TriggerId,int MapId,F32 x,F32 y,F32 z)
{
	mTransLineId = LineId;
	mTransTriggerId = TriggerId;
	mTransMapId = MapId;
	mTransX = x;
	mTransY = y;
	mTransZ = z;

	ServerGameNetEvent* pEvent = new ServerGameNetEvent( INFO_ENTER_TRANSPORT );
	GameConnection* pClient = getControllingClient();
	pEvent->SetInt32ArgValues( 1, MapId );
	if( pClient )
		pClient->postNetEvent( pEvent );
}


// ----------------------------------------------------------------------------
// ��������ʹ���
void Player::transportObject(int LineId,int TriggerId,int MapId,F32 x,F32 y,F32 z)
{
	GameConnection*	pClient = getControllingClient();
	if(!pClient)
		return;

#ifndef SWITCH_LINE_DEBUG
	if(isTransporting())		//��ֹ��δ�������
		return;
#endif

	//1.�ж�����Ƿ���ϴ����������Ƿ���Ա�����
	bool bResult = true;

	if(bResult)
	{
		//�ж��Ƿ�Ϊ�ڴ���
		bool isSameMap = false;

		if(g_ServerGameplayState->getZoneId() == MapId && 
			g_ServerGameplayState->getCurrentLineId() == LineId)
		{
			isSameMap = true;
		}

#ifdef SWITCH_LINE_DEBUG
		isSameMap = false;
#endif

		bool isSaveDestInfo = true; //�Ƿ�洢Ŀ����Ϣ����Ϊս��������

		if(isSameMap)
		{
			//�ڴ���
			MatrixF Pos(true);
			Pos.setColumn(3,Point3F(x,y,z));
			setTransform(Pos);
			GameConnection *pCon = getControllingClient();
			if(pCon)	
				pCon->forceMismatch();

			//���ʹ�����Ϣ���ͻ���׼������
			TransportEvent* event = new TransportEvent;
			event->setParam(NONE_ERROR,true);
			pClient->postNetEvent(event);
		}
		else
		{
			//�⴫��
			ServerPacketProcess *pPacket = g_ServerGameplayState->GetPacketProcess();
			if(!pPacket)
				return;

			// �ر�һ�н�����Ϊ
			setInteraction(NULL, INTERACTION_NONE);
			//2.����ڴ��͹����в�������ܶ�֮���������������������һ��״̬����ֹ��Ҵ��͹����б�����			
			setTransporting();

			mCopymapInstId = 1;

			try
			{
				stPlayerStruct* pPlayerData = buildPlayerData();
				pPacket->Send_World_TransportPlayerRequest(mUID,pPlayerData,LineId,TriggerId,MapId,x,y,z);

				// �����¼�
				if(g_ServerGameplayState->getCurrentLineId() != LineId)
					Con::executef("SvrEventSwitchLine", Con::getIntArg(getId()));
				// �����¼�
				if(g_ServerGameplayState->getZoneId() != MapId)
					Con::executef("SvrEventTransport", Con::getIntArg(getId()));
			}
			catch(...)
			{
				setTransporting( false );
			}
		}
	}
	else
	{
		MessageEvent::send(pClient, SHOWTYPE_ERROR, 1001);
	}
}

void Player::testTransportObject()
{
	Point3F pos; 
	getTransform().getColumn(3,&pos);
	transportObject(g_ServerGameplayState->getCurrentLineId(),mTriggerId,g_ServerGameplayState->getZoneId(),pos.x,pos.y,pos.z+10);
}

void Player::sendToWorld()
{
	stPlayerStruct* pPlayer = buildPlayerData();

	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );

	// here we build a fake packet head
	IPacket::BuildPacketHead( sendPacket, 0 );

	int nPos = sendPacket.getPosition();

	pPlayer->WriteData( &sendPacket );

	g_ServerGameplayState->GetDataAgentHelper()->Send( g_ServerGameplayState->getZoneId(), SERVICE_ZONESERVER, sendPacket );

	stPlayerStruct player;
	sendPacket.setPosition( nPos );

	player.ReadData( &sendPacket );
}

void Player::saveToWorld()
{
	mIsSaveDirty = false;
	sendToWorld();

	//stPlayerStruct* pPlayer = buildPlayerData();
	//char buf[MAX_PACKET_SIZE];
	//Base::BitStream stream( buf, MAX_PACKET_SIZE );
	//pPlayer->WriteData( &stream );
	//stream.setPosition( 0 );

	//g_ServerGameplayState->RemovePlayerSaveData( pPlayer->BaseData.PlayerId );
	//
	//stPlayerStruct* pPlayerOld = new stPlayerStruct();
	//pPlayerOld->ReadData( &stream );

	//g_ServerGameplayState->AddPlayerSaveData( pPlayerOld );

	//Con::printf( "SAVE PLAYER POS :%s", pPlayerOld->DispData.LeavePos );

	//g_ServerGameplayState->GetPacketProcess()->Send_World_SavePlayerRequest( mPlayerId );
}

// ----------------------------------------------------------------------------
// ���ͽ�ɫ�������ݸ��ͻ���
bool Player::sendPlayerEvent(U32 mask, bool isFirstUpdate)
{
	GameConnection*	client = getControllingClient();
	if (!client)
		return false;

	PlayerBaseProEvent* event = new PlayerBaseProEvent(mask, isFirstUpdate);
	return client->postNetEvent(event);
}

// ----------------------------------------------------------------------------
// ��ͻ��˸�����ҳ�ʼ������
bool Player::sendInitializEvent()
{
	mIsInitializeEvent = true;
	// ���ͳ�ʼ����ɫ�������ݵ��ͻ���
	sendPlayerEvent(0xFFFFFFFF, true);

	// �����������ݵ��ͻ���
	g_MissionManager->SendInitialize(this);
	g_MissionManager->ContinueTime(this);
	initializeSendSkill();
	initSendLivingSkill();
	initSendPrescription();

	inventoryList.UpdateMaxSlots(getControllingClient());
	bankList.UpdateMaxSlots(getControllingClient());

	inventoryList.SendInitialData(getControllingClient());
	equipList.SendInitialData(getControllingClient());
	panelList.SendInitialData(getControllingClient());

	//����ϵͳ��ʼ������
	PetTable& petTable = (PetTable&)getPetTable();
	petTable.sendInitialData(this);
	//Ԫ��ϵͳ��ʼ������
	mSpiritTable.sendInitialData(this);
	
	// ����ʱ�䴥�������ݵ��ͻ���
	mTimerTriggerMgr->SendInitialData();
	// ���ͳ�������ʱ�䴥�������ݵ��ͻ���
	mTimerPetStudyMgr->SendInitialData();
	pMPManagerInfo->sendEvent(this,0xFFFF,true);
	// ������Ʒ�ֽ����ݵ��ͻ���
	mItemSplitList.sendInitialData(this);
	// ������Ʒ�ֳ����ݵ��ͻ���
	mItemComposeList.sendInitialData(this);
	// ��ʼ��ȴʱ��
	m_CooldownTable.sendInit(getControllingClient());
	// ����������Ϣ
	if(mFlyPath.pathId)
		setFlyPath(mFlyPath.pathId, mFlyPath.mountId, mFlyPath.flyPos);

	sendInitializCompleteEvent();

	// ��ҵ�½���
	Con::executef("SvrEventLogin", Con::getIntArg(getId()));

	return true;
}

// ----------------------------------------------------------------------------
// ��ɫ��ʼ������PACK�������
void Player::sendInitializCompleteEvent()
{
	ServerGameNetEvent* ev = new ServerGameNetEvent(INFO_INITIALIZCOMPLETE);
	if(getControllingClient())
		getControllingClient()->postNetEvent(ev);
}

void Player::initializeSendSkill()
{
	if(getControllingClient()){
		SkillEvent* event = new SkillEvent();
		getControllingClient()->postNetEvent(event);
	}
}
void Player::initSendLivingSkill()
{
	if (getControllingClient())
	{
		LivingSkillEvent* Event = new LivingSkillEvent();
		getControllingClient()->postNetEvent(Event);
	}
}
void Player::initSendPrescription()
{
	if (pPrescription)
		pPrescription->updateToClient(getControllingClient());
}

// ----------------------------------------------------------------------------
// ��ȡ�������������귽��
StringTableEntry Player::getFlagsDate(S32 index)
{
	S32 year, month, day, hour, minute, second;
	U32 var = 0;
	var |= (getFlagsByte(index) << 24);
	var |= (getFlagsByte(index+1) << 16);
	var |= (getFlagsByte(index+2) << 8);
	var |= getFlagsByte(index+3);

	year	= ((var >> 26) & 0x7) + 2010;		// ��
	month	= (var >> 22) & 0xF;				// ��
	day		= (var >> 17) & 0x1F;				// ��
	hour	= (var >> 12) & 0x1F;				// ʱ
	minute	= (var >> 6) & 0x3F;				// ��
	second	=  var & 0x3F;						// ��

	char* returnString = Con::getReturnBuffer( 256);
	dSprintf(returnString, 256, "%d %d %d %d %d %d", year, month, day, hour, minute, second);
	return returnString;
}

// ----------------------------------------------------------------------------
// �����������������귽��
bool Player::setFlagsDate(S32 index, S32 year, S32 month, S32 day, S32 hour, S32 minute, S32 second)
{
	U32 var = 0;										// total: 32bit
	var |= ((U32)(0x7 & (year - 2010)))	<< 26;			// 3bit--��
	var |= ((U32)(0xF & month))			<< 22;			// 4bit--��
	var |= ((U32)(0x1F & day))			<< 17;			// 5bit--��
	var |= ((U32)(0x1F & hour))			<< 12;			// 5bit--Сʱ
	var |= ((U32)(0x3F & minute))		<< 6;			// 6bit--����
	var |= ((U32)(0x3F & second));						// 6bit--��

	setFlagsByte(index, (((var & 0xFF000000) >> 24) & 0xFF));
	setFlagsByte(index + 1, (((var & 0xFF0000) >> 16) & 0xFF));
	setFlagsByte(index + 2, (((var & 0xFF00) >> 8) & 0xFF));
	setFlagsByte(index + 3, (var & 0xFF));
	return true;
}

// ----------------------------------------------------------------------------
// ��ȡĳ����λ�����ֵ
S32 Player::getFlagsByte(S32 index)
{
	if(index >= sizeof(mPlayerDataFlags) || index < 0)
		return 0;
	return (S32)mPlayerDataFlags[index];
}

// ----------------------------------------------------------------------------
// ����ĳ����λ�����ֵ
bool Player::setFlagsByte(S32 index, S32 ibyte)
{
	if(index >= sizeof(mPlayerDataFlags) || index < 0)
		return false;

	mPlayerDataFlags[index] = (U8)(ibyte & 0x000000FF);
	return true;
}

// ----------------------------------------------------------------------------
// �ж�ĳ����λ��bit��״̬
bool Player::getFlagsBit(S32 index, S32 ibit)
{
	if(index >= sizeof(mPlayerDataFlags) || index < 0)
		return false;

	if(ibit > 7 || ibit < 0)
		return false;

	return mPlayerDataFlags[index] & (U8)BIT(ibit);
}
// ----------------------------------------------------------------------------
// ����ĳ����λ��bit��״̬
bool Player::setFlagsBit(S32 index, S32 ibit, bool bFlag)
{
	if(index >= sizeof(mPlayerDataFlags) || index < 0)
		return false;

	if(ibit > 7 || ibit < 0)
		return false;

	if(bFlag)
		mPlayerDataFlags[index] = mPlayerDataFlags[index] | BIT(ibit);
	else
		mPlayerDataFlags[index] = mPlayerDataFlags[index] & (~BIT(ibit));

	return true;
}

// ----------------------------------------------------------------------------
// �Ƚ������������
S32 Player::compareFlagsDate(S32 index, S32 Y, S32 M, S32 D, S32 h, S32 m, S32 s)
{
	if(index >= sizeof(mPlayerDataFlags) || index < 0)
		return 1;

	S32 year, month, day, hour, minute, second;
	U32 var = 0;
	var |= (getFlagsByte(index) << 24);
	var |= (getFlagsByte(index+1) << 16);
	var |= (getFlagsByte(index+2) << 8);
	var |= getFlagsByte(index+3);

	year	= ((var >> 26) & 0x7) + 2010;		// ��
	month	= (var >> 22) & 0xF;				// ��
	day		= (var >> 17) & 0x1F;				// ��
	hour	= (var >> 12) & 0x1F;				// ʱ
	minute	= (var >> 6) & 0x3F;				// ��
	second	=  var & 0x3F;						// ��

	if(year > Y)
		return 1;
	else if(year < Y)
		return -1;

	if(month > M)
		return 1;
	else if(month < M)
		return -1;

	if(day > D)
		return 1;
	else if(day < D)
		return -1;

	if(hour > h)
		return 1;
	else if(hour < h)
		return -1;

	if(minute > m)
		return 1;
	else if(minute < m)
		return -1;

	if(second > s)
		return 1;
	else if(second < s)
		return -1;
	return 0;
}

// ----------------------------------------------------------------------------
// ���浱ǰ����ʱ���������
bool Player::saveFlagsDate(S32 index)
{
	if(index >= sizeof(mPlayerDataFlags) || index < 0)
		return false;

	Platform::LocalTime date;
	Platform::getLocalTime(date);
	return setFlagsDate(index, date.year + 1900, date.month + 1, date.monthday, date.hour, date.min, date.sec);
}

#endif

////////////////////////////////////////////////////////////�ͻ��˴���////////////////////////////////////////////////////////////�ͻ��˴���
////////////////////////////////////////////////////////////�ͻ��˴���////////////////////////////////////////////////////////////�ͻ��˴���

#ifdef NTJ_CLIENT
void Player::onAddToClient()
{
	PLAYER_MGR->AddLocalPlayer( mPlayerId, this );
}

void Player::onRemoveFromClient()
{
	PLAYER_MGR->RemoveLocalPlayer( mPlayerId );
}

bool Player::updateData(Player* pPlayer,Vector<GFXTexHandle> *pTexList,GuiControlProfile** pStyleList,RectF *rect)
{
	bool bUpdate = false;

	if(NULL == pTexList || pTexList->size() < dGuiMouseGamePlay::totalTextureNum -1)
		return bUpdate;

	if(m_IsDirty || GuiCanvas::mGlobalDirty || m_BuffTexture.isNull())
	{
		GuiControlProfile* pStyle = pStyleList[dGuiMouseGamePlay::StyleOne];
		GuiControlProfile* pStyle1 = pStyleList[dGuiMouseGamePlay::StyleTwo];
		if(NULL == pStyle || NULL == pStyle1)
			return bUpdate;

		GFXTexHandle &TeamTexture    = (*pTexList)[dGuiMouseGamePlay::TeamTexture];
		GFXTexHandle &PkstateTexture = (*pTexList)[dGuiMouseGamePlay::PkstateTexture];
		GFXTexHandle &FamilyTexture  = (*pTexList)[dGuiMouseGamePlay::FamilyTexture];

		char strBuff[256] = {0,};
#ifdef DEBUG
		if(pPlayer == this)
		{
			//Ray: ������
			dSprintf(strBuff,sizeof(strBuff),"Ts:%d ,S:%d ,Y:%d, O:%d, DP:%d, Tr:%d, Sd:%d, Sa:%d, Sk:%d, Qr:%d, Rc:%02d",
				gClientSceneGraph->staticNum,
				gClientSceneGraph->getInCameraStatics().size(),
				gClientSceneGraph->getInCameraShadows().size(),
				gClientSceneGraph->getInCameraObjects().size(),
				gClientSceneGraph->getDPtimes(),
				gClientSceneGraph->getTrDPtimes(),
				gClientSceneGraph->getSdDPtimes(),
				gClientSceneGraph->getSaDPtimes(),
				gClientSceneGraph->getSkDPtimes(),
				gClientSceneGraph->getQueryTimes(),
				gClientSceneGraph->getRectCount()
				);

			pStyle1->mFont->SetLineDis(1024);
		}
		else
#endif
		{
			//���
			dSprintf(strBuff,sizeof(strBuff),"%s","<�ųǻ��긫ͷ���>");
		}
		rect->extent.x = pStyle1->mFont->getStrWidthOutline(strBuff);
		rect->extent.y = pStyle1->mFont->getHeightOutline();

		if (dStrcmp(getPlayerName(),""))
		{
			dSprintf(strBuff,sizeof(strBuff),"%s",getPlayerName());
			rect->extent.x = getMax((U32)rect->extent.x,pStyle->mFont->getStrWidthOutline(strBuff));
			rect->extent.y += pStyle->mFont->getHeightOutline();
		}

		if(getPKState())
			rect->extent.x += PkstateTexture.getWidth();

		if(getTeamInfo().m_bInTeam)
			rect->extent.x += TeamTexture.getWidth();

		//if (getFamily() != Family_None)
		//	rect->extent.x += FamilyTexture.getWidth() / (MaxFamilies -1);

		if( !((GFXTextureObject*)m_BuffTexture && m_BuffTexture.getWidth() == rect->extent.x 
			&& m_BuffTexture.getHeight() == rect->extent.y ) )
		{
			m_BuffTexture.set(rect->extent.x,rect->extent.y,GFXFormat_32BIT,&GFXDefaultRenderTargetProfile);
		}

		bUpdate = true;
	}
	else if(m_BuffTexture.getPointer()->setDirty())
	{
		bUpdate = true;
	}

	Point3F screenPos;
	Point3F objectPos;
	objectPos = getRenderPosition(); 
	objectPos.z += getWorldBox().len_z();
	if (g_UIMouseGamePlay->project(objectPos,&screenPos))
	{
		rect->point.x = screenPos.x - rect->extent.x * 0.5f;
		rect->point.y = screenPos.y - (rect->extent.y + dGuiMouseGamePlay::HpOffsetY + 2);
	}

	return bUpdate;
}

//����ͷ������
void Player::drawObjectInfo(GameObject *mainPlayer,void *pParam1,void *pParam2)
{
	Vector<GFXTexHandle> *pTexList = (Vector<GFXTexHandle> *)pParam1;
	if(NULL == pTexList || pTexList->size() < dGuiMouseGamePlay::totalTextureNum -1)
		return;

	GuiControlProfile** pStyleList = (GuiControlProfile **)pParam2;
	Player* control_player = (Player*)mainPlayer; 

	if (NULL == pTexList || NULL == control_player)
		return;

	// ����״̬ && �������ѡ�� ����ʾͷ����Ϣ
	if((m_BuffTable.GetEffect() & BuffData::Effect_Invisibility) && mainPlayer->GetTarget() != this)
		return;

#ifdef DEBUG
	if(control_player == this)
		setDirty(true);
#endif

	if(updateData(control_player,pTexList,pStyleList,&texRect))
	{
		GuiControlProfile* pStyle = pStyleList[dGuiMouseGamePlay::StyleOne];
		GuiControlProfile* pStyle1 = pStyleList[dGuiMouseGamePlay::StyleTwo];
		if(NULL == pStyle || NULL == pStyle1)
			return;

		GFXTexHandle &PkstateTexture     = (*pTexList)[dGuiMouseGamePlay::PkstateTexture];
		GFXTexHandle &FamilyTexture      = (*pTexList)[dGuiMouseGamePlay::FamilyTexture];
		GFXTexHandle &TeamTexture        = (*pTexList)[dGuiMouseGamePlay::TeamTexture];
		GFXTexHandle &TeamCaptainTexture = (*pTexList)[dGuiMouseGamePlay::TeamCaptainTexture];
		GFXTexHandle &TeammateTexture    = (*pTexList)[dGuiMouseGamePlay::TeammateTexture];

		char objectName[COMMON_STRING_LENGTH]  = {0,};
		char factionName[256] = {0,};
		char titleName[COMMON_STRING_LENGTH]   = {0,};

		Point2I objectNameOffset;
		Point2I factionNameOffset;
		Point2I titleNameOffset;

		ColorI objectNameColor(COLORI_PLAYER_ME);
		ColorI factionNameColor(COLORI_FACTION_ME);
		ColorI titleNameColor(COLORI_PLAYER_TITLE);
		U32 teamState = 0; // ����״̬

		GFX->getDrawUtil()->setBlendTyps(GFXDrawUtil::SeparateAlphaBlend);
		GFX->pushActiveRenderTarget();
		GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
		myTarg->attachTexture(GFXTextureTarget::Color0, m_BuffTexture );
		GFX->setActiveRenderTarget(myTarg);
		GFX->clear( GFXClearTarget, ColorI( 0, 0, 0, 0 ), 1.0f, 0 );
		RectI clipRect = GFX->getClipRect();	
		GFX->setClipRect(RectI(0,0,texRect.extent.x,texRect.extent.y));

#ifdef DEBUG
		if(control_player == this)
		{
			//Ray: ������
			dSprintf(factionName,sizeof(factionName),"Ts:%d ,S:%d ,Y:%d, O:%d, DP:%d, Tr:%d, Sd:%d, Sa:%d, Sk:%d, Qr:%d, Rc:%02d",
				gClientSceneGraph->staticNum,
				gClientSceneGraph->getInCameraStatics().size(),
				gClientSceneGraph->getInCameraShadows().size(),
				gClientSceneGraph->getInCameraObjects().size(),
				gClientSceneGraph->getDPtimes(),
				gClientSceneGraph->getTrDPtimes(),
				gClientSceneGraph->getSdDPtimes(),
				gClientSceneGraph->getSaDPtimes(),
				gClientSceneGraph->getSkDPtimes(),
				gClientSceneGraph->getQueryTimes(),
				gClientSceneGraph->getRectCount()
				);
		}
		else
#endif
		{
			//���
			dSprintf(factionName,sizeof(factionName),"%s","<�ųǻ��긫ͷ���>");
		}

		factionNameOffset.x = (texRect.extent.x - pStyle1->mFont->getStrWidthOutline(factionName))*0.5f;
		factionNameOffset.y = texRect.extent.y - pStyle1->mFont->getHeightOutline();

		//�����
		if (dStrcmp(getPlayerName(),"")) 
		{
			dSprintf(objectName,sizeof(objectName),"%s",getPlayerName());
			objectNameOffset.x = pStyle->mFont->getStrWidthOutline(objectName); 
		}
		objectNameOffset.x = (texRect.extent.x - objectNameOffset.x)*0.5f;
		objectNameOffset.y = factionNameOffset.y - pStyle->mFont->getHeightOutline();

		if(isDisabled())
			objectNameColor.set(COLORI_PLAYER_BODY);
		else if(control_player != this && control_player->CanAttackTarget(this))
			objectNameColor.set(COLORI_PLAYER_ENEMY);
		else
			objectNameColor.set(COLORI_PLAYER_ME);

		RectI teamRect;
		RectI pkRect;
		//PK״̬
		if(getPKState())
		{
			//���¼���ƫ��
			objectNameOffset.x = objectNameOffset.x + PkstateTexture.getWidth()*0.5f;

			pkRect.set(objectNameOffset.x - PkstateTexture.getWidth(),objectNameOffset.y,
				PkstateTexture.getWidth(),PkstateTexture.getHeight());

			teamRect.set(objectNameOffset.x-(PkstateTexture.getWidth() + TeamTexture.getWidth()),objectNameOffset.y,
				TeamTexture.getWidth(),TeamTexture.getHeight());

			objectNameColor.set(COLORI_PLAYER_ENEMY);
		}

		//����
		if(getTeamInfo().m_bInTeam)
		{
			if (CLIENT_TEAM->IsTeammate(getPlayerID()))
			{
				if (getTeamInfo().m_bIsCaption)
					teamState = CClientTeam::TeamCaptainState;
				else
					teamState = CClientTeam::TeammateState; 

				if(control_player->getPlayerID() != getPlayerID())
					objectNameColor.set(COLORI_PLAYER_TEAMMATE);
			}
			else
			{
				teamState = CClientTeam::TeamState;
			}
			//���¼���ƫ��
			if(!getPKState())
			{
				objectNameOffset.x = objectNameOffset.x + TeamTexture.getWidth()*0.5f;
				teamRect.set(objectNameOffset.x- TeamTexture.getWidth(),objectNameOffset.y,TeamTexture.getWidth(),TeamTexture.getHeight());
			}
		}

		GFX->getDrawUtil()->clearBitmapModulation();
		if(teamState ==  CClientTeam::TeamCaptainState)
			GFX->getDrawUtil()->drawBitmapStretch(TeamCaptainTexture,teamRect);
		else if (teamState == CClientTeam::TeammateState)
			GFX->getDrawUtil()->drawBitmapStretch(TeammateTexture,teamRect);
		else if (teamState == CClientTeam::TeamState)
			GFX->getDrawUtil()->drawBitmapStretch(TeamTexture,teamRect);

		if(getPKState())
			GFX->getDrawUtil()->drawBitmapStretch(PkstateTexture,pkRect);

		GFX->getDrawUtil()->drawTextOutline(pStyle1->mFont,factionNameOffset,factionName,&objectNameColor,true,pStyle1->mFontOutline);
		GFX->getDrawUtil()->drawTextOutline(pStyle->mFont,objectNameOffset,objectName,&objectNameColor,true,pStyle->mFontOutline);

		//����
		//if (getFamily() != Family_None)
		//{ 
		//	U16 imageWidth = FamilyTexture.getWidth() / (MaxFamilies -1);
		//	RectI srcRect(imageWidth * (getFamily() - 1),0,imageWidth,FamilyTexture.getHeight());
		//	RectI FamilyRect(objectNameOffset.x + pStyle->mFont->getStrWidth(objectName),
		//		objectNameOffset.y,imageWidth,FamilyTexture.getHeight());

		//	GFX->getDrawUtil()->clearBitmapModulation();
		//	GFX->getDrawUtil()->drawBitmapStretchSR(FamilyTexture,FamilyRect,srcRect);
		//}

		GFX->popActiveRenderTarget();
		GFX->setClipRect( clipRect );
		setDirty(false);
	}

	GFX->getDrawUtil()->setBlendTyps(GFXDrawUtil::LerpAlphaBlend);
	GFX->getDrawUtil()->clearBitmapModulation();
	GFX->getDrawUtil()->setZ(mLastZ);
	RectI rect(mCeil(texRect.point.x),mCeil(texRect.point.y),texRect.extent.x,texRect.extent.y);
	GFX->getDrawUtil()->drawBitmapStretch(m_BuffTexture,rect);
	GFX->getDrawUtil()->setZ(0.0f);
}

void Player::drawObjectWord(void *pParam1)
{
	GuiControlProfile** pStyle = (GuiControlProfile **)pParam1;

	if(NULL == getChatInfo() || NULL == pStyle)
		return;

	GuiControlProfile* pProfile = pStyle[dGuiMouseGamePlay::StyleThree];
	if(NULL == pProfile)
		return;

	Point2I iExtent(200,75);
	//���ɹ̶���С����(200X75)
	if(mBuffChatTexture.isNull())
		mBuffChatTexture.set(iExtent.x,iExtent.y,GFXFormat_32BIT,&GFXDefaultRenderTargetProfile);

	if(getChatDirty() || GuiCanvas::mGlobalDirty || mBuffChatTexture.getPointer()->setDirty())
	{
		CRichTextDrawer  textDrawer;
		//�ı���ʽ
		CRichTextDrawer::Style style;
		dStrcpy( style.fontName, sizeof(style.fontName), pProfile->mFontType);
		style.fontSize = pProfile->mFontSize;
		style.fontColor = pProfile->mFontColor;
		style.fontIsBitmap = pProfile->mBitmapFont;
		style.outline = pProfile->mFontOutline;
		textDrawer.setDefaultStyle( style );
		//���ı�Ĭ���и�
		textDrawer.setWidth(iExtent.x - 30);
		textDrawer.setDefaultLineHeight(16);

		GFX->getDrawUtil()->setBlendTyps(GFXDrawUtil::SeparateAlphaBlend);
		GFX->pushActiveRenderTarget();
		GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
		myTarg->attachTexture(GFXTextureTarget::Color0, mBuffChatTexture );
		GFX->setActiveRenderTarget(myTarg);
		GFX->clear( GFXClearTarget, ColorI( 0, 0, 0, 0 ), 1.0f, 0 );
		RectI clipRect = GFX->getClipRect();
		GFX->setClipRect(RectI(0,0,iExtent.x,iExtent.y));

		textDrawer.setContent(getChatInfo());
		U16 sizeHeight = textDrawer.getHeight();
		U16 sizeWidth  = textDrawer.getRealLineWidth();
		//�߿� �ϡ��¡����� �������ı�15��������ʾ��λ
		RectI borderRect;
		borderRect.point.x = 0;
		borderRect.point.y = mClamp(iExtent.y - sizeHeight -30,0,iExtent.y);
		borderRect.extent.x = mClamp(sizeWidth+30,0,iExtent.x);
		borderRect.extent.y =  mClamp(sizeHeight+30,0,iExtent.y);
		renderBorder(borderRect,pProfile);
		//�ı���point(15,10)��ʼ��
		GFX->setClipRect(RectI(0,0,iExtent.x,iExtent.y - 15));
		textDrawer.render(Point2I(15,borderRect.point.y + 10),borderRect);

		GFX->popActiveRenderTarget();
		GFX->setClipRect( clipRect );
		setChatDirty(false);
	}

	GFX->getDrawUtil()->setBlendTyps(GFXDrawUtil::LerpAlphaBlend);
	GFX->getDrawUtil()->clearBitmapModulation();
	GFX->getDrawUtil()->setZ(mLastZ);
	RectI rect(mCeil(texRect.point.x + texRect.extent.x/2),mCeil(texRect.point.y - iExtent.y),iExtent.x,iExtent.y);
	GFX->getDrawUtil()->drawBitmapStretch(mBuffChatTexture,rect);
	GFX->getDrawUtil()->setZ(0.0f);
}


void Player::setPickupPath(PrizeBox *pPrizeBox, PickupType enType)
{
	if (!canInteraction(pPrizeBox))
	{
		//�����Զ������AI��Ŀ����ȥ
		m_enPickUpType = enType;
		mSelPrizeBoxPtr = pPrizeBox;
		SetTarget(pPrizeBox);
		//SetNextAIState(PlayerAISimple::PLAYER_AI_PICKUPSINGLE);
		return;
	}

	//����Ͻ�������ʰȡ�����������
	if (enType == Type_OpenDropList)
	{
		if (pPrizeBox->m_IsBox)
		{
			DropItemEvent::ClientSendOpenDropItemList(pPrizeBox);
		}
		else
		{
			if (pPrizeBox->mCanPickup)
			{
				DropItemEvent::ClientSendPickupPrizeItem(pPrizeBox);
			}				
		}
	}
	else if (enType == Type_AutoPickup)
	{
		DropItemEvent::ClientSendAutoPickup(pPrizeBox);
	}
}

void Player::setCollectionPath(CollectionObject * pCoillectionObj)
{
	if (!pCoillectionObj)
		return;

	mSelCollectionPtr = pCoillectionObj;
	SetTarget(pCoillectionObj);
}

void Player::addToCollectionList(CollectionObject *pCollectionObj)
{
	if (!pCollectionObj)
		return;

	Vector< SimObjectPtr<CollectionObject> >::iterator it = mCollectionList.begin();
	while(it != mCollectionList.end())
	{
		if ( (*it).getObject() == pCollectionObj)
			return;
		++it;
	}
	SimObjectPtr<CollectionObject> pObjPtr = pCollectionObj;
	mCollectionList.push_back(pObjPtr);
}

void Player::deleteFromCollectionList(CollectionObject *pCollectionObj)
{
	if (!pCollectionObj)
		return;

	Vector< SimObjectPtr<CollectionObject> >::iterator it = mCollectionList.begin();
	while(it != mCollectionList.end())
	{
		if ( (*it).getObject() == pCollectionObj)
		{
			mCollectionList.erase(it);
			return;
		}
		++it;
	}
}
void Player::SetStallPath(Point3F stallPos)
{
	Point3F pos = getPosition();
	if (!pos.equal(stallPos))
	{
		mStallPosition = stallPos;
		SetTarget(stallPos, PlayerAISimple::MASK_STALL);
	}
	else
	{
		SetStall();
	}
}

void Player::SetStall()
{
	GameConnection* conn = getControllingClient();
	if(conn)
	{
		ClientGameNetEvent* event = new ClientGameNetEvent(INFO_STALL);
		event->SetInt32ArgValues(3, TRADE_BEGIN, getPlayerID(), stall_individual);
		conn->postNetEvent(event);
	}
}

// ���õ�ǰNPC��ͷ��Ч��(ͷ���ʺŻ�̾��Ч��)
void Player::inNpcScope(GameObject* obj_npc)
{
	if (obj_npc)
	{
		Con::executef(this, "inNpcScope", Con::getIntArg(obj_npc->getId()));
	}
}

ConsoleMethod(Player, SetCollectionPath, void, 3, 3, "%player.SetCollectionPath(%nIndex)")
{
	S32 nIndex = atoi(argv[2]);
	if (nIndex < 0 || nIndex >= object->mCollectionList.size())
		return;

	CollectionObject *pCollectionObj = object->mCollectionList[nIndex].getObject();
	object->setCollectionPath(pCollectionObj);
}
#endif //NTJ_CLIENT