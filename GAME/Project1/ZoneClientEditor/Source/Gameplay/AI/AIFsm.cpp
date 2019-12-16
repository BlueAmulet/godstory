//-----------------------------------------------------------------------------
// AI的有限状态机类
//-----------------------------------------------------------------------------

#include "math/mRandom.h"
#include "T3D/moveManager.h"
#include "Gameplay/ai/AIFsm.h"
#include "Gameplay/ai/AIData.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/PetObject.h"

#ifdef NTJ_CLIENT
#include "ui/dGuiMouseGamePlay.h"
#endif

#define roundPerTick 1.0f

const F32 AIBase::cReachDiff = 0.1f;


//-----------------------------------------------------------------------------
// AI的有限状态机基础类
//-----------------------------------------------------------------------------

AIFsm::AIFsm()
{
	m_CurrentAIState = 0;
	m_NextAIState = 0;
	m_pState = NULL;
}

AIFsm::~AIFsm()
{
}

void AIFsm::Update()
{
	AssertFatal( false, "AI, We got a problem here !!");
}


//-----------------------------------------------------------------------------
// AI基础类
//-----------------------------------------------------------------------------

AIBase::AIBase(GameObject* obj):
	m_pObj(obj),
	m_AICharacter(NULL),
	m_FollowLenSquared(4),
	m_NewMove(false),
	m_NewCast(false),
	m_pSkillData(NULL),
	m_bTarget(false),
	m_bTargetSelf(false),
	m_SkillRangeMinSq(0),
	m_SkillRangeMaxSq(0),
	m_ItemType(0),
	m_ItemIdx(0),
	m_ItemUID(0)
{
	dMemset(m_ExitTime, 0, sizeof(m_ExitTime[AICharacter::MaxAIStates]));
	if(obj && obj->mDataBlock)
	{
		m_AICharacter = g_AIRepository.getAIChar(obj->mDataBlock->aiChar);
		m_AISpell = g_AIRepository.getAISpell(obj->mDataBlock->aiSpell);
		if(m_AISpell)
		{
			for (S32 i=0; i<AISpell::MaxSpells; ++i)
			{
				if(m_AISpell->entry[i])
					m_pObj->AddSkill(m_AISpell->entry[i]->skillId, false);
				else
					break;
			}
		}
	}
	m_prevPosition.set(0,0,0);
}

AIBase::~AIBase()
{
}

void AIBase::RecordTime()
{
	m_EnterTime = Platform::getVirtualMilliseconds();
	m_ExitTime[m_CurrentAIState] = Platform::getVirtualMilliseconds();
}

void AIBase::SetStepDest(Point3F& destination)
{
	m_StepDest = destination;
}

void AIBase::ClearAIPath()
{
	m_AiPath.ClearPath();
	m_StepDest = m_pObj->getPosition();
}

void AIBase::SetFollowObject(SceneObject* obj, F32 len/* =2 */, U32 mask/* =0 */)
{
	if(!obj || obj == m_pObj || m_pObj->isDisabled())
	{
		if(m_FlwObj)
			ClearAIPath();
		m_FlwObj = NULL;
		return;
	}
	m_FlwObj = obj;
	m_AIMask = mask;
	// 考虑对象的大小
	len += (m_pObj->getObjBox().len_y() / 2.0f) + (m_FlwObj->getObjBox().len_y() / 2.0f);
	// 适当调整
	len = len - getMax(len * 0.1f, 1.0f);
	m_FollowLenSquared = getMax(1.0f, len*len);
}

bool AIBase::IsFollowing()
{
	return (m_FlwObj && m_pObj != m_FlwObj);
}

bool AIBase::CheckFollowState()
{
	return (IsFollowing() && !m_pObj->IsAutoCastEnable());
}

bool AIBase::CheckChaseState()
{
	return (IsFollowing() && m_pObj->IsAutoCastEnable());
}

bool AIBase::CheckCastState()
{
	return m_pSkillData;
}

bool AIBase::IsReachDest()
{
	Point3F location = m_pObj->getPosition();
	Point3F Diff = m_StepDest - location;
	if (mFabs(Diff.x) < cReachDiff && mFabs(Diff.y) < cReachDiff)
		return true;
	else
		return false;
}

