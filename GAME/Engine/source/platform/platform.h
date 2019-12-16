//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifndef _POWERCONFIG_H_
#include "platform/EngineConfig.h"
#endif

#ifndef _POWER_TYPES_H_
#include "platform/types.h"
#endif

#ifndef _PLATFORMASSERT_H_
#include "platform/platformAssert.h"
#endif

#ifndef _MSGBOX_H_
#include "platform/nativeDialogs/msgBox.h"
#endif

#ifndef _PLATFORM_STRINGS_H_
#include "platform/platformStrings.h"
#endif

#ifndef _VERSION_H_
#include "app/version.h"
#endif

#include "MemoryManagerInclude.h"

#define DSO ".cmp"
#define EDSO ".cmp"
#define GDSO ".gmp"

//------------------------------------------------------------------------------
// Endian conversions
#ifdef POWER_LITTLE_ENDIAN

inline U16 convertHostToLEndian(const U16 i) { return i; }
inline U16 convertLEndianToHost(const U16 i) { return i; }
inline U32 convertHostToLEndian(const U32 i) { return i; }
inline U32 convertLEndianToHost(const U32 i) { return i; }
inline S16 convertHostToLEndian(const S16 i) { return i; }
inline S16 convertLEndianToHost(const S16 i) { return i; }
inline S32 convertHostToLEndian(const S32 i) { return i; }
inline S32 convertLEndianToHost(const S32 i) { return i; }

inline F32 convertHostToLEndian(const F32 i) { return i; }
inline F32 convertLEndianToHost(const F32 i) { return i; }

inline F64 convertHostToLEndian(const F64 i) { return i; }
inline F64 convertLEndianToHost(const F64 i) { return i; }

inline U16 convertHostToBEndian(const U16 i)
{
   return U16((i << 8) | (i >> 8));
}

inline U16 convertBEndianToHost(const U16 i)
{
   return U16((i << 8) | (i >> 8));
}

inline S16 convertHostToBEndian(const S16 i)
{
   return S16(convertHostToBEndian(U16(i)));
}

inline S16 convertBEndianToHost(const S16 i)
{
   return S16(convertBEndianToHost(U16(i)));
}

inline U32 convertHostToBEndian(const U32 i)
{
   return ((i << 24) & 0xff000000) |
          ((i <<  8) & 0x00ff0000) |
          ((i >>  8) & 0x0000ff00) |
          ((i >> 24) & 0x000000ff);
}

inline U32 convertBEndianToHost(const U32 i)
{
   return ((i << 24) & 0xff000000) |
          ((i <<  8) & 0x00ff0000) |
          ((i >>  8) & 0x0000ff00) |
          ((i >> 24) & 0x000000ff);
}

inline S32 convertHostToBEndian(const S32 i)
{
   return S32(convertHostToBEndian(U32(i)));
}

inline S32 convertBEndianToHost(const S32 i)
{
   return S32(convertBEndianToHost(U32(i)));
}

inline U64 convertBEndianToHost(const U64 i)
{
   U32 *inp = (U32 *) &i;
   U64 ret = 0;
   U32 *outp = (U32 *) &ret;
   outp[0] = convertBEndianToHost(inp[1]);
   outp[1] = convertBEndianToHost(inp[0]);
   return ret;
}

inline U64 convertHostToBEndian(const U64 i)
{
   U32 *inp = (U32 *) &i;
   U64 ret = 0;
   U32 *outp = (U32 *) &ret;
   outp[0] = convertHostToBEndian(inp[1]);
   outp[1] = convertHostToBEndian(inp[0]);
   return ret;
}

inline F64 convertBEndianToHost(const F64 in_swap)
{
   U64 result = convertBEndianToHost(* ((U64 *) &in_swap) );
   return * ((F64 *) &result);
}

inline F64 convertHostToBEndian(const F64 in_swap)
{
   U64 result = convertHostToBEndian(* ((U64 *) &in_swap) );
   return * ((F64 *) &result);
}

#elif defined(POWER_BIG_ENDIAN)

