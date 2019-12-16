// AccountServer.cpp : 定义控制台应用程序的入口点。
//

#include <signal.h>
#include "DBLib/dbPool.h"
#include "Common/Script.h"
#include "Common/CommonServer.h"
#include "Common/CommonClient.h"
#include "Common/WorkQueue.h"
#include "Common/TimerMgr.h"

#include "AccountServer.h"
#include "UserPacketProcess.h"
#include "WorldClientPacketProcess.h"
#include "EventProcess.h"

#include "PlayerMgr.h"

SERVER_CLASS_NAME *	SERVER_CLASS_NAME::m_pInstance		=	NULL;
HANDLE				SERVER_CLASS_NAME::m_ExitEvent		=	NULL;
CScript *			SERVER_CLASS_NAME::m_pScript		=	NULL;
CDBConnPool *		SERVER_CLASS_NAME::m_pDBConnPool	=	NULL;
CWorkQueue *		SERVER_CLASS_NAME::m_pWorkQueue		=	NULL;


const char GLOBAL_EXIT_EVENT[] = "flt_accountserver";

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

SERVER_CLASS_NAME::SERVER_CLASS_NAME()
{
	m_Inited			=	false;
	m_pUserService		=	NULL;
	m_pDBConnPool		=	NULL;
	m_pScript			=	NULL;
	m_pWorkQueue		=	NULL;
	m_pWorldClient		=	NULL;
	m_pPlayerManager	=	NULL;
	m_pTimeMgr			=	NULL;

	m_StartEvent		= INVALID_HANDLE_VALUE;

	ISocket::InitNetLib();
}

SERVER_CLASS_NAME::~SERVER_CLASS_NAME()
{
	if(m_pUserService)			delete m_pUserService;
	if(m_pDBConnPool)			delete m_pDBConnPool;
	if(m_pScript)				delete m_pScript;
	if(m_pWorkQueue)			delete m_pWorkQueue;
	if(m_pTimeMgr)				delete m_pTimeMgr;
	if(m_pPlayerManager)		delete m_pPlayerManager;
	if(m_pWorldClient)			delete m_pWorldClient;

	m_pInstance = NULL;
	ISocket::UninitNetLib();
}

