//
// NTianJi Game Engine V1.0
//
//
// Name:
//		ServerGameplayState.h
//
// Abstract:
//

#pragma once

#include <time.h>
#include <hash_map>
#include "platform/platform.h"
#include "common/PacketType.h"
#include "common/PlayerStruct.h"
#include "common/TeamBase.h"
#include <hash_set>

// ========================================================================================================================================
//	ServerGameplayState
// ========================================================================================================================================
///	Server gameplay state.
///
struct PlayerLoginBlock
{
	T_UID UID;
	U32	  Time;
	
	stTeamInfo teamInfo;

	stPlayerStruct *pPlayerData;

	PlayerLoginBlock()
	{
		memset( this, 0, sizeof( PlayerLoginBlock ) );
	}
};

class Player;
class TCPObject2;
class ServerPacketProcess;
class CDataAgentHelper;
struct stTeamInfo;
class CLogHelper;

typedef void* HANDLE;

class ServerGameplayState
{
	StringTableEntry mClientMisFileName;
	U32				 mClientMisFileCrc;
	U32				 mZoneId;
	U32				 mMaxPlayerNum;
public:
	HANDLE			m_StartHandle;
	HANDLE			m_StopHandle;
private:
	TCPObject2		*m_pGateLink;
	int				 m_LoginGateIp;
	int				 m_LoginGatePort;
	int				 mCurrentLineId;
	int				 mCurrentGateId;

	typedef stdext::hash_map<U32,PlayerLoginBlock> HashPlayerLoginBlock;
	HashPlayerLoginBlock	m_PlayerLoginData;
	typedef stdext::hash_map<U32,stPlayerStruct*> HashPlayerSaveMap;
	HashPlayerSaveMap		m_PlayerSaveMap;
	typedef stdext::hash_map< int, int > HashPlayerSIDMap;

	typedef stdext::hash_map<U32,Player *> HashPlayer;
	typedef stdext::hash_map<U32,U32> HashAccount;
	HashAccount				m_AccountMap;
	HashPlayer				m_PlayerManager;
	HashPlayerSIDMap		m_PlayerSIDMap;

	CDataAgentHelper*		m_pDataAgentHelper;
    CLogHelper*             m_pLogHelper;
	stdext::hash_set<int>	m_copyMapSet;
public:
	ServerGameplayState										();
	~ServerGameplayState									();

	void			 Initialize								();
	void			 preShutdown							();
	void			 Shutdown								();

	void			 TimeLoop								(S32 elapsedTime);
	U32				 getZoneId								()								{return mZoneId;}

	void			 setGateIP								(int IP)						{m_LoginGateIp = IP;}
	int				 getGateIP								()								{return m_LoginGateIp;}
	void			 setGatePort							(int Port)						{m_LoginGatePort = Port;}
	int				 getGatePort							()								{return m_LoginGatePort;}

	void			 setCurrentLineId						(int LineId)					{mCurrentLineId = LineId;}
	int				 getCurrentLineId						()								{return mCurrentLineId;}
	void			 setCurrentGateId						(int GateId)					{mCurrentGateId = GateId;}
	int				 getCurrentGateId						()								{return mCurrentGateId;}

	void			 setMaxPlayerNum						(U32 MaxPlayerNum)				{mMaxPlayerNum=MaxPlayerNum;}
	U32				 getMaxPlayerNum						()								{return mMaxPlayerNum;}

	StringTableEntry getClientMisFileName					()								{return mClientMisFileName;}
	U32				 getClientMisFileCRC					()								{return mClientMisFileCrc; }
	void			 setClientMisFileName					(StringTableEntry MisFileName)	{mClientMisFileName = MisFileName;}
	void			 setClientMisFileCRC					(U32 MisCrc)					{mClientMisFileCrc = MisCrc; }

	ServerPacketProcess* GetPacketProcess					();
	CDataAgentHelper*	GetDataAgentHelper					()								{return m_pDataAgentHelper;}
    CLogHelper*         GetLog(void) ;                                                       

	stTeamInfo*		GetPlayerTeam							(T_UID UID,int PlayerId);

	//玩家登陆数据管理
	void			 AddPlayerLoginData						(T_UID UID,stTeamInfo*,stPlayerStruct *);
	stPlayerStruct	*GetPlayerLoginData						(T_UID UID,int PlayerId);
	void			 DeletePlayerLoginData					(T_UID UID,int PlayerId);
	PlayerLoginBlock FakePlayerLoginData					(int PlayerId);
	void			 FilterPlayerLoginData					(int curtime);
	void			 SetPlayerSID							( int playerId, int SID ) { m_PlayerSIDMap[playerId] = SID; }
	int				GetPlayerSID							( int playerId ) {return m_PlayerSIDMap[playerId];}
	void			 AddInPlayerManager						(Player *);
	void			 DeleteFromPlayerManager				(Player *);

	Player *		 GetPlayer								(int PlayerId);
	Player *		GetPlayerByAccount						(int accountId);

	void			openCopyMap								(int nCopyMapInstId);
	void			closeCopyMap							(int nCopyMapInstId);
	bool			isCopyMapOpen							(int nCopyMapInstId);

public:
	stPlayerStruct* GetPlayerLoginData						( int playerId );
	stPlayerStruct* GetPlayerSaveData( U32 playerId );
	void AddPlayerSaveData( stPlayerStruct* pPlayer );
	void RemovePlayerSaveData( U32 playerId );

	bool			m_bIsRobotTest;
};

// ========================================================================================================================================
//	Global Variables
// ========================================================================================================================================
extern ServerGameplayState* g_ServerGameplayState;
