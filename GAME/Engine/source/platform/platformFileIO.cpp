//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "core/tVector.h"
#include "console/console.h"
#include "core/stringTable.h"
#include "core/resManager.h"
#include "util/tempAlloc.h"

//-----------------------------------------------------------------------------

StringTableEntry Platform::getTemporaryDirectory()
{
   StringTableEntry path = osGetTemporaryDirectory();

   if(! Platform::isDirectory(path))
      path = Platform::getCurrentDirectory();

   return path;
}

ConsoleFunction(getTemporaryDirectory, const char *, 1, 1, "()")
{
   return Platform::getTemporaryDirectory();
}

StringTableEntry Platform::getTemporaryFileName()
{
   char buf[512];
   StringTableEntry path = Platform::getTemporaryDirectory();

   dSprintf(buf, sizeof(buf), "%s/tgb.%08x.%02x.tmp", path, Platform::getRealMilliseconds(), U32(Platform::getRandom() * 255));

   // [tom, 9/7/2006] This shouldn't be needed, but just in case
   if(Platform::isFile(buf))
      return Platform::getTemporaryFileName();

   return StringTable->insert(buf);
}

ConsoleFunction(getTemporaryFileName, const char *, 1, 1, "()")
{
   return Platform::getTemporaryFileName();
}

//-----------------------------------------------------------------------------

static StringTableEntry sgMainCSDir = NULL;

StringTableEntry Platform::getMainDotCsDir()
{
   if(sgMainCSDir == NULL)
      sgMainCSDir = Platform::getExecutablePath();

   return sgMainCSDir;
}

void Platform::setMainDotCsDir(const char *dir)
{
   sgMainCSDir = StringTable->insert(dir);
}

//-----------------------------------------------------------------------------

typedef Vector<char*> CharVector;
static CharVector gPlatformDirectoryExcludeList;

void Platform::addExcludedDirectory(const char *pDir)
{
   gPlatformDirectoryExcludeList.push_back(dStrdup(pDir));
}

void Platform::clearExcludedDirectories()
{
   while(gPlatformDirectoryExcludeList.size())
   {
      dFree(gPlatformDirectoryExcludeList.last());
      gPlatformDirectoryExcludeList.pop_back();
   }
}

bool Platform::isExcludedDirectory(const char *pDir)
{
   for(CharVector::iterator i=gPlatformDirectoryExcludeList.begin(); i!=gPlatformDirectoryExcludeList.end(); i++)
      if(!dStrcmp(pDir, *i))
         return true;

   return false;
}

//-----------------------------------------------------------------------------

inline void catPath(char *dst, const char *src, U32 len)
{
   if(*dst != '/')
   {
      ++dst; --len;
      *dst = '/';
   }

   ++dst; --len;

   dMemcpy(dst, src, len);
   dst[len - 1] = 0;
}

// converts the posix root path "/" to "c:/" for win32
// FIXME: this is not ideal. the c: drive is not guaranteed to exist.
#if defined(POWER_OS_WIN32)
static inline void _resolveLeadingSlash(char* buf, U32 size)
{
   if(buf[0] != '/')
      return;

   AssertFatal(dStrlen(buf) + 2 < size, "Expanded path would be too long");
   dMemmove(buf + 2, buf, dStrlen(buf));
   buf[0] = 'c';
   buf[1] = ':';
}
#endif

static void makeCleanPathInPlace( char* path )
{
   U32 pathDepth = 0;
   char* fromPtr = path;
   char* toPtr = path;

   bool isAbsolute = false;
   if( *fromPtr == '/' )
   {
      fromPtr ++;
      toPtr ++;
      isAbsolute = true;
   }
   else if( fromPtr[ 0 ] != '\0' && fromPtr[ 1 ] == ':' )
   {
      toPtr += 3;
      fromPtr += 3;
      isAbsolute = true;
   }

   while( *fromPtr )
   {
      if( fromPtr[ 0 ] == '.' && fromPtr[ 1 ] == '.' && fromPtr[ 2 ] == '/' )
      {
         // Back up from '../'

         if( pathDepth > 0 )
         {
            pathDepth --;
            toPtr -= 2;
            while( toPtr >= path && *toPtr != '/' )
               toPtr --;
            toPtr ++;
         }
         else if( !isAbsolute )
         {
            dMemcpy( toPtr, fromPtr, 3 );
            toPtr += 3;
         }

         fromPtr += 3;
      }
      else if( fromPtr[ 0 ] == '.' && fromPtr[ 1 ] == '/' )
      {
         // Ignore.
         fromPtr += 2;
      }
      else
      {
         if( fromPtr[ 0 ] == '/' )
            pathDepth ++;

         *toPtr ++ = *fromPtr ++;
      }
   }

   *toPtr = '\0';
}

