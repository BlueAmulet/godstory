//-----------------------------------------------------------------------------
// NPCAI
//-----------------------------------------------------------------------------
#pragma once
#include "Gameplay/ai/AIFsm.h"
#include "sceneGraph/pathManager.h"

//-----------------------------------------------------------------------------
// NPC AI������״̬����
//-----------------------------------------------------------------------------

class NpcObject;

class NpcAI: public AIBase
{
public:

	typedef AIBase Parent;

	enum
	{
		NpcTickMax				= 32,	// ����tickִ��һ��ѭ��
	};

	enum NpcAIState
	{
		NPC_AI_NONE,					//״̬����Чֵ(����NPC_AI_NONEС��NPC_AI_MAX)
		NPC_AI_IDLE,					//������������
		NPC_AI_WALK,					//NPC�߶�
		NPC_AI_WAYPOINT,                //NPCѲ��
		NPC_AI_CHASE,                   //NPC׷��
		NPC_AI_CAST,                    //NPC����
		NPC_AI_RUNAWAY,                 //NPC����
		NPC_AI_GOHOME,                  //NPC��ԭ
		NPC_AI_SCARED,                  //NPC�־�
		NPC_AI_DEATH,                   //NPC����

		NPC_AI_MAX,
	};

private:
	static S32				m_RefCount;
	static AIStateBase*		m_Fsm[NPC_AI_MAX];
	static GameObject*		m_SearchTarget;

protected:
	NpcObject*				m_pObj;
	S32						m_Tick;

public:
	NpcAI(NpcObject* obj);
	~NpcAI();

	virtual void		Update();

	static void npcSearchEnemyCallback(SceneObject* sceneObj, void *key);
	bool IsOutRange(Point3F& pos1, Point3F& pos2, F32 len);
	void SearchEnemy();

	// ============================== NpcAI ==============================
	virtual void		NpcAINone_onEnter				();
	virtual void		NpcAINone_onExec				();
	virtual void		NpcAINone_onExit				();

	virtual void		NpcAIIdle_onEnter				();
	virtual void		NpcAIIdle_onExec				();
	virtual void		NpcAIIdle_onExit				();

	virtual void		NpcAIWalk_onEnter				();
	virtual void		NpcAIWalk_onExec				();
	virtual void		NpcAIWalk_onExit				();

	virtual void		NpcAIWayPoint_onEnter			();
	virtual void		NpcAIWayPoint_onExec			();
	virtual void		NpcAIWayPoint_onExit			();

	virtual void		NpcAIChase_onEnter				();
	virtual void		NpcAIChase_onExec				();
	virtual void		NpcAIChase_onExit				();

	virtual void		NpcAICast_onEnter				();
	virtual void		NpcAICast_onExec				();
	virtual void		NpcAICast_onExit				();

	virtual void		NpcAIRunAway_onEnter			();
	virtual void		NpcAIRunAway_onExec				();
	virtual void		NpcAIRunAway_onExit				();

	virtual void		NpcAIGoHome_onEnter				();
	virtual void		NpcAIGoHome_onExec				();
	virtual void		NpcAIGoHome_onExit				();

	virtual void		NpcAIScared_onEnter				();
	virtual void		NpcAIScared_onExec				();
	virtual void		NpcAIScared_onExit				();

	virtual void		NpcAIDeath_onEnter				();
	virtual void		NpcAIDeath_onExec				();
	virtual void		NpcAIDeath_onExit				();
};

