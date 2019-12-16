//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/ast.h"
#include "core/resManager.h"
#include "core/fileStream.h"
#include "console/compiler.h"
#include "platform/event.h"
#include "platform/platformInput.h"
#include "platform/EngineConfig.h"
#include "core/frameAllocator.h"


// Buffer for expanding script filenames.
static char scriptFilenameBuffer[1024];

//-------------------------------------- Helper Functions
static void forwardslash(char *str)
{
   while(*str)
   {
      if(*str == '\\')
         *str = '/';
      str++;
   }
}

//----------------------------------------------------------------
ConsoleFunctionGroupBegin( FileSystem, "Functions allowing you to search for files, read them, write them, and access their properties.");

static ResourceObject *firstMatch = NULL;

ConsoleFunction(findFirstFile, const char *, 2, 2, "(string pattern) Returns the first file in the directory system matching the given pattern.")
{
   argc;
   const char *fn;
   firstMatch = NULL;
   if(Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
   {
      char fullPath[1024];
      Platform::makeFullPathName(scriptFilenameBuffer, fullPath, sizeof(fullPath));
      firstMatch = ResourceManager->findMatch(fullPath, &fn, NULL);
   
      if(firstMatch)
         return fn;
   }

   return "";
}

ConsoleFunction(findNextFile, const char *, 2, 2, "(string pattern) Returns the next file matching a search begun in findFirstFile.")
{
   argc;
   const char *fn;
   if(Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
   {
      char fullPath[1024];
      Platform::makeFullPathName(scriptFilenameBuffer, fullPath, sizeof(fullPath));
      firstMatch = ResourceManager->findMatch(fullPath, &fn, firstMatch);
      if(firstMatch)
         return fn;
   }
   else
      firstMatch = NULL;
   return "";
}

ConsoleFunction(getFileCount, S32, 2, 2, "(string pattern)returns the number of files in the directory tree that match the given pattern")
{
   argc;
   const char* fn;
   U32 count = 0;
   firstMatch = ResourceManager->findMatch(argv[1], &fn, NULL);
   if ( firstMatch )
   {
      count++;
      while ( 1 )
      {
         firstMatch = ResourceManager->findMatch(argv[1], &fn, firstMatch);
         if ( firstMatch )
            count++;
         else
            break;
      }
   }

   return( count );
}

ConsoleFunction(findFirstFileMultiExpr, const char *, 2, 2, "(string pattern) Returns the first file in the directory system matching the given pattern.")
{
   argc;
   const char *fn;
   firstMatch = NULL;
   if(Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
   {
      char fullPath[1024];
      Platform::makeFullPathName(scriptFilenameBuffer, fullPath, sizeof(fullPath));
      firstMatch = ResourceManager->findMatchMultiExprs(fullPath, &fn, NULL);
      if(firstMatch)
         return fn;
   }
   return "";
}

ConsoleFunction(findNextFileMultiExpr, const char *, 2, 2, "(string pattern) Returns the next file matching a search begun in findFirstFile.")
{
   argc;
   const char *fn;
   if(Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
   {
      char fullPath[1024];
      Platform::makeFullPathName(scriptFilenameBuffer, fullPath, sizeof(fullPath));
      firstMatch = ResourceManager->findMatchMultiExprs(fullPath, &fn, firstMatch);
      if(firstMatch)
         return fn;
   }
   else
      firstMatch = NULL;
   return "";
}

ConsoleFunction(getFileCountMultiExpr, S32, 2, 2, "(string pattern)returns the number of files in the directory tree that match the given pattern")
{
   argc;
   const char* fn;
   U32 count = 0;
   firstMatch = ResourceManager->findMatchMultiExprs(argv[1], &fn, NULL);
   if ( firstMatch )
   {
      count++;
      while ( 1 )
      {
         firstMatch = ResourceManager->findMatchMultiExprs(argv[1], &fn, firstMatch);
         if ( firstMatch )
            count++;
         else
            break;
      }
   }

   return( count );
}

ConsoleFunction(getFileCRC, S32, 2, 2, "getFileCRC(filename)")
{
   argc;
   U32 crcVal;
   Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);

   if(!ResourceManager->getCrc(scriptFilenameBuffer, crcVal))
      return(-1);
   return(S32(crcVal));
}


ConsoleFunction(isFile, bool, 2, 2, "isFile(fileName)")
{
   argc;
   Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);
   return bool(ResourceManager->find(scriptFilenameBuffer));
}

ConsoleFunction( IsDirectory, bool, 2, 2, "( string: directory of form \"foo/bar\", do not include trailing /, case insensitive, directory must have files in it if you expect the directory to be in a zip )" )
{
   bool isDir = Platform::isDirectory( argv[1] );
   if (!isDir)
   {
      // may be in a zip, see if the resource manager knows about it
      // resource manager does not currently add directories to its dictionary, so we
      // interate through all the resources looking for files that have the target directory
      // as the path
      // WARNING: read the doc string so that you understand the limitations of this implementation
      Con::printf("IsDirectory: traversing all resources to look for directory");

      ResourceManager->startResourceTraverse();

      ResourceObject* obj = NULL;
      while (!isDir && (obj = ResourceManager->getNextResource()) != NULL)
      {
         if (obj->path != NULL && dStricmp(obj->path, argv[1]) == 0)
            isDir = true;
      }
   }
   return isDir;
}

ConsoleFunction(isWriteableFileName, bool, 2, 2, "isWriteableFileName(fileName)")
{
   argc;
   // in a writeable directory?
   Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);
   if(!ResourceManager->isValidWriteFileName(scriptFilenameBuffer))
      return(false);

   // exists?
   FileStream fs;
   if(!fs.open(scriptFilenameBuffer, FileStream::Read))
      return(true);

   // writeable? (ReadWrite will create file if it does not exist)
   fs.close();
   if(!fs.open(scriptFilenameBuffer, FileStream::ReadWrite))
      return(false);

   return(true);
}

ConsoleFunction(getDirectoryList, const char*, 2, 3, "getDirectoryList(%path, %depth)")
{
   // Grab the full path.
   char path[1024];
   Platform::makeFullPathName(dStrcmp(argv[1], "/") == 0 ? "" : argv[1], path, sizeof(path));

   //dSprintf(path, 511, "%s/%s", Platform::getWorkingDirectory(), argv[1]);

   // Append a trailing backslash if it's not present already.
   if (path[dStrlen(path) - 1] != '/')
   {
      S32 pos = dStrlen(path);
      path[pos] = '/';
      path[pos + 1] = '\0';
   }

   // Grab the depth to search.
   S32 depth = 0;
   if (argc > 2)
      depth = dAtoi(argv[2]);

   // Dump the directories.
   Vector<StringTableEntry> directories;
   Platform::dumpDirectories(path, directories, depth, true);

   if( directories.empty() )
      return "";

   // Grab the required buffer length.
   S32 length = 0;

   for (S32 i = 0; i < directories.size(); i++)
      length += dStrlen(directories[i]) + 1;

   // Get a return buffer.
   char* buffer = Con::getReturnBuffer(length);
   char* p = buffer;

   // Copy the directory names to the buffer.
   for (S32 i = 0; i < directories.size(); i++)
   {
      //dStrcpy(p, directories[i]);
      dMemcpy(p, directories[i], (U32)(strlen(directories[i]) + 1));
      p += dStrlen(directories[i]);
      // Tab separated.
      p[0] = '\t';
      p++;
   }
   p--;
   p[0] = '\0';

   return buffer;
}

ConsoleFunction(fileSize, S32, 2, 2, "fileSize(fileName) returns filesize or -1 if no file")
{
   argc;
   Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);
   return Platform::getFileSize( scriptFilenameBuffer );
}

