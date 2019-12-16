#ifndef EVENT_FN_H
#define EVENT_FN_H

#include "wintcp/IPacket.h"

namespace EventFn
{
	void Initialize();

	bool HandleRegisterRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	bool HandleRegisterResponse(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);

	bool HandleClientLoginResponse(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);

	bool HandleServerChatSendMessageAll(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	bool HandleServerChatSendMessage(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);

	void SwitchSendToClient(stPacketHead *pHead,Base::BitStream &switchPacket);
	void SwitchSendToZone(stPacketHead *pHead,Base::BitStream &switchPacket);
	void SwitchSendToWorld(stPacketHead *pHead,Base::BitStream &switchPacket);
	void SwitchSendToDataAgent( stPacketHead* pHead, Base::BitStream &switchPacket);

	void NotifyWorldZoneLost( int ZoneId );
}


#endif