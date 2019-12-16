#ifndef TEAM_BASE_H
#define TEAM_BASE_H

#define MAX_TEAM_MAP_MARK_COUNT	6
#define MAX_TEAM_TARGET_COUNT	9

#include "base/bitStream.h"

#ifndef TCP_SERVER
	#include "core/bitStream.h"
#endif

//#ifdef NTJ_SERVER
//	#ifndef BASE_BITSTREAM
//		#include "core/bitStream.h"
//		#undef BITSTREAM
//		#define BITSTREAM BitStream
//	#else
//		#define BITSTREAM Base::BitStream
//	#endif
//#else
//	#ifndef BITSTREAM
//		#define BITSTREAM Base::BitStream
//	#endif
//#endif
//
//#ifdef NTJ_CLIENT
//#ifndef BASE_BITSTREAM
//#include "core/bitStream.h"
//#undef BITSTREAM
//#define BITSTREAM BitStream
//#else
//#define BITSTREAM Base::BitStream
//#endif
//#else
//#ifndef BITSTREAM
//#define BITSTREAM Base::BitStream
//#endif
//#endif
//
//#ifdef NTJ_EDITOR
//#ifndef BASE_BITSTREAM
//#include "core/bitStream.h"
//#undef  BITSTRAM
//#define BITSTREAM BitStream
//#else
//#define BITSTREAM Base::BitStream
//#endif
//#else
//#ifndef BITSTREAM
//#define BITSTREAM Base::BitStream
//#endif
//#endif

enum
{
	TEAM_ACTION_BUILD,		// 组队
	TEAM_ACTION_JOIN,		// 其他人申请入队
	TEAM_ACTION_ADD,		// 队长加其他人
};

struct stTeamMapMark
{
	short sMapId;
	short sX;
	short sY;

	stTeamMapMark()
	{
		memset( this, 0, sizeof( stTeamMapMark ) );
	}

	template<class T>
	void WritePacket( T* pPacket )
	{
		pPacket->writeInt( sMapId, 16 );
		pPacket->writeInt( sX, 16 );
		pPacket->writeInt( sY, 16 );
	}

	template<class T>
	void ReadPacket( T* pPacket )
	{
		sMapId = pPacket->readInt( 16 );
		sX = pPacket->readInt( 16 );
		sY = pPacket->readInt( 16 );
	}
};

struct stTeamTargetMark
{
	short sMapId;
	short sTargetId;

	stTeamTargetMark()
	{
		memset( this, 0, sizeof( stTeamTargetMark ) );
	}

	template<class T>
	void WritePacket( T* pPacket )
	{
		pPacket->writeInt( sMapId, 16 );
		pPacket->writeInt( sTargetId, 16 );
	}

	template<class T>
	void ReadPacket( T* pPacket )
	{
		sMapId = pPacket->readInt( 16 );
		sTargetId = pPacket->readInt( 16 );
	}
};

// 个人的队伍信息, 暂定,添加中...
// 
struct stTeamInfo
{
	bool	m_bInTeam;
	int		m_nId;
	char	m_szName[255];
	bool	m_bIsCaption;
	int		m_nCopymap;

	stTeamInfo()
	{
		memset( this, 0, sizeof( stTeamInfo ) );
	}

	template<class T>
	void WritePacket( T* pPacket )
	{
		if(	pPacket->writeFlag( m_bInTeam ) )
		{
			pPacket->writeInt( m_nId, 32 );
			pPacket->writeString( m_szName );
			pPacket->writeFlag( m_bIsCaption );
			pPacket->writeInt( m_nCopymap, 32 );
		}
	}

	template<class T>
	void ReadPacket( T* pPacket )
	{
		m_bInTeam = pPacket->readFlag();
		if( m_bInTeam )
		{
			m_nId = pPacket->readInt( 32 );
			pPacket->readString( m_szName ,255);
			m_bIsCaption = pPacket->readFlag();
			m_nCopymap = pPacket->readInt( 32 );
		}	
		else
		{
			m_nId = 0;
			*m_szName = 0;
			m_bIsCaption = 0;
			m_nCopymap = 0;
		}
	}
} ;

#endif