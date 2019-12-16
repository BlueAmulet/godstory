//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/GameEvents/ServerGameNetEvents.h"
#include "T3D/gameConnection.h"
#include "Gameplay/GameObjects/Skill.h"

// =========================================================================================================================================================
// 游戏逻辑数据
//

inline void GameObject::setInCombat(bool val, GameObject* pObj /* = NULL */)
{
	if(mIsInCombat != val)
	{
		mIsInCombat = val;
		if(mIsInCombat)
			onCombatBegin(pObj);
		else
			onCombatEnd(pObj);
#ifdef NTJ_SERVER
		setMaskBits(HPMask);
#endif
	}
}

inline bool GameObject::uncontrollableMove()
{
	return mJumpPath.jumping || mBlink;
}


inline bool GameObject::isSilenceBuff()
{
	return m_BuffTable.GetEffect() & BuffData::Buff_Silence;
}

inline bool GameObject::isCanNotAttackBuff()
{
	return m_BuffTable.GetEffect() & BuffData::Buff_CanNotAttack;
}

inline void GameObject::SetRefreshGui(bool val)
{
#ifdef NTJ_CLIENT
	m_BuffTable.SetRefresh(val);
#endif
}

inline void GameObject::SetRefreshStats(bool val)
{
#ifdef NTJ_SERVER
	m_BuffTable.SetRefresh(val);
#endif
}

inline U32 GameObject::getGameObjectMask()
{
	return mGameObjectMask;
}

inline GameObjectData::ArmStatus GameObject::getArmStatus()
{
	return m_ArmStatus;
}

inline void GameObject::setMountAction(GameObjectData::Animations mountAction)
{
	m_MountAction = mountAction;
}

inline GameObjectData::Animations GameObject::getMountAction()
{
	return m_MountAction;
}

// 是否在做某种动作
#define ClampAction(action) (mClamp(action, GameObjectData::Root_a, GameObjectData::NumActionAnims-1))

#define IsRootAnim(action) ((action) >= GameObjectData::Root_a && (action) <= GameObjectData::Root_j)
#define IsIdleAnim(action) ((action) >= GameObjectData::Idle_a0 && (action) <= GameObjectData::Idle_j2)
#define IsRunAnim(action) ((action) >= GameObjectData::Run_a && (action) <= GameObjectData::Run_j)
#define IsReadyAnim(action) ((action) >= GameObjectData::Ready_a && (action) <= GameObjectData::Ready_j)
#define IsAttackAnim(action) ((action) >= GameObjectData::Attack_a0 && (action) <= GameObjectData::Attack_j2)
#define IsSkillAttackAnim(action) ((action) >= GameObjectData::SkillAttack_a0 && (action) <= GameObjectData::SkillAttack_j2)
#define IsCriticalAnim(action) ((action) >= GameObjectData::Critical_a && (action) <= GameObjectData::Critical_j)
#define IsFatalAnim(action) ((action) >= GameObjectData::Fatal_a && (action) <= GameObjectData::Fatal_j)
#define IsReadySpellAnim(action) ((action) >= GameObjectData::ReadySpell_a0 && (action) <= GameObjectData::ReadySpell_j0)
#define IsReadySpellAnim(action) ((action) >= GameObjectData::ReadySpell_a0 && (action) <= GameObjectData::ReadySpell_j0)
#define IsSpellCastAnim(action) ((action) >= GameObjectData::SpellCast_a0 && (action) <= GameObjectData::SpellCast_j0)
#define IsChannelCastAnim(action) ((action) >= GameObjectData::ChannelCast_a0 && (action) <= GameObjectData::ChannelCast_j0)
#define IsMountAnim(action) ((action) >= GameObjectData::Mount_a && (action) <= GameObjectData::Mount_j)
#define IsEmoteAnim(action) ((action) >= GameObjectData::Emote_00 && (action) <= GameObjectData::Emote_29)

// 基础动作可以被其他任意动作取代，不会分截，其内部优先级为Root<Idle<Emote<Ready
#define IsBasicAnim(action) (IsRootAnim(action) || IsIdleAnim(action) || IsReadyAnim(action) || IsEmoteAnim(action))

