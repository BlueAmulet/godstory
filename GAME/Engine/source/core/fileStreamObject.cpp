//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "core/fileStreamObject.h"

//-----------------------------------------------------------------------------
// Local Globals
//-----------------------------------------------------------------------------

static const struct
{
   const char *strMode;
   FileStream::AccessMode mode;
} gModeMap[]=
{
   { "read", FileStream::Read },
   { "write", FileStream::Write },
   { "readwrite", FileStream::ReadWrite },
   { "writeappend", FileStream::WriteAppend },
   { NULL, (FileStream::AccessMode)0 }
};

//-----------------------------------------------------------------------------
// Constructor/Destructor
//-----------------------------------------------------------------------------

FileStreamObject::FileStreamObject()
{
}

FileStreamObject::~FileStreamObject()
{
   close();
}

IMPLEMENT_CONOBJECT(FileStreamObject);

//-----------------------------------------------------------------------------

bool FileStreamObject::onAdd()
{
   // [tom, 2/12/2007] Skip over StreamObject's onAdd() so that we can
   // be instantiated from script.
   return SimObject::onAdd();
}

//-----------------------------------------------------------------------------
// Public Methods
//-----------------------------------------------------------------------------

bool FileStreamObject::open(const char *filename, FileStream::AccessMode mode)
{
   close();

   if(! mFileStream.open(filename, mode))
      return false;

   mStream = &mFileStream;
   return true;
}

void FileStreamObject::close()
{
   mFileStream.close();
   mStream = NULL;
}

//-----------------------------------------------------------------------------
// Console Methods
//-----------------------------------------------------------------------------

ConsoleMethod(FileStreamObject, open, bool, 4, 4, "(filename, mode) Open a file. Mode can be one of Read, Write, ReadWrite or WriteAppend.")
{
   for(S32 i = 0;gModeMap[i].strMode;++i)
   {
      if(dStricmp(gModeMap[i].strMode, argv[3]) == 0)
      {
         FileStream::AccessMode mode = gModeMap[i].mode;
         char buffer[1024];
         Con::expandScriptFilename(buffer, sizeof(buffer), argv[2]);
         return object->open(buffer, mode);
      }
   }

   Con::errorf("FileStreamObject::open - Mode must be one of Read, Write, ReadWrite or WriteAppend.");
   return false;
}

ConsoleMethod(FileStreamObject, close, void, 2, 2, "() Close the file.")
{
   object->close();
}
