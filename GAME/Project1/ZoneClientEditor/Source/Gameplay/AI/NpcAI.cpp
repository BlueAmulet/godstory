//-----------------------------------------------------------------------------
// NPCAI
//-----------------------------------------------------------------------------

#include "Gameplay/ai/NpcAI.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/GameObjects/PlayerObject.h"
//-----------------------------------------------------------------------------
// NPC AI的有限状态机类
//-----------------------------------------------------------------------------
#define NPC_CAST_TIMEOUT 20000

S32 NpcAI::m_RefCount = 0;
AIStateBase* NpcAI::m_Fsm[] = {};
GameObject* NpcAI::m_SearchTarget = NULL;

NpcAI::NpcAI(NpcObject* obj) : AIBase(obj),m_pObj(obj)
{
	m_CurrentAIState = NPC_AI_NONE;
	m_NextAIState = NPC_AI_NONE;

	m_Tick = m_RefCount % NpcTickMax;
	if(1 == ++m_RefCount)
	{
		m_Fsm[NPC_AI_NONE]		= new AIStateBase(&AIBase::NpcAINone_onEnter, &AIBase::NpcAINone_onExec, &AIBase::NpcAINone_onExit);
		m_Fsm[NPC_AI_IDLE]		= new AIStateBase(&AIBase::NpcAIIdle_onEnter, &AIBase::NpcAIIdle_onExec, &AIBase::NpcAIIdle_onExit);
		m_Fsm[NPC_AI_WALK]		= new AIStateBase(&AIBase::NpcAIWalk_onEnter, &AIBase::NpcAIWalk_onExec, &AIBase::NpcAIWalk_onExit);
		m_Fsm[NPC_AI_WAYPOINT]	= new AIStateBase(&AIBase::NpcAIWayPoint_onEnter, &AIBase::NpcAIWayPoint_onExec, &AIBase::NpcAIWayPoint_onExit);
		m_Fsm[NPC_AI_CHASE]		= new AIStateBase(&AIBase::NpcAIChase_onEnter, &AIBase::NpcAIChase_onExec, &AIBase::NpcAIChase_onExit);
		m_Fsm[NPC_AI_CAST]		= new AIStateBase(&AIBase::NpcAICast_onEnter, &AIBase::NpcAICast_onExec, &AIBase::NpcAICast_onExit);
		m_Fsm[NPC_AI_RUNAWAY]	= new AIStateBase(&AIBase::NpcAIRunAway_onEnter, &AIBase::NpcAIRunAway_onExec, &AIBase::NpcAIRunAway_onExit);
		m_Fsm[NPC_AI_GOHOME]	= new AIStateBase(&AIBase::NpcAIGoHome_onEnter, &AIBase::NpcAIGoHome_onExec, &AIBase::NpcAIGoHome_onExit);
		m_Fsm[NPC_AI_SCARED]	= new AIStateBase(&AIBase::NpcAIScared_onEnter, &AIBase::NpcAIScared_onExec, &AIBase::NpcAIScared_onExit);
		m_Fsm[NPC_AI_DEATH]		= new AIStateBase(&AIBase::NpcAIDeath_onEnter, &AIBase::NpcAIDeath_onExec, &AIBase::NpcAIDeath_onExit);
	}
	m_pState = m_Fsm[m_CurrentAIState];
}

NpcAI::~NpcAI()
{
	if(0 == --m_RefCount)
	{
		for(int i=0; i<NpcAI::NPC_AI_MAX; i++)
			SAFE_DELETE(m_Fsm[i]);
	}
}

void NpcAI::Update()
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

void NpcAI::npcSearchEnemyCallback(SceneObject* sceneObj, void *key)
{
	if((sceneObj->getType() & GameObjectType) && sceneObj != key)
	{
		if(((GameObject*)key)->CanAttackTarget(((GameObject*)sceneObj)))
		{
			m_SearchTarget = (GameObject*)sceneObj;
		}
	}
}