bool AIBase::GetNextMove(Move &curMove)
{
	if(m_pObj->isDisabled() || m_pObj->isImmobilityBuff())
		return true;

	// 如果已经到达终点
	PROFILE_START(IsEmpty);
	if(m_AiPath.IsEmpty()  )
	{
		curMove = NullMove;
		// 自动朝向施法对象
		if(m_pObj->GetSpell().IsSpellRunning())
		{
			curMove.freeLook = MoveManager::mFreeLook;
			Point3F Diff = (m_pObj->GetSpell().GetTarget() ? m_pObj->GetSpell().GetTarget()->getPosition() : m_pObj->GetSpell().GetDest()) - m_pObj->getPosition();
			// 朝向目标
			if (!isZero(Diff.x) || !isZero(Diff.y))
			{
				F32 curYaw = m_pObj->getRotation().z;
				F32 newYaw = mAtan( Diff.x, Diff.y );
				F32 yawDiff = newYaw - curYaw;
				if( yawDiff > M_PI_F )
					yawDiff -= M_2PI_F;
				else if( yawDiff < -M_PI_F )
					yawDiff += M_2PI_F;
				curMove.yaw = yawDiff;
			}
		}
#ifdef NTJ_SERVER
		m_pObj->updateMove_Grid(&curMove,false);
		m_pObj->updatePos_Grid();
		m_pObj->SetPosToMove(&curMove, m_pObj->getPosition());
#endif
		PROFILE_END(IsEmpty);
		return true;
	}
	PROFILE_END(IsEmpty);

	PROFILE_START(IsReachDest);
	// 到达结点，在跟随模式下，不会删除最后一个结点，即不会到达终点
	if(IsReachDest() && (!IsFollowing() || m_AiPath.GetPathList().size() > 1))
	{
		m_AiPath.OnReachStep();
		// 更新结点，如果取结点失败说明到达终点了
		if(!m_AiPath.GetStep(m_StepDest))
		{
			curMove = NullMove;
			#ifdef NTJ_SERVER
			m_pObj->updateMove_Grid(&curMove,false);
			m_pObj->updatePos_Grid();
			m_pObj->SetPosToMove(&curMove, m_pObj->getPosition());
			#endif
			PROFILE_END(IsReachDest);
			return true;
		}
	}
	PROFILE_END(IsReachDest);

	Point3F location = m_pObj->getPosition();
	Point3F rotation = m_pObj->getRotation();

	Point3F Diff = m_StepDest - location;
	Point3F Diff_ = IsFollowing() ? (m_FlwObj->getPosition() - location) : Diff;
	Diff.z = 0;

	bool bMove = (IsFollowing() && m_AiPath.GetPathList().size() == 1) ? Diff_.lenSquared() > m_FollowLenSquared : true;
	// 朝向目标
	if (!isZero(Diff.x) || !isZero(Diff.y))
	{
		F32 curYaw = rotation.z;
		// find the yaw offset
		F32 newYaw = mAtan( Diff.x, Diff.y );
		F32 yawDiff = newYaw - curYaw;

		// now make sure we take the short way around the circle
		if( yawDiff > M_PI_F )
			yawDiff -= M_2PI_F;
		else if( yawDiff < -M_PI_F )
			yawDiff += M_2PI_F;

		if((m_pObj->getGameObjectMask() & PlayerObjectType) && mFabs(yawDiff) > roundPerTick/*roundPerTick * 2 && Diff.lenSquared() < 1*/)
			bMove = false;
		else if(!(m_pObj->getGameObjectMask() & PlayerObjectType) && mFabs(yawDiff) > roundPerTick && Diff.lenSquared() < 1)
			bMove = false;
		yawDiff = yawDiff > 0 ? getMin(yawDiff, roundPerTick) : getMax(yawDiff, -roundPerTick);

		curMove.yaw = yawDiff;
	}
	// todo : 特殊情况的朝向

	// 必定前进
	curMove.y = bMove ? 1.0f : 0.0f;

	// 当距离较小的时候，不应超过该速度
	if( Diff.lenSquared() / TickSec < 15.6f)
		curMove.mVelocity = Diff / TickSec;

	PROFILE_START(SetPosToMove);
#ifdef NTJ_SERVER
	m_pObj->updateMove_Grid(&curMove,false);
	m_pObj->updatePos_Grid();
	m_pObj->SetPosToMove(&curMove, m_pObj->getPosition());
#endif
	curMove.isMounted = m_pObj->isMounted();
	PROFILE_END(SetPosToMove);

	return true;
}

