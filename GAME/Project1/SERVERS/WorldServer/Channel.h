#ifndef CHAT_CHANNEL_H
#define CHAT_CHANNEL_H

#include <string>
#include <sstream>
#include <list>
#include <hash_map>
#include <hash_set>

#include "base/Locker.h"
#include "Common/ChatBase.h"
#include "PlayerGroup.h"
#include "Event.h"

#include "LockCommon.h"

class CChannel
{
public:
	friend class CChannelManager;

	void GetPlayerList( std::list<int>& playerList );

	UINT GetId();
	const char* GetName();
	char GetMessageType();
	bool HasPlayer( int nPlayreId );
private:
	CChannel( UINT nChannelID, char cMessageType, std::string strChannelName );
	virtual ~CChannel();

	void AddPlayer( int nPlayerId );
	void RemovePlayer( int nPlayerId );


	unsigned int	m_nChannelID;			// ID
	char			m_cMessageType;			// 消息类型
	string			m_strChannelName;		// 名称

	stdext::hash_set<int>	m_playerList;
} ;

class CChannelManager : public CEventListener, public ILockable
{
public:
	CChannelManager();
	virtual ~CChannelManager();

	CChannel* RegisterChannel( char cMessageType, std::string strChannelName );
	void UnregisterChannel( UINT nChannelID );

	CChannel* GetChannel( UINT nChannelID );
	CChannel* GetChannel( int nPlayerId, char cMessageType );

	void AddPlayer( int nPlayerId, int nChannelId, bool bLink = true );
	void RemovePlayer( int nPlayerId, int nChannelId, bool bClearLink = true );

	static UINT BuildChannelID();

	void RemoveAllChannel();
	void ClearAccount( int nAccountId );		// 清理帐号的频道数据 

	void OnEvent( CEventSource* pEventSouce, const stEventArg& eventArg );

private:
	stdext::hash_map<int, CChannel*> m_hmChannelMap;
	stdext::hash_map<__int64, int> m_hmPlayerChannelMap;
	static UINT m_nChannelIDSeed;
};

#endif