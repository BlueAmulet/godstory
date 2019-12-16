//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "app/version.h"
#include "console/console.h"

static const U32 csgVersionNumber = POWER_GAME_ENGINE;

U32 getVersionNumber()
{
   return csgVersionNumber;
}

const char* getVersionString()
{
   return POWER_GAME_ENGINE_VERSION_STRING;
}

const char* getEngineProductString()
{
	return "PowerEngine";
}

const char* getCompileTimeString()
{
   return __DATE__ " at " __TIME__;
}
//----------------------------------------------------------------

ConsoleFunctionGroupBegin( CompileInformation, "Functions to get version information about the current executable." )

ConsoleFunction( getVersionNumber, S32, 1, 1, "Get the version of the build, as a string.")
{
   return getVersionNumber();
}

ConsoleFunction( getVersionString, const char*, 1, 1, "Get the version of the build, as a string.")
{
   return getVersionString();
}

ConsoleFunction( getEngineName, const char*, 1, 1, "Get the name of the engine product that this is running from, as a string.")
{
   return getEngineProductString();
}

ConsoleFunction( getCompileTimeString, const char*, 1, 1, "Get the time of compilation.")
{
   return getCompileTimeString();
}

ConsoleFunction( getBuildString, const char*, 1, 1, "Get the type of build, \"Debug\" or \"Release\".")
{
#ifdef POWER_DEBUG
   return "Debug";
#else
   return "Release";
#endif
}

ConsoleFunctionGroupEnd( CompileInformation );

