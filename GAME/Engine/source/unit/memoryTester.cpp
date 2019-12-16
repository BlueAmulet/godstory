//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "unit/memoryTester.h"

using namespace UnitTesting;

void MemoryTester::mark()
{
#ifdef POWER_DEBUG_GUARD
   //Memory::flagCurrentAllocs();
#endif
}

bool MemoryTester::check()
{
#ifdef POWER_DEBUG_GUARD
   //return Memory::countUnflaggedAllocs(NULL) == 0;
    return true;
#else
   //UnitTesting::UnitPrint("MemoryTester::check - unavailable w/o POWER_DEBUG_GUARD defined!");
   return true;
#endif
}
