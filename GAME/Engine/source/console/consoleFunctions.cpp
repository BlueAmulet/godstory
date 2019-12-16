//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
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
#include "util/journal/journal.h"

// This is a temporary hack to get tools using the library to
// link in this module which contains no other references.
bool LinkConsoleFunctions = false;

// Buffer for expanding script filenames.
static char scriptFilenameBuffer[1024];

//----------------------------------------------------------------

ConsoleFunctionGroupBegin(StringFunctions, "General string manipulation functions.");

ConsoleFunction(strasc, int, 2, 2, "(char)")
{
   argc;
   return (int)(*argv[1]);
}

ConsoleFunction(strformat, const char *, 3, 3, "(string format, value)"
                "Formats the given given value as a string, given the printf-style format string.")
{
   argc;
   char* pBuffer = Con::getReturnBuffer(64);
   const char *pch = argv[1];

   pBuffer[0] = '\0';
   while (*pch != '\0' && *pch !='%')
      pch++;
   while (*pch != '\0' && !dIsalpha(*pch))
      pch++;
   if (*pch == '\0')
   {
      Con::errorf("strFormat: Invalid format string!\n");
      return pBuffer;
   }
   
   switch(*pch)
   {
      case 'c':
      case 'C':
      case 'd':
      case 'i':
      case 'o':
      case 'u':
      case 'x':
      case 'X':
         dSprintf(pBuffer, 64, argv[1], dAtoi(argv[2]));
         break;

      case 'e':
      case 'E':
      case 'f':
      case 'g':
      case 'G':
         dSprintf(pBuffer, 64, argv[1], dAtof(argv[2]));
         break;

      default:
         Con::errorf("strFormat: Invalid format string!\n");
         break;
   }

   return pBuffer;
}

ConsoleFunction(strcmp, S32, 3, 3, "(string one, string two)"
                "Case sensitive string compare.")
{
   argc;
   return dStrcmp(argv[1], argv[2]);
}

ConsoleFunction(stricmp, S32, 3, 3, "(string one, string two)"
                "Case insensitive string compare.")
{
   argc;
   return dStricmp(argv[1], argv[2]);
}

ConsoleFunction(strlen, S32, 2, 2, "(string str)"
               "Calculate the length of a string in characters.")
{
   argc;
   return dStrlen(argv[1]);
}

ConsoleFunction(strstr, S32 , 3, 3, "(string one, string two) "
                "Returns the start of the sub string two in one or"
                " -1 if not found.")
{
   argc;
   // returns the start of the sub string argv[2] in argv[1]
   // or -1 if not found.

   const char *retpos = dStrstr(argv[1], argv[2]);
   if(!retpos)
      return -1;
   return retpos - argv[1];
}

ConsoleFunction(strpos, S32, 3, 4, "(string hay, string needle, int offset=0) "
                "Find needle in hay, starting offset bytes in.")
{
   S32 start = 0;
   if(argc == 4)
      start = dAtoi(argv[3]);
   U32 sublen = dStrlen(argv[2]);
   U32 strlen = dStrlen(argv[1]);
   if(start < 0)
      return -1;
   if(sublen + start > strlen)
      return -1;
   for(; start + sublen <= strlen; start++)
      if(!dStrncmp(argv[1] + start, argv[2], sublen))
         return start;
   return -1;
}

ConsoleFunction(ltrim, const char *,2,2,"(string value)")
{
   argc;
   const char *ret = argv[1];
   while(*ret == ' ' || *ret == '\n' || *ret == '\t')
      ret++;
   return ret;
}

ConsoleFunction(rtrim, const char *,2,2,"(string value)")
{
   argc;
   S32 firstWhitespace = 0;
   S32 pos = 0;
   const char *str = argv[1];
   while(str[pos])
   {
      if(str[pos] != ' ' && str[pos] != '\n' && str[pos] != '\t')
         firstWhitespace = pos + 1;
      pos++;
   }
   char *ret = Con::getReturnBuffer(firstWhitespace + 1);
   dMemcpy(ret, argv[1], firstWhitespace);
   ret[firstWhitespace] = 0;
   return ret;
}

ConsoleFunction(trim, const char *,2,2,"(string)")
{
   argc;
   const char *ptr = argv[1];
   while(*ptr == ' ' || *ptr == '\n' || *ptr == '\t')
      ptr++;
   S32 firstWhitespace = 0;
   S32 pos = 0;
   const char *str = ptr;
   while(str[pos])
   {
      if(str[pos] != ' ' && str[pos] != '\n' && str[pos] != '\t')
         firstWhitespace = pos + 1;
      pos++;
   }
   char *ret = Con::getReturnBuffer(firstWhitespace + 1);
   dMemcpy(ret, ptr, firstWhitespace);
   ret[firstWhitespace] = 0;
   return ret;
}

ConsoleFunction(stripChars, const char*, 3, 3, "(string value, string chars) "
                "Remove all the characters in chars from value." )
{
   argc;
   S32 length = dStrlen( argv[1] ) + 1;
   char* ret = Con::getReturnBuffer( length );
   dStrcpy( ret, length, argv[1] );
   U32 pos = dStrcspn( ret, argv[2] );
   while ( pos < dStrlen( ret ) )
   {
      dStrcpy( ret + pos, length - pos, ret + pos + 1 );
      pos = dStrcspn( ret, argv[2] );
   }
   return( ret );
}

ConsoleFunction(stripColorCodes, const char*, 2,2,  "(stringtoStrip) - "
                "remove PowerEngineML color codes from the string.")
{
   S32 length = dStrlen( argv[1] ) + 1;
   char* ret = Con::getReturnBuffer( length );
   dStrcpy(ret, length, argv[1]);
   Con::stripColorChars(ret);
   return ret;
}

ConsoleFunction(strlwr,const char *,2,2,"(string) "
                "Convert string to lower case.")
{
   argc;
   S32 length = dStrlen( argv[1] ) + 1;
   char *ret = Con::getReturnBuffer( length );
   dStrcpy(ret, length, argv[1]);
   return dStrlwr(ret);
}

