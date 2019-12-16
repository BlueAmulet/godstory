#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include <string>
#include <hash_map>
#include <list>

#include "Base/Locker.h"
#include "LockCommon.h"

struct stServerInfo
{
	int Type;			//服务类型编号
	int LineId;			//服务线路编号
	int GateId;			//服务网关编号
	int Ip;				//服务IP
	int Port;			//服务端口
	int SocketId;		//连接句柄
};

struct stZoneInfo
{
	int ZoneId;			//地图服务器编号
	int MaxPlayerNum;	//最大玩家数量
	int ServicePort;    //服务端口号
};

struct stServerZone
{
	stServerInfo *pGateInfo;
	int ZoneIp;
	int ZonePort;
	int ConnectSeq;
	int Socket;
};

class CServerManager : public ILockable
{
protected:
	typedef stdext::hash_map<int,stServerInfo*> HashSocketMap;
	typedef stdext::hash_map<int,stServerZone>  HashZoneMap;
	typedef stdext::hash_map<int, HashSocketMap > HashLineGateMap;
	typedef stdext::hash_map<int, HashZoneMap > HashLineZoneMap;
	typedef stdext::hash_map<int,stZoneInfo> HashZoneInfo;

	HashSocketMap	m_SocketMap;
	HashLineGateMap m_LineGateMap;
	HashLineZoneMap m_LineZoneMap;
	stServerInfo  *	m_pAccountInfo;

	HashZoneInfo	m_ZoneInfoList;

public:
	CServerManager();
	~CServerManager();

	bool Initialize();
	void AddServerMap(stServerInfo &ServerInfo);
	void ReleaseServerMap(int SocketId,bool postEvent=false);
	stServerInfo *GetServerInfo(int ServerType,int ServerLineId=0,int ServerId=0);
	stServerInfo *GetServerInfoBySocket(int SocketId);
	void PackGateData(Base::BitStream &);

	void PackZoneData(Base::BitStream &);
	void AddZoneToGate(int SocketId,int ZoneId,int Ip,int Port,int ConnectSeq);
	int GetSocketId(int SocketId,int ZoneId);
	bool GetZoneIpPort(int SocketId,int ZoneId,int &Ip,int &Port);
	int GetLineId(int SocketId);
	stServerZone* GetZoneInfo(int SocketId, int ZoneId);
	void RemoveZoneFromGate(int SocketId, int ZoneId);

	void GetAllGates( std::list<stServerInfo*>& serverInfoList );
	void GetAllGates( std::list<stServerInfo*>& serverInfoList, int nLineId );
	stServerInfo* GetGateServer( int nLineId, int nGateId );
	stServerInfo* GetGateServerByZoneId(int nLineId, int nZoneId );
	stServerInfo* GetGateServerByPlayerId( int nPlayerId );
	int GetZoneSocket(int LineId, int ZoneId );
};

#endif