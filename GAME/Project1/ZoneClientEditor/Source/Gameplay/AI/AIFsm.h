//-----------------------------------------------------------------------------
// AI的有限状态机类
//-----------------------------------------------------------------------------

#pragma once
#include "T3D/shapeBase.h"
#include "T3D/gameProcess.h"
#include "Gameplay/ai/AIPath.h"
#include "Gameplay/ai/HateList.h"
#include "Gameplay/GameObjects/Spell.h"
#include "Gameplay/ai/AIData.h"

struct Move;
class AIStateBase;
class GameObject;
class SkillData;


//-----------------------------------------------------------------------------
// AI的有限状态机基础类，类似事件形式
//-----------------------------------------------------------------------------

class AIFsm
{
protected:
	S32				m_CurrentAIState;  
	S32				m_NextAIState;
	AIStateBase*	m_pState;

public:
	AIFsm();
	
	virtual ~AIFsm();
	virtual void	Update();

	S32				GetCurrentAIState()					{return m_CurrentAIState;}
	S32				GetNextAIState()					{return m_NextAIState;}
	void			SetNextAIState(S32 state)			{m_NextAIState = state;}
};


//-----------------------------------------------------------------------------
// AI基础类，管理AI相关的数据和方法
//-----------------------------------------------------------------------------

class AIBase: public AIFsm
{
public:
	const static F32	cReachDiff;
	Point3F				m_StepDest;
	Point3F				m_Move;

	// 
	U32                 m_WayPointID;			// 路径ID
	U32					m_nSeqIndex;			// 路径结点ID
protected:
	GameObject*			m_pObj;
	SimObjectPtr<SceneObject> m_FlwObj;
	U32					m_AIMask;
	CAIPath				m_AiPath;
	SimTime				m_EnterTime;			// 进入本AI状态的时间
	SimTime				m_ExitTime[AICharacter::MaxAIStates];			// 离开AI状态的时间
	const AICharacter*	m_AICharacter;			// 性格
	const AISpell*		m_AISpell;				// 技能

	bool				m_NewMove;
	F32					m_FollowLenSquared;		// 跟随的距离的平方，小于该伐值就不会移动
	Point3F				m_prevPosition;			// 上一个AI状态中对象的位置

	SkillData*			m_pSkillData;			// 需要施放的技能
	bool				m_NewCast;				// 需要施放法术
	bool				m_bTarget;				// 施放的对象是否为目标对象
	bool				m_bTargetSelf;			// 施放的对象是否为自己
	Point3F				m_CastDest;				// 技能释放目标
	F32					m_SkillRangeMinSq;		// 技能释放距离最小值的平方
	F32					m_SkillRangeMaxSq;		// 技能释放距离最大值的平方
	U32					m_ItemType;				// 物品类型
	U32					m_ItemIdx;				// 物品编号
	U64					m_ItemUID;				// 物品UID
public:
	AIBase(GameObject* obj);
	~AIBase();

	void				RecordTime				();
	CAIPath*			GetAIPath				() { return &m_AiPath;}
	void				ClearAIPath				();
	// 移动相关
	bool				IsFollowing				();
	bool				CheckFollowState		();
	bool				CheckChaseState			();
	bool				CheckCastState			();

	void				SetFollowObject			(SceneObject* obj, F32 len=2, U32 mask=0);
	bool				IsReachDest				();
	bool				GetNextMove				(Move &curMove);
	void				SetStepDest				(Point3F& pos);

	Point3F&			GetPrevPosition			()	{ return m_prevPosition; }
	void				SetPrevPosition			(Point3F& pos) { m_prevPosition = pos; }

	inline void			SetMove					(Point3F& move, U32 mask = 0) { m_Move = move; m_NewMove = true; SetFollowObject(NULL); m_AIMask = mask;}
	inline void			ClearMove				() { m_NewMove = false;}
	inline bool			IsNewMove				() { return m_NewMove;}
	void				SetRunPath				();
	bool				RefreshPath_Move		();
	bool				RefreshPath_Follow		();
	bool				RefreshCast				();