inline U16 convertHostToBEndian(const U16 i) { return i; }
inline U16 convertBEndianToHost(const U16 i) { return i; }
inline U32 convertHostToBEndian(const U32 i) { return i; }
inline U32 convertBEndianToHost(const U32 i) { return i; }
inline S16 convertHostToBEndian(const S16 i) { return i; }
inline S16 convertBEndianToHost(const S16 i) { return i; }
inline S32 convertHostToBEndian(const S32 i) { return i; }
inline S32 convertBEndianToHost(const S32 i) { return i; }
inline F32 convertHostToBEndian(const F32 i) { return i; }
inline F32 convertBEndianToHost(const F32 i) { return i; }
inline F64 convertHostToBEndian(const F64 i) { return i; }
inline F64 convertBEndianToHost(const F64 i) { return i; }

inline U16 convertHostToLEndian(const U16 i)
{
   return (i << 8) | (i >> 8);
}
inline U16 convertLEndianToHost(const U16 i)
{
   return (i << 8) | (i >> 8);
}
inline U32 convertHostToLEndian(const U32 i)
{
   return ((i << 24) & 0xff000000) |
          ((i <<  8) & 0x00ff0000) |
          ((i >>  8) & 0x0000ff00) |
          ((i >> 24) & 0x000000ff);
}
inline U32 convertLEndianToHost(const U32 i)
{
   return ((i << 24) & 0xff000000) |
          ((i <<  8) & 0x00ff0000) |
          ((i >>  8) & 0x0000ff00) |
          ((i >> 24) & 0x000000ff);
}

inline U64 convertLEndianToHost(const U64 i)
{
   U32 *inp = (U32 *) &i;
   U64 ret = 0;
   U32 *outp = (U32 *) &ret;
   outp[0] = convertLEndianToHost(inp[1]);
   outp[1] = convertLEndianToHost(inp[0]);
   return ret;
}

inline U64 convertHostToLEndian(const U64 i)
{
   U32 *inp = (U32 *) &i;
   U64 ret = 0;
   U32 *outp = (U32 *) &ret;
   outp[0] = convertHostToLEndian(inp[1]);
   outp[1] = convertHostToLEndian(inp[0]);
   return ret;
}

inline F64 convertLEndianToHost(const F64 in_swap)
{
   U64 result = convertLEndianToHost(*reinterpret_cast<const U64*>(&in_swap) );
   return *reinterpret_cast<F64*>(&result);
}

inline F64 convertHostToLEndian(const F64 in_swap)
{
   U64 result = convertHostToLEndian(*reinterpret_cast<const U64*>(&in_swap) );
   return *reinterpret_cast<F64*>(&result);
}

inline F32 convertHostToLEndian(const F32 i)
{
   U32 result = convertHostToLEndian( *reinterpret_cast<const U32*>(&i) );
   return *reinterpret_cast<F32*>(&result);
}

inline F32 convertLEndianToHost(const F32 i)
{
   U32 result = convertLEndianToHost( *reinterpret_cast<const U32*>(&i) );
   return *reinterpret_cast<F32*>(&result);
}

inline S16 convertHostToLEndian(const S16 i) { return S16(convertHostToLEndian(U16(i))); }
inline S16 convertLEndianToHost(const S16 i) { return S16(convertLEndianToHost(U16(i))); }
inline S32 convertHostToLEndian(const S32 i) { return S32(convertHostToLEndian(U32(i))); }
inline S32 convertLEndianToHost(const S32 i) { return S32(convertLEndianToHost(U32(i))); }

#else
#error "Endian define not set"
#endif

/// Global processor identifiers.
///
/// @note These enums must be globally scoped so that they work with the inline assembly
enum ProcessorType
{
   // x86
   CPU_X86Compatible,
   CPU_Intel_Unknown,
   CPU_Intel_486,
   CPU_Intel_Pentium,
   CPU_Intel_PentiumMMX,
   CPU_Intel_PentiumPro,
   CPU_Intel_PentiumII,
   CPU_Intel_PentiumCeleron,
   CPU_Intel_PentiumIII,
   CPU_Intel_Pentium4,
   CPU_AMD_K6,
   CPU_AMD_K6_2,
   CPU_AMD_K6_3,
   CPU_AMD_Athlon,
   CPU_AMD_Unknown,
   CPU_Cyrix_6x86,
   CPU_Cyrix_MediaGX,
   CPU_Cyrix_6x86MX,
   CPU_Cyrix_GXm,          ///< Media GX w/ MMX
   CPU_Cyrix_Unknown,

