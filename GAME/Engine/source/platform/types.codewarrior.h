//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef INCLUDED_TYPES_CODEWARRIOR_H
#define INCLUDED_TYPES_CODEWARRIOR_H

#pragma once

// If using the IDE detect if DEBUG build was requested
#if __ide_target("PowerEngine-W32-Debug")
   #define POWER_DEBUG
#elif __ide_target("PowerEngine-MacCarb-Debug")
   #define POWER_DEBUG
#elif __ide_target("PowerEngine-MacX-Debug")
   #define POWER_DEBUG
#endif


//--------------------------------------
// Types
typedef signed long long   S64;     ///< Compiler independent Signed 64-bit integer
typedef unsigned long long U64;     ///< Compiler independent Unsigned 64-bit integer



//--------------------------------------
// Compiler Version
#define POWER_COMPILER_CODEWARRIOR __MWERKS__

#define POWER_COMPILER_STRING "CODEWARRIOR"


//--------------------------------------
// Identify the Operating System
#if defined(_WIN32)
#  define POWER_OS_STRING "Win32"
#  define POWER_OS_WIN32
#  include "platform/types.win32.h"

#elif defined(macintosh) || defined(__APPLE__)
#  define POWER_OS_STRING "Mac"
#  define POWER_OS_MAC
#  if defined(__MACH__)
#     define POWER_OS_MAC_OSX
#  endif
#  include "platform/types.ppc.h"
// for the moment:
#  include "platformMacCarb/macCarb_common_prefix.h"
#else 
#  error "CW: Unsupported Operating System"
#endif


//--------------------------------------
// Identify the CPU
#if defined(_M_IX86)
#  define POWER_CPU_STRING "x86"
#  define POWER_CPU_X86
#  define POWER_LITTLE_ENDIAN
#  define POWER_SUPPORTS_NASM
#  define POWER_SUPPORTS_VC_INLINE_X86_ASM

   // Compiling with the CW IDE we cannot use NASM :(
#  if __ide_target("PowerEngine-W32-Debug")
#     undef POWER_SUPPORTS_NASM
#  elif __ide_target("PowerEngine-W32-Release")
#     undef POWER_SUPPORTS_NASM
#  endif

#elif defined(__POWERPC__)
#  define POWER_CPU_STRING "PowerPC"
#  define POWER_CPU_PPC
#  define POWER_BIG_ENDIAN

#else
#  error "CW: Unsupported Target CPU"
#endif


#endif // INCLUDED_TYPES_CODEWARRIOR_H

