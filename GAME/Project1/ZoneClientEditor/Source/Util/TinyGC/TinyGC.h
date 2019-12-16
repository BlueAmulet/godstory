#pragma once

#include <cstdlib>
#include <hash_set>

#include <WinSock2.h>
#include <windows.h>

#ifdef new
#undef new
#endif

/************************************************************************/
/* 
You can use your own alloctor
*/
/************************************************************************/

struct CTinyGCAlloctor
{
	virtual void* alloc( size_t size ) { return malloc( size ); }
	virtual void free( void* pMemory ) { ::free( pMemory ); }
};

template< class _alloc = CTinyGCAlloctor >
class CTinyGC
{
public:
	friend class CPtr;

	class CPtr
	{
	public:
		friend class CTinyGC<_alloc>;

		CPtr()
		{

		}

		CPtr( void* pMemory )
		{
			m_pMemory = pMemory;
			CTinyGC<_alloc>::GetInstance()->_addPtr( this );
		}

		virtual ~CPtr()
		{
			CTinyGC<_alloc>::GetInstance()->_removePtr( this );		
		}

		inline void operator = ( void* pMemory )
		{
			CTinyGC<_alloc>::GetInstance()->_removePtr( this );	
			m_pMemory = pMemory;
			CTinyGC<_alloc>::GetInstance()->_addPtr( this );
		}

		inline operator void* () const
		{
			return m_pMemory;
		}

		inline operator const char* () const
		{
			return ( const char* )m_pMemory;
		}

		inline operator char* () const
		{
			return ( char* )m_pMemory;
		}

	private:
		void* m_pMemory;
	};

	struct ObjectBase
	{
		virtual ~ObjectBase() { ; }

		inline void* operator new ( size_t size )
		{
			return CTinyGC<_alloc>::GetInstance()->m_pAlloc->alloc( size );
		}

	};

	template< class T = ObjectBase >
	class ObjectPtr : public CPtr
	{
	public:
		ObjectPtr( T* srcObject ) : CPtr( srcObject )			{ CTinyGC<_alloc>::GetInstance()->_addObject( srcObject ); }
		ObjectPtr( const T* srcObject ) : CPtr( srcObject )		{ CTinyGC<_alloc>::GetInstance()->_addObject( srcObject ); }
		ObjectPtr() : CPtr()									{ ; }

	private:
		inline operator T* ()		const	{ return static_cast<T*>m_pMemory; }
		inline T* operator ->()		const	{ return static_cast<T*>m_pMemory; }

		void* operator new ( size_t size )
		{
			return ::operator new( size );
		}

		inline void operator = ( const T* srcPtr )
		{
			CTinyGC<_alloc>::GetInstance()->_removePtr( this );	
			m_pMemory = srcPtr;
			CTinyGC<_alloc>::GetInstance()->_addPtr( this );
			CTinyGC<_alloc>::GetInstance()->_addObject( srcPtr );
		}
	};

	static CTinyGC<_alloc>* GetInstance()
	{
		static CTinyGC<_alloc> theInstance;

		return &theInstance;
	}

	int GC()
	{
		return _gc();
	}

private:
	CRITICAL_SECTION m_cs;

private:

	CTinyGC()
	{
		::InitializeCriticalSection( &m_cs );

		m_pAlloc = new _alloc();
	}

	~CTinyGC()
	{
		_gc();

		delete m_pAlloc;

		::DeleteCriticalSection( &m_cs );
	}

	void _lock()
	{
		::EnterCriticalSection( &m_cs );
	}

	void _unlock()
	{
		::LeaveCriticalSection( &m_cs );
	}

	void _addPtr( CPtr* pPtr )
	{
		_lock();
		m_ptrMap.insert( pPtr );
		m_memMap.insert( pPtr->m_pMemory );
		_unlock();
	}

	void _removePtr( CPtr* pPtr )
	{
		_lock();
		m_ptrMap.erase( pPtr );
		_unlock();
	}

	void _addObject( ObjectBase* pObject )
	{
		_lock();
		m_objMap.insert( pObject );
		_unlock();
	}

	int _gc()
	{
		_lock();
		stdext::hash_set< void* > garbageMap;
		stdext::hash_set< void* > availableMap;

		stdext::hash_set< CPtr* >::iterator itPtr;
		stdext::hash_set< void* >::iterator it;

		int garbageCount = 0;

		for( itPtr = m_ptrMap.begin(); itPtr != m_ptrMap.end(); itPtr++ )
		{
			availableMap.insert( (*itPtr)->m_pMemory );
		}

		for( it = m_memMap.begin(); it != m_memMap.end(); it++ )
		{
			if( availableMap.find( *it ) == availableMap.end() )
			{
				garbageCount++;

				if( m_objMap.find( static_cast<ObjectBase*>(*it) ) != m_objMap.end() )
					static_cast<ObjectBase*>(*it)->~ObjectBase();

				GetInstance()->m_pAlloc->free( *it );
			}
		}

		m_memMap = availableMap;
		_unlock();
		return garbageCount;

	}

	CTinyGCAlloctor*					m_pAlloc;
	stdext::hash_set< CPtr* >			m_ptrMap;
	stdext::hash_set< void* >			m_memMap;
	stdext::hash_set< ObjectBase* >		m_objMap;
};

#include "platform/platform.h"