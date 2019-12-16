#ifndef READ_WRITE_LOCK
#define READ_WRITE_LOCK

#include <windows.h>
#include "base/Locker.h"

class CReaderWriterLock
{
public:
	CReaderWriterLock()
	{	
		mReaderRefCount = 0;
		mReaderOwner = 0;
	}

	virtual ~CReaderWriterLock()
	{
		// wait for all thread to release the lock
		while( mReaderOwner > 0 )
		{
			Sleep( 0 );
		}
	}

	void acquireReaderLock( LONG lOwner )
	{
		// spin until get control
		while( ::InterlockedCompareExchange( &mReaderOwner, lOwner, 0 ) != 0 )
			;

		if( mReaderRefCount == 0 )
		{
			mReaderOwner = 0;
			mCS.Lock();
		}

		::InterlockedIncrement( &mReaderRefCount );

		// release
		mReaderOwner = 0;
	}

	void releaseReaderLock( LONG lOwner )
	{
		// spin until get control
		while( ::InterlockedCompareExchange( &mReaderOwner, lOwner, 0 ) != 0 )
			;

		if( mReaderRefCount == 0 )
			mCS.Unlock();

		::InterlockedDecrement( &mReaderRefCount );

		mReaderOwner = 0;
	}

	void acquireWriterLock()
	{
		mCS.Lock();
	}

	void releaseWriterLock()
	{
		mCS.Unlock();
	}

private:
	CMyCriticalSection mCS;

	LONG volatile mReaderRefCount;
	LONG volatile mReaderOwner;
};

struct CReaderLocker
{
	CReaderLocker(CReaderWriterLock& lock)
		: mLock( lock );
	{
		mLock.acquireReaderLock( (LONG)this );
	}

	virtual ~CReaderLocker()
	{
		mLock.releaseReaderLock( (LONG)this );
	}

private:
	CReaderWriterLock& mLock;
};

struct CWriterLocker
{
	CWriterLocker(CReaderWriterLock& lock)
		: mLock( lock );
	{
		mLock.acquireWriterLock( (LONG)this );
	}

	virtual ~CWriterLocker()
	{
		mLock.releaseWriterLock( (LONG)this );
	}

private:
	CReaderWriterLock& mLock;
};


#endif