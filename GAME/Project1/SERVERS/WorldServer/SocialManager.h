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

	// ����ҵ������Ϣ���
	void packSocialInfoList( Base::BitStream& packet, int playerId );

	//void _packSingleSocialInfo( stSocialInfo &info, PlayerDataRef pPlayerData, int i, int &friendPlayer, bool &isOffline, PlayerDataRef &pFriendPlayer, int friendValue, Base::BitStream& packet );
	// �ı���ҵ�ǰ״̬
	void changePlayerStatus( int playerId, PlayerStatusType::Type status );

	// ֪ͨZone��Һ�����Ϣ
	void notifyZonePlayerSocialInfo( int playerId );

	// ֪ͨ����к���״̬�ı�
	void notifyPlayerStatusChange( int playerId, int destPlayerId, PlayerStatusType::Type status );

	// �����������Ϣ
	void onPlayerEnter( int playerId );

	// ������˳�
	void onPlayerQuit( int playerId );

	// make a link
	int makeLink( int playerId1, int playerId2, SocialType::Type type );

	// destory a link
	int destoryLink( int playerId1, int playerId2, bool isClientDestory = false );

	// ���Ӻ��Ѷ�
	void addFirendValue( int player1, int player2, int value );

	// ��ѯ���Ѷ�
	int queryFriendValue( int player1, int player2 );

	// ������
	bool checkFriend( int player1, int player2 );

	// ����
	int chear( int playerSrc, int playerDest );

	/*
	������Ե��
	Ҫ��ȫ�����߽�ɫ����ѡһ���ֺ��ʵ����,Ȼ�����ѡ�����е�һλ���ض�������
	�ص�:	(1)���ȷ���������
			(2)���ȷ�����ʵ����
			(3)������
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

	ChearMap	mChearMap;			// �ϴμ���ʱ��
	LotLockMap	mLotLockMap;
	
	NoAddedMap	mNoAddedMap;

	PlayerList	mMaleList;
	PlayerList  mFemaleList;

	static const int MaxChearCount = 10;
	static const int MaxChearedCount = 10;

	static const int MAX_CACHE_COUNT = 1024;
} ;



#endif /* SOCIAL_MANAGER_H */