void SERVER_CLASS_NAME::OnBreak(int)
{
	if(SERVER_CLASS_NAME::m_ExitEvent)
		SetEvent(SERVER_CLASS_NAME::m_ExitEvent);
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

void SERVER_CLASS_NAME::ShowMessage()
{
	printf("**********************************************************\n");
	printf("*\tUserCount\t\t\t%d\n", SCRIPT->get<int>("UserCount"));
	printf("*\tMaxUserCount\t\t\t%d\n", SCRIPT->get<int>("MaxUserCount"));
	printf("*\tUserNetIP\t\t\t%s:%d\n", SCRIPT->get<CStr>("UserNetIP"), SCRIPT->get<int>("UserNetPort"));		
	printf("*\tWorldServerIP\t\t\t%s:%d\n", SCRIPT->get<CStr>("WorldServerIP"), SCRIPT->get<int>("WorldServerPort"));		
	printf("*\tMaxDBConnection\t\t\t%d\n", SCRIPT->get<int>("MaxDBConnection"));
	printf("*\tDB_Server\t\t\t%s\n", SCRIPT->get<CStr>("DB_Server"));
	printf("*\tDB_Name\t\t\t\t%s\n", SCRIPT->get<CStr>("DB_Name"));
	printf("*\tDB_UserId\t\t\t%s\n", SCRIPT->get<CStr>("DB_UserId"));
	printf("*\tDB_Password\t\t\t%s\n", SCRIPT->get<CStr>("DB_Password"));		
	printf("**********************************************************\n");
}

void SERVER_CLASS_NAME::OnServerStarted()
{
	printf("*****************帐号服务已启动****************************\n");
}

bool SERVER_CLASS_NAME::InitializeDatabase()
{
	if(!m_pScript)
		return false;

	m_pDBConnPool = new CDBConnPool;

	int nCount = SCRIPT->get<int>("MaxDBConnection");
	const char* szServer = SCRIPT->get<CStr>("DB_Server");
	const char* szUserId = SCRIPT->get<CStr>("DB_UserId");
	const char* szPassword = SCRIPT->get<CStr>("DB_Password");
	const char* szDBName = SCRIPT->get<CStr>("DB_Name");

	return m_pDBConnPool->Open(nCount
		,szServer
		,szUserId
		,szPassword
		,szDBName

		);
}

bool isEnd;

bool SERVER_CLASS_NAME::Initialize()
{
	if(m_Inited)
		return true;

	if(!InstallBreakHandlers())
		return false;

	//设置日志文件名
	g_Log.SetLogName("AccountLog");

	//执行初始化脚本
	m_pScript = CScript::GetInstance();
	m_pScript->Initialize();
	SCRIPT->loadFile(".\\AccountServer.c");

	//if(!SCRIPT->check())
	//	return false;

	ShowMessage();

	//初始化数据库连接
	if(!InitializeDatabase())		//用于模拟帐号验证平台
		return false;

	//初始化工作队列
	m_pWorkQueue = new CWorkQueue;
	m_pWorkQueue->Initialize(EventProcess,10);

	m_pTimeMgr = new CTimerMgr(isEnd);
	m_pPlayerManager = new CPlayerManager;
	m_pTimeMgr->AddObject(m_pPlayerManager);

	//消息处理绑定
	EventFn::Initialize();

	stServerParam Param;
	//开放给用户的服务
	m_pUserService		=	new CommonServer<UserPacketProcess>;  //CommonServer<>(Create_UserPacketProcess);
	m_pUserService->SetConnectionTimeout(60*5);						//5分钟连接超时
	Param.MinClients	=	SCRIPT->get<int>("UserCount");
	Param.MaxClients	=	SCRIPT->get<int>("MaxUserCount");
	m_pUserService->Initialize(SCRIPT->get<CStr>("UserNetIP"),SCRIPT->get<int>("UserNetPort"),&Param);

	//连接世界服务器
	m_pWorldClient		=	new CommonClient<WorldClientPacketProcess>;	//CommonClient<>(Create_WorldClientPacketProcess)
	m_pWorldClient->Initialize(SCRIPT->get<CStr>("WorldServerIP"),SCRIPT->get<int>("WorldServerPort"));
	m_pWorldClient->SetConnectType(ISocket::SERVER_CONNECT);
   
	m_Inited	= true;
	return m_Inited;
}

void SERVER_CLASS_NAME::StartService()
{
	if(m_pWorldClient)		m_pWorldClient->Start();
}

void SERVER_CLASS_NAME::OnServerStart()
{
	if(m_pUserService)
	{
		if(m_pUserService->GetState() == ISocket::SSF_DEAD)
		{
			m_pUserService->Start();
			OnServerStarted();
			m_StartEvent = CreateEventA( NULL, FALSE, FALSE, "fl_accountserver" );

		}
	}
}

void SERVER_CLASS_NAME::Maintenance()
{
	while(1)
	{
		if(WaitForSingleObject(m_ExitEvent,2000) == WAIT_OBJECT_0)
			break;

		if(m_pWorldClient)
		{
			if(m_pWorldClient->GetState() == ISocket::SSF_DEAD)
			{
				m_pWorldClient->Restart();
			}
		}
	}
}

void SERVER_CLASS_NAME::DenyService()
{

}

void SERVER_CLASS_NAME::StopService()
{
	if(m_pUserService)		m_pUserService->Stop();
	if(m_pWorldClient)		m_pWorldClient->Stop();
	if(m_pWorkQueue)		m_pWorkQueue->Stop();

	if( m_StartEvent != INVALID_HANDLE_VALUE )
		CloseHandle( m_StartEvent );

	ResetEvent(m_ExitEvent);
}