void AIBase::SetRunPath()
{
#ifdef NTJ_SERVER
	if(!m_pObj->GetTarget())
		m_pObj->SetTarget(m_pObj->mHateList.update());
	if(!m_pObj->GetTarget())
		return;

	Point3F diff = m_pObj->getPosition() - m_pObj->GetTarget()->getPosition();
	diff.normalize(5);
	diff.x += gRandGen.randF(-5.0f, 5.0f);
	diff.y += gRandGen.randF(-5.0f, 5.0f);
	diff += m_pObj->getPosition();
	m_pObj->SetTarget(diff);
#endif
}

bool AIBase::RefreshPath_Move()
{
	if(m_NewMove)
	{
		if (m_AiPath.FindPath(m_pObj->getPosition(), m_Move))
		{
			Point3F pos;
			if (m_AiPath.GetStep(pos))
				SetStepDest(pos);
			ClearMove();
			return true;
		}
		ClearMove();
		return false;
	}
	return !m_AiPath.IsEmpty();
}

bool AIBase::RefreshPath_Follow()
{
	if(IsFollowing())
	{
		Point3F diff = m_FlwObj->getPosition() - GetAIPath()->GetEnd();
		Point3F pathLen = m_FlwObj->getPosition() - m_pObj->getPosition();
		if(mFabs(diff.x) + mFabs(diff.y) < getMax(0.32f, 0.1f * (mFabs(pathLen.x) + mFabs(pathLen.y))))
			return true;
		if (m_AiPath.FindPath(m_pObj->getPosition(), m_FlwObj->getPosition()))
		{
			Point3F pos;
			if (m_AiPath.GetStep(pos))
				SetStepDest(pos);
			return true;
		}
	}
	return false;
}

bool AIBase::RefreshCast()
{
	if(!m_NewCast)
		return false;
	// 检查CD
	if(!m_pObj->CheckCastCooldown(m_pSkillData))
		return true;

	if(m_bTarget)
	{
		GameObject* pTargetObj = m_bTargetSelf ? m_pObj : m_pObj->GetTarget();
		if(!pTargetObj)
			return true;

		// 所有的距离都已经转化为两点间的距离了，直接取两点间距离
		F32 lenSq = (m_pObj->getPosition() - pTargetObj->getPosition()).lenSquared();
		if(lenSq > m_SkillRangeMaxSq)
			RefreshPath_Follow();
		else
		{
			m_pObj->CastSpell(m_pSkillData->GetID(), *pTargetObj, m_ItemType, m_ItemIdx, m_ItemUID);
			ClearAIPath();
			ClearMove();
		}
	}
	else
	{
		F32 lenSq = (m_pObj->getPosition() - m_CastDest).lenSquared();
		if(lenSq > m_SkillRangeMaxSq)
			RefreshPath_Move();
		else
		{
			m_pObj->CastSpell(m_pSkillData->GetID(), m_CastDest, m_ItemType, m_ItemIdx, m_ItemUID);
			ClearAIPath();
			ClearMove();
		}
	}

	return true;
}