ConsoleFunction(strupr,const char *,2,2,"(string) "
                "Convert string to upper case.")
{
   argc;
   S32 length = dStrlen( argv[1] ) + 1;
   char *ret = Con::getReturnBuffer( length );
   dStrcpy(ret, length, argv[1]);
   return dStrupr(ret);
}

ConsoleFunction(strchr,const char *,3,3,"(string,char)")
{
   argc;
   const char *ret = dStrchr(argv[1], argv[2][0]);
   return ret ? ret : "";
}

ConsoleFunction(strrchr,const char *,3,3,"(string,char)")
{
   argc;
   const char *ret = dStrrchr(argv[1], argv[2][0]);
   return ret ? ret : "";
}

ConsoleFunction(strreplace, const char *, 4, 4, "(string source, string from, string to)")
{
   argc;
   S32 fromLen = dStrlen(argv[2]);
   if(!fromLen)
      return argv[1];

   S32 toLen = dStrlen(argv[3]);
   S32 count = 0;
   const char *scan = argv[1];
   while(scan)
   {
      scan = dStrstr(scan, argv[2]);
      if(scan)
      {
         scan += fromLen;
         count++;
      }
   }
   char *ret = Con::getReturnBuffer(dStrlen(argv[1]) + 1 + (toLen - fromLen) * count);
   U32 scanp = 0;
   U32 dstp = 0;
   for(;;)
   {
      const char *scan = dStrstr(argv[1] + scanp, argv[2]);
      if(!scan)
      {
         //dStrcpy(ret + dstp, argv[1] + scanp);
	     dMemcpy(ret + dstp, argv[1] + scanp, dStrlen(argv[1] + scanp) + 1);
         return ret;
      }
      U32 len = scan - (argv[1] + scanp);
      dMemcpy(ret + dstp, argv[1] + scanp, len);
      dstp += len;
      //dStrcpy(ret + dstp, argv[3]);
	  dMemcpy(ret + dstp, argv[3], dStrlen(argv[3]) + 1);
      dstp += toLen;
      scanp += len + fromLen;
   }
   return ret;
}

ConsoleFunction(getSubStr, const char *, 4, 4, "getSubStr(string str, int start, int numChars) "
                "Returns the substring of str, starting at start, and continuing "
                "to either the end of the string, or numChars characters, whichever "
                "comes first.")
{
   argc;
   // Returns the substring of argv[1], starting at argv[2], and continuing
   //  to either the end of the string, or argv[3] characters, whichever
   //  comes first.
   //
   S32 startPos   = dAtoi(argv[2]);
   S32 desiredLen = dAtoi(argv[3]);
   if (startPos < 0 || desiredLen < 0) {
      Con::errorf(ConsoleLogEntry::Script, "getSubStr(...): error, starting position and desired length must be >= 0: (%d, %d)", startPos, desiredLen);

      return "";
   }

   S32 baseLen = dStrlen(argv[1]);
   if (baseLen < startPos)
      return "";

   U32 actualLen = desiredLen;
   if (startPos + desiredLen > baseLen)
      actualLen = baseLen - startPos;

   char *ret = Con::getReturnBuffer(actualLen + 1);
   dMemcpy(ret, argv[1] + startPos, actualLen);
   ret[actualLen] = '\0';

   return ret;
}

// Used?
ConsoleFunction( stripTrailingSpaces, const char*, 2, 2, "stripTrailingSpaces( string )" )
{
   argc;
   S32 temp = S32(dStrlen( argv[1] ));
   if ( temp )
   {
      while ( ( argv[1][temp - 1] == ' ' || argv[1][temp - 1] == '_' ) && temp >= 1 )
         temp--;

      if ( temp )
      {
         char* returnString = Con::getReturnBuffer( temp + 1 );
         dMemcpy( returnString, argv[1], U32(temp) );
         returnString[temp] = '\0';
         return( returnString );			
      }
   }

   return( "" );	
}

ConsoleFunctionGroupEnd(StringFunctions);

//--------------------------------------

#include "util/stringUnit.h"

//--------------------------------------
ConsoleFunctionGroupBegin( FieldManipulators, "Functions to manipulate data returned in the form of \"x y z\".");

ConsoleFunction(getWord, const char *, 3, 4, "(string text, int index,string set)") 
{
   if (argc == 3)
	   return Con::getReturnBuffer( StringUnit::getUnit(argv[1], dAtoi(argv[2]), " \t\n") );
   else
	  return Con::getReturnBuffer( StringUnit::getUnit(argv[1], dAtoi(argv[2]), argv[3]) );
}

ConsoleFunction(getWords, const char *, 3, 5, "(string text, int index, int endIndex=INF,string set)")
{
   U32 endIndex;
   std::string set = " \t\n";
   if(argc==3)
   {
      endIndex = 1000000;
   }
   else if(argc==4)
   {
      endIndex = dAtoi(argv[3]);
   }
   else
   {
	   endIndex = dAtoi(argv[3]);
	   set = argv[4];
   }
   return Con::getReturnBuffer( StringUnit::getUnits(argv[1], dAtoi(argv[2]), endIndex, set.c_str()) );
}

ConsoleFunction(setWord, const char *, 4, 4, "newText = setWord(text, index, replace)")
{
   argc;
   return Con::getReturnBuffer( StringUnit::setUnit(argv[1], dAtoi(argv[2]), argv[3], " \t\n") );
}

ConsoleFunction(removeWord, const char *, 3, 3, "newText = removeWord(text, index)")
{
   argc;
   return Con::getReturnBuffer( StringUnit::removeUnit(argv[1], dAtoi(argv[2]), " \t\n") );
}

ConsoleFunction(getWordCount, S32, 2, 3, "getWordCount(text,string set)")
{
   
   if(argc == 2)
	   return StringUnit::getUnitCount(argv[1], " \t\n");
   else
	   return StringUnit::getUnitCount(argv[1], argv[2]);
}

