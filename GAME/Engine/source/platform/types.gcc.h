//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef INCLUDED_TYPES_GCC_H
#define INCLUDED_TYPES_GCC_H


// For additional information on GCC predefined macros
// http://gcc.gnu.org/onlinedocs/gcc-3.0.2/cpp.html


//--------------------------------------
// Types
typedef signed long long    S64;
typedef unsigned long long  U64;


//--------------------------------------
// Compiler Version
#define POWER_COMPILER_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)


//--------------------------------------
// Identify the compiler string

#if defined(__MINGW32__)
#  define POWER_COMPILER_STRING "GCC (MinGW)"
#  define POWER_COMPILER_MINGW
#elif defined(__CYGWIN__)
#  define POWER_COMPILER_STRING "GCC (Cygwin)"
#  define POWER_COMPILER_MINGW
#else
#  define POWER_COMPILER_STRING "GCC "
#endif


//--------------------------------------
// Identify the Operating System
#if defined(__WIN32__) || defined(_WIN32)
#  define POWER_OS_STRING "Win32"
#  define POWER_OS_WIN32
#  define POWER_SUPPORTS_NASM
#  define POWER_SUPPORTS_GCC_INLINE_X86_ASM
#  include "platform/types.win32.h"

#elif defined(linux)
#  define POWER_OS_STRING "Linux"
#  define POWER_OS_LINUX
#  define POWER_SUPPORTS_NASM
#  define POWER_SUPPORTS_GCC_INLINE_X86_ASM
#  include "platform/types.posix.h"

#elif defined(__OpenBSD__)
#  define POWER_OS_STRING "OpenBSD"
#  define POWER_OS_OPENBSD
#  define POWER_SUPPORTS_NASM
#  define POWER_SUPPORTS_GCC_INLINE_X86_ASM
#  include "platform/types.posix.h"

#elif defined(__FreeBSD__)
#  define POWER_OS_STRING "FreeBSD"
#  define POWER_OS_FREEBSD
#  define POWER_SUPPORTS_NASM
#  define POWER_SUPPORTS_GCC_INLINE_X86_ASM
#  include "platform/types.posix.h"

#elif defined(__APPLE__)
#  define POWER_OS_MAC
#  define POWER_OS_MAC_OSX
#if defined(i386)
#  define POWER_SUPPORTS_NASM
#endif
#  include "platform/types.ppc.h"
// for the moment:
#  include "platformMacCarb/macCarb_common_prefix.h"
#else 
#  error "GCC: Unsupported Operating System"
#endif


//--------------------------------------
// Identify the CPU
#if defined(i386)
#  define POWER_CPU_STRING "Intel x86"
#  define POWER_CPU_X86
#  define POWER_LITTLE_ENDIAN

#elif defined(__ppc__)
#  define POWER_CPU_STRING "PowerPC"
#  define POWER_CPU_PPC
#  define POWER_BIG_ENDIAN

#else
#  error "GCC: Unsupported Target CPU"
#endif


#endif // INCLUDED_TYPES_GCC_H

