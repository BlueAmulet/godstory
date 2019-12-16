//-----------------------------------------------------------------------------
// Torque Game Engine Advanced
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include <CoreServices/CoreServices.h>
#include "platform/platform.h"
#include "platform/platformSemaphore.h"

void * Semaphore::createSemaphore(U32 initialCount)
{
   MPSemaphoreID sid;
   OSStatus s = MPCreateSemaphore(S32_MAX - 1, initialCount, &sid);

   AssertFatal(s == noErr, "Failed to allocate semaphore!");
   return (void*)(sid);
}

void Semaphore::destroySemaphore(void * semaphore)
{
   OSStatus s = MPDeleteSemaphore((MPSemaphoreID)semaphore);
   AssertFatal(s == noErr, "Failed to destroy semaphore!");
}

bool Semaphore::acquireSemaphore(void * semaphore, bool block)
{
   OSStatus s = MPWaitOnSemaphore((MPSemaphoreID)semaphore, block ? kDurationForever : kDurationImmediate);
   return(s == noErr);
}

void Semaphore::releaseSemaphore(void * semaphore)
{
   OSStatus s = MPSignalSemaphore((MPSemaphoreID)semaphore);
   AssertFatal(s == noErr, "Failed to release semaphore!");
}
