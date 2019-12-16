#include "EventManager.h"
#include "WorldServer.h"
#include "wintcp/dtServerSocket.h"
#include "Common/CommonPacket.h"

CEventManager::CEventManager()
{
	memset( mEvents, 0, sizeof( stGlobalEvent ) * stGlobalEvent::MaxEventCount );

	registerEvent( ZONE_WORLD_EventNotify, &CEventManager::HandleZoneEventNotify );
}

void CEventManager::HandleZoneEventNotify( int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet )
{
	int id = Packet->readInt( 8 );

	mEvents[id].id = id;
	mEvents[id].time = Packet->readInt( 32 );
	mEvents[id].type = Packet->readInt( 8 );
}

void CEventManager::HandleClientEventRequest( int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet )
{
	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, CLIENT_WORLD_EventResponse, pHead->Id, SERVICE_CLIENT );

	for( int i = 0; i < stGlobalEvent::MaxEventCount; i++ )
	{
		sendPacket.writeInt( mEvents[i].time, 32 );
		sendPacket.writeInt( mEvents[i].type, 8 );
	}

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket );
}