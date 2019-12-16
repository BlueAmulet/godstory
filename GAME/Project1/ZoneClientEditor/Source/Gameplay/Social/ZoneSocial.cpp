#include <memory.h>
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/ServerGameplayState.h"
#include "NetWork/ServerPacketProcess.h"
#include "console/console.h"
#include "ZoneSocial.h"
#include "Gameplay/GameObjects/MPManager.h"

EventMethod gEventMethod[END_NET_MESSAGE];
CZoneSocial g_zoneSocial;

CZoneSocial::CZoneSocial()
{
	gEventMethod[ZONE_WORLD_MakeLinkResponse]		= &CZoneSocial::HandleMakeLinkResponse;
	gEventMethod[ZONE_WORLD_DestoryLinkResponse]	= &CZoneSocial::HandleDestoryLinkResponse;
	gEventMethod[ZONE_WORLD_QueryFriendValueResponse] = &CZoneSocial::HandleQueryFriendResponse;
	gEventMethod[WORLD_ZONE_SocialInfo]				= &CZoneSocial::HandleSocialInfo;
	gEventMethod[ZONE_WORLD_ChangeLinkResponse]		= &CZoneSocial::HandleChanageLink;
}

CZoneSocial::~CZoneSocial()
{
 
}

void CZoneSocial::makeLink( int playerId, int destPlayerId, SocialType::Type type )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_MakeLinkRequest, playerId, SERVICE_WORLDSERVER, destPlayerId );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ServerGameplayState->GetPacketProcess()->Send( sendPacket );
}

void CZoneSocial::destoryLink( int playerId, int destPlayerId )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_MakeLinkRequest, playerId, SERVICE_WORLDSERVER, destPlayerId );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ServerGameplayState->GetPacketProcess()->Send( sendPacket );
}

void CZoneSocial::changePlayerStatus( int playerId, int status )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_PlayerStatusChangeRequest, playerId, SERVICE_WORLDSERVER, playerId );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ServerGameplayState->GetPacketProcess()->Send( sendPacket );
}

void CZoneSocial::addFriendValue( int player1, int player2, int value )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_AddFriendValue, player1, SERVICE_WORLDSERVER, player2, value );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ServerGameplayState->GetPacketProcess()->Send( sendPacket );
}

void CZoneSocial::queryFriendValue( int player1, int player2, int action )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_QueryFriendValueResponse, player1, SERVICE_WORLDSERVER, player2, action );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ServerGameplayState->GetPacketProcess()->Send( sendPacket );
}

void CZoneSocial::HandleMakeLinkResponse( stPacketHead* pHead, Base::BitStream& recvPacket )
{
	// TODO: 处理加好友响应脚本
	int nErr = pHead->Id;
}

void CZoneSocial::HandleDestoryLinkResponse( stPacketHead* pHead, Base::BitStream& recvPacket )
{
	// TODO: 处理删好友响应脚本
	int nErr = pHead->Id;
}

void CZoneSocial::HandleQueryFriendResponse(stPacketHead* pHead, Base::BitStream& recvPacket)
{
	// TODO: 处理查询友好度

	int action = pHead->Id;
}

// 玩家卜卦
void CZoneSocial::Encourage( int playerId, int encourageId )
{
	
	// 向WorldServer发送奖励报告
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_EncourageNotify, playerId, SERVICE_WORLDSERVER, encourageId, g_ServerGameplayState->getZoneId() );
	pSendHead->PacketSize = sendPacket.getPosition()- sizeof( stPacketHead );
	ServerPacketProcess* pProecess = g_ServerGameplayState->GetPacketProcess();
	if( pProecess )
		pProecess->Send( sendPacket );
}

void CZoneSocial::HandleSocialInfo( stPacketHead* pHead, Base::BitStream& recvPacket )
{
	Player* pPlayer = g_ServerGameplayState->GetPlayer( pHead->Id );
	if( !pPlayer )
		return ;

	int length = recvPacket.readInt( 16 );
	pPlayer->mSocialCount = length;

	for( int i = 0; i < length; i++ )
	{
		pPlayer->mSocialItems[i].playerId = recvPacket.readInt( 32 );
		pPlayer->mSocialItems[i].type = (SocialType::Type) recvPacket.readInt( 8 );
		pPlayer->mSocialItems[i].friendValue = recvPacket.readInt( 16 );
	}
}

