/**********************************************************************

Filename    :   GStd.h
Content     :   Standard C function interface
Created     :   2/25/2007
Authors     :   Ankur Mohan, Michael Antonov, Artem Bolgar
Copyright   :   (c) 2007 Scaleform Corp. All Rights Reserved.

This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR ANY PURPOSE.

**********************************************************************/

#ifndef INC_GStd_H
#define INC_GStd_H

#include "GTypes.h"
#include <stdarg.h> // for va_list args
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#if !defined(GFC_OS_WINCE) && defined(GFC_CC_MSVC) && (GFC_CC_MSVC >= 1400)
#define GFC_MSVC_SAFESTRING
#include <errno.h>
#endif


#if defined(GFC_OS_WIN32)
inline char* gfc_itoa(int val, char *dest, size_t destsize, int radix)
{
#if defined(GFC_MSVC_SAFESTRING)
    _itoa_s(val, dest, destsize, radix);
    return dest;
#else
    GUNUSED(destsize);
    return itoa(val, dest, radix);
#endif
}
#endif // GFC_OS_WIN32

// String functions

inline size_t gfc_strlen(const char* str)
{
    return strlen(str);
}

inline char* gfc_strcpy(char* dest, size_t destsize, const char* src)
{
#if defined(GFC_MSVC_SAFESTRING)
    strcpy_s(dest, destsize, src);
    return dest;
#else
    GUNUSED(destsize);
    return strcpy(dest, src);
#endif
}

inline char* gfc_strncpy(char* dest, size_t destsize, const char* src, size_t count)
{
#if defined(GFC_MSVC_SAFESTRING)
    strncpy_s(dest, destsize, src, count);
    return dest;
#else
    GUNUSED(destsize);
    return strncpy(dest, src, count);
#endif
}

inline char * gfc_strcat(char* dest, size_t destsize, const char* src)
{
#if defined(GFC_MSVC_SAFESTRING)
    strcat_s(dest, destsize, src);
    return dest;
#else
    GUNUSED(destsize);
    return strcat(dest, src);
#endif
}

inline int gfc_strcmp(const char* dest, const char* src)
{
    return strcmp(dest, src);
}

inline const char* gfc_strchr(const char* str, char c)
{
    return strchr(str, c);
}

inline char* gfc_strchr(char* str, char c)
{
    return strchr(str, c);
}


inline double gfc_strtod(const char* string, char** tailptr)
{
    return strtod(string, tailptr);
}

inline long gfc_strtol(const char* string, char** tailptr, int radix)
{
    return strtol(string, tailptr, radix);
}

inline long gfc_strtoul(const char* string, char** tailptr, int radix)
{
    return strtoul(string, tailptr, radix);
}

inline int gfc_strncmp(const char* ws1, const char* ws2, size_t size)
{
    return strncmp(ws1, ws2, size);
}

inline UInt64 gfc_strtouq(const char *nptr, char **endptr, int base)
{
#if defined(GFC_CC_MSVC) && !defined(GFC_OS_WINCE)
    return _strtoui64(nptr, endptr, base);
#elif (defined(GFC_OS_PS2) && defined(GFC_CC_MWERKS)) || (defined(GFC_OS_PSP) && defined(GFC_CC_SNC)) || defined(GFC_OS_WINCE)
    return strtoul(nptr, endptr, base);
#else
    return strtoull(nptr, endptr, base);
#endif
}

inline SInt64 gfc_strtoq(const char *nptr, char **endptr, int base)
{
#if defined(GFC_CC_MSVC) && !defined(GFC_OS_WINCE)
    return _strtoi64(nptr, endptr, base);
#elif (defined(GFC_OS_PS2) && defined(GFC_CC_MWERKS)) || (defined(GFC_OS_PSP) && defined(GFC_CC_SNC)) || defined(GFC_OS_WINCE)
    return strtol(nptr, endptr, base);
#else
    return strtoll(nptr, endptr, base);
#endif
}


