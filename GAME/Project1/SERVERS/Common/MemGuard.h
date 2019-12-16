#ifndef MEM_GUARD_H
#define MEM_GUARD_H

#include "base/memPool.h"

class CMemGuard
{
	int m_size;
	MemPoolEntry m_pBuffer;
public:
	CMemGuard(int Size)
	{
		m_size = Size;
		m_pBuffer = CMemPool::GetInstance()->Alloc(Size);
	}

	~CMemGuard()
	{
		CMemPool::GetInstance()->Free(m_pBuffer);
	}

	char *get()
	{
		return  reinterpret_cast<char*>(m_pBuffer);
	}

	int size()
	{
		return m_size;
	}
};

#endif

