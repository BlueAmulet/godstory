#ifndef PLAYER_CACHE_H
#define PLAYER_CACHE_H

#include "PlayerOperator.h"

#include <hash_map>
#include <deque>
#include "base/Locker.h"


class CPlayerCache : private CPlayerOperator
{
public:
	CPlayerCache();
	virtual ~CPlayerCache();

	void showStatus();

	PlayerDataRef loadPlayer(  int playerId ); 
	PlayerDataRef loadPlayer( std::string playerName );

	int savePlayerData( PlayerDataRef pPlayerData );
	//int savePlayerData( stPlayerStruct* pPlayerData );

	//---------------------------------------------------------------
	// 刷新数据库缓冲区
	//---------------------------------------------------------------
	void flush();

	//---------------------------------------------------------------
	// 处理数据
	//---------------------------------------------------------------
	void processTick();
	
private:
	void updatePlayer( PlayerDataRef pPlayerData );

	static void WorkThread( void* pParam );
	void saveToDB( PlayerDataRef pPlayerData );
	typedef stdext::hash_map< int, PlayerDataRef > HashPlayerIdMap;
	typedef stdext::hash_map< std::string, PlayerDataRef > HashPlayerNameMap;

	typedef std::list< PlayerDataRef > PlayerDataList;
	typedef stdext::hash_map< int, int > PlayerIdSet;

	PlayerDataList		mPlayerDataList;
	PlayerDataList		mSavePlayerDataList;

	HashPlayerIdMap		mPlayerIdMap;
	HashPlayerNameMap	mPlayerNameMap;

	CMyCriticalSection  mSaveLock;
	CMyCriticalSection  mLoadLock;
	PlayerIdSet			mDeleteSet;

	typedef CPlayerOperator Parent;

	HANDLE				mThreadHandle;
	bool				mIsEnd;

	static const int	CacheSize = 128;

	int					mLastCacheCheckTime;
	int					mHitCount;
	int					mVisitCount;

	float				mHitPrecent;
};

#endif