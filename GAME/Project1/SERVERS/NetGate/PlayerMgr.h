#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include <string>
#include <hash_map>
#include <Time.h>
#include <list>

#include "Base/Locker.h"
#include "Common/TimerMgr.h"
#include "Common/PacketType.h"

struct stSocketInfo
{
	int PlayerId;
	int LoginIP;
	T_UID UID;
};

struct stPlayerInfo
{
	int LastTime;
	T_UID UID;
	int socketId;
	bool isAdult;
	int PointNum;
	char AccountName[COMMON_STRING_LENGTH];
	char Password1[MD5_STRING_LENGTH];
	char Password2[MD5_STRING_LENGTH];
	char Password3[MD5_STRING_LENGTH];

	stPlayerInfo()
	{
		LastTime	 = (int)time(NULL);
		UID			 = 0;
		socketId	 = 0;
		isAdult		 = true;		
		PointNum	 = 0;
		Password1[0] = 0;
		Password2[0] = 0;
		Password3[0] = 0;
		AccountName[0] = 0;
	}
};

class CPlayerManager : public CTimeCriticalObject
{
protected:
	typedef stdext::hash_map<int,stSocketInfo> HashSocketMap;
	typedef stdext::hash_map<int,stPlayerInfo> HashPlayerMap;

	HashSocketMap m_SocketMap;
	HashPlayerMap m_PlayerMap;

	static int m_uidSeed;
public:
	CMyCriticalSection m_cs;

public:
	CPlayerManager();
	~CPlayerManager();

	bool TimeProcess(bool bExit);

	//玩家连接管理
	bool AddSocketMap(int SocketId,const char *Ip);
	bool ReleaseSocketMap(int SocketId,bool postEvent=false);
	stSocketInfo *GetSocketMap(int SocketId);
	int GetSocketHandle(int PlayerId);

	//玩家信息管理
	int GetPlayerIdBySocket( int nSocket,T_UID& UID);
	int AddPlayerMap(int SocketId,T_UID UID,int PlayerId);
	bool		  ReleasePlayerMap(int PlayerId);
	stPlayerInfo *GetPlayerMap(int PlayerId);
	void GetAllPlayers( std::list<stPlayerInfo*>& playerInfoList );
	stdext::hash_map<int,stPlayerInfo>* GetAllPlayers();
};

#endif