//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/Buff.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "math/mMathFn.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Effects/EffectPacket.h"


// ========================================================================================================================================
//  CountBitSet
// ========================================================================================================================================

CountBitSet::CountBitSet()
{
	clearAll();
}

void CountBitSet::clearAll()
{
	_bits = 0;
	dMemset(this, 0, sizeof(CountBitSet));
}

void CountBitSet::clear(U32 bits)
{
	if(0 == (_bits & bits))
		return;
	_bits &= ~bits;
	for (S32 i=0; i<CountBitSet_Max; ++i)
	{
		if(bits & BIT(i))
			_counts[i] = 0;
	}
}

void CountBitSet::set(U32 bits, U32 counts)
{
	if(0 == bits)
		return;

	if(0 == counts)
		_bits &= ~bits;
	else
		_bits |= bits;

	for (S32 i=0; i<CountBitSet_Max; ++i)
	{
		if(bits & BIT(i))
			_counts[i] = counts;
	}
}

bool CountBitSet::test(U32 bits) const
{
	return (bool)(_bits & bits);
}

void CountBitSet::add(U32 bits, U32 counts /* = 1 */)
{
	if(0 == bits || 0 == counts)
		return;
	for (S32 i=0; i<CountBitSet_Max; ++i)
	{
		if(bits & BIT(i))
		{
			_counts[i] += counts;

			if(0 == _counts[i])
				_bits &= ~BIT(i);
			else
				_bits |= BIT(i);
		}
	}
}

void CountBitSet::sub(U32 bits, U32 counts /* = 1 */)
{
	if(0 == bits || 0 == counts)
		return;
	for (S32 i=0; i<CountBitSet_Max; ++i)
	{
		if(bits & BIT(i))
		{
			_counts[i] = (_counts[i] > counts) ? (_counts[i] - counts) : 0;

			if(0 == _counts[i])
				_bits &= ~BIT(i);
			else
				_bits |= BIT(i);
		}
	}
}

// ========================================================================================================================================
//  Buff
// ========================================================================================================================================

Buff::Buff():
	m_BuffData(NULL),
	m_Origin(Origin_Base),
	m_Count(0),
	m_RemainTime(0),
	m_RemainTimes(0),
	m_LimitTime(0),
	m_LimitTimes(0),
	m_TriggerCDTime(0),
	m_TimeScale(1.0f),
	m_TickMask(0),
	m_RemoveMask(0)
{
}

Buff::~Buff()
{
}

void Buff::Remove(GameObject* pObj, U32 mask)
{
	if(!m_RemoveMask)
	{
		m_RemoveMask = mask;
		if(m_RemoveMask && pObj)
		{
			pObj->mShapeStack.remove(ShapeStack::Stack_Buff, m_BuffData->m_ShapesSetId);
			pObj->mShapeStack.refresh(pObj);
		}
	}
}

bool Buff::Process(GameObject* pObj, SimTime dt)
{
	if(m_RemoveMask)
		return false;
	if(m_Count == 0 || m_BuffData->m_LimitTime == 0 || (pObj->isDisabled() && !m_BuffData->IsFlags(BuffData::Flags_ExistOnDisabled)))
	{
		Remove(pObj, Mask_Remove_Sys);
		return false;
	}

	m_RemainTime = getMax(0, (S32)m_RemainTime - (S32)dt);
	S32 tickTimes = m_RemainTimes - mCeil((F32)m_RemainTime/m_BuffData->m_LimitTime * m_BuffData->m_LimitTimes);
	while(tickTimes > 0)
	{
		// 按时触发
		ProcessTick(pObj);
		m_TickMask = BuffData::Trigger_BuffTick;
		--tickTimes;
		--m_RemainTimes;
	}
	if(0 == m_RemainTime)
	{
		if(m_BuffData->IsFlags(BuffData::Flags_Infinity))
			m_RemainTime = m_BuffData->m_LimitTime;
		else
			Remove(pObj, BuffData::Trigger_RemoveTimer);
	}

	if(m_TickMask)
	{
		// 注意触发安全，如果这里出现问题，只能要求策划做修改了
		onTrigger(m_TickMask, pObj, NULL);
		m_TickMask = 0;
	}
	return (bool)m_RemainTime;
}

void Buff::ProcessTick(GameObject* pObj)
{
	if(m_BuffData->GetDamageType())
	{
		g_Stats.Clear();
		g_Stats.Add(m_BuffStats, m_Count);
		Spell::SpellDamage(g_Stats, m_pSource, pObj, m_BuffData->GetBuffId());
	}
}

void Buff::onTrigger(U32 mask, GameObject* carrier, GameObject* trigger)
{
	if(!carrier || !(mask & m_BuffData->m_TotalMask) || (m_RemoveMask && !(m_RemoveMask & mask)))
		return;

	// 如果遇到移除事件，不进行其他触发
	if(mask & m_BuffData->m_RemoveMask)
	{
		Remove(carrier, BuffData::Trigger_RemoveTimer);
		return;
	}
	if((mask & m_BuffData->m_CountMask) && m_Count > 0)
	{
		--m_Count;
		if(0 >= m_Count)
		{
			Remove(carrier, BuffData::Trigger_RemoveCounter);
			return;
		}
	}

	for (S32 i=0; i<BuffData::TriggerMax; ++i)
	{
		if (mask & m_BuffData->m_Trigger[i].mask)
		{
			BuffData::Trigger& tri = m_BuffData->m_Trigger[i];
			if(tri.buffId_A)
			{
				if(BuffData::TT_Trigger == tri.buffTT_A && trigger)
					trigger->AddBuff(Buff::Origin_Buff, tri.buffId_A, m_pSource, tri.buffCount_A);
				else if(BuffData::TT_Carrier == tri.buffTT_A)
					carrier->AddBuff(Buff::Origin_Buff, tri.buffId_A, m_pSource, tri.buffCount_A);
				else if(BuffData::TT_Target == tri.buffTT_A && carrier->GetTarget())
					carrier->GetTarget()->AddBuff(Buff::Origin_Buff, tri.buffId_A, m_pSource, tri.buffCount_A);
				else if(m_pSource)
					m_pSource->AddBuff(Buff::Origin_Buff, tri.buffId_A, m_pSource, tri.buffCount_A);
			}
			if(tri.buffId_B)
			{
				if(BuffData::TT_Trigger == tri.buffTT_B && trigger)
					trigger->AddBuff(Buff::Origin_Buff, tri.buffId_B, m_pSource, tri.buffCount_B);
				else if(BuffData::TT_Carrier == tri.buffTT_B)
					carrier->AddBuff(Buff::Origin_Buff, tri.buffId_B, m_pSource, tri.buffCount_B);
				else if(BuffData::TT_Target == tri.buffTT_B && carrier->GetTarget())
					carrier->GetTarget()->AddBuff(Buff::Origin_Buff, tri.buffId_B, m_pSource, tri.buffCount_B);
				else if(m_pSource)
					m_pSource->AddBuff(Buff::Origin_Buff, tri.buffId_B, m_pSource, tri.buffCount_B);
			}
			if(tri.buffId_C)
			{
				if(BuffData::TT_Trigger == tri.buffTT_C && trigger)
					trigger->AddBuff(Buff::Origin_Buff, tri.buffId_C, m_pSource, tri.buffCount_C);
				else if(BuffData::TT_Carrier == tri.buffTT_C)
					carrier->AddBuff(Buff::Origin_Buff, tri.buffId_C, m_pSource, tri.buffCount_C);
				else if(BuffData::TT_Target == tri.buffTT_C && carrier->GetTarget())
					carrier->GetTarget()->AddBuff(Buff::Origin_Buff, tri.buffId_C, m_pSource, tri.buffCount_C);
				else if(m_pSource)
					m_pSource->AddBuff(Buff::Origin_Buff, tri.buffId_C, m_pSource, tri.buffCount_C);
			}
			if(tri.skillId_A)
				triggerSpell(tri.skillId_A, tri.skillTT_A, m_pSource, carrier, trigger);
			if(tri.skillId_B)
				triggerSpell(tri.skillId_B, tri.skillTT_B, m_pSource, carrier, trigger);
			if(tri.skillId_C)
				triggerSpell(tri.skillId_C, tri.skillTT_C, m_pSource, carrier, trigger);
		}
	}
}

