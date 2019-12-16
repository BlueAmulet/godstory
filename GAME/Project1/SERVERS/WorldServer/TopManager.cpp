#include "DBLib\dbPool.h"
#include "TopManager.h"
#include "WorldServer.h"
#include "PlayerMgr.h"
#include "wintcp/dtServerSocket.h"
#include "ServerMgr.h"

//---------------------------------------------------------------
// ��ҽ�ɫͳ�Ʒ��ؽ�ɫ�ȼ�
//---------------------------------------------------------------
int CTopLevelInst::getValue( PlayerDataRef pPlayerData )
{
	return pPlayerData->DispData.Level;
}

//---------------------------------------------------------------
// ע���������ͳ������
//---------------------------------------------------------------
void CTopManager::registerTopInstance( int type, CTopInstance* topInstance )
{
	mInstanceMap[type] = topInstance;
} 

void CTopManager::onInit()
{
	// ��������ݿ��ȡ����
	CDBOperator db( SERVER->GetDBConnPool() );

	for( int i = 0; i < TopType::Count; i++ )
	{
		TopPlayers& players = mTopPlayersMap[i];
		TopPlayers& cplayers = mCurrentTopPlayersMap[i];
		
		db->SQL( "SELECT COUNT( * ) AS ResCount FROM TopRank WHERE type = %d", i );

		int count = 0;
		if( db->More() )		
			count = db->GetInt( "ResCount" );

		players.currentCount = count;
		cplayers.currentCount = count;

		db->SQL( "SELECT * FROM TopRank WHERE type = %d", i );

		while( db->More() )
		{
			int index = db->GetInt( "idx" );
			int indexValue = db->GetInt( "idxValue" );
			int playerId = db->GetInt( "playerId" );
			std::string name = db->GetString( "playerName" );
			int value = db->GetInt( "pValue" );
			int honour = db->GetInt( "honour" );

			players[index].indexValue = indexValue;
			cplayers[index].indexValue = indexValue;

			players[index].playerId = playerId;
			cplayers[index].playerId = playerId;

			players[index].name = name;
			cplayers[index].name = name;

			players[index].honour = honour;
			cplayers[index].honour = honour;

			players[index].value = value;
			cplayers[index].value = value;
		}

	}

}
//---------------------------------------------------------------
// �����µ�ͳ������ʱ�����µ�ǰ�ĵ�ͳ�������б�
//---------------------------------------------------------------
void CTopManager::onNewDataComming( PlayerDataRef pPlayerData )
{
	if( pPlayerData.isNull() )
		return ;

	TopInstanceMap::iterator it;
	for( it =  mInstanceMap.begin(); it != mInstanceMap.end(); it++ )
	{
		int value = it->second->getValue( pPlayerData );
		int playerId = pPlayerData->BaseData.PlayerId;
		int honour = pPlayerData->DispData.honour;

		// ����ͳ�����ݣ��������ʹ�ö��̲߳���
		onData( it->first, playerId, value, pPlayerData->BaseData.PlayerName, honour );
	}
}

//---------------------------------------------------------------
// ͳ�ƾ�������
//---------------------------------------------------------------
void CTopManager::onData( int type, int playerId, int value, std::string name, int honour )
{
	removePlayer( type, playerId );

	int index = binarySearch( type, value );

	if( index == - 1 )
		return ;

	insertNew( type, index, playerId, value, name, honour );
}

//---------------------------------------------------------------
// �ر�ϵͳ����������д�����ݿ���
//---------------------------------------------------------------
void CTopManager::onShutdown()
{
	// ��������ݿ�д�����ݿ�
	CDBOperator db( SERVER->GetDBConnPool() );

	db->SQL( "DELETE FROM TopRank" );
	db->Exec();

	for( int i = 0; i < TopType::Count; i++ )
	{
		TopPlayers& players = mCurrentTopPlayersMap[i];

		for( int j = 0; j < players.currentCount; j++ )
		{
			db->SQL( "INSERT INTO TopRank ( playerId, playerName, idx, idxValue, pValue, honour, type ) VALUES (%d, '%s', %d, %d, %d, %d, %d )", players[j].playerId, players[j].name.c_str(), j,  players[j].indexValue, players[j].value, players[j].honour, i );
			db->Exec();
		}
	}
}