//--------------------------------------
ConsoleFunction(getField, const char *, 3, 3, "getField(text, index)")
{
   argc;
   return Con::getReturnBuffer( StringUnit::getUnit(argv[1], dAtoi(argv[2]), "\t\n") );
}

ConsoleFunction(getFields, const char *, 3, 4, "getFields(text, index [,endIndex])")
{
   U32 endIndex;
   if(argc==3)
      endIndex = 1000000;
   else
      endIndex = dAtoi(argv[3]);
   return Con::getReturnBuffer( StringUnit::getUnits(argv[1], dAtoi(argv[2]), endIndex, "\t\n") );
}

ConsoleFunction(setField, const char *, 4, 4, "newText = setField(text, index, replace)")
{
   argc;
   return Con::getReturnBuffer( StringUnit::setUnit(argv[1], dAtoi(argv[2]), argv[3], "\t\n") );
}

ConsoleFunction(removeField, const char *, 3, 3, "newText = removeField(text, index)" )
{
   argc;
   return Con::getReturnBuffer( StringUnit::removeUnit(argv[1], dAtoi(argv[2]), "\t\n") );
}

ConsoleFunction(getFieldCount, S32, 2, 2, "getFieldCount(text)")
{
   argc;
   return StringUnit::getUnitCount(argv[1], "\t\n");
}

//--------------------------------------
ConsoleFunction(getRecord, const char *, 3, 3, "getRecord(text, index)")
{
   argc;
   return Con::getReturnBuffer( StringUnit::getUnit(argv[1], dAtoi(argv[2]), "\n") );
}

ConsoleFunction(getRecords, const char *, 3, 4, "getRecords(text, index [,endIndex])")
{
   U32 endIndex;
   if(argc==3)
      endIndex = 1000000;
   else
      endIndex = dAtoi(argv[3]);
   return Con::getReturnBuffer( StringUnit::getUnits(argv[1], dAtoi(argv[2]), endIndex, "\n") );
}

ConsoleFunction(setRecord, const char *, 4, 4, "newText = setRecord(text, index, replace)")
{
   argc;
   return Con::getReturnBuffer( StringUnit::setUnit(argv[1], dAtoi(argv[2]), argv[3], "\n") );
}

ConsoleFunction(removeRecord, const char *, 3, 3, "newText = removeRecord(text, index)" )
{
   argc;
   return Con::getReturnBuffer( StringUnit::removeUnit(argv[1], dAtoi(argv[2]), "\n") );
}

ConsoleFunction(getRecordCount, S32, 2, 2, "getRecordCount(text)")
{
   argc;
   return StringUnit::getUnitCount(argv[1], "\n");
}
//--------------------------------------
ConsoleFunction(firstWord, const char *, 2, 2, "firstWord(text)")
{
   argc;
   const char *word = dStrchr(argv[1], ' ');
   U32 len;
   if(word == NULL)
      len = dStrlen(argv[1]);
   else
      len = word - argv[1];
   char *ret = Con::getReturnBuffer(len + 1);
   dMemcpy(ret, argv[1], len);
   ret[len] = 0;
   return ret;
}

ConsoleFunction(restWords, const char *, 2, 2, "restWords(text)")
{
   argc;
   S32 length = 0;
   const char *word = dStrchr(argv[1], ' ');
   if(word == NULL)
      return "";

   length = dStrlen(word + 1) + 1;
   char *ret = Con::getReturnBuffer( length );
   dStrcpy(ret, length, word + 1);
   return ret;
}

static bool isInSet(char c, const char *set)
{
   if (set)
      while (*set)
         if (c == *set++)
            return true;

   return false;
}

ConsoleFunction(NextToken,const char *,4,4,"nextToken(str,token,delim)")
{
   argc;

   char *str = (char *) argv[1];
   const char *token = argv[2];
   const char *delim = argv[3];

   if (str)
   {
      // skip over any characters that are a member of delim
      // no need for special '\0' check since it can never be in delim
      while (isInSet(*str, delim))
         str++;

      // skip over any characters that are NOT a member of delim
      const char *tmp = str;

      while (*str && !isInSet(*str, delim))
         str++;

      // terminate the token
      if (*str)
         *str++ = 0;

      // set local variable if inside a function
      if (gEvalState.stack.size() && 
         gEvalState.stack.last()->scopeName)
         Con::setLocalVariable(token,tmp);
      else
         Con::setVariable(token,tmp);

      // advance str past the 'delim space'
      while (isInSet(*str, delim))
         str++;
   }

   return str;
}

ConsoleFunctionGroupEnd( FieldManipulators )
//----------------------------------------------------------------

ConsoleFunctionGroupBegin( TaggedStrings, "Functions dealing with tagging/detagging strings.");

ConsoleFunction(detag, const char *, 2, 2, "detag(textTagString)")
{
   argc;
   S32 length = 0;

   if(argv[1][0] == StringTagPrefixByte)
   {
      const char *word = dStrchr(argv[1], ' ');
      if(word == NULL)
         return "";

	  length = dStrlen(word + 1) + 1;
      char *ret = Con::getReturnBuffer(length);
      dStrcpy(ret, length, word + 1);
      return ret;
   }
   else
      return argv[1];
}

ConsoleFunction(getTag, const char *, 2, 2, "getTag(textTagString)")
{
   argc;
   if(argv[1][0] == StringTagPrefixByte)
   {
      const char * space = dStrchr(argv[1], ' ');

      U32 len;
      if(space)
         len = space - argv[1];
      else
         len = dStrlen(argv[1]) + 1;

      char * ret = Con::getReturnBuffer(len);
      dMemcpy(ret, argv[1] + 1, len - 1);
      ret[len - 1] = 0;

      return(ret);
   }
   else
      return(argv[1]);
}

ConsoleFunctionGroupEnd( TaggedStrings );

