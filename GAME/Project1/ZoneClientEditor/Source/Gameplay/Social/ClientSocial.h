#ifndef CLIENT_SOCIAL_H
#define CLIENT_SOCIAL_H

#include "Base/bitStream.h"
#include "Common/PacketType.h"
#include "Common/CommonPacket.h"

class CClientSocial
{
public:
	CClientSocial();
	virtual ~CClientSocial();

	void cleanup();

	void makeFriend( int playerId, int destPlayerId, bool isTemp = true );
	void destoryFriend( int playerId, int destPlayerId );

	void HandleClientWorldMakeLinkResponse( stPacketHead* pHead, Base::BitStream& packet );
	void HandleClientWorldDestoryLinkResponse( stPacketHead* pHead, Base::BitStream& packet );

	void chearFriend( int playerId, int destPlayerId );
	void sendSocialRequest( int playerId );
	void sendPlayerInfoRequest( int playerId );

	void sendNoAddedRequest( int playerId, int isOpen );

	void findPlayerById( int playerId, int destPlayerId );
	void findPlayerByName( int playerId, const char* playerName );

	void HandleClientWorldChearResponse( stPacketHead* pHead, Base::BitStream& packet );
	void HandleClientWorldSocialListResponse( stPacketHead* pHead, Base::BitStream& packet );
	void HandleWorldClientChearNotify( stPacketHead* pHead, Base::BitStream& packet );

	void HandleWorldClientSocialAdded( stPacketHead* pHead, Base::BitStream& packet );
	void HandleWorldClientSocialRemoved( stPacketHead* pHead, Base::BitStream& packet );
	void HandleClientWorldFindPlayerResponse( stPacketHead* pHead, Base::BitStream& packet );
	void init();
	void HandleClientWorldQueryFriendResponse( stPacketHead* pHead, Base::BitStream& packet );
	void sendQueryFriend( int playerId, int destPlayerId );
	void HandlePlayerStatusChange( stPacketHead* pHead, Base::BitStream& packet );
	void HandleClientWorldQueryPlayerHotResponse(stPacketHead* pHead, Base::BitStream& packet);
	void HandleClientWorldQueryChearChoutResponse(stPacketHead* pHead, Base::BitStream& packet);
	void sendQueryPlayerHotRequest( int playerId );
	void sendQueryChearCountRequest( int playerId );
	void HandleWorldClientEncourageNotify(stPacketHead* pHead, Base::BitStream& packet);
	void HandleClientWorldPlayerInfoResponse(stPacketHead* pHead, Base::BitStream& packet);
	typedef stdext::hash_map< int, stSocialInfo > SocialMap;

	inline SocialMap&          getClientSocialInfo()           { return mSocialMap;}
	inline U8                  getQueryPrenticeNum()           { return mQueryPrenticeNum;}
	inline void                clearQueryPrenticeNum()        { mQueryPrenticeNum = 0;}

private:
	SocialMap mSocialMap;
	U8 mQueryPrenticeNum;         // 师徒系统查询次数
};

extern CClientSocial g_clientSocial;

#define CLIENT_SOCIAL (&g_clientSocial)

#ifdef _MSG
//class AddSocialEvent : public GameNetEvent
//{
//	typedef GameNetEvent Parent;
//
//	bool   mIsSameMap;	//是否内传送
//public:
//	TransportBeginEvent()				{ }
//	void setParam(bool isSameMap)	{ mIsSameMap = isSameMap;}
//
//	void pack(NetConnection *pCon, BitStream *bstream);
//	void unpack(NetConnection *pCon, BitStream *bstream);
//	void process(NetConnection *pCon);
//
//	DECLARE_CONOBJECT(TransportBeginEvent);
//};
#endif

#endif