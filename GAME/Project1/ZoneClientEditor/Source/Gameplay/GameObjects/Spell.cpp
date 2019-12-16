//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/Spell.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "core/bitStream.h"
#include "effects/EffectPacket.h"
#include "Gameplay/GameObjects/SpellObject.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/Item/Res.h"

#ifdef NTJ_CLIENT
#include "Gameplay/GameObjects/ScheduleManager.h"
#endif


#define SPELL_GHOST_TIME_LIMIT 500

// ========================================================================================================================================
//  AutoCastTimer
// ========================================================================================================================================

AutoCastTimer::AutoCastTimer():
	m_BeginCastTime(0),
	m_CastCooldown(0),
	m_BeginAnimateTime(0),
	m_AnimateDuration(0),
	m_TimeScale(0),
	m_CooldownTime(0),
	m_Enable(false)
{
}

void AutoCastTimer::UpdateTimer(SimTime CastCooldown, SimTime AnimateDuration)
{
	m_BeginCastTime = Platform::getVirtualMilliseconds();
	m_CastCooldown = CastCooldown;
	m_BeginAnimateTime = m_BeginCastTime;
	m_AnimateDuration = getMin(CastCooldown, AnimateDuration);
	m_TimeScale = (F32)(AnimateDuration)/(F32)(m_AnimateDuration);
	m_CooldownTime = m_BeginCastTime + m_CastCooldown;
}

void AutoCastTimer::SetDelay(SimTime EndTime)
{
	m_CooldownTime = getMax(m_CooldownTime, EndTime);
}


// ========================================================================================================================================
//  ScheduleSpell
// ========================================================================================================================================

ScheduleSpell::ScheduleSpell()
{
	mSource = NULL;
	mTarget = NULL;
	mIsTarget = true;
	mSkillData = NULL;
	mEffectPacket = 0;
	mSelectable = SkillData::Selectable_A;
	mRemainTime = mLimitTime = 0;
	mRemainTimes = mLimitTimes = 0;
	mDest.set(0,0,0);
	mDamageStats.Clear();
}

ScheduleSpell::ScheduleSpell(SimTime time, S32 times, GameObject* src, bool bTgt, GameObject* tgt, Point3F& pos, SkillData* pData, Stats& stats, U32 ep, SkillData::Selectable sl):
mSource(src),mIsTarget(bTgt),mTarget(tgt),mSkillData(pData),mEffectPacket(ep),mSelectable(sl)
{
	mRemainTime = mLimitTime = time;
	mRemainTimes = mLimitTimes = times;
	mDest.set(pos);
	mDamageStats.Clear();
	mDamageStats += stats;
}

ScheduleSpell::~ScheduleSpell()
{
}

bool ScheduleSpell::Advance(F32 dt)
{
	// 目标不存在或死亡
	if(mIsTarget && (!mTarget || mTarget->isDisabled()))
		return false;
	if(mLimitTime == 0)
		return false;

	mRemainTime = getMax(mRemainTime - (S32)dt, 0);
#ifdef NTJ_SERVER
	S32 tickTimes = mRemainTimes - mCeil((F32)mRemainTime/mLimitTime * mLimitTimes);
	while(tickTimes > 0)
	{
		// 按时触发
		if(!mSkillData->IsRegion())
			Spell::SpellDamage(mDamageStats, mSource, mTarget, mSkillData->m_Operation[mSelectable].buffId);
		else
			Spell::SpellDamage_Region(mDamageStats, mSource, mIsTarget ? mTarget->getPosition() : mDest, mSkillData->GetID());
		--tickTimes;
		--mRemainTimes;

		// 触发技能和状态
		Spell::OnTrigger(mSource, mTarget, mSkillData, mSelectable, mDamageStats, true, !mSkillData->IsRegion());
	}
#endif

	return (bool)mRemainTime;
}


// ========================================================================================================================================
//  ItemInterface
// ========================================================================================================================================

bool Spell::ItemInterface::onUse(Player* player, GameObject* target)
{
#ifdef NTJ_SERVER
	if(!UID)
		return true;
	ItemShortcut* pItem = NULL;
	// 首先根据类型、行列去找
	ShortcutObject* pShortcut = g_ItemManager->getShortcutSlot(player,type, index);
	if(pShortcut && pShortcut->isItemObject())
	{
		pItem = dynamic_cast<ItemShortcut*>(pShortcut);
	}
	// 其次根据UID去找
	if(!pItem || !pItem->getRes() || pItem->getRes()->getUID() != UID)
	{
		pItem = g_ItemManager->getItemByUID(player, UID, index);
	}
	if(!pItem)
		return false;
	//AssertFatal(pItem->getType() == type, "ItemInterface::onUse" );
	if(g_ItemManager->useItem(player, type, index, target) != MSG_NONE)
		return false;

#endif
	return true;
}


// ========================================================================================================================================
//  Spell
// ========================================================================================================================================

bool Spell::SetPreSpell(U32 skillId, U32 itemType /* = 0 */, U32 itemIdx /* = 0 */, U64 UID /* = 0 */)
{
	SkillData* pData = g_SkillRepository.GetSkill(skillId);
	if(!pData || !m_Source || !m_Source->CheckCastCooldown(pData))
		return false;

	// 无需指定目标
	if(pData->m_CastLimit.object != SkillData::Object_Destine)
		return false;

	m_PreSpell.pSkillData = pData;
	m_PreSpell.itemInter.type = itemType;
	m_PreSpell.itemInter.index = itemIdx;
	m_PreSpell.itemInter.UID = UID;
	return true;
}

Spell::PreSpell& Spell::GetPreSpell()
{
	return m_PreSpell;
}

void Spell::ResetPreSpell()
{
	m_PreSpell.pSkillData = NULL;
}

void Spell::AddChainEP(U32 skillId, U32 carrier)
{
	if(m_ChainEP.overwrite && skillId && carrier)
	{
		m_ChainEP.overwrite = false;
		m_ChainEP.remainTime = ChainEPTimeMs;
		m_ChainEP.skillId = skillId;
		m_ChainEP.carriers.clear();
		m_ChainEP.carriers.push_back(carrier);
		m_Source->setMaskBits(GameObject::ChainEPMask);
	}
	else if(skillId == m_ChainEP.skillId && skillId && carrier)
		m_ChainEP.carriers.push_back(carrier);
}

Spell::Spell():
	m_SkillId(0),
	m_bCast(false),
	m_bReady(false),
	m_bChannel(false),
	m_bConsume(false),
	m_bTarget(false),
	m_bBreak(false),
	m_bLastBreak(false),
	m_bCastSuc(false),
	m_VelDelayTicks(0),
	m_Status(Status_None),
	m_pSkill(NULL),
	m_pSkillData(NULL),
	m_SourceEffect(0),
	m_TargetEffect(0),
	m_Selectable(SkillData::Selectable_A)
{
	m_Dest.set(0,0,0);
	m_PlusData.Clear();
}

Spell::~Spell()
{
	Clear();

	for(S32 i=0; i < m_ScheduleSpell.size(); i++)
		delete m_ScheduleSpell[i];
	m_ScheduleSpell.clear();
}

void Spell::Clear()
{
	if(m_Source.isNull())
		return;
#ifndef NTJ_EDITOR
	if(m_Source && m_Source->m_pAI)
		m_Source->m_pAI->ClearAICast();
#endif
#ifdef NTJ_SERVER
	// 自己受到持续施法状态影响
	if(m_pSkillData && m_pSkillData->m_Operation[m_Selectable].channelSrcBuff)
		m_Source->RemoveBuffBySrc(m_pSkillData->m_Operation[m_Selectable].channelSrcBuff, m_Source);
	// 单体目标受到持续施法状态影响
	if(m_pSkillData && m_pSkillData->m_Operation[m_Selectable].channelTgtBuff && m_bTarget)
		m_Target->RemoveBuffBySrc(m_pSkillData->m_Operation[m_Selectable].channelSrcBuff, m_Target);
#endif
#ifdef NTJ_CLIENT
	if(m_SourceEffect)
		g_EffectPacketContainer.removeEffectPacket(m_SourceEffect);
	if(m_TargetEffect)
		g_EffectPacketContainer.removeEffectPacket(m_TargetEffect);
	if(m_Source->getControllingClient() && !m_pSkillData->IsFlags(SkillData::Flags_HideChannel))
	{
		if(m_bBreak)
			Con::executef("cancleFetchTimeProgress");
		else
			Con::executef("CloseFetchTimeProgress");
	}
#endif

	m_bReady = false;
	m_bChannel = false;
	m_SourceEffect = 0;
	m_TargetEffect = 0;
	SetStatus(Status_None);
	m_SkillId = 0;
	m_pSkill = NULL;
	m_pSkillData = NULL;
	m_VelDelayTicks = 0;
	m_PlusData.Clear();
	m_ItemInterface.clear();
}

