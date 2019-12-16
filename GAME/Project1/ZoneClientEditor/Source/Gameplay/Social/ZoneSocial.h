#ifndef ZONE_SOCIAL_H
#define ZONE_SOCIAL_H

#include "Common/SocialBase.h"
#include "NetWork/EventCommon.h"

class CZoneSocial
{
public:
	CZoneSocial();
	virtual ~CZoneSocial();

	void makeLink( int playerId, int destPlayerId, SocialType::Type type );
	void destoryLink( int playerId, int destPlayerId );
	void chanageLink( int playerId, int destPlayerId, SocialType::Type oldType, SocialType::Type newType );
	void changePlayerStatus( int playerId, int status );

	void HandleChearNotify(  stPacketHead* pHead, Base::BitStream& recvPacket  );
	void HandleMakeLinkResponse( stPacketHead* pHead, Base::BitStream& recvPacket );
	void HandleDestoryLinkResponse( stPacketHead* pHead, Base::BitStream& recvPacket );
	void addFriendValue( int player1, int player2, int value );
	void HandleQueryFriendResponse(stPacketHead* pHead, Base::BitStream& recvPacket);
	void HandleSocialInfo(stPacketHead* pHead, Base::BitStream& recvPacket);
	void HandleChanageLink(stPacketHead* pHead, Base::BitStream& recvPacket);
	void queryFriendValue( int player1, int player2, int action );
	void Encourage( int playerId, int encourageId );
};

extern CZoneSocial g_zoneSocial;

#endif