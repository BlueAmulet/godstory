#ifndef SAVE_PLAYER_JOB_H
#define SAVE_PLAYER_JOB_H

#include "Common/TimerMgr.h"
#include "Common/PlayerStruct.h"
#include "base/Locker.h"
#include <hash_map>
#include <list>
#include "LockCommon.h"

class CSavePlayerJob
	: public CTimeCriticalObject
{
public:
	bool TimeProcess( bool bExit );

	bool Update();
	void PostPlayerData( unsigned long UID, stPlayerStruct* pPlayerData );

	void SetDataAgentSocketHandle( int nPlayerId, int nSocket );
	CMyCriticalSection m_cs;

private:
	stdext::hash_map< int, stPlayerStruct > m_playerMap;	// PlayerIdÎª¼üÖµ
	stdext::hash_map< int, unsigned long> m_playerUIDMap;	

	std::list<int> m_playerList;

	stdext::hash_map<int, int> m_dataAgentMap;

};


#endif