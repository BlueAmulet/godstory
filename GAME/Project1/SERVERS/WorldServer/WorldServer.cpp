// WorldServer.cpp : 定义控制台应用程序的入口点。
//

#include <signal.h>
#include "DBLib/dbPool.h"
//#include "Common/Script.h"
#include "Common/CommonServer.h"
#include "Common/CommonClient.h"
#include "Common/WorkQueue.h"
#include "Common/TimerMgr.h"
#include "Common\LogHelper.h"
#include "EventManager.h"

#include "TeamManager.h"
#include "ChatMgr.h"
#include "PlayerMgr.h"
#include "ServerMgr.h"
#include "WorldServer.h"
#include "UserPacketProcess.h"
#include "EventProcess.h"
#include "CopymapManager.h"

#include "SavePlayerJob.h"
#include "MailManager.h"

#include "ExportScript.h"
#include "gm/GMRecver.h"

#include "BanList.h"

#include "SocialManager.h"
#include "TopManager.h"

SERVER_CLASS_NAME *	SERVER_CLASS_NAME::m_pInstance		    =	NULL;
HANDLE				SERVER_CLASS_NAME::m_ExitEvent		    =	NULL;
CScript *			SERVER_CLASS_NAME::m_pScript		    =	NULL;
CDBConnPool *		SERVER_CLASS_NAME::m_pDBConnPool	    =	NULL;
CDBConnPool*        SERVER_CLASS_NAME::m_pAccountDBConnPool =   NULL;
CDBConnPool*        SERVER_CLASS_NAME::m_pLogDBConnPool     =   NULL;
CWorkQueue *		SERVER_CLASS_NAME::m_pWorkQueue		    =	NULL;

const char GLOBAL_EXIT_EVENT[] = "flt_worldserver";

bool isEnd = false;

SERVER_CLASS_NAME::SERVER_CLASS_NAME()
{
	m_Inited			=	false;

	m_pServerService	=	NULL;
	m_pDBConnPool		=	NULL;
    m_pAccountDBConnPool=   NULL;
    m_pLogDBConnPool    =   NULL;
	m_pWorkQueue		=	NULL;
	m_pTimeMgr			=	NULL;
	m_pPlayerManager	=	NULL;
	m_pServerManager	=	NULL;
	m_pTimeMgr			=	NULL;
	m_pChatManager		=	NULL;
	m_pSaveTimeMgr		=	NULL;
	m_pMailManager		=	NULL;
	m_pSavePlayerJob	=	NULL;
	m_pCopyMapManager	=	NULL;
	m_pSocialManager	=	NULL;
    m_logHelper         =   NULL;
	m_EventManager		=	NULL;
	m_pTopManager		=	NULL;

	ISocket::InitNetLib();

	__hook( &VirtualConsole::onInput, &m_Console, &SERVER_CLASS_NAME::onCommand );
}

SERVER_CLASS_NAME::~SERVER_CLASS_NAME()
{
	if(m_pTopManager)			delete m_pTopManager;
	if(m_pServerService)		delete m_pServerService;
	if(m_pDBConnPool)			delete m_pDBConnPool;
    if(m_pAccountDBConnPool)    delete m_pAccountDBConnPool;
    if(m_pLogDBConnPool)        delete m_pLogDBConnPool;
	if(m_pWorkQueue)			delete m_pWorkQueue;
	if(m_pTimeMgr)				delete m_pTimeMgr;
	if(m_pPlayerManager)		delete m_pPlayerManager;
	if(m_pServerManager)		delete m_pServerManager;
	if(m_pChatManager)			delete m_pChatManager;
	if(m_pSavePlayerJob)		delete m_pSavePlayerJob;
	if(m_pSaveTimeMgr)			delete m_pSaveTimeMgr;
	if(m_pMailManager)			delete m_pMailManager;
	if(m_pCopyMapManager)		delete m_pCopyMapManager;
	if(m_pSocialManager)		delete m_pSocialManager;
    if(m_logHelper)             delete m_logHelper;
	if(m_EventManager)			delete m_EventManager;

	m_pInstance = NULL;
	ISocket::UninitNetLib();
}

void SERVER_CLASS_NAME::OnBreak(int)
{
	if(SERVER_CLASS_NAME::m_ExitEvent)
		SetEvent(SERVER_CLASS_NAME::m_ExitEvent);
}

