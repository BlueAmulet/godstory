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
		timeBits = 28,			// ���룬�������
	};

	U32 EnabledTime;			// �´ο���ʱ��
	U32 TotalTime;				// �ܵ���ȴʱ��

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
		GlobalCooldownTime		= 1000,					// ����������CD
#else
		GlobalCooldownTime		= 1500,					// �ͻ��˹���CD����һ����ʱ������
#endif
	};

	Cooldown					m_Table[COOLDOWN_MAX];	// ��ȴʱ�����ݣ���250��
	Cooldown					m_Global;				// ����CD

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

