//-----------------------------------------------------------------------------
// 玩家AI
//-----------------------------------------------------------------------------

#include "Gameplay/ai/PlayerAISimple.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/Item/PrizeBox.h"
#include "Gameplay/GameObjects/CollectionObject.h"


//-----------------------------------------------------------------------------
// 玩家AI的有限状态机类
//-----------------------------------------------------------------------------

S32 PlayerAISimple::m_RefCount = 0;
AIStateBase* PlayerAISimple::m_Fsm[] = {};

PlayerAISimple::PlayerAISimple(Player* obj) : AIBase(obj),m_pObj(obj)
{
	m_CurrentAIState = PLAYER_AI_IDLE;
	m_NextAIState = PLAYER_AI_IDLE;

	if(1 == ++m_RefCount)
	{
		m_Fsm[PLAYER_AI_IDLE]		= new AIStateBase(&AIBase::PlayerAISimpleIdle_onEnter, &AIBase::PlayerAISimpleIdle_onExec, &AIBase::PlayerAISimpleIdle_onExit);
		m_Fsm[PLAYER_AI_MOVE]		= new AIStateBase(&AIBase::PlayerAISimpleMove_onEnter, &AIBase::PlayerAISimpleMove_onExec, &AIBase::PlayerAISimpleMove_onExit);
		m_Fsm[PLAYER_AI_FOLLOW]		= new AIStateBase(&AIBase::PlayerAISimpleFollow_onEnter, &AIBase::PlayerAISimpleFollow_onExec, &AIBase::PlayerAISimpleFollow_onExit);
		m_Fsm[PLAYER_AI_CHASE]		= new AIStateBase(&AIBase::PlayerAISimpleChase_onEnter, &AIBase::PlayerAISimpleChase_onExec, &AIBase::PlayerAISimpleChase_onExit);
		m_Fsm[PLAYER_AI_CAST]		= new AIStateBase(&AIBase::PlayerAISimpleCast_onEnter, &AIBase::PlayerAISimpleCast_onExec, &AIBase::PlayerAISimpleCast_onExit);
		m_Fsm[PLAYER_AI_DEATH]		= new AIStateBase(&AIBase::PlayerAISimpleDeath_onEnter, &AIBase::PlayerAISimpleDeath_onExec, &AIBase::PlayerAISimpleDeath_onExit);
	}
	m_pState = m_Fsm[m_CurrentAIState];
}

PlayerAISimple::~PlayerAISimple()
{
	if(0 == --m_RefCount)
	{
		for(int i=0; i<PlayerAISimple::PLAYER_AI_MAX; i++)
			SAFE_DELETE(m_Fsm[i]);
	}
}

void PlayerAISimple::Update()
{
	if(m_CurrentAIState != m_NextAIState)
	{
		RecordTime();
		(this->*(m_pState->onExit))();
		m_CurrentAIState = m_NextAIState;
		m_pState = m_Fsm[m_CurrentAIState];
		(this->*(m_pState->onEnter))();
	}
	if(m_CurrentAIState == m_NextAIState)
	{
		(this->*(m_pState->onExec))();
		if(m_CurrentAIState == m_NextAIState)
			advanceAICharacter();
	}
}


//-----------------------------------------------------------------------------
// 玩家AI状态
//-----------------------------------------------------------------------------


// --------------------------------- Idle -------------------------------------

void PlayerAISimple::PlayerAISimpleIdle_onEnter()
{
}

void PlayerAISimple::PlayerAISimpleIdle_onExec()
{
	if(CheckCastState())
	{
		SetNextAIState(PlayerAISimple::PLAYER_AI_CAST);
		return;
	}
	// 有目标、开启自动攻击、移动为跟随模式时 进入追击状态
	if(CheckChaseState())
	{
		SetNextAIState(PlayerAISimple::PLAYER_AI_CHASE);
		return;
	}
	// 有目标、关闭自动攻击、移动为跟随模式时 进入跟随状态
	if(CheckFollowState())
	{
		SetNextAIState(PlayerAISimple::PLAYER_AI_FOLLOW);
		return;
	}
	// 有新的移动目标
	if (IsNewMove())
	{
		SetNextAIState(PlayerAISimple::PLAYER_AI_MOVE);
		return;
	}
}

void PlayerAISimple::PlayerAISimpleIdle_onExit()
{
}

// --------------------------------- Move -------------------------------------

void PlayerAISimple::PlayerAISimpleMove_onEnter()
{
}

void PlayerAISimple::PlayerAISimpleMove_onExec()
{
	if(CheckCastState())
	{
		SetNextAIState(PlayerAISimple::PLAYER_AI_CAST);
		return;
	}
	if(CheckChaseState())
	{
		SetNextAIState(PlayerAISimple::PLAYER_AI_CHASE);
		return;
	}
	if(CheckFollowState())
	{
		SetNextAIState(PlayerAISimple::PLAYER_AI_FOLLOW);
		return;
	}

	// 更新移动目的地
	if(!RefreshPath_Move())
	{
		if(m_AIMask & MASK_STALL)
			m_pObj->SetStall();
		SetNextAIState(PlayerAISimple::PLAYER_AI_IDLE);
		g_ClientGameplayState->endFindPath();
		return;
	}
}

