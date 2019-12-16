#include "Locker.h"
#include <hash_map>

class CCriticalSectionMap
{
	typedef stdext::hash_map< void*, CMyCriticalSection* > csMap;
	CMyCriticalSection m_cs;
	csMap mCSMap;

public:
	virtual ~CCriticalSectionMap()
	{
		for(csMap::iterator it = mCSMap.begin(); it != mCSMap.end(); ++it)
		{
			delete it->second;
		}
	}

	static CCriticalSectionMap* get();

	CMyCriticalSection& getCS( void* key )
	{
		m_cs.Lock();

		CMyCriticalSection* pCS = NULL;
		csMap::iterator it = mCSMap.find( key );
		if( it == mCSMap.end() )
		{
			pCS = new CMyCriticalSection();
			mCSMap[key] = pCS;

			m_cs.Unlock();
			return *pCS;
		}
		pCS = it->second;

		m_cs.Unlock();
		return *pCS;
	}
};

static CCriticalSectionMap cs_map;
CCriticalSectionMap* CCriticalSectionMap::get()
{		
	return &cs_map;
}


CLocker::CLocker( void* key, bool bInitialLock /*= true*/ )
{
	CLockableObject& Lockable = CCriticalSectionMap::get()->getCS( key );
	m_bIsLocked = false;
	m_pLockable = &Lockable;
	if (bInitialLock)
	{
		m_pLockable->Lock();
		m_bIsLocked = true;
	}
}

CLocker::CLocker( CLockableObject* pLockable, bool bInitialLock /*= true*/ )
{
	m_bIsLocked = false;
	m_pLockable = pLockable;
	if (bInitialLock)
	{
		m_pLockable->Lock();
		m_bIsLocked = true;
	}
}

CLocker::CLocker( CLockableObject& Lockable, bool bInitialLock /*= true*/ )
{
	m_bIsLocked = false;
	m_pLockable = &Lockable;
	if (bInitialLock)
	{
		m_pLockable->Lock();
		m_bIsLocked = true;
	}
}

CMyCriticalSection::CMyCriticalSection()
{
	InitializeCriticalSection(&m_CritSect);
}

CMyCriticalSection::~CMyCriticalSection()
{
	DeleteCriticalSection(&m_CritSect);
}

void CMyCriticalSection::Lock()
{
	EnterCriticalSection(&m_CritSect);
}

void CMyCriticalSection::Unlock()
{
	LeaveCriticalSection(&m_CritSect);
}