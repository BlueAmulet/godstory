//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _DEBUGOUTPUTCONSUMER_H_
#define _DEBUGOUTPUTCONSUMER_H_

#include "platform/platform.h"

//#define POWER_LOCBUILD

#if defined(POWER_RELEASE) && defined(POWER_OS_XENON) && !defined(POWER_LOCBUILD)
#define DISABLE_DEBUG_SPEW
#endif

#include "console/console.h"

namespace DebugOutputConsumer
{
   extern bool debugOutputEnabled;

   void init();
   void destroy();
   void logCallback( ConsoleLogEntry::Level level, const char *consoleLine );

   void enableDebugOutput( bool enable = true );
};

#endif