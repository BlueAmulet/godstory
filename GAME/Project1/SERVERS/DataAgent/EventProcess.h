#ifndef EVENT_FN_H
#define EVENT_FN_H

#include "wintcp/IPacket.h"

namespace EventFn
{
	void Initialize();
	bool HandleServerDataAgentMessage(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	bool HandleWorldNotifyAck(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
}


#endif