void CZoneSocial::HandleChanageLink( stPacketHead* pHead, Base::BitStream& recvPacket )
{
	int error = pHead->LineServerId;
	int playerId = pHead->Id;
	int destPlayerId = pHead->SrcZoneId;
	SocialType::Type oldType = (SocialType::Type) recvPacket.readInt( 8 );
	SocialType::Type newType = (SocialType::Type) recvPacket.readInt( 8 );

	Player* pPlayer = g_ServerGameplayState->GetPlayer(playerId);
	Player* destPlayer = g_ServerGameplayState->GetPlayer(destPlayerId);
	if(!pPlayer || !destPlayer)
		return;

	GameConnection* conn = pPlayer->getControllingClient();
	if(!conn)
		return;
//有错
//--------------------------------------------------------------------------
	if(error)
	{
		if(oldType == SocialType::Friend )
		{
			if(SocialType::Master)
			{
				MessageEvent::send(conn,SHOWTYPE_NOTIFY,avar("您向%s拜师失败",destPlayer->getPlayerName()),SHOWPOS_SCREEN);
				return;
			}
			if(SocialType::Prentice)
			{
				MessageEvent::send(conn,SHOWTYPE_NOTIFY,avar("您招%s收徒弟失败",destPlayer->getPlayerName()),SHOWPOS_SCREEN);
				return;
			}
		}

		return;
	}
//无错误
//-------------------------------------------------------------------------
	if(oldType == SocialType::Friend )
	{
		if(newType == SocialType::Master)
		{
			MessageEvent::send(conn,SHOWTYPE_NOTIFY,avar("%s已成为您师傅",destPlayer->getPlayerName()),SHOWPOS_SCREEN);
			if(pPlayer->getMPInfo())
				pPlayer->getMPInfo()->setPrenticeState(pPlayer,true);
			return;
		}

		if(newType == SocialType::Prentice)
		{
			MessageEvent::send(conn,SHOWTYPE_NOTIFY,avar("%s已成为您徒弟",destPlayer->getPlayerName()),SHOWPOS_SCREEN);
			if(pPlayer->getMPInfo())
			{
				U8 iNum =  pPlayer->getMPInfo()->getCurrPrenticeNum();
				pPlayer->getMPInfo()->setPrenticeNum(pPlayer,iNum+1);
			}
			return;
		}
	}

	if(newType == SocialType::Friend)
	{
		if(oldType == SocialType::Master)
		{
			MessageEvent::send(conn,SHOWTYPE_NOTIFY,avar("%s恭喜您成功出师",pPlayer->getPlayerName()),SHOWPOS_SCREEN);
			if(pPlayer->getMPInfo())
			{
				pPlayer->getMPInfo()->setPrenticeState(pPlayer,false);
				pPlayer->getMPInfo()->setFarPrentice(pPlayer,true);
			}
			return;
		}
		if (oldType == SocialType::Prentice)
		{
			MessageEvent::send(conn,SHOWTYPE_NOTIFY,avar("%s您徒弟成功出师",destPlayer->getPlayerName()),SHOWPOS_SCREEN);
			if(pPlayer->getMPInfo())
			{
				U8 iNum =  pPlayer->getMPInfo()->getCurrPrenticeNum();
				pPlayer->getMPInfo()->setPrenticeNum(pPlayer,iNum - 1);
			}
			return;
		}
	}
}

void CZoneSocial::chanageLink( int playerId, int destPlayerId, SocialType::Type oldType, SocialType::Type newType )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );
	stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_ChangeLinkRequest, playerId, SERVICE_WORLDSERVER, g_ServerGameplayState->getZoneId() );
	sendPacket.writeInt( destPlayerId, 32 );
	sendPacket.writeInt( oldType, 8 );
	sendPacket.writeInt( newType, 8 );
	pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	ServerPacketProcess* pProcess = g_ServerGameplayState->GetPacketProcess();
	if( pProcess )
		pProcess->Send( sendPacket );
}

void CZoneSocial::HandleChearNotify( stPacketHead* pHead, Base::BitStream& recvPacket )
{

}


ConsoleFunction( SptSocial_Encourage, void, 2, 2, "" )
{
	Player* pPlayer = g_ServerGameplayState->GetPlayer( atoi( argv[1] ) );
	if( !pPlayer )
		return ;

	/*std::string ret = Con::evaluatef( "bugua(%d);", pPlayer->getId() );
	int nRet = atoi( ret );*/
	int nRet = 101010000;
	Con::evaluatef( "%d.AddTimerControl(999999, 10000, 1000, 1, 1, 1);", pPlayer->getId() );
	Con::evaluatef( "%d.SetTimerControl(999999, 1, \"%d\");", pPlayer->getId(), nRet );

	// 玩家卜卦成功
	if( nRet > 0 )
	{
		g_zoneSocial.Encourage( atoi(argv[1]), nRet );
	}
	else
	{
		
	}
}