   // PowerPC
   CPU_PowerPC_Unknown,
   CPU_PowerPC_601,
   CPU_PowerPC_603,
   CPU_PowerPC_603e,
   CPU_PowerPC_603ev,
   CPU_PowerPC_604,
   CPU_PowerPC_604e,
   CPU_PowerPC_604ev,
   CPU_PowerPC_G3,
   CPU_PowerPC_G4,
   CPU_PowerPC_G4_7450,
   CPU_PowerPC_G4_7455,
   CPU_PowerPC_G4_7447, 
   CPU_PowerPC_G5,

   // Xenon
   CPU_Xenon,

};

/// Properties for x86 architecture chips.
enum x86Properties
{ 
   CPU_PROP_C         = (1<<0),  ///< We should use C fallback math functions.
   CPU_PROP_FPU       = (1<<1),  ///< Has an FPU. (It better!)
   CPU_PROP_MMX       = (1<<2),  ///< Supports MMX instruction set extension.
   CPU_PROP_3DNOW     = (1<<3),  ///< Supports AMD 3dNow! instruction set extension.
   CPU_PROP_SSE       = (1<<4),  ///< Supports SSE instruction set extension.
   CPU_PROP_RDTSC     = (1<<5),  ///< Supports Read Time Stamp Counter op.
   CPU_PROP_SSE2      = (1<<6),  ///< Supports SSE2 instruction set extension.
   CPU_PROP_MP        = (1<<7),  ///< This is a multi-processor system.
};

/// Properties for PowerPC architecture chips.
enum PPCProperties
{
   CPU_PROP_PPCMIN    = (1<<0),
   CPU_PROP_ALTIVEC   = (1<<1),     ///< Supports AltiVec instruction set extension.
   CPU_PROP_PPCMP     = (1<<7)      ///< Multi-processor system
};

/// Processor info manager. 
struct Processor
{
   /// Gather processor state information.
   static void init();
};

#if defined(POWER_SUPPORTS_GCC_INLINE_X86_ASM)
#define POWER_DEBUGBREAK() { asm ( "int 3"); }
#elif defined (POWER_SUPPORTS_VC_INLINE_X86_ASM) // put this test second so that the __asm syntax doesn't break the Visual Studio Intellisense parser
#define POWER_DEBUGBREAK() { __asm { int 3 }; } 
#else
/// Macro to do in-line debug breaks, used for asserts.  Does inline assembly when possible.
#define POWER_DEBUGBREAK() Platform::debugBreak();
#endif

/// Physical type of a drive.
enum DriveType
{
   DRIVETYPE_FIXED = 0,       ///< Non-removable fixed drive.
   DRIVETYPE_REMOVABLE = 1,   ///< Removable drive.
   DRIVETYPE_REMOTE = 2,      ///< Networked/remote drive.
   DRIVETYPE_CDROM = 3,       ///< CD-Rom.
   DRIVETYPE_RAMDISK = 4,     ///< A ramdisk!
   DRIVETYPE_UNKNOWN = 5      ///< Don't know.
};

// Some forward declares for later.
class Point2I;
template<class T> class Vector;
struct InputEventInfo;

struct Platform
{
   // Time
   struct LocalTime
   {
      U8  sec;        ///< Seconds after minute (0-59)
      U8  min;        ///< Minutes after hour (0-59)
      U8  hour;       ///< Hours after midnight (0-23)
      U8  month;      ///< Month (0-11; 0=january)
      U8  monthday;   ///< Day of the month (1-31)
      U8  weekday;    ///< Day of the week (0-6, 6=sunday)
      U16 year;       ///< Current year minus 1900
      U16 yearday;    ///< Day of year (0-365)
      bool isdst;     ///< True if daylight savings time is active
   };

   static void getLocalTime(LocalTime &);
   static U32  getTime();
   static U32  getVirtualMilliseconds();
   static U32  getRealMilliseconds();
   static void advanceTime(U32 delta);
   static S32 getBackgroundSleepTime();
   static void getDateTime(U32, LocalTime&);

