//-----------------------------------------------------------------------------
// ����AI
//-----------------------------------------------------------------------------

#include "Gameplay/ai/PetAI.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/GameObjects/PetObject.h"
#include "Gameplay/GameObjects/PlayerObject.h"

//-----------------------------------------------------------------------------
// ����AI������״̬����
//-----------------------------------------------------------------------------

S32 PetAI::m_RefCount = 0;
AIStateBase* PetAI::m_Fsm[] = {};

PetAI::PetAI(PetObject* obj) : AIBase(obj), m_pObj(obj)
{
	m_CurrentAIState = PET_AI_NONE;
	m_NextAIState = PET_AI_NONE;

	if(1 == ++m_RefCount)
	{
		m_Fsm[PET_AI_NONE]		= new AIStateBase(&AIBase::PetAINone_onEnter, &AIBase::PetAINone_onExec, &AIBase::PetAINone_onExit);
		m_Fsm[PET_AI_IDLE]		= new AIStateBase(&AIBase::PetAIIdle_onEnter, &AIBase::PetAIIdle_onExec, &AIBase::PetAIIdle_onExit);
		m_Fsm[PET_AI_FOLLOW]	= new AIStateBase(&AIBase::PetAIFollow_onEnter, &AIBase::PetAIFollow_onExec, &AIBase::PetAIFollow_onExit);
		m_Fsm[PET_AI_CHASE]		= new AIStateBase(&AIBase::PetAIChase_onEnter, &AIBase::PetAIChase_onExec, &AIBase::PetAIChase_onExit);
		m_Fsm[PET_AI_CAST]		= new AIStateBase(&AIBase::PetAINone_onEnter, &AIBase::PetAINone_onExec, &AIBase::PetAINone_onExit);
		m_Fsm[PET_AI_SCARED]	= new AIStateBase(&AIBase::PetAINone_onEnter, &AIBase::PetAINone_onExec, &AIBase::PetAINone_onExit);
		m_Fsm[PET_AI_DEATH]		= new AIStateBase(&AIBase::PetAIDeath_onEnter, &AIBase::PetAIDeath_onExec, &AIBase::PetAIDeath_onExit);
	}
	m_pState = m_Fsm[m_CurrentAIState];
}

PetAI::~PetAI()
{
	if(0 == --m_RefCount)
	{
		for(int i=0; i<PetAI::PET_AI_MAX; i++)
			SAFE_DELETE(m_Fsm[i]);
	}
}

void PetAI::Update()
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
// ����AI״̬
//-----------------------------------------------------------------------------

// --------------------------------- None -------------------------------------

void PetAI::PetAINone_onEnter()
{
}

void PetAI::PetAINone_onExec()
{
	SetNextAIState(PetAI::PET_AI_IDLE);
}

void PetAI::PetAINone_onExit()
{
}

// --------------------------------- Idle -------------------------------------


void PetAI::PetAIIdle_onEnter()
{
}

void PetAI::PetAIIdle_onExec()
{
	SetNextAIState(PetAI::PET_AI_FOLLOW);
}

void PetAI::PetAIIdle_onExit()
{
}

// --------------------------------- Follow -------------------------------------

void PetAI::PetAIFollow_onEnter()
{
}

void PetAI::PetAIFollow_onExec()
{
	if(m_pObj->getMaster()->IsAutoCastEnable())
		m_pObj->SetTarget(m_pObj->getMaster()->GetTarget());
	else
		m_pObj->SetTarget(NULL);
	if(CheckChaseState())
	{
		SetNextAIState(PetAI::PET_AI_CHASE);
		return;
	}

	// ����ĸ����Ϊ���⣬����Ӧȡ�ó����������꣬Ȼ���ƶ��������겢��������
	static Point3F diff, petPos;
	static bool empty, reach;
	// ȡ�ó����������꣬ʵ������
	petPos = m_pObj->getMaster()->getPetPos();
	diff = petPos - (GetAIPath()->IsEmpty() ? m_pObj->getPosition() : GetAIPath()->GetEnd());
	// ��ǰ�Ƿ���Ѱ������ǰ�Ƿ���Ҫ����Ѱ��
	empty = GetAIPath()->IsEmpty();
	reach = mFabs(diff.x) < AIBase::cReachDiff && mFabs(diff.y) < AIBase::cReachDiff;

	if(empty && reach)
	{
		if(mFabs(m_pObj->getRotation().z - m_pObj->getMaster()->getRotation().z) > 0.05f)
			m_pObj->setRotation(m_pObj->getMaster()->getRotation().z, true);
	}
	else if(empty && !reach)
	{
		m_pObj->SetTarget(petPos);
	}
	else if(!empty && !reach)
	{
		static Point3F diff2;
		diff2 = petPos - m_pObj->getPosition();
		if((mFabs(diff.x) + mFabs(diff.y))*3 > mFabs(diff2.x) + mFabs(diff2.y))
			m_pObj->SetTarget(petPos);
	}

	if(!RefreshPath_Move())
	{
		if(!reach)
			m_pObj->setTransform(m_pObj->getMaster()->getTransform());
		SetNextAIState(PetAI::PET_AI_IDLE);
		return;
	}
}

void PetAI::PetAIFollow_onExit()
{
	ClearAIPath();
}

// --------------------------------- Chase -------------------------------------

void PetAI::PetAIChase_onEnter()
{
}

void PetAI::PetAIChase_onExec()
{
	// ˢ��׷��Ŀ���λ�ã������˳�׷��
	if(!CheckChaseState() || !RefreshPath_Follow() || !m_pObj->getMaster()->IsAutoCastEnable())
	{
		m_pObj->mHateList.reset();
		SetNextAIState(PetAI::PET_AI_FOLLOW);
		return;
	}
}

void PetAI::PetAIChase_onExit()
{
	ClearAIPath();
}

// --------------------------------- Cast -------------------------------------

void PetAI::PetAICast_onEnter()
{
}

void PetAI::PetAICast_onExec()
{
}

void PetAI::PetAICast_onExit()
{
}

// --------------------------------- Scared -------------------------------------

void PetAI::PetAIScared_onEnter()
{
}

void PetAI::PetAIScared_onExec()
{
}

void PetAI::PetAIScared_onExit()
{
}

// --------------------------------- Death -------------------------------------

void PetAI::PetAIDeath_onEnter()
{
	ClearAIPath();
	ClearMove();
	SetFollowObject(NULL);
}

void PetAI::PetAIDeath_onExec()
{
	// ɾ���Լ�
	if(m_EnterTime + 5000 < Platform::getVirtualMilliseconds())
	{
		m_pObj->safeDeleteObject();
	}
}

void PetAI::PetAIDeath_onExit()
{
}

