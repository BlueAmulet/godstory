#ifndef CLIENT_PLAYER_MANAGER_H
#define CLIENT_PLAYER_MANAGER_H

#include <hash_set>
#include <hash_map>
#include <string>

//#include "Base/Locker.h"
#include "Base/bitStream.h"
#include "Common/PacketType.h"


struct stPlayerTransferBase;
struct stPlayerTransferNoraml;
struct stPacketHead;
class Player;

#define MAX_PLAYER_DATA_TRANS_PENDING_TIME		10000		//每十秒钟允许一次查询

class CClientPlayerManager
{
public:
	CClientPlayerManager();
	virtual ~CClientPlayerManager();

	void SendPlayerDataTransRequest( int nPlayerId, int nFlag );

	int GetPlayerData( int nPlayerId, stPlayerTransferBase& playerBase );
	int GetPlayerData( int nPlayerId, stPlayerTransferNormal& playerNormal );

	void HandlePlayerDataTransResponse( stPacketHead *pHead,Base::BitStream &RecvPacket );

	void SetPlayerIdByName( std::string strName, int nPlayerId );
	int GetPlayerIdByName( std::string strName );
	std::string& getPlayerName( int playerId );

	stPlayerDisp& GetPlayerDisp( int playerId );

	void AddLocalPlayer( int nPlayer, Player* pPlayer );
	void RemoveLocalPlayer( int nPlayer );
	Player* GetLocalPlayer( int nPlayer );
	stdext::hash_map<int,Player*>* GetLocalPlayerList();
	void ClearLocalPlayers();

	bool IsPlayerOffline( int nPlayerId );

	void UnpackPlayerDataTransBase( stPlayerTransferBase& playerBase, Base::BitStream& packet );
	void UnpackPlayerDataTransNormal( stPlayerTransferNormal& playerNormal, Base::BitStream& packet );

	static CClientPlayerManager* GetInstance();

	template< typename _Ty >
	void HandleInfoPacket( _Ty* packet )
	{
		stPlayerBase base;
		stPlayerDisp disp;

		base.ReadData( packet );
		disp.ReadData( packet );
		int TeamId = packet->readInt( 32 );

		stPlayerDisp& playerDisp = GetPlayerDisp( base.PlayerId );
		std::string name = base.PlayerName;
		SetPlayerIdByName( name, base.PlayerId );
		playerDisp.Sex = disp.Sex;
		playerDisp.Family = disp.Family;
		playerDisp.Level = disp.Level;
		playerDisp.TeamId = TeamId;
	}

private:
	stdext::hash_map<int,unsigned int>				m_pandingMap;

	stdext::hash_map<int,stPlayerTransferBase>		m_playerBaseMap;
	stdext::hash_map<int,stPlayerTransferNormal>	m_playerNormalMap;

	stdext::hash_map<int,stPlayerDisp>				m_playerDispMap;
	stdext::hash_map<int, std::string>				m_playerNameMap;

	stdext::hash_set<int>							m_offlinePlayers;
public:
	stdext::hash_map<int,Player*>					m_localPlayerMap;
	
	stdext::hash_map<std::string, int>				m_playerNameIdMap;
};

#define PLAYER_MGR CClientPlayerManager::GetInstance()

#endif