   // Platform control
   static void init();
   static void initConsole();
   static void shutdown();
   static void process();
   
   // Process control
   static void sleep(U32 ms);
   static bool excludeOtherInstances(const char *string);
   static bool checkOtherInstances(const char *string);
   static void restartInstance();
   static void postQuitMessage(const U32 in_quitVal);
   static void forceShutdown(S32 returnValue);

   // Debug
   static void outputDebugString(const char *string);
   static void debugBreak();
   
   // Random
   static float getRandom();
   static S32 getRandomI(S32 i,S32 n);
   
   // Window state
   static void setWindowLocked(bool locked);
   static void minimizeWindow();
   //static const Point2I &getWindowSize();
   static void setWindowSize( U32 newWidth, U32 newHeight, bool fullScreen );
   static void closeWindow();

   // File stuff
   static bool doCDCheck();
   static StringTableEntry createPlatformFriendlyFilename(const char *filename);
   struct FileInfo
   {
      const char* pFullPath;
      const char* pFileName;
      U32 fileSize;
   };
   static bool cdFileExists(const char *filePath, const char *volumeName, S32 serialNum);
   static void fileToLocalTime(const FileTime &ft, LocalTime *lt);
   /// compare file times returns < 0 if a is earlier than b, >0 if b is earlier than a
   static S32 compareFileTimes(const FileTime &a, const FileTime &b);
   static bool stringToFileTime(const char * string, FileTime * time);
   static bool fileTimeToString(FileTime * time, char * string, U32 strLen);

   // Directory functions.  Dump path returns false iff the directory cannot be
   //  opened.
   
   static StringTableEntry getCurrentDirectory();
   static bool             setCurrentDirectory(StringTableEntry newDir);

   static StringTableEntry getTemporaryDirectory();
   static StringTableEntry getTemporaryFileName();

   /// Returns the filename of the PowerEngine executable.
   /// On Win32, this is the .exe file.
   /// On Mac, this is the .app/ directory bundle.
   static StringTableEntry getExecutableName();
   /// Returns full pathname of the PowerEngine executable without filename
   static StringTableEntry getExecutablePath();
   
   /// Returns the full path to the directory that contains main.cs.
   /// Tools scripts are validated as such if they are in this directory or a
   /// subdirectory of this directory.
   static StringTableEntry getMainDotCsDir();

   /// Set main.cs directory. Used in runEntryScript()
   static void setMainDotCsDir(const char *dir);

   static StringTableEntry getPrefsPath(const char *file = NULL);

   static char *makeFullPathName(const char *path, char *buffer, U32 size, const char *cwd = NULL);
   static StringTableEntry stripBasePath(const char *path);
   static bool isFullPath(const char *path);
   static StringTableEntry makeRelativePathName(const char *path, const char *to);

   static bool dumpPath(const char *in_pBasePath, Vector<FileInfo>& out_rFileVector, S32 recurseDepth = -1);
   static bool dumpDirectories( const char *path, Vector<StringTableEntry> &directoryVector, S32 depth = 0, bool noBasePath = false );
   static bool hasSubDirectory( const char *pPath );
   static bool getFileTimes(const char *filePath, FileTime *createTime, FileTime *modifyTime);
   static bool isFile(const char *pFilePath);
   static S32  getFileSize(const char *pFilePath);
   static bool isDirectory(const char *pDirPath);
   static bool isSubDirectory(const char *pParent, const char *pDir);
	//功能：删除目录pDir下的所有文件名为pFilename的文件，不含子目录
	//pDir：目录名，如c:\temp
	//pFilename：文件名，如*.txt,hello.jpg，支持*.txt
	static bool deleteDirFiles(const char* pDir, const char* pFilename);

   static void addExcludedDirectory(const char *pDir);
   static void clearExcludedDirectories();
   static bool isExcludedDirectory(const char *pDir);

   /// Given a directory path, create all necessary directories for that path to exist.
   static bool createPath(const char *path); // create a directory path

   static void AdjustCMPfileName(char *path);
   // Alerts
   static void AlertOK(const char *windowTitle, const char *message);
   static bool AlertOKCancel(const char *windowTitle, const char *message);
   static bool AlertRetry(const char *windowTitle, const char *message);