void NpcAI::SearchEnemy()
{
	static Box3F box;
	static Point3F pos;

	// 先要清除结果
	m_SearchTarget = NULL;
	if(m_Tick != (gServerProcessList.getTotalTicks() % NpcTickMax))
		return;

	F32 visualField = ((NpcObjectData*)m_pObj->getDataBlock())->visualField;
	pos = m_pObj->getPosition();
	box.min.x = pos.x - visualField;
	box.min.y = pos.y - visualField;
	box.min.z = pos.z - visualField;
	box.max.x = pos.x + visualField;
	box.max.y = pos.y + visualField;
	box.max.z = pos.z + visualField;

	// 副本的怪搜索副本层，其他的怪搜索1层
	gServerContainer.findObjects(box, GameObjectType, npcSearchEnemyCallback, m_pObj, m_pObj->getLayerID() ? m_pObj->getLayerID() : 1);
}

//-----------------------------------------------------------------------------
// NPC AI状态
//-----------------------------------------------------------------------------

// --------------------------------- None -------------------------------------

void NpcAI::NpcAINone_onEnter()
{
}

void NpcAI::NpcAINone_onExec()
{
	SetNextAIState(NpcAI::NPC_AI_IDLE);
}

void NpcAI::NpcAINone_onExit()
{
	// 计算得到一个随机的位置
	if(isZero(m_pObj->m_InitialPosition.x))
		m_pObj->m_InitialPosition.set(m_pObj->getPosition());
}

// --------------------------------- Idle -------------------------------------


void NpcAI::NpcAIIdle_onEnter()
{
}

void NpcAI::NpcAIIdle_onExec()
{
	if(m_pObj->mHateList.isInCombat())
	{
		SetPrevPosition(m_pObj->getPosition());
		SetNextAIState(NpcAI::NPC_AI_CHASE);
		return;
	}
	if(((NpcObjectData*)m_pObj->getDataBlock())->isFlags(NpcObjectData::Flags_Active))
	{
		SearchEnemy();
		if(m_SearchTarget)
		{
			m_pObj->mHateList.addHate(m_SearchTarget, 1);
			SetPrevPosition(m_pObj->getPosition());
			SetNextAIState(NpcAI::NPC_AI_CHASE);
			return;
		}
	}

	U32  t_WayPointID  = m_pObj->m_WayPointID;
	//
	if(gServerPathManager->isValidPath(t_WayPointID))
	{
		SetNextAIState(NpcAI::NPC_AI_WAYPOINT);
		return;
	}
	else if(m_pObj->canWalk() && !IsIdleAnim(m_pObj->getUpperAction()) && (mRandF(0.0f, 1.0f) < 0.032f))
	{
		SetNextAIState(NpcAI::NPC_AI_WALK);
		return;
	}
}

void NpcAI::NpcAIIdle_onExit()
{
}

// --------------------------------- Walk -------------------------------------

void NpcAI::NpcAIWalk_onEnter()
{
	F32 x = mRandF(-1.0f, 1.0f);
	F32 y = mRandF(-1.0f, 1.0f);
	Point3F MoveDest;
	MoveDest.set(x,y,0);
	MoveDest.normalize(m_pObj->m_MovementAreaRadius_FreeMove * mRandF(0.0f, 1.0f));
	// 得到真实的位置
	MoveDest += m_pObj->m_InitialPosition;
	// 寻径
	SetMove(MoveDest);
	if(!RefreshPath_Move())
	{
		SetNextAIState(NpcAI::NPC_AI_IDLE);
		return;
	}

	// 设置速度为步行速度
	m_pObj->setWalking(true);
}

void NpcAI::NpcAIWalk_onExec()
{
	if(m_pObj->mHateList.isInCombat())
	{
		SetPrevPosition(m_pObj->getPosition());
		SetNextAIState(NpcAI::NPC_AI_CHASE);
		return;
	}
	if(((NpcObjectData*)m_pObj->getDataBlock())->isFlags(NpcObjectData::Flags_Active))
	{
		SearchEnemy();
		if(m_SearchTarget)
		{
			m_pObj->mHateList.addHate(m_SearchTarget, 1);
			SetPrevPosition(m_pObj->getPosition());
			SetNextAIState(NpcAI::NPC_AI_CHASE);
			return;
		}
	}
	if(!RefreshPath_Move())
	{
		SetNextAIState(NpcAI::NPC_AI_IDLE);
	}
}

