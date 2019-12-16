#ifndef MAIL_BASE_H
#define MAIL_BASE_H

#include "Base/BitStream.h"

#define MAX_MAIL_ITEM_LENGTH 1024

struct stMailHeader
{
	int nRecver;
	int nPage;
	int nCount;
	int nRecvCount;
	int nLetterCount;
	int nPackageCount;

	stMailHeader()
	{
		memset( this, 0, sizeof( stMailHeader ) );
	}

	template< typename _Ty >
	void PackData( _Ty& packet )
	{
		packet.writeInt( nRecver, 32 );
		packet.writeInt( nPage, 8 );
		packet.writeInt( nCount, 8 );
		if( nPage == 0 )
		{
			packet.writeInt( nRecvCount, 16 );
			packet.writeInt( nLetterCount, 16 );
			packet.writeInt( nPackageCount, 16 );
		}
	}

	template< typename _Ty >
	void UnpackData( _Ty& packet )
	{
		nRecver = packet.readInt( 32 );
		nPage = packet.readInt( 8 );
		nCount = packet.readInt( 8 );
		if( nPage == 0 )
		{
			nRecvCount = packet.readInt( 16 );
			nLetterCount = packet.readInt( 16 );
			nPackageCount = packet.readInt( 16 );
		}
	}
};

struct stMailItem
{
	int id;
	int nSender;
	char szSenderName[COMMON_STRING_LENGTH];
	int nLength;
	int nMoney;
	int nItemId;
	int nItemCount;
	int nTime;
	bool isReaded;
	int isSystem;

	char title[COMMON_STRING_LENGTH];
	char buf[MAX_MAIL_ITEM_LENGTH];

	stMailItem()
	{
		memset( this, 0, sizeof( stMailItem ) );
	}

	stMailItem( const stMailItem& mailItem )
	{
		id = mailItem.id;
		nSender = mailItem.nSender;
		nLength = mailItem.nLength;
		nMoney = mailItem.nMoney;
		nItemId = mailItem.nItemId;
		nItemCount = mailItem.nItemCount;
		nTime = mailItem.nTime;
		isReaded = mailItem.isReaded;
		isSystem = mailItem.isSystem;

		strcpy_s( buf, sizeof( buf ), mailItem.buf );
		strcpy_s( szSenderName, sizeof( szSenderName ), mailItem.szSenderName );
		strcpy_s( title, sizeof( title ), mailItem.title );
	}

	template< typename _Ty >
	void PackData( _Ty& packet )
	{	
		packet.writeInt( id, 32 );
		packet.writeInt( nSender, 32 );
		packet.writeInt( nLength, 16 );
		packet.writeInt( nMoney, 32 );
		packet.writeInt( nItemId, 32 );
		packet.writeInt( nItemCount, 16 );
		packet.writeInt( nTime, 32 );
		packet.writeFlag( isReaded );
		packet.writeFlag( isSystem );

		packet.writeString( szSenderName, COMMON_STRING_LENGTH );
		packet.writeString( buf, MAX_MAIL_ITEM_LENGTH );
		packet.writeString( title, COMMON_STRING_LENGTH );
	}
	
	template< typename _Ty >
	void UnpackData( _Ty& packet )
	{
		id = packet.readInt( 32 );
		nSender = packet.readInt( 32 );
		nLength = packet.readInt( 16 );
		nMoney = packet.readInt( 32 );
		nItemId = packet.readInt( 32 );
		nItemCount = packet.readInt( 16 );
		nTime = packet.readInt( 32 );
		isReaded = packet.readFlag();
		isSystem = packet.readFlag();

		packet.readString( szSenderName,COMMON_STRING_LENGTH );
		packet.readString( buf ,MAX_MAIL_ITEM_LENGTH);
		packet.readString( title ,COMMON_STRING_LENGTH);
	}
};

#endif