   // Volumes
   struct VolumeInformation
   {
      StringTableEntry  RootPath;
      StringTableEntry  Name;
      StringTableEntry  FileSystem;
      U32               SerialNumber;
      U32               Type;
      bool              ReadOnly;
   }*PVolumeInformation;

   // Volume functions.
   static void getVolumeNamesList( Vector<const char*>& out_rNameVector, bool bOnlyFixedDrives = false );
   static void getVolumeInformationList( Vector<VolumeInformation>& out_rVolumeInfoVector, bool bOnlyFixedDrives = false );

   static struct SystemInfo_struct
   {
      struct Processor
      {
         ProcessorType type;
         const char *name;
         U32         mhz;
         U32         properties;      // CPU type specific enum
      } processor;
   } SystemInfo;

   // Web page launch function:
   static bool openWebBrowser( const char* webAddress );

   static void openFolder( const char* path );

   static bool RegGamePath(const char* szPathName);

   static const char* getClipboard();
   static bool setClipboard(const char *text);

   // User Specific Functions
   static StringTableEntry getUserHomeDirectory();
   static StringTableEntry getUserDataDirectory();
   static bool getUserIsAdministrator();
   
   // Displays a fancy platform specific message box
   static S32 messageBox(const UTF8 *title, const UTF8 *message, MBButtons buttons = MBOkCancel, MBIcons icon = MIInformation);
   
   /// Description of a keyboard input we want to ignore.
   struct KeyboardInputExclusion
   {
      KeyboardInputExclusion()
      {
         key = 0;
         orModifierMask = 0;
         andModifierMask = 0;
      }

      /// The key code to ignore, e.g. KEY_TAB. If this and the other
      /// conditions match, ignore the key.
      S32 key;

      /// if(modifiers | orModifierMask) and the other conditions match,
      /// ignore the key.
      U32 orModifierMask;

      /// if((modifiers & andModifierMask) == andModifierMask) and the
      /// other conditions match, ignore the key stroke.
      U32 andModifierMask;

      /// Based on the values above, determine if a given input event
      /// matchs this exclusion rule.
      const bool checkAgainstInput(const InputEventInfo *info) const;
   };

   /// Reset the keyboard input exclusion list.
   static void clearKeyboardInputExclusion();
   
   /// Add a new keyboard exclusion.
   static void addKeyboardInputExclusion(const KeyboardInputExclusion &kie);

   /// Check if a given input event should be excluded.
   static const bool checkKeyboardInputExclusion(const InputEventInfo *info);
};

//------------------------------------------------------------------------------
// Unicode string conversions
// UNICODE is a windows platform API switching flag. Don't define it on other platforms.
#ifdef UNICODE
#define dT(s)    L##s
#else
#define dT(s)    s
#endif

//------------------------------------------------------------------------------
// Misc StdLib functions
#define QSORT_CALLBACK FN_CDECL
inline void dQsort(void *base, U32 nelem, U32 width, int (QSORT_CALLBACK *fcmp)(const void *, const void *))
{
   qsort(base, nelem, width, fcmp);
}

extern void* dMalloc_r(dsize_t in_size, const char*, const dsize_t);
extern void* dRealloc_r(void* in_pResize, dsize_t in_size, const char*, const dsize_t);
extern void* dRealMalloc(dsize_t);
extern void  dRealFree(void*);

extern void* dMemcpy(void *dst, const void *src, dsize_t size);
extern void* dMemmove(void *dst, const void *src, dsize_t size);
extern void* dMemset(void *dst, int c, dsize_t size);
extern int   dMemcmp(const void *ptr1, const void *ptr2, dsize_t size);


//------------------------------------------------------------------------------
// FileIO functions
extern bool dFileDelete(const char *name);
extern bool dFileRename(const char *oldName, const char *newName);
extern bool dFileTouch(const char *name);
extern bool dPathCopy(const char *fromName, const char *toName, bool nooverwrite = true);

typedef void* FILE_HANDLE;
enum DFILE_STATUS
{
   DFILE_OK = 1
};

