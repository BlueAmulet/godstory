//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/threads/mutex.h"
#include "platformWin32/platformWin32.h"
#include "util/safeDelete.h"

//-----------------------------------------------------------------------------
// Mutex Data
//-----------------------------------------------------------------------------

struct PlatformMutexData
{
   HANDLE mMutex;

   PlatformMutexData()
   {
      mMutex = NULL;
   }
};

//-----------------------------------------------------------------------------
// Constructor/Destructor
//-----------------------------------------------------------------------------

Mutex::Mutex()
{
   mData = new PlatformMutexData;

   mData->mMutex = CreateMutex(NULL, FALSE, NULL);
}

Mutex::~Mutex()
{
   if(mData && mData->mMutex)
      CloseHandle(mData->mMutex);
   
   SAFE_DELETE(mData);
}

//-----------------------------------------------------------------------------
// Public Methods
//-----------------------------------------------------------------------------

bool Mutex::lock(bool block /* = true */)
{
   if(mData == NULL || mData->mMutex == NULL)
      return false;

   return (bool)WaitForSingleObject(mData->mMutex, block ? INFINITE : 0) == WAIT_OBJECT_0;
}

void Mutex::unlock()
{
   if(mData == NULL || mData->mMutex == NULL)
      return;

   ReleaseMutex(mData->mMutex);
}

class PlatformCSData
{
public:
    PlatformCSData()
    {
        InitializeCriticalSection(&m_CritSect); 
    }

    ~PlatformCSData() 
    { 
        DeleteCriticalSection(&m_CritSect); 
    };

    CRITICAL_SECTION m_CritSect;
};

CCSLock::CCSLock()
{
    m_pData = (PlatformCSData*)malloc(sizeof(PlatformCSData));
	constructInPlace(m_pData);
}

CCSLock::~CCSLock()
{
    //SAFE_DELETE(m_pData);
	free(m_pData);
}

void CCSLock::Lock()
{
    EnterCriticalSection(&m_pData->m_CritSect); 
}

void CCSLock::Unlock()
{
    LeaveCriticalSection(&m_pData->m_CritSect); 
}


//void Mutex::set( void*data )
//{
//   if(mData && mData->mMutex)
//      CloseHandle(mData->mMutex);
//
//   if( mData == NULL )
//      mData = new PlatformMutexData;
//
//   mData->mMutex = (HANDLE)data;
//
//}