//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "core/zip/compressor.h"

#include "core/zipSubStream.h"

namespace Zip
{

ImplementCompressor(Deflate, Deflated);

CompressorCreateReadStream(Deflate)
{
   ZipSubRStream *stream = new ZipSubRStream;
   stream->attachStream(zipStream);
   stream->setUncompressedSize(cdir->mUncompressedSize);

   return stream;
}

CompressorCreateWriteStream(Deflate)
{
   ZipSubWStream *stream = new ZipSubWStream;
   stream->attachStream(zipStream);

   return stream;
}

} // end namespace Zip