void Spell::SetStatus(Status status)
{
	if(m_Status == status)
		return;
	m_Status = status;

#ifdef NTJ_SERVER
	if(m_Status != Status_SpellCast && !m_pSkillData->IsFlags(SkillData::Flags_AutoCast) && m_Source && (m_PlusData.m_Cast.channelTime + m_PlusData.m_Cast.readyTime >= SPELL_GHOST_TIME_LIMIT))
		m_Source->setMaskBits(GameObject::SpellMask);
#endif
}

void Spell::AddScheduleSpell()
{
	// 内部使用，故省去了变量的合法性判断
	if(!m_Source || !m_pSkillData->IsFlags(SkillData::Flags_ScheduleSpell))
	{
#ifdef NTJ_CLIENT
		if(!m_pSkillData->IsFlags(SkillData::Flags_AutoCast))
			AddDelayEP(m_pSkillData, m_Source, m_Target);
#endif
		return;
	}

	// 确定作用次数及排定时间
	SimTime time = 1000;
	S32 times = 1;
	if(m_bTarget)
	{
		if(!m_Target)
			return;

		Point3F dist = m_Target->getPosition() - m_Source->getPosition();
		time = dist.len() / getMax(m_pSkillData->m_Cast.projectileVelocity, 1.0f) * 1000;
#ifdef NTJ_CLIENT
		time = time * 3;
#endif
	}
	else
	{
		BuffData* pBuff = g_BuffRepository.GetBuff(m_pSkillData->m_Operation[m_Selectable].buffId);
		if(!pBuff)
			return;

		time = pBuff->m_LimitTime;
		times = pBuff->m_LimitTimes;
	}

	// 计算伤害
	CalculateDamage(g_Stats, m_Source, m_pSkillData->m_Operation[m_Selectable].buffId, NULL);

	// 若次数大于1，则创建SpellObject
	if (time > 1 && !m_bTarget)
	{
#ifdef NTJ_SERVER
		SpellObject* pSpellObj = new SpellObject(time, times, m_Source, m_bTarget, m_Target, m_Dest, m_pSkillData, g_Stats, 0, m_Selectable);
		if(!pSpellObj->registerObject())
			pSpellObj->deleteObject();
		else
			pSpellObj->setPosition(m_Dest);
#endif
		return;
	}

	// 加入列表
	ScheduleSpell* pScheduleSpell = new ScheduleSpell(time, times, m_Source, m_bTarget, m_Target, m_Dest, m_pSkillData, g_Stats, m_SourceEffect, m_Selectable);
	if(pScheduleSpell)
		m_ScheduleSpell.push_back(pScheduleSpell);
#ifdef NTJ_SERVER
	m_Source->setSkillDataEP(m_pSkillData->GetID(), m_Selectable);
	if(m_Target && m_bTarget)
		m_Source->addTargetObjEP(m_Target->getId());
	else if(!m_bTarget)
		m_Source->setDestEP(m_Dest);
	else
	{
		m_Source->clearAnimateEP();
		AssertFatal("Spell::AddScheduleSpell",false);
	}
#endif

	// 轨迹特效ID已保存，清除之
	m_SourceEffect = 0;
}

void Spell::AddDelayEP(SkillData* pSkillData, GameObject* source, GameObject* target)
{
	if(!pSkillData)
		return;

	// 注意，延时法术应该为有延时的特效
	if(source && pSkillData->m_Cast.sourceDelayEP)
		g_EffectPacketContainer.addEffectPacket(pSkillData->m_Cast.sourceDelayEP, source, source->getRenderTransform(), source, source->getPosition());
	if(target && pSkillData->m_Cast.targetDelayEP)
	{
		U32 id = g_EffectPacketContainer.addEffectPacket(pSkillData->m_Cast.targetDelayEP, target, target->getRenderTransform(), target, target->getPosition());
		//if(!id)
		//	int cc=0;
		target->onWound();
	}
}

bool Spell::CheckVelocity()
{
	// 只有施放状态不需要检测速度
	if(m_Status == Status_SpellCast)
		return true;
	if(m_Source->getVelocity().isZero())
	{
		m_VelDelayTicks = 0;
		return true;
	}
	if(m_VelDelayTicks > 0)
	{
		--m_VelDelayTicks;
		return true;
	}
	else
		return false;
}

bool Spell::CanCast(SkillData* pSkillData, GameObject* target)
{
	if(!pSkillData || !m_Source)
		return false;

	enWarnMessage msg = m_Source->CanCast(pSkillData, target, NULL);
	if(MSG_NONE == msg)
		return true;
#ifdef NTJ_SERVER
	MessageEvent::send(m_Source->getControllingClient(), SHOWTYPE_WARN, msg, SHOWPOS_SCREEN);
#endif
	return false;
}

bool Spell::CanCast(SkillData* pSkillData, Point3F& dest)
{
	if(!pSkillData || !m_Source)
		return false;

	enWarnMessage msg = m_Source->CanCast(pSkillData, NULL, &dest);
	if(MSG_NONE == msg)
		return true;
#ifdef NTJ_SERVER
	MessageEvent::send(m_Source->getControllingClient(), SHOWTYPE_WARN, msg, SHOWPOS_SCREEN);
#endif
	return false;
}

bool Spell::Cast(U32 skillId, GameObject& target, U32 itemType /* = 0 */, U32 itemIdx /* = 0 */, U64 UID /* = 0 */)
{
	m_bCastSuc = false;
#ifdef NTJ_SERVER
	m_Source->setMaskBits(GameObject::SpellMask);
#endif
	if (IsSpellRunning() || !m_Source)
		return false;

	m_pSkill = m_Source->GetSkillTable().GetSkill(Macro_GetSkillSeriesId(skillId));
	m_pSkillData = m_pSkill ? m_pSkill->GetData() : NULL;

#ifdef NTJ_CLIENT
	// 客户端可能没有技能数据，所以m_pSkill可能为NULL
	if(!m_pSkillData)
		m_pSkillData = g_SkillRepository.GetSkill(skillId);
#endif
#ifdef NTJ_SERVER
	if(!m_pSkillData && UID)
		m_pSkillData = g_SkillRepository.GetSkill(skillId);
#endif

	m_VelDelayTicks = MaxVelDelayTicks;
	if(!m_pSkillData)
		return false;
	else if((m_pSkillData->m_Cast.readyTime || m_pSkillData->m_Cast.channelTime) && !CheckVelocity())
		return false;
	if(target.canTame() != m_pSkillData->IsFlags(SkillData::Flags_Tame))
		return false;
	// to do:天赋加成
	dMemcpy(&m_PlusData, m_pSkillData, sizeof(SkillData));

	// to do : 检查施放条件
	if(!CanCast(m_pSkillData, &target))
		return false;

	m_bTarget = true;	// 对目标对象施放
	if(m_pSkillData->m_CastLimit.object == SkillData::Object_Self)
		m_Target = m_Source;
	else
		m_Target = &target;
	m_Dest.set(0,0,0);
	m_ItemInterface.type = itemType;
	m_ItemInterface.index = itemIdx;
	m_ItemInterface.UID = UID;

	m_bCastSuc = OnBegin();
	return m_bCastSuc;
}

