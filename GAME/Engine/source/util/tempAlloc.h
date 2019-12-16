//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _TEMPALLOC_H_
#define _TEMPALLOC_H_

#include "platform/platform.h"

template< typename T >
struct TempAlloc
{
   T* ptr;
   U32 size;

   TempAlloc( U32 size )
      : size( size )
   {
      ptr = ( T* ) dMalloc( size * sizeof( T ) );
   }
   ~TempAlloc()
   {
      if( ptr )
         dFree( ptr );
   }
   operator T*()
   {
      return ptr;
   }
};

#endif // _TEMPALLOC_H_
