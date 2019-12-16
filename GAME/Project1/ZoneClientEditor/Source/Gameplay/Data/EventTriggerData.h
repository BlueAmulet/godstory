#pragma once

#include "platform/types.h"
#include <hash_map>

struct EventTriggerData
{
	U32					mTriggerId;			//�¼�������ID
	U32					mCoolDownTime;		//��ȴʱ��
	StringTableEntry	mTiggerScript;		//�����ű�
	U32					mLivingSkillId;		//�����ID
	U8					mEventType;			//�����¼�����
	U32					mEventId;			//�����¼�ID
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