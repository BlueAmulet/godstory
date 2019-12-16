#ifndef USER_EVENT_FN_H
#define USER_EVENT_FN_H

#include "wintcp/IPacket.h"

namespace UserEventFn
{
	void Initialize();
	void SwitchSendToWorld(stPacketHead *pHead,Base::BitStream &switchPacket);
	void NotifyWorldClientLost( int nPlayerId,T_UID uid);

	bool HandleClientLoginRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
}


#endif