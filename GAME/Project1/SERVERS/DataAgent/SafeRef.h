#ifndef SAFE_REF_H
#define SAFE_REF_H

struct SafeObject
{
	SafeObject()
	{
		mRefCount = 0;
	}

	inline void addRef()
	{
		InterlockedIncrement( &mRefCount );
	}

	inline void releaseRef()
	{
		InterlockedDecrement( &mRefCount );
	}

	virtual ~SafeObject()
	{
		assert( mRefCount == 0 );
	}

private:
	LONG volatile mRefCount;
};

template<class T>
struct SafeRef
{
	SafeRef()
	{
		mRef = NULL;
	}

	SafeRef( const T& obj )
	{
		mRef = (T*)&obj;
		mRef->addRef();
	}

	SafeRef( const SafeRef<T>& objRef )
	{
		mRef = (T*)&objRef.mRef;
		mRef->addRef();
	}

	virtual ~SafeRef()
	{
		mRef->releaseRef();
	}

	operator const T ()
	{
		return *mRef;
	}

	const T* operator -> ()
	{
		return mRef;
	}

	void operator = ( const SafeRef<T> objRef )
	{
		if( mRef )
			mRef->releaseRef();

		mRef = (T*)objRef.mRef;

		mRef->addRef();
	}

private:
	T* mRef;
};


#endif