bool Spell::Cast(U32 skillId, Point3F& dest, U32 itemType /* = 0 */, U32 itemIdx /* = 0 */, U64 UID /* = 0 */)
{
	m_bCastSuc = false;
#ifdef NTJ_SERVER
	m_Source->setMaskBits(GameObject::SpellMask);
#endif
	if (IsSpellRunning() || !m_Source)
		return false;

	m_pSkill = m_Source->GetSkillTable().GetSkill(Macro_GetSkillSeriesId(skillId));
	m_pSkillData = m_pSkill ? m_pSkill->GetData() : NULL;

#ifdef NTJ_CLIENT
	// 客户端可能没有技能数据，所以m_pSkill可能为NULL
	if(!m_pSkillData)
		m_pSkillData = g_SkillRepository.GetSkill(skillId);
#endif
#ifdef NTJ_SERVER
	if(!m_pSkillData && UID)
		m_pSkillData = g_SkillRepository.GetSkill(skillId);
#endif

	m_VelDelayTicks = MaxVelDelayTicks;
	if(!m_pSkillData)
		return false;
	else if((m_pSkillData->m_Cast.readyTime || m_pSkillData->m_Cast.channelTime) && !CheckVelocity())
		return false;
	// to do:天赋加成
	dMemcpy(&m_PlusData, m_pSkillData, sizeof(SkillData));

	// to do : 检查施放条件
	if(!CanCast(m_pSkillData, dest))
		return false;

	m_bTarget = false;	// 对目标坐标施放
	m_Target = NULL;
	m_Dest = dest;
	m_ItemInterface.type = itemType;
	m_ItemInterface.index = itemIdx;
	m_ItemInterface.UID = UID;

	m_bCastSuc = OnBegin();
	return m_bCastSuc;
}

bool Spell::ClientCast(U32 skillId, GameObject& target)
{
#ifdef NTJ_CLIENT
	if (!m_Source)
		return false;

	m_pSkill = m_Source->GetSkillTable().GetSkill(Macro_GetSkillSeriesId(skillId));
	m_pSkillData = m_pSkill ? m_pSkill->GetData() : NULL;

	// 客户端可能没有技能数据，所以m_pSkill可能为NULL
	if(!m_pSkillData)
		m_pSkillData = g_SkillRepository.GetSkill(skillId);
	if(!m_pSkillData || m_pSkillData->IsFlags(SkillData::Flags_AutoCast))
		return false;
	// to do:天赋加成
	dMemcpy(&m_PlusData, m_pSkillData, sizeof(SkillData));

	m_bTarget = true;	// 对目标对象施放
	m_Target = &target;
	m_Dest.set(0,0,0);

	OnBegin();
#endif
	return true;
}

bool Spell::ClientCast(U32 skillId, Point3F& dest)
{
#ifdef NTJ_CLIENT
	if (!m_Source)
		return false;

	m_pSkill = m_Source->GetSkillTable().GetSkill(Macro_GetSkillSeriesId(skillId));
	m_pSkillData = m_pSkill ? m_pSkill->GetData() : NULL;

	// 客户端可能没有技能数据，所以m_pSkill可能为NULL
	if(!m_pSkillData)
		m_pSkillData = g_SkillRepository.GetSkill(skillId);
	if(!m_pSkillData || m_pSkillData->IsFlags(SkillData::Flags_AutoCast))
		return false;
	// to do:天赋加成
	dMemcpy(&m_PlusData, m_pSkillData, sizeof(SkillData));

	m_bTarget = false;	// 对目标坐标施放
	m_Target = NULL;
	m_Dest = dest;

	OnBegin();
#endif
	return true;
}

bool Spell::OnBegin()
{
	m_bCast = false;
	m_bReady = false;
	m_bChannel = false;
	m_bConsume = false;
	m_ReadySpellTime = m_PlusData.m_Cast.readyTime;
	m_ChannelCastTime = m_PlusData.m_Cast.channelTime;
	m_SkillId = m_pSkillData->m_SkillId;
	// 目标选择通道
	m_Selectable = SkillData::Selectable_A;
	if(m_pSkillData->m_CastLimit.object == SkillData::Object_Target)
	{
		U32 CanAtk = m_Source->CanAttackTarget(m_Target) ? SkillData::Target_Enemy : SkillData::Target_Friend;
		if(CanAtk & m_pSkillData->m_CastLimit.targetSl[SkillData::Selectable_A])
			m_Selectable = SkillData::Selectable_A;
		else if(CanAtk & m_pSkillData->m_CastLimit.targetSl[SkillData::Selectable_B])
			m_Selectable = SkillData::Selectable_B;
		else
			AssertFatal(false, "Spell::OnBegin()!");
	}
	// 自动攻击不会覆盖某些信息
	if(!m_pSkillData->IsFlags(SkillData::Flags_AutoCast))
	{
		m_bLastBreak = m_bBreak;
		m_bBreak = false;
	}
	// 设置公共CD
	if(!m_pSkillData->IsFlags(SkillData::Flags_AutoCast) && m_pSkillData->IsFlags(SkillData::Flags_SetCDGlobal))
		m_Source->SetCooldown_Global();

#ifdef NTJ_SERVER
	// 尽量在一个tick内完成，减少瞬发技能被打断的几率
	if (m_ReadySpellTime > 0)
		SetStatus(Status_ReadySpell);
	else
		SetStatus(Status_SpellCast);

	// 先清除相应的特效
	m_Source->clearAnimateEP();
#endif

#ifdef NTJ_CLIENT
	// 客户端在开始施放的时候，会停留在Status_Begin状态，直到收到服务器的回应
	SetStatus(Status_Begin);
	// 客户端做动作，即使是瞬发技能也要求有吟唱动作
	if(!m_Source->isSameUpper(m_pSkillData->m_Cast.readySpell) && m_Source->cullCastAnim())
	{
		m_Source->setActionThread(m_pSkillData->m_Cast.readySpell, true, false, true, false, false);

		if(m_SourceEffect)
			g_EffectPacketContainer.removeEffectPacket(m_SourceEffect);
		m_SourceEffect = g_EffectPacketContainer.addEffectPacket(m_pSkillData->m_Cast.sourceEP, m_Source, m_Source->getRenderTransform(), m_Target, m_Source->getPosition());
		if(m_TargetEffect)
			g_EffectPacketContainer.removeEffectPacket(m_TargetEffect);
		if(m_Target)
		{
			m_TargetEffect = g_EffectPacketContainer.addEffectPacket(m_pSkillData->m_Cast.targetEP, m_Target, m_Target->getRenderTransform(), m_Target, m_Target->getPosition());
		}
	}
#endif

	return true;
}

bool Spell::OnReadySpell()
{
#ifdef NTJ_SERVER
	if(m_ReadySpellTime <= 0)
	{
		SetStatus(Status_SpellCast);
	}
#endif

#ifdef NTJ_CLIENT
	// 动作都放入客户端执行
	if(!m_Source->isSameUpper(m_pSkillData->m_Cast.readySpell) && m_Source->cullCastAnim())
	{
		m_Source->setActionThread(m_pSkillData->m_Cast.readySpell, true, false, true, false, false);

		if(m_SourceEffect)
			g_EffectPacketContainer.removeEffectPacket(m_SourceEffect);
		m_SourceEffect = g_EffectPacketContainer.addEffectPacket(m_pSkillData->m_Cast.sourceEP, m_Source, m_Source->getRenderTransform(), m_Target, m_Source->getPosition());
		if(m_TargetEffect)
			g_EffectPacketContainer.removeEffectPacket(m_TargetEffect);
		if(m_Target)
		{
			m_TargetEffect = g_EffectPacketContainer.addEffectPacket(m_pSkillData->m_Cast.targetEP, m_Target, m_Target->getRenderTransform(), m_Target, m_Target->getPosition());
		}
	}
	if(!m_bReady && m_Source->getControllingClient() && !m_pSkillData->IsFlags(SkillData::Flags_HideChannel))
	{
		Con::executef("InitFetchTimeProgress", Con::getIntArg(m_ReadySpellTime),Con::getIntArg((S32)VocalStatus::VOCALSTATUS_SKILL),m_pSkillData->GetName());
		m_bReady = true;
	}
#endif

	return true;
}

