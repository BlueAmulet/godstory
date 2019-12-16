#include "BackgroundLoad.h"
#include <assert.h>
#include <Windows.h>

CBackgroundLoad::CBackgroundLoad(void):
	m_thread(0),
	m_status(IDLE),
	m_isSuccess(0)
{

}

CBackgroundLoad::~CBackgroundLoad(void)
{
	Finish();
}

void CBackgroundLoad::BackgroundLoad(const char* name)
{
	if (0 == name)
		name = "";

	Finish();

	assert(IDLE == m_status && "invalid background load status");

	m_loadFileName = name;
	m_status       = LOADING;

	//创建线程
	if (0 == m_thread)
		m_thread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)__WorkThread,this,0,0);
}

CBackgroundLoad::LoadStatus CBackgroundLoad::GetStatus(void)
{
	if (LOADING    == m_status ||
		CANCELLING == m_status ||
		PAUSING    == m_status)
	{
		if (m_semaphore.acquire(false))
		{
			__Clean();
		}
	}

	return m_status;
}

bool CBackgroundLoad::Finish(void)
{
	if (PAUSED == m_status)
	{
		if (0 != m_thread)
			ResumeThread(m_thread);

		m_status = LOADING;
	}

	if (LOADING    == m_status ||
		CANCELLING == m_status)
	{
		m_semaphore.acquire();
		__Clean();
	}

	return m_isSuccess;
}

void CBackgroundLoad::Cancel(void)
{
	if (PAUSED == m_status)
	{
		m_status = CANCELLING;

		if (0 != m_thread)
			ResumeThread(m_thread);
	}
	
	if (LOADING == m_status)
	{
		m_status = CANCELLING;
	}
}

void CBackgroundLoad::Pause(void)
{
	if (LOADING == m_status)
	{
		m_status = PAUSING;
	}
}

void CBackgroundLoad::Resume(void)
{
	if (PAUSED == m_status)
	{
		if (0 != m_thread)
			ResumeThread(m_thread);

		m_status = LOADING;
	}
}

unsigned long CBackgroundLoad::__WorkThread(void* pThis)
{
	CBackgroundLoad* parent = static_cast<CBackgroundLoad*>(pThis);
	parent->__OnBackgroundLoad();

	return 0;
}

void CBackgroundLoad::__OnBackgroundLoad(void)
{
	//调用子类的载入方法
	try 
	{
		m_isSuccess = OnLoad(m_loadFileName.c_str());
	}
	catch(...)
	{
		m_isSuccess = false;
	}

	if (CANCELLING == m_status)
	{
		m_isSuccess = false;
		m_semaphore.release();
		return;
	}

	if (PAUSING == m_status)
	{
		m_status = PAUSED;

		if (0 != m_thread)
			SuspendThread(m_thread);
	}

	//调用结束函数,注意这里是异步的
	OnLoadComplete(m_isSuccess);
	m_semaphore.release();
}

void CBackgroundLoad::__Clean(void)
{
	if (0 != m_thread)
	{
		WaitForSingleObject(m_thread,INFINITE);
		CloseHandle(m_thread);
	}

	m_thread = 0;
	m_status = IDLE;
}

