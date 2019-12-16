#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include <string>
#include <hash_map>
#include <hash_set>
#include <set>
#include <Time.h>

#include "Base/bitStream.h"
#include "Base/Locker.h"
#include "Common/TimerMgr.h"
#include "Common/PacketType.h"
#include "Common/PlayerStruct.h"
#include "Common/Tracable.h"

#include "AccountHandler.h"
#include "LockCommon.h"
#include "PlayerCache.h"

// 与引擎中的Res.h保持一致
enum EquipPlaceFlag
{

	EQUIPPLACEFLAG_UNKNOWN		= 0,			// 未知类型

	EQUIPPLACEFLAG_HEAD			= 1,			// 头部
	EQUIPPLACEFLAG_NECK			= 2,			// 颈部
	EQUIPPLACEFLAG_BACK			= 3,			// 背部
	EQUIPPLACEFLAG_SHOULDER		= 4,			// 肩部
	EQUIPPLACEFLAG_BODY			= 5,			// 身体
	EQUIPPLACEFLAG_HAND			= 6,			// 手部
	EQUIPPLACEFLAG_WAIST		= 7,			// 腰部
	EQUIPPLACEFLAG_FOOT			= 8,			// 脚部
	EQUIPPLACEFLAG_RING			= 9,			// 戒指
	EQUIPPLACEFLAG_AMULET		= 10,			// 护身符
	EQUIPPLACEFLAG_WEAPON		= 11,			// 武器
	EQUIPPLACEFLAG_TRUMP		= 12,			// 法宝类武器
	EQUIPPLACEFLAG_GATHER		= 13,           // 采集道具
	EQUIPPLACEFLAG_RIDE			= 14,			// 坐骑道具(在UI上隐藏)
	EQUIPPLACEFLAG_PET			= 15,			// 宠物道具(在UI上隐藏)
	EQUIPPLACEFLAG_FASHIONHEAD  = 16,           // 时装头
	EQUIPPLACEFLAG_FASHIONBODY  = 17,           // 时装身体
	EQUIPPLACEFLAG_FASHIONBACK  = 18,           // 时装背部
	EQUIPPLACEFLAG_PACK1		= 19,			// 扩展背包1
	EQUIPPLACEFLAG_PACK2		= 20,			// 扩展背包2
	EQUIPPLACEFLAG_STALLPACK1	= 21,			// 摆摊扩展格1
	EQUIPPLACEFLAG_STALLPACK2	= 22,			// 摆摊扩展格2
	EQUIPPLACEFLAG_STALLPACK3	= 23,			// 摆摊扩展格3
};

struct stSocketInfo
{
	int PlayerId;
	int LoginIP;
};

class CDBOperator;

/************************************************************************/
/* 管理当前所有的帐号
/************************************************************************/
class CPlayerManager : 
    public CTimeCriticalObject, public ILockable, public CPlayerCache
{
public:

    typedef stdext::hash_map<int,stAccountInfo*>         HashAccountMap;
	typedef stdext::hash_map<std::string,stAccountInfo*> HashPlayerNameMap;
	typedef stdext::hash_map<int,HashAccountMap>         HashZonePlayerMap;
	typedef stdext::hash_map<int,HashZonePlayerMap>      HashLineZonePlayerMap;
	typedef stdext::hash_map<int,int>                    HashAccountPlayerMap;
	typedef stdext::hash_map<int,int>					 HashAccountSocketMap;
	typedef stdext::hash_map< int, int >				 HashPlayerSaveIDMap;

	HashPlayerNameMap       m_PlayerNameMap;			//登录的PlayerName为键值
	HashAccountMap          m_PlayerMap;				//登录的PlayerId为键值
	HashAccountMap          m_AccountMap;			    //登录的帐号为键值
	HashLineZonePlayerMap   m_LineZonePlayer;	        //用地图组织的玩家
	HashAccountSocketMap	m_AccountSocketMap;			//登录的Socket为键值

	HashPlayerSaveIDMap		m_PlayerSIDMap;				//记录玩家当前的SID

public:
	stdext::hash_set<int> loginPendingList;
public:
	CPlayerManager();
	~CPlayerManager();
    
    /************************************************************************/
    /* 添加帐号管理
    /************************************************************************/
    int AddAccount(const stAccountInfo& accountInfo);

    //stAccountInfo* GetAccountInfo(int AccountId);

	void showAllAccountInfo();

	bool TimeProcess(bool bExit);
    void Update(void);

	void deleteAccount( stAccountInfo* pAccountInfo );
	//玩家信息管理
	PlayerDataRef GetPlayerData(int PlayerId);
	PlayerDataRef GetPlayerData( std::string playerName );

	PlayerDataRef LoadPlayer( int playerId );
	PlayerDataRef LoadPlayer( std::string playerName );
	
	PlayerDataRef GetOfflinePlayerData( int playerId );
	PlayerDataRef GetOfflinePlayerData( std::string playerName );

	int ResetPlayerStatus(T_UID UID,int AccountId);
	int VerifyUID(T_UID UID,int AccountId);

	void PackPlayerSelectData(int AccountId,Base::BitStream &);
	void PackPlayerSelectData(int AccountId,int PlayerId,Base::BitStream &SendPacket);
	int PackPlayerData(int PlayerId,Base::BitStream &);

	void SendSimplePacket( int nPlayerId, unsigned short cMessageType, int nDestZoneId = 0, int nSrcZoneId = 0 );
	void SendZoneSimplePacket( int nPlayerId, unsigned short cMessageType,int id = 0,  int nSrcZoneId = 0,int param1 = 0, int param2 = 0 );

	int SavePlayerData( stPlayerStruct* pPlayerData,bool isFromGMCmd = false);
	int SavePlayerData( PlayerDataRef pPlayerData );

	int updatePlayerSID( int playerId );

	int AddPlayerMap(int SocketId,T_UID UID,int LineId,int AccountId,int PlayerId,bool isFirst);
	int GetPlayerZoneId(int PlayerId);
	int DeletePlayer(T_UID UID,int AccountId,int Playerid);
	int CreatePlayer(T_UID UID,int AccountId,const char *PlayerName,int Sex,int Body,int Face,int Hair,int HairCol,U32 ItemId,int ZoneId,int &Playerid);
	int CreatePlayerData(CDBOperator &dbHandle,stPlayerData *pPlayer);

	int GetCurrentPlayerSID( int playerId );

	int GetSelectedPlayer( int nAccountId );
	bool IsGM( int nPlayerId );

	void RemoveAccount( int nAccountId );
	void RemovePlayer( int nPlayerId );

	void GetLineAllPlayers( std::list<int>& playerList, int nLindId );
	void GetZoneAllPlayers( std::list<int>& playerList, int nLineId, int nZoneId );
	void GetWorldAllPlayers( std::list<int>& playerList );
	void PutPlayersInGate(  std::list<int>& playerList, stdext::hash_map<int,stdext::hash_set<int> >& gatePlayerListMap );

	stAccountInfo *GetAccountInfo(int nPlayerId);
	stAccountInfo *GetAccountMap( int AccountId );

	HashAccountMap& GetPlayerMap();
	

	stAccountInfo* GetAccountInfoBySocket( int socketId );

	void PackPlayerTransDataBase( int nPlayerId, Base::BitStream& packet );
	void PackPlayerTransDataNormal( int nPlayerId, Base::BitStream& packet );

	void OnZoneServerRestart(int LineId,int ZoneId);
	void SendSimpleInfoPacket( int nPlayerId, unsigned short cMessageType, int nDestZoneId, int nSrcZoneId = 0 );
};

#endif