bool Spell::OnSpellCast()
{
#ifdef NTJ_SERVER
	if(!OnConsume())
	{
		BreakSpell();
		return false;
	}

	if(m_pSkillData->IsFlags(SkillData::Flags_TriPhyEnabled))
		m_Source->onBuffTrigger(BuffData::Trigger_PhyAttack, m_bTarget ? m_Target : NULL); // 统一状态触发<物理攻击>
	if(m_pSkillData->IsFlags(SkillData::Flags_TriSplEnabled))
		m_Source->onBuffTrigger(BuffData::Trigger_SplAttack, m_bTarget ? m_Target : NULL); // 统一状态触发<法术攻击>

	if(m_pSkillData->IsFlags(SkillData::Flags_AutoCast))
	{
		m_Source->UpdateAutoCast(m_pSkillData->m_Cast.spellCast);
		m_Source->setActionThread(GameObjectData::Attack_a0 + ((Platform::getVirtualMilliseconds() & 0x55555555) % 3), true, false, true, false, true);
	}
	else
		m_Source->setActionThread(m_pSkillData->m_Cast.spellCast, true, false, false, false, true);

	// 仇恨
	m_Source->mHateList.modifyHate(m_pSkillData->m_Cast.modifyHate);
	// 设置CD
	m_Source->SetCooldown_Group(m_pSkillData->GetCDGroup(), m_PlusData.m_Cast.cooldown, m_PlusData.m_Cast.cooldown);
	m_ChainEP.clear();

	if(m_ChannelCastTime > 0)
	{
		// 自己受到持续施法状态影响
		if(m_pSkillData->m_Operation[m_Selectable].channelSrcBuff)
			m_Source->AddBuff(Buff::Origin_Buff, m_pSkillData->m_Operation[m_Selectable].channelSrcBuff, m_Source);
		// 单体目标受到持续施法状态影响
		if(m_pSkillData->m_Operation[m_Selectable].channelTgtBuff && m_bTarget)
			m_Target->AddBuff(Buff::Origin_Buff, m_pSkillData->m_Operation[m_Selectable].channelSrcBuff, m_Target);

		SetStatus(Status_ChannelCast);
		// 技能的数值
		BuffData* pBuffData = g_BuffRepository.GetBuff(m_pSkillData->m_Operation[m_Selectable].buffId);
		if(pBuffData)
		{
			m_RemainTimes = pBuffData->m_LimitTimes;
			m_LimitTimes = pBuffData->m_LimitTimes;
		}
	}
	else
	{
		SetStatus(Status_End);
		// 伤害计算逻辑处理，延时施法
		if(m_pSkillData->IsFlags(SkillData::Flags_ScheduleSpell))
			AddScheduleSpell();
		else
		{
			if(!m_pSkillData->IsRegion())
			{
				SpellDamage(m_Source, m_Target, m_pSkillData->m_Operation[m_Selectable].buffId);
				m_Source->setSkillDataEP(m_pSkillData->GetID(), m_Selectable);
				if(m_Target && m_bTarget)
					m_Source->addTargetObjEP(m_Target->getId());
				else if(!m_bTarget)
					m_Source->setDestEP(m_Dest);
				else
				{
					m_Source->clearAnimateEP();
					AssertFatal("Spell::OnSpellCast",false);
				}
			}
			else
				SpellDamage_Region(m_Source, m_bTarget ? m_Target->getPosition() : m_Dest, m_pSkillData->GetID(), true);

			// 触发技能和状态
			Spell::OnTrigger(m_Source, m_Target, m_pSkillData, m_Selectable, g_Stats, true, !m_pSkillData->IsRegion());
		}
	}

	if(m_pSkillData->IsFlags(SkillData::Flags_Show))
		CombatLogEvent::show(CombatLogEvent::CombatInfo_Spell, m_Source, NULL, m_pSkillData->GetID(),0,0,0);
#endif

#ifdef NTJ_CLIENT

	if(!m_pSkillData->IsFlags(SkillData::Flags_AutoCast))
		m_Source->setActionThread(m_pSkillData->m_Cast.spellCast, true, false, false, false, true);

	// 客户端会多次执行，所以要判断是否已经施放了
	if(m_bCast)
		return true;

	if(m_SourceEffect)
		g_EffectPacketContainer.setEffectPacketStatus(m_SourceEffect, EffectPacketItem::Status_B);
	if(m_TargetEffect)
		g_EffectPacketContainer.setEffectPacketStatus(m_TargetEffect, EffectPacketItem::Status_B);

	if(m_pSkillData->IsEffect(SkillData::Effect_Jump) && !m_bTarget)
		m_Source->setJumpPath(m_Dest);
	if(m_pSkillData->IsEffect(SkillData::Effect_Blink) && !m_bTarget)
		m_Source->setBlink(m_Dest);
	AddScheduleSpell();
#endif
	m_bCast = true;
	return true;
}

bool Spell::OnChannelCast()
{
#ifdef NTJ_SERVER

	if(m_PlusData.m_Cast.channelTime == 0)
		return true;
	S32 tickTimes = m_RemainTimes - mCeil((F32)m_ChannelCastTime/m_PlusData.m_Cast.channelTime * m_LimitTimes);
	while(tickTimes > 0)
	{
		// 按时触发
		if(!m_pSkillData->IsRegion())
			SpellDamage(m_Source, m_Target, m_pSkillData->m_Operation[m_Selectable].buffId);
		else
			SpellDamage_Region(m_Source, m_bTarget ? m_Target->getPosition() : m_Dest, m_pSkillData->GetID());
		--tickTimes;
		--m_RemainTimes;
	}

	if(m_ChannelCastTime <= 0)
	{
		SetStatus(Status_End);
	}
	// 持续施法动作紧跟着施放动作 所以交给客户端处理
#endif

#ifdef NTJ_CLIENT

	//Con::printf("onchannel cast !!!!! %d", m_Source->GetUpperAction());
	// 如果施放动作结束则接持续施法动作
	if(!m_Source->isSameUpper(m_pSkillData->m_Cast.readySpell) && m_Source->cullCastAnim())
		m_Source->setActionThread(m_pSkillData->m_Cast.channelCast, true, false, true, false, false);

	if(!m_bChannel)
	{
		if(m_Source->getControllingClient() && !m_pSkillData->IsFlags(SkillData::Flags_HideChannel))
			Con::executef("InitFetchTimeProgress", Con::getIntArg(m_ChannelCastTime),Con::getIntArg((S32)VocalStatus::VOCALSTATUS_SKILL),m_pSkillData->GetName());
		if(m_pSkillData->m_Cast.sourceEP && !m_SourceEffect)
		{
			m_SourceEffect = g_EffectPacketContainer.addEffectPacket(m_pSkillData->m_Cast.sourceEP, m_Source, m_Source->getRenderTransform(), m_Target, m_Source->getPosition());
		}
		if(m_pSkillData->m_Cast.targetEP && !m_TargetEffect && m_Target)
		{
			m_TargetEffect = g_EffectPacketContainer.addEffectPacket(m_pSkillData->m_Cast.targetEP, m_Target, m_Target->getRenderTransform(), m_Target, m_Target->getPosition());
		}

		if(m_SourceEffect)
			g_EffectPacketContainer.setEffectPacketStatus(m_SourceEffect, EffectPacketItem::Status_C);
		if(m_TargetEffect)
			g_EffectPacketContainer.setEffectPacketStatus(m_TargetEffect, EffectPacketItem::Status_C);
		m_bChannel = true;
	}
#endif

	return true;
}

bool Spell::OnEnd()
{
	// 和死亡一样，需要强制刷新动作
	if(m_Source->getUpperAction() == m_Source->getAnim(m_pSkillData->m_Cast.channelCast) ||
		m_Source->getUpperAction() == m_Source->getAnim(m_pSkillData->m_Cast.readySpell))
		m_Source->pickActionAnimation(true);

#ifdef NTJ_SERVER
	// 除去自己受到的持续施法状态
	if(m_pSkillData->m_Operation[m_Selectable].channelSrcBuff)
		m_Source->RemoveBuffBySrc(m_pSkillData->m_Operation[m_Selectable].channelSrcBuff, m_Source);
	// 除去单体目标受到的持续施法状态
	if(m_pSkillData->m_Operation[m_Selectable].channelTgtBuff && m_bTarget)
		m_Target->RemoveBuffBySrc(m_pSkillData->m_Operation[m_Selectable].channelSrcBuff, m_Target);

	// 某些技能延缓了自动攻击
	if(m_pSkillData->IsFlags(SkillData::Flags_AutoCastDelay))
		m_Source->SetAutoCastDelay(500);

	Clear();
#endif

#ifdef NTJ_CLIENT
	Clear();
#endif

	return true;
}