void Buff::triggerSpell(U32 skillId, U32 tgtType, GameObject* source, GameObject* carrier, GameObject* trigger)
{
	if(!carrier || !skillId)
		return;
	SkillData* pData = g_SkillRepository.GetSkill(skillId);
	if(!pData)
		return;

	// 确定目标
	GameObject* target = NULL;
	switch (tgtType)
	{
	case BuffData::TT_Source:
		target = source;
		break;
	case BuffData::TT_Carrier:
		target = carrier;
		break;
	case BuffData::TT_Trigger:
		target = trigger;
		break;
	case BuffData::TT_Target:
		target = carrier ? carrier->GetTarget() : NULL;
		break;
	case BuffData::TT_NearestFriend:
		break;
	case BuffData::TT_NearestEnemy:
		break;
	}
	if(!target)
		return;

	if(!pData->IsRegion())
		Spell::SpellDamage(source, target, pData->m_Operation[SkillData::Selectable_A].buffId);
	else
		Spell::SpellDamage_Region(source, target->getPosition(), pData->GetID());

	// 触发技能和状态
	Spell::OnTrigger(carrier, target, pData, SkillData::Selectable_A, g_Stats, true, !pData->IsRegion());
}


// ========================================================================================================================================
//  BuffTable
// ========================================================================================================================================

BuffTable::BuffTable() :
	m_pObj(NULL)
{
	m_Refresh = false;
	m_RefreshSet = false;
	m_TriggerLocked = false;
	m_EffectMask = 0;
}

BuffTable::~BuffTable()
{
	Clear();
}

void BuffTable::Clear()
{
	clearTrigger();
	BuffTableMap::iterator itr;
	for(itr = m_General.begin(); itr != m_General.end(); ++itr)
		delete itr->second;
	S32 i;
	for(i = m_Buff.size()-1; i >= 0; --i)
		SAFE_DELETE(m_Buff[i]);
	for(i = m_Debuff.size()-1; i >= 0; --i)
		SAFE_DELETE(m_Debuff[i]);
	for(i = m_System.size()-1; i >= 0; --i)
		SAFE_DELETE(m_System[i]);

	m_General.clear();
	m_Buff.clear();
	m_Debuff.clear();
	m_System.clear();
	m_Stats.Clear();
	m_Summation.Clear();
	m_Refresh = false;
	m_RefreshSet = false;
	m_TriggerLocked = false;
	m_EffectMask = 0;
}

bool BuffTable::SaveData(stPlayerStruct* playerInfo)
{
#ifdef NTJ_SERVER
	// 30秒以下的状态不保存
	const static SimTime clampTickMs = 30000;
	if(!playerInfo)
	{
		AssertFatal(false, "BuffTable::SaveData error!!");
		return false;
	}

	BuffTableType::iterator it = m_Buff.begin();
	for(S32 i=0; it!=m_Buff.end() || i>=BUFF_MAXSLOTS; ++it)
	{
		if(it && (*it)->m_BuffData && (*it)->m_RemainTime > clampTickMs && !(*it)->m_BuffData->IsFlags(BuffData::Flags_Coexist))
		{
			playerInfo->MainData.BuffInfo[i].buffId = (*it)->m_BuffData->GetBuffId();
			playerInfo->MainData.BuffInfo[i].time = (*it)->m_RemainTime;
			playerInfo->MainData.BuffInfo[i].count = (*it)->m_Count;
			playerInfo->MainData.BuffInfo[i].self = ((*it)->m_pSource == m_pObj);
			++i;
		}
	}
	it = m_Debuff.begin();
	for(S32 i=0; it!=m_Debuff.end() || i>=BUFF_MAXSLOTS; ++it)
	{
		if((*it) && (*it)->m_BuffData && (*it)->m_RemainTime > clampTickMs && !(*it)->m_BuffData->IsFlags(BuffData::Flags_Coexist))
		{
			playerInfo->MainData.DebuffInfo[i].buffId = (*it)->m_BuffData->GetBuffId();
			playerInfo->MainData.DebuffInfo[i].time = (*it)->m_RemainTime;
			playerInfo->MainData.DebuffInfo[i].count = (*it)->m_Count;
			playerInfo->MainData.DebuffInfo[i].self = ((*it)->m_pSource == m_pObj);
			++i;
		}
	}
	it = m_System.begin();
	for(S32 i=0; it!=m_System.end() || i>=BUFF_MAXSLOTS; ++it)
	{
		if((*it) && (*it)->m_BuffData && (*it)->m_RemainTime > clampTickMs && !(*it)->m_BuffData->IsFlags(BuffData::Flags_Coexist))
		{
			playerInfo->MainData.SysBuffInfo[i].buffId = (*it)->m_BuffData->GetBuffId();
			playerInfo->MainData.SysBuffInfo[i].time = (*it)->m_RemainTime;
			playerInfo->MainData.SysBuffInfo[i].count = (*it)->m_Count;
			playerInfo->MainData.SysBuffInfo[i].self = ((*it)->m_pSource == m_pObj);
			++i;
		}
	}
#endif
	return true;
}