//----------------------------------------------------------------

ConsoleFunctionGroupBegin( Output, "Functions to output to the console." );

ConsoleFunction(echo, void, 2, 0, "echo(text [, ... ])")
{
   U32 len = 0;
   S32 i;
   for(i = 1; i < argc; i++)
      len += dStrlen(argv[i]);

   char *ret = Con::getReturnBuffer(len + 1);
   ret[0] = 0;
   for(i = 1; i < argc; i++)
      dStrcat(ret, len + 1, argv[i]);

   Con::printf("%s", ret);
   ret[0] = 0;
}

ConsoleFunction(warn, void, 2, 0, "warn(text [, ... ])")
{
   U32 len = 0;
   S32 i;
   for(i = 1; i < argc; i++)
      len += dStrlen(argv[i]);

   char *ret = Con::getReturnBuffer(len + 1);
   ret[0] = 0;
   for(i = 1; i < argc; i++)
      dStrcat(ret, len + 1, argv[i]);

   Con::warnf(ConsoleLogEntry::General, "%s", ret);
   ret[0] = 0;
}

ConsoleFunction(error, void, 2, 0, "error(text [, ... ])")
{
   U32 len = 0;
   S32 i;
   for(i = 1; i < argc; i++)
      len += dStrlen(argv[i]);

   char *ret = Con::getReturnBuffer(len + 1);
   ret[0] = 0;
   for(i = 1; i < argc; i++)
      dStrcat(ret, len + 1, argv[i]);

   Con::errorf(ConsoleLogEntry::General, "%s", ret);
   ret[0] = 0;
}

ConsoleFunction(debugv, void, 2, 2, "debugv(\"<variable>\") outputs the value of the <variable> in the format <variable> = <variable value>")
{
   if (argv[1][0] == '%')
      Con::errorf("%s = %s", argv[1], Con::getLocalVariable(argv[1]));
   else
      Con::errorf("%s = %s", argv[1], Con::getVariable(argv[1]));
}

ConsoleFunction(expandEscape, const char *, 2, 2, "expandEscape(text)")
{
   argc;
   char *ret = Con::getReturnBuffer(dStrlen(argv[1])*2 + 1);  // worst case situation
   expandEscape(ret, argv[1]);
   return ret;
}

ConsoleFunction(collapseEscape, const char *, 2, 2, "collapseEscape(text)")
{
   argc;
   S32 length = dStrlen(argv[1]) + 1;
   char *ret = Con::getReturnBuffer(length);  // worst case situation
   dStrcpy( ret, length, argv[1] );
   collapseEscape( ret );
   return ret;
}

ConsoleFunction(setLogMode, void, 2, 2, "setLogMode(mode);")
{
   argc;
   Con::setLogMode(dAtoi(argv[1]));
}

ConsoleFunction(setEchoFileLoads, void, 2, 2, "setEchoFileLoads(bool);")
{
   argc;
   ResourceManager->setFileNameEcho(dAtob(argv[1]));
}

ConsoleFunctionGroupEnd( Output );

//----------------------------------------------------------------

ConsoleFunction(quit, void, 1, 1, "quit() End execution of PowerEngine.")
{
   argc; argv;
   Platform::postQuitMessage(0);
}

ConsoleFunction(delDir, void, 1, 1, "quit() End execution of PowerEngine.")
{
	argc; argv;
	{
#pragma message(ENGINE(程序退出是删除程序生成的shader文件))
		const char filename[] = "*.*";
		char dir[260];
		dMemset(dir, 0, sizeof(dir));
		const char *shaderPath = Con::getVariable("$pref::Video::shaderPath");
		if (dStrlen(shaderPath) == 0)
		{
			shaderPath = "temp";
		}
		dSprintf(dir, sizeof(dir), "%s/%s", Platform::getCurrentDirectory(), shaderPath);
		Platform::deleteDirFiles(dir, filename);
	}
}

ConsoleFunction(quitWithErrorMessage, void, 2, 2, "quitWithErrorMessage(msg)"
                " - Quit, showing the provided error message. This is equivalent"
                " to an AssertISV.")
{
   AssertISV(false, argv[1]);
}

//----------------------------------------------------------------

ConsoleFunction( gotoWebPage, void, 2, 2, "( address ) - Open a URL in the user's favorite web browser." )
{
   argc;
   char* protocolSep = dStrstr(argv[1],"://");

   if( protocolSep != NULL )
   {
      Platform::openWebBrowser(argv[1]);
      return;
   }

   // if we don't see a protocol seperator, then we know that some bullethead
   // sent us a bad url. We'll first check to see if a file inside the sandbox
   // with that name exists, then we'll just glom "http://" onto the front of 
   // the bogus url, and hope for the best.
   
   char urlBuf[2048];
   if(Platform::isFile(argv[1]) || Platform::isDirectory(argv[1]))
   {
#ifdef POWER_TOOLS
      dSprintf(urlBuf, sizeof(urlBuf), "file://%s",argv[1]);
#else
      dSprintf(urlBuf, sizeof(urlBuf), "file://%s/%s",Platform::getCurrentDirectory(),argv[1]);
#endif
   }
   else
      dSprintf(urlBuf, sizeof(urlBuf), "http://%s",argv[1]);
   
   Platform::openWebBrowser(urlBuf);
   return;
}

//----------------------------------------------------------------

ConsoleFunctionGroupBegin(MetaScripting, "Functions that let you manipulate the scripting engine programmatically.");

ConsoleFunction(call, const char *, 2, 0, "call(funcName [,args ...])")
{
   return Con::execute(argc - 1, argv + 1);
}

static U32 execDepth = 0;
static U32 journalDepth = 1;

