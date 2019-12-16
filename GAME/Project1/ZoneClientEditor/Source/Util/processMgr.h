#ifndef PROCESS_MGR_H
#define PROCESS_MGR_H

#include <process.h>
#include "base/locker.h"
#include "base/memPool.h"
#include "platform/threads/mutex.h"
#include "platform/threads/semaphore.h"

#include <vector>
#include <stack>

#pragma once

enum WORKQUEUE_MESSGAE
{
	WQ_PACKET,
	WQ_QUIT,
};

#define MAX_THREAD_NUM 4

typedef void (*ThreadCallback)(void *pParam);

struct stThreadParam : public CVirtualAllocBase
{
	U8 type;
	void *pShapeIns;
	void *pObj;
	void *pParam;
	ThreadCallback pFunc;

	stThreadParam()
	{
		dMemset(this,0,sizeof(stThreadParam));
	}
};

enum typelist
{
	ParticleType,
	RibbonType,
	MeshType,
};

class CMultThreadProcess
{
	int						m_ThreadNum;
	bool					m_Stop;
public:
	volatile  long			mItemNum;

public:
	static void init();
	static void shutdown();

	bool initilize(int num=0);

	bool start();
	bool addWorkItem(stThreadParam *);
	bool end();
	bool exit();
	bool isend();

	void handleZombifyEvent  (U32 did);
	
private:
    static unsigned int WINAPI __WorkRoutine(LPVOID Param);

    stThreadParam* __Dequeue(void);

    //syn to thread items
    CCSLock     m_lock;

    //awake threads
    Semaphore   m_semaphore;

    //thread pool
    std::vector<HANDLE> m_threadPool;

    //work items pool
    std::stack<stThreadParam*> m_workItemPool;
};

extern CMultThreadProcess *g_CurrentThreadProcess;
extern CMultThreadProcess *g_ThreadProcess1;
extern CMultThreadProcess *g_ThreadProcess2;

#endif