bool BuffTable::AddBuff(Buff::Origin _origin, U32 _buffId, GameObject* _src, S32 _count /* = 1 */, SimTime _time /* = 0 */)
{
	BuffData* pBuffData = g_BuffRepository.GetBuff(_buffId);
	if(!pBuffData || !m_pObj)
		return false;

	// 如果是重复性施放
	BuffTableType* pTable = NULL;
	S32 newCount = _count;
	if(_origin == Buff::Origin_Buff)
	{
		// 免疫
		if((pBuffData->m_Group & m_ImmunitySet).any())
			return false;

		// 直接产生伤害
		if(!pBuffData->IsFlags(BuffData::Flags_CalcStats) && pBuffData->m_LimitTime == 0)
		{
			Spell::SpellDamage(_src, m_pObj, pBuffData->GetBuffId());
			return true;
		}

		// 确定属于哪个列表
		if(pBuffData->m_Flags & BuffData::Flags_System)
			pTable = &m_System;
		else if(pBuffData->m_Flags & BuffData::Flags_Buff)
			pTable = &m_Buff;
		else
			pTable = &m_Debuff;
		// 遍历所有状态，查找是否有相同对象施放了相同的状态
		for(S32 i=pTable->size()-1; i>=0; --i)
		{
			Buff* pBuff = (*pTable)[i];
			if((pBuff->m_BuffData->GetSeriesId() == Macro_GetBuffSeriesId(_buffId) && (_count < 0 || pBuffData->IsFlags(BuffData::Flags_ReplaceUpper) || pBuff->m_BuffData->GetLevel() <= Macro_GetBuffLevel(_buffId)))
				&& (pBuff->m_pSource == _src || !pBuff->m_BuffData->IsFlags(BuffData::Flags_Coexist)))
			{
				// 叠加删除前的状态个数
				newCount = mClamp(_count + pBuff->m_Count, 0, pBuffData->m_LimitCount);
				// 只是状态叠加减少
				if(_count < 0 && newCount > 0){
					RemoveBuff((*pTable), pBuff->m_BuffData->GetBuffId(), i, mAbs(_count), Buff::Mask_Remove_Sys);
					return true;
				}
				else{
					RemoveBuff((*pTable), pBuff->m_BuffData->GetBuffId(), i, pBuff->m_Count, Buff::Mask_Remove_Sys);
					break;
				}
			}
		}
		if(newCount <= 0)
			return true;
		// 有组触发，且要求删除自己
		if(onTriggerGroup(pBuffData, _src))
			return true;
	}
	else if(!pBuffData->IsFlags(BuffData::Flags_CalcStats))
		return false;	// m_General状态必须是计算属性的状态

	Buff *pBuff = new Buff;
	pBuff->m_BuffData = pBuffData;
	pBuff->m_Origin = _origin;
	pBuff->m_pSource = _src;
	// 计算属性增强
	g_BuffData.Clear();
	g_BuffData.Plus(g_BuffData, *pBuffData);
	if(_src && (_src->getGameObjectMask() & PlayerObjectType))
	{
		Player* player = (Player*)(_src);
		Vector<U32>* plusList = player->mTalentTable.getIdList(pBuffData->GetSeriesId());
		if(plusList && plusList->size())
		{
			BuffData* pPlus;
			for (S32 i=0; i<plusList->size(); i++)
			{
				pPlus = g_BuffRepository.GetBuff((*plusList)[i]);
				g_BuffData.Plus(g_BuffData, *pPlus);
				pBuff->m_Plus.push_back(pPlus->m_BuffId);
			}
		}
	}
	g_BuffData.PlusDone();
	pBuff->m_BuffStats.Clear();
	pBuff->m_BuffStats += g_BuffData.GetStats();
	// DOT HOT 可能需要叠加人物属性
	if(pBuffData->IsFlags(BuffData::Flags_PlusSrc) && _src)
		pBuff->m_BuffStats.PlusSrc(_src->GetStats(), pBuffData->m_PlusSrcRate);

	// 被天赋增强的属性
	pBuff->m_Count = mClamp(newCount, 1, g_BuffData.m_LimitCount);
	pBuff->m_RemainTime = pBuff->m_LimitTime = g_BuffData.m_LimitTime;
	pBuff->m_RemainTimes = pBuff->m_LimitTimes = g_BuffData.m_LimitTimes;
	// 调整剩余时间
	if(_time > 0 && _time < pBuff->m_RemainTime)
	{
		pBuff->m_RemainTime = _time;
		pBuff->m_RemainTimes = mCeil((F32)_time/pBuff->m_LimitTime * pBuff->m_LimitTimes);
	}

	// 添加时触发tick
	if(pBuffData->IsFlags(BuffData::Flags_OnAddTickSpell))
		pBuff->m_TickMask = BuffData::Trigger_BuffTick;

	if(pTable)
	{
		pTable->push_back(pBuff);
		//m_pObj->setMaskBits(GameObject::BuffMask);

		// 注意目前参与触发的状态是Buff::Origin_Buff类型的状态
		m_TriggerMask.add(pBuff->m_BuffData->m_TotalMask);
	}
	else
		m_General.insert(BuffTableMap::value_type(_buffId, pBuff));

	if(pBuffData->IsFlags(BuffData::Flags_CalcStats))
	{
		m_Summation.Add(pBuff->m_BuffStats, pBuffData->IsFlags(BuffData::Flags_PlusStats) ? pBuff->m_Count : 1);
		m_Refresh = true;
	}
	// 隐身
	if(pBuffData->GetEffect() & BuffData::Effect_Invisibility)
		m_pObj->onInvisible();
#ifdef NTJ_SERVER
	// 嘲讽
	if(pBuffData->GetEffect() & BuffData::Effect_Mock)
		m_pObj->mHateList.setUpdateForce();
#endif

	m_RefreshSet = true;
	if(pBuff->m_BuffData->m_ShapesSetId)
	{
		m_pObj->mShapeStack.add(ShapeStack::Stack_Buff, pBuff->m_BuffData->m_ShapesSetId);
		m_pObj->mShapeStack.refresh(m_pObj);
	}
	return true;
}

bool BuffTable::RemoveBuff(U32 _buffId)
{
	// buff通过其他方法删除
	BuffData* pBuffData = g_BuffRepository.GetBuff(_buffId);
	if(!pBuffData || !m_pObj)
		return false;

	BuffTableMap::iterator itr = m_General.find(_buffId);
	if(itr == m_General.end() || !itr->second)
	{
		AssertFatal(false, "BuffTable::RemoveBuff !");
		return false;
	}

	// 计算属性增强
	Buff* pBuff = itr->second;
	/*
	g_BuffData.Clear();
	g_BuffData.Plus(g_BuffData, *pBuffData);
	BuffData* pPlus;

	for (S32 i=0; i<pBuff->m_Plus.size(); i++)
	{
		pPlus = g_BuffRepository.GetBuff(pBuff->m_Plus[i]);
		if(pPlus)
			g_BuffData.Plus(g_BuffData, *pPlus);
	}
	*/

	if(pBuff->m_BuffData->IsFlags(BuffData::Flags_CalcStats))
	{
		m_Summation.Add(pBuff->m_BuffStats, pBuffData->IsFlags(BuffData::Flags_PlusStats) ? -(S32)pBuff->m_Count : -1);
		m_Refresh = true;
	}

	m_RefreshSet = true;
	//m_TriggerMask.sub(pBuff->m_BuffData->m_TotalMask);
	if(pBuff->m_BuffData->m_ShapesSetId && !pBuff->m_RemoveMask)
	{
		m_pObj->mShapeStack.remove(ShapeStack::Stack_Buff, pBuff->m_BuffData->m_ShapesSetId);
		m_pObj->mShapeStack.refresh(m_pObj);
	}
	delete pBuff;
	m_General.erase(itr);
	return true;
}