inline SInt64 gfc_atoq(const char* string)
{
#if defined(GFC_CC_MSVC) && !defined(GFC_OS_WINCE)
    return _atoi64(string);
#elif (defined(GFC_OS_PS2) && defined(GFC_CC_MWERKS)) || (defined(GFC_OS_PSP) && defined(GFC_CC_SNC)) || defined(GFC_OS_WINCE)
    return strtol(string, NULL, 10);
#elif defined(GFC_OS_PS2)
    return strtoll(string, NULL, 10);
#else
    return atoll(string);
#endif
}

inline UInt64 gfc_atouq(const char* string)
{
  return gfc_strtouq(string, NULL, 10);
}


// Implemented in GStd.cpp in platform-specific manner.
int gfc_stricmp(const char* dest, const char* src);
int gfc_strnicmp(const char* dest, const char* src, size_t count);

inline size_t gfc_sprintf(char *dest, size_t destsize, const char* format, ...)
{
    va_list argList;
    va_start(argList,format);
    size_t ret;
#if defined(GFC_CC_MSVC)
    #if defined(GFC_MSVC_SAFESTRING)
        ret = _vsnprintf_s(dest, destsize, _TRUNCATE, format, argList);
        GASSERT(ret != -1);
    #else
        GUNUSED(destsize);
        ret = _vsnprintf(dest, destsize - 1, format, argList); // -1 for space for the null character
        GASSERT(ret != -1);
        dest[destsize-1] = 0;
    #endif
#elif defined(GFC_OS_LINUX) || defined(GFC_OS_MAC)
    GUNUSED(destsize);
    ret = vsnprintf(dest, destsize, format, argList);
    GASSERT(ret < destsize);
#else
    GUNUSED(destsize);
    ret = vsprintf(dest, format, argList);
    GASSERT(ret < destsize);
#endif
    va_end(argList);
    return ret;
}

inline size_t gfc_vsprintf(char *dest, size_t destsize, const char * format, va_list argList)
{
    size_t ret;
#if defined(GFC_CC_MSVC)
    #if defined(GFC_MSVC_SAFESTRING)
        dest[0] = '\0';
        int rv = vsnprintf_s(dest, destsize, _TRUNCATE, format, argList);
        if (rv == -1)
        {
            dest[destsize - 1] = '\0';
            ret = destsize - 1;
        }
        else
            ret = (size_t)rv;
    #else
        GUNUSED(destsize);
        int rv = _vsnprintf(dest, destsize - 1, format, argList);
        GASSERT(rv != -1);
        ret = (size_t)rv;
        dest[destsize-1] = 0;
    #endif
#elif defined(GFC_OS_LINUX) || defined(GFC_OS_MAC)
    GUNUSED(destsize);
    ret = (size_t)vsnprintf(dest, destsize, format, argList);
    GASSERT(ret < destsize);
#else
    GUNUSED(destsize);
    ret = (size_t)vsprintf(dest, format, argList);
    GASSERT(ret < destsize);
#endif
    return ret;
}

// Wide-char funcs
#if !defined(GFC_OS_SYMBIAN) && !defined(GFC_CC_RENESAS) && !defined(GFC_OS_PS2)
# include <wchar.h>
#endif

#if !defined(GFC_OS_SYMBIAN) && !defined(GFC_CC_RENESAS) && !defined(GFC_OS_PS2) && !defined(GFC_CC_SNC)
# include <wctype.h>
#endif

wchar_t*    gfc_wcscpy(wchar_t* dest, size_t destsize, const wchar_t* src);
wchar_t*    gfc_wcscat(wchar_t* dest, size_t destsize, const wchar_t* src);
size_t      gfc_wcslen(const wchar_t* str);

inline int  gfc_iswspace(wchar_t c)
{
#if defined(GFC_OS_SYMBIAN) || defined(GFC_OS_WII) || defined(GFC_CC_RENESAS) || defined(GFC_OS_PS2) || defined(GFC_CC_SNC)
    return ((c) < 128 ? isspace((char)c) : 0);
#else
    return iswspace(c);
#endif
}