// 取得某种动作，应根据人物属性获得
inline U32 GameObject::getRootAnim()
{
	return GameObjectData::Root_a;
}

inline F32 GameObject::getMoveSpeed()
{
#ifdef NTJ_EDITOR
	return MAX_FORWARD_SPEED;
#else
	static F32 speed;
	speed = mDataBlock ? mDataBlock->maxForwardSpeed : MAX_FORWARD_SPEED;

	if (getGameObjectMask() & MountObjectType)
	{
		GameObject* obj = (GameObject*)getMountedObject(0);
		if(obj)
			return speed * mClampF(obj->GetStats().MountedVelocity_gPc/GAMEPLAY_PERCENT_SCALE, MIN_GAMEPLAY_PERCENT, MAX_GAMEPLAY_PERCENT);
		return speed;
	}
	else
		return speed * mClampF(GetStats().Velocity_gPc/GAMEPLAY_PERCENT_SCALE, MIN_GAMEPLAY_PERCENT, MAX_GAMEPLAY_PERCENT);
#endif
}

inline void GameObject::getBuffAnim(U32& action)
{
	if(!(m_BuffTable.GetEffect() & BuffData::Buff_Animations))
		return;

	if(m_BuffTable.GetEffect() & BuffData::Effect_Stun)
	{
		action = GameObjectData::Stun;
	}
}

// =========================================================================================================================================================
// 战斗相关
//

inline void GameObject::SetCooldown_Group(S32 cdGroup, U32 cdTime, U32 totalTime)
{
	m_CooldownTable.SetCooldown_Group(cdGroup, cdTime, totalTime);

#ifdef NTJ_SERVER
	GameConnection* conn = getControllingClient();
	if(conn)
	{
		ServerGameNetEvent* pEvent = new ServerGameNetEvent(INFO_COOLDOWN);
		pEvent->SetIntArgValues(1, cdGroup);
		pEvent->SetInt32ArgValues(2, cdTime, totalTime);
		conn->postNetEvent(pEvent);
	}
#endif
}

inline bool GameObject::GetCooldown(S32 cdGroup, U32& cdTime, U32& totalTime, bool ignoreGlobal /* = false */)
{
	const Cooldown* cd = m_CooldownTable.GetCooldown_Group(cdGroup);
	const Cooldown* gcd = m_CooldownTable.GetCooldown_Global();
	if(!gcd)
		return false;

	if(ignoreGlobal)
	{
		if(!cd)
			return false;
		cdTime = cd->EnabledTime;
		totalTime = cd->TotalTime;
	}
	else
	{
		if(cd)
		{
			cdTime = (gcd->EnabledTime > cd->EnabledTime) ? gcd->EnabledTime : cd->EnabledTime;
			totalTime = (gcd->EnabledTime > cd->EnabledTime) ? gcd->TotalTime : cd->TotalTime;
		}
		else
		{
			cdTime = gcd->EnabledTime;
			totalTime = gcd->TotalTime;
		}
	}

	if(cdTime > Platform::getVirtualMilliseconds())
	{
		cdTime = cdTime - Platform::getVirtualMilliseconds();
		totalTime = getMax(cdTime, totalTime);
		return true;
	}

	return false;
}

void GameObject::setFamily(enFamily val)
{
	if(m_Family == val || val < Family_None || val >= MaxFamilies)
		return;
	// 更新数值
	m_BuffTable.RemoveBuff(Macro_GetBuffId(g_FamilyBuff[m_Family], m_Level));
	m_BuffTable.AddBuff(Buff::Origin_Base, Macro_GetBuffId(g_FamilyBuff[val], m_Level), this);
	m_Family = val;
	setMaskBits(BaseInfoMask);
} 

#ifdef NTJ_CLIENT
void GameObject::setChatDirty(bool value)
{
	mChatDirty = value;
}

bool GameObject::getChatDirty()
{
	return mChatDirty;
}

char* GameObject::getChatInfo()
{
	if(mChatInfo[0] == '\0')
		return NULL;
	else
		return mChatInfo;
}

void GameObject::clearChatInfo()
{
	dMemset(&mChatInfo,0,sizeof(mChatInfo));
}
#endif