bool BuffTable::RemoveBuff(BuffTableType& _buffTab, U32 _buffId, U32 index, U32 _count /* = 1 */, U32 mask /* = Buff::Mask_Remove_Sys */)
{
	if(index >= _buffTab.size() || !m_pObj)
		return false;
	Buff* pBuff = _buffTab[index];
	AssertFatal( pBuff && pBuff->m_BuffData, "BuffTable::RemoveBuff !");
	// 可能由于网络延迟的关系，出现偏差
	if(Macro_GetBuffSeriesId(_buffId) != pBuff->m_BuffData->GetSeriesId())
		return false;

	// 叠加删除前的状态个数
	U32 newCount = pBuff->m_Count>_count ? (pBuff->m_Count-_count) : 0;
	// 只是状态叠加减少，此时不进行触发
	if(newCount > 0)
	{
		if(pBuff->m_BuffData->IsFlags(BuffData::Flags_CalcStats) && pBuff->m_BuffData->IsFlags(BuffData::Flags_PlusStats))
		{
			m_Summation.Add(pBuff->m_BuffStats, -(S32)_count);
			m_Refresh = true;
		}
		pBuff->m_Count = newCount;
		m_pObj->setMaskBits(GameObject::BuffMask);
		return true;
	}

	// 非系统删除有触发
	if(mask & Buff::Mask_Remove)
		pBuff->onTrigger(mask, m_pObj, NULL);

	// 计算属性的状态
	if(pBuff->m_BuffData->IsFlags(BuffData::Flags_CalcStats))
	{
		/*
		// 计算属性增强
		g_BuffData.Clear();
		g_BuffData.Plus(g_BuffData, *pBuff->m_BuffData);
		BuffData* pPlus;

		for (S32 i=0; i<pBuff->m_Plus.size(); i++)
		{
			pPlus = g_BuffRepository.GetBuff(pBuff->m_Plus[i]);
			if(pPlus)
				g_BuffData.Plus(g_BuffData, *pPlus);
		}
		*/

		m_Summation.Add(pBuff->m_BuffStats, pBuff->m_BuffData->IsFlags(BuffData::Flags_PlusStats) ? -(S32)pBuff->m_Count : -1);
		m_Refresh = true;
	}

#ifdef NTJ_SERVER
	if(mask != Buff::Mask_Remove_Sys)
	{
		// 脚本触发
		char spt[1024];
		ParsePlayerScript(spt, pBuff->m_BuffData->m_OnRemoveScript, m_pObj->getId());
		if(spt && spt[0])
			Con::evaluatef(spt);
	}
#endif
	m_RefreshSet = true;
	m_TriggerMask.sub(pBuff->m_BuffData->m_TotalMask);
	if(pBuff->m_BuffData->m_ShapesSetId && !pBuff->m_RemoveMask)
	{
		m_pObj->mShapeStack.remove(ShapeStack::Stack_Buff, pBuff->m_BuffData->m_ShapesSetId);
		m_pObj->mShapeStack.refresh(m_pObj);
	}
	delete pBuff;
	_buffTab.erase(index);
	//m_pObj->setMaskBits(GameObject::BuffMask);
	return true;
}

bool BuffTable::RemoveBuffById(U32 _buffId, U32 _count /* = 1 */, U32 mask /* = Buff::Mask_Remove_Sys */, bool seriesId /* = false */)
{
	BuffData* pBuffData = g_BuffRepository.GetBuff(seriesId ? Macro_GetBuffId(_buffId, 1) : _buffId);
	if(!pBuffData || !m_pObj)
		return false;

	BuffTableType* pBuffTable;
	if(pBuffData->m_Flags & BuffData::Flags_System)
		pBuffTable = &m_System;
	else if(pBuffData->m_Flags & BuffData::Flags_Buff)
		pBuffTable = &m_Buff;
	else
		pBuffTable = &m_Debuff;

	for (S32 i=pBuffTable->size()-1; i>=0; --i)
	{
		if(seriesId ? _buffId == (*pBuffTable)[i]->m_BuffData->GetSeriesId() : _buffId == (*pBuffTable)[i]->m_BuffData->GetBuffId())
			//RemoveBuff(*pBuffTable, (*pBuffTable)[i]->m_BuffData->GetBuffId(), i, _count, mask);
			(*pBuffTable)[i]->Remove(m_pObj, mask);
	}
	return true;
}

bool BuffTable::RemoveBuffBySrc(U32 _buffId, GameObject* src, U32 _count /* = 1 */, U32 mask /* = Buff::Mask_Remove_Sys */, bool seriesId /* = false */)
{
	BuffData* pBuffData = g_BuffRepository.GetBuff(seriesId ? Macro_GetBuffId(_buffId, 1) : _buffId);
	if(!pBuffData || !m_pObj)
		return false;

	BuffTableType* pBuffTable;
	if(pBuffData->m_Flags & BuffData::Flags_System)
		pBuffTable = &m_System;
	else if(pBuffData->m_Flags & BuffData::Flags_Buff)
		pBuffTable = &m_Buff;
	else
		pBuffTable = &m_Debuff;

	for (S32 i=pBuffTable->size()-1; i>=0; --i)
	{
		if((seriesId ? _buffId == (*pBuffTable)[i]->m_BuffData->GetSeriesId() : _buffId == (*pBuffTable)[i]->m_BuffData->GetBuffId()) && (*pBuffTable)[i]->m_pSource == src)
		{
			//RemoveBuff(*pBuffTable, (*pBuffTable)[i]->m_BuffData->GetBuffId(), i, _count, mask);
			(*pBuffTable)[i]->Remove(m_pObj, mask);
			return true;
		}
	}
	return false;
}

bool BuffTable::HandleRemoveRequest(U32 _buffId, U32 index)
{
#ifdef NTJ_SERVER
	if(index >= m_Buff.size())
		return false;
	// 网络延时有可能产生偏差
	if(!(m_Buff[index]) || !(m_Buff[index]->m_BuffData) || (_buffId != m_Buff[index]->m_BuffData->GetBuffId()))
		return false;

	RemoveBuff(m_Buff, _buffId, index, m_Buff[index]->m_Count, Buff::Mask_Remove_Common);
	return true;
#endif

#ifdef NTJ_CLIENT
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(conn)
	{
		ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_CANCELBUFF);
		pEvent->SetIntArgValues(1,index);
		pEvent->SetInt32ArgValues(1,_buffId);
		conn->postNetEvent(pEvent);
		return true;
	}
#endif
	return false;
}

void BuffTable::ClearDebuff()
{
	for(S32 i = m_Debuff.size()-1; i >= 0; --i)
		m_Debuff[i]->Remove(m_pObj, Buff::Mask_Remove_Sys);
}

