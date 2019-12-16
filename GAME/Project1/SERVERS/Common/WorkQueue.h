#pragma once

#include <process.h>
#include "base/locker.h"
#include "base/memPool.h"

enum WORKQUEUE_MESSGAE
{
	WQ_CONNECT,
	WQ_DISCONNECT,
	WQ_PACKET,
	WQ_LOGIC,
	WQ_COMMAND,
    WQ_TIMER,
	WQ_QUIT,
};

typedef int (*WORK_QUEUE_FN)(LPVOID Param);

struct WorkQueueItemStruct
{
	int					Id;
	WORKQUEUE_MESSGAE	opCode;
	int					size;
	char *				Buffer;
};

class CWorkQueue
{
protected:
	HANDLE					m_QueueHandle;
	HANDLE					*m_ThreadHandle;
	int						m_ThreadNum;
	WORK_QUEUE_FN			m_Callback;
	bool					m_Stop;

public:
	CWorkQueue()
	{
		m_QueueHandle	= NULL;
		m_ThreadHandle	= NULL;
		m_Callback		= NULL;
		m_ThreadNum		= 0;
		m_Stop			= true;
	}

	~CWorkQueue()
	{
		if(!m_Stop)
			Stop();
	}

	bool Initialize(WORK_QUEUE_FN fn,int ThreadNum)
	{
		m_Callback = fn;
		if(!m_Callback)
			return false;

		m_QueueHandle = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)this, 0);
		if(!m_QueueHandle)
			return false;

		m_ThreadHandle = new HANDLE[ThreadNum];

		m_ThreadNum = ThreadNum;
		unsigned int threadID;
		for (int i=0; i < m_ThreadNum; i++ )
		{
			m_ThreadHandle[i] = (HANDLE)_beginthreadex( NULL, 0, WorkRoutine, this, 0, &threadID );
		}

		m_Stop = false;
		return true;
	}

	bool PostEvent(int Id,void *data,int size,bool CopyData=false,WORKQUEUE_MESSGAE QueueMsg=WQ_PACKET)
	{
		if(m_Stop || !CMemPool::GetInstance())
			return false;

		WorkQueueItemStruct *pStruct = (WorkQueueItemStruct *)CMemPool::GetInstance()->Alloc(sizeof(WorkQueueItemStruct));
		if(!pStruct)
		{
			if(!CopyData)
				CMemPool::GetInstance()->Free((MemPoolEntry)data);
			return false;
		}

		if(CopyData)
		{
			pStruct->Buffer = (char *)CMemPool::GetInstance()->Alloc(size);
			memcpy(pStruct->Buffer,data,size);
		}
		else
		{
			pStruct->Buffer = (char *)data;
		}

		pStruct->Id		= Id;
		pStruct->size	= size;
		pStruct->opCode = QueueMsg;

		PostQueuedCompletionStatus(m_QueueHandle,0,(int)QueueMsg,(LPOVERLAPPED)pStruct);
		return true;
	}

	void Stop()
	{
		m_Stop = true;

		for(int i=0;i<m_ThreadNum;i++)
			PostQueuedCompletionStatus(m_QueueHandle,0,(int)WQ_QUIT,NULL);

		WaitForMultipleObjectsEx(m_ThreadNum,m_ThreadHandle,TRUE,10000,FALSE);

		for(int i=0;i<m_ThreadNum;i++)
			CloseHandle(m_ThreadHandle[i]);
		delete [] m_ThreadHandle;

		CloseHandle(m_QueueHandle);
	}

	static unsigned int WINAPI WorkRoutine(LPVOID Param)
	{
		CWorkQueue *pQueue = (CWorkQueue*)Param;
		DWORD		dwByteCount=0,dwErrorCode=0,dwCode=0;
		WorkQueueItemStruct		*pData = NULL;

		while(1)
		{
			::GetQueuedCompletionStatus(pQueue->m_QueueHandle, &dwByteCount, (ULONG_PTR *)&dwCode,(LPOVERLAPPED*)&pData, INFINITE );
			if(dwCode == (int)WQ_QUIT)
				return 0;

			pQueue->m_Callback(pData);
			
			CMemPool::GetInstance()->Free((MemPoolEntry)pData->Buffer);
			CMemPool::GetInstance()->Free((MemPoolEntry)pData);
		}
	}
};


