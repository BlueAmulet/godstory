#include "GamePlay/ClientGamePlayState.h"
#include "GamePlay/GameObjects/PlayerObject.h"
#include "NetWork/UserPacketProcess.h"
#include "ClientPlayerManager.h"
#include "Common/PlayerStruct.h"

CClientPlayerManager::CClientPlayerManager()
{

}

CClientPlayerManager::~CClientPlayerManager()
{

}

void CClientPlayerManager::SendPlayerDataTransRequest( int nPlayerId, int nFlag )
{
	UserPacketProcess* pProcess = g_ClientGameplayState->GetPacketProcess();
	if( !pProcess )
		return ;

	pProcess->SendSimplePacket( nPlayerId, CLIENT_WORLD_PlayerDataTransRequest, nFlag );
}

void CClientPlayerManager::HandlePlayerDataTransResponse( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int nPlayerId = pHead->DestZoneId;
	int nType = pHead->SrcZoneId;

	if( nType == 0 )
	{
		m_offlinePlayers.insert( nPlayerId );
		return ;
	}
	else
	{
		m_offlinePlayers.erase( nPlayerId );
	}

	if( nType & ( 1 << PLAYER_TRANS_TYPE_BASE ) )
		UnpackPlayerDataTransBase( m_playerBaseMap[nPlayerId], RecvPacket );

	if( nType & ( 1 << PLAYER_TRANS_TYPE_NORMAL ) )
		UnpackPlayerDataTransNormal( m_playerNormalMap[nPlayerId], RecvPacket );
}

bool CClientPlayerManager::IsPlayerOffline( int nPlayerId )
{
	return m_offlinePlayers.find( nPlayerId ) != m_offlinePlayers.end();
}

void CClientPlayerManager::SetPlayerIdByName( std::string strName, int nPlayerId )
{
	m_playerNameIdMap[ strName ] = nPlayerId;
	m_playerNameMap[nPlayerId] = strName;
}

int CClientPlayerManager::GetPlayerIdByName( std::string strName )
{
	stdext::hash_map<std::string, int>::iterator it;
	it = m_playerNameIdMap.find( strName );
	if( it != m_playerNameIdMap.end() )
		return it->second;

	return NULL;
}

void CClientPlayerManager::AddLocalPlayer( int nPlayer, Player* pPlayer )
{
	if( !pPlayer )
		return ;

	stPlayerDisp& playerDisp = GetPlayerDisp( nPlayer );

	playerDisp.Level = pPlayer->getLevel();
	playerDisp.Sex = pPlayer->getSex();
	playerDisp.Family = pPlayer->getFamily();

	m_localPlayerMap[ nPlayer ] = pPlayer;

	SetPlayerIdByName( pPlayer->getPlayerName(), nPlayer );

	m_offlinePlayers.erase( nPlayer );
}

void CClientPlayerManager::RemoveLocalPlayer( int nPlayer )
{
	m_localPlayerMap.erase( nPlayer );
}

int CClientPlayerManager::GetPlayerData( int nPlayerId, stPlayerTransferNormal& playerNormal )
{
	stdext::hash_map<int,Player*>::iterator lit;

	lit = m_localPlayerMap.find( nPlayerId );
	if( lit != m_localPlayerMap.end() )
	{
		dStrcpy( playerNormal.szPlayerName, sizeof(playerNormal.szPlayerName), lit->second->getPlayerName() );	
		return true;
	}

	stdext::hash_map<int,stPlayerTransferNormal>::iterator it;

	it = m_playerNormalMap.find( nPlayerId );
	if( it != m_playerNormalMap.end() )
	{
		memcpy( &playerNormal, &(it->second), sizeof( stPlayerTransferNormal ) );
		return true;
	}

	return false;
}

int CClientPlayerManager::GetPlayerData( int nPlayerId, stPlayerTransferBase& playerBase )
{
	stdext::hash_map<int,Player*>::iterator lit;

	lit = m_localPlayerMap.find( nPlayerId );
	if( lit != m_localPlayerMap.end() )
	{
		return -1;
	}

	stdext::hash_map<int,stPlayerTransferBase>::iterator it;

	it = m_playerBaseMap.find( nPlayerId );
	if( it != m_playerBaseMap.end() )
	{
		memcpy( &playerBase, &(it->second), sizeof( stPlayerTransferBase ) );
		return 1;
	}

	return false;
}

