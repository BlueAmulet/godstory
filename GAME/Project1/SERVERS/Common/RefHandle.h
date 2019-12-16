#ifndef REF_HANDLE_H
#define REF_HANDLE_H

#include <winsock2.h>
#include <Windows.h>

template< typename _Ty >
class RefObject
{
public:
	RefObject() {mRefCount = 0;}
	virtual ~RefObject() {}

	void addRef()
	{
		InterlockedIncrement( &mRefCount );
	}

	void releaseRef()
	{
		InterlockedDecrement( &mRefCount );

		if( mRefCount == 0 )
		{
			MEMPOOL->FreeObj( (RefObject<_Ty>*)this );
		}
	}
	
private:
	volatile LONG mRefCount;
};

template< typename _Ty >
class RefHandle
{
public:
	RefHandle()
	{
		pData = NULL;
	}

	RefHandle( _Ty* data )
	{
		pData = data;
		if( pData )
			pData->addRef();
	}

	RefHandle( const _Ty* data )
	{
		pData = (stPlayerData*)data;
		if( pData )
			pData->addRef();
	}

	RefHandle( const RefHandle<_Ty>& other )
	{
		pData = other.pData;
		if( pData )
			pData->addRef();
	}

	~RefHandle()
	{
		if( pData )
			pData->releaseRef();
	}

	_Ty* operator ->()
	{
		assert( pData != NULL );
		return pData;
	}

	operator _Ty* ()
	{
		assert( pData != NULL );
		return pData;
	}

	operator bool ()
	{
		return pData != NULL;
	}

	bool operator == ( const _Ty* data )
	{
		return pData == data;
	}

	RefHandle<_Ty>& operator = ( const _Ty* data )
	{
		if( pData )
			pData->releaseRef();

		pData = (stPlayerData*)data;
		
		if( pData )
			pData->addRef();
		return *this;
	}

	RefHandle<_Ty>& operator = ( const RefHandle<_Ty>& other )
	{
		if( pData )
			pData->releaseRef();

		pData = other.pData;
		if( pData )
			pData->addRef();
		return *this;
	}

	bool isNull()
	{
		return pData == NULL;
	}

private:
	_Ty* pData;
};

#endif