ConsoleFunction(fileDelete, bool, 2,2, "fileDelete('path')")
{
   static char fileName[1024];
   static char sandboxFileName[1024];

   Con::expandScriptFilename( fileName, sizeof( fileName ), argv[1] );
   Platform::makeFullPathName(fileName, sandboxFileName, sizeof(sandboxFileName));

   return dFileDelete(sandboxFileName);
}


//----------------------------------------------------------------

ConsoleFunction(fileExt, const char *, 2, 2, "fileExt(fileName)")
{
   argc;
   const char *ret = dStrrchr(argv[1], '.');
   if(ret)
      return ret;
   return "";
}

ConsoleFunction(fileBase, const char *, 2, 2, "fileBase(fileName)")
{
   S32 length = 0;
   S32 pathLen = dStrlen( argv[1] );
   FrameTemp<char> szPathCopy( pathLen + 1);

   dStrcpy( szPathCopy, pathLen + 1, argv[1] );
   forwardslash( szPathCopy );

   argc;
   const char *path = dStrrchr(szPathCopy, '/');
   if(!path)
      path = szPathCopy;
   else
      path++;

   length = dStrlen(path) + 1;
   char *ret = Con::getReturnBuffer(length);
   dStrcpy(ret, length, path);
   char *ext = dStrrchr(ret, '.');
   if(ext)
      *ext = 0;
   return ret;
}

ConsoleFunction(fileName, const char *, 2, 2, "fileName(filePathName)")
{
   S32 length = 0;
   S32 pathLen = dStrlen( argv[1] );
   FrameTemp<char> szPathCopy( pathLen + 1 );

   dStrcpy( szPathCopy, pathLen + 1, argv[1] );
   forwardslash( szPathCopy );

   argc;
   const char *name = dStrrchr(szPathCopy, '/');
   if(!name)
      name = szPathCopy;
   else
      name++;

   length = dStrlen(name) + 1;
   char *ret = Con::getReturnBuffer(length);
   dStrcpy(ret, length, name);
   return ret;
}