static StringTableEntry getDSOPath(const char *scriptPath)
{
#ifndef POWER_TOOLS

   // [tom, 11/17/2006] Force old behavior for the player. May not want to do this.
   const char *slash = dStrrchr(scriptPath, '/');
   return StringTable->insertn(scriptPath, slash - scriptPath, true);

#else

   char relPath[1024], dsoPath[1024];
   bool isPrefs = false;

   // [tom, 11/17/2006] Prefs are handled slightly differently to avoid dso name clashes
   StringTableEntry prefsPath = Platform::getPrefsPath();
   if(dStrnicmp(scriptPath, prefsPath, dStrlen(prefsPath)) == 0)
   {
      relPath[0] = 0;
      isPrefs = true;
   }
   else
   {
      StringTableEntry strippedPath = Platform::stripBasePath(scriptPath);
      dStrcpy(relPath, sizeof(relPath), strippedPath);

      char *slash = dStrrchr(relPath, '/');
      if(slash)
         *slash = 0;
   }

   const char *overridePath;
   if(! isPrefs)
      overridePath = Con::getVariable("$Scripts::OverrideDSOPath");
   else
      overridePath = prefsPath;

   if(overridePath && *overridePath)
      Platform::makeFullPathName(relPath, dsoPath, sizeof(dsoPath), overridePath);
   else
   {
      char t[1024];
      dSprintf(t, sizeof(t), "compiledScripts/%s", relPath);
      Platform::makeFullPathName(t, dsoPath, sizeof(dsoPath), Platform::getPrefsPath());
   }

   return StringTable->insert(dsoPath);

#endif
}

ConsoleFunction(getDSOPath, const char *, 2, 2, "(scriptFileName)")
{
   Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);
   
   char scriptFullNameBuffer[1024];
   Platform::makeFullPathName( scriptFilenameBuffer, scriptFullNameBuffer, sizeof(scriptFullNameBuffer) );

   const char *filename = getDSOPath(scriptFullNameBuffer);
   if(filename == NULL || *filename == 0)
      return "";

   return filename;
}

ConsoleFunction(compile, bool, 2, 2, "compile(fileName)")
{
   argc;

   Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);

   char scriptFullNameBuffer[1024];
   Platform::makeFullPathName( scriptFilenameBuffer, scriptFullNameBuffer, sizeof(scriptFullNameBuffer) );
   dStrcpy( scriptFilenameBuffer, sizeof(scriptFilenameBuffer), scriptFullNameBuffer );

   // Figure out where to put DSOs
   StringTableEntry dsoPath = getDSOPath(scriptFilenameBuffer);

   // If the script file extention is '.ed.cs' then compile it to a different compiled extention
   bool isEditorScript = false;
   const char *ext = dStrrchr( scriptFilenameBuffer, '.' );
   if( ext && ( dStricmp( ext, ".cs" ) == 0 ) )
   {
      const char* ext2 = ext - 3;
      if( dStricmp( ext2, ".ed.cs" ) == 0 )
         isEditorScript = true;
   }
   else if( ext && ( dStricmp( ext, ".gui" ) == 0 ) )
   {
      const char* ext2 = ext - 3;
      if( dStricmp( ext2, ".ed.gui" ) == 0 )
         isEditorScript = true;
   }

   const char *filenameOnly = dStrrchr(scriptFilenameBuffer, '/');
   if(filenameOnly)
      ++filenameOnly;
   else
      filenameOnly = scriptFilenameBuffer;
 
   char nameBuffer[512];

   if( isEditorScript )
      dStrcpyl(nameBuffer, sizeof(nameBuffer), dsoPath, "/", filenameOnly, EDSO, NULL);
   else
      dStrcpyl(nameBuffer, sizeof(nameBuffer), dsoPath, "/", filenameOnly, DSO, NULL);

   Platform::AdjustCMPfileName(nameBuffer);
   
   Stream   *s = ResourceManager->openStream(scriptFilenameBuffer);
   char     *script = NULL;

   if(s)
   {
      U32   scriptSize = ResourceManager->getSize(scriptFilenameBuffer);
      if ( scriptSize > 0 )
      {
         script = new char [scriptSize+1];
         s->read(scriptSize, script);
         script[scriptSize] = 0;
      }

      ResourceManager->closeStream(s);
   }

   if (!script)
   {
      Con::errorf(ConsoleLogEntry::Script, "compile: invalid script file %s.", scriptFilenameBuffer);
      return false;
   }

#ifdef POWER_DEBUG
   Con::printf("Compiling %s...", scriptFilenameBuffer);
#endif 

   CodeBlock *code = new CodeBlock();
   code->compile(nameBuffer, scriptFilenameBuffer, script);
   delete code;
   delete[] script;

   return true;
}

ConsoleFunction(exec, bool, 2, 4, "exec(fileName [, nocalls [,journalScript]])")
{
   bool journal = false;

   execDepth++;
   if(journalDepth >= execDepth)
      journalDepth = execDepth + 1;
   else
      journal = true;

   bool noCalls = false;
   bool ret = false;

   if(argc >= 3 && dAtoi(argv[2]))
      noCalls = true;

   if(argc >= 4 && dAtoi(argv[3]) && !journal)
   {
      journal = true;
      journalDepth = execDepth;
   }

   // Determine the filename we actually want...
   Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);

   char scriptFullNameBuffer[1024];
   Platform::makeFullPathName( scriptFilenameBuffer, scriptFullNameBuffer, sizeof(scriptFullNameBuffer) );
   dStrcpy( scriptFilenameBuffer, sizeof(scriptFilenameBuffer), scriptFullNameBuffer );

   // Figure out where to put DSOs
   StringTableEntry dsoPath = getDSOPath(scriptFilenameBuffer);

   const char *ext = dStrrchr(scriptFilenameBuffer, '.');

   if(!ext)
   {
      // We need an extension!
      Con::errorf(ConsoleLogEntry::Script, "exec: invalid script file name %s.", scriptFilenameBuffer);
      execDepth--;
      return false;
   }

   // Check Editor Extensions
   bool isEditorScript = false;

   // If the script file extension is '.ed.cs' then compile it to a different compiled extension
   if( dStricmp( ext, ".cs" ) == 0 )
   {
      const char* ext2 = ext - 3;
      if( dStricmp( ext2, ".ed.cs" ) == 0 )
         isEditorScript = true;
   }
   else if( dStricmp( ext, ".gui" ) == 0 )
   {
      const char* ext2 = ext - 3;
      if( dStricmp( ext2, ".ed.gui" ) == 0 )
         isEditorScript = true;
   }


   StringTableEntry scriptFileName = StringTable->insert(scriptFilenameBuffer);