//---------------------------------------------------------------
// �������������ڸ������а�
//---------------------------------------------------------------
int CTopManager::binarySearch( int type, int value )
{
	TopPlayers& players = mTopPlayersMap[type];
	int count = players.currentCount;

	// ��Ϊ��
	if( count == 0 )
		return 0;

	// û���ϰ�
	if( value < players[count-1].value && count == MaxTopCount )
		return -1;

	// ��δ
	if( count < MaxTopCount && value <= players[count-1].value )
	{
		return count;
	}

	int index = 0;

	if( value > players[0].value )
	{
		return 0;
	}
	else
	{	
		int startIndex = 0;
		int endIndex = count-1;
		int halfIndex = endIndex;

		while( !( startIndex == endIndex || endIndex - startIndex == 1) )
		{ 
			halfIndex = ( startIndex + endIndex ) >> 1;

			if( players[halfIndex].value == value )
			{
				index = halfIndex;
				break;
			}

			if( players[halfIndex].value > value )
			{
				startIndex = halfIndex;
				continue;
			}

			if( players[halfIndex].value < value )
			{
				endIndex = halfIndex;
				continue;
			}
		}

		index = endIndex;
	}

	return index;
}

//---------------------------------------------------------------
// �����µ���
//---------------------------------------------------------------
void CTopManager::insertNew( int type, int index, int playerId, int value, std::string name, int honour )
{
	TopPlayers& players = mTopPlayersMap[type];

	if( players.currentCount < MaxTopCount )
		players.currentCount++;	

	int count = players.currentCount;

	int i;

	for( i = count-1; i > index ; i-- )
	{
		players[i].value = players[i-1].value;
		players[i].playerId = players[i-1].playerId;
		players[i].name = players[i-1].name;
		players[i].honour = players[i-1].honour;
	}

	players[index].playerId = playerId;
	players[index].value = value;
	players[index].name = name;
	players[index].honour = honour;
}

CTopManager::CTopManager()
{
	// ע�����а����
	static CTopLevelInst topInst;
	registerTopInstance( TopType::Level, &topInst );

	// ע�������¼�����
	registerEvent( Client_WORLD_RequestRankSeriesInfo, &CTopManager::HandleRankRequest );
	registerEvent( Client_WORLD_RequestSnooty, &CTopManager::HandleSnootyReuqest );
	registerEvent( Client_WORLD_RequestRevere, &CTopManager::HandleRevereReuqest );
	registerEvent( ZONE_WORLD_UpdateTop, &CTopManager::HandleUpdateRankRequest );

	// ע��ȫ����
	gLock.registerLockable( this, Lock::Top );

	onInit();
}

//---------------------------------------------------------------
// �������а���Ϣ����
//---------------------------------------------------------------
void CTopManager::HandleRankRequest( int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet )
{
	DO_LOCK( Lock::Player | Lock::Top );

	int playerId = pHead->Id;
	int nPage = pHead->SrcZoneId;
	int nType = pHead->DestZoneId;

	stAccountInfo* pa = SERVER->GetPlayerManager()->GetAccountInfo( playerId );

	if( !pa )
		return ;

	CMemGuard buf( MAX_PACKET_SIZE );

	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, Client_WORLD_RequestRankSeriesInfo, pa->AccountId, SERVICE_CLIENT, nType, nPage );

	packTopData( playerId, nType, sendPacket, nPage );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket );
}

CTopManager::~CTopManager()
{
	onShutdown();
}
//---------------------------------------------------------------
// ����������е����
//---------------------------------------------------------------
void CTopManager::removePlayer( int type, int playerId )
{
	TopPlayers& players = mTopPlayersMap[type];
	int count = players.currentCount;

	for( int i = 0; i < count; i++ )
	{
		if( players[i].playerId == playerId )
		{
			for( int j = i; j < count - 1; j++ )
			{
				players[j].playerId = players[j+1].playerId;
				players[j].value = players[j+1].value;
				players[j].name = players[j+1].name;
				players[j].honour = players[j+1].honour;
			}

			players.currentCount--;
			break;
		}
	}
}

//---------------------------------------------------------------
// ������Ϣ���
//---------------------------------------------------------------
void CTopManager::packTopData( int playerId, int type, Base::BitStream& packet, int page )
{
	TopPlayers& players = mCurrentTopPlayersMap[type];
	int count = players.currentCount;

	int playerIndex = -1;

	DO_LOCK( Lock::Player | Lock::Top );

	PlayerDataRef pPlayer;

	int start = 0;
	int end = 9;

	if( page > 1 )
	{
		start = ( page - 1 ) * 15 + 9;
		end = start + 15;
	}

	if( count > end )
		count = end;

	if( count > players.currentCount )
		count = players.currentCount;

	packet.writeInt( count, 16 );

	for( int i = start; i < end && i < players.currentCount; i++ )
	{
		packet.writeInt( players[i].playerId, 32 );
		packet.writeInt( players[i].value, 32 );
		packet.writeInt( players[i].honour, 32 );
		packet.writeSignedInt( players[i].indexValue, 16 );
		packet.writeString( players[i].name.c_str() );
		pPlayer = SERVER->GetPlayerManager()->GetPlayerData( players[i].playerId );
		packet.writeFlag( !pPlayer.isNull() ); // ����Ƿ�����

		if( players[i].playerId == playerId )
			playerIndex = i;
	}

	packet.writeSignedInt( playerIndex, 16 );
}

