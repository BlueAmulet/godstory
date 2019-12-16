//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SHADERGENMANAGER_H_
#define _SHADERGENMANAGER_H_

#include "core/stringTable.h"
#include "core/stream.h"
#include "core/tVector.h"

#define DEFAULT_BUFFER_SIZE 1 << 14//16K

struct TrackedAutoGenShader
{
   StringTableEntry fileName;
   bool shaderInMemory;
   char *shaderData;
   U32 shaderDataSize;

   TrackedAutoGenShader() : fileName( NULL ), shaderInMemory( false ), shaderData( NULL ) {};
   ~TrackedAutoGenShader()
   {
      if( shaderData != NULL && shaderInMemory )
         dFree(shaderData);
   }
   
};

namespace ShaderGenManager
{
   extern Vector<TrackedAutoGenShader> _mTrackedShaders;
   extern Stream *_mOpenStream;
   extern Stream *_mOpenReadStream;

   Stream *openNewShaderStream( const char *fileName, bool shaderInMemory = false );
   void closeNewShaderStream( Stream *s );

   Stream *readShaderStream( const char *fileName );
   void closeShaderStream( Stream *s );
};

#endif