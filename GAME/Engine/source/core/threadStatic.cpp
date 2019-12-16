//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "core/threadStatic.h"

//-----------------------------------------------------------------------------
// Statics
U32 _PowerEngineThreadStatic::mListIndex = 0;

_PowerEngineThreadStaticReg *_PowerEngineThreadStaticReg::smFirst = NULL;
//-----------------------------------------------------------------------------

inline Vector<PowerEngineThreadStaticList> &_PowerEngineThreadStaticReg::getThreadStaticListVector()
{
   // This function assures that the static vector of ThreadStatics will get initialized
   // before first use.
   static Vector<PowerEngineThreadStaticList> sPowerEngineThreadStaticVec;

   return sPowerEngineThreadStaticVec;
}

//-----------------------------------------------------------------------------

// Destructor, size should == 1 otherwise someone didn't clean up, or someone
// did horrible things to list index 0
_PowerEngineThreadStaticReg::~_PowerEngineThreadStaticReg()
{
   AssertFatal( getThreadStaticListVector().size() == 1, "Destruction of static list was not performed on program exit" );
}

//-----------------------------------------------------------------------------

void _PowerEngineThreadStaticReg::destroyInstances()
{
   // mThreadStaticInstances[0] does *not* need to be deallocated
   // because all members of the list are pointers to static memory
   while( getThreadStaticListVector().size() > 1 )
   {
      // Delete the members of this list
      while( getThreadStaticListVector().last().size() )
      {
         _PowerEngineThreadStatic *biscuit = getThreadStaticListVector().last().first();

         // Erase the vector entry
         getThreadStaticListVector().last().pop_front();

         // And finally the memory
         delete biscuit;
      }

      // Remove the entry from the list of lists
      getThreadStaticListVector().pop_back();
   }
}

//-----------------------------------------------------------------------------

void _PowerEngineThreadStaticReg::destroyInstance( PowerEngineThreadStaticList *instanceList )
{
   AssertFatal( instanceList != &getThreadStaticListVector().first(), "Cannot delete static instance list index 0" );

   while( instanceList->size() )
   {
      _PowerEngineThreadStatic *biscuit = getThreadStaticListVector().last().first();

      // Erase the vector entry
      getThreadStaticListVector().last().pop_front();

      // And finally the memory
      delete biscuit;
   }

   getThreadStaticListVector().erase( instanceList );
}

//-----------------------------------------------------------------------------

PowerEngineThreadStaticListHandle _PowerEngineThreadStaticReg::spawnThreadStaticsInstance()
{
   AssertFatal( getThreadStaticListVector().size() > 0, "List is not initialized somehow" );

   // Add a new list of static instances
   getThreadStaticListVector().increment();

   // Copy mThreadStaticInstances[0] (master copy) into new memory, and
   // pass it back.
   for( int i = 0; i < getThreadStaticListVector()[0].size(); i++ )
   {
      getThreadStaticListVector().last().push_back( getThreadStaticListVector()[0][i]->_createInstance() );
   }

   // Return list index of newly allocated static instance list
   return &getThreadStaticListVector().last();
}