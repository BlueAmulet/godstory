#ifndef PLAYER_MGR_H
#define PLAYER_MGR_H

#include "DataAgentServer.h"
#include "base/locker.h"
#include "Common/IdGenerator.h"
#include "Common/PlayerStruct.h"
#include "Common/BackWorker.h"
#include "Common/ServerFramwork.h"

#include <hash_map>

class CPlayerManager : public IServerModule< CDataAgent, MSG_DATAAGENT_BEGIN, MSG_DATAAGENT_END>
{
public:
	DECLARE_SERVER_MODULE_MESSAGE();

	CPlayerManager();
	virtual ~CPlayerManager();

	void savePlayerData( stPlayerStruct* pPlayerData );
	void removePlayerData( int PlayerId, int SID );

	void doCheckData();

	void sendPlayerData( int playerId );

private:

	struct stPlayerRecData
	{
		stPlayerStruct	mPlayerData;
		unsigned long	mLastSendTime;
		unsigned long	mSendCount;

		stPlayerRecData()
		{
			mLastSendTime = 0;
			mSendCount = 0;
		}
	};

	void _sendPlayerData( int nPlayerID );

	typedef stdext::hash_map<int,stPlayerRecData>	PlayerDataMap;

	PlayerDataMap	mPlayerDataMap;		// UID --> PlayerData

private:
	CBackWorker* mBackWorker;
	CIdGenerator mIdGen;
	CMyCriticalSection m_cs;
};

#endif