//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "platform/types.h"
#include "gfxVertexBuffer.h"
#include "gfx/gfxDevice.h"

//-----------------------------------------------------------------------------
// Set
//-----------------------------------------------------------------------------
void GFXVertexBufferHandleBase::set(GFXDevice *theDevice, U32 numVerts, U32 flags, U32 vertexSize, GFXBufferType type,U8 streamNum)
{
   RefPtr<GFXVertexBuffer>::operator=( theDevice->allocVertexBuffer(numVerts, flags, vertexSize, type,streamNum) );
}

void GFXVertexBuffer::describeSelf( char* buffer, U32 sizeOfBuffer )
{
   const char* bufType;
   switch(mBufferType)
   {
   case GFXBufferTypeStatic:
      bufType = "Static";
      break;
   case GFXBufferTypeDynamic:
      bufType = "Dynamic";
      break;
   case GFXBufferTypeVolatile:
      bufType = "Volatile";
      break;
   case GFXBufferTypeNull:
	   bufType = "Null";
	   break;
   default:
      bufType = "Unknown";
      break;
   }
   dSprintf(buffer, sizeOfBuffer, "numVerts: %i vertSize: %i bufferType: %s", mNumVerts, mVertexSize, bufType);
}