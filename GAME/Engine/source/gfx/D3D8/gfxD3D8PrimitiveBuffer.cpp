//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include <d3d8.h>
#include <d3dx8math.h>
#include "gfx/D3D8/gfxD3D8Device.h"
#include "gfx/D3D8/gfxD3D8EnumTranslate.h"

GFXD3D8PrimitiveBuffer::~GFXD3D8PrimitiveBuffer() 
{
   if(mBufferType != GFXBufferTypeVolatile)
      SAFE_RELEASE( ib );
}

void GFXD3D8PrimitiveBuffer::lock(U16 indexStart, U16 indexEnd, U16 **indexPtr)
{
   mLocked = true;
   U32 flags=0;
   switch(mBufferType)
   {
   case GFXBufferTypeStatic:
      // flags |= D3DLOCK_DISCARD;
      break;
   case GFXBufferTypeDynamic:
      // AssertISV(false, "D3D doesn't support dynamic primitive buffers. -- BJG");
      // Does too. -- BJG
      break;
   case GFXBufferTypeVolatile:
      // Get our range now...
      AssertFatal(indexStart == 0,                "Cannot get a subrange on a volatile buffer.");
      AssertFatal(indexEnd < MAX_DYNAMIC_INDICES, "Cannot get more than MAX_DYNAMIC_INDICES in a volatile buffer. Up the constant!");

      // Get the primtive buffer
      mVolatileBuffer = ((GFXD3D8Device*)mDevice)->mDynamicPB;

      AssertFatal( mVolatileBuffer, "No dynamic primitive buffer was available!");

      // We created the pool when we requested this volatile buffer, so assume it exists...
      if( mVolatileBuffer->mIndexCount + indexEnd > MAX_DYNAMIC_INDICES ) 
      {
         flags |= D3DLOCK_DISCARD;
         mVolatileStart = indexStart  = 0;
         indexEnd       = indexEnd;
      }
      else 
      {
         flags |= D3DLOCK_NOOVERWRITE;
         mVolatileStart = indexStart  = mVolatileBuffer->mIndexCount;
         indexEnd                    += mVolatileBuffer->mIndexCount;
      }

      mVolatileBuffer->mIndexCount = indexEnd + 1;
      ib = mVolatileBuffer->ib;

      break;
   }

   D3D8Assert( ib->Lock(indexStart * sizeof(U16), (indexEnd - indexStart) * sizeof(U16), (BYTE**)indexPtr, flags),
      "GFXD3D8PrimitiveBuffer::lock - Could not lock primitive buffer.");

}

void GFXD3D8PrimitiveBuffer::unlock()
{
   mLocked = false;
   D3D8Assert( ib->Unlock(),
      "GFXD3D8PrimitiveBuffer::unlock - Failed to unlock primitive buffer.");

   mIsFirstLock = false;
}

void GFXD3D8PrimitiveBuffer::prepare()
{
	((GFXD3D8Device*)mDevice)->setPrimitiveBuffer(this);
}

void GFXD3D8PrimitiveBuffer::zombify()
{
   if(mBufferType != GFXBufferTypeDynamic)
      return;
   AssertFatal(!mLocked, "GFXD3D8PrimitiveBuffer::zombify - Cannot zombify a locked buffer!");
   SAFE_RELEASE(ib);
}

void GFXD3D8PrimitiveBuffer::resurrect()
{
   if(mBufferType != GFXBufferTypeDynamic)
      return;
   U32 usage = 0;
   D3DPOOL pool = D3DPOOL_DEFAULT;

   switch(mBufferType)
   {
   case GFXBufferTypeDynamic:
      AssertISV(false, "GFXD3D8PrimitiveBuffer::resurrect - D3D does not support dynamic primitive buffers. -- BJG");
      //usage |= D3DUSAGE_DYNAMIC;
      break;

   case GFXBufferTypeVolatile:
      pool = D3DPOOL_DEFAULT;
      break;
   }
   D3D8Assert(static_cast<GFXD3D8Device*>(mDevice)->mD3DDevice->CreateIndexBuffer( sizeof(U16) * mIndexCount , 
      usage , GFXD3D8IndexFormat[GFXIndexFormat16], pool, &ib),
      "GFXD3D8PrimitiveBuffer::resurrect - Failed to allocate an index buffer.");
}
