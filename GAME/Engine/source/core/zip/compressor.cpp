//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "core/zip/compressor.h"

namespace Zip
{

static Compressor *gCompressorInitList = NULL;

//-----------------------------------------------------------------------------
// Constructor/Destructor
//-----------------------------------------------------------------------------

Compressor::Compressor(S32 method, const char *name)
{
   mName = name;
   mMethod = method;

   mNext = gCompressorInitList;
   gCompressorInitList = this;
}

//-----------------------------------------------------------------------------
// Static Methods
//-----------------------------------------------------------------------------

Compressor *Compressor::findCompressor(S32 method)
{
   for(Compressor *walk = gCompressorInitList;walk;walk = walk->mNext)
   {
      if(walk->getMethod() == method)
         return walk;
   }

   return NULL;
}

Compressor *Compressor::findCompressor(const char *name)
{
   for(Compressor *walk = gCompressorInitList;walk;walk = walk->mNext)
   {
      if(dStricmp(walk->getName(), name) == 0)
         return walk;
   }

   return NULL;
}

} // end namespace Zip
