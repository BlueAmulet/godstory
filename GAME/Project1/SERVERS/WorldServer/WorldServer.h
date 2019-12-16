#ifndef WORLD_SERVER_H
#define WORLD_SERVER_H

#include <WinSock2.h>
#include <windows.h>
#include "Event.h"
#include "TeamManager.h"
#include "Common/LogHelper.h"
#include "Common/VirtualConsole.h"

#define SERVER_CLASS_NAME CWorldServer

struct stPacketHead;

class CScript;
class dtServerSocket;
class CDBConnPool;
class CAsyncSocket2;
class CWorkQueue;
class CTimerMgr;
class CPlayerManager;
class CServerManager;
class CFriendManager;
//class CEventSource;
//class CTeamManager;
class CSavePlayerJob;
class CMailManager;
class CChatManager;
class CCopyMapManager;
class CMyLog;
class CSocialManager;
struct dbStruct;
class CEventManager;
class CTopManager;


#define SERVER		        SERVER_CLASS_NAME::GetInstance()
#define DATABASE	        SERVER_CLASS_NAME::GetDBConnPool()
#define DATABASE_ACCOUNT    SERVER_CLASS_NAME::GetAccountDBConnPool()
#define DATABASE_LOG        SERVER_CLASS_NAME::GetLogDBConnPool()
#define MEMPOOL		        CMemPool::GetInstance()

class SERVER_CLASS_NAME
{
	static HANDLE				m_ExitEvent;
	static SERVER_CLASS_NAME *	m_pInstance;
	static CScript *			m_pScript;
	static CDBConnPool *		m_pDBConnPool;
    static CDBConnPool*         m_pAccountDBConnPool;
    static CDBConnPool*         m_pLogDBConnPool;
	static CWorkQueue *			m_pWorkQueue;

	HANDLE	m_StartEvent;

	bool						m_Inited;
	dtServerSocket	*			m_pServerService;
	CTimerMgr *					m_pTimeMgr;
	CTimerMgr *					m_pSaveTimeMgr;
	CPlayerManager *			m_pPlayerManager;
	CServerManager *			m_pServerManager;
	CChatManager *				m_pChatManager;
	CTopManager *				m_pTopManager;

	CEventSource 				m_playerEvent;
	CTeamManager				m_teamManager;

	CSavePlayerJob*				m_pSavePlayerJob;
	CMailManager*				m_pMailManager;
	CCopyMapManager*			m_pCopyMapManager;

	CSocialManager*				m_pSocialManager;
    CLogHelper*                 m_logHelper;
	CEventManager*				m_EventManager;

private:
	SERVER_CLASS_NAME();

	bool InstallBreakHandlers();
	bool InitializeDatabase();

	static void OnBreak(int);

	static void executeCmd( const char* cmd );

public:
	~SERVER_CLASS_NAME();

	static SERVER_CLASS_NAME	*GetInstance()
	{
		if(!m_pInstance)
			m_pInstance = new SERVER_CLASS_NAME;

		return m_pInstance;
	}

	dtServerSocket	*GetServerSocket()	{		return m_pServerService;}
	CPlayerManager  *GetPlayerManager()	{		return m_pPlayerManager;}
	CServerManager  *GetServerManager()	{		return m_pServerManager;}
	CChatManager	*GetChatManager()	{		return m_pChatManager;	}
	CTeamManager	*GetTeamManager()	{		return &m_teamManager;	}
	CEventSource	*GetPlayerEventSource() {	return &m_playerEvent;  }
	CSavePlayerJob	*GetSavePlayerJob()	{		return m_pSavePlayerJob;}
	CMailManager	*GetMailManager()	{		return m_pMailManager;	}
	CCopyMapManager	*GetCopyMapManager(){		return m_pCopyMapManager;}
	CSocialManager	*GetSocialManager()	{		return m_pSocialManager;}
	CEventManager*	GetEventManager() {			return m_EventManager; }
	CTopManager*	GetTopManager()		{		return m_pTopManager; }

    CLogHelper*      GetLog(void)     
    {       
        if (0 != m_logHelper)
            return m_logHelper;
    
        static CLogHelper dummy;
        return &dummy;
    }

    void Log(dbStruct& log);

	static CDBConnPool		*GetDBConnPool()	    {		return m_pDBConnPool;	}
    static CDBConnPool      *GetAccountDBConnPool() {       return m_pAccountDBConnPool;}
    static CDBConnPool      *GetLogDBConnPool()     {       return m_pLogDBConnPool;}
	static CWorkQueue		*GetWorkQueue()		    {		return m_pWorkQueue;	}
	static int				EventProcess(LPVOID Param);

	bool Initialize();
	void StartService();
	void StopService();
	void DenyService();
	void Maintenance();

	void onCommand( const char* cmd, int size );

	VirtualConsole				m_Console;


	int currentAreaId;
	// ²âÊÔÓÃ------------------------------------------
	int nTestChannel;

	void OnServerStarted();
	void ShowMessage();

};

#define printf(p, ... ) SERVER->m_Console.printf( p, __VA_ARGS__ )

#endif
