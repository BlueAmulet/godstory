//-----------------------------------------------------------------------------
// 玩家AI
//-----------------------------------------------------------------------------
#pragma once
#include "Gameplay/ai/AIFsm.h"
#include "sceneGraph/sceneObject.h"

//-----------------------------------------------------------------------------
// 玩家简单AI的有限状态机类
//-----------------------------------------------------------------------------

class PlayerAISimple: public AIBase
{
public:
	enum PlayerAIState
	{
		PLAYER_AI_IDLE,
		PLAYER_AI_MOVE,
		PLAYER_AI_FOLLOW,
		PLAYER_AI_CHASE,
		PLAYER_AI_CAST,
		PLAYER_AI_DEATH,

		PLAYER_AI_MAX,
	};

	enum PlayerAIMask
	{
		MASK_STALL				= BIT(0),
	};

private:
	static S32		m_RefCount;
	static AIStateBase*	m_Fsm[PLAYER_AI_MAX];

protected:
	Player*			m_pObj;

public:
	PlayerAISimple(Player* obj);
	~PlayerAISimple();

	virtual void	Update();

	// ============================== PlayerAISimple ==============================
	virtual void		PlayerAISimpleIdle_onEnter		();
	virtual void		PlayerAISimpleIdle_onExec		();
	virtual void		PlayerAISimpleIdle_onExit		();

	virtual void		PlayerAISimpleMove_onEnter		();
	virtual void		PlayerAISimpleMove_onExec		();
	virtual void		PlayerAISimpleMove_onExit		();

	virtual void		PlayerAISimpleFollow_onEnter	();
	virtual void		PlayerAISimpleFollow_onExec		();
	virtual void		PlayerAISimpleFollow_onExit		();

	virtual void		PlayerAISimpleChase_onEnter		();
	virtual void		PlayerAISimpleChase_onExec		();
	virtual void		PlayerAISimpleChase_onExit		();

	virtual void		PlayerAISimpleCast_onEnter		();
	virtual void		PlayerAISimpleCast_onExec		();
	virtual void		PlayerAISimpleCast_onExit		();

	virtual void		PlayerAISimpleDeath_onEnter		();
	virtual void		PlayerAISimpleDeath_onExec		();
	virtual void		PlayerAISimpleDeath_onExit		();
};

