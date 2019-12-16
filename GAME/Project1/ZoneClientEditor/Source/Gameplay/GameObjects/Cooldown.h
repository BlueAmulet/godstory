//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "platform/types.h"
#include "platform/platform.h"
#include "core/bitStream.h"
#include "Common/PlayerStruct.h"
#include "console/sim.h"

// ========================================================================================================================================
//  Cooldown
// ========================================================================================================================================

struct Cooldown
{
	enum transformBits
	{
		timeBits = 28,			// 毫秒，最大三天
	};

	U32 EnabledTime;			// 下次可用时间
	U32 TotalTime;				// 总的冷却时间

	Cooldown() : EnabledTime(0),TotalTime(0)
	{
	}
	void inline SetCooldown(U32 enabledTime, U32 totalTime)
	{
		EnabledTime = enabledTime;
		TotalTime = totalTime;
	}
	U32 inline GetCooldown()
	{
		if(EnabledTime > Platform::getVirtualMilliseconds())
			return EnabledTime - Platform::getVirtualMilliseconds();
		else
			return 0;
	}
	void inline Pack(BitStream* stream)
	{
		if(stream->writeFlag(EnabledTime > Platform::getVirtualMilliseconds()))
		{
			stream->writeInt(EnabledTime - Platform::getVirtualMilliseconds(), timeBits);
			stream->writeInt(TotalTime, timeBits);
		}
	}
	void inline Unpack(BitStream* stream)
	{
		if(stream->readFlag())
		{
			EnabledTime = stream->readInt(timeBits) + Platform::getVirtualMilliseconds();
			TotalTime = stream->readInt(timeBits);
		}
	}
};


// ========================================================================================================================================
//  CooldownTable
// ========================================================================================================================================
class GameConnection;
class CooldownTable
{
protected:
	enum
	{
#ifdef NTJ_SERVER
		GlobalCooldownTime		= 1000,					// 服务器公共CD
#else
		GlobalCooldownTime		= 1500,					// 客户端公共CD，留一定的时间冗余
#endif
	};

	Cooldown					m_Table[COOLDOWN_MAX];	// 冷却时间数据，共250个
	Cooldown					m_Global;				// 公共CD

public:
	~CooldownTable();

	void						Clear					();
	bool						SaveData				(stPlayerStruct* playerInfo);
	SimTime						GetCooldown				(S32 cdGroup, bool ignoreGlobal = false);
	inline const Cooldown*		GetCooldown_Group		(S32 cdGroup);
	inline const Cooldown*		GetCooldown_Global		();
	void						SetCooldown_Group		(S32 cdGroup, U32 cdTime, U32 TotalTime);
	void						SetCooldown_Global		();

	void						sendInit				(GameConnection* con);
};


const Cooldown* CooldownTable::GetCooldown_Group(S32 cdGroup)
{
	if(cdGroup < 0 || cdGroup >= COOLDOWN_MAX)
		return NULL;
	return &(m_Table[cdGroup]);
}

const Cooldown* CooldownTable::GetCooldown_Global()
{
	return &m_Global;
}

