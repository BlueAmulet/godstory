// AccountServer.cpp : 定义控制台应用程序的入口点。
//

#include <tchar.h>

#include "ChatMgr.h"
#include "TeamManager.h"
#include "WorldServer.h"
//#include "Common/Script.h"


int _tmain(int argc, _TCHAR* argv[])
{
	SERVER_CLASS_NAME *pServerInstance = SERVER_CLASS_NAME::GetInstance();
	if(!pServerInstance->Initialize())
	{
		delete pServerInstance;
		return 0;
	}

	pServerInstance->StartService();

	pServerInstance->Maintenance();

	pServerInstance->DenyService();

	pServerInstance->StopService();

	delete pServerInstance;
	


	return 0;
}

/*
#include "DBLib/dblib.h"

void		*m_pdbHandle;					//数据库句柄
char		m_strSvr[256];						//服务器名
char		m_strUser[256];						//用户名
char		m_strPwd[256];						//密码
char		m_strDB[256];						//数据库

int _tmain(int argc, _TCHAR* argv[])
{
	dbinit();

	strcpy_s(m_strSvr, 256, "192.168.1.119\\SQLEXPRESS");
	strcpy_s(m_strUser, 256, "hulingyun");
	strcpy_s(m_strPwd, 256, "hulingyun");
	strcpy_s(m_strDB, 256, "NewActor");


	PLOGINREC pLoginHandle = dblogin();
	if(pLoginHandle == (PLOGINREC)NULL)
	{
		return false;
	}

	DBSETLUSER(pLoginHandle, m_strUser);
	DBSETLPWD(pLoginHandle,m_strPwd);
	DBSETLVERSION(pLoginHandle, DBVER60);
	DBSETLAPP(pLoginHandle, "DBLib");

	unsigned long nSize = 255;
	char Buf[256];

	BCP_SETL(pLoginHandle, true);
	if(GetComputerName(Buf, &nSize))
		DBSETLHOST(pLoginHandle, Buf);
	else
		DBSETLHOST(pLoginHandle, "UNKNOW");

	strcpy(Buf, m_strSvr);
	m_pdbHandle=dbopen(pLoginHandle, (LPCSTR)Buf);

	dbfreelogin(pLoginHandle);
}

*/