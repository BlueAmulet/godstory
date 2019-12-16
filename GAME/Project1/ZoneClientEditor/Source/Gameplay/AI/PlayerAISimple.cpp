//-----------------------------------------------------------------------------
// ���AI
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
// ���AI������״̬����
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
// ���AI״̬
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
	// ��Ŀ�ꡢ�����Զ��������ƶ�Ϊ����ģʽʱ ����׷��״̬
	if(CheckChaseState())
	{
		SetNextAIState(PlayerAISimple::PLAYER_AI_CHASE);
		return;
	}
	// ��Ŀ�ꡢ�ر��Զ��������ƶ�Ϊ����ģʽʱ �������״̬
	if(CheckFollowState())
	{
		SetNextAIState(PlayerAISimple::PLAYER_AI_FOLLOW);
		return;
	}
	// ���µ��ƶ�Ŀ��
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

	// �����ƶ�Ŀ�ĵ�
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
	// ˢ�¸���Ŀ���λ�ã��������׷��
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
		// ʰȡ��
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
		// �ɼ���
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
		// Player ��������
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
					// �����˷�����NPC��������Ϣ
					ClientGameNetEvent* ev = new ClientGameNetEvent(INFO_QUEST_TRIGGER_NPC);
					ev->SetInt32ArgValues(3, npc->getServerId(), 0, -1);
					conn->postNetEvent(ev);
				}
			}
		}
	}

	// �������״̬
	SetNextAIState(PlayerAISimple::PLAYER_AI_IDLE);
}

void PlayerAISimple::PlayerAISimpleFollow_onExit()
{
	SetFollowObject(NULL);
	ClearAIPath();
}

// --------------------------------- Chase -------------------------------------
// ׷��״̬ʱ�����е���һ�����Ŀ�꣬���趨�ĺ���Χ��ͣס����ʱϵͳ������Զ�����
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
	// ˢ��׷��Ŀ���λ�ã������˳�׷��
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
		// ��idleһ��
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
	// ����ʩ��
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