void Platform::AdjustCMPfileName(char *path)
{
	int Len = dStrlen(path);

	int dl = 0;
	char met[1024];

	for(int i=Len-1;i>0;i--,dl++)
	{
		if(path[i] == '.')
		{
			dStrncpy(met, sizeof(met), &path[i],dl+1);
			met[dl+1]= 0;

			if(!dStricmp(met,DSO))
			{
				dl = -1;
			}
			else if(!dStricmp(met,".cs"))
			{
				dMemcpy(&path[i], DSO, dStrlen(DSO) + 1);
				return;
			}
			else if(!dStricmp(met,".gui"))
			{
				dMemcpy(&path[i], GDSO, dStrlen(GDSO) + 1);
				return;
			}
		}
	}
}

char * Platform::makeFullPathName(const char *path, char *buffer, U32 size, const char *cwd /* = NULL */)
{
   char bspath[1024];
   dStrncpy(bspath, sizeof(bspath), path, sizeof(bspath));
   bspath[sizeof(bspath)-1] = 0;
   int len = dStrlen(bspath);
  
   for(S32 i = 0;i < len;++i)
   {
      if(bspath[i] == '\\')
         bspath[i] = '/';
   }

   if(Platform::isFullPath(bspath))
   {
      // Already a full path
      #if defined(POWER_OS_WIN32)
         _resolveLeadingSlash(bspath, sizeof(bspath));
      #endif
      dStrncpy(buffer, size, bspath, size);
      buffer[size-1] = 0;
      return buffer;
   }

   // [rene, 05/05/2008] Based on overall file handling in PowerEngine, it does not seem to make
   //    that much sense to me to base things off the current working directory here.

   if(cwd == NULL)
      cwd = Con::isCurrentScriptToolScript() ? Platform::getMainDotCsDir() : Platform::getCurrentDirectory();

   dStrncpy(buffer, size, cwd, size);
   buffer[size-1] = 0;

   const char* defaultDir = Con::getVariable("defaultGame");

   char *ptr = bspath;
   char *slash = NULL;
   char *endptr = buffer + dStrlen(buffer) - 1;

   do
   {
      slash = dStrchr(ptr, '/');
      if(slash)
      {
         *slash = 0;

         // Directory

         if(dStrcmp(ptr, "..") == 0)
         {
            // Parent
            endptr = dStrrchr(buffer, '/');
         }
         else if(dStrcmp(ptr, ".") == 0)
         {
            // Current dir
         }
         else if(dStrcmp(ptr, "~") == 0)
         {
            catPath(endptr, defaultDir, size - (endptr - buffer));
            endptr += dStrlen(endptr) - 1;
         }
         else if(endptr)
         {
            catPath(endptr, ptr, size - (endptr - buffer));
            endptr += dStrlen(endptr) - 1;
         }
         
         ptr = slash + 1;
      }
      else if(endptr)
      {
         // File

         catPath(endptr, ptr, size - (endptr - buffer));
         endptr += dStrlen(endptr) - 1;
      }

   } while(slash);

   return buffer;
}

bool Platform::isFullPath(const char *path)
{
   // Quick way out
   if(path[0] == '/' || path[1] == ':')
      return true;

   return false;
}

//-----------------------------------------------------------------------------

