#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include <string>
#include <hash_map>

#include "Base/Locker.h"

struct stServerInfo
{
	int ZoneId;			//��ͼ���
	int SocketId;		//���Ӿ��
	int Ip;
	int Port;
	int ConnectSeq;		//������ţ������ж��ǵ�ͼ�������Ͽ������˻���崻���
};

struct stZoneInfo
{
	int ZoneId;			//��ͼ���������
	int MaxPlayerNum;	//����������
	int ServicePort;    //����˿ں�
};

class CServerManager
{
protected:
	typedef stdext::hash_map<int,stServerInfo*> HashSocketMap;
	typedef stdext::hash_map<int,stServerInfo*> HashZoneMap;
	typedef stdext::hash_map<int,stZoneInfo> HashZoneInfo;

	HashSocketMap	m_SocketMap;
	HashZoneMap		m_ZoneMap;

	HashZoneInfo	m_ZoneInfoList;

	int				mDataAgentSocket;
public:
	CMyCriticalSection m_cs;

public:
	CServerManager();
	~CServerManager();

	bool Initialize();
	void AddServerMap(stServerInfo &ServerInfo);
	void ReleaseServerMap(int SocketId,bool postEvent=false);
	stServerInfo *GetServerInfo(int ZoneId);
	stServerInfo *GetZoneInfo( int socketId );
	int GetZoneSocketId(int ZoneId);
	int GetDataAgentSocket();
	void SetDataAgentSocket( int socketId );

	void UnpackZoneData(Base::BitStream *);
	bool GetZoneInfo(int ZoneId,stZoneInfo &);
	stServerInfo* GetZoneInfo(int SocketId, int ZoneId);
	void PackZoneMgrList(Base::BitStream &);
	void RemoveZoneFromGate(int SocketId, int ZoneId);
};

#endif