#ifndef POWER_OS_XENON
   // Is this a file we should compile? (anything in the prefs path should not be compiled)
   StringTableEntry prefsPath = Platform::getPrefsPath();
   bool compiled = dStricmp(ext, ".mis") && !journal && !Con::getBoolVariable("Scripts::ignoreDSOs");

   // [tom, 12/5/2006] stripBasePath() fucks up if the filename is not in the exe
   // path, current directory or prefs path. Thus, getDSOFilename() will also screw
   // up and so this allows the scripts to still load but without a DSO.
   if(Platform::isFullPath(Platform::stripBasePath(scriptFilenameBuffer)))
      compiled = false;

   // [tom, 11/17/2006] It seems to make sense to not compile scripts that are in the
   // prefs directory. However, getDSOPath() can handle this situation and will put
   // the dso along with the script to avoid name clashes with tools/game dsos.
   if( prefsPath && prefsPath[ 0 ] && dStrnicmp(scriptFileName, prefsPath, dStrlen(prefsPath)) == 0 )
      compiled = false;
#else
   bool compiled = false;  // Don't try to compile things on the 360, ignore DSO's when debugging
                           // because PC prefs will screw up stuff like SFX.
#endif

   // If we're in a journaling mode, then we will read the script
   // from the journal file.
   if(journal && Journal::IsPlaying())
   {
      char fileNameBuf[256];
      bool fileRead;
      U32 fileSize;

      Journal::ReadString(fileNameBuf);
      Journal::Read(&fileRead);

      if(!fileRead)
      {
         Con::errorf(ConsoleLogEntry::Script, "Journal script read (failed) for %s", fileNameBuf);
         execDepth--;
         return false;
      }
      Journal::Read(&fileSize);
      char *script = new char[fileSize + 1];
      Journal::Read(fileSize, script);
      script[fileSize] = 0;
      Con::printf("Executing (journal-read) %s.", scriptFileName);
      CodeBlock *newCodeBlock = new CodeBlock();
      newCodeBlock->compileExec(scriptFileName, script, noCalls, 0);
      delete [] script;

      execDepth--;
      return true;
   }

   // Ok, we let's try to load and compile the script.
   ResourceObject *rScr = ResourceManager->find(scriptFileName);
   ResourceObject *rCom = NULL;

   char nameBuffer[512];
   char* script = NULL;
   U32 scriptSize = 0;
   U32 version;

   Stream *compiledStream = NULL;
   FileTime comModifyTime, scrModifyTime;

   // Check here for .edso
   bool edso = false;
   if( dStricmp( ext, EDSO ) == 0  && rScr )
   {
      edso = true;
      rCom = rScr;
      rScr = NULL;

      rCom->getFileTimes( NULL, &comModifyTime );
      dStrcpy( nameBuffer, sizeof(nameBuffer), scriptFileName );
   }

   // If we're supposed to be compiling this file, check to see if there's a DSO
   if(compiled && !edso)
   {
      const char *filenameOnly = dStrrchr(scriptFileName, '/');
      if(filenameOnly)
         ++filenameOnly;
      else
         filenameOnly = scriptFileName;

      char pathAndFilename[1024];
      Platform::makeFullPathName(filenameOnly, pathAndFilename, sizeof(pathAndFilename), dsoPath);

	  if( isEditorScript )
		  dStrcpyl(nameBuffer, sizeof(nameBuffer), pathAndFilename, EDSO, NULL);
	  else
		  dStrcpyl(nameBuffer, sizeof(nameBuffer), pathAndFilename, DSO, NULL);

	  Platform::AdjustCMPfileName(nameBuffer);

	  rCom = ResourceManager->find(nameBuffer);

      if(rCom)
         rCom->getFileTimes(NULL, &comModifyTime);
      if(rScr)
         rScr->getFileTimes(NULL, &scrModifyTime);
   }

   // Let's do a sanity check to complain about DSOs in the future.
   //
   // MM:	This doesn't seem to be working correctly for now so let's just not issue
   //		the warning until someone knows how to resolve it.
   //
   //if(compiled && rCom && rScr && Platform::compareFileTimes(comModifyTime, scrModifyTime) < 0)
   //{
      //Con::warnf("exec: Warning! Found a DSO from the future! (%s)", nameBuffer);
   //}

   // If we had a DSO, let's check to see if we should be reading from it.
   if(compiled && rCom && (!rScr || Platform::compareFileTimes(comModifyTime, scrModifyTime) >= 0))
   {
      compiledStream = ResourceManager->openStream(nameBuffer);
      if (compiledStream)
      {
         // Check the version!
         compiledStream->read(&version);
         if(version != Con::DSOVersion)
         {
            Con::warnf("exec: Found an old DSO (%s, ver %d < %d), ignoring.", nameBuffer, version, Con::DSOVersion);
            ResourceManager->closeStream(compiledStream);
            compiledStream = NULL;
         }
      }
   }

   // If we're journalling, let's write some info out.
   if(journal && Journal::IsRecording())
      Journal::WriteString(scriptFileName);

   if(rScr && !compiledStream)
   {
      // If we have source but no compiled version, then we need to compile
      // (and journal as we do so, if that's required).

      Stream *s = ResourceManager->openStream(scriptFileName);
      if(journal && Journal::IsRecording())
         Journal::Write(bool(s != NULL));

      if(s)
      {
         scriptSize = ResourceManager->getSize(scriptFileName);
         script = new char [scriptSize+1];
         s->read(scriptSize, script);

         if(journal && Journal::IsRecording())
         {
            Journal::Write(scriptSize);
            Journal::Write(scriptSize, script);
         }
         ResourceManager->closeStream(s);
         script[scriptSize] = 0;
      }

      if (!scriptSize || !script)
      {
         delete [] script;
         Con::errorf(ConsoleLogEntry::Script, "exec: invalid script file %s.", scriptFileName);
         execDepth--;
         return false;
      }

//#ifndef POWER_NO_DSO_GENERATION
      if(compiled)
      {
         // compile this baddie.
#ifdef POWER_DEBUG
         Con::printf("Compiling %s...", scriptFileName);
#endif   

         CodeBlock *code = new CodeBlock();
         code->compile(nameBuffer, scriptFileName, script);
         delete code;
         code = NULL;

         compiledStream = ResourceManager->openStream(nameBuffer);
         if(compiledStream)
         {
            compiledStream->read(&version);
         }
         else
         {
            // We have to exit out here, as otherwise we get double error reports.
            delete [] script;
            execDepth--;
            return false;
         }
      }
//#endif
   }
   else
   {
      if(journal && Journal::IsRecording())
         Journal::Write(bool(false));
   }

   if(compiledStream)
   {
      // Delete the script object first to limit memory used
      // during recursive execs.
      delete [] script;
      script = 0;

      // We're all compiled, so let's run it.
#ifdef POWER_DEBUG
      Con::printf("Loading compiled script %s.", scriptFileName);
#endif   
      CodeBlock *code = new CodeBlock;
      code->read(scriptFileName, *compiledStream);
      ResourceManager->closeStream(compiledStream);
      code->exec(0, scriptFileName, NULL, 0, NULL, noCalls, NULL, 0);
      ret = true;
   }
   else
      if(rScr)
      {
         // No compiled script,  let's just try executing it
         // directly... this is either a mission file, or maybe
         // we're on a readonly volume.
#ifdef POWER_DEBUG
         Con::printf("Executing %s.", scriptFileName);
#endif   

         CodeBlock *newCodeBlock = new CodeBlock();
         StringTableEntry name = StringTable->insert(scriptFileName);

         newCodeBlock->compileExec(name, script, noCalls, 0);
         ret = true;
      }
      else
      {
         // Don't have anything.
         Con::warnf(ConsoleLogEntry::Script, "Missing file: %s!", scriptFileName);
         ret = false;
      }

   delete [] script;
   execDepth--;
   return ret;
}