void BuffTable::Process(SimTime dt)
{
	if(!m_pObj)
		return;
	bool changed = m_RefreshSet;
	if(m_RefreshSet)
	{
		m_ImmunitySet.reset();
		m_RestrainSet.reset();
	}
	// 先处理触发事件
	processTrigger();

	// effectMask需要实时刷新
	m_EffectMask = 0;
	// 注意触发的处理时机，应避免对状态表同时进行增减操作
	Buff* pBuff;
	for(S32 i=m_Buff.size()-1; i>=0; --i)
	{
		pBuff = m_Buff[i];
		if(!pBuff)
		{
			m_Buff.erase(i);
			continue;
		}

		if(!pBuff->Process(m_pObj, dt))
			RemoveBuff(m_Buff, pBuff->m_BuffData->GetBuffId(), i, pBuff->m_Count, pBuff->m_RemoveMask);
		else
		{
			m_EffectMask |= pBuff->m_BuffData->m_BuffEffect;
			if(m_RefreshSet)
			{
				m_ImmunitySet |= pBuff->m_BuffData->m_ImmunityGroup;
				m_RestrainSet |= pBuff->m_BuffData->m_RestrainGroup;
			}
		}
	}

	for(S32 i=m_Debuff.size()-1; i>=0; --i)
	{
		pBuff = m_Debuff[i];
		if(!pBuff)
		{
			m_Buff.erase(i);
			continue;
		}

		if(!pBuff->Process(m_pObj, dt))
			RemoveBuff(m_Debuff, pBuff->m_BuffData->GetBuffId(), i, pBuff->m_Count, pBuff->m_RemoveMask);
		else
		{
			m_EffectMask |= pBuff->m_BuffData->m_BuffEffect;
			if(m_RefreshSet)
			{
				m_ImmunitySet |= pBuff->m_BuffData->m_ImmunityGroup;
				m_RestrainSet |= pBuff->m_BuffData->m_RestrainGroup;
			}
		}
	}

	for(S32 i=m_System.size()-1; i>=0; --i)
	{
		pBuff = m_System[i];
		if(!pBuff)
		{
			m_Buff.erase(i);
			continue;
		}

		if(!pBuff->Process(m_pObj, dt))
			RemoveBuff(m_System, pBuff->m_BuffData->GetBuffId(), i, pBuff->m_Count, pBuff->m_RemoveMask);
		else
		{
			m_EffectMask |= pBuff->m_BuffData->m_BuffEffect;
			if(m_RefreshSet)
			{
				m_ImmunitySet |= pBuff->m_BuffData->m_ImmunityGroup;
				m_RestrainSet |= pBuff->m_BuffData->m_RestrainGroup;
			}
		}
	}
	// 这里会有一个tick的误差
	if(changed)
	{
		m_RefreshSet = false;
		m_pObj->setMaskBits(GameObject::BuffMask);
	}
}

bool BuffTable::CalculateStats(const enFamily family)
{
	if(m_Refresh)
	{
		static S32 hp;
		static S32 mp;
		static S32 pp;
		hp = m_Stats.HP;
		mp = m_Stats.MP;
		pp = m_Stats.PP;
		m_Stats.Calc(m_Summation, g_CalcFactor[family]);
		m_Refresh = false;
		setHP(hp);
		setMP(mp);
		setPP(pp);
		return true;
	}
	return false;
}

bool BuffTable::CalculateStats_Pet(const Stats& stats, const CalcFactor& factor)
{
	if(m_Refresh)
	{
		static S32 hp;
		static S32 mp;
		static S32 pp;
		hp = m_Stats.HP;
		mp = m_Stats.MP;
		pp = m_Stats.PP;
		g_Stats.Clear();
		g_Stats += m_Summation;
		g_Stats += stats;
		m_Stats.Calc(g_Stats, factor);
		m_Refresh = false;
		setHP(hp);
		setMP(mp);
		setPP(pp);
		return true;
	}
	return false;
}

bool BuffTable::checkBuff(U32 id, S32 count, bool seriesId /* = false */) const
{
	count = mAbs(count);
	if(count <= 0 || id == 0)
		return true;

	Buff* pBuff;
	for(S32 i=m_Buff.size()-1; i>=0; --i)
	{
		pBuff = m_Buff[i];
		if(pBuff && (seriesId ? pBuff->m_BuffData->GetSeriesId() : pBuff->m_BuffData->GetBuffId()) == id)
			if(pBuff->m_Count >= count)
				return true;
			else
				return false;
	}

	for(S32 i=m_Debuff.size()-1; i>=0; --i)
	{
		pBuff = m_Debuff[i];
		if(pBuff && (seriesId ? pBuff->m_BuffData->GetSeriesId() : pBuff->m_BuffData->GetBuffId()) == id)
			if(pBuff->m_Count >= count)
				return true;
			else
				return false;
	}

	for(S32 i=m_System.size()-1; i>=0; --i)
	{
		pBuff = m_System[i];
		if(pBuff && (seriesId ? pBuff->m_BuffData->GetSeriesId() : pBuff->m_BuffData->GetBuffId()) == id)
			if(pBuff->m_Count >= count)
				return true;
			else
				return false;
	}
	return false;
}

U32 BuffTable::getBuffCount(U32 seriesId, U32 level) const
{
	if(!seriesId)
		return 0;

	U32 id = level ? Macro_GetBuffId(seriesId, level) : seriesId;
	Buff* pBuff;
	for(S32 i=m_Buff.size()-1; i>=0; --i)
	{
		pBuff = m_Buff[i];
		if(pBuff && (level ? pBuff->m_BuffData->GetBuffId() : pBuff->m_BuffData->GetSeriesId()) == id)
			return pBuff->m_Count;
	}

	for(S32 i=m_Debuff.size()-1; i>=0; --i)
	{
		pBuff = m_Debuff[i];
		if(pBuff && (seriesId ? pBuff->m_BuffData->GetBuffId() : pBuff->m_BuffData->GetSeriesId()) == id)
			return pBuff->m_Count;
	}

	for(S32 i=m_System.size()-1; i>=0; --i)
	{
		pBuff = m_System[i];
		if(pBuff && (seriesId ? pBuff->m_BuffData->GetBuffId() : pBuff->m_BuffData->GetSeriesId()) == id)
			return pBuff->m_Count;
	}
	return 0;
}

GameObject* BuffTable::getMocker() const
{
	if(!(m_EffectMask & BuffData::Effect_Mock))
		return NULL;
	for (S32 i=m_Buff.size(); i>=0; --i)
	{
		if(m_Buff[i]->m_BuffData->GetEffect() & BuffData::Effect_Mock)
			return m_Buff[i]->m_pSource;
	}
	return NULL;
}

