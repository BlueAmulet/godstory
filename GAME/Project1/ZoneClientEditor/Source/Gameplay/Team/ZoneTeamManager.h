#ifndef ZONE_TEAM_MANAGER_H
#define ZONE_TEAM_MANAGER_H

#include "Base/Locker.h"
#include "Base/bitStream.h"

#include "Common/CommonPacket.h"

#include <hash_map>
#include <list>
#include <string>

class CZoneTeam
{
public:
	CZoneTeam( int nId = 0 );
	friend class CZoneTeamManager;

	void SetCaption( int nPlayerId );
	int GetCaption();

	void AddPlayer( int nPlayerId );
	void RemovePlayer( int nPlayerId );

	bool IsTeammate( int nPlayerId );

	void SetTeamName( std::string strName );
	void GetTeamName( char* strName, U32 szSize);

	int GetTeammateCount();

	int GetPlayerId( int nIndex );

	int GetTeamId();

	void AddShareMission(int iMissionID);
	void DelShareMission(int iMissionID);
	bool GetShareMission(int nPlayerId);

private:
	std::list<int>::iterator FindPlayer( int nPlayerId );

	int m_nId;
	char m_cU;
	char m_cV;
	std::string m_strName;
	CMyCriticalSection m_cs;

	stdext::hash_map<S32, S32> m_ShareMissionList;		// 玩家共享任务列表

	std::list<int> m_playerList;
};

class CZoneTeamManager
{
public:
	CZoneTeamManager();
	virtual ~CZoneTeamManager();

public:
	CMyCriticalSection m_cs;
	
public:

	void OnTeamInfoNotify( stPacketHead* pHead, Base::BitStream& packet );
	void OnTeamBaseInfoNotify( stPacketHead* pHead, Base::BitStream& packet );
	void OnTeamPlayerJoin( int nPlayerId, int nTeamId );
	void OnTeamPlayerLeave( int nPlayerId, int nTeamId );

	void OnPlayerEnterZone( int nPlayerId, int nTeamId );
	void OnPlayerLeaveZone( int nPlayerId );

	void OnTeamDisband( int nTeamId );

	CZoneTeam* GetTeam( int nTeamId );
	CZoneTeam* GetTeamByPlayerId( int nPlayerId );

	stdext::hash_map<int,CZoneTeam*> m_playerTeamMap;		// PlayerId 为键值
	stdext::hash_map<int,CZoneTeam*> m_idTeamMap;			// TeamId 为键值
};

extern CZoneTeamManager g_ZoneTeamManager;

#define ZONE_TEAM_MGR (&g_ZoneTeamManager)

#endif