ConsoleFunction(eval, const char *, 2, 2, "eval(consoleString)")
{
   argc;
   return Con::evaluate(argv[1], false, NULL);
}

ConsoleFunction(getVariable, const char *, 2, 2, "(string varName)")
{
   return Con::getVariable(argv[1]);
}

ConsoleFunction(isFunction, bool, 2, 2, "(string funcName)")
{
   return Con::isFunction(argv[1]);
}

ConsoleFunction(isMethod, bool, 3, 3, "(string namespace, string method)" )
{
   Namespace* ns = Namespace::find( StringTable->insert( argv[1] ) );
   Namespace::Entry* nse = ns->lookup( StringTable->insert( argv[2] ) );
   if( !nse )
      return false;

   return true;
}

//------------------------------------------------------------------------------

ConsoleFunction(isCurrentScriptToolScript, bool, 1, 1, "() Returns true if the calling script is a tools script")
{
   return Con::isCurrentScriptToolScript();
}

ConsoleFunction(getModNameFromPath, const char *, 2, 2, "(string path) Attempts to extract a mod directory from path. Returns empty string on failure.")
{
   StringTableEntry modPath = Con::getModNameFromPath(argv[1]);
   return modPath ? modPath : "";
}

//----------------------------------------------------------------

ConsoleFunction(getPrefsPath, const char *, 1, 2, "([relativeFileName])")
{
   const char *filename = Platform::getPrefsPath(argc > 1 ? argv[1] : NULL);
   if(filename == NULL || *filename == 0)
      return "";
     
   return filename;
}

ConsoleFunction(execPrefs, bool, 2, 4, "execPrefs(relativeFileName [, nocalls [,journalScript]])")
{
   const char *filename = Platform::getPrefsPath(argv[1]);
   if(filename == NULL || *filename == 0)
      return false;

   // Scripts do this a lot, so we may as well help them out
   // FIXME [tom, 11/17/2006] This should use the resource manager in addition to file system check
   if(! Platform::isFile(filename) && ! ResourceManager->find(filename))
      return true;

   argv[0] = "exec";
   argv[1] = filename;
   return dAtob(Con::execute(argc, argv));
}

ConsoleFunction(export, void, 2, 4, "export(searchString [, relativeFileName [,append]])")
{
   const char *filename = NULL;
   bool append = (argc == 4) ? dAtob(argv[3]) : false;

   if (argc >= 3)
   {
#ifdef POWER_PLAYER
      if(Con::expandScriptFilename(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[2]))
         filename = scriptFilenameBuffer;
#else
      filename = Platform::getPrefsPath(argv[2]);
      if(filename == NULL || *filename == 0)
         return;
#endif
   }

   gEvalState.globalVars.exportVariables(argv[1], filename, append);
}

ConsoleFunction(deleteVariables, void, 2, 2, "deleteVariables(wildCard)")
{
   argc;
   gEvalState.globalVars.deleteVariables(argv[1]);
}

//----------------------------------------------------------------

ConsoleFunction(trace, void, 2, 2, "trace(bool)")
{
   argc;
   gEvalState.traceOn = dAtob(argv[1]);
   Con::printf("Console trace is %s", gEvalState.traceOn ? "on." : "off.");
}

//----------------------------------------------------------------

#if defined(POWER_DEBUG) || !defined(POWER_SHIPPING)
ConsoleFunction(debug, void, 1, 1, "debug()")
{
   argv; argc;
   Platform::debugBreak();
}
#endif

