#include <process.h>
#include "TimerMgr.h"

unsigned int  WINAPI CTimerMgr::TimeRoutine(LPVOID pParam)
{
	CTimerMgr *pMgr = (CTimerMgr*)pParam;

	while( true )
	{
		Sleep( pMgr->m_nTime );

		if( !pMgr->TimeProcess( false ) )
			break;
	}

	return 0;
}

CTimerMgr::CTimerMgr(bool& isEnd, int nTime) : mIsEnd( isEnd )
{
	unsigned int threadID;
	m_nTime = nTime;
	m_hTimeThread = (HANDLE)_beginthreadex(NULL, 0, TimeRoutine, this, 0, &threadID);
}

CTimerMgr::~CTimerMgr()
{
    TerminateThread( m_hTimeThread, 0 );
	CloseHandle(m_hTimeThread);
}

bool CTimerMgr::TimeProcess(bool bExit)
{
	CLocker Lock(m_TimeCS);
 
	stdext::hash_map<DWORD,CTimeCriticalObject *>::iterator itb = m_List.begin();
	stdext::hash_map<DWORD,CTimeCriticalObject *>::iterator ite = m_List.end();

	bool bRet = true;
	for(;itb!=ite;itb++){
		if( !itb->second->TimeProcess(bExit) )
			bRet = false;
	}

	return bRet;
}

void CTimerMgr::AddObject(CTimeCriticalObject *pObject)
{
	CLocker Lock(m_TimeCS);

	m_List[(DWORD)pObject] = pObject;
}

void CTimerMgr::RemoveObject(CTimeCriticalObject *pObject)
{
	CLocker Lock(m_TimeCS);

	stdext::hash_map<DWORD,CTimeCriticalObject *>::iterator it = m_List.find((DWORD)pObject);
	if(it != m_List.end()){
		m_List.erase(it);
	}
}

