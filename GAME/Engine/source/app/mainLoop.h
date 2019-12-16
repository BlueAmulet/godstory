//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _APP_MAINLOOP_H_
#define _APP_MAINLOOP_H_

#include "platform/platform.h"

/// Support class to simplify the process of writing a main loop for PowerEngine apps.
class StandardMainLoop
{
public:
   /// Initialize core libraries and call registered init functions
   static void init();

   /// Pass command line arguments to registered functions and main.cs
   static bool handleCommandLine(S32 argc, const char **argv);

   //use from gm client
   static void initMainLoop();
   static void doMainLoopOnce();

   /// A standard mainloop implementation.
   static void doMainLoop();

   /// Shut down the core libraries and call registered shutdown fucntions.
   static void shutdown();

   static void setRestart( bool restart );
   static bool requiresRestart();

private:
   /// Handle "pre shutdown" tasks like notifying scripts BEFORE we delete
   /// stuff from under them.
   static void preShutdown();
};

#endif