void NpcAI::NpcAIWalk_onExit()
{
	// 设置速度为跑步速度
	m_pObj->setWalking(false);
}

// --------------------------------- WayPoint -------------------------------------

void NpcAI::NpcAIWayPoint_onEnter()
{
	//初始化waypointd的信息,并判断是否有效
	//U32  t_WayPointID = obj->m_WayPointID;
	U32  t_WayPointID  = m_pObj->m_WayPointID;
	m_nSeqIndex = 0;
	//
	if(!gServerPathManager->isValidPath(t_WayPointID))
	{
		Con::printf("error PathID !");
		SetNextAIState(NpcAI::NPC_AI_IDLE);
		return;
	}
}

void NpcAI::NpcAIWayPoint_onExec()
{
	if(m_pObj->mHateList.isInCombat())
	{
		SetPrevPosition(m_pObj->getPosition());
		SetNextAIState(NpcAI::NPC_AI_CHASE);
		return;
	}

	U32  t_WayPointID = 0;
	t_WayPointID = m_pObj->m_WayPointID;
	if(m_nSeqIndex >=  gServerPathManager->getPathNumWaypoints(t_WayPointID))
	{
		//往返和循环对setp的初始化是有区别的
		/*
		if(循环)
			obj->m_nSeqIndex = 0; 
		else(往返)
			-- obj->m_nSeqIndex;
		*/
		m_nSeqIndex= 0;
	}

	Point3F MoveDest;
	U32     msToNext;  //停留时间

   /* if(obj->m_pAI->m_nSeqIndex != 0)
	{
		if( (!obj->m_pAI->IsReachDest()) || (obj->m_pAI->GetAIPath()->IsEmpty() ))
			return;
	}*/
	
	// 是否到达了某个路点
	if(!RefreshPath_Move())
	{
		// to do:停留时间

		// 得到路径中下一个路点的坐标
		m_nSeqIndex ++;
		if(!gServerPathManager->GetPathPostionBySeq(t_WayPointID,m_nSeqIndex,MoveDest,msToNext))
		{
			Con::printf("error MoveDest !");
			SetNextAIState(NpcAI::NPC_AI_IDLE);
			return;
		}
		// 寻径至下个路点
		SetMove(MoveDest);
		if(!RefreshPath_Move())
		{
			SetNextAIState(NpcAI::NPC_AI_IDLE);
			return;
		}
	}
}

void NpcAI::NpcAIWayPoint_onExit()
{

	Con::printf("End Waypostion !");
}

// --------------------------------- Chase -------------------------------------

void NpcAI::NpcAIChase_onEnter()
{
	GameObject* target = m_pObj->mHateList.update(true);
	if(!target)
	{
		SetNextAIState(NpcAI::NPC_AI_GOHOME);
		return;
	}
	m_pObj->SetTarget(target);
	// 攻击距离
	SetFollowObject(target, m_pObj->GetFollowRange());
}

void NpcAI::NpcAIChase_onExec()
{
	if(m_Tick == (gServerProcessList.getTotalTicks() % NpcTickMax))
		advanceAISpell();
	if(CheckCastState())
	{
		SetNextAIState(NpcAI::NPC_AI_CAST);
		return;
	}

	// 追击距离的检测
	static Point3F diff;
	diff = m_pObj->getPosition() - GetPrevPosition();
	if(diff.lenSquared() > m_pObj->m_MovementAreaRadius_Trace * m_pObj->m_MovementAreaRadius_Trace)
		m_pObj->mHateList.setTimeLimitEnabled(true);

	// 没有目标的时候强制刷新
	m_pObj->SetTarget(m_pObj->mHateList.update(!m_pObj->GetTarget()));

	if(!CheckChaseState() || !RefreshPath_Follow())
	{
		SetNextAIState(NpcAI::NPC_AI_GOHOME);
		return;
	}
}