void Spell::PackChainEP(NetConnection* conn, BitStream* stream)
{
	if(stream->writeFlag(m_ChainEP.skillId))
	{
		stream->writeInt(m_ChainEP.skillId, Base::Bit32);
		stream->writeInt(m_ChainEP.carriers.size(), Base::Bit8);
		for (S32 i=0; i<m_ChainEP.carriers.size(); ++i)
		{
			GameObject* pObj = (GameObject*)Sim::findObject(m_ChainEP.carriers[i]);
			stream->writeInt(pObj ? conn->getGhostIndex(pObj) : 0, NetConnection::GhostIdBitSize);
		}
	}
}

void Spell::UnpackChainEP(NetConnection* conn, BitStream* stream)
{
	if(stream->readFlag())
	{
		m_ChainEP.carriers.clear();
		U32 id = stream->readInt(Base::Bit32);
		U32 size = stream->readInt(Base::Bit8);
		for (S32 i=0; i<size; ++i)
		{
			GameObject* obj = dynamic_cast<GameObject*>(conn->resolveGhost(stream->readInt(NetConnection::GhostIdBitSize)));
			if(obj)
				m_ChainEP.carriers.push_back((U32)obj);
		}

		// ep
		SkillData* pData = g_SkillRepository.GetSkill(id);
		if(!pData)
			return;
		GameObject* obj, *nextObj;
		for (S32 i=0; i<size-1; ++i)
		{
			obj = (GameObject*)(m_ChainEP.carriers[i]);
			nextObj = (GameObject*)(m_ChainEP.carriers[i+1]);
			g_EffectPacketContainer.addEffectPacket(pData->m_Cast.targetDelayEP, nextObj, nextObj->getTransform(), obj, nextObj->getPosition());
		}
	}
	m_ChainEP.clear();
}

void Spell::Pack(NetConnection* conn, BitStream* stream)
{
	if (stream->writeFlag(m_SkillId))
	{
		// 自动攻击不发送到客户端
		if(stream->writeFlag(m_pSkillData && m_pSkillData->IsFlags(SkillData::Flags_AutoCast)))
			return;

		stream->writeInt(m_SkillId, Base::Bit32);
		stream->writeInt(m_Status, StatusBits);
		stream->writeFlag(m_bLastBreak);
		if(stream->writeFlag(m_bTarget))
			stream->writeInt(conn->getGhostIndex(m_Target), NetConnection::GhostIdBitSize);
	}
	else
	{
		stream->writeFlag(m_bCastSuc);
		stream->writeFlag(m_bBreak);
	}
}

void Spell::Unpack(NetConnection* conn, BitStream* stream)
{
	if (stream->readFlag())
	{
		if(stream->readFlag())
			return;

		U32 newSkillId = stream->readInt(Base::Bit32);
		U32 newStatus = stream->readInt(StatusBits);
		m_bLastBreak = stream->readFlag();
		m_bTarget = stream->readFlag();
		if (m_bTarget)
		{
			S32 id = stream->readInt(NetConnection::GhostIdBitSize);
			m_Target = dynamic_cast<GameObject*>(conn->resolveGhost(id));
		}

		GotoSpell(newSkillId, (Spell::Status)newStatus);
	}
	else
	{
		m_bCastSuc = stream->readFlag();
		m_bBreak = stream->readFlag();
		if(!m_bCastSuc)
		{
			//AssertFatal(m_Status <= Status_Begin, avar("Spell::Unpack : client status error %d", m_Status));
			BreakSpell();				// 客户端打断施法
		}
		else if(m_bBreak)
			BreakSpell();				// 客户端打断施法
		else
			GotoSpell(0, Status_End);	// 正常结束
	}
}

void Spell::BreakSpell()
{
	// 不需要打断和更新
	if(m_Status == Status_None)
		return;

	// 和死亡一样，需要强制刷新动作
	m_Source->pickActionAnimation(true);

	m_bBreak = true;
	Clear();
}

void Spell::GotoSpell(U32 skillid, Spell::Status status)
{
	if(!m_Source)
		return;
	// 未知目标，可以考虑添加专用的未知目标对象
	if(m_bTarget && !m_Target)
		return;

	// 技能ID是0，说明施放已经结束
	if(skillid == 0)
	{
		if(!m_SkillId)
			return;
		skillid = m_SkillId;
		status = Status_End;
	}
	// 是其他技能的话，先强行结束
	else if(skillid != m_SkillId)
	{
		if(m_bLastBreak)
			BreakSpell();
		else
			GotoSpell(0, Status_End);
	}
	// 刚开始施放
	if(m_Status == Status_End || m_Status == Status_None)
	{
		if(m_bTarget)
		{
			if(!ClientCast(skillid, *m_Target))
				return;
		}
		else if(!ClientCast(skillid, m_Source->getPosition()))
			return;
	}
	AssertFatal(status>=Status_None && status<=Status_End,"Spell::GotoSpell");
	AssertFatal(m_pSkillData->m_SkillId == skillid, "Spell::GotoSpell");

	// 逻辑处理
	if (m_PlusData.m_Cast.readyTime > 0 && m_Status < Status_ReadySpell && status >= Status_ReadySpell)
	{
		m_Status = Status_ReadySpell;
		OnReadySpell();
	}
	if (m_Status < Status_SpellCast && status >= Status_SpellCast)
	{
		m_Status = Status_SpellCast;
		OnSpellCast();
	}
	if (m_PlusData.m_Cast.channelTime > 0 && m_Status < Status_ChannelCast && status >= Status_ChannelCast)
	{
		m_Status = Status_ChannelCast;
		OnChannelCast();
	}
	if (m_Status < Status_End && status >= Status_End)
	{
		m_Status = Status_End;
		OnEnd();
	}
}

void Spell::AdvanceSpell(F32 dt)
{
	AssertFatal(!m_Source.isNull(), "");
	// 目标不存在或死亡
	if(m_bTarget && (!m_Target || m_Target->isDisabled()))
	{
		BreakSpell();
		return;
	}
	// 服务端推进
#ifdef NTJ_SERVER
	if(!CheckVelocity() || m_Source->isSilenceBuff())
	{
		BreakSpell();
		return;
	}
	m_bCastSuc = true;
	switch (m_Status)
	{
	case Status_ReadySpell:
		{
			// to do : 检查合法性
			m_ReadySpellTime -= dt;
			OnReadySpell();
		}
		break;
	case Status_SpellCast:
		{
			OnSpellCast();
		}
		break;
	case Status_ChannelCast:
		{
			m_ChannelCastTime -= dt;
			OnChannelCast();
		}
		break;
	default:
		AssertFatal(false, " Spell::AdvanceSpell_Server ");
	}
	if(m_Status == Status_End)
	{
		OnEnd();
	}
#endif

	// 客户端推进
#ifdef NTJ_CLIENT
	switch (m_Status)
	{
	case Status_Begin:
		{
			// 客户端直接进入下一阶段
		}
		break;
	case Status_ReadySpell:
		{
			// 客户端模拟推进
			m_ReadySpellTime = getMax(m_ReadySpellTime - (S32)dt, 0);
			OnReadySpell();
		}
		break;
	case Status_SpellCast:
		{
			OnSpellCast();
		}
		break;
	case Status_ChannelCast:
		{
			// 客户端模拟推进
			m_ChannelCastTime = getMax(m_ChannelCastTime - (S32)dt, 0);
			OnChannelCast();
		}
		break;
	}
	if(m_Status == Status_End)
	{
		OnEnd();
	}
#endif
}

