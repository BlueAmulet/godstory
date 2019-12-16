//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/Cooldown.h"
#include "platform/platform.h"
#include "T3d/gameConnection.h"
#include "Gameplay/GameEvents/ServerGameNetEvents.h"

// ========================================================================================================================================
//  Cooldown
// ========================================================================================================================================

// ========================================================================================================================================
//  CooldownTable
// ========================================================================================================================================

CooldownTable::~CooldownTable()
{
	Clear();
}

void CooldownTable::Clear()
{
}

bool CooldownTable::SaveData(stPlayerStruct* playerInfo)
{
#ifdef NTJ_SERVER
	if(!playerInfo)
	{
		AssertFatal(false, "SkillTable::SaveData error!!");
		return false;
	}

	for (S32 i=0; i<COOLDOWN_MAX; ++i)
	{
		playerInfo->MainData.CooldownInfo[i].Cooldown = m_Table[i].GetCooldown()/1000;
	}
#endif
	return true;
}

SimTime CooldownTable::GetCooldown(S32 cdGroup, bool ignoreGlobal /* = false */)
{
	if(cdGroup < 0 || cdGroup >= COOLDOWN_MAX)
		return (!ignoreGlobal && m_Global.EnabledTime > Platform::getVirtualMilliseconds()) ? m_Global.EnabledTime - Platform::getVirtualMilliseconds() : 0;;

	static SimTime cdTime;
	cdTime = ignoreGlobal ? m_Table[cdGroup].EnabledTime : getMax(m_Table[cdGroup].EnabledTime, m_Global.EnabledTime);
	return (cdTime > Platform::getVirtualMilliseconds()) ? cdTime - Platform::getVirtualMilliseconds() : 0;
}

void CooldownTable::SetCooldown_Group(S32 cdGroup, U32 cdTime, U32 TotalTime)
{
	if(cdGroup < 0 || cdGroup >= COOLDOWN_MAX)
		return;

	if(m_Table[cdGroup].EnabledTime < Platform::getVirtualMilliseconds() + cdTime)
	{
		m_Table[cdGroup].SetCooldown(Platform::getVirtualMilliseconds() + cdTime, getMax(TotalTime, cdTime));
	}
}

void CooldownTable::SetCooldown_Global()
{
	if(m_Global.EnabledTime < Platform::getVirtualMilliseconds() + GlobalCooldownTime)
	{
		m_Global.SetCooldown(Platform::getVirtualMilliseconds() + GlobalCooldownTime, GlobalCooldownTime);
	}
}

void CooldownTable::sendInit(GameConnection* con)
{
	if(con)
	{
		for (S32 i=0; i<COOLDOWN_MAX; ++i)
		{
			if(m_Table[i].EnabledTime > Platform::getVirtualMilliseconds())
			{
				ServerGameNetEvent* pEvent = new ServerGameNetEvent(INFO_COOLDOWN);
				pEvent->SetIntArgValues(1, i);
				pEvent->SetInt32ArgValues(2, m_Table[i].EnabledTime - Platform::getVirtualMilliseconds(), m_Table[i].TotalTime);
				con->postNetEvent(pEvent);
			}
		}
	}
}
