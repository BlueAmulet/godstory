//-----------------------------------------------------------------------------
// Torque Game Engine Advanced
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//------------------------------
//maccarb_common_prefix.h
//------------------------------

#define TORQUE_OS_MAC_CARB       1     // always defined right now...

#if defined(TORQUE_OS_MAC_OSX)
//#define Z_PREFIX  // OSX comes with zlib, so generate unique symbols.
#endif

// defines for the mac headers to activate proper Carbon codepaths.
#define TARGET_API_MAC_CARBON    1   // apple carbon header flag to take right defpaths.
//#define OTCARBONAPPLICATION      1   // means we can use the old-style funcnames

// determine the OS version we're building on...
//  MAC_OS_X_VERSION_MAX_ALLOWED will have the local OSX version,
//  or it will have the version of OSX for the sdk we're cross compiling with.
#include <AvailabilityMacros.h>

// Pasteboards were introduced in 10.3, and are not available before 10.3
// QDGetCGDirectDisplayID was introduced in 10.3, but there is a manual workaround
//  MAC_OS_X_VERSION_10_3 == 1030 , and may not exist if we're pre 10.3, so use the raw value here
#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1030
#define TORQUE_MAC_HAS_PASTEBOARD
#define TORQUE_MAC_HAS_QDGETCGDIRECTDISPLAYID
#endif

// Some gestalt values we're currently using were introduced in 10.3, or 10.4.
// We'll define them here if we know we don't have them.
#if MAC_OS_X_VERSION_MAX_ALLOWED < 1030
 // gestalt values introduced in 10.3+ frameworks.
#define gestaltCPUG47447   0x0112
#define gestaltCPU970      0x0139
#endif

#if MAC_OS_X_VERSION_MAX_ALLOWED < 1040
 // gestalt values introduced in 10.4+ frameworks.
#define gestaltCPU970FX    0x013C
#define gestaltCPUPentium4 'i5iv'
#endif