void AIBase::SetAICast(Spell::PreSpell& pre, F32 min, F32 max, bool self, const Point3F* pos)
{
	// 照顾小白玩家，不检查cd，当cd完成后自动施放
	if(!pre.pSkillData)
		return;
	m_pSkillData = pre.pSkillData;
	m_NewCast = true;
	m_bTarget = !(bool)pos;
	if(pos)
		m_CastDest = *pos;
	m_bTargetSelf = m_bTarget && self;
	m_ItemType = pre.itemInter.type;
	m_ItemIdx = pre.itemInter.index;
	m_ItemUID = pre.itemInter.UID;
	if(min > max)
		_swap(min,max);
	m_SkillRangeMinSq = min*min;
	if(!m_bTarget)
	{
		max = max - getMax(max * 0.05f, 0.5f);
		m_SkillRangeMaxSq = max*max;
	}
	// 这时候应该是跟随模式，技能释放距离应比跟随距离略大
	else if(m_bTargetSelf || m_pObj->GetTarget())
	{
		GameObject* tgt = m_bTargetSelf ? m_pObj : m_pObj->GetTarget();
		max += (m_pObj->getObjBox().len_y() / 2.0f) + (tgt->getObjBox().len_y() / 2.0f);
		max = max - getMax(max * 0.05f, 0.5f);
		m_SkillRangeMaxSq = getMax(m_SkillRangeMinSq, max*max);
		m_SkillRangeMaxSq = getMax(m_FollowLenSquared + 0.32f, m_SkillRangeMaxSq);
	}
	// 施放失败
	else
		ClearAICast();
}

void AIBase::ClearAICast()
{
	m_pSkillData = NULL;
	m_NewCast = false;
	m_SkillRangeMinSq = 0;
	m_SkillRangeMaxSq = 0;
	m_ItemType = 0;
	m_ItemIdx = 0;
	m_ItemUID = 0;
}

//-----------------------------------------------------------------------------
// AISpell
//
S32 AIBase::randomizeAISpell()
{
	if(!m_AISpell)
		return -1;

	S32 sum = 0;
	S32 rd = gRandGen.randI(1, 10000);
	for (S32 i=0; i<AISpell::MaxSpells; ++i)
	{
		if(!m_AISpell->entry[i])
			return -1;
		sum += m_AISpell->entry[i]->rate;
		if(rd <= sum)
			return i;
	}
	return -1;
}

void AIBase::advanceAISpell()
{
	static const AISpellEntry* pEntry;
	static GameObject* pObj;
	static bool ret;
	static S32 iMin,iMax;

	if(!m_AISpell)
		return;

	if(true)
	{
		iMin = iMax = randomizeAISpell();
		if(iMin<0 || iMax>=AISpell::MaxSpells)
			return;
	}
	else
	{
		iMin = 0;
		iMax = AISpell::MaxSpells-1;
	}
	for (S32 i=iMin; i<=iMax; ++i)
	{
		pEntry = m_AISpell->entry[i];
		if(!pEntry)
			break;
		// 判断条件
		if(!pEntry->condition[0])
			ret = true;
		else
		{
			pObj = getAICharObject(pEntry->condition[0]->type , pEntry->condition[0]->object);
			if(!pObj)
				continue;
			ret = (this->*(AICondition::funcCom[pEntry->condition[0]->func]))(pEntry->condition[0]->param1, pEntry->condition[0]->param2, pEntry->condition[0]->cmp, pEntry->condition[0]->val);
			if(pEntry->flagNOT & BIT(0))
				ret = !ret;
			for (S32 j=1; j<AIGoto::MaxCondition && pEntry->condition[j]; ++j)
			{
				pObj = getAICharObject(pEntry->condition[j]->type , pEntry->condition[j]->object);
				if(!pObj)
				{
					ret = false;
					break;
				}
				bool re = (this->*(AICondition::funcCom[pEntry->condition[j]->func]))(pEntry->condition[j]->param1, pEntry->condition[j]->param2, pEntry->condition[j]->cmp, pEntry->condition[j]->val);
				if(pEntry->flagNOT & BIT(j))
					re = !re;
				if(pEntry->flagOR & BIT(j))
					ret = (ret || re);
				else
					ret = (ret && re);
			}
		}

		// 判断成功
		if(ret && m_pObj->GetTarget())
		{
			m_pObj->AI_CastSpell(pEntry->skillId, m_pObj->GetTarget(), &m_pObj->GetTarget()->getPosition());
			break;
		}
	}
}


//-----------------------------------------------------------------------------
// AICharacter
//

