#include "dblib/dbLib.h"
#include "MailManager.h"
#include "ChatMgr.h"
#include "TeamManager.h"
#include "Event.h"
#include "WorldServer.h"

#include "Common/MailBase.h"


CMailManager::CMailManager()
{
	registerEvent( ZONE_WORLD_MailListRequest,	&CMailManager::HandleZoneMailRequest );
	registerEvent( ZONE_WORLD_SendMailRequest,	&CMailManager::HandleZoneSendMailRequest );
	registerEvent( ZONE_WORLD_DeleteMailRequest,&CMailManager::DeleteMail );
	registerEvent( ZONE_WORLD_MailReaded,		&CMailManager::MailReaded );
	registerEvent( ZONE_WORLD_MailTryGetItems,  &CMailManager::HandleTryGetItems );
	registerEvent( ZONE_WORLD_MailTryGetMoney,  &CMailManager::HandleTryGetMoney );
	registerEvent( ZONE_WORLD_MailGetItems,		&CMailManager::GetMailItems );
	registerEvent( ZONE_WORLD_MailGetMoney,		&CMailManager::GetMailMoney );
	registerEvent( ZONE_WORLD_DeleteAllMail,	&CMailManager::DeleteAllMail );
	gLock.registerLockable( this, Lock::Mail );
}

CMailManager::~CMailManager()
{

}

void CMailManager::HandleZoneMailRequest( int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet )
{
	DO_LOCK( Lock::Mail );

	stMailHeader mailHeader;

	mailHeader.UnpackData( *Packet );

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );
	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_MailListResponse, pHead->Id, SERVICE_ZONESERVER, pHead->SrcZoneId );

	CDBOperator db( SERVER->GetDBConnPool() );

	// 收取所有邮件
	db->SQL("UPDATE MailList SET received = 1 WHERE id IN ( SELECT TOP 30 id FROM MailList ORDER BY [id] ASC )");
	db->Exec();

	if( mailHeader.nPage == 0 )
	{
		// 取得已收取邮件数量
		db->SQL("SELECT COUNT( [id] ) AS c FROM MailList WHERE [recver] = %d AND received = 1", mailHeader.nRecver );
		try
		{
			if( db->More() )
			{
				mailHeader.nRecvCount = db->GetInt("c");
			}
		}
		DB_CATCH_LOG( GetMailCount );

		// 取得未收取信件邮件数量
		db->SQL("SELECT COUNT( [id] ) AS c FROM MailList WHERE [recver] = %d AND received = 0 AND money = 0 AND itemId = 0", mailHeader.nRecver );
		try
		{
			if( db->More() )
			{
				mailHeader.nLetterCount = db->GetInt("c");
			}
		}
		DB_CATCH_LOG( GetMailCount );

		// 取得未收取包裹邮件数量
		db->SQL("SELECT COUNT( [id] ) AS c FROM MailList WHERE [recver] = %d AND received = 0 AND ( money > 0 OR itemId > 0 )", mailHeader.nRecver );
		try
		{
			if( db->More() )
			{
				mailHeader.nPackageCount = db->GetInt("c");
			}
		}
		DB_CATCH_LOG( GetMailCount );

	}
	mailHeader.PackData( sendPacket );	

	// 取得一页的邮件内容
	db->SQL( "SELECT TOP %d * FROM MailList WHERE [recver] = %d AND [id] NOT IN ( SELECT TOP %d [id] FROM MailList WHERE [recver] = %d AND received = 1 ORDER BY [id] ASC ) AND received = 1 ORDER BY [id] ASC", mailHeader.nCount * ( mailHeader.nPage + 1 ), mailHeader.nRecver,  mailHeader.nCount * mailHeader.nPage, mailHeader.nRecver );

	stMailItem mailItem;

	try
	{
		while( db->More() )
		{
			sendPacket.writeFlag( true );
			mailItem.id = db->GetInt("id");
			mailItem.nSender = db->GetInt("sender");
			strcpy_s( mailItem.szSenderName, sizeof( mailItem.szSenderName ), db->GetString("sender_name") );
			strcpy_s( mailItem.title, sizeof( mailItem.title ), db->GetString("title") );
			mailItem.nLength = db->GetColDataLen( 6 );
			strcpy_s( mailItem.buf, sizeof( mailItem.buf), db->GetString("msg") );
			mailItem.nMoney = db->GetInt("money");
			mailItem.nItemId = db->GetInt("itemId");
			mailItem.nItemCount = db->GetInt("item_count");
			mailItem.nTime = db->GetInt("send_time");
			mailItem.isReaded = db->GetInt("readed");
			mailItem.isSystem = db->GetInt("isSystem");

			mailItem.PackData( sendPacket );
		}
	}
	DB_CATCH_LOG( SelectMailItem );

	sendPacket.writeFlag( false );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket );
}

