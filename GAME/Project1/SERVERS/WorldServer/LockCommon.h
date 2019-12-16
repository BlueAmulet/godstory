#ifndef LOCK_COMMON_H
#define LOCK_COMMON_H

#include <bitset>

#include "base/Locker.h"
#include <hash_map>

struct ILockable
{
	CMyCriticalSection m_cs;
};

#define DECLARE_INDEX(p) Index##p = 1 << p,

class CGlobalLock
{
public:
	enum IndexType
	{
		DECLARE_INDEX( 0 )
		DECLARE_INDEX( 1 )
		DECLARE_INDEX( 2 )
		DECLARE_INDEX( 3 )
		DECLARE_INDEX( 4 )
		DECLARE_INDEX( 5 )
		DECLARE_INDEX( 6 )
		DECLARE_INDEX( 7 )
		DECLARE_INDEX( 8 )
		DECLARE_INDEX( 9 )
		DECLARE_INDEX( 10 )
		DECLARE_INDEX( 11 )
		DECLARE_INDEX( 12 )
		DECLARE_INDEX( 13 )
		DECLARE_INDEX( 14 )
		DECLARE_INDEX( 15 )
		DECLARE_INDEX( 16 )
		DECLARE_INDEX( 17 )
		DECLARE_INDEX( 18 )
		DECLARE_INDEX( 19 )
		DECLARE_INDEX( 20 )
		DECLARE_INDEX( 21 )
		DECLARE_INDEX( 22 )
		DECLARE_INDEX( 23 )
		DECLARE_INDEX( 24 )
		DECLARE_INDEX( 25 )
		DECLARE_INDEX( 26 )
		DECLARE_INDEX( 27 )
		DECLARE_INDEX( 28 )
		DECLARE_INDEX( 29 )
		DECLARE_INDEX( 30 )

		Count

	};

private:

	typedef stdext::hash_map< int, ILockable* > LockMap;
	LockMap mLockMap;

public:
	void registerLockable( ILockable* pLockable, IndexType index )
	{
		mLockMap[(int)index] = pLockable;
	}	

	void Lock( int lockMask )
	{
		int index;

		for( int i = 0; i < 31; i++ )
		{
			index = i < i;

			if( lockMask & index )
			{
				if( mLockMap[index] != NULL )
					mLockMap[index]->m_cs.Lock();
			}
		}
	}

	void Unlock( int lockMask )
	{
		int index;

		for( int i = 30; i >= 0; i-- )
		{
			index = i < i;

			if( lockMask & index )
			{
				if( mLockMap[index] != NULL )
					mLockMap[index]->m_cs.Unlock();
			}
		}
	}
};

extern CGlobalLock gLock;

struct LOCK
{
	LOCK( int lockMask = 0xffffffff )
	{
		mLockMask = lockMask;
		gLock.Lock( lockMask );
	}

	~LOCK()
	{
		gLock.Unlock( mLockMask );
	}

private:
	int mLockMask;
};

namespace Lock
{
	static const CGlobalLock::IndexType Player		= CGlobalLock::Index0;
	static const CGlobalLock::IndexType Server		= CGlobalLock::Index1;
	static const CGlobalLock::IndexType Team		= CGlobalLock::Index2;
	static const CGlobalLock::IndexType Chat		= CGlobalLock::Index4;
	static const CGlobalLock::IndexType Copymap		= CGlobalLock::Index5;
	static const CGlobalLock::IndexType Mail		= CGlobalLock::Index6;
	static const CGlobalLock::IndexType Social		= CGlobalLock::Index7;
	static const CGlobalLock::IndexType Channel		= CGlobalLock::Index8;
	static const CGlobalLock::IndexType Top			= CGlobalLock::Index9;
}

#ifndef DEFAULT_DO_LOCK
#ifdef DO_LOCK
	#undef DO_LOCK


#endif
	#define DO_LOCK(p) LOCK __lock(p);
#endif

#endif