bool Spell::OnConsume()
{
	// ====================== 先检查条件 ==========================
	if(m_bConsume)
		return true;

	if(m_pSkillData->m_Cost.HP >= m_Source->GetStats().HP)
		return false;
	if(m_pSkillData->m_Cost.MP > m_Source->GetStats().MP)
		return false;
	if(m_pSkillData->m_Cost.PP > m_Source->GetStats().PP)
		return false;

	// 检查状态需求
	// 目标、自身前置状态限制
	const BuffTable& bt = m_Source->GetBuffTable();
	bool PreA = false, PreB = false, tPreA = false, tPreB = false;
	PreA = m_pSkillData->GetCastLimit().srcPreBuff_A ? bt.checkBuff(m_pSkillData->GetCastLimit().srcPreBuff_A, m_pSkillData->GetCastLimit().srcPreBuffCount_A, true) : true;
	PreB = m_pSkillData->GetCastLimit().srcPreBuff_B ? bt.checkBuff(m_pSkillData->GetCastLimit().srcPreBuff_B, m_pSkillData->GetCastLimit().srcPreBuffCount_B, true) : true;
	if(m_pSkillData->GetCastLimit().srcPreBuffOp & SkillData::Op_NOT_0) PreA = !PreA;
	if(m_pSkillData->GetCastLimit().srcPreBuffOp & SkillData::Op_NOT_1) PreB = !PreB;
	if(!((m_pSkillData->GetCastLimit().srcPreBuffOp & SkillData::Op_OR_0) ? (PreA || PreB) : (PreA && PreB)))
		return MSG_SKILL_CANNOTDOTHAT;
	if(m_Target)
	{
		tPreA = m_pSkillData->GetCastLimit().tgtPreBuff_A ? bt.checkBuff(m_pSkillData->GetCastLimit().tgtPreBuff_A, m_pSkillData->GetCastLimit().tgtPreBuffCount_A, true) : true;
		tPreB = m_pSkillData->GetCastLimit().tgtPreBuff_B ? bt.checkBuff(m_pSkillData->GetCastLimit().tgtPreBuff_B, m_pSkillData->GetCastLimit().tgtPreBuffCount_B, true) : true;
		if(m_pSkillData->GetCastLimit().srcPreBuffOp & SkillData::Op_NOT_0) tPreA = !tPreA;
		if(m_pSkillData->GetCastLimit().srcPreBuffOp & SkillData::Op_NOT_1) tPreB = !tPreB;
		if(!((m_pSkillData->GetCastLimit().tgtPreBuffOp & SkillData::Op_OR_0) ? (tPreA || tPreB) : (tPreA && tPreB)))
			return MSG_SKILL_CANNOTDOTHAT;
	}

	// ====================== 开始扣除 ==========================
	// 是否成功使用物品？
	if(!m_ItemInterface.onUse((Player*)(m_Source.getObject()), m_Target))
		return false;

	if(m_pSkillData->m_Cost.HP) m_Source->addHP(-m_pSkillData->m_Cost.HP);
	if(m_pSkillData->m_Cost.MP) m_Source->addMP(-m_pSkillData->m_Cost.MP);
	if(m_pSkillData->m_Cost.PP) m_Source->addPP(-m_pSkillData->m_Cost.PP);

	if(m_pSkillData->GetCastLimit().srcPreBuffOp & SkillData::Op_NOT_0) PreA = !PreA;
	if(m_pSkillData->GetCastLimit().srcPreBuffOp & SkillData::Op_NOT_1) PreB = !PreB;
	if(m_pSkillData->GetCastLimit().srcPreBuffOp & SkillData::Op_NOT_0) tPreA = !tPreA;
	if(m_pSkillData->GetCastLimit().srcPreBuffOp & SkillData::Op_NOT_1) tPreB = !tPreB;
	if(m_pSkillData->GetCastLimit().srcPreBuffOp & SkillData::Op_OR_0)
	{
		if(PreA){
			if(m_pSkillData->GetCastLimit().srcPreBuff_A && m_pSkillData->GetCastLimit().srcPreBuffCount_A < 0)
				m_Source->RemoveBuffById(m_pSkillData->GetCastLimit().srcPreBuff_A, mAbs(m_pSkillData->GetCastLimit().srcPreBuffCount_A), Buff::Mask_Remove_Sys, true);
		}
		else if(m_pSkillData->GetCastLimit().srcPreBuff_B && m_pSkillData->GetCastLimit().srcPreBuffCount_B < 0)
			m_Source->RemoveBuffById(m_pSkillData->GetCastLimit().srcPreBuff_B, mAbs(m_pSkillData->GetCastLimit().srcPreBuffCount_B), Buff::Mask_Remove_Sys, true);
	}
	else
	{
		if(m_pSkillData->GetCastLimit().srcPreBuff_A && m_pSkillData->GetCastLimit().srcPreBuffCount_A < 0)
			m_Source->RemoveBuffById(m_pSkillData->GetCastLimit().srcPreBuff_A, mAbs(m_pSkillData->GetCastLimit().srcPreBuffCount_A), Buff::Mask_Remove_Sys, true);
		if(m_pSkillData->GetCastLimit().srcPreBuff_B && m_pSkillData->GetCastLimit().srcPreBuffCount_B < 0)
			m_Source->RemoveBuffById(m_pSkillData->GetCastLimit().srcPreBuff_B, mAbs(m_pSkillData->GetCastLimit().srcPreBuffCount_B), Buff::Mask_Remove_Sys, true);
	}
	if(m_Target && (m_pSkillData->GetCastLimit().tgtPreBuffOp & SkillData::Op_OR_0))
	{
		if(tPreA){
			if(m_pSkillData->GetCastLimit().tgtPreBuff_A && m_pSkillData->GetCastLimit().tgtPreBuffCount_A < 0)
				m_Source->RemoveBuffById(m_pSkillData->GetCastLimit().tgtPreBuff_A, mAbs(m_pSkillData->GetCastLimit().tgtPreBuffCount_A), Buff::Mask_Remove_Sys, true);
		}
		else if(m_pSkillData->GetCastLimit().tgtPreBuff_B && m_pSkillData->GetCastLimit().tgtPreBuffCount_B < 0)
			m_Source->RemoveBuffById(m_pSkillData->GetCastLimit().tgtPreBuff_B, mAbs(m_pSkillData->GetCastLimit().tgtPreBuffCount_B), Buff::Mask_Remove_Sys, true);
	}
	else
	{
		if(m_pSkillData->GetCastLimit().tgtPreBuff_A && m_pSkillData->GetCastLimit().tgtPreBuffCount_A < 0)
			m_Source->RemoveBuffById(m_pSkillData->GetCastLimit().tgtPreBuff_A, mAbs(m_pSkillData->GetCastLimit().tgtPreBuffCount_A), Buff::Mask_Remove_Sys, true);
		if(m_pSkillData->GetCastLimit().tgtPreBuff_B && m_pSkillData->GetCastLimit().tgtPreBuffCount_B < 0)
			m_Source->RemoveBuffById(m_pSkillData->GetCastLimit().tgtPreBuff_B, mAbs(m_pSkillData->GetCastLimit().tgtPreBuffCount_B), Buff::Mask_Remove_Sys, true);
	}
	return true;
}

void Spell::AdvanceScheduleSpell(F32 dt)
{
	for(S32 i=m_ScheduleSpell.size()-1; i>=0; i--)
	{
		if(!m_ScheduleSpell[i] || !(m_ScheduleSpell[i]->Advance(dt)))
		{
			SAFE_DELETE(m_ScheduleSpell[i]);
			m_ScheduleSpell.erase(i);
		}
	}
}

void Spell::AdvanceChainEP(F32 dt)
{
#ifdef NTJ_SERVER
	if(m_ChainEP.skillId)
	{
		m_ChainEP.remainTime -= (S32)dt;
		if(m_ChainEP.remainTime <= 0)
			m_ChainEP.clear();
	}
#endif
}

// 当特效删除时调用
void Spell::DeleteSpellEffect(U32 id)
{
#ifdef NTJ_CLIENT
	Vector<ScheduleSpell*>::iterator it = m_ScheduleSpell.begin();
	for(;it!=m_ScheduleSpell.end(); it++)
	{
		if((*it) && (*it)->mEffectPacket == id)
		{
			AddDelayEP((*it)->mSkillData, (*it)->mSource, (*it)->mTarget);
			SAFE_DELETE(*it);
			m_ScheduleSpell.erase(it);
			break;
		}
	}
	if(m_SourceEffect == id)
		m_SourceEffect = 0;
	if(m_TargetEffect == id)
		m_TargetEffect = 0;
#endif
}