StringTableEntry Platform::makeRelativePathName(const char *path, const char *to)
{
   // Make sure 'to' is a proper absolute path terminated with a forward slash.

   char buffer[ 1024 ];
   if( !Platform::isFullPath( to ) )
   {
      dSprintf( buffer, sizeof( buffer ), "%s/%s/", Platform::getMainDotCsDir(), to );
      makeCleanPathInPlace( buffer );
      to = buffer;
   }
   else if( to[ dStrlen( to ) - 1 ] != '/' )
   {
      U32 length = getMin( dStrlen( to ), sizeof( buffer ) - 2 );
      dMemcpy( buffer, to, length );
      buffer[ length ] = '/';
      buffer[ length + 1 ] = '\0';
      to = buffer;
   }

   // If 'path' isn't absolute, make it now.  Let's us use a single
   // absolute/absolute merge path from here on.

   char buffer2[ 1024 ];
   if( !Platform::isFullPath( path ) )
   {
      dSprintf( buffer2, sizeof( buffer2 ), "%s/%s", Platform::getMainDotCsDir(), path );
      makeCleanPathInPlace( buffer2 );
      path = buffer2;
   }

   // First, find the common prefix and see where 'path' branches off from 'to'.

   const char *pathPtr, *toPtr, *branch = path;
   for(pathPtr = path, toPtr = to;*pathPtr && *toPtr && dTolower(*pathPtr) == dTolower(*toPtr);++pathPtr, ++toPtr)
   {
      if(*pathPtr == '/')
         branch = pathPtr;
   }

   // If there's no common part, the two paths are on different drives and
   // there's nothing we can do.

   if( pathPtr == path )
      return StringTable->insert( path );

   // If 'path' and 'to' are identical (minus trailing slash or so), we can just return './'.

   else if((*pathPtr == 0 || (*pathPtr == '/' && *(pathPtr + 1) == 0)) &&
      (*toPtr == 0 || (*toPtr == '/' && *(toPtr + 1) == 0)))
   {
      char* bufPtr = buffer;
      *bufPtr ++ = '.';

      if(*pathPtr == '/' || *(pathPtr - 1) == '/')
         *bufPtr++ = '/';

      *bufPtr = 0;
      return StringTable->insert(buffer);
   }

   // If 'to' is a proper prefix of 'path', the remainder of 'path' is our relative path.

   else if( *toPtr == '\0' && toPtr[ -1 ] == '/' )
      return StringTable->insert( pathPtr );

   // Otherwise have to step up the remaining directories in 'to' and then
   // append the remainder of 'path'.

   else
   {
      if((*pathPtr == 0 && *toPtr == '/') || (*toPtr == '/' && *pathPtr == 0))
         branch = pathPtr;

      // Allocate a new temp so we aren't prone to buffer overruns.

      TempAlloc< char > temp( dStrlen( toPtr ) + dStrlen( branch ) + 1 );
      char* bufPtr = temp;

      // Figure out parent dirs

      for(toPtr = to + (branch - path);*toPtr;++toPtr)
      {
         if(*toPtr == '/' && *(toPtr + 1) != 0)
         {
            *bufPtr++ = '.';
            *bufPtr++ = '.';
            *bufPtr++ = '/';
         }
      }
      *bufPtr = 0;

      // Copy the rest
      if(*branch)
         dMemcpy(bufPtr, branch + 1, dStrlen(branch + 1) + 1);
      else
         *--bufPtr = 0;

      return StringTable->insert( temp );
   }
}

//-----------------------------------------------------------------------------

static StringTableEntry tryStripBasePath(const char *path, const char *base)
{
   U32 len = dStrlen(base);
   if(dStrnicmp(path, base, len) == 0)
   {
      if(*(path + len) == '/') ++len;
      return StringTable->insert(path + len, true);
   }
   return NULL;
}

StringTableEntry Platform::stripBasePath(const char *path)
{
   StringTableEntry str = tryStripBasePath(path, Platform::getMainDotCsDir());
   
   if(str != NULL )
      return str;

   str = tryStripBasePath(path, Platform::getCurrentDirectory());
   if(str != NULL )
      return str;

   str = tryStripBasePath(path, Platform::getPrefsPath());
   if(str != NULL )
      return str;

   return path;
}

//-----------------------------------------------------------------------------

StringTableEntry Platform::getPrefsPath(const char *file /* = NULL */)
{
#ifdef POWER_PLAYER
   return StringTable->insert(file ? file : "");
#else
   char buf[1024];
   const char *company = Con::getVariable("$Game::CompanyName");
   if(company == NULL || *company == 0)
      company = "FairRain";

   const char *appName = Con::getVariable("$Game::GameName");
   if(appName == NULL || *appName == 0)
      appName = POWER_APP_NAME;

   if(file)
   {
      if(dStrstr(file, ".."))
      {
         Con::errorf("getPrefsPath - filename cannot be relative");
         return NULL;
      }

      dSprintf(buf, sizeof(buf), "%s/%s/%s/%s", Platform::getUserDataDirectory(), company, appName, file);
   }
   else
      dSprintf(buf, sizeof(buf), "%s/%s/%s", Platform::getUserDataDirectory(), company, appName);

   return StringTable->insert(buf, true);
#endif
}

//-----------------------------------------------------------------------------

ConsoleFunction(getUserDataDirectory, const char*, 1, 1, "getUserDataDirectory()")
{
   return Platform::getUserDataDirectory();
}

ConsoleFunction(getUserHomeDirectory, const char*, 1, 1, "getUserHomeDirectory()")
{
   return Platform::getUserHomeDirectory();
}