ConsoleFunctionGroupEnd( MetaScripting );

//----------------------------------------------------------------

ConsoleFunction(isspace, bool, 3, 3, "(string, index): return true if character at specified index in string is whitespace")
{
   S32 idx = dAtoi(argv[2]);
   if (idx >= 0 && idx < dStrlen(argv[1]))
      return dIsspace(argv[1][idx]);
   else
      return false;
}

ConsoleFunction(isalnum, bool, 3, 3, "(string, index): return true if character at specified index in string is alnum")
{
   S32 idx = dAtoi(argv[2]);
   if (idx >= 0 && idx < dStrlen(argv[1]))
      return dIsalnum(argv[1][idx]);
   else
      return false;
}

ConsoleFunction(startswith, bool, 3, 3, "(src string, target string) case insensitive")
{
  const char* src = argv[1];
  const char* target = argv[2];

  // if the target string is empty, return true (all strings start with the empty string)
  S32 srcLen = dStrlen(src);
  S32 targetLen = dStrlen(target);
  if (targetLen == 0)
     return true;
  // else if the src string is empty, return false (empty src does not start with non-empty target)
  else if (srcLen == 0)
     return false;

  // both src and target are non empty, create temp buffers for lowercase operation
  char* srcBuf = new char[srcLen + 1];
  char* targetBuf = new char[targetLen + 1];

  // copy src and target into buffers
  dStrcpy(srcBuf, srcLen + 1, src);
  dStrcpy(targetBuf, targetLen + 1, target);

  // reassign src/target pointers to lowercase versions
  src = dStrlwr(srcBuf);
  target = dStrlwr(targetBuf);

  // do the comparison
  bool startsWith = dStrncmp(src, target, targetLen) == 0;

  // delete temp buffers
  delete [] srcBuf;
  delete [] targetBuf;

  return startsWith;
}

ConsoleFunction(endswith, bool, 3, 3, "(src string, target string) case insensitive")
{
  const char* src = argv[1];
  const char* target = argv[2];

  // if the target string is empty, return true (all strings end with the empty string)
  S32 srcLen = dStrlen(src);
  S32 targetLen = dStrlen(target);
  if (targetLen == 0)
     return true;
  // else if the src string is empty, return false (empty src does not end with non-empty target)
  else if (srcLen == 0)
     return false;

  // both src and target are non empty, create temp buffers for lowercase operation
  char* srcBuf = new char[srcLen + 1];
  char* targetBuf = new char[targetLen + 1];

  // copy src and target into buffers
  dStrcpy(srcBuf, srcLen + 1, src);
  dStrcpy(targetBuf, targetLen + 1, target);

  // reassign src/target pointers to lowercase versions
  src = dStrlwr(srcBuf);
  target = dStrlwr(targetBuf);

  // set the src pointer to the appropriate place to check the end of the string
  src += srcLen - targetLen;

  // do the comparison
  bool endsWith = dStrcmp(src, target) == 0;

  // delete temp buffers
  delete [] srcBuf;
  delete [] targetBuf;

  return endsWith;
}

ConsoleFunction(strrchrpos,S32,3,3,"strrchrpos(string,char)")
{
  argc;
  const char *ret = dStrrchr(argv[1], argv[2][0]);
  return ret ? ret - argv[1] : -1;
}

ConsoleFunction(strswiz, const char *,3,3,"strswiz(string,len)")
{
   argc;
   S32 lenIn = dStrlen(argv[1]);
   S32 lenOut = getMin(dAtoi(argv[2]),lenIn);
   char * ret = Con::getReturnBuffer(lenOut+1);
   for (S32 i=0; i<lenOut; i++)
   {
      if (i&1)
         ret[i] = argv[1][i>>1];
      else
         ret[i] = argv[1][lenIn-(i>>1)-1];
   }
   ret[lenOut]='\0';
   return ret;
}

ConsoleFunction(countBits, S32, 2, 2, "count the number of bits in the specified 32 bit integer")
{
   S32 c = 0;
   S32 v = dAtoi(argv[1]);

   // from 
   // http://graphics.stanford.edu/~seander/bithacks.html

   // for at most 32-bit values in v:
   c =  ((v & 0xfff) * 0x1001001001001ULL & 0x84210842108421ULL) % 0x1f;
   c += (((v & 0xfff000) >> 12) * 0x1001001001001ULL & 0x84210842108421ULL) % 
      0x1f;
   c += ((v >> 24) * 0x1001001001001ULL & 0x84210842108421ULL) % 0x1f;

#ifndef POWER_SHIPPING
   // since the above isn't very obvious, for debugging compute the count in a more 
   // traditional way and assert if it is different
   {
      S32 c2 = 0;
      S32 v2 = v;
      for (c2 = 0; v2; v2 >>= 1)
      {
         c2 += v2 & 1;
      }
      if (c2 != c)
         Con::errorf("countBits: Uh oh bit count mismatch");
      AssertFatal(c2 == c, "countBits: uh oh, bit count mismatch");
   }
#endif

   return c;
}

ConsoleFunction(isShippingBuild, bool, 1, 1, "Returns true if this is a shipping build, false otherwise")
{
#ifdef POWER_SHIPPING
   return true;
#else
   return false;
#endif
}

ConsoleFunction(isDebugBuild, bool, 1, 1, "isDebugBuild() - Returns true if the script is running in a debug PowerEngine executable" )
{
#ifdef POWER_DEBUG
   return true;
#else
   return false;
#endif
}

//显示当前日期时间
ConsoleFunction(getLocalTime, const char*, 1, 1, "getLocalTime() - 获取当前日期时间(年 月 日 时 分 秒)")
{
	char* returnString = Con::getReturnBuffer(20);
	Platform::LocalTime lt;
	Platform::getLocalTime(lt);
	dSprintf(returnString, 32, "%d %d %d %d %d %d", lt.year + 1900, lt.month + 1, lt.yearday, lt.hour, lt.min, lt.sec);
	return returnString;
}