bool Spell::CalculateDamage(Stats& stats, GameObject* pSource, U32 buffId, Vector<U32>* pPlusList)
{
#ifdef NTJ_SERVER
	// 先清除属性
	stats.Clear();

	if(!pSource || buffId == 0)
		return false;

	BuffData* pBuffData = g_BuffRepository.GetBuff(buffId);
	if(!pBuffData || pBuffData->IsFlags(BuffData::Flags_CalcStats))
		return false;

	// 状态本身属性
	stats.Plus(stats, pBuffData->GetStats());
	// 天赋增强
	if(!pPlusList && (pSource->getGameObjectMask() & PlayerObjectType))
	{
		Player* player = (Player*)pSource;
		pPlusList = player->mTalentTable.getIdList(pBuffData->GetSeriesId());
	}
	if(pPlusList)
	{
		BuffData* pPlus;
		for (S32 i=0; i<pPlusList->size(); i++)
		{
			pPlus = g_BuffRepository.GetBuff((*pPlusList)[i]);
			if(pPlus)
				stats.Plus(stats, pPlus->m_Stats);
		}
	}
	stats.PlusDone();
	// 是否叠加施放者属性
	if(pBuffData->IsFlags(BuffData::Flags_PlusSrc) && pSource)
		stats.PlusSrc(pSource->m_BuffTable.GetStats(), pBuffData->m_PlusSrcRate);

#endif
	return true;
}

static Point3F sgSortSpellDest;
int QSORT_CALLBACK cmpSearchSpellDest(const void* inP1, const void* inP2)
{
	SceneObject* p1 = *((SceneObject**)inP1);
	SceneObject* p2 = *((SceneObject**)inP2);
	Point3F temp;
	F32 d1, d2;

	if (bool(p1))
	{
		d1 = (p1->getPosition() - sgSortSpellDest).lenSquared();
	}
	else
	{
		d1 = 0;
	}
	if (p2)
	{
		d2 = (p2->getPosition() - sgSortSpellDest).lenSquared();
	}
	else
	{
		d2 = 0;
	}

	if (d1 > d2)
		return 1;
	else if (d1 < d2)
		return -1;
	else
		return 0;
}

void Spell::SpellDamage(GameObject* pSource, GameObject* pTarget, U32 buffId, Vector<U32>* pPlusList)
{
#ifdef NTJ_SERVER
	if(!pTarget || !CalculateDamage(g_Stats, pSource, buffId, pPlusList))
		return;

	pTarget->ApplyDamage(g_Stats, pSource, buffId);
#endif
}

void Spell::SpellDamage_Region(GameObject* pSource, Point3F& pos, U32 skillId, bool skillEP /* = false */, Vector<U32>* pPlusList /* = NULL */)
{
#ifdef NTJ_SERVER
	if(!pSource || skillId == 0)
		return;

	SkillData* pData = g_SkillRepository.GetSkill(skillId);
	if(!pData)
		return;

	static bool applyDam;
	static Box3F region;
	static F32 x,y,z,lenSq;
	static Point3F delta, dest;
	static MatrixF mat;

	applyDam = CalculateDamage(g_Stats, pSource, pData->m_Operation[SkillData::Selectable_A].buffId, pPlusList);

	// 取得范围box
	bool bCircle = (pData->m_EffectLimit.region == SkillData::Region_Circle);
	if(bCircle)
	{
		x = y = z = pData->m_EffectLimit.regionRect.x;
		lenSq = x * x;
		region.max.set(pos.x + x, pos.y + y, pos.z + z);
		region.min.set(pos.x - x, pos.y - y, pos.z - z);
	}
	else
	{
		x = pData->m_EffectLimit.regionRect.x;
		y = pData->m_EffectLimit.regionRect.y;
		z = (pData->m_EffectLimit.regionRect.x + pData->m_EffectLimit.regionRect.y) / 2;
		// 范围
		region.max.set(x, y, z);
		region.min.set(-x, -y, -z);
		// 范围真正的中心点
		delta = pos - pSource->getPosition();
		delta.normalize(y);
		dest = delta + pSource->getPosition();
		// 旋转
		delta.z = mAtan( delta.x, delta.y );
		delta.x = 0;
		delta.y = 0;
		mat.set(delta);
		mat.mul(region);
		// 位移
		region.max += dest;
		region.min += dest;
	}

	// 在box内的物体
	SimpleQueryList queryList;
	gServerContainer.findObjects(region, GameObjectType, SimpleQueryList::insertionCallback, &queryList, pSource->getLayerID() ? pSource->getLayerID() : 1);
	if (queryList.mList.size() > 1 && (pData->m_Cast.attenuation > POINT_EPSILON || pData->IsFlags(SkillData::Flags_ChainEP)))
	{
		sgSortSpellDest = pos;
		dQsort(queryList.mList.address(), queryList.mList.size(), sizeof(SceneObject*), cmpSearchSpellDest);
	}

	if(pData->IsFlags(SkillData::Flags_ChainEP))
		pSource->GetSpell().AddChainEP(pData->GetID(), pSource->getId());
	if(skillEP)
		pSource->setSkillDataEP(pData->GetID(), SkillData::Selectable_A);

	if(!bCircle)
	{
		region.max.set(x, y, z);
		region.min.set(-x, -y, -z);
		delta.z = -delta.z;
		mat.set(delta);
	}
	for (U32 i=0, j=pData->m_EffectLimit.maxTargets; i<queryList.mList.size() && j>0; ++i, --j)
	{
		// 去掉不在圆内的，忽略z轴
		if(bCircle)
		{
			delta = pos - queryList.mList[i]->getPosition();
			if(delta.x * delta.x + delta.y * delta.y > lenSq)
				continue;
		}
		else
		{
			delta = dest - queryList.mList[i]->getPosition();
			mat.mulV(delta);
			if(!region.isContained(delta))
				continue;
		}
		// 范围技能只有针对一种目标
		U32 CanAtk = pSource->CanAttackTarget((GameObject*)queryList.mList[i]) ? SkillData::Target_Enemy : SkillData::Target_Friend;
		if(CanAtk & pData->m_CastLimit.targetSl[SkillData::Selectable_A])
		{
			if(!IsEffectObject(pSource, (GameObject*)queryList.mList[i], NULL, pData->m_EffectLimit.effectTarget[SkillData::Selectable_A], (CanAtk == SkillData::Target_Enemy) ? true : false))
				continue;
			if(applyDam)
			{
				((GameObject*)queryList.mList[i])->ApplyDamage(g_Stats, pSource, pData->m_Operation[SkillData::Selectable_A].buffId);
				g_Stats *= mClampF(1.0f - pData->m_Cast.attenuation, 0.0f, 1.0f);
			}
			if(pData->IsFlags(SkillData::Flags_ChainEP))
				pSource->GetSpell().AddChainEP(pData->GetID(), ((GameObject*)queryList.mList[i])->getId());
			// 相关触发
			Spell::OnTrigger(pSource, (GameObject*)queryList.mList[i], pData, SkillData::Selectable_A, g_Stats, false, true);
			if(skillEP)
				pSource->addTargetObjEP(((GameObject*)queryList.mList[i])->getId());
		}
		else if(CanAtk & pData->m_CastLimit.targetSl[SkillData::Selectable_B])
		{
			if(!IsEffectObject(pSource, (GameObject*)queryList.mList[i], NULL, pData->m_EffectLimit.effectTarget[SkillData::Selectable_B], (CanAtk == SkillData::Target_Enemy) ? true : false))
				continue;
			if(applyDam)
			{
				((GameObject*)queryList.mList[i])->ApplyDamage(g_Stats, pSource, pData->m_Operation[SkillData::Selectable_B].buffId);
				g_Stats *= mClampF(1.0f - pData->m_Cast.attenuation, 0.0f, 1.0f);
			}
			if(pData->IsFlags(SkillData::Flags_ChainEP))
				pSource->GetSpell().AddChainEP(pData->GetID(), ((GameObject*)queryList.mList[i])->getId());
			// 相关触发
			Spell::OnTrigger(pSource, (GameObject*)queryList.mList[i], pData, SkillData::Selectable_B, g_Stats, false, true);
			if(skillEP)
				pSource->addTargetObjEP(((GameObject*)queryList.mList[i])->getId());
		}
	}
#endif
}

