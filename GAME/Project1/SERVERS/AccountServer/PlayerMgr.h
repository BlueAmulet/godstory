#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include <string>
#include <hash_map>
#include <Time.h>

#include "Base/Locker.h"
#include "Common/TimerMgr.h"
#include "Common/PacketType.h"

struct stSocketInfo
{
	int AccountId;
	int LoginIP;
};

struct stAccountInfo
{
	int LastTime;
	T_UID UID;
	int socketId;
	int	AccountId;
	char AccountName[COMMON_STRING_LENGTH];
	char Password1[MD5_STRING_LENGTH];
	char Password2[MD5_STRING_LENGTH];
	char Password3[MD5_STRING_LENGTH];

	stAccountInfo()
	{
		LastTime	 = (int)time(NULL);
		UID			 = 0;
		AccountId	 = 0;
		socketId	 = 0;
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
	typedef stdext::hash_map<int,stAccountInfo> HashAccountMap;
    typedef stdext::hash_map<int,std::string>   IPMAP;
	HashSocketMap  m_SocketMap;
	HashAccountMap m_AccountMap;
    IPMAP          m_ips;

	static int m_uidSeed;
public:
	CMyCriticalSection m_cs;

public:
	CPlayerManager();
	~CPlayerManager();

	bool TimeProcess(bool bExit);
    
    void AddIPMap(int SocketId,const char* ip);
	//玩家连接管理
	bool AddSocketMap(int SocketId,const char *Ip);
	bool ReleaseSocketMap(int SocketId,bool postEvent=false);
	stSocketInfo *GetSocketMap(int SocketId);

	//玩家信息管理
	stAccountInfo *AddAccountMap(int SocketId,int AccountId);
	bool		  ReleaseAccountMap(int AccountId);
	stAccountInfo *GetAccountMap(int AccountId);
};

#endif