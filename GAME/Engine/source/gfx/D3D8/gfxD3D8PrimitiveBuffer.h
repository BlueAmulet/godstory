//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFX_D3D8_GFXD3D8PRIMITIVEBUFFER_H_
#define _GFX_D3D8_GFXD3D8PRIMITIVEBUFFER_H_

#include "gfx/D3D8/gfxD3D8Device.h"

//-----------------------------------------------------------------------------

class GFXD3D8PrimitiveBuffer : public GFXPrimitiveBuffer 
{
   public:
      IDirect3DIndexBuffer8 *ib;
      RefPtr<GFXD3D8PrimitiveBuffer> mVolatileBuffer;
      U32 mVolatileStart;

      bool mLocked;
      bool                  mIsFirstLock;

      GFXD3D8PrimitiveBuffer(GFXDevice *device, U32 indexCount, U32 primitiveCount, GFXBufferType bufferType);
      ~GFXD3D8PrimitiveBuffer();

      virtual void lock(U16 indexStart, U16 indexEnd, U16 **indexPtr);
      virtual void unlock();

      virtual void prepare();      

      // GFXResource interface
      virtual void zombify();
      virtual void resurrect();

#ifdef POWER_DEBUG
   //GFXD3D8PrimitiveBuffer *next;
#endif
};

inline GFXD3D8PrimitiveBuffer::GFXD3D8PrimitiveBuffer(GFXDevice *device, U32 indexCount, U32 primitiveCount, GFXBufferType bufferType) 
   : GFXPrimitiveBuffer(device, indexCount, primitiveCount, bufferType)
{
   mVolatileStart = 0;
   ib             = NULL;
   mIsFirstLock   = true;
   mLocked = false;
}

#endif