void CMailManager::HandleZoneSendMailRequest( int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet )
{
	DO_LOCK( Lock::Mail );

	stMailItem mailItem;

	mailItem.UnpackData( *Packet );

	int nRecver = pHead->DestZoneId;

	CDBOperator db( SERVER->GetDBConnPool() );

	int time = ::_time32( NULL );

	// 插入一条记录(以后要改为存储过程)
	db->SQL("INSERT INTO MailList (recver,sender,sender_name, itemId, item_count, money, msg, send_time, isSystem, title ) VALUES (%d,%d,'%s',%d,%d,%d,'%s',%d, %d, '%s')", nRecver, mailItem.nSender, mailItem.szSenderName, mailItem.nItemId, mailItem.nItemCount, mailItem.nMoney, mailItem.buf, mailItem.nTime, mailItem.isSystem, mailItem.title );
	try
	{
		db->Exec();
	}

	//TODO log
	//lg_mail log;
	//log.sendName  = mailItem.szSenderName;
	////log.recvName  = nRecver;
	//log.itemCount = mailItem.nItemCount;
	//log.gold      = mailItem.nMoney;
	//log.time      = time(0);

	//LOG(log);

	DB_CATCH_LOG(SendMailRequest);

	SERVER->GetPlayerManager()->SendSimplePacket( nRecver, WORLD_CLIENT_MailNotify );
}

void CMailManager::DeleteMail( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket)
{
	DO_LOCK( Lock::Mail );

	int nId = pHead->Id;

	CDBOperator db( SERVER->GetDBConnPool() );

	// 插入一条记录(以后要改为存储过程)
	db->SQL("DELETE FROM MailList WHERE [id] = %d", nId );
	try
	{
		db->Exec();
	}
	DB_CATCH_LOG(DeleteMailRequest);

}

void CMailManager::MailReaded( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
{
	DO_LOCK( Lock::Mail );

	int nId = pHead->Id;

	CDBOperator db( SERVER->GetDBConnPool() );

	// 标记已读(以后要改为存储过程)
	db->SQL("UPDATE MailList SET readed = 1 WHERE [id] = %d", nId );
	try
	{
		db->Exec();
	}
	DB_CATCH_LOG(MailReaded);
}

void CMailManager::GetMailItems( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
{
	DO_LOCK( Lock::Mail );

	int nId = pHead->Id;

	CDBOperator db( SERVER->GetDBConnPool() );

	// 标记已读(以后要改为存储过程)
	db->SQL("UPDATE MailList SET money = 0 WHERE [id] = %d", nId );
	try
	{
		db->Exec();
	}
	DB_CATCH_LOG(MailReaded);
}

void CMailManager::GetMailMoney( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
{
	DO_LOCK( Lock::Mail );

	int nId = pHead->Id;

	CDBOperator db( SERVER->GetDBConnPool() );

	// 标记已读(以后要改为存储过程)
	db->SQL("UPDATE MailList SET itemId = 0, item_count = 0 WHERE [id] = %d", nId );
	try
	{
		db->Exec();
	}
	DB_CATCH_LOG(MailReaded);
}

void CMailManager::HandleTryGetItems( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
{
	int nMailId = pHead->Id;
	int srcZoneId = pHead->SrcZoneId;
	int nPlayerId = pHead->DestZoneId;

	int nItemId;
	int nItemCount;

	CDBOperator db( SERVER->GetDBConnPool() );

	db->SQL("SELECT itemId, item_count FROM MailList WHERE [id] = %d", nMailId );
	try
	{
		while( db->More() )
		{
			nItemId = db->GetInt("itemId");
			nItemCount = db->GetInt("item_count");
		}
	}
	DB_CATCH_LOG(MailReaded);

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );

	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_MailTryGetItems, nMailId, SERVICE_ZONESERVER, srcZoneId, nPlayerId );

	sendPacket.writeInt( nItemId, 32 );
	sendPacket.writeInt( nItemCount, 16 );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket );
}

void CMailManager::HandleTryGetMoney( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
{
	int nMailId = pHead->Id;
	int srcZoneId = pHead->SrcZoneId;
	int nPlayerId = pHead->DestZoneId;

	int nMoney;

	CDBOperator db( SERVER->GetDBConnPool() );

	db->SQL("SELECT money FROM MailList WHERE [id] = %d", nMailId );
	try
	{
		while( db->More() )
		{
			nMoney = db->GetInt("money");
		}
	}
	DB_CATCH_LOG(MailReaded);

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );

	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_MailTryGetMoney, nMailId, SERVICE_ZONESERVER, srcZoneId, nPlayerId, nMoney );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket );
}

void CMailManager::DeleteAllMail( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket )
{
	int nPlayerId = pHead->Id;
	int srcZoneId = pHead->SrcZoneId;

	DO_LOCK( Lock::Mail );

	CDBOperator db( SERVER->GetDBConnPool() );

	try
	{
		db->SQL("DELETE MailList FROM ( SELECT TOP 30 * FROM MailList ORDER BY id ASC ) AS tb WHERE tb.[recver] = %d and tb.received = 1", nPlayerId );
		db->Exec();
	}
	DB_CATCH_LOG(DeleteAllMail);

	CMemGuard buf( MAX_PACKET_SIZE );
	Base::BitStream sendPacket( buf.get(), MAX_PACKET_SIZE );

	stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, ZONE_WORLD_DeleteAllMail, nPlayerId, SERVICE_ZONESERVER, srcZoneId );

	pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

	SERVER->GetServerSocket()->Send( SocketHandle, sendPacket );
}