ConsoleFunction(filePath, const char *, 2, 2, "filePath(fileName)")
{
   S32 pathLen = dStrlen( argv[1] );
   FrameTemp<char> szPathCopy( pathLen + 1);

   dStrcpy( szPathCopy, pathLen + 1, argv[1] );
   forwardslash( szPathCopy );

   argc;
   const char *path = dStrrchr(szPathCopy, '/');
   if(!path)
      return "";
   U32 len = path - (char*)szPathCopy;
   char *ret = Con::getReturnBuffer(len + 1);
   dMemcpy(ret, szPathCopy, len);
   ret[len] = 0;
   return ret;
}

ConsoleFunction(getWorkingDirectory, const char *, 1, 1, "alias to getCurrentDirectory()")
{
   return Platform::getCurrentDirectory();
}

//-----------------------------------------------------------------------------

// [tom, 5/1/2007] I changed these to be ordinary console functions as they
// are just string processing functions. They are needed by the 3D tools which
// are not currently built with POWER_TOOLS defined.

ConsoleFunction(makeFullPath, const char *, 2, 3, "(string path, [string currentWorkingDir])")
{
   char *buf = Con::getReturnBuffer(512);
   Platform::makeFullPathName(argv[1], buf, 512, argc > 2 ? argv[2] : NULL);
   return buf;
}

ConsoleFunction(makeRelativePath, const char *, 3, 3, "(string path, string to)")
{
   return Platform::makeRelativePathName(argv[1], argv[2]);
}

ConsoleFunction(pathConcat, const char *, 3, 0, "(string path, string file1, [... fileN])")
{
   char *buf = Con::getReturnBuffer(1024);
   char pathBuf[1024];
   dStrcpy(buf, 1024, argv[1]);

   // CodeReview [tom, 5/1/2007] I don't think this will work as expected with multiple file names

   for(S32 i = 2;i < argc;++i)
   {
      Platform::makeFullPathName(argv[i], pathBuf, 1024, buf);
      dStrcpy(buf, 1024, pathBuf);
   }
   return buf;
}

//-----------------------------------------------------------------------------

ConsoleFunction(getExecutableName, const char *, 1, 1, "getExecutableName()")
{
   return Platform::getExecutableName();
}

ConsoleFunction(getMainDotCsDir, const char *, 1, 1, "getExecutableName()")
{
   return Platform::getMainDotCsDir();
}
ConsoleFunction(GetTMonsterDataCount, int, 1,1, "GetTMonsterDataCount();get g_TMonsterDataRepository->m_Monstdata.size()")
{
	/*return g_TMonsterDataRepository.GetTMonsterDataCount();*/
	return NULL;
}

ConsoleFunction( createPath, bool, 2,2, "createPath(\"file name or path name\");  creates the path or path to the file name")
{
	static char pathName[1024];

	Con::expandScriptFilename( pathName, sizeof( pathName ), argv[1] );

	return Platform::createPath( pathName );
}
//-----------------------------------------------------------------------------
// Tools Only Functions
//-----------------------------------------------------------------------------

#ifdef POWER_TOOLS

ConsoleToolFunction(openFolder, void, 2 ,2,"openFolder(%path);")
{
   Platform::openFolder( argv[1] );
}

ConsoleToolFunction(pathCopy, bool, 3, 4, "pathCopy(fromFile, toFile [, nooverwrite = true])")
{
   bool nooverwrite = true;

   if( argc > 3 )
      nooverwrite = dAtob( argv[3] );

   static char fromFile[1024];
   static char toFile[1024];

   static char qualifiedFromFile[1024];
   static char qualifiedToFile[1024];

   Con::expandScriptFilename( fromFile, sizeof( fromFile ), argv[1] );
   Con::expandScriptFilename( toFile, sizeof( toFile ), argv[2] );

   Platform::makeFullPathName(fromFile, qualifiedFromFile, sizeof(qualifiedFromFile));
   Platform::makeFullPathName(toFile, qualifiedToFile, sizeof(qualifiedToFile));

   return dPathCopy( qualifiedFromFile, qualifiedToFile, nooverwrite );
}

ConsoleToolFunction(getCurrentDirectory, const char *, 1, 1, "getCurrentDirectory()")
{
   return Platform::getCurrentDirectory();
}

ConsoleToolFunction( setCurrentDirectory, bool, 2, 2, "setCurrentDirectory(absolutePathName)" )
{
   return Platform::setCurrentDirectory( StringTable->insert( argv[1] ) );

}

//ConsoleToolFunction( createPath, bool, 2,2, "createPath(\"file name or path name\");  creates the path or path to the file name")
//{
//   static char pathName[1024];
//
//   Con::expandScriptFilename( pathName, sizeof( pathName ), argv[1] );
//
//   return Platform::createPath( pathName );
//}
#endif // POWER_TOOLS

//-----------------------------------------------------------------------------

ConsoleFunctionGroupEnd( FileSystem );
