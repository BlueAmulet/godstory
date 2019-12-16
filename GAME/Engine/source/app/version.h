//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _VERSION_H_
#define _VERSION_H_

/// This is our global version number for the engine source code that
/// we are using. See <game>/source/platform/EngineConfig.h for the game's source
/// code version, the game name, and which type of game it is 
///
/// Version number is major * 1000 + minor * 100 + revision * 10.
#define POWER_GAME_ENGINE          1710

/// Human readable engine version string.
#define POWER_GAME_ENGINE_VERSION_STRING  "2.0"

/// Gets the specified version number.  The version number is specified as a global in version.cc
U32 getVersionNumber();
/// Gets the version number in string form
const char* getVersionString();
/// Gets the engine product name in string form
const char* getEngineProductString();
/// Gets the compile date and time
const char* getCompileTimeString();

#endif
