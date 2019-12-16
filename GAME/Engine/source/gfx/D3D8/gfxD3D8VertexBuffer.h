//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFXD3D8_VERTEXBUFFER_H_
#define _GFXD3D8_VERTEXBUFFER_H_

#include "gfx/gfxVertexBuffer.h"
#include "gfx/D3D8/gfxD3D8Device.h"

//*****************************************************************************
// GFXD3D8VertexBuffer 
//*****************************************************************************
class GFXD3D8VertexBuffer : public GFXVertexBuffer 
{
public:
   IDirect3DVertexBuffer8      *vb;
   RefPtr<GFXD3D8VertexBuffer> mVolatileBuffer;

   bool                        mIsFirstLock;

   GFXD3D8VertexBuffer();
   GFXD3D8VertexBuffer(GFXDevice *device, U32 numVerts, U32 vertexType, U32 vertexSize, GFXBufferType bufferType);
   ~GFXD3D8VertexBuffer();

   void lock(U32 vertexStart, U32 vertexEnd, void **vertexPtr);
   void unlock();
   void prepare();


#ifdef POWER_DEBUG
   char *name; 

   /// In debug compile, the verts will be chained together and the device
   /// will examine the chain when it's destructor is called, this will
   /// allow developers to see which vertex buffers are not destroyed
   GFXD3D8VertexBuffer *next;
#endif
   void setName( const char *n );

   // GFXResource interface
   virtual void zombify();
   virtual void resurrect();
};

//-----------------------------------------------------------------------------
// This is for debugging vertex buffers and trying to track down which vbs
// aren't getting free'd

inline GFXD3D8VertexBuffer::GFXD3D8VertexBuffer()
  : GFXVertexBuffer(0,0,0,0,(GFXBufferType)0)
{
#ifdef POWER_DEBUG
   name = NULL;
#endif
   vb = NULL;
   mIsFirstLock = true;
   lockedVertexEnd = lockedVertexStart = 0;
}

inline GFXD3D8VertexBuffer::GFXD3D8VertexBuffer(GFXDevice *device, U32 numVerts, U32 vertexType, U32 vertexSize, GFXBufferType bufferType)
   : GFXVertexBuffer(device, numVerts, vertexType, vertexSize, bufferType)
{
#ifdef POWER_DEBUG
   name = NULL;
#endif
   vb = NULL;
   mIsFirstLock = true;
   lockedVertexEnd = lockedVertexStart = 0;
}

inline GFXD3D8VertexBuffer::~GFXD3D8VertexBuffer() 
{

#ifdef POWER_DEBUG
   SAFE_DELETE( name );
#endif

   if(mBufferType == GFXBufferTypeDynamic)
   {
      ((GFXD3D8Device*)mDevice)->deallocVertexBuffer( this );
   }

   // don't want to kill entire volatile pool  
   if(mBufferType != GFXBufferTypeVolatile)
   {
      SAFE_RELEASE( vb );
   }
}

#ifdef POWER_DEBUG

inline void GFXD3D8VertexBuffer::setName( const char *n ) 
{
   SAFE_DELETE( name );
   S32 length = dStrlen(n) + 1;
   name = new char[length];
   dStrcpy( name, length, n );
}

#else

inline void GFXD3D8VertexBuffer::setName( const char *n ) { }

#endif

#endif // _GFXD3D_VERTEXBUFFER_H_

