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

// �������е�Res.h����һ��
enum EquipPlaceFlag
{

	EQUIPPLACEFLAG_UNKNOWN		= 0,			// δ֪����

	EQUIPPLACEFLAG_HEAD			= 1,			// ͷ��
	EQUIPPLACEFLAG_NECK			= 2,			// ����
	EQUIPPLACEFLAG_BACK			= 3,			// ����
	EQUIPPLACEFLAG_SHOULDER		= 4,			// �粿
	EQUIPPLACEFLAG_BODY			= 5,			// ����
	EQUIPPLACEFLAG_HAND			= 6,			// �ֲ�
	EQUIPPLACEFLAG_WAIST		= 7,			// ����
	EQUIPPLACEFLAG_FOOT			= 8,			// �Ų�
	EQUIPPLACEFLAG_RING			= 9,			// ��ָ
	EQUIPPLACEFLAG_AMULET		= 10,			// �����
	EQUIPPLACEFLAG_WEAPON		= 11,			// ����
	EQUIPPLACEFLAG_TRUMP		= 12,			// ����������
	EQUIPPLACEFLAG_GATHER		= 13,           // �ɼ�����
	EQUIPPLACEFLAG_RIDE			= 14,			// �������(��UI������)
	EQUIPPLACEFLAG_PET			= 15,			// �������(��UI������)
	EQUIPPLACEFLAG_FASHIONHEAD  = 16,           // ʱװͷ
	EQUIPPLACEFLAG_FASHIONBODY  = 17,           // ʱװ����
	EQUIPPLACEFLAG_FASHIONBACK  = 18,           // ʱװ����
	EQUIPPLACEFLAG_PACK1		= 19,			// ��չ����1
	EQUIPPLACEFLAG_PACK2		= 20,			// ��չ����2
	EQUIPPLACEFLAG_STALLPACK1	= 21,			// ��̯��չ��1
	EQUIPPLACEFLAG_STALLPACK2	= 22,			// ��̯��չ��2
	EQUIPPLACEFLAG_STALLPACK3	= 23,			// ��̯��չ��3
};

struct stSocketInfo
{
	int PlayerId;
	int LoginIP;
};

class CDBOperator;

/************************************************************************/
/* ����ǰ���е��ʺ�
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

	HashPlayerNameMap       m_PlayerNameMap;			//��¼��PlayerNameΪ��ֵ
	HashAccountMap          m_PlayerMap;				//��¼��PlayerIdΪ��ֵ
	HashAccountMap          m_AccountMap;			    //��¼���ʺ�Ϊ��ֵ
	HashLineZonePlayerMap   m_LineZonePlayer;	        //�õ�ͼ��֯�����
	HashAccountSocketMap	m_AccountSocketMap;			//��¼��SocketΪ��ֵ

	HashPlayerSaveIDMap		m_PlayerSIDMap;				//��¼��ҵ�ǰ��SID

public:
	stdext::hash_set<int> loginPendingList;
public:
	CPlayerManager();
	~CPlayerManager();
    
    /************************************************************************/
    /* ����ʺŹ���
    /************************************************************************/
    int AddAccount(const stAccountInfo& accountInfo);

    //stAccountInfo* GetAccountInfo(int AccountId);

	void showAllAccountInfo();

	bool TimeProcess(bool bExit);
    void Update(void);

	void deleteAccount( stAccountInfo* pAccountInfo );
	//�����Ϣ����
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