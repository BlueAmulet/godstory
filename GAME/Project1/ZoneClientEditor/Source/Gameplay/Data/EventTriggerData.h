#pragma once

#include "platform/types.h"
#include <hash_map>

struct EventTriggerData
{
	U32					mTriggerId;			//事件触发器ID
	U32					mCoolDownTime;		//冷却时间
	StringTableEntry	mTiggerScript;		//触发脚本
	U32					mLivingSkillId;		//生活技能ID
	U8					mEventType;			//触发事件类型
	U32					mEventId;			//触发事件ID
};

class EventTriggerRepository
{
public:
	typedef stdext::hash_map<U32, EventTriggerData *> EventTriggerMap;

	EventTriggerRepository();
	~EventTriggerRepository();

	void read();
	EventTriggerData* getEventTriggerData(U32 nTriggerId);

private:
	void clear();
	bool insert(EventTriggerData* pEventTriggerData);
	void AssertErrorMsg(bool bAssert, const char *msg, int rowNum);

private:
	EventTriggerMap m_EventTriggerMap;
};

extern EventTriggerRepository gEventTriggerRepository;