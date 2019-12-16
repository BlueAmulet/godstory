#include "dbPool.h"
#include <time.h>


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CDBConn
CDBConn::CDBConn(CDBConnPool *pool)
{
	m_pPool = pool;
}

CDBConn::CDBConn()
{
	m_pPool = NULL;
}

CDBConn::~CDBConn()
{
}

void CDBConn::Detach()
{
	if(m_pPool)
	{
		m_pPool->Unlock(this);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CDBPool

CDBConnPool::CDBConnPool()
{
}

CDBConnPool::~CDBConnPool()
{
    Close();
}

bool CDBConnPool::Open(int nConnCount,const char * svr, const char * usr, const char * pwd, const char * db)
{
	if(!nConnCount)
		return false;

	m_nConnCount = nConnCount;

	strcpy(m_Server,svr);
	strcpy(m_UserName,usr);
	strcpy(m_Password,pwd);
	strcpy(m_Database,db);

	try
	{
		CDBConn *pObj;
		for(int i=0;i<m_nConnCount;i++)
		{
			pObj = new CDBConn(this);
			pObj->Open(m_Server,m_UserName,m_Password,m_Database);
			m_pFreeList.push_back(pObj);
		}
	}
	catch(CDBException &e)
	{
		Close();
	}
	catch(...)
	{
		Close();
		return false;
	}

    return true;
}

void CDBConnPool::Close()
{
	CDBConn *pObj;

	CLocker lock(m_csFreeList);

	while(m_pFreeList.size()>0)
    {
        pObj = m_pFreeList.front();
		delete pObj;
        m_pFreeList.pop_front();
    }
}

CDBConn * CDBConnPool::Lock()
{
	DWORD dwOneTimeout = 1000;	//1s;
	DWORD dwInfinite = 30;	//30s
	DWORD dwTotal=0;

	time_t id;
	time(&id);
	bool ShowMsg = false;

	CLocker lock(m_csFreeList,false);

	while(1)
	{
		lock.Lock();

		if(m_pFreeList.size() > 0) 
		{
			lock.Unlock();
			break;
		}

		if(!ShowMsg){
			char Msg[100];
			sprintf(Msg,"数据库连接池满,请求被等待.id=%d.........\n",(DWORD)id);
			ShowMsg = true;
		}

		lock.Unlock();

		Sleep(dwOneTimeout);

		dwTotal += (dwOneTimeout/1000);
		if(dwTotal > dwInfinite)
		{
			return NULL;
		}
	}

	if(ShowMsg){
		char Msg[100];
		sprintf(Msg,"id=%d 的数据库请求获得了连接.........\n",(DWORD)id);
	}

	//有未使用连接,从链表头摘包
	lock.Lock();
	CDBConn *pConn = m_pFreeList.front();
	m_pFreeList.pop_front();
	lock.Unlock();
	return pConn;
}

bool CDBConnPool::Unlock(CDBConn * pConn)
{
	CLocker lock(m_csFreeList);

	m_pFreeList.push_front(pConn);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CDBOperator

CDBOperator::CDBOperator(CDBConnPool *pPool)
{
	m_pObject = NULL;

	if(pPool)
	{
		m_pObject = pPool->Lock();
		if(!m_pObject)
		{
			throw CDBException(-1, -1, -1, "长时间无法获得数据库连接,数据库操作不能执行,被当作错误撤消", "", "pPool->Lock()", 0);
		}
	}
}

CDBOperator::~CDBOperator()
{
	if(m_pObject)
		m_pObject->Detach();
}

void ConvertHex(char *pBuff,const unsigned char *pData,int iSize)
{
	for(int i=0;i<iSize;i++)
	{
		if(pData[i] < 0x10)
			sprintf(pBuff,"%s0%X",pBuff,pData[i]);
		else
			sprintf(pBuff,"%s%X",pBuff,pData[i]);
	}
}

