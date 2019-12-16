// AccountServer.cpp : �������̨Ӧ�ó������ڵ㡣
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

void		*m_pdbHandle;					//���ݿ���
char		m_strSvr[256];						//��������
char		m_strUser[256];						//�û���
char		m_strPwd[256];						//����
char		m_strDB[256];						//���ݿ�

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