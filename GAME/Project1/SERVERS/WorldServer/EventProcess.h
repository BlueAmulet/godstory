#ifndef EVENT_FN_H
#define EVENT_FN_H

#include "wintcp/IPacket.h"
#include <hash_set>
typedef stdext::hash_map<int, stAccountInfo> IdAccountMap;

extern IdAccountMap g_loginPending;



/************************************************************************/
/* 全局消息的响应
/************************************************************************/
namespace EventFn
{
	void Initialize();

	//Net Event
	bool HandleRegisterRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	bool HandleZoneRegisterRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	bool HandleClientLoginAccountRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);

	bool HandleGateZoneLost(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);

	bool HandleClientWorldSavePlayerRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet );
	bool HandleZonePlayerDispChanged( int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet );
	bool HandleDataAgentPlayerDataNotify( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet );
    bool HandleGateClientLost( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet );
    bool HandleTickPlayerFromZone( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet );
    bool HandleGetPlayerPos( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet );
	bool HandleZoneAddPoint( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet );

	
	//Game Event
	bool OnZoneServerRestart(const char *,int );
	
	// PlayerTrans Event
	bool HandleClientPlayerDataTransRequest(int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket );

    //gm message handle
    bool HandleGMMessage(int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket);
	void SendSavePlayerRequest( int accountId );
	bool HandleWorldZoneSavePlayerResponse( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet );
	bool HandleClientWorldItemRequest( int SocketHandle, stPacketHead* pHead, Base::BitStream* Packet );
}

#endif