GameObject* AIBase::getAICharObject(AI_ObjectType type, AIDefedObject object)
{
	GameObject* pObj = NULL;
	switch(object)
	{
	case AIDefedObject_Self:
		{
			pObj = m_pObj;
		}
		break;
	case AIDefedObject_Target:
		{
			pObj = m_pObj->GetTarget();
		}
		break;
	case AIDefedObject_TargetTgt:
		{
			pObj = m_pObj->GetTarget() ? m_pObj->GetTarget()->GetTarget() : NULL;
		}
		break;
	case AIDefedObject_Master:
		{
			pObj = (m_pObj->getGameObjectMask() & PetObjectType) ? ((PetObject*)m_pObj)->getMaster() : NULL;
		}
		break;
	case AIDefedObject_MasterTgt:
		{
			pObj = (m_pObj->getGameObjectMask() & PetObjectType) ? ((PetObject*)m_pObj)->getMaster() : NULL;
			pObj = pObj ? pObj->GetTarget() : NULL;
		}
		break;
	case AIDefedObject_Pet:
		{
			pObj = (m_pObj->getGameObjectMask() & PlayerObjectType) ? ((Player*)m_pObj)->getPet(0) : NULL;
		}
		break;
	case AIDefedObject_PetTgt:
		{
			pObj = (m_pObj->getGameObjectMask() & PlayerObjectType) ? ((Player*)m_pObj)->getPet(0) : NULL;
			pObj = pObj ? pObj->GetTarget() : NULL;
		}
		break;
	}
	if(!pObj)
		return NULL;
	switch(type)
	{
	case AIObjectType_Common:
		{
			return pObj;
		}
		break;
	case AIObjectType_Player:
		{
			if(pObj->getGameObjectMask() & PlayerObjectType)
				return pObj;
		}
		break;
	case AIObjectType_Npc:
		{
			if(pObj->getGameObjectMask() & NpcObjectType)
				return pObj;
		}
		break;
	case AIObjectType_Pet:
		{
			if(pObj->getGameObjectMask() & PetObjectType)
				return pObj;
		}
		break;
	case AIObjectType_Totem:
		{
			if(pObj->getGameObjectMask() & TotemObjectType)
				return pObj;
		}
		break;
	}
	return NULL;
}

void AIBase::advanceAICharacter()
{
	static const AIGoto* pGoto;
	static GameObject* pObj;
	static bool ret;

	if(!m_AICharacter)
		return;

	for (S32 i=0; i<AICharacter::MaxAIStates; ++i)
	{
		pGoto = m_AICharacter->aiGoto[m_CurrentAIState][i];
		if(!pGoto)
			break;
		// 判断条件
		ret = false;
		if(!pGoto->condition[0])
			ret = true;
		else
		{
			pObj = getAICharObject(pGoto->condition[0]->type , pGoto->condition[0]->object);
			if(!pObj)
				continue;
			ret = (this->*(AICondition::funcCom[pGoto->condition[0]->func]))(pGoto->condition[0]->param1, pGoto->condition[0]->param2, pGoto->condition[0]->cmp, pGoto->condition[0]->val);
			if(pGoto->flagNOT & BIT(0))
				ret = !ret;
			for (S32 j=1; j<AIGoto::MaxCondition && pGoto->condition[j]; ++j)
			{
				pObj = getAICharObject(pGoto->condition[j]->type , pGoto->condition[j]->object);
				if(!pObj)
				{
					ret = false;
					break;
				}
				bool re = (this->*(AICondition::funcCom[pGoto->condition[j]->func]))(pGoto->condition[j]->param1, pGoto->condition[j]->param2, pGoto->condition[j]->cmp, pGoto->condition[j]->val);
				if(pGoto->flagNOT & BIT(j))
					re = !re;
				if(pGoto->flagOR & BIT(j))
					ret = (ret || re);
				else
					ret = (ret && re);
			}
		}

		// 进行相关处理
		if(ret)
		{
			SetNextAIState(pGoto->tgtState);
			for (S32 k=0; k<AIGoto::MaxOperation && pGoto->operation[k]; ++k)
			{
				(this->*(AIOperation::funcCom[pGoto->operation[k]->func]))(pGoto->operation[k]->param1, pGoto->operation[k]->param2);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// AI条件
//

bool AIBase::AICd_Compare(S32 val1, S32 val2, S32 cmp)
{
	switch (cmp)
	{
	case AICondition::Cmp_Equal:
		{
			if(val1 == val2)
				return true;
		}
		break;
	case AICondition::Cmp_NotEqual:
		{
			if(val1 != val2)
				return true;
		}
		break;
	case AICondition::Cmp_Less:
		{
			if(val1 < val2)
				return true;
		}
		break;
	case AICondition::Cmp_LessEqual:
		{
			if(val1 <= val2)
				return true;
		}
		break;
	case AICondition::Cmp_Greater:
		{
			if(val1 > val2)
				return true;
		}
		break;
	case AICondition::Cmp_GreaterEqual:
		{
			if(val1 >= val2)
				return true;
		}
		break;
	default:
		AssertFatal(false, "cmp error!");
	}
	return false;
}

bool AIBase::AICd_GetRace(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->getRace(), val, cmp);
}

bool AIBase::AICd_GetSex(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->getSex(), val, cmp);
}

bool AIBase::AICd_GetFamily(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->getFamily(), val, cmp);
}

bool AIBase::AICd_GetLevel(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->getLevel(), val, cmp);
}

