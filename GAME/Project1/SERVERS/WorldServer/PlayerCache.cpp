#include "PlayerCache.h"
#include "AccountHandler.h"
#include <process.h>
#include "Base\Log.h"
#include "WorldServer.h"
#include "ExportScript.h"
#include "PlayerMgr.h"

CPlayerCache::~CPlayerCache()
{
	// ˢ�����ݻ���
	flush();

	mIsEnd = true;

	::WaitForSingleObject( mThreadHandle, -1 );
}

CPlayerCache::CPlayerCache()
{
	mIsEnd = false;

	mLastCacheCheckTime = 0;
	mHitPrecent = 0.0f;

	// ��ʼһ����������������������
	_beginthread( WorkThread, 0, this );
}

void CPlayerCache::flush()
{
	CLocker l( mLoadLock );

	PlayerDataList::iterator it;
	for( it = mPlayerDataList.begin(); it != mPlayerDataList.end(); it++ )
	{
		CLocker lock( mSaveLock );
		mSavePlayerDataList.push_back( *it );
		mDeleteSet[(*it)->BaseData.PlayerId]++;
	}
}

PlayerDataRef CPlayerCache::loadPlayer( int playerId )
{
	CLocker lock( mLoadLock );

	if( GetTickCount() - mLastCacheCheckTime > 1000 )
	{
		mHitPrecent = float( mHitCount ) / mVisitCount;
		mHitCount = 0;
		mVisitCount = 0;
		mLastCacheCheckTime = GetTickCount();
	}

	mVisitCount++;

	PlayerDataRef player;

	HashPlayerIdMap::iterator it = mPlayerIdMap.find( playerId );
	if( it != mPlayerIdMap.end() )
	{
		mHitCount++;
		player = it->second;
	}
	else
	{
		player = Parent::loadPlayer( playerId );
		updatePlayer( player );
	}

	return player;
}

PlayerDataRef CPlayerCache::loadPlayer( std::string playerName )
{
	CLocker lock( mLoadLock );

	if( GetTickCount() - mLastCacheCheckTime > 1000 )
	{
		mHitPrecent = float( mHitCount ) / mVisitCount;
		mHitCount = 0;
		mVisitCount = 0;
		mLastCacheCheckTime = GetTickCount();
	}

	mVisitCount++;

	PlayerDataRef player;

	HashPlayerNameMap::iterator it = mPlayerNameMap.find( playerName );
	if( it != mPlayerNameMap.end() )
	{
		mHitCount++;
		player = it->second;
	}
	else
	{
		player = Parent::loadPlayer( playerName );
		updatePlayer( player );
	}

	return player;
}

int CPlayerCache::savePlayerData( PlayerDataRef pPlayerData )
{
	if( pPlayerData.isNull() )
		return false;

	saveToDB( pPlayerData );

	return true;
}

void CPlayerCache::saveToDB( PlayerDataRef pPlayerData )
{
	CLocker lock( mSaveLock );

	mSavePlayerDataList.push_back( pPlayerData );
}

void CPlayerCache::WorkThread( void* pParam )
{
	CPlayerCache* pThis = static_cast< CPlayerCache* >( pParam );

	while( 1 )
	{
		pThis->mLoadLock.Lock();

		pThis->mSaveLock.Lock();

		if( pThis->mSavePlayerDataList.size() > 0 )
		{
			PlayerDataRef pPlayerData = *( pThis->mSavePlayerDataList.begin() );

			if( !pPlayerData.isNull() )
			{
				int PlayerId = pPlayerData->BaseData.PlayerId;
				{
					// ������Ƕ�����Ҫɾ����
					if( pThis->mDeleteSet[PlayerId] > 0 )
					{
						pThis->mPlayerIdMap.erase( PlayerId );
						pThis->mPlayerNameMap.erase( pPlayerData->BaseData.PlayerName );
						pThis->mDeleteSet.erase( PlayerId );
					}
				}

				pThis->mSavePlayerDataList.erase( pThis->mSavePlayerDataList.begin() );

				pThis->mSaveLock.Unlock();

				pThis->mLoadLock.Unlock();

				// ���浽���ݿ���
				pThis->savePlayer( pPlayerData );

			}
			else
			{
				pThis->mSavePlayerDataList.erase( pThis->mSavePlayerDataList.begin() );

				pThis->mSaveLock.Unlock();

				pThis->mLoadLock.Unlock();
			}
		}
		else
		{
			pThis->mSaveLock.Unlock();

			pThis->mLoadLock.Unlock();

			if( pThis->mIsEnd )
				break;

			Sleep( 1000 );
		}
	}
}

void CPlayerCache::processTick()
{
	
}

//---------------------------------------------------------------
// ���ڸ������
//---------------------------------------------------------------
void CPlayerCache::updatePlayer( PlayerDataRef pPlayerData )
{
	CLocker lock( mSaveLock );

	if( pPlayerData.isNull() )
		return ;

	int playerId = pPlayerData->BaseData.PlayerId;

	PlayerDataList::iterator it;

	// ��ѯ��������Ƿ������ݻ����У�ԽƵ�����ʵ����ݲ�ѯʱ��Խ�̣�
	for( it=mPlayerDataList.begin(); it != mPlayerDataList.end(); it++ )
	{
		if( ( *it )->BaseData.PlayerId == playerId )
			break;
	}

	// û�з��ָ������ڻ�����
	if( it == mPlayerDataList.end() )
	{
		// ��������
		if( mPlayerDataList.size() > CacheSize )
		{
			PlayerDataRef pData = *mPlayerDataList.rbegin();

			if( pData.isNull() )
				return ;


			{
				CLocker lock( mSaveLock );
				
				mSavePlayerDataList.push_back( pData );

				mDeleteSet[pData->BaseData.PlayerId]++;
			}

			mPlayerIdMap.erase( pData->BaseData.PlayerId );
			mPlayerNameMap.erase( pData->BaseData.PlayerName );

			mPlayerDataList.pop_back();
		}
		else
		{
			mPlayerDataList.push_front( pPlayerData );

			mPlayerIdMap[pPlayerData->BaseData.PlayerId] = pPlayerData;
			mPlayerNameMap[pPlayerData->BaseData.PlayerName] = pPlayerData;
		}
	}
	else
	{
		// ����������ƶ�������ͷ

		mPlayerDataList.erase( it );

		mPlayerDataList.push_front( pPlayerData );
	}
}

void CPlayerCache::showStatus()
{
	CLocker l( mLoadLock );
	CLocker m( mSaveLock );

	printf( "Cache size: %d\n", mPlayerDataList.size() );
	printf( "Save queue size: %d\n", mSavePlayerDataList.size() );
	printf( "Cache hit precent: %.2f%%\n", mHitPrecent * 100.0f );
}

CmdFunction( ShowCacheStatus )
{
	SERVER->GetPlayerManager()->showStatus();
}