const char* g_genExitEventName(void)
{
    char path[256] = {0};
    GetModuleFileName(0,path,sizeof(path));

    static std::string eventName;

    eventName = "e_";

    char buf[256] = {0};
    _splitpath(path,0,0,buf,0);
    eventName += buf;

    itoa(GetCurrentProcessId(),buf,10);
    eventName += buf;

    return eventName.c_str();
}

void SERVER_CLASS_NAME::ShowMessage()
{
	printf("**********************************************************\n");
	printf("*\tRemoteCount\t\t\t%d\n",  SCRIPT->get<int>("RemoteCount"));
	printf("*\tMaxRemoteCount\t\t\t%d\n", SCRIPT->get<int>("MaxRemoteCount"));
	printf("*\tRemoteNetIP\t\t\t%s:%d\n", SCRIPT->get<CStr>("RemoteNetIP"), SCRIPT->get<int>("RemoteNetPort"));
	printf("*\tMaxDBConnection\t\t\t%d\n",  SCRIPT->get<int>("MaxDBConnection"));
	printf("*\tDB_Server\t\t\t%s\n", SCRIPT->get<CStr>("DB_Server"));
	printf("*\tDB_Name\t\t\t\t%s\n", SCRIPT->get<CStr>("DB_Name"));
	printf("*\tDB_UserID\t\t\t%s\n", SCRIPT->get<CStr>("DB_UserId"));
	printf("*\tDB_Password\t\t\t%s\n", SCRIPT->get<CStr>("DB_Password"));
	printf("**********************************************************\n");
}

void SERVER_CLASS_NAME::OnServerStarted()
{
	printf("*****************世界服务已启动****************************\n");
}

bool SERVER_CLASS_NAME::InstallBreakHandlers()
{
    signal(SIGBREAK, OnBreak);
    signal(SIGINT, OnBreak);

    m_ExitEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,g_genExitEventName());

    if(m_ExitEvent)
    {
        return true; 
    }

    m_ExitEvent = CreateEvent(NULL,TRUE,FALSE,g_genExitEventName());
    return true;
}

void *Create_UserPacketProcess()
{
	return new UserPacketProcess;
}

bool SERVER_CLASS_NAME::InitializeDatabase()
{
	m_pDBConnPool = new CDBConnPool;

	int nCount = CSCRIPT->get<int>("MaxDBConnection");

	const char* szServer = CSCRIPT->get<CStr>("DB_Server");
	const char* szUserId = CSCRIPT->get<CStr>("DB_UserId");
	const char* szPassword = CSCRIPT->get<CStr>("DB_Password");
	const char* szDBName = CSCRIPT->get<CStr>("DB_Name");

	if(!m_pDBConnPool->Open(nCount
		,szServer
		,szUserId
		,szPassword
		,szDBName
		))
    {
        return false;
    }

    m_pLogDBConnPool = new CDBConnPool;

    szServer   = CSCRIPT->get<CStr>("DB_LogServer");
    szUserId   = CSCRIPT->get<CStr>("DB_LogUserId");
    szPassword = CSCRIPT->get<CStr>("DB_LogPassword");
    szDBName   = CSCRIPT->get<CStr>("DB_LogName");

    m_pLogDBConnPool->Open(nCount,szServer,szUserId,szPassword,szDBName);

    m_pAccountDBConnPool = new CDBConnPool;

    szServer   = CSCRIPT->get<CStr>("DB_AccountServer");
    szUserId   = CSCRIPT->get<CStr>("DB_AccountUserId");
    szPassword = CSCRIPT->get<CStr>("DB_AccountPassword");
    szDBName   = CSCRIPT->get<CStr>("DB_AccountName");

    return m_pAccountDBConnPool->Open(nCount,szServer,szUserId,szPassword,szDBName);
}

