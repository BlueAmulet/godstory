//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _SCHEDULEMANAGER_H_
#define _SCHEDULEMANAGER_H_
#include "platform/types.h"
#include "console/sim.h"

#ifdef NTJ_CLIENT
class VocalStatus
{
public:
	enum VocalStatusType
	{
		VOCALSTATUS_NONE				= 0,              // 无吟唱状态             
		VOCALSTATUS_COMPOSE				= 1,              // 合成
		VOCALSTATUS_COLLECT				= 2,			  // 采集
		VOCALSTATUS_SPAWN_PET			= 3,			  // 召唤宠物
		VOCALSTATUS_USE_PET_EGG			= 4,			  // 使用宠物蛋
		VOCALSTATUS_USE_MOUNT_PET_EGG	= 5,			  // 使用骑宠蛋
		VOCALSTATUS_SPAWN_MOUNT_PET		= 6,			  // 召唤骑宠
	    VOCALSTATUS_SKILL               = 7,              // 施法
		VOCALSTATUS_ITEMSPLIT           = 8,              // 物品分解
		VOCALSTATUS_ITEMCOMPOSE         = 9,              // 物品合成
		VOCALSTATUS_EXCEPT				= 10,             // 吟唱异常
		VOCALSTATUS_MAX
	};

public:
	VocalStatus():mVocalStatus(false),mVocalType(VOCALSTATUS_NONE),mAnimation(0){}
	~VocalStatus(){ clear();}
	void                      clear();
	inline	bool              getStatus()     { return mVocalStatus;}
	bool                      setStatus(VocalStatusType vocalType,bool isVocal, U32 animation = 0);
	inline VocalStatusType    getStatusType() { return mVocalType;};
	void                      cancelVocalEvent();
	StringTableEntry          getStateName();
	U32                       getAnimation() { return mAnimation;}
private:
	bool             mVocalStatus;      // 吟唱状态
	VocalStatusType  mVocalType;        // 吟唱类型
	U32              mAnimation;        // 动作
};
#endif
///////////////////////////////////////////////////////////////////////////////////////
#ifdef NTJ_SERVER
class Player;
class ScheduleEvent
{
public:
	ScheduleEvent():mEventId(0),mTime(0),mAnimation(0){}
	~ScheduleEvent(){clear();};
	void                 clear();
	inline  S32          getEventID() {return mEventId;}
	inline  SimTime      getTime()    {return mTime;}
	void                 setEvent(S32 iEventId,SimTime iTime,U32 animation = 0) {mEventId = iEventId;mTime = iTime;mAnimation = animation;}
	void                 cancelEvent(Player* pPlayer);
	U32                  getAnimation() { return mAnimation;}
private:
	S32      mEventId;
	SimTime  mTime;
	U32      mAnimation; // 动作
};
#endif

#endif