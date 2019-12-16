#ifndef ACCOUNT_SERVER_H
#define ACCOUNT_SERVER_H

#include <windows.h>
#include <assert.h>

#define SERVER_CLASS_NAME CAccountServer

class CScript;
class dtServerSocket;
class CDBConnPool;
class CAsyncSocket2;
class CWorkQueue;
class CTimerMgr;
class CPlayerManager;

#define SERVER		SERVER_CLASS_NAME::GetInstance()
#define DATABASE	SERVER_CLASS_NAME::GetDBConnPool()
#define MEMPOOL		CMemPool::GetInstance()

class SERVER_CLASS_NAME
{
	HANDLE m_StartEvent;
	static HANDLE				m_ExitEvent;
	static SERVER_CLASS_NAME *	m_pInstance;
	static CScript *			m_pScript;
	static CDBConnPool *		m_pDBConnPool;
	static CWorkQueue *			m_pWorkQueue;

	bool						m_Inited;
	dtServerSocket	*			m_pUserService;
	CAsyncSocket2	*			m_pWorldClient;
	CTimerMgr *					m_pTimeMgr;
	CPlayerManager *			m_pPlayerManager;

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
	dtServerSocket	*GetServerSocket()	{		return m_pUserService;	}
	CPlayerManager  *GetPlayerManager()	{		return m_pPlayerManager;}

	static CDBConnPool		*GetDBConnPool()	{		return m_pDBConnPool;	}
	static CWorkQueue		*GetWorkQueue()		{		return m_pWorkQueue;	}
	static int				EventProcess(LPVOID Param);

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
