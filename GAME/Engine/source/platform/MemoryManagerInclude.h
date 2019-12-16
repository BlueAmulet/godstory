#ifndef _MEMORYMANAGERINCLUDE_H_
#define _MEMORYMANAGERINCLUDE_H_

#ifndef _POWERCONFIG_H_
#include "platform/EngineConfig.h"
#endif

#include "MemoryManager.h"
#include <new.h>

template <class T>
inline T* constructInPlace(T* p)
{
    return new(p) T;
}

template <class T>
inline T* constructInPlace(T* p, const T* copy)
{
    return new(p) T(*copy);
}

template <class T>
inline void destructInPlace(T* p)
{
    p->~T();
}

#define placenew(x)  new(x)

//�ڴ�������ͷ������ǲ����ݵ�
#ifdef POWER_MEMORY_ALLOC
#undef POWER_MEMORY_MANAGER
#endif

#if defined POWER_MEMORY_MANAGER || defined POWER_MEMORY_ALLOC

extern void* operator new(size_t size);
extern void* operator new[](size_t size);
extern void  operator delete(void* ptr);
extern void  operator delete[](void* ptr);

#endif

#ifdef POWER_MEMORY_MANAGER

#define dNew             new(__FILE__,__LINE__,__FUNCTION__)
#define dDelete          (CMemoryManager::Instance()->_setOwner(__FILE__,__LINE__,__FUNCTION__),false) ? CMemoryManager::Instance()->_setOwner("",0,"") : delete

#define dMalloc(sz)      CMemoryManager::Instance()->_allocMem(__FILE__,__LINE__,__FUNCTION__, m_alloc_malloc, sz)
#define dRealloc(ptr,sz) CMemoryManager::Instance()->_rllocMem(__FILE__,__LINE__,__FUNCTION__, m_alloc_realloc,sz, ptr)
#define dMallocEx(sz,file,line)      CMemoryManager::Instance()->_allocMem(file,line,"?", m_alloc_malloc, sz)
#define dReallocEx(ptr,sz,file,line) CMemoryManager::Instance()->_rllocMem(file,line,"?", m_alloc_realloc,sz, ptr)
#define dFree(ptr)       CMemoryManager::Instance()->_dllocMem(__FILE__,__LINE__,__FUNCTION__, m_alloc_free,       ptr)

//our override new
extern void* operator new(size_t size, const char* file,const unsigned int,const char* fun);
extern void* operator new[](size_t size,const char* file,const unsigned int,const char* fun);
extern void  operator delete(void* ptr,const char* file,const unsigned int,const char* fun);
extern void  operator delete[](void* ptr,const char* file,const unsigned int,const char* fun);

#define new new(__FILE__,__LINE__,__FUNCTION__)

//�ض���new��ϵͳ������һЩnew��������,mass
//�������ڽ���d3dxmath��Ľ��д��������ͨ���ļ�����˳�������
#define __D3DX9MATH_H__

//dumy class
class CVirtualAllocBase
{
public:
};

#define vMalloc(x)     dMalloc(x)
#define vFree(x)       dFree(x)
#define vRealloc(x, y) dRealloc(x,y)

#else

#define dNew                        new
#define dDelete                     delete
#define dMalloc(x)                  malloc(x)
#define dFree(ptr)                  free(ptr)
#define dRealloc(x, y)              realloc(x,y)
#define dMallocEx(x,file,line)	    malloc(x)
#define dReallocEx(x,y,file,line)   realloc(x,y)


//�Ƿ�����virtual alloc��������������Ҫ�ر�ԭ�����ڴ������
#ifdef VIRTUAL_ALLOC

extern void* _VirtualAlloc(unsigned int len);
extern void* _VirtualRealloc(void* ptr,unsigned int len);
extern void  _VirtualFree(void* ptr);

#define vMalloc(x)     _VirtualAlloc(x)
#define vFree(x)       _VirtualFree(x)
#define vRealloc(x, y) _VirtualRealloc(x,y)

//��Ҫ���������ڴ��ͨ���������������ñ���
class CVirtualAllocBase
{
public:
    inline void* operator new(size_t size)
    {
        return _VirtualAlloc(size);
    }

    inline void* operator new[](size_t size)
    {
        return _VirtualAlloc(size);
    }

    inline void  operator delete(void* ptr)
    {
        _VirtualFree(ptr);
    }

    inline void  operator delete[](void* ptr)
    {
        _VirtualFree(ptr);
    }
};

#else

#define vMalloc(x)     malloc(x)
#define vFree(x)       free(x)
#define vRealloc(x, y) realloc(x,y)

//dumy class
class CVirtualAllocBase
{
public:
};

#endif

#endif /*POWER_MEMORY_MANAGER*/

#endif /*_MEMORYMANAGERINCLUDE_H_*/