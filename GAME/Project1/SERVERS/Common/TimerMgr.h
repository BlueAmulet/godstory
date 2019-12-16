#ifndef _TIMER_MGR_H
#define _TIMER_MGR_H
#include "Base/Locker.h"
#include <hash_map>

class CTimeCriticalObject
{
public:
	virtual bool TimeProcess(bool bExit)=0;

	virtual ~CTimeCriticalObject(){};
};


class CTimerMgr
{
	CMyCriticalSection		m_TimeCS;
	HANDLE m_hTimeThread;
	stdext::hash_map<DWORD,CTimeCriticalObject *> m_List;

	static unsigned int  WINAPI TimeRoutine(LPVOID pParam);

public:
	HANDLE m_Exit;
public:
	CTimerMgr(bool& isEnd, int nTime = 1000);
	~CTimerMgr();

	bool TimeProcess(bool bExit);

	void AddObject(CTimeCriticalObject *pObject);

	void RemoveObject(CTimeCriticalObject *pObject);

private:
	int m_nTime;
    bool& mIsEnd;
};

#endif