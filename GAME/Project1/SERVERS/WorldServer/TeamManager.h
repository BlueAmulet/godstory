#ifndef TEAM_MANAGER_H
#define TEAM_MANAGER_H

#include <hash_map>
#include <hash_set>
#include <list>
#include <deque>
#include "Base/Locker.h"
#include "Event.h"
#include "common/CommonPacket.h"
#include <string>
#include <memory.h>
#include "common/TeamBase.h"
#include "LockCommon.h"
#include "ManagerBase.h"

#define MAX_TEAMMATE_NUM		6
#define MAX_TEAM_PENDING_COUNT	6
#define MAX_TEAM_NAME_LENGTH	40


class CTeam
{
public:
	friend class CTeamManager;

	CTeam()
	{
		m_nId = 0;
		m_bIsSquad = false;
		m_cV = 0;		// 物品分配方式
		m_cU = 0;		// 经验分配方式
		m_nCopymapId = 0; // 副本编号
		memset( m_szName, 0, MAX_TEAM_NAME_LENGTH );
	}

	int GetId();
	int GetCaption();
	void SetCaption( int nPlayerId );
	bool IsCaption( int nPlayerId );
	bool IsTeammate( int nPlayerId )	;
	std::list<int> * GetTeammate();
	void SetName( const char* str );
	const char* GetName();
	std::list<int>::iterator FindTeammate( int nPlayer );
	stTeamMapMark mapMark[MAX_TEAM_MAP_MARK_COUNT];
	stTeamTargetMark targetMark[MAX_TEAM_TARGET_COUNT];

	int		m_nId;
	char	m_szName[MAX_TEAM_NAME_LENGTH];
	bool	m_bIsSquad;
	char	m_cU;
	char	m_cV;

	int		m_nCopymapId;

	int Join( int nPlayerId );
	int Leave( int nPlayerId );
	std::list<int> m_teammate;

	// 如果是WORLDSERVER,需要有聊天频道
#ifdef WORLDSERVER

public:
	int GetChatChannelId();
	void SetCopymap( int copyMapId );
	int GetCopymap();
private:
	int m_nChatChannelId;

#endif
};

class stAccountInfo;

class CTeamManager : public CEventListener, ILockable,  public CManagerBase< CTeamManager, MSG_TEAM_BEGIN, MSG_TEAM_END >
{
public:
    CTeamManager(void);

	int BuildTeam( int nPlayerId1, int nPlayerId2, int& nTeamId );
	CTeam* GetTeam( int nTeamId );
	CTeam* GetTeamByPlayer( int nPlayerId );

	int GetTeamId( int nPlayerId );

	int JoinTeam( int nPlayerId, int nTeamId );
	void addPlayerToTeamChannel( int nPlayerId );

	void ClearPendingList( int nPlayerId );
	int LeaveTeam( int nPlayerId, int nTeamId );
	int DisbandTeamByPlayer( int nPlayerId );
	int DisbandTeam( int nTeamId );

	int CheckBuildTeam( int nPlayerId1, int nPlayerId2 );
	int CheckJoinTeam( int nPlayerId );

	void GetPlayerTeamInfo( int nPlayerId, stTeamInfo& teamInfo );
    
	// 消息处理相关
	void HandleBuildAlongRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	void HandleBuildRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	void HandleAcceptBuildRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	void HandleJoinRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	void HandleAcceptJoinRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	void HandleLeaveRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	void HandleDropRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	void HandleAddRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	void HandleAcceptAddRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	void HandleRefused(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	void HandleBaseInfo(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	void HandleDisband(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	void HandleMapMark(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	void HandleTargetMark(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	void HandleInfoRequest( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket );
	void HandleInvationCopymap( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket );
	void OnEvent(CEventSource* pEventSouce, const stEventArg& eventArg);
	
// private:
	int BuildTeamId();

	void NotifyPlayerJoined( int nPlayerId, int nPlayerIdTo, int nTeamId );
	void NotifyPlayerLeave( int nPlayerId, int nPlayerIdTo );

	void NotifyZonePlayerJoined( int nLineId, int nPlayerId, int nTeamId );
	void NotifyZonePlayerLeave( int nLIneId, int nPlayerId, int nTeamId );
	
	void SendTeamInfo( int nPlayerId );
	void SendTeamInfoToZone( int nLineId, int nTeamId );
	void SendBuildRequest( int nPlayerId, int nPlayerIdTo );

	void SendError( int nPlayerId, int nAck );
	void SendRefuse( int nPlayerId, int nPlayerIdTo, int mType );
	void SendCancel( int nPlayerId, int nPlayerIdTo, int mType );
	void HandleFollowRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	stdext::hash_map<int,CTeam*> m_teamMap;
	stdext::hash_map<int,CTeam*> m_playerTeamMap;

	typedef stdext::hash_map< int, stdext::hash_set<int> > PendingList;

	PendingList m_joinTeamRecver;		// 这个人收到了多少份邀请[队长]
	PendingList m_addTeamRecver;		// 这个人收到了多少份邀请[非队长]
	
	static int m_nTeamIdSeed;

};

#endif

