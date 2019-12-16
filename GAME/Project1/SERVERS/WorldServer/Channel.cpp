#include "Base/Locker.h"
#include "Channel.h"
#include "PlayerMgr.h"
#include "ChatMgr.h"
#include "TeamManager.h"
#include "WorldServer.h"

CChannel::CChannel( UINT nChannelID, char cMessageType, std::string strChannelName )
{
	m_nChannelID = nChannelID;
	m_cMessageType = cMessageType;
	m_strChannelName = strChannelName;
}

CChannel::~CChannel()
{

}

UINT CChannel::GetId()
{
	return m_nChannelID;
}

const char* CChannel::GetName()
{
	return m_strChannelName.c_str();
}

char CChannel::GetMessageType()
{
	return m_cMessageType;
}

void CChannel::AddPlayer( int nPlayerId )
{
	m_playerList.insert( nPlayerId );
}

void CChannel::RemovePlayer( int nPlayerId )
{
	m_playerList.erase( nPlayerId );
}

void CChannel::GetPlayerList( std::list<int>& playerList )
{
	stdext::hash_set<int>::iterator it;

	for( it = m_playerList.begin(); it != m_playerList.end(); it++ )
	{
		playerList.push_back( *it );
	}
}

UINT CChannelManager::m_nChannelIDSeed = 1;

CChannelManager::CChannelManager()
{
	gLock.registerLockable( this, Lock::Channel );
}

CChannelManager::~CChannelManager()
{
	RemoveAllChannel();
}

CChannel* CChannelManager::RegisterChannel( char cMessageType, std::string strChannelName )
{
	// 大规模消息不能创建频道
	if( cMessageType < CHAT_MSG_TYPE_RACE )
		return NULL;

	DO_LOCK( Lock::Channel );

	UINT nId = BuildChannelID();
	CChannel* pChannel = new CChannel( nId, cMessageType, strChannelName );

	m_hmChannelMap[ nId ] = pChannel;
	return pChannel;
}

void CChannelManager::UnregisterChannel( UINT nChannelID )
{
	DO_LOCK( Lock::Channel );

	stdext::hash_map<int, CChannel*>::iterator it;

	it = m_hmChannelMap.find( nChannelID );
	if( it == m_hmChannelMap.end() )
		return ;

	delete it->second;
	m_hmChannelMap.erase( it );
}

void CChannelManager::RemoveAllChannel()
{
	DO_LOCK( Lock::Channel );

	stdext::hash_map<int, CChannel*>::iterator it;

	for( it = m_hmChannelMap.begin(); it != m_hmChannelMap.end(); it++ )
	{
		delete it->second;
	}

	m_hmChannelMap.clear();
	m_hmPlayerChannelMap.clear();
}

UINT CChannelManager::BuildChannelID()
{
	return m_nChannelIDSeed++;
}

CChannel* CChannelManager::GetChannel( UINT nChannelID )
{
	return m_hmChannelMap[nChannelID];
}

CChannel* CChannelManager::GetChannel( int nPlayerId, char cMessageType )
{
	// 对于大规模消息来说，没有意义
	if( cMessageType < CHAT_MSG_TYPE_RACE )
		return NULL;

	__int64 v = nPlayerId;
	v = ( v << 8 ) | cMessageType;
	return GetChannel( m_hmPlayerChannelMap[v] );
}

void CChannelManager::AddPlayer( int nPlayerId, int nChannelId, bool bLink )
{
	DO_LOCK( Lock::Channel );

	CChannel* pChannel = GetChannel( nChannelId );
	if( !pChannel )
		return ;

	pChannel->AddPlayer( nPlayerId );

	if( !bLink )
		return ;


	__int64 v = nPlayerId;
	v = ( v << 8 ) | pChannel->GetMessageType();
	m_hmPlayerChannelMap[v] = pChannel->GetId();
}


// bClearLink 为一个标志， 此标志不清除可以方便玩家快速加回原来的频道
void CChannelManager::RemovePlayer( int nPlayerId, int nChannelId, bool bClearLink )
{
	DO_LOCK( Lock::Channel );

	CChannel* pChannel = GetChannel( nChannelId );
	if( !pChannel )
		return ;

	pChannel->RemovePlayer( nPlayerId );

	if( !bClearLink )
		return ;


	__int64 v = nPlayerId;
	v = ( v << 8 ) | pChannel->GetMessageType();
	m_hmPlayerChannelMap.erase( v );
}

bool CChannel::HasPlayer( int nPlayreId )
{
	return m_playerList.find( nPlayreId ) != m_playerList.end();
}


void CChannelManager::OnEvent( CEventSource* pEventSouce, const stEventArg& eventArg )
{
	// 处理玩家离线，要清空该玩家所拥有的所有数据
	// 暂不处理该事件

}

void CChannelManager::ClearAccount( int nAccountId )
{
	DO_LOCK( Lock::Channel | Lock::Player );

	stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountMap( nAccountId );
	
	if( !pAccountInfo )
    {
        return ;
    }

    CChannel* pChannel = NULL;
	int nPlayerId = 0;
	
	for( int id = 0; id < pAccountInfo->PlayerNum; id ++ )
	{
		for( int i = 0; i < CHAT_MSG_TYPE_UNKNOWN; i++ )
		{
			nPlayerId = pAccountInfo->PlayerIdList[id];
			pChannel = GetChannel( nPlayerId, i );
			if( pChannel )
				RemovePlayer( nPlayerId, pChannel->GetId() );
		}
	}
}