	void				SetAICast				(Spell::PreSpell& pre, F32 min, F32 max, bool self, const Point3F* pos);
	void				ClearAICast				();

	// ============================== AISpell =============================
	S32					randomizeAISpell				();
	void				advanceAISpell					();

	// ============================== AICharacter =============================
	GameObject*			getAICharObject					(AI_ObjectType type, AIDefedObject object);
	void				advanceAICharacter				();

	// ============================== AICondition =============================
	bool				AICd_Compare					(S32 val1, S32 val2, S32 cmp);
	bool				AICd_GetSex						(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetRace					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetFamily					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetLevel					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetExp						(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_IsCombative				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_IsInCombat					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_CheckBuff					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetAIStateTimer			(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_IsPlayer					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_IsNpc						(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_IsPet						(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_IsTotem					(S32 param1, S32 param2, S32 cmp, S32 val);
                                                                               
	bool				AICd_GetHP						(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetMP						(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetPP						(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetMaxHP					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetMaxMP					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetMaxPP					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetHP_Pc					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetMP_Pc					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetPP_Pc					(S32 param1, S32 param2, S32 cmp, S32 val);

	bool				AICd_GetStamina					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetMana					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetStrength				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetIntellect				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetAgility					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetPneuma					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetInsight					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetLuck					(S32 param1, S32 param2, S32 cmp, S32 val);

	bool				AICd_GetVelocity				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetAttackSpeed				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetSpellSpeed				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetCritical_Pc				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetAntiCritical_Pc			(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetFatal_Pc				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetDodge_Pc				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetHit_Pc					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetDamageHandle_Pc			(S32 param1, S32 param2, S32 cmp, S32 val);

	bool				AICd_GetPhyDamage				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetPhyDefence				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetMuDamage				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetMuDefence				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetHuoDamage				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetHuoDefence				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetTuDamage				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetTuDefence				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetJinDamage				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetJinDefence				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetShuiDamage				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetShuiDefence				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetPneumaDamage			(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetPneumaDefence			(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetHeal					(S32 param1, S32 param2, S32 cmp, S32 val);

	bool				AICd_GetPhyScale				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetSplScale				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetHealScale				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetHateScale				(S32 param1, S32 param2, S32 cmp, S32 val);

	bool				AICd_GetDeriveHP_Pc				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetDeriveMP_Pc				(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetDerivePP_Pc				(S32 param1, S32 param2, S32 cmp, S32 val);

	bool				AICd_GetMu						(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetHuo						(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetTu						(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetJin						(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetShui					(S32 param1, S32 param2, S32 cmp, S32 val);
	bool				AICd_GetExitInterval			(S32 param1, S32 param2, S32 cmp, S32 val);

	// ============================== AIOperation =============================
	void				AIOp_AddBuff					(S32 param1, S32 param2);
	void				AIOp_Say						(S32 param1, S32 param2);

	// ============================== PlayerAISimple ==============================
	virtual void		PlayerAISimpleIdle_onEnter		() {}
	virtual void		PlayerAISimpleIdle_onExec		() {}
	virtual void		PlayerAISimpleIdle_onExit		() {}

	virtual void		PlayerAISimpleMove_onEnter		() {}
	virtual void		PlayerAISimpleMove_onExec		() {}
	virtual void		PlayerAISimpleMove_onExit		() {}

	virtual void		PlayerAISimpleFollow_onEnter	() {}
	virtual void		PlayerAISimpleFollow_onExec		() {}
	virtual void		PlayerAISimpleFollow_onExit		() {}

	virtual void		PlayerAISimpleChase_onEnter		() {}
	virtual void		PlayerAISimpleChase_onExec		() {}
	virtual void		PlayerAISimpleChase_onExit		() {}

	virtual void		PlayerAISimpleCast_onEnter		() {}
	virtual void		PlayerAISimpleCast_onExec		() {}
	virtual void		PlayerAISimpleCast_onExit		() {}

	virtual void		PlayerAISimpleDeath_onEnter		() {}
	virtual void		PlayerAISimpleDeath_onExec		() {}
	virtual void		PlayerAISimpleDeath_onExit		() {}

	// ============================== NpcAI ==============================
	virtual void		NpcAINone_onEnter				() {}
	virtual void		NpcAINone_onExec				() {}
	virtual void		NpcAINone_onExit				() {}

	virtual void		NpcAIIdle_onEnter				() {}
	virtual void		NpcAIIdle_onExec				() {}
	virtual void		NpcAIIdle_onExit				() {}

	virtual void		NpcAIWalk_onEnter				() {}
	virtual void		NpcAIWalk_onExec				() {}
	virtual void		NpcAIWalk_onExit				() {}

	virtual void		NpcAIWayPoint_onEnter			() {}
	virtual void		NpcAIWayPoint_onExec			() {}
	virtual void		NpcAIWayPoint_onExit			() {}

	virtual void		NpcAIChase_onEnter				() {}
	virtual void		NpcAIChase_onExec				() {}
	virtual void		NpcAIChase_onExit				() {}

	virtual void		NpcAICast_onEnter				() {}
	virtual void		NpcAICast_onExec				() {}
	virtual void		NpcAICast_onExit				() {}

	virtual void		NpcAIRunAway_onEnter			() {}
	virtual void		NpcAIRunAway_onExec				() {}
	virtual void		NpcAIRunAway_onExit				() {}

	virtual void		NpcAIGoHome_onEnter				() {}
	virtual void		NpcAIGoHome_onExec				() {}
	virtual void		NpcAIGoHome_onExit				() {}

	virtual void		NpcAIScared_onEnter				() {}
	virtual void		NpcAIScared_onExec				() {}
	virtual void		NpcAIScared_onExit				() {}

	virtual void		NpcAIDeath_onEnter				() {}
	virtual void		NpcAIDeath_onExec				() {}
	virtual void		NpcAIDeath_onExit				() {}

	// ============================== PetAI ==============================
	virtual void		PetAINone_onEnter				() {}
	virtual void		PetAINone_onExec				() {}
	virtual void		PetAINone_onExit				() {}

	virtual void		PetAIIdle_onEnter				() {}
	virtual void		PetAIIdle_onExec				() {}
	virtual void		PetAIIdle_onExit				() {}

	virtual void		PetAIFollow_onEnter				() {}
	virtual void		PetAIFollow_onExec				() {}
	virtual void		PetAIFollow_onExit				() {}

	virtual void		PetAIChase_onEnter				() {}
	virtual void		PetAIChase_onExec				() {}
	virtual void		PetAIChase_onExit				() {}

	virtual void		PetAICast_onEnter				() {}
	virtual void		PetAICast_onExec				() {}
	virtual void		PetAICast_onExit				() {}

	virtual void		PetAIScared_onEnter				() {}
	virtual void		PetAIScared_onExec				() {}
	virtual void		PetAIScared_onExit				() {}

	virtual void		PetAIDeath_onEnter				() {}
	virtual void		PetAIDeath_onExec				() {}
	virtual void		PetAIDeath_onExit				() {}
};


//-----------------------------------------------------------------------------
// AI状态的基础类，所有AI状态类都应该派生于此类
//-----------------------------------------------------------------------------

class AIStateBase
{
public:
	typedef void (AIBase::*AIEvent)(void);

	AIEvent onEnter;
	AIEvent onExec;
	AIEvent onExit;

	AIStateBase(AIEvent _onEnter, AIEvent _onExec, AIEvent _onExit) : onEnter(_onEnter),onExec(_onExec),onExit(_onExit) {}
};
