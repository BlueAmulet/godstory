#ifndef SOCIAL_MANAGER_H
#define SOCIAL_MANAGER_H

#include <hash_map>
#include <string>
#include <deque>

#include "Common/SocialBase.h"
#include "base/Locker.h"
#include "Common/PlayerStruct.h"
#include "LockCommon.h"
#include "ManagerBase.h"
#include "AccountHandler.h"

class CSocialManager : public ILockable, public CManagerBase< CSocialManager, MSG_SOCIAL_BEGIN, MSG_SOCIAL_END >
{
public:
	typedef stdext::hash_map< int, SocialType::Type > LinkMap;
	typedef stdext::hash_map< int, LinkMap > LinkManMap;

	typedef stdext::hash_map< int , PlayerStatusType::Type > StatusMap;

	typedef stdext::hash_map< int, bool > NoAddedMap;

	typedef stdext::hash_map< int, int > LotLockMap;


	typedef stdext::hash_map< int, stdext::hash_map< int, int > >					ChearMap;

	typedef std::deque< int > PlayerList;

public:
	CSocialManager();
	virtual ~CSocialManager();

	// 将玩家的社会信息打包
	void packSocialInfoList( Base::BitStream& packet, int playerId );

	//void _packSingleSocialInfo( stSocialInfo &info, PlayerDataRef pPlayerData, int i, int &friendPlayer, bool &isOffline, PlayerDataRef &pFriendPlayer, int friendValue, Base::BitStream& packet );
	// 改变玩家当前状态
	void changePlayerStatus( int playerId, PlayerStatusType::Type status );

	// 通知Zone玩家好友信息
	void notifyZonePlayerSocialInfo( int playerId );

	// 通知玩家有好友状态改变
	void notifyPlayerStatusChange( int playerId, int destPlayerId, PlayerStatusType::Type status );

	// 当载入玩家信息
	void onPlayerEnter( int playerId );

	// 当玩家退出
	void onPlayerQuit( int playerId );

	// make a link
	int makeLink( int playerId1, int playerId2, SocialType::Type type );

	// destory a link
	int destoryLink( int playerId1, int playerId2, bool isClientDestory = false );

	// 增加好友度
	void addFirendValue( int player1, int player2, int value );

	// 查询好友度
	int queryFriendValue( int player1, int player2 );

	// 检查好友
	bool checkFriend( int player1, int player2 );

	// 加油
	int chear( int playerSrc, int playerDest );

	/*
	命运有缘人
	要从全部在线角色中挑选一部分合适的玩家,然后随机选择其中的一位跟特定玩家配对
	重点:	(1)如何确定玩家在线
			(2)如何确定合适的玩家
			(3)如何随机
	*/

	// save to db
	void saveDb();

	// read from db
	void readDb();

	DECLARE_EVENT_FUNCTION( HandleClientWorldSocialInfoRequest );
	DECLARE_EVENT_FUNCTION( HandleZoneWorldPlayerStatusChangeRequest );

	DECLARE_EVENT_FUNCTION( HandleZoneWorldAddFriendValue );

	DECLARE_EVENT_FUNCTION( HandleClientWorldMakeRequest );
	DECLARE_EVENT_FUNCTION( HandleClientWorldDestoryRequest );
	DECLARE_EVENT_FUNCTION( HandleZoneWorldMakeRequest );
	DECLARE_EVENT_FUNCTION( HandleZoneWorldDestoryRequest );
	DECLARE_EVENT_FUNCTION( HandleZoneWorldChangeLinkReuquest );
	DECLARE_EVENT_FUNCTION( HandleClientWorldChearRequest );
	DECLARE_EVENT_FUNCTION( HandleClientWorldFindPlayerRequest );
	DECLARE_EVENT_FUNCTION( HandleClientWorldNoAdded );
	DECLARE_EVENT_FUNCTION( HandleClientWorldFindFriendRequest );
	DECLARE_EVENT_FUNCTION( HandleQueryPlayerHot );
	DECLARE_EVENT_FUNCTION( HandleQueryChearCount );
	DECLARE_EVENT_FUNCTION( HandleZoneWorldEncourageNotify );
	DECLARE_EVENT_FUNCTION( HandleZoneWorldLotRequest );
	DECLARE_EVENT_FUNCTION( HandleZoneWorldAddLotRequest );
	DECLARE_EVENT_FUNCTION( HandleZoneWorldRemoveLotRequest );
	DECLARE_EVENT_FUNCTION( HandleClientWorldPlayerInfoRequest );
	

	bool packSingleSocialInfo( stSocialInfo &info, PlayerDataRef pPlayerData, char type, int friendPlayer, int friendValue, Base::BitStream& packet );
	void packPlayerSocialInfoList( Base::BitStream& sendPakcet, int playerId, PlayerDataRef pPlayerData );
	char queryFriendType( int player1, int player2 );
	void removePlayerFriendTemp( PlayerDataRef pPlayer );
private:


private:
	LinkManMap	mLinkManMap;
	StatusMap	mStatusMap;

	ChearMap	mChearMap;			// 上次加油时间
	LotLockMap	mLotLockMap;
	
	NoAddedMap	mNoAddedMap;

	PlayerList	mMaleList;
	PlayerList  mFemaleList;

	static const int MaxChearCount = 10;
	static const int MaxChearedCount = 10;

	static const int MAX_CACHE_COUNT = 1024;
} ;



#endif /* SOCIAL_MANAGER_H */