void NpcAI::NpcAIChase_onExit()
{
	m_pObj->mHateList.setTimeLimitEnabled(false);
	ClearAIPath();
}

// --------------------------------- Cast -------------------------------------

void NpcAI::NpcAICast_onEnter()
{
}

void NpcAI::NpcAICast_onExec()
{
	if(!CheckCastState())
	{
		if(CheckChaseState())
		{
			SetNextAIState(NpcAI::NPC_AI_CHASE);
			return;
		}
		if (IsNewMove())
		{
			SetNextAIState(NpcAI::NPC_AI_WALK);
			return;
		}
		SetNextAIState(NpcAI::NPC_AI_GOHOME);
		return;
	}
	// 正在施法
	if(m_pObj->GetSpell().IsSpellRunning())
	{
		m_NewCast = false;
		return;
	}
	// 超时
	else if(m_EnterTime + NPC_CAST_TIMEOUT < Platform::getVirtualMilliseconds())
	{
		ClearAICast();
	}
	if(!RefreshCast())
		ClearAICast();
}

void NpcAI::NpcAICast_onExit()
{
	SetFollowObject(NULL);
	ClearAIPath();
}

// --------------------------------- RunAway -------------------------------------

void NpcAI::NpcAIRunAway_onEnter()
{
	m_pObj->SetAutoCastEnable(false);
	SetRunPath();
	Con::printf("NpcAIRunAway_onEnter");
}

void NpcAI::NpcAIRunAway_onExec()
{
	if(!RefreshPath_Move())
	{
		SetRunPath();
	}
}

void NpcAI::NpcAIRunAway_onExit()
{
	m_pObj->SetTarget(m_pObj->mHateList.update());
	m_pObj->SetAutoCastEnable(true);
	Con::printf("NpcAIRunAway_onExit");
}

// --------------------------------- GoHome -------------------------------------

void NpcAI::NpcAIGoHome_onEnter()
{
	SetMove(GetPrevPosition());
	m_pObj->onReset();
}

void NpcAI::NpcAIGoHome_onExec()
{
	if (!RefreshPath_Move())
	{
		SetNextAIState(NpcAI::NPC_AI_IDLE);
	}
}

void NpcAI::NpcAIGoHome_onExit()
{
	m_pObj->onReset();
	ClearAIPath();
	ClearMove();
	SetFollowObject(NULL);
	m_pObj->setRotation(m_pObj->m_InitialRotation.z);
}

// --------------------------------- Scared -------------------------------------

void NpcAI::NpcAIScared_onEnter()
{
}

void NpcAI::NpcAIScared_onExec()
{
}

void NpcAI::NpcAIScared_onExit()
{
}

// --------------------------------- Death -------------------------------------

void NpcAI::NpcAIDeath_onEnter()
{
	Player* player = m_pObj->getBelongTo();
	if(player)
	{
		// 需要处理归属者及归属者其队友的声望、经验、物品掉落
		player->killNpc(m_pObj);
		m_pObj->setBelongTo(NULL);
	}

	// 设置复活时间
	m_pObj->SetRiseTime();

	// 死亡后清除仇恨等信息
	m_pObj->mHateList.reset();
	ClearAIPath();
	ClearMove();
	SetFollowObject(NULL);
	m_pObj->SetTarget(NULL);
}

void NpcAI::NpcAIDeath_onExec()
{
	// 复活计时
	if(m_pObj->CheckRiseTime())
	{
		m_pObj->setPosition(m_pObj->m_InitialPosition, m_pObj->canWalk() ? m_pObj->getRotation() : m_pObj->m_InitialRotation);
		m_pObj->setMaskBits(GameObject::MoveMask);
		m_pObj->setDamageState(ShapeBase::Enabled);
		SetNextAIState(NpcAI::NPC_AI_IDLE);
	}
}

void NpcAI::NpcAIDeath_onExit()
{
	m_pObj->onSpawn();
}

