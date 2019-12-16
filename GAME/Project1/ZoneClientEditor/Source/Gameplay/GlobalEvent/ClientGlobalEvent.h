//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef CLIENT_GLOBAL_EVENT_H
#define CLIENT_GLOBAL_EVENT_H

#include "Base/bitStream.h"
#include "Common/PacketType.h"
#include "Common/CommonPacket.h"
#include "Common/GlobalEvent.h"

#define UpdateInterval   1000

class Player;
class CClientGlobalEvent 
{
protected:
	bool            mbShowAliveNpc;                          // ����Npc
	//bool            mbReliveNpc;                             // ����֪ͨ
	bool            mbShowActiveStart;                       // ��ǰ�����Ļ
	bool            mbShowCurrentDay;                        // ��ʾ����
	//bool            mbEventNotify;                           // �¼�֪ͨ
	bool            mbActivityNotify;                        // �������ʾ
	bool            mFirstUpdate;

	SimTime		    mLastUpdateTime;                          //< Unit: ms.
	SimTime		    mCurrentTime;

	Platform::LocalTime   mLocalTime;                                // ���ط���


private:
	stGlobalEvent mEvents[stGlobalEvent::MaxEventCount];
public:
	CClientGlobalEvent();
	~CClientGlobalEvent();

	inline bool getShowAliveNpc    ()              { return mbShowAliveNpc;}
	inline void setShowAliveNpc    (bool value)    { mbShowAliveNpc = value;}
	//inline bool getReliveNpc       ()              { return mbReliveNpc;}
	//inline void setReliveNpc       (bool value)    { mbReliveNpc = value;}
	inline bool getActiveStart     ()              { return mbShowActiveStart;}
	inline void setActiveStart     (bool value)    { mbShowActiveStart = value;}
	inline bool getShowCurrentDay  ()              { return mbShowCurrentDay;}
	inline void setShowCurrentDay  (bool value)    { mbShowCurrentDay = value;}
	//inline bool getEventNotify     ()              { return mbEventNotify;}
	//inline void setEventNotify     (bool value)    { mbEventNotify = value;}
	inline bool getActivityNotify  ()              { return mbActivityNotify;}
	inline void setActivityNotify  (bool value)    { mbActivityNotify = value;}
	inline bool isFirstUpdate      ()              { return mFirstUpdate;}
	inline void setFirstUpdate     (bool value)    { mFirstUpdate = value;}

	void loadConfigIni();
	void writeCoinfgIni();

	bool updateData();
	void queryEveryday(Platform::LocalTime &lt);
	void queryWeekDay(Platform::LocalTime &lt);
	void queryDaily(Platform::LocalTime &lt);
	void queryEvents();

	void queryCallboardMessage(Player* pPlayer);
	void handleClientWorldEventResponse( stPacketHead* pHead, Base::BitStream& packet );
};

extern CClientGlobalEvent g_CClientGlobalEvent;

#endif