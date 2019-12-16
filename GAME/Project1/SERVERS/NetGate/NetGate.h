#ifndef NET_GATE_H
#define NET_GATE_H

#include "Common/DataAgentHelper.h"

#include <windows.h>

#define SERVER_CLASS_NAME CNetGate

class CScript;
class dtServerSocket;
class CDBConnPool;
class CAsyncSocket2;
class CWorkQueue;
class CTimerMgr;
class CPlayerManager;
class CServerManager;
class CUDPServer;
struct WorkQueueItemStruct;
struct stPacketHead;

class CMemoryShare;

#define SERVER		SERVER_CLASS_NAME::GetInstance()
#define DATABASE	SERVER_CLASS_NAME::GetDBConnPool()
#define MEMPOOL		CMemPool::GetInstance()

class SERVER_CLASS_NAME
{
	static HANDLE				m_ExitEvent;
	static SERVER_CLASS_NAME *	m_pInstance;
	static CScript *			m_pScript;
	static CDBConnPool *		m_pDBConnPool;
	static CWorkQueue *			m_pWorkQueue;
	static CWorkQueue *			m_pUserWorkQueue;

	HANDLE m_StartEvent;

	bool						m_Inited;
	dtServerSocket	*			m_pServerService;
	dtServerSocket	*			m_pUserService;
	CAsyncSocket2	*			m_pWorldClient;
	CUDPServer		*			m_pUDPServerSide;
	CUDPServer		*			m_pUDPClientSide;
	CTimerMgr *					m_pTimeMgr;
	CPlayerManager *			m_pPlayerManager;
	CServerManager *			m_pServerManager;

	CDataAgentHelper			m_dataAgentHelper;

private:
	SERVER_CLASS_NAME();

	bool InstallBreakHandlers();
	bool InitializeDatabase();

	static void OnBreak(int);

public:
	~SERVER_CLASS_NAME();

	static SERVER_CLASS_NAME	*GetInstance()
	{
		if(!m_pInstance)
			m_pInstance = new SERVER_CLASS_NAME;

		return m_pInstance;
	}

	CAsyncSocket2	*GetWorldSocket()	{		return m_pWorldClient;	}
	dtServerSocket	*GetServerSocket()	{		return m_pServerService;}
	dtServerSocket	*GetUserSocket()	{		return m_pUserService;	}
	CPlayerManager  *GetPlayerManager()	{		return m_pPlayerManager;}
	CServerManager  *GetServerManager()	{		return m_pServerManager;}

	CDataAgentHelper*GetDataAgentHelper(){		return &m_dataAgentHelper;}



	static CDBConnPool		*GetDBConnPool()	{		return m_pDBConnPool;	}
	static CWorkQueue		*GetWorkQueue()		{		return m_pWorkQueue;	}
	static CWorkQueue		*GetUserWorkQueue()	{		return m_pUserWorkQueue;}
	static int				EventProcess(LPVOID Param);

	void SendToDataAgent( WorkQueueItemStruct * pItem, stPacketHead* pHead );

	static int				UserEventProcess(LPVOID Param);
	int GateId;
	int LineId;

	bool Initialize();
	void StartService();
	void StopService();
	void DenyService();
	void Maintenance();

	void OnServerStart();

	void OnServerStarted();
	void ShowMessage();
};

#endif