//---------------------------------------------------------------
// ͳ������ ��Ҫ�����ϵ���Ҽ�������ֵ��
//---------------------------------------------------------------
void CTopManager::statistic( int type )
{
	DO_LOCK( Lock::Top );

	TempTopPlayersMap map;

	// �����ʱ��
	int count = 0;
	TopPlayers& players = mCurrentTopPlayersMap[type];
	count = players.currentCount;

	for( int i = 0; i < count; i++ )
	{
		map[players[i].playerId].name = players[i].name;
		map[players[i].playerId].pIdx = i + 1;
		map[players[i].playerId].value = players[i].value;
		map[players[i].playerId].honour = players[i].honour;
	}

	players = mTopPlayersMap[type];
	count = players.currentCount;

	for( int i = 0; i < count; i++ )
	{
		map[players[i].playerId].name = players[i].name;
		map[players[i].playerId].nIdx = i + 1;
		map[players[i].playerId].value = players[i].value;
		map[players[i].playerId].honour = players[i].honour;
	}

	// �����������
	TempTopPlayersMap::iterator it;
	TempTopPlayersMap::iterator itt;
	for( it = map.begin(); it != map.end(); )
	{
		// ��������
		if( it->second.nIdx == 0 )
		{
			itt = it;
			itt++;
			map.erase( it );
			it = itt;
			continue;
		}

		if( it->second.pIdx == 0 )
			it->second.indexValue = MaxTopCount - it->second.nIdx;
		else
			it->second.indexValue = -( it->second.nIdx - it->second.pIdx );

		it++;
	}

	// ���µİ���Ϣ���Ƶ��ɵİ���Ϣ��
	TopPlayers& oldPlayers = mCurrentTopPlayersMap[type];

	oldPlayers.currentCount = map.size();

	for( itt = map.begin(); itt != map.end(); itt++ )
	{
		int index = itt->second.nIdx - 1;
		oldPlayers[index].honour = itt->second.honour;
		oldPlayers[index].name = itt->second.name;
		oldPlayers[index].value = itt->second.value;
		oldPlayers[index].playerId = itt->first;
		oldPlayers[index].indexValue = itt->second.indexValue;
	}
}

//---------------------------------------------------------------
// ��������Zone�ĸ��°�����
//---------------------------------------------------------------
void CTopManager::HandleUpdateRankRequest( int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet )
{
	int type = pHead->Id;

	statistic( type );
}

//---------------------------------------------------------------
// ��������
//---------------------------------------------------------------
void CTopManager::HandleSnootyReuqest( int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet )
{
	int destPlayerId = pHead->Id;

	DO_LOCK( Lock::Player );

	stAccountInfo* pInfo = SERVER->GetPlayerManager()->GetAccountInfo( destPlayerId );
	PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( destPlayerId );

	if( !pInfo || pPlayer.isNull() )
		return ;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WORLD_ZONE_Snooty, destPlayerId, SERVICE_ZONESERVER );

	int serverSocket = SERVER->GetServerManager()->GetZoneSocket( pInfo->LineId, pPlayer->DispData.ZoneId );
	if( !serverSocket )
		return ;

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( serverSocket, sendPacket );
}

//---------------------------------------------------------------
// ��������
//---------------------------------------------------------------
void CTopManager::HandleRevereReuqest( int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet )
{
	int destPlayerId = pHead->Id;

	DO_LOCK( Lock::Player );

	stAccountInfo* pInfo = SERVER->GetPlayerManager()->GetAccountInfo( destPlayerId );
	PlayerDataRef pPlayer = SERVER->GetPlayerManager()->GetPlayerData( destPlayerId );

	if( !pInfo || pPlayer.isNull() )
		return ;

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WORLD_ZONE_Revere, destPlayerId, SERVICE_ZONESERVER );

	int serverSocket = SERVER->GetServerManager()->GetZoneSocket( pInfo->LineId, pPlayer->DispData.ZoneId );
	if( !serverSocket )
		return ;

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( serverSocket, sendPacket );
}