bool AIBase::AICd_GetExp(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->getExp(), val, cmp);
}

bool AIBase::AICd_IsCombative(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->isCombative(), val, cmp);
}

bool AIBase::AICd_IsInCombat(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->isInCombat(), val, cmp);
}

bool AIBase::AICd_CheckBuff(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetBuffTable().checkBuff((param1 > 100000) ? Macro_GetBuffSeriesId(param1) : param1, param2, true), val, cmp);
}

bool AIBase::AICd_GetAIStateTimer(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(Platform::getVirtualMilliseconds() - m_EnterTime, val, cmp);
}

bool AIBase::AICd_IsPlayer(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(bool(m_pObj->getGameObjectMask() & PlayerObjectType), val, cmp);
}

bool AIBase::AICd_IsNpc(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(bool(m_pObj->getGameObjectMask() & NpcObjectType), val, cmp);
}

bool AIBase::AICd_IsPet(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(bool(m_pObj->getGameObjectMask() & PetObjectType), val, cmp);
}

bool AIBase::AICd_IsTotem(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(bool(m_pObj->getGameObjectMask() & TotemObjectType), val, cmp);
}

bool AIBase::AICd_GetHP(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().HP, val, cmp);
}

bool AIBase::AICd_GetMP(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().MP, val, cmp);
}

bool AIBase::AICd_GetPP(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().PP, val, cmp);
}

bool AIBase::AICd_GetMaxHP(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().MaxHP, val, cmp);
}

bool AIBase::AICd_GetMaxMP(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().MaxMP, val, cmp);
}

bool AIBase::AICd_GetMaxPP(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().MaxPP, val, cmp);
}

bool AIBase::AICd_GetHP_Pc(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(mFloor((F32)m_pObj->GetStats().HP/(F32)m_pObj->GetStats().MaxHP * GAMEPLAY_PERCENT_SCALE), val, cmp);
}

bool AIBase::AICd_GetMP_Pc(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(mFloor((F32)m_pObj->GetStats().MP/(F32)m_pObj->GetStats().MaxMP * GAMEPLAY_PERCENT_SCALE), val, cmp);
}

bool AIBase::AICd_GetPP_Pc(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(mFloor((F32)m_pObj->GetStats().PP/(F32)m_pObj->GetStats().MaxPP * GAMEPLAY_PERCENT_SCALE), val, cmp);
}

bool AIBase::AICd_GetStamina(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Stamina, val, cmp);
}

bool AIBase::AICd_GetMana(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Mana, val, cmp);
}

bool AIBase::AICd_GetStrength(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Strength, val, cmp);
}

bool AIBase::AICd_GetIntellect(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Intellect, val, cmp);
}

bool AIBase::AICd_GetAgility(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Agility, val, cmp);
}

bool AIBase::AICd_GetPneuma(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Pneuma, val, cmp);
}

bool AIBase::AICd_GetInsight(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Insight, val, cmp);
}

bool AIBase::AICd_GetLuck(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Luck, val, cmp);
}