inline int  gfc_iswdigit(wchar_t c)
{
#if defined(GFC_OS_SYMBIAN) || defined(GFC_OS_WII) || defined(GFC_CC_RENESAS) || defined(GFC_OS_PS2) || defined(GFC_CC_SNC)
    return ((c) < 128 ? isdigit((char)c) : 0);
#else
    return iswdigit(c);
#endif
}

inline int  gfc_iswxdigit(wchar_t c)
{
#if defined(GFC_OS_SYMBIAN) || defined(GFC_OS_WII) || defined(GFC_CC_RENESAS) || defined(GFC_OS_PS2) || defined(GFC_CC_SNC)
    return ((c) < 128 ? isxdigit((char)c) : 0);
#else
    return iswxdigit(c);
#endif
}

inline int  gfc_iswalpha(wchar_t c)
{
#if defined(GFC_OS_SYMBIAN) || defined(GFC_OS_WII) || defined(GFC_CC_RENESAS) || defined(GFC_OS_PS2) || defined(GFC_CC_SNC)
    return ((c) < 128 ? isalpha((char)c) : 0);
#else
    return iswalpha(c);
#endif
}

inline int  gfc_iswalnum(wchar_t c)
{
#if defined(GFC_OS_SYMBIAN) || defined(GFC_OS_WII) || defined(GFC_CC_RENESAS) || defined(GFC_OS_PS2) || defined(GFC_CC_SNC)
    return ((c) < 128 ? isalnum((char)c) : 0);
#else
    return iswalnum(c);
#endif
}

inline wchar_t gfc_towlower(wchar_t c)
{
#if defined(GFC_OS_SYMBIAN) || defined(GFC_CC_RENESAS) || defined(GFC_OS_PS2) || (defined(GFC_OS_PS3) && defined(GFC_CC_SNC))
    return (wchar_t)tolower((char)c);
#else
    return (wchar_t)towlower(c);
#endif
}

inline wchar_t gfc_towupper(wchar_t c)
{
#if defined(GFC_OS_SYMBIAN) || defined(GFC_CC_RENESAS) || defined(GFC_OS_PS2) || (defined(GFC_OS_PS3) && defined(GFC_CC_SNC))
    return (wchar_t)toupper((char)c);
#else
    return (wchar_t)towupper(c);
#endif
}


inline double gfc_wcstod(const wchar_t* string, wchar_t** tailptr)
{
#if defined(GFC_OS_PSP) || defined (GFC_OS_PS2) || defined(GFC_OS_OTHER)
    GUNUSED(tailptr);
    char buffer[64];
    char* tp = NULL;
    size_t max = gfc_wcslen(string);
    if (max > 63) max = 63;
    unsigned char c = 0;
    for (size_t i=0; i < max; i++)
    {
        c = (unsigned char)string[i];
        buffer[i] = ((c) < 128 ? (char)c : '!');
    }
    buffer[max] = 0;
    return strtod(buffer, &tp);
#else
    return wcstod(string, tailptr);
#endif
}

inline long gfc_wcstol(const wchar_t* string, wchar_t** tailptr, int radix)
{
#if defined(GFC_OS_PSP) || defined(GFC_OS_PS2) || defined(GFC_OS_OTHER)
    GUNUSED(tailptr);
    char buffer[64];
    char* tp = NULL;
    size_t max = gfc_wcslen(string);
    if (max > 63) max = 63;
    unsigned char c = 0;
    for (size_t i=0; i < max; i++)
    {
        c = (unsigned char)string[i];
        buffer[i] = ((c) < 128 ? (char)c : '!');
    }
    buffer[max] = 0;
    return strtol(buffer, &tp, radix);
#else
    return wcstol(string, tailptr, radix);
#endif
}

#endif // INC_GSTD_H
