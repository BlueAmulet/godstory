//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "T3D/gameConnection.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/ScheduleManager.h"

#ifdef NTJ_CLIENT
#include "Gameplay/GameEvents/ClientGameNetEvents.h"

static char* szStatusMsg[VocalStatus::VOCALSTATUS_MAX] =
{
	"",
	"制造中",
	"采集中",
	"召唤宠物",
	"使用宠物蛋",
	"使用骑宠蛋",
	"召唤骑宠",
	"施法中",
	"物品分解中",
	"物品合成中",
	"吟唱异常"
};
void VocalStatus::clear()
{
	mVocalStatus  = false;
	mVocalType    = VocalStatus::VOCALSTATUS_NONE;
	mAnimation    = GameObjectData::Root_a;
}

bool VocalStatus::setStatus(VocalStatusType vocalType,bool isVocal, U32 animation /* = 0 */)
{
	if(mVocalType == VocalStatus::VOCALSTATUS_NONE && !mVocalStatus)
	{
		mVocalStatus = isVocal;
		mVocalType = vocalType;
		mAnimation = animation;
		return true;
	}

	return false;
}
void VocalStatus::cancelVocalEvent()
{
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(!conn)
		return;

	if(!getStatus())
		return;

	//发送取消吟唱调度 
	ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_CANCELVOCAL);
	conn->postNetEvent(pEvent);

	//执行脚本终止吟唱
	char szType[8] = {0,};
	dSprintf(szType,sizeof(szType),"%d",getStatusType());
	Con::executef("recoveryButtonState",szType);
	Con::executef("cancleFetchTimeProgress");
}
StringTableEntry VocalStatus::getStateName()
{
	return szStatusMsg[getStatusType()];
}
#endif
///////////////////////////////////////////////////////
#ifdef NTJ_SERVER
void ScheduleEvent::clear()
{
	mEventId = 0;
	mTime    = 0;
	mAnimation = 0;
}

void ScheduleEvent::cancelEvent(Player* pPlayer)
{
	if(pPlayer && getEventID())
	{
		Sim::cancelEvent(getEventID());
		clear();

		ServerGameNetEvent* pEvent = new ServerGameNetEvent(INFO_CANCELVOCAL);
		GameConnection* conn = pPlayer->getControllingClient();
		if(conn)
			conn->postNetEvent(pEvent);
	}
}
#endif
