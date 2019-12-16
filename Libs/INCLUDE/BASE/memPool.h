#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

#include "Base/Types.h"
#include "TCMalloc/TCMallocInclude.h"

#define _1M		1048576
#define _1K		1024
#define _1B		1


#define MEMPOOL_ALLOC malloc
#define MEMPOOL_FREE free

typedef U8* MemPoolEntry;

class CMemPool
{
	CMemPool();

	static CMemPool *m_pInstance;

public:
	~CMemPool();

	static CMemPool *GetInstance()
	{
		if(!m_pInstance)
			m_pInstance = new CMemPool;
			
		return m_pInstance;
	}

	MemPoolEntry Alloc(U32 Size);
	void Free(MemPoolEntry);

	template <class T>
	__inline void AllocObj(T *&pObj)
	{
		pObj = (T *)Alloc(sizeof(T));
		constructInPlace<T>(pObj);
	}

	template <class T>
	__inline void FreeObj(T *&pObj)
	{
		destructInPlace<T>(pObj);
		Free((MemPoolEntry)pObj);
		pObj = NULL;
	}
};

__inline MemPoolEntry CMemPool::Alloc(U32 Size)
{
	void* p = MEMPOOL_ALLOC(Size);
	return (MemPoolEntry)p;
}

__inline void CMemPool::Free(MemPoolEntry p)
{
	MEMPOOL_FREE(p);
	p = NULL;
}

#endif