bool SERVER_CLASS_NAME::Initialize()
{
	if(m_Inited)
		return true;

	if(!InstallBreakHandlers())
		return false;

	//设置日志文件名
	g_Log.SetLogName("WorldLog");

	////执行初始化脚本
	//m_pScript = CScript::GetInstance();
	//m_pScript->Initialize();
	CSCRIPT->loadFile( ".\\WorldServer.c" );

	ShowMessage();

	//初始化数据库连接
	if(!InitializeDatabase())
		return false;

	m_pSavePlayerJob = new CSavePlayerJob;

	//初始化工作队列
	m_pWorkQueue = new CWorkQueue;
	m_pWorkQueue->Initialize(EventProcess,1);



	m_pTimeMgr = new CTimerMgr(isEnd);
	m_pPlayerManager = new CPlayerManager;

	m_pTimeMgr->AddObject(m_pPlayerManager);

	m_pSaveTimeMgr = new CTimerMgr( isEnd, 1 );
	m_pSaveTimeMgr->AddObject( m_pSavePlayerJob );

	m_pServerManager = new CServerManager;
	m_pServerManager->Initialize();

	m_pChatManager = new CChatManager;
	m_pChatManager->Initialize();

	m_pCopyMapManager = new CCopyMapManager();
	m_pCopyMapManager->Initialize();

	m_pMailManager = new CMailManager();

	m_pTopManager = new CTopManager();

	m_pSocialManager = new CSocialManager();

	m_EventManager = new CEventManager();

    m_logHelper = new CLogHelper;
    m_logHelper->connect(CSCRIPT->get<CStr>("LogServerIP"),CSCRIPT->get<int>("LogServerPort"));

	currentAreaId = CSCRIPT->get<int>("AreaId");

	//消息处理绑定
	EventFn::Initialize();

	stServerParam Param;
	//开放给帐号服务器和网关服务器的服务
	m_pServerService	=	new CommonServer<UserPacketProcess>;  //CommonServer<>(Create_UserPacketProcess);
	Param.MinClients	=	CSCRIPT->get<int>("RemoteCount");
	Param.MaxClients	=	CSCRIPT->get<int>("MaxRemoteCount");
	m_pServerService->Initialize(CSCRIPT->get<CStr>("RemoteNetIP"),CSCRIPT->get<int>("RemoteNetPort"),&Param);

	// 测试用默认建立一个虚拟的队伍频道
	CChannel* pChannel = GetChatManager()->GetChannelManager()->RegisterChannel( CHAT_MSG_TYPE_REALCITY, "杭州" );
	nTestChannel = pChannel->GetId();

	// 初始化全局玩家事件
	// 这些对象都要处理玩家断线的事件
	// 暂时不用这种机制
	//m_playerEvent.RegisterEventListener( GetChatManager()->GetChannelManager() );
	//m_playerEvent.RegisterEventListener( GetTeamManager() );

    //监听GM端口
    CGMRecver::Instance()->Initialize();
    
    CBanlist::Instance()->Init();
	m_Inited	= true;

	return m_Inited;
}

void SERVER_CLASS_NAME::StartService()
{
	if(m_pServerService)	m_pServerService->Start();

	OnServerStarted();

	m_StartEvent = CreateEventA( NULL, FALSE, FALSE, "fl_worldserver" );
}

void SERVER_CLASS_NAME::Maintenance()
{
    //WaitForSingleObject(m_ExitEvent,INFINITE);

    while(1)
    {
        if(WaitForSingleObject(m_ExitEvent,2000) == WAIT_OBJECT_0)
            break;

        m_logHelper->check();
            
        //SERVER->GetChatManager()->Update();
    }
}

void SERVER_CLASS_NAME::DenyService()
{

}

void SERVER_CLASS_NAME::StopService()
{
	g_Log.WriteLog( "WorldServer 正常退出 ... " );

    //m_pTimeMgr = new CTimerMgr(m_ExitEvent);
    //m_pPlayerManager = new CPlayerManager;

    //m_pTimeMgr->AddObject(m_pPlayerManager);

    //m_pSaveTimeMgr = new CTimerMgr( m_ExitEvent, 1 );
    //m_pSaveTimeMgr->AddObject( m_pSavePlayerJob );

    if( m_pSaveTimeMgr && m_pSavePlayerJob )
        m_pSaveTimeMgr->RemoveObject( m_pSavePlayerJob );

    if( m_pTimeMgr && m_pPlayerManager )
        m_pTimeMgr->RemoveObject( m_pPlayerManager );

    if(m_logHelper)         m_logHelper->disconnect();
	if(m_pServerService)	m_pServerService->Stop();
	if(m_pWorkQueue)		m_pWorkQueue->Stop();

	CloseHandle( m_StartEvent );
	ResetEvent(m_ExitEvent);
}

void SERVER_CLASS_NAME::Log(dbStruct& log)
{
    if (0 == m_logHelper)
        return;

    m_logHelper->writeLog(&log);
}

void SERVER_CLASS_NAME::onCommand( const char* cmd, int size )
{
	GetWorkQueue()->PostEvent( 0, (void*)cmd, size, true, WQ_COMMAND );
}




