#include "Gameplay/Data/EventTriggerData.h"
#include "Gameplay/Data/readDataFile.h"

#define GAME_EVENT_TRIGGER_DATA_FILE "gameres/data/repository/EventTriggerRepository.dat"

EventTriggerRepository gEventTriggerRepository;

EventTriggerRepository::EventTriggerRepository()
{
	
}

EventTriggerRepository::~EventTriggerRepository()
{
	clear();
}

void EventTriggerRepository::read()
{
	CDataFile op;
	RData tempdata;
	char filename[256];
	EventTriggerData* pEventTriggerData = NULL;

	Platform::makeFullPathName(GAME_EVENT_TRIGGER_DATA_FILE, filename, sizeof(filename));
	if(!op.readDataFile(filename))
		return;

	for(int i=0; i<op.RecordNum; ++i)
	{
		pEventTriggerData = new EventTriggerData;

		// 事件触发器ID
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "EventTriggerRepository.dat::Read - failed to read mTriggerId!", i);
		pEventTriggerData->mTriggerId = tempdata.m_U32;

		// 触发器CD(cool down time)
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "EventTriggerRepository.dat::Read - failed to read mCoolDownTime!", i);
		pEventTriggerData->mCoolDownTime = tempdata.m_U32;

		// 触发脚本
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "EventTriggerRepository.dat::Read - failed to read mTiggerScript!", i);
		pEventTriggerData->mTiggerScript = tempdata.m_string;

		// 生活技能ID
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "EventTriggerRepository.dat::Read - failed to read mLivingSkillId!", i);
		pEventTriggerData->mLivingSkillId = tempdata.m_U32;

		// 触发事件类型    (掉物事件、技能对象、脚本触发)
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_enum8, "EventTriggerRepository.dat::Read - failed to read mEventType!", i);
		pEventTriggerData->mEventType = tempdata.m_Enum8;

		// 触发事件ID
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "EventTriggerRepository.dat::Read - failed to read mEventId!", i);
		pEventTriggerData->mEventId = tempdata.m_U32;

		insert(pEventTriggerData);
	}
}

void EventTriggerRepository::clear()
{
	for(EventTriggerMap::iterator it = m_EventTriggerMap.begin(); it != m_EventTriggerMap.end(); ++it)
	{
		if(it->second)
			delete it->second;
	}
	m_EventTriggerMap.clear();
}

bool EventTriggerRepository::insert( EventTriggerData* pEventTriggerData )
{
	if(!pEventTriggerData)
		return false;

	m_EventTriggerMap.insert( EventTriggerMap::value_type(pEventTriggerData->mTriggerId, pEventTriggerData) );
	return true;
}

EventTriggerData* EventTriggerRepository::getEventTriggerData( U32 nTriggerId )
{
	EventTriggerMap::iterator it = m_EventTriggerMap.find(nTriggerId);

	if (it != m_EventTriggerMap.end())
		return it->second;

	return NULL;
}

void EventTriggerRepository::AssertErrorMsg( bool bAssert, const char *msg, int rowNum )
{
	char buf[256];

	if (!bAssert)
	{
		dSprintf(buf, sizeof(buf), "RowNum: %d  ", rowNum);
		dStrcat(buf, sizeof(buf), msg);
		AssertRelease(false, buf);
	}
}