//-----------------------------------------------------------------------------
// PowerEngine Shader Engine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _LIGHTALLOCATOR_H_
#define _LIGHTALLOCATOR_H_

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

class LightInfo;
class LightManager;


/// This is a simple class for managing the allocation 
/// of a bunch of lights that will be recycled on the 
/// next rendered frame.
///
/// TODO: If fragmentation becomes an issue we should
/// consider moving to a Chunker<sgLightInfo> instead.
///
class LightAllocator 
{
public:
   LightAllocator();
   virtual ~LightAllocator();

   /// Deletes all the lights.
   void clear();

   /// Ensures that the requested amount of lights are available in the free list.
   void reserve( LightManager* lm, S32 count );   
  
   /// Allocates a new light or pulls one from the free list.
   LightInfo* alloc( LightManager* lm );

   /// Moves all the lights into the free list.
   void free();

protected:
   Vector<LightInfo*> mFreeLights;
   Vector<LightInfo*> mAllocLights;
};


#endif // _LIGHTALLOCATOR_H_
