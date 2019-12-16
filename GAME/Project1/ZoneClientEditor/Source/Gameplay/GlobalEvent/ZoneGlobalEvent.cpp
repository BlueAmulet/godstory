#include "../ServerGameplayState.h"
#include "ZoneGlobalEvent.h"
#include "..\ServerGameplayState.h"
#include "common/GlobalEvent.h"
#include "NetWork/ServerPacketProcess.h"
#include "console/console.h"

extern void CZoneGlobalEvent::sendGlobalEvent( int id, int type )
{

	char buf[MAX_PACKET_SIZE];

	Base::BitStream sendPakcet( buf, MAX_PACKET_SIZE );
	stPacketHead* pHead = IPacket::BuildPacketHead( sendPakcet, ZONE_WORLD_EventNotify, id, SERVICE_WORLDSERVER, type, _time32(NULL) );
	pHead->PacketSize = sendPakcet.getPosition() - sizeof( stPacketHead );

	ServerPacketProcess* pProcess =  g_ServerGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send( sendPakcet );
}

ConsoleFunction( SptZone_SendEvent, void, 3, 3, "SptZone_SendEvent(%id, %type)" )
{
	CZoneGlobalEvent::sendGlobalEvent( atoi( argv[1] ), atoi( argv[2] ) );
} 