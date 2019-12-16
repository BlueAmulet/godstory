#include <WinSock2.h>
#include <malloc.h>
#include "platform/EngineConfig.h"
#include "platform/types.h"
#include "util/processMgr.h"


CMultThreadProcess *g_CurrentThreadProcess = NULL;
CMultThreadProcess *g_ThreadProcess1 = NULL;
CMultThreadProcess *g_ThreadProcess2 = NULL;

void CMultThreadProcess::init()
{
#ifdef USE_MULTITHREAD_ANIMATE
	//if(!g_ThreadProcess1)
	//{
	//	g_ThreadProcess1 = new CMultThreadProcess;
	//	g_ThreadProcess1->initilize(1);
	//}

	if(!g_ThreadProcess2)
	{
		g_ThreadProcess2 = new CMultThreadProcess;
		g_ThreadProcess2->initilize();
	}

	g_ThreadProcess1 = g_ThreadProcess2;

	g_CurrentThreadProcess = g_ThreadProcess1;
#endif
}

void CMultThreadProcess::shutdown()
{
	if(g_CurrentThreadProcess)
	{
		g_CurrentThreadProcess = NULL;
	}

	if(g_ThreadProcess2)
	{
		g_ThreadProcess2->exit();
		delete g_ThreadProcess2;
	}
}

unsigned int WINAPI CMultThreadProcess::__WorkRoutine(LPVOID Param)
{
	CMultThreadProcess* parent = (CMultThreadProcess*)Param;
    
    while (!parent->m_Stop)
    {
        //wait for signal
        stThreadParam* pItem = parent->__Dequeue();

        if (0 != pItem)
        {
			try
			{
				pItem->pFunc(pItem->pParam);
				delete pItem;
			}
			catch(...)
			{
				AssertRelease(false, "CMultThreadProcess::__WorkRoutine dump");
			}
            InterlockedDecrement(&parent->mItemNum);
        }
    }

    return 0;
}

bool CMultThreadProcess::initilize(int threadnum)
{
    //TODO shutdown
    m_ThreadNum = threadnum;

	if(m_ThreadNum == 0)
	{
		SYSTEM_INFO		l_si;
		::GetSystemInfo( &l_si );
		m_ThreadNum	= l_si.dwNumberOfProcessors * 2;
	}

	if(m_ThreadNum > MAX_THREAD_NUM)
		m_ThreadNum = MAX_THREAD_NUM;

	unsigned int threadID;
    m_Stop = false;
    mItemNum = 0;

	for (int i=0; i < m_ThreadNum; i++ )
	{
        m_threadPool.push_back((HANDLE)_beginthreadex(0,0,&CMultThreadProcess::__WorkRoutine,this,0,&threadID));
	}

	return true;
}

bool CMultThreadProcess::start()
{
	mItemNum = 0;
	return true;
}

bool CMultThreadProcess::addWorkItem(stThreadParam* pParam)
{
	if(0 == pParam || m_Stop || !CMemPool::GetInstance())
		return false;
    
    InterlockedIncrement(&mItemNum);

    m_lock.Lock();
    m_workItemPool.push(pParam);
    m_lock.Unlock();

    //notify threads
    m_semaphore.release();
	
	return true;
}

stThreadParam* CMultThreadProcess::__Dequeue(void)
{
    m_semaphore.acquire();

    stThreadParam* pItem = 0;

    //get work items!
    m_lock.Lock();

    if (0 != m_workItemPool.size())
    {
        pItem = m_workItemPool.top();
        m_workItemPool.pop();
    }
    
    m_lock.Unlock();
    
    return pItem;
}

bool CMultThreadProcess::end()
{
    //wait for end
	while(true)
    {
        if (0 == InterlockedCompareExchange(&mItemNum,0,0))
            break;
    }

	return true;
}

bool CMultThreadProcess::isend()
{
	return m_workItemPool.size() == 0;
}

bool CMultThreadProcess::exit()
{
	m_Stop = true;

	//singal all threads
    for (size_t i = 0; i < 2 * m_threadPool.size(); ++i)
    {
        m_semaphore.release();
    }
    
    //wait for all thread to stopped
    for (size_t i = 0; i < m_threadPool.size(); ++i)
    {
        WaitForSingleObject(m_threadPool[i],10000);
        CloseHandle(m_threadPool[i]);
    }

    m_threadPool.clear();
	return true;
}

void CMultThreadProcess::handleZombifyEvent(U32 did)
{
	end();
}