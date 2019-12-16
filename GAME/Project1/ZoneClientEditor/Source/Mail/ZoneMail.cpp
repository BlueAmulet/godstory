#include "Gameplay/ServerGameplayState.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "common/PacketType.h"
#include "Network/ServerPacketProcess.h"
#include "ZoneMail.h"

void CZoneMail::RecvMail( int nRecver, int nPage, int nCount )
{
	Player* pPlayer = g_ServerGameplayState->GetPlayer( nRecver );
	if( !pPlayer )
		return ;

	stMailHeader mailHeader;
	mailHeader.nCount = nCount;
	mailHeader.nRecver = nRecver;
	mailHeader.nPage = nPage;
	
	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );

	stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_MailListRequest, nRecver, SERVICE_WORLDSERVER, 0, g_ServerGameplayState->getZoneId() );

	mailHeader.PackData( sendPacket );

	pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ServerGameplayState->GetPacketProcess()->Send( sendPacket );
}

void CZoneMail::ReadMail( int nMailId )
{
	char buf[100];
	Base::BitStream sendPacket( buf, 100 );
	stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_MailReaded, nMailId, SERVICE_WORLDSERVER );

	pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	g_ServerGameplayState->GetPacketProcess()->Send( sendPacket );
}

void CZoneMail::DeleteMail( int nMailId )
{
	char buf[100];
	Base::BitStream sendPacket( buf, 100 );
	stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_DeleteMailRequest, nMailId, SERVICE_WORLDSERVER );

	pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	g_ServerGameplayState->GetPacketProcess()->Send( sendPacket );
}

void CZoneMail::SendMail( int nSenderId, int nRecverId, int nItemId, int nItemCount, int nMoney, const char* title, const char* szMsg, bool isSystem )
{
	Player* pPlayer = g_ServerGameplayState->GetPlayer( nSenderId );
	if( !pPlayer )
		return ;

	stMailItem mailItem;

	dStrcpy( mailItem.buf, sizeof(mailItem.buf), szMsg );
	dStrcpy( mailItem.title, sizeof( mailItem.title ), title );
	mailItem.nSender = nSenderId;
	mailItem.nItemId = nItemId;
	mailItem.nItemCount = nItemCount;
	mailItem.nMoney = nMoney;
	mailItem.isSystem = isSystem;
	dStrcpy( mailItem.szSenderName, sizeof(mailItem.szSenderName), pPlayer->getPlayerName());

	char buf[MAX_PACKET_SIZE];
	Base::BitStream sendPacket( buf, MAX_PACKET_SIZE );

	stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_SendMailRequest,nSenderId,SERVICE_WORLDSERVER,nRecverId);
	mailItem.PackData( sendPacket );

	pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	g_ServerGameplayState->GetPacketProcess()->Send( sendPacket );
}

void CZoneMail::GetItems( int nMailId )
{
	char buf[100];
	Base::BitStream sendPacket( buf, 100 );
	stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_MailGetItems, nMailId, SERVICE_WORLDSERVER );

	pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	g_ServerGameplayState->GetPacketProcess()->Send( sendPacket );

}

void CZoneMail::GetMoney( int nMailId )
{
	char buf[100];
	Base::BitStream sendPacket( buf, 100 );
	stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_MailGetMoney, nMailId, SERVICE_WORLDSERVER );

	pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	g_ServerGameplayState->GetPacketProcess()->Send( sendPacket );

}

void CZoneMail::TryGetItems( int nMailId, int nPlayerId )
{
	char buf[100];
	Base::BitStream sendPacket( buf, 100 );
	stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_MailTryGetItems, nMailId, SERVICE_WORLDSERVER, nPlayerId, g_ServerGameplayState->getZoneId() );

	pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	g_ServerGameplayState->GetPacketProcess()->Send( sendPacket );

}

void CZoneMail::TryGetMoney( int nMailId, int nPlayerId )
{
	char buf[100];
	Base::BitStream sendPacket( buf, 100 );
	stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_MailTryGetMoney, nMailId, SERVICE_WORLDSERVER, nPlayerId, g_ServerGameplayState->getZoneId() );

	pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	g_ServerGameplayState->GetPacketProcess()->Send( sendPacket );

}

void CZoneMail::DeleteAllMail( int nPlayerId )
{
	char buf[100];
	Base::BitStream sendPacket( buf, 100 );
	stPacketHead* pHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_DeleteAllMail, nPlayerId, SERVICE_WORLDSERVER, 0, g_ServerGameplayState->getZoneId() );

	pHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );
	g_ServerGameplayState->GetPacketProcess()->Send( sendPacket );
}

ConsoleFunction( SptMail_Send, void, 7, 7, "" )
{
	CZoneMail::SendMail( 0, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5], argv[6], 1 );
}