extern FILE_HANDLE dOpenFileRead(const char *name, DFILE_STATUS &error);
extern FILE_HANDLE dOpenFileReadWrite(const char *name, bool append, DFILE_STATUS &error);
extern int dFileRead(FILE_HANDLE handle, U32 bytes, char *dst, DFILE_STATUS &error);
extern int dFileWrite(FILE_HANDLE handle, U32 bytes, const char *dst, DFILE_STATUS &error);
extern void dFileClose(FILE_HANDLE handle);

extern StringTableEntry osGetTemporaryDirectory();

//------------------------------------------------------------------------------
struct Math
{
   /// Initialize the math library with the appropriate libraries
   /// to support hardware acceleration features.
   ///
   /// @param properties Leave zero to detect available hardware. Otherwise,
   ///                   pass CPU instruction set flags that you want to load
   ///                   support for.
   static void init(U32 properties = 0);
};

extern void InitMemManager(void);


//#pragma once
//
//#ifndef RC_INVOKED
//// Avoid problems with the resource compiler if included
//
//// This defines bock the creation in the header files
//#pragma message("Using private assemblies for the MS runtimes")
//#define _STL_NOFORCE_MANIFEST
//#define _CRT_NOFORCE_MANIFEST
//#define _AFX_NOFORCE_MANIFEST
////#define _ATL_NOFORCE_MANIFEST
//
//// The next statements block the linker from including object files in the
//// CRT and the MFC, that would create manifest pragmas too.
//#ifdef __cplusplus
//extern "C" {            /* Assume C declarations for C++ */
//#endif
//
//	__declspec(selectany)		int _forceCRTManifest;
//	__declspec(selectany)		int _forceMFCManifest;
//	// __declspec(selectany)	int _forceAtlDllManifest;
//
//#ifdef __cplusplus
//}						/* __cplusplus */
//#endif
//
//// We use crtassem.h with the defines there. It just gives us the
//// versions and name parts for the dependencies.
//// Note that there is also a MFCassem.h but this include file has the
//// manifest pragma's already in it. So we can't use it
////
//// Three files are contrlling this crtassem.h, MFCassem.h and atlassem.h!
//// Happily __LIBRARIES_ASSEMBLY_NAME_PREFIX is used in CRT, MFC and ATL!
//// Doing it right would need to use _MFC_ASSEMBLY_VERSION for the MFC
//// but in fact _CRT_ASSEMBLY_VERSION and _MFC_ASSEMBLY_VERSION and
//// _ATL_ASSEMBLY_VERSION arethe same (VC-2005 SP1 8.0.50727.762)
//
//#include <crtassem.h>
//
//// We don't have a seperate block for the Debug version. We just handle
//// this with a extra define here.
//#ifdef _DEBUG
//#define __LIBRARIES_SUB_VERSION	"Debug"
//#else
//#define __LIBRARIES_SUB_VERSION	""
//#endif
//
//// Manifest for the CRT
//#pragma comment(linker,"/manifestdependency:\"type='win32' "						\
//	"name='" __LIBRARIES_ASSEMBLY_NAME_PREFIX "." __LIBRARIES_SUB_VERSION "CRT' "   \
//	"version='" _CRT_ASSEMBLY_VERSION "' "											\
//	"processorArchitecture='x86' \"")
//
//// Manifest for the MFC
////#pragma comment(linker,"/manifestdependency:\"type='win32' "						\
////	"name='" __LIBRARIES_ASSEMBLY_NAME_PREFIX "." __LIBRARIES_SUB_VERSION "MFC' "   \
////	"version='" _CRT_ASSEMBLY_VERSION "' "											\
////	"processorArchitecture='x86'\"")
//
//// #pragma comment(linker,"/manifestdependency:\"type='win32' "						\
//// 	"name='" __LIBRARIES_ASSEMBLY_NAME_PREFIX ".MFCLOC' "							\
//// 	"version='" _CRT_ASSEMBLY_VERSION "' "											\
//// 	"processorArchitecture='x86'\"")
//
//// Manifest for the ATL
//// #pragma comment(linker,"/manifestdependency:\"type='win32' "						\
////	"name='" __LIBRARIES_ASSEMBLY_NAME_PREFIX ".ATL' "								\
////	"version='" _CRT_ASSEMBLY_VERSION "' "											\
////	"processorArchitecture='x86' \"")
//
//#endif // RC_INVOKED

#endif


