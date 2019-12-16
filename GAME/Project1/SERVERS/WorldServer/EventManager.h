#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "Common/GlobalEvent.h"
#include "ManagerBase.h"
#include "LockCommon.h"

class CEventManager : public ILockable, public CManagerBase< CEventManager, MSG_EVENT_BEGIN, MSG_EVENT_END >
{
public:
	CEventManager();
	void HandleZoneEventNotify( int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet );
	void HandleClientEventRequest(  int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet  );

private:
	stGlobalEvent mEvents[stGlobalEvent::MaxEventCount];
};

#endif