void Spell::SpellDamage(Stats& stats, GameObject* pSource, GameObject* pTarget, U32 buffId)
{
#ifdef NTJ_SERVER
	if(!pTarget)
		return;

	pTarget->ApplyDamage(stats, pSource, buffId);
#endif
}

void Spell::SpellDamage_Region(Stats& stats, GameObject* pSource, Point3F& pos, U32 skillId)
{
#ifdef NTJ_SERVER
	if(!pSource || skillId == 0)
		return;

	SkillData* pData = g_SkillRepository.GetSkill(skillId);
	if(!pData)
		return;

	static Box3F region;
	static F32 x,y,z,lenSq;
	static Point3F delta, dest;
	static MatrixF mat;

	// 取得范围box
	bool bCircle = (pData->m_EffectLimit.region == SkillData::Region_Circle);
	if(bCircle)
	{
		x = y = z = pData->m_EffectLimit.regionRect.x;
		lenSq = x * x;
		region.max.set(pos.x + x, pos.y + y, pos.z + z);
		region.min.set(pos.x - x, pos.y - y, pos.z - z);
	}
	else
	{
		x = pData->m_EffectLimit.regionRect.x;
		y = pData->m_EffectLimit.regionRect.y;
		z = (pData->m_EffectLimit.regionRect.x + pData->m_EffectLimit.regionRect.y) / 2;
		// 范围
		region.max.set(x, y, z);
		region.min.set(-x, -y, -z);
		// 范围真正的中心点
		delta = pos - pSource->getPosition();
		delta.normalize(y);
		dest = delta + pSource->getPosition();
		// 旋转
		delta.z = mAtan( delta.x, delta.y );
		delta.x = 0;
		delta.y = 0;
		mat.set(delta);
		mat.mul(region);
		// 位移
		region.max += dest;
		region.min += dest;
	}

	// 在box内的物体
	SimpleQueryList queryList;
	gServerContainer.findObjects(region, GameObjectType, SimpleQueryList::insertionCallback, &queryList, pSource->getLayerID() ? pSource->getLayerID() : 1);
	if (queryList.mList.size() > 1 && (pData->m_Cast.attenuation > POINT_EPSILON || pData->IsFlags(SkillData::Flags_ChainEP)))
	{
		sgSortSpellDest = pos;
		dQsort(queryList.mList.address(), queryList.mList.size(), sizeof(SceneObject*), cmpSearchSpellDest);
	}

	if(pData->IsFlags(SkillData::Flags_ChainEP))
		pSource->GetSpell().AddChainEP(pData->GetID(), pSource->getId());

	if(!bCircle)
	{
		region.max.set(x, y, z);
		region.min.set(-x, -y, -z);
		delta.z = -delta.z;
		mat.set(delta);
	}
	for (U32 i=0, j=pData->m_EffectLimit.maxTargets; i<queryList.mList.size() && j>0; ++i, --j)
	{
		// 去掉不在圆内的，忽略z轴
		if(bCircle)
		{
			delta = pos - queryList.mList[i]->getPosition();
			if(delta.x * delta.x + delta.y * delta.y > lenSq)
				continue;
		}
		else
		{
			delta = dest - queryList.mList[i]->getPosition();
			mat.mulV(delta);
			if(!region.isContained(delta))
				continue;
		}
		// 范围技能只有针对一种目标
		U32 CanAtk = pSource->CanAttackTarget((GameObject*)queryList.mList[i]) ? SkillData::Target_Enemy : SkillData::Target_Friend;
		if(CanAtk & pData->m_CastLimit.targetSl[SkillData::Selectable_A])
		{
			if(!IsEffectObject(pSource, (GameObject*)queryList.mList[i], NULL, pData->m_EffectLimit.effectTarget[SkillData::Selectable_A], (CanAtk == SkillData::Target_Enemy) ? true : false))
				continue;
			((GameObject*)queryList.mList[i])->ApplyDamage(stats, pSource, pData->m_Operation[SkillData::Selectable_A].buffId);
			stats *= mClampF(1.0f - pData->m_Cast.attenuation, 0.0f, 1.0f);
			if(pData->IsFlags(SkillData::Flags_ChainEP))
				pSource->GetSpell().AddChainEP(pData->GetID(), ((GameObject*)queryList.mList[i])->getId());
			// 相关触发
			Spell::OnTrigger(pSource, (GameObject*)queryList.mList[i], pData, SkillData::Selectable_A, stats, false, true);
		}
		else if(CanAtk & pData->m_CastLimit.targetSl[SkillData::Selectable_B])
		{
			if(!IsEffectObject(pSource, (GameObject*)queryList.mList[i], NULL, pData->m_EffectLimit.effectTarget[SkillData::Selectable_B], (CanAtk == SkillData::Target_Enemy) ? true : false))
				continue;
			((GameObject*)queryList.mList[i])->ApplyDamage(stats, pSource, pData->m_Operation[SkillData::Selectable_B].buffId);
			stats *= mClampF(1.0f - pData->m_Cast.attenuation, 0.0f, 1.0f);
			if(pData->IsFlags(SkillData::Flags_ChainEP))
				pSource->GetSpell().AddChainEP(pData->GetID(), ((GameObject*)queryList.mList[i])->getId());
			// 相关触发
			Spell::OnTrigger(pSource, (GameObject*)queryList.mList[i], pData, SkillData::Selectable_B, stats, false, true);
		}
	}
#endif
}

void Spell::OnTrigger(GameObject* src, GameObject* tgt, SkillData* pData, SkillData::Selectable sl, Stats& stats, bool triSrc /* = true */, bool triTgt /* = true */)
{
#ifdef NTJ_SERVER
	if(!pData)
		return;
	if(triTgt && tgt)
	{
		if(pData->m_Trigger[sl].buffId_A)
			tgt->AddBuff(Buff::Origin_Buff, pData->m_Trigger[sl].buffId_A, src, pData->m_Trigger[sl].buffCount_A);
		if(pData->m_Trigger[sl].buffId_B)
			tgt->AddBuff(Buff::Origin_Buff, pData->m_Trigger[sl].buffId_B, src, pData->m_Trigger[sl].buffCount_B);
		//if(pData->m_Trigger[sl].skillId_A)
		//	tgt->GetSpell().OnTriggerSpell(pData->m_Trigger[sl].skillId_A, src, stats, sl, times, trace);
		//if(pData->m_Trigger[sl].skillId_B)
		//	tgt->GetSpell().OnTriggerSpell(pData->m_Trigger[sl].skillId_B, src, stats, sl, times, trace);
	}
	if(triSrc && src)
	{
		if(pData->m_Trigger[sl].buffId_C)
			src->AddBuff(Buff::Origin_Buff, pData->m_Trigger[sl].buffId_C, src, pData->m_Trigger[sl].buffCount_C);
		//if(pData->m_Trigger[sl].skillId_C)
		//	src->GetSpell().OnTriggerSpell(pData->m_Trigger[sl].skillId_C, src, stats, sl, times, trace);
	}

	// to do : 触发脚本
#endif
}

bool Spell::IsEffectObject(GameObject* pSource, GameObject* pObject, GameObject* pTarget, U32 effectTarget, bool canAtk)
{
	if(!pSource || !pObject || !effectTarget)
		return false;
	if(effectTarget & SkillData::EffectTarget_Self)
	{
		if(pSource == pObject)
			return true;
	}
	if(effectTarget & SkillData::EffectTarget_Target)
	{
		if(pObject == pTarget)
			return true;
	}
	if(effectTarget & SkillData::EffectTarget_Friend)
	{
		if(!canAtk && pSource != pObject && pObject != pTarget)
			return true;
	}
	if(effectTarget & SkillData::EffectTarget_Enemy)
	{
		if(canAtk && pSource != pObject && pObject != pTarget)
			return true;
	}
	if(effectTarget & SkillData::EffectTarget_Team)
	{
		if(1)
			return true;
	}
	if(effectTarget & SkillData::EffectTarget_Raid)
	{
		if(1)
			return true;
	}
	return false;
}

