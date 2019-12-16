//-----------------------------------------------------------------------------
// 宠物AI
//-----------------------------------------------------------------------------
#pragma once
#include "Gameplay/ai/AIFsm.h"

//-----------------------------------------------------------------------------
// 宠物AI的有限状态机类
//-----------------------------------------------------------------------------

class PetObject;

class PetAI: public AIBase
{
public:
	enum PetAIState
	{
		PET_AI_NONE,
		PET_AI_IDLE,
		PET_AI_FOLLOW,
		PET_AI_CHASE,
		PET_AI_CAST,
		PET_AI_SCARED,
		PET_AI_DEATH,

		PET_AI_MAX,
	};

private:
	static S32		m_RefCount;
	static AIStateBase*	m_Fsm[PET_AI_MAX];

protected:
	PetObject*		m_pObj;

public:
	PetAI(PetObject* obj);
	~PetAI();

	virtual void	Update();

	// ============================== PetAI ==============================
	virtual void		PetAINone_onEnter				();
	virtual void		PetAINone_onExec				();
	virtual void		PetAINone_onExit				();

	virtual void		PetAIIdle_onEnter				();
	virtual void		PetAIIdle_onExec				();
	virtual void		PetAIIdle_onExit				();

	virtual void		PetAIFollow_onEnter				();
	virtual void		PetAIFollow_onExec				();
	virtual void		PetAIFollow_onExit				();

	virtual void		PetAIChase_onEnter				();
	virtual void		PetAIChase_onExec				();
	virtual void		PetAIChase_onExit				();

	virtual void		PetAICast_onEnter				();
	virtual void		PetAICast_onExec				();
	virtual void		PetAICast_onExit				();

	virtual void		PetAIScared_onEnter				();
	virtual void		PetAIScared_onExec				();
	virtual void		PetAIScared_onExit				();

	virtual void		PetAIDeath_onEnter				();
	virtual void		PetAIDeath_onExec				();
	virtual void		PetAIDeath_onExit				();
};

