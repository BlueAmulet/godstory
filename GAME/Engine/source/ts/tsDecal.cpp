//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "ts/tsDecal.h"
#include "math/mMath.h"
#include "math/mathIO.h"
#include "ts/tsShapeInstance.h"
#include "core/frameAllocator.h"

//-----------------------------------------------------
// TSDecalMesh assembly/dissembly methods
// used for transfer to/from memory buffers
//-----------------------------------------------------

#define alloc TSShape::alloc

void TSDecalMesh::assemble(bool)
{
   CLockGuard guard(alloc.GetMutex());

   if (TSShape::smReadVersion<20)
   {
      // read empty mesh...decals used to be derived from meshes
      alloc.checkGuard();
      alloc.getPointer32(15);
   }

   S32 sz = alloc.get32();
   S32 * ptr32 = alloc.copyToShape32(0); // get current shape address w/o doing anything
   for (S32 i=0; i<sz; i++)
   {
      alloc.copyToShape16(2);
      alloc.copyToShape32(1);
   }
   alloc.align32();
   primitives.set(ptr32,sz);

   sz = alloc.get32();
   S16 * ptr16 = alloc.copyToShape16(sz);
   alloc.align32();
   indices.set(ptr16,sz);

   if (TSShape::smReadVersion<20)
   {
      // read more empty mesh stuff...decals used to be derived from meshes
      alloc.getPointer32(3);
      alloc.checkGuard();
   }

   sz = alloc.get32();
   ptr32 = alloc.copyToShape32(sz);
   startPrimitive.set(ptr32,sz);

   if (TSShape::smReadVersion>=19)
   {
      ptr32 = alloc.copyToShape32(sz*4);
      texgenS.set(ptr32,startPrimitive.size());
      ptr32 = alloc.copyToShape32(sz*4);
      texgenT.set(ptr32,startPrimitive.size());
   }
   else
   {
      texgenS.set(NULL,0);
      texgenT.set(NULL,0);
   }

   materialIndex = alloc.get32();

   alloc.checkGuard();
}

void TSDecalMesh::disassemble()
{
   CLockGuard guard(alloc.GetMutex());

   alloc.set32(primitives.size());
   for (S32 i=0; i<primitives.size(); i++)
   {
      alloc.copyToBuffer16((S16*)&primitives[i],2);
      alloc.copyToBuffer32(((S32*)&primitives[i])+1,1);
   }

   alloc.set32(indices.size());
   alloc.copyToBuffer16((S16*)indices.address(),indices.size());

   alloc.set32(startPrimitive.size());
   alloc.copyToBuffer32((S32*)startPrimitive.address(),startPrimitive.size());

   alloc.copyToBuffer32((S32*)texgenS.address(),texgenS.size()*4);
   alloc.copyToBuffer32((S32*)texgenT.address(),texgenT.size()*4);

   alloc.set32(materialIndex);

   alloc.setGuard();
}






