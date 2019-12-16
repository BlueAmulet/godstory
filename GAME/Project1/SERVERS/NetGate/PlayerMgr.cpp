
#include "wintcp/dtServerSocket.h"
#include "PlayerMgr.h"
#include "NetGate.h"

CPlayerManager::CPlayerManager()
{

}

CPlayerManager::~CPlayerManager()
{

}

bool CPlayerManager::AddSocketMap(int SocketId,const char *Ip)
{
	CLocker lock(m_cs);

	HashSocketMap::iterator it = m_SocketMap.find(SocketId);
	if(it == m_SocketMap.end())
	{
		stSocketInfo Sinfo;
		Sinfo.PlayerId			= 0;
		Sinfo.LoginIP			= inet_addr(Ip);
		m_SocketMap[SocketId]	= Sinfo;
		return true;
	}

	return false;
}

bool CPlayerManager::ReleaseSocketMap(int SocketId,bool postEvent)
{
	CLocker lock(m_cs);

	HashSocketMap::iterator sit = m_SocketMap.find(SocketId);
	if(sit == m_SocketMap.end())
		return true;

	stSocketInfo *psInfo = &sit->second;
	
	if(psInfo->PlayerId)
	{
		HashPlayerMap::iterator pit = m_PlayerMap.find(psInfo->PlayerId);
		if(pit != m_PlayerMap.end())
		{
			m_PlayerMap.erase(pit);
		}
	}	

	m_SocketMap.erase(sit);

	if(postEvent)
		SERVER->GetServerSocket()->PostEvent(dtServerSocket::OP_CODE::OP_RESTART,SocketId);

	return false;
}

stSocketInfo *CPlayerManager::GetSocketMap(int SocketId)
{
	HashSocketMap::iterator it = m_SocketMap.find(SocketId);
	if(it == m_SocketMap.end())
		return NULL;

	return &it->second;
}

#include <assert.h>

int CPlayerManager::AddPlayerMap(int SocketId,int UID,int PlayerId)
{
	CLocker lock(m_cs);

	AddSocketMap(SocketId,"0.0.0.0");

	HashSocketMap::iterator sit = m_SocketMap.find(SocketId);
	if(sit == m_SocketMap.end())
	{
		//assert(0);
		return UNKNOW_ERROR;
	}

	stSocketInfo *psInfo = &sit->second;
	if(psInfo->PlayerId && psInfo->PlayerId != PlayerId )
		return PLAYER_ERROR;

	psInfo->PlayerId = PlayerId;

	HashPlayerMap::iterator pit = m_PlayerMap.find(PlayerId);
	if( pit != m_PlayerMap.end() )
	{
		if(pit->second.UID < UID)
		{
			ReleaseSocketMap( pit->second.socketId, true );

			stSocketInfo* pSockInfo = &m_SocketMap[SocketId];
			pSockInfo->PlayerId = PlayerId;
			pSockInfo->UID = UID;
		}
		else if( pit->second.UID > UID )
		{
			ReleaseSocketMap( pit->second.socketId, true );

			return GAME_UID_ERROR;
		}
	}

	{
		stPlayerInfo pinfo;
		pinfo.LastTime = (int)time(NULL);
		pinfo.socketId = SocketId;
		pinfo.UID	   = UID;
		m_PlayerMap[PlayerId] = pinfo;
	}
	
	return NONE_ERROR;
}

bool CPlayerManager::ReleasePlayerMap(int PlayerId)
{
	CLocker lock(m_cs);

	HashPlayerMap::iterator pit = m_PlayerMap.find(PlayerId);
	if(pit == m_PlayerMap.end())
		return true;
	
	stPlayerInfo *ppInfo = &pit->second;
	m_SocketMap.erase(ppInfo->socketId);
	if(ppInfo->socketId)
		SERVER->GetServerSocket()->PostEvent(dtServerSocket::OP_CODE::OP_RESTART,ppInfo->socketId);
	m_PlayerMap.erase(pit);
	return true;
}

int CPlayerManager::GetSocketHandle(int PlayerId)
{
	CLocker lock(m_cs);

	HashPlayerMap::iterator pit = m_PlayerMap.find(PlayerId);
	if(pit != m_PlayerMap.end())
		return pit->second.socketId;

	return 0;
}

stPlayerInfo *CPlayerManager::GetPlayerMap(int PlayerId)
{
	HashPlayerMap::iterator pit = m_PlayerMap.find(PlayerId);
	if(pit != m_PlayerMap.end())
		return &pit->second;

	return NULL;
}

void CPlayerManager::GetAllPlayers( std::list<stPlayerInfo*>& playerInfoList )
{
	CLocker lock(m_cs);

	HashPlayerMap::iterator it = m_PlayerMap.begin();
	for( ; it != m_PlayerMap.end(); it++ )
	{
		playerInfoList.push_back( &(it->second) );
	}
}

stdext::hash_map<int,stPlayerInfo>* CPlayerManager::GetAllPlayers()
{
	return &m_PlayerMap;
}

bool CPlayerManager::TimeProcess(bool bExit)
{
	//扫描所有的管理对象，超时的被认为是下线删除
	return false;
}

int CPlayerManager::GetPlayerIdBySocket( int nSocket,T_UID& UID)
{
	HashSocketMap::iterator it;
	it = m_SocketMap.find( nSocket );

	if( it != m_SocketMap.end() )
    {
        UID = it->second.UID;
		return it->second.PlayerId;
    }

	return NULL;
}