bool AIBase::AICd_GetVelocity(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Velocity_gPc, val, cmp);
}

bool AIBase::AICd_GetAttackSpeed(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().AttackSpeed, val, cmp);
}

bool AIBase::AICd_GetSpellSpeed(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().SpellSpeed_gPc, val, cmp);
}

bool AIBase::AICd_GetCritical_Pc(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Critical_gPc, val, cmp);
}

bool AIBase::AICd_GetAntiCritical_Pc(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().AntiCritical_gPc, val, cmp);
}

bool AIBase::AICd_GetFatal_Pc(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Fatal_gPc, val, cmp);
}

bool AIBase::AICd_GetDodge_Pc(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Dodge_gPc, val, cmp);
}

bool AIBase::AICd_GetHit_Pc(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Hit_gPc, val, cmp);
}

bool AIBase::AICd_GetDamageHandle_Pc(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().DamageHandle_gPc, val, cmp);
}

bool AIBase::AICd_GetPhyDamage(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().PhyDamage, val, cmp);
}

bool AIBase::AICd_GetPhyDefence(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().PhyDefence, val, cmp);
}

bool AIBase::AICd_GetMuDamage(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().MuDamage, val, cmp);
}

bool AIBase::AICd_GetMuDefence(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().MuDefence, val, cmp);
}

bool AIBase::AICd_GetHuoDamage(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().HuoDamage, val, cmp);
}

bool AIBase::AICd_GetHuoDefence(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().HuoDefence, val, cmp);
}

bool AIBase::AICd_GetTuDamage(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().TuDamage, val, cmp);
}

bool AIBase::AICd_GetTuDefence(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().TuDefence, val, cmp);
}

bool AIBase::AICd_GetJinDamage(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().JinDamage, val, cmp);
}

bool AIBase::AICd_GetJinDefence(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().JinDefence, val, cmp);
}

bool AIBase::AICd_GetShuiDamage(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().ShuiDamage, val, cmp);
}

bool AIBase::AICd_GetShuiDefence(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().ShuiDefence, val, cmp);
}

bool AIBase::AICd_GetPneumaDamage(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().PneumaDamage, val, cmp);
}

bool AIBase::AICd_GetPneumaDefence(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().PneumaDefence, val, cmp);
}

bool AIBase::AICd_GetHeal(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Heal, val, cmp);
}

bool AIBase::AICd_GetPhyScale(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().PhyScale_gPc, val, cmp);
}

bool AIBase::AICd_GetSplScale(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().SplScale_gPc, val, cmp);
}

bool AIBase::AICd_GetHealScale(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().HealScale_gPc, val, cmp);
}

bool AIBase::AICd_GetHateScale(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().HateScale_gPc, val, cmp);
}

bool AIBase::AICd_GetDeriveHP_Pc(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().DeriveHP_gPc, val, cmp);
}

bool AIBase::AICd_GetDeriveMP_Pc(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().DeriveMP_gPc, val, cmp);
}

bool AIBase::AICd_GetDerivePP_Pc(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().DerivePP_gPc, val, cmp);
}

bool AIBase::AICd_GetMu(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Mu, val, cmp);
}

bool AIBase::AICd_GetHuo(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Huo, val, cmp);
}

bool AIBase::AICd_GetTu(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Tu, val, cmp);
}

bool AIBase::AICd_GetJin(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Jin, val, cmp);
}

bool AIBase::AICd_GetShui(S32 param1, S32 param2, S32 cmp, S32 val)
{
	return AICd_Compare(m_pObj->GetStats().Shui, val, cmp);
}

bool AIBase::AICd_GetExitInterval(S32 param1, S32 param2, S32 cmp, S32 val)
{
	if(param1 < 0 || param1 > AICharacter::MaxAIStates)
		return AICd_Compare(0x7FFFFFFF, val, cmp);
	return AICd_Compare(Platform::getVirtualMilliseconds() - m_ExitTime[param1], val, cmp);
}

//-----------------------------------------------------------------------------
// AI处理
//

void AIBase::AIOp_AddBuff(S32 param1, S32 param2)
{
	m_pObj->AddBuff(Buff::Origin_Buff, param1, m_pObj, param2);
}

