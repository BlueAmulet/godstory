//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PLATFORM_STRINGS_H_
#define _PLATFORM_STRINGS_H_

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "BuildPrefix.h"
#include "platform/types.h"


#if defined(POWER_OS_WIN32) || defined(POWER_OS_XBOX) || defined(POWER_OS_XENON)
// These standard functions are not defined on Win32 and other Microsoft platforms...
#define strcasecmp   _stricmp
#define strncasecmp  _strnicmp
#define strtof       (double)strtod
#endif


//------------------------------------------------------------------------------
// standard string functions [defined in platformString.cpp]

inline char *dStrcat(char *dst, dsize_t dstSize, const char *src)
{
   char *ret = NULL;

   if (strcat_s(dst, dstSize, src) == 0)
   {
		ret = dst;
   }

   return ret;
}   

inline char *dStrncat(char *dst, dsize_t dstSize, const char *src, dsize_t len)
{
   char *ret = NULL;

   if (strncat_s(dst, dstSize, src, len) == 0)
   {
	   ret = dst;
   }

   return ret;
}

inline int  dStrcmp(const char *str1, const char *str2)
{
   return strcmp(str1, str2);   
}

inline int  dStrncmp(const char *str1, const char *str2, dsize_t len)
{
   return strncmp(str1, str2, len);   
}  

inline int  dStricmp(const char *str1, const char *str2)
{
   return strcasecmp( str1, str2 );
}

inline int  dStrnicmp(const char *str1, const char *str2, dsize_t len)
{
   return strncasecmp( str1, str2, len );
}

inline char *dStrcpy(char *dst, dsize_t dstSize, const char *src)
{
   if (strcpy_s(dst, dstSize, src) == 0)
	   return dst;

   return NULL;
}   

inline char *dStrncpy(char *dst, dsize_t dstSize, const char *src, dsize_t len)
{
	if (strncpy_s(dst, dstSize, src, len) == 0)
		return dst;

	return NULL;
}

inline wchar_t *dWcscpy(wchar_t *dst, dsize_t dstSize, const wchar_t *src )
{
	if (wcscpy_s(dst, dstSize, src) == 0)
		return dst;

	return NULL;
}

inline wchar_t *dWcsncpy(wchar_t *dst, dsize_t dstSize, const wchar_t *src, dsize_t len)
{
	if (wcsncpy_s(dst, dstSize, src, len) == 0)
		return dst;

	return NULL;
}

inline dsize_t dStrlen(const char *str)
{
   return (dsize_t)strlen(str);
}   

inline char *dStrchr(char *str, int c)
{
   return strchr(str,c);
}   

inline const char *dStrchr(const char *str, int c)
{
   return strchr(str,c);
}   

inline char *dStrrchr(char *str, int c)
{
   return strrchr(str,c);
}

inline const char *dStrrchr(const char *str, int c)
{
   return strrchr(str,c);
}   

inline dsize_t dStrspn(const char *str, const char *set)
{
   return (dsize_t)strspn(str, set);
}

inline dsize_t dStrcspn(const char *str, const char *set)
{
   return (dsize_t)strcspn(str, set);
}   

inline char *dStrstr(const char *str1, const char *str2)
{
   return strstr((char *)str1,str2);
}   


inline char *dStrtok(char *str, const char *sep, char **context)
{
	return strtok_s(str, sep, context);
}

inline S32 dAtoi(const char *str)
{
   return strtol(str, NULL, 10);
}

inline U32 dAtol(const char *str)
{
	return strtoul(str, NULL, 10);
}

inline F64 dAtof(const char *str)
{
   return strtod(str, NULL);
}

inline char dToupper(const char c)
{
   return toupper( (int)c );
}

inline char dTolower(const char c)
{
   return tolower( (int)c );
}

inline bool dIsalnum(const char c)
{
   return isalnum((int)c);
}

inline bool dIsalpha(const char c)
{
   return isalpha((int)c);
}

inline bool dIsspace(const char c)
{
   return isspace((int)c);
}

inline bool dIsdigit(const char c)
{
   return isdigit((int)c);
}


//------------------------------------------------------------------------------
// non-standard string functions [defined in platformString.cpp]

#define dStrdup(x) dStrdup_r(x, __FILE__, __LINE__)
extern char* dStrdup_r(const char *src, const char*, dsize_t);

extern char*       dStrcpyl(char *dst, dsize_t dstSize, ...);
extern char*       dStrcatl(char *dst, dsize_t dstSize, ...);

extern int         dStrcmp(const UTF16 *str1, const UTF16 *str2);

extern char*       dStrupr(char *str);
extern char*       dStrlwr(char *str);

inline bool dAtob(const char *str)
{
   return !dStricmp(str, "true") || dAtof(str);
}

//------------------------------------------------------------------------------
// standard I/O functions [defined in platformString.cpp]

extern void   dPrintf(const char *format, ...);
extern int    dVprintf(const char *format, void *arglist);
extern int    dSprintf(char *buffer, U32 bufferSize, const char *format, ...);
extern int    dVsprintf(char *buffer, U32 bufferSize, const char *format, void *arglist);
extern int    dSscanf(const char *buffer, const char *format, ...);

#endif