stdext::hash_map<int,Player*>* CClientPlayerManager::GetLocalPlayerList()
{
	return &m_localPlayerMap;
}

void CClientPlayerManager::UnpackPlayerDataTransBase( stPlayerTransferBase& playerBase, Base::BitStream& packet )
{
	playerBase.nPlayerHp = packet.readInt( Base::Bit32 );
	playerBase.nPlayerMp = packet.readInt( Base::Bit32 );
}

void CClientPlayerManager::UnpackPlayerDataTransNormal( stPlayerTransferNormal& playerNormal, Base::BitStream& packet )
{
	packet.readString( playerNormal.szPlayerName,COMMON_STRING_LENGTH );
}

Player* CClientPlayerManager::GetLocalPlayer( int nPlayer )
{
	stdext::hash_map<int,Player*>::iterator lit;

	lit = m_localPlayerMap.find( nPlayer );
	if( lit != m_localPlayerMap.end() )
	{
		return lit->second;
	}

	return NULL;
}

void CClientPlayerManager::ClearLocalPlayers()
{
	m_localPlayerMap.clear();
}

stPlayerDisp& CClientPlayerManager::GetPlayerDisp( int playerId )
{
	return m_playerDispMap[playerId];
}

std::string& CClientPlayerManager::getPlayerName( int playerId )
{
	std::string& strName = m_playerNameMap[playerId];

	Player* pPlayer = GetLocalPlayer( playerId );
	if( pPlayer )
		strName = pPlayer->getPlayerName();

	return strName;
}

CClientPlayerManager* CClientPlayerManager::GetInstance()
{
	static CClientPlayerManager instance;
	return &instance;
}
ConsoleFunction( GetLocalPlayerCount, int, 1, 1, "" )
{
	return PLAYER_MGR->GetLocalPlayerList()->size();
}

ConsoleFunction( GetLocalPlayerName,  const char*, 2,2,"" )
{
	Player* pPlayer = PLAYER_MGR->GetLocalPlayer( atoi(argv[1]) );
	if( pPlayer )
		return pPlayer->getPlayerName();

	return NULL;
}

ConsoleFunction( GetPlayerTeamId, int, 2,2, "" )
{
	Player* pPlayer = PLAYER_MGR->GetLocalPlayer( atoi(argv[1]) );
	if( pPlayer )
		return pPlayer->getTeamInfo().m_nId;

	return NULL;
}

ConsoleFunction( GetPlayerIdByName, int, 2, 2, "" )
{
	return PLAYER_MGR->GetPlayerIdByName( argv[1] );
}

ConsoleFunction( Cache_GetPlayerName, const char*, 2, 2, "" )
{
	const char* playerName = PLAYER_MGR->getPlayerName(atoi(argv[1])).c_str();

	return playerName;
}

ConsoleFunction( Cache_GetPlayerSex, int, 2, 2, "" )
{
	stPlayerDisp& disp = PLAYER_MGR->GetPlayerDisp(atoi(argv[1]));
	Player* pPlayer = PLAYER_MGR->GetLocalPlayer(atoi(argv[1]));
	if( pPlayer )
		disp.Sex = pPlayer->getSex();

	return disp.Sex;
}

ConsoleFunction( Cache_GetPlayerLevel, int, 2, 2, "" )
{
	stPlayerDisp& disp = PLAYER_MGR->GetPlayerDisp(atoi(argv[1]));
	Player* pPlayer = PLAYER_MGR->GetLocalPlayer(atoi(argv[1]));
	if( pPlayer )
		disp.Level = pPlayer->getLevel();

	return disp.Level;
}

ConsoleFunction( Cache_GetPlayerFamily, int, 2, 2, "" )
{
	stPlayerDisp& disp = PLAYER_MGR->GetPlayerDisp(atoi(argv[1]));
	Player* pPlayer = PLAYER_MGR->GetLocalPlayer(atoi(argv[1]));
	if( pPlayer )
		disp.Family = pPlayer->getFamily();

	return disp.Family;
}

ConsoleFunction( Cache_GetPlayerTeamId, int, 2, 2, "" )
{
	stPlayerDisp& disp = PLAYER_MGR->GetPlayerDisp(atoi(argv[1]));
	Player* pPlayer = PLAYER_MGR->GetLocalPlayer(atoi(argv[1]));
	if( pPlayer )
		disp.TeamId = pPlayer->getTeamInfo().m_nId;

	return disp.TeamId;
}