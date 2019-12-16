//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include <d3d8.h>
#include <d3dx8math.h>
#include "gfx/D3D8/GFXD3D8VertexBuffer.h"


//-----------------------------------------------------------------------------
// Lock
//-----------------------------------------------------------------------------
void GFXD3D8VertexBuffer::lock(U32 vertexStart, U32 vertexEnd, void **vertexPtr)
{
   AssertFatal(lockedVertexStart == 0 && lockedVertexEnd == 0, "Cannot lock a buffer more than once!");

   U32 flags = 0;
   
   switch(mBufferType)
   {
      case GFXBufferTypeStatic:
         //flags |= D3DLOCK_DISCARD;
         break;

      case GFXBufferTypeDynamic:
         flags |= D3DLOCK_DISCARD;
         break;

      case GFXBufferTypeVolatile:

         // Get or create the volatile buffer...
         GFXD3D8Device *d = static_cast<GFXD3D8Device*>(mDevice);

         mVolatileBuffer = d->findVBPool( mVertexType );
         if( !mVolatileBuffer )
            mVolatileBuffer = d->createVBPool( mVertexType, mVertexSize );

         vb = mVolatileBuffer->vb;

         // Get our range now...
         AssertFatal(vertexStart == 0,              "Cannot get a subrange on a volatile buffer.");
         AssertFatal(vertexEnd < MAX_DYNAMIC_VERTS, "Cannot get more than MAX_DYNAMIC_VERTS in a volatile buffer. Up the constant!");
         AssertFatal(mVolatileBuffer->lockedVertexStart == 0 && mVolatileBuffer->lockedVertexEnd == 0, "Got more than one lock on the volatile pool.");

         // We created the pool when we requested this volatile buffer, so assume it exists...
         if( mVolatileBuffer->mNumVerts + vertexEnd > MAX_DYNAMIC_VERTS ) 
         {
            flags |= D3DLOCK_DISCARD;
            mVolatileStart = vertexStart  = 0;
            vertexEnd      = vertexEnd;
         }
         else 
         {
            flags |= D3DLOCK_NOOVERWRITE;
            mVolatileStart = vertexStart  = mVolatileBuffer->mNumVerts;
            vertexEnd                    += mVolatileBuffer->mNumVerts;
         }

         mVolatileBuffer->mNumVerts = vertexEnd+1;

         mVolatileBuffer->lockedVertexStart = vertexStart;
         mVolatileBuffer->lockedVertexEnd   = vertexEnd;
         break;
   }

   lockedVertexStart = vertexStart;
   lockedVertexEnd   = vertexEnd;

//      Con::printf("%x: Locking %s range (%d, %d)", this, (mBufferType == GFXBufferTypeVolatile ? "volatile" : "static"), lockedVertexStart, lockedVertexEnd);

   D3D8Assert( vb->Lock(vertexStart * mVertexSize, (vertexEnd - vertexStart) * mVertexSize, (BYTE**)vertexPtr, flags),
              "Unable to lock vertex buffer.");

}

//-----------------------------------------------------------------------------
// Unlock
//-----------------------------------------------------------------------------
void GFXD3D8VertexBuffer::unlock()
{
   D3D8Assert( vb->Unlock(),
              "Unable to unlock vertex buffer.");
   mIsFirstLock = false;

//      Con::printf("%x: Unlocking %s range (%d, %d)", this, (mBufferType == GFXBufferTypeVolatile ? "volatile" : "static"), lockedVertexStart, lockedVertexEnd);

   lockedVertexEnd = lockedVertexStart = 0;

   if(mVolatileBuffer.isValid())
   {
      mVolatileBuffer->lockedVertexStart = 0;
      mVolatileBuffer->lockedVertexEnd   = 0;
      mVolatileBuffer = NULL;
      //vb->Release();
      //vb = NULL;
   }
}

//-----------------------------------------------------------------------------
// Prepare
//-----------------------------------------------------------------------------
void GFXD3D8VertexBuffer::prepare()
{
	((GFXD3D8Device*)mDevice)->setVB(this);
}

void GFXD3D8VertexBuffer::zombify()
{
   AssertFatal(lockedVertexStart == 0 && lockedVertexEnd == 0, "GFXD3D8VertexBuffer::zombify - Cannot zombify a locked buffer!");
   // Static buffers are managed by D3D
   if(mBufferType == GFXBufferTypeDynamic)
   {
      SAFE_RELEASE(vb);
   }
}

void GFXD3D8VertexBuffer::resurrect()
{
   // Static buffers are managed by D3D
   if(mBufferType == GFXBufferTypeDynamic)
   {
      D3D8Assert(static_cast<GFXD3D8Device*>(mDevice)->mD3DDevice->CreateVertexBuffer( mVertexSize * mNumVerts,
         D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
         mVertexType, 
         D3DPOOL_DEFAULT,
         &vb),
         "GFXD3D8VertexBuffer::resurrect - Failed to allocate VB" );
   }
}
