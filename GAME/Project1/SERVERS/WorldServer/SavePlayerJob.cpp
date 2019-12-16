#define DEFAULT_DO_LOCK

#include "SavePlayerJob.h"

#include "wintcp/AsyncSocket.h"
#include "ChatMgr.h"
#include "TeamManager.h"
#include "Event.h"
#include "PlayerMgr.h"
#include "Common/PlayerStruct.h"
#include "WorldServer.h"
#include "EventProcess.h"
#include "Common/WorkQueue.h"

bool CSavePlayerJob::TimeProcess( bool bExit )
{
	SERVER->GetWorkQueue()->PostEvent(1,0,0,0,WQ_TIMER);

	return true;
}

void CSavePlayerJob::PostPlayerData( unsigned long UID, stPlayerStruct* pPlayerData )
{
	OLD_DO_LOCK( m_cs );
	
	int nPlayerId = pPlayerData->BaseData.PlayerId;

	char buf[MAX_PACKET_SIZE];
	Base::BitStream s( buf, MAX_PACKET_SIZE );


	pPlayerData->WriteData( &s );
	s.setPosition( 0 );
	m_playerMap[ nPlayerId ].ReadData( &s );
	//memcpy( &, pPlayerData, sizeof( stPlayerStruct ) );

	m_playerList.push_back( nPlayerId );
}

void CSavePlayerJob::SetDataAgentSocketHandle( int nPlayerId, int nSocket )
{
	OLD_DO_LOCK( m_cs );

	m_dataAgentMap[nPlayerId] = nSocket;
}

bool CSavePlayerJob::Update()
{
	OLD_DO_LOCK( m_cs );

	if( m_playerList.begin() == m_playerList.end() )
		return false;

	int nPlayerId = m_playerList.front();
	stPlayerStruct* pPlayerData = &m_playerMap[nPlayerId];
	int SID = pPlayerData->BaseData.SaveUID;

	SERVER->GetPlayerManager()->SavePlayerData( pPlayerData );

	m_playerMap.erase( nPlayerId );

	m_playerList.pop_front();

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream SendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( SendPacket, DATAAGENT_WORLD_NotifyPlayerDataAck, nPlayerId, SERVICE_DATAAGENT, SID );
	pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );
	SERVER->GetServerSocket()->Send( m_dataAgentMap[nPlayerId], SendPacket );
	return true;
}