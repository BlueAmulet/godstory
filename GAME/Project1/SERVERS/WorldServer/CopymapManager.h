#pragma once

#include <string>
#include <memory.h>
#include <hash_map>
#include <hash_set>
#include <list>
#include "Base/Locker.h"
#include "Base/bitStream.h"
#include "common/CommonPacket.h"
#include "wintcp/dtServerSocket.h"

#include "LockCommon.h"
#include "ManagerBase.h"
#include "Common/GameData.h"

#define COPYMAP_IDSEED 10000

class CopymapData
{
	DECLARE_GAMEDATA_CLASS(CopymapData);
public:
	int			mId;
	int			mMaxPlayer;
	int			mMapCount;
	std::string mName;
	std::string mIcon;
	std::string mDesc;
	int			mMaxPlayerLevel;
	int			mMinPlayerLevel;
	int			mMaxPlayerCount;
	int			mMinPlayerCount;
	int			mRegen;
	int			mRegenCount;
	int			mCloseTime;
	int			mRequireItemId;
	int			mRequireItemCount;
	int			mRequireMoney;
	int			mJoinItemId;
	int			mJoinItemCount;
	int			mJoinMoney;
	int			mJoinable;
	int			mDataAnaylise;
	int			mCopymapConsumer;
	int			mMaxAdventureCount;
	int			mLeaveTime;
	int			mPartCount;
	std::string mLoadingImage;
	int			mLeaderInvation;
};

// [LivenHotch]: copy map type related information
struct stCopyMapTypeMapInfo
{
	int nMapId;					// 地图编号
	int nMaxCopyMapCount;		// 最大副本承载数量
	int nMaxPlayerCount;		// 最大玩家人数

	int nPlayerCount;
	int nCopyMapCount;

	stCopyMapTypeMapInfo()
	{
		memset( this, 0, sizeof( stCopyMapTypeMapInfo ) );
	}
};

// [LivenHotch]: 副本类型相关信息
struct stCopyMapTypeInfo
{
	int nId;					// 编号
	std::string strName;		// 名称

	stdext::hash_map< int, stCopyMapTypeMapInfo > mapMap;

	stCopyMapTypeInfo()
	{
		nId = 0;
		strName = "";
	}

	void operator = ( stCopyMapTypeInfo& info )
	{
		nId = info.nId;
		strName = info.strName;
		mapMap = info.mapMap;
	}
};

// [LivenHotch]: 副本实例信息
struct stCopyMapInstInfo
{
	int nId;
	int nTypeId;
	int nMapId;
	int nCreator;
	int nLineId;

	stCopyMapInstInfo()
	{
		memset( this, 0, sizeof( stCopyMapInstInfo ) );
	}
};

enum CopyMapError
{
	COPYMAP_ERROR_NONE = 0,
	COPYMAP_ERROR_NOSERVER,
	COPYMAP_ERROR_NOTEXISTED,
	COPYMAP_ERROR_FULL,
	COPYMAP_ERROR_ALREADYIN,
	COPYMAP_ERROR_UNKNOWN,
};

class stAccountInfo;

class CCopyMapManager : ILockable,public CManagerBase< CCopyMapManager, MSG_COPYMAP_BEGIN, MSG_COPYMAP_END >
{
public:
	CCopyMapManager(void);
	~CCopyMapManager(void);

	void Initialize();			
	
	int OpenCopymap( int nPlayerId, int nCopyMapTypeId, int& nCopyMapInstanceId );		// 开启一个副本
	int CloseCopyMap( int nId );										// 关闭一个副本

	void LoadCopyMapInfo();												// 读取副本信息

	int EnterCopyMap( int nPlayerId, int nCopyMapInstId );
	int LeaveCopyMap( int nPlayerId );

	int FindFreeMap( int nCopyMapTypeId );

	int GetPlayerCopyMapId( int nPlayerId );			// 获得玩家当前第的副本实例ID

	void TickPlayer( int nPlayerId );					// 将玩家从副本中踢出

	void HandleEnterRequest( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket );
	void HandleEnterRequest2( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket );
	void HandleOpenRequest( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket );
	void HandleOpenResponse( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket );

	void HandleCopymapPlayerDataResponse( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket );

	void HandleClose( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket );
	void HandleLeave( int SocketHandle, stPacketHead* pHead, Base::BitStream* pPacket );

	bool NotifyZoneOpenCopyMap( int nLineId, int nZoneId, int nCopyMapInstId, int nSrcZoneId, int nSrcLineId, int nPlayerId );
	void NotifyZoneCloseCopyMap( int socketHandle, int ZoneId, int nCopyMapInstId );
    
    void HandleCopymapEvent(stAccountInfo* pAccount,int opcode,int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet);
	bool SendCopymapPlayerData( int PlayerId, int ZoneId,int copymapInstId );
	void SendClientEnterResponse( int nErr, int playerId, int nCopyMapInstId );

	void ShowStatus();
private:

	stdext::hash_map<int,int>						m_playerCopyMap;
	stdext::hash_map<int, stdext::hash_set< int > > m_copyMapPlayer;
	stdext::hash_map<int,stCopyMapTypeInfo>			m_copyMapInfoTypeMap;
	stdext::hash_map<int,stCopyMapInstInfo>			m_copyMapInfoInstMap;

	int m_nIdSeed;
};

//---------------------------------------------------------------
// 组对象
//---------------------------------------------------------------
class CGroup
{
public:
	CGroup();
	virtual ~CGroup();

	void addPlayer( int playerId );
	
	typedef std::list<int>::iterator Iterator;

	Iterator begin();
	Iterator end();
	Iterator erase( Iterator& where );

	int size();

private:
	std::list< int > playerList;
};

//---------------------------------------------------------------
// 排队系统规则
//---------------------------------------------------------------
class QueueSystemRule
{
public:
	QueueSystemRule();
	virtual ~QueueSystemRule();
};

//---------------------------------------------------------------
// 队列对象
//---------------------------------------------------------------
class CQueue
{
public:
	CQueue();
	virtual ~CQueue();

public:
	// 制定规则
	void assignRule( QueueSystemRule* pRule );

	// 新排队玩家
	CGroup* onNewPlayer( int playerId );

private:
	QueueSystemRule* mRule;
};

//---------------------------------------------------------------
// 排队系统
//---------------------------------------------------------------
class QueueSystem
{
public:
	QueueSystem();
	virtual ~QueueSystem();

	// 创建一个队列
	CQueue* createQueue( QueueSystemRule* pRule );
};