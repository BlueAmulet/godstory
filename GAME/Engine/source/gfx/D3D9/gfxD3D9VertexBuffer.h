//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFXD3D_VERTEXBUFFER_H_
#define _GFXD3D_VERTEXBUFFER_H_

#include "gfx/gfxVertexBuffer.h"
#include "gfx/D3D9/gfxD3D9Device.h"

#define MAX_STREAM_NUM 10
//*****************************************************************************
// GFXD3D9VertexBuffer 
//*****************************************************************************
class GFXD3D9VertexBuffer : public GFXVertexBuffer
{
public:
   IDirect3DVertexBuffer9 *vb;
   IDirect3DVertexDeclaration9 *vDecl;
   RefPtr<GFXD3D9VertexBuffer> mVolatileBuffer;

   bool mIsFirstLock;
   bool mClearAtFrameEnd;
   bool mZombifyFlag;

   int mStreamVertexSize[MAX_STREAM_NUM];


   GFXD3D9VertexBuffer();
   GFXD3D9VertexBuffer(GFXDevice *device, U32 numVerts, U32 vertexType, U32 vertexSize, GFXBufferType bufferType,U8 streamNum = 0);
   virtual ~GFXD3D9VertexBuffer();

   void lock(U32 vertexStart, U32 vertexEnd, void **vertexPtr);
   void unlock();
   bool isNull(){	return (vb==NULL || mZombifyFlag);}
   void prepare();
   void declare(const GFXStreamVertexAttribute *pDecl,int elementNum);


#ifdef POWER_DEBUG
   char *name; 

   /// In debug compile, the verts will be chained together and the device
   /// will examine the chain when it's destructor is called, this will
   /// allow developers to see which vertex buffers are not destroyed
   GFXD3D9VertexBuffer *next;
#endif
   void setName( const char *n );

   // GFXResource interface
   virtual void zombify();
   virtual void resurrect();
};

//-----------------------------------------------------------------------------
// This is for debugging vertex buffers and trying to track down which vbs
// aren't getting free'd

inline GFXD3D9VertexBuffer::GFXD3D9VertexBuffer()
: GFXVertexBuffer(0,0,0,0,(GFXBufferType)0)
{
#ifdef POWER_DEBUG
   name = NULL;
#endif
   vb = NULL;
   vDecl = NULL;
   mIsFirstLock = true;
   lockedVertexEnd = lockedVertexStart = 0;
   mClearAtFrameEnd = false;
   mZombifyFlag = false;

   memset(mStreamVertexSize,0,sizeof(int)*MAX_STREAM_NUM);
}

inline GFXD3D9VertexBuffer::GFXD3D9VertexBuffer(GFXDevice *device, U32 numVerts, U32 vertexType, U32 vertexSize, GFXBufferType bufferType,U8 streamNum)
: GFXVertexBuffer(device, numVerts, vertexType, vertexSize, bufferType)
{
#ifdef POWER_DEBUG
   name = NULL;
#endif
   vb = NULL;
   vDecl = NULL;
   mIsFirstLock = true;
   mClearAtFrameEnd = false;
   lockedVertexEnd = lockedVertexStart = 0;
   mStreamNum = streamNum;
   memset(mStreamVertexSize,0,sizeof(int)*MAX_STREAM_NUM);
}

#ifdef POWER_DEBUG

inline void GFXD3D9VertexBuffer::setName( const char *n ) 
{
   SAFE_DELETE( name );
   S32 length = dStrlen(n) + 1;
   name = new char[length];
   dStrcpy( name, length, n );
}

#else

inline void GFXD3D9VertexBuffer::setName( const char *n ) { }

#endif

#endif // _GFXD3D_VERTEXBUFFER_H_