void PlayerAISimple::PlayerAISimpleMove_onExit()
{
	ClearAIPath();
}

// --------------------------------- Follow -------------------------------------

void PlayerAISimple::PlayerAISimpleFollow_onEnter()
{
}

void PlayerAISimple::PlayerAISimpleFollow_onExec()
{
	// 刷新跟随目标的位置，否则跟随追击
	if(!CheckFollowState() || !RefreshPath_Follow())
	{
		SetNextAIState(PlayerAISimple::PLAYER_AI_IDLE);
		return;
	}

	if(!m_pObj->getVelocity().isZero() || m_pObj->getDistance(m_FlwObj) >= INTERACTIONDISTANCE)
		return;

	// item
	if (m_FlwObj->getType() & ItemObjectType)
	{
		Item* pItem = (Item*)(m_FlwObj.getObject());
		// 拾取物
		if (pItem->getItemMask() & PrizeBoxType)
		{
			PrizeBox* pBox = (PrizeBox*)(pItem);
			if (m_pObj->m_enPickUpType == Player::Type_OpenDropList)
			{
				if (pBox->m_IsBox)
					DropItemEvent::ClientSendOpenDropItemList(pBox);
				else if (pBox->mCanPickup)
					DropItemEvent::ClientSendPickupPrizeItem(pBox);
			}
			else if (m_pObj->m_enPickUpType == Player::Type_AutoPickup)
				DropItemEvent::ClientSendAutoPickup(pBox);
		}
		// 采集物
		else if(pItem->getItemMask() & CollectionObjectType)
		{
			CollectionObject* pCollectionObject = (CollectionObject*)(pItem);
			m_pObj->setCollectionTarget(pCollectionObject);
		}
	}
	// GameObject
	else if (m_FlwObj->getType() & GameObjectType)
	{
		AssertFatal(m_FlwObj == m_pObj->GetTarget(), "");
		GameObject* pObj = (GameObject*)(m_FlwObj.getObject());
		// Player 继续跟随
		if (pObj->getGameObjectMask() & PlayerObjectType)
			return;
		// NPC
		else if ((pObj->getGameObjectMask() & NpcObjectType) && !pObj->isCombative())
		{
			NpcObject* npc = static_cast<NpcObject*>(m_pObj->GetTarget());
			if(m_pObj->canInteraction(npc))
			{
				GameConnection* conn = GameConnection::getConnectionToServer();
				if(conn)
				{
					// 向服务端发送与NPC交互的消息
					ClientGameNetEvent* ev = new ClientGameNetEvent(INFO_QUEST_TRIGGER_NPC);
					ev->SetInt32ArgValues(3, npc->getServerId(), 0, -1);
					conn->postNetEvent(ev);
				}
			}
		}
	}

	// 进入待机状态
	SetNextAIState(PlayerAISimple::PLAYER_AI_IDLE);
}

void PlayerAISimple::PlayerAISimpleFollow_onExit()
{
	SetFollowObject(NULL);
	ClearAIPath();
}

// --------------------------------- Chase -------------------------------------
// 追击状态时，所有的玩家会跑向目标，在设定的合理范围内停住，此时系统会进行自动攻击
void PlayerAISimple::PlayerAISimpleChase_onEnter()
{
}

void PlayerAISimple::PlayerAISimpleChase_onExec()
{
	if(CheckCastState())
	{
		SetNextAIState(PlayerAISimple::PLAYER_AI_CAST);
		return;
	}
	// 刷新追击目标的位置，否则退出追击
	if(!CheckChaseState() || !RefreshPath_Follow())
	{
		SetNextAIState(PlayerAISimple::PLAYER_AI_IDLE);
		return;
	}
}

void PlayerAISimple::PlayerAISimpleChase_onExit()
{
	ClearAIPath();
}

// --------------------------------- Cast -------------------------------------
void PlayerAISimple::PlayerAISimpleCast_onEnter()
{
}

void PlayerAISimple::PlayerAISimpleCast_onExec()
{
	if(!CheckCastState())
	{
		// 与idle一样
		if(CheckChaseState())
		{
			SetNextAIState(PlayerAISimple::PLAYER_AI_CHASE);
			return;
		}
		if(CheckFollowState())
		{
			SetNextAIState(PlayerAISimple::PLAYER_AI_FOLLOW);
			return;
		}
		if (IsNewMove())
		{
			SetNextAIState(PlayerAISimple::PLAYER_AI_MOVE);
			return;
		}
		SetNextAIState(PlayerAISimple::PLAYER_AI_IDLE);
		return;
	}
	// 正在施法
	if(m_pObj->GetSpell().IsSpellRunning())
	{
		m_NewCast = false;
		return;
	}
	if(!RefreshCast())
		ClearAICast();
}

void PlayerAISimple::PlayerAISimpleCast_onExit()
{
	SetFollowObject(NULL);
	ClearAIPath();
}

// --------------------------------- Death -------------------------------------

void PlayerAISimple::PlayerAISimpleDeath_onEnter()
{
}

void PlayerAISimple::PlayerAISimpleDeath_onExec()
{
}

void PlayerAISimple::PlayerAISimpleDeath_onExit()
{
	ClearAIPath();
	ClearMove();
	SetFollowObject(NULL);
}

