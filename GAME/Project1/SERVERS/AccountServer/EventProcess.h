#ifndef EVENT_FN_H
#define EVENT_FN_H

#include "wintcp/IPacket.h"

namespace EventFn
{
	void Initialize();
	bool HandleRegisterResponse(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	bool SwitchSendToWorld(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);

	bool HandleClientLoginRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	bool HandleClientCheckRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	bool HandleWorldClientLoginResponse(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
}


#endif