void BuffTable::packStats(BitStream *stream)
{
	stream->writeSignedInt(m_Stats.HP, StatsBits);
	stream->writeSignedInt(m_Stats.MP, StatsBits);
	stream->writeSignedInt(m_Stats.PP, StatsBits);
	stream->writeSignedInt(m_Stats.MaxHP, StatsBits);
	stream->writeSignedInt(m_Stats.MaxMP, StatsBits);
	stream->writeSignedInt(m_Stats.MaxPP, StatsBits);

	stream->writeSignedInt(m_Stats.Stamina, StatsBits);
	stream->writeSignedInt(m_Stats.Mana, StatsBits);
	stream->writeSignedInt(m_Stats.Strength, StatsBits);
	stream->writeSignedInt(m_Stats.Intellect, StatsBits);
	stream->writeSignedInt(m_Stats.Agility, StatsBits);
	stream->writeSignedInt(m_Stats.Pneuma, StatsBits);
	stream->writeSignedInt(m_Stats.Insight, StatsBits);
	stream->writeSignedInt(m_Stats.Luck, StatsBits);

	stream->writeSignedInt(m_Stats.AttackSpeed, Base::Bit16);

	stream->writeSignedInt(m_Stats.CriticalRating, Base::Bit16);
	stream->writeSignedInt(m_Stats.CriticalTimes, Base::Bit16);
	stream->writeSignedInt(m_Stats.AntiCriticalRating, Base::Bit16);
	stream->writeSignedInt(m_Stats.DodgeRating, Base::Bit16);

	stream->writeSignedInt(m_Stats.PhyDamage, StatsBits);
	stream->writeSignedInt(m_Stats.PhyDefence, StatsBits);
	stream->writeSignedInt(m_Stats.MuDamage, StatsBits);
	stream->writeSignedInt(m_Stats.MuDefence, StatsBits);
	stream->writeSignedInt(m_Stats.HuoDamage, StatsBits);
	stream->writeSignedInt(m_Stats.HuoDefence, StatsBits);
	stream->writeSignedInt(m_Stats.TuDamage, StatsBits);
	stream->writeSignedInt(m_Stats.TuDefence, StatsBits);
	stream->writeSignedInt(m_Stats.JinDamage, StatsBits);
	stream->writeSignedInt(m_Stats.JinDefence, StatsBits);
	stream->writeSignedInt(m_Stats.ShuiDamage, StatsBits);
	stream->writeSignedInt(m_Stats.ShuiDefence, StatsBits);
	stream->writeSignedInt(m_Stats.PneumaDamage, StatsBits);
	stream->writeSignedInt(m_Stats.PneumaDefence, StatsBits);
	stream->writeSignedInt(m_Stats.Heal, StatsBits);

	stream->writeSignedInt(m_Stats.Mu, StatsWuXingBits);
	stream->writeSignedInt(m_Stats.Huo, StatsWuXingBits);
	stream->writeSignedInt(m_Stats.Tu, StatsWuXingBits);
	stream->writeSignedInt(m_Stats.Jin, StatsWuXingBits);
	stream->writeSignedInt(m_Stats.Shui, StatsWuXingBits);
}

void BuffTable::packMaxStats(BitStream* stream)
{
	stream->writeSignedInt(m_Stats.MaxHP, StatsBits);
	stream->writeSignedInt(m_Stats.MaxMP, StatsBits);
	stream->writeSignedInt(m_Stats.MaxPP, StatsBits);
}

void BuffTable::unpackMaxStats(BitStream* stream)
{
	m_Stats.MaxHP = stream->readSignedInt(StatsBits);
	m_Stats.MaxMP = stream->readSignedInt(StatsBits);
	m_Stats.MaxPP = stream->readSignedInt(StatsBits);
}

void BuffTable::unpackStats(BitStream *stream)
{
	m_Stats.HP = stream->readSignedInt(StatsBits);
	m_Stats.MP = stream->readSignedInt(StatsBits);
	m_Stats.PP = stream->readSignedInt(StatsBits);
	m_Stats.MaxHP = stream->readSignedInt(StatsBits);
	m_Stats.MaxMP = stream->readSignedInt(StatsBits);
	m_Stats.MaxPP = stream->readSignedInt(StatsBits);

	m_Stats.Stamina = stream->readSignedInt(StatsBits);
	m_Stats.Mana = stream->readSignedInt(StatsBits);
	m_Stats.Strength = stream->readSignedInt(StatsBits);
	m_Stats.Intellect = stream->readSignedInt(StatsBits);
	m_Stats.Agility = stream->readSignedInt(StatsBits);
	m_Stats.Pneuma = stream->readSignedInt(StatsBits);
	m_Stats.Insight = stream->readSignedInt(StatsBits);
	m_Stats.Luck = stream->readSignedInt(StatsBits);

	m_Stats.AttackSpeed = stream->readSignedInt(Base::Bit16);

	m_Stats.CriticalRating = stream->readSignedInt(Base::Bit16);
	m_Stats.CriticalTimes = stream->readSignedInt(Base::Bit16);
	m_Stats.AntiCriticalRating = stream->readSignedInt(Base::Bit16);
	m_Stats.DodgeRating = stream->readSignedInt(Base::Bit16);

	m_Stats.PhyDamage = stream->readSignedInt(StatsBits);
	m_Stats.PhyDefence = stream->readSignedInt(StatsBits);
	m_Stats.MuDamage = stream->readSignedInt(StatsBits);
	m_Stats.MuDefence = stream->readSignedInt(StatsBits);
	m_Stats.HuoDamage = stream->readSignedInt(StatsBits);
	m_Stats.HuoDefence = stream->readSignedInt(StatsBits);
	m_Stats.TuDamage = stream->readSignedInt(StatsBits);
	m_Stats.TuDefence = stream->readSignedInt(StatsBits);
	m_Stats.JinDamage = stream->readSignedInt(StatsBits);
	m_Stats.JinDefence = stream->readSignedInt(StatsBits);
	m_Stats.ShuiDamage = stream->readSignedInt(StatsBits);
	m_Stats.ShuiDefence = stream->readSignedInt(StatsBits);
	m_Stats.PneumaDamage = stream->readSignedInt(StatsBits);
	m_Stats.PneumaDefence = stream->readSignedInt(StatsBits);
	m_Stats.Heal = stream->readSignedInt(StatsBits);

	m_Stats.Mu = stream->readSignedInt(StatsWuXingBits);
	m_Stats.Huo = stream->readSignedInt(StatsWuXingBits);
	m_Stats.Tu = stream->readSignedInt(StatsWuXingBits);
	m_Stats.Jin = stream->readSignedInt(StatsWuXingBits);
	m_Stats.Shui = stream->readSignedInt(StatsWuXingBits);


}

void BuffTable::packSpeed(BitStream* stream)
{
	stream->writeSignedInt(m_Stats.AttackSpeed_gPc, Base::Bit16);
	stream->writeSignedInt(m_Stats.Velocity_gPc, Base::Bit16);
	stream->writeSignedInt(m_Stats.MountedVelocity_gPc, Base::Bit16);
}

void BuffTable::unpackSpeed(BitStream* stream)
{
	m_Stats.AttackSpeed_gPc = stream->readSignedInt(Base::Bit16);
	m_Stats.Velocity_gPc = stream->readSignedInt(Base::Bit16);
	m_Stats.MountedVelocity_gPc = stream->readSignedInt(Base::Bit16);
}

void BuffTable::packBuff(BitStream *stream)
{
#ifdef NTJ_SERVER
	stream->writeInt(m_EffectMask, Base::Bit32);
	stream->writeInt(m_Buff.size(), BuffNumBits);
	for (S32 i=0; i<m_Buff.size(); i++)
	{
		stream->writeInt(m_Buff[i]->m_BuffData->GetBuffId(), BuffIdBits);
		stream->writeInt(m_Buff[i]->m_RemainTime, BuffTimeBits);
		stream->writeInt(m_Buff[i]->m_Count, BuffCountBits);
	}
	stream->writeInt(m_Debuff.size(), BuffNumBits);
	for (S32 i=0; i<m_Debuff.size(); i++)
	{
		stream->writeInt(m_Debuff[i]->m_BuffData->GetBuffId(), BuffIdBits);
		stream->writeInt(m_Debuff[i]->m_RemainTime, BuffTimeBits);
		stream->writeInt(m_Debuff[i]->m_Count, BuffCountBits);
	}
#endif
}

void BuffTable::unpackBuff(BitStream *stream)
{
#ifdef NTJ_CLIENT
	m_EffectMask = stream->readInt(Base::Bit32);
	S32 newSize = stream->readInt(BuffNumBits);
	for (S32 i=0; i<newSize; i++)
	{
		if (i>=m_Buff.size())
		{
			Buff *pBuff = new Buff;
			m_Buff.push_back(pBuff);
		}
		m_Buff[i]->m_BuffData = g_BuffRepository.GetBuff(stream->readInt(BuffIdBits));
		m_Buff[i]->m_RemainTime = stream->readInt(BuffTimeBits) + Platform::getVirtualMilliseconds();
		m_Buff[i]->m_Count = stream->readInt(BuffCountBits);
	}
	for (S32 i=m_Buff.size()-1; i>=newSize && i>=0; --i)
		delete m_Buff[i];
	m_Buff.setSize(newSize);

	newSize = stream->readInt(BuffNumBits);
	for (S32 i=0; i<newSize; i++)
	{
		if (i>=m_Debuff.size())
		{
			Buff *pBuff = new Buff;
			m_Debuff.push_back(pBuff);
		}
		m_Debuff[i]->m_BuffData = g_BuffRepository.GetBuff(stream->readInt(BuffIdBits));
		m_Debuff[i]->m_RemainTime = stream->readInt(BuffTimeBits) + Platform::getVirtualMilliseconds();
		m_Debuff[i]->m_Count = stream->readInt(BuffCountBits);
	}
	for (S32 i=m_Debuff.size()-1; i>=newSize && i>=0; --i)
		delete m_Debuff[i];
	m_Debuff.setSize(newSize);

	// 做循环检查特效，可以优化
	EPMap::iterator it = m_EP.begin();
	for (; it != m_EP.end();)
	{
		bool finded = false;
		for (S32 i=0; i<m_Buff.size() && !finded; ++i)
		{
			if(it->first == m_Buff[i]->m_BuffData->m_BuffEP)
				finded = true;
		}
		for (S32 i=0; i<m_Debuff.size() && !finded; ++i)
		{
			if(it->first == m_Debuff[i]->m_BuffData->m_BuffEP)
				finded = true;
		}
		if(!finded)
		{
			g_EffectPacketContainer.removeEffectPacket(it->second);
			m_EP.erase(it++);
		}
		else
			++it;
	}

	for (S32 i=0; i<m_Buff.size(); ++i)
		if(m_Buff[i]->m_BuffData->m_BuffEP && m_EP.find(m_Buff[i]->m_BuffData->m_BuffEP) == m_EP.end())
		{
			U32 id = g_EffectPacketContainer.addEffectPacket(m_Buff[i]->m_BuffData->m_BuffEP, m_pObj, m_pObj->getTransform(), m_pObj, m_pObj->getPosition());
			m_EP.insert(EPMap::value_type(m_Buff[i]->m_BuffData->m_BuffEP, id));
		}
	for (S32 i=0; i<m_Debuff.size(); ++i)
		if(m_Debuff[i]->m_BuffData->m_BuffEP && m_EP.find(m_Debuff[i]->m_BuffData->m_BuffEP) == m_EP.end())
		{
			U32 id = g_EffectPacketContainer.addEffectPacket(m_Debuff[i]->m_BuffData->m_BuffEP, m_pObj, m_pObj->getTransform(), m_pObj, m_pObj->getPosition());
			m_EP.insert(EPMap::value_type(m_Debuff[i]->m_BuffData->m_BuffEP, id));
		}
#endif
}

void BuffTable::clearTrigger()
{
	if(m_TriggerTable.empty())
		return;
	TriggerTableType::iterator itrUnit;
	for(itrUnit = m_TriggerTable.begin(); itrUnit != m_TriggerTable.end(); ++itrUnit)
		delete (*itrUnit);
	m_TriggerTable.clear();
}

void BuffTable::processTrigger()
{
	if(m_TriggerTable.empty() || m_TriggerLocked)
		return;

	m_TriggerLocked = true;
	TriggerTableType::iterator itrUnit;
	bool isObjDisabled = false;
	for(itrUnit = m_TriggerTable.begin(); itrUnit != m_TriggerTable.end(); ++itrUnit)
	{
		isObjDisabled = isObjDisabled || ((*itrUnit)->mask & BuffData::Trigger_Death);
		for(S32 i=m_Buff.size()-1; i>=0; --i)
			m_Buff[i]->onTrigger((*itrUnit)->mask, m_pObj, (*itrUnit)->trigger);

		for(S32 i=m_Debuff.size()-1; i>=0; --i)
			m_Debuff[i]->onTrigger((*itrUnit)->mask, m_pObj, (*itrUnit)->trigger);

		for(S32 i=m_System.size()-1; i>=0; --i)
			m_System[i]->onTrigger((*itrUnit)->mask, m_pObj, (*itrUnit)->trigger);
		delete (*itrUnit);
	}
	if(isObjDisabled)
		onObjectDisabled();
	m_TriggerTable.clear();
	m_TriggerLocked = false;
}

void BuffTable::onTrigger(U32 mask, GameObject* trigger)
{
	if(!m_pObj || !m_TriggerMask.test(mask))
		return;

	if(m_TriggerLocked || m_TriggerTable.size() >= BuffTriggerMax)
		return;

	TriggerUnit* pUnit = new TriggerUnit;
	if(pUnit)
	{
		pUnit->mask = mask;
		pUnit->trigger = trigger;
		m_TriggerTable.push_back(pUnit);
	}
}

bool BuffTable::onTriggerGroup(BuffData* pBuffData, GameObject* src)
{
	if(!pBuffData->m_RemoveGroupA.any() && !pBuffData->m_RemoveGroupB.any() && !pBuffData->m_RemoveGroupC.any())
		return false;

	for(S32 i=m_Buff.size()-1; i>=0; --i)
	{
		if(!m_Buff[i] || m_Buff[i]->m_RemoveMask)
			continue;
		// 只移除相同来源的状态
		if(pBuffData->IsFlags(BuffData::Flags_RemoveSameSrc) && m_Buff[i]->m_pSource != src)
			continue;

		std::bitset<BUFF_GROUP_BITS>& sets = m_Buff[i]->m_BuffData->m_Group;
		if((pBuffData->m_RemoveGroupA & sets).any())
		{
			m_Buff[i]->Remove(m_pObj, BuffData::Trigger_RemoveA);
			if(pBuffData->IsFlags(BuffData::Flags_RemoveSelf))
				return true;
		}
		if((pBuffData->m_RemoveGroupB & sets).any())
		{
			m_Buff[i]->Remove(m_pObj, BuffData::Trigger_RemoveB);
			if(pBuffData->IsFlags(BuffData::Flags_RemoveSelf))
				return true;
		}
		if((pBuffData->m_RemoveGroupC & sets).any())
		{
			m_Buff[i]->Remove(m_pObj, BuffData::Trigger_RemoveC);
			if(pBuffData->IsFlags(BuffData::Flags_RemoveSelf))
				return true;
		}
	}
	for(S32 i=m_Debuff.size()-1; i>=0; --i)
	{
		if(!m_Debuff[i] || m_Debuff[i]->m_RemoveMask)
			continue;
		// 只移除相同来源的状态
		if(pBuffData->IsFlags(BuffData::Flags_RemoveSameSrc) && m_Debuff[i]->m_pSource != src)
			continue;

		std::bitset<BUFF_GROUP_BITS>& sets = m_Debuff[i]->m_BuffData->m_Group;
		if((pBuffData->m_RemoveGroupA & sets).any())
		{
			m_Debuff[i]->Remove(m_pObj, BuffData::Trigger_RemoveA);
			if(pBuffData->IsFlags(BuffData::Flags_RemoveSelf))
				return true;
		}
		if((pBuffData->m_RemoveGroupB & sets).any())
		{
			m_Debuff[i]->Remove(m_pObj, BuffData::Trigger_RemoveB);
			if(pBuffData->IsFlags(BuffData::Flags_RemoveSelf))
				return true;
		}
		if((pBuffData->m_RemoveGroupC & sets).any())
		{
			m_Debuff[i]->Remove(m_pObj, BuffData::Trigger_RemoveC);
			if(pBuffData->IsFlags(BuffData::Flags_RemoveSelf))
				return true;
		}
	}
	return false;
}

bool BuffTable::onTriggerShield(S32& damage)
{
	S32 count;
	for(S32 i=m_Buff.size()-1; i>=0 && mAbs(damage); --i)
	{
		if(!m_Buff[i] || m_Buff[i]->m_RemoveMask || m_Buff[i]->m_Count<=0 || !m_Buff[i]->m_BuffData)
			continue;
		if(m_Buff[i]->m_BuffData->m_ShieldInfo.triggerRate <= 0)
			continue;
		if(m_Buff[i]->m_BuffData->m_ShieldInfo.triggerRate < 10000 && gRandGen.randI(1, 10000) > m_Buff[i]->m_BuffData->m_ShieldInfo.triggerRate)
			continue;
		// 抵挡伤害 或 抵挡治疗
		if(!m_Buff[i]->m_BuffData->IsFlags(BuffData::Flags_ShieldDamage) && damage > 0)
			continue;
		if(m_Buff[i]->m_BuffData->IsFlags(BuffData::Flags_ShieldDamage) && damage < 0)
			continue;

		count = getMin(mAbs(damage), (S32)m_Buff[i]->m_Count);
		if(damage > 0)
			damage -= count * mClampF(m_Buff[i]->m_BuffData->m_ShieldInfo.absorbRate/GAMEPLAY_PERCENT_SCALE, 0.0f, 2.0f);
		else
			damage += count * mClampF(m_Buff[i]->m_BuffData->m_ShieldInfo.absorbRate/GAMEPLAY_PERCENT_SCALE, 0.0f, 2.0f);
		m_Buff[i]->m_Count -= count;

		if(0 == m_Buff[i]->m_Count)
			m_Buff[i]->Remove(m_pObj, BuffData::Trigger_RemoveCounter);
	}
	for(S32 i=m_Debuff.size()-1; i>=0 && mAbs(damage); --i)
	{
		if(!m_Debuff[i] || m_Debuff[i]->m_RemoveMask || m_Debuff[i]->m_Count<=0 || !m_Debuff[i]->m_BuffData)
			continue;
		if(m_Debuff[i]->m_BuffData->m_ShieldInfo.triggerRate <= 0)
			continue;
		if(m_Debuff[i]->m_BuffData->m_ShieldInfo.triggerRate < 10000 && gRandGen.randI(1, 10000) > m_Debuff[i]->m_BuffData->m_ShieldInfo.triggerRate)
			continue;
		// 抵挡伤害 或 抵挡治疗
		if(!m_Debuff[i]->m_BuffData->IsFlags(BuffData::Flags_ShieldDamage) && damage > 0)
			continue;
		if(m_Debuff[i]->m_BuffData->IsFlags(BuffData::Flags_ShieldDamage) && damage < 0)
			continue;

		count = getMin(mAbs(damage), (S32)m_Debuff[i]->m_Count);
		if(damage > 0)
			damage -= count * mClampF(m_Debuff[i]->m_BuffData->m_ShieldInfo.absorbRate/GAMEPLAY_PERCENT_SCALE, 0.0f, 2.0f);
		else
			damage += count * mClampF(m_Debuff[i]->m_BuffData->m_ShieldInfo.absorbRate/GAMEPLAY_PERCENT_SCALE, 0.0f, 2.0f);
		m_Debuff[i]->m_Count -= count;

		if(0 == m_Debuff[i]->m_Count)
			m_Debuff[i]->Remove(m_pObj, BuffData::Trigger_RemoveCounter);
	}
	return (bool)mAbs(damage);
}

void BuffTable::onObjectDisabled()
{
	// 做这一步主要是为了防止1个tick内出现死亡又复活的情况
	for(S32 i=m_Buff.size()-1; i>=0; --i)
		if(!m_Buff[i]->m_BuffData->IsFlags(BuffData::Flags_ExistOnDisabled))
			m_Buff[i]->Remove(m_pObj, Buff::Mask_Remove_Sys);

	for(S32 i=m_Debuff.size()-1; i>=0; --i)
		if(!m_Debuff[i]->m_BuffData->IsFlags(BuffData::Flags_ExistOnDisabled))
			m_Debuff[i]->Remove(m_pObj, Buff::Mask_Remove_Sys);

	for(S32 i=m_System.size()-1; i>=0; --i)
		if(!m_System[i]->m_BuffData->IsFlags(BuffData::Flags_ExistOnDisabled))
			m_System[i]->Remove(m_pObj, Buff::Mask_Remove_Sys);
}

