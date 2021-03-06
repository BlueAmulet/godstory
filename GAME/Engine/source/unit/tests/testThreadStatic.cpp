//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "unit/test.h"
#include "core/threadStatic.h"
#include "unit/memoryTester.h"

using namespace UnitTesting;

//-----------------------------------------------------------------------------
// This unit test will blow up without thread static support
#ifdef POWER_ENABLE_THREAD_STATICS

// Declare a test thread static
DITTS( U32, gUnitTestFoo, 42 );
DITTS( F32, gUnitTestF32, 1.0 );

CreateUnitTest( TestThreadStatic, "Core/ThreadStatic" )
{
   void run()
   {
      MemoryTester m;
      m.mark();

      // ThreadStatic list should be initialized right now, so lets see if it has
      // any entries.
      test( !_PowerEngineThreadStaticReg::getStaticList().empty(), "Self-registration has failed, or no statics declared" );

      // Spawn a new copy.
      PowerEngineThreadStaticListHandle testInstance = _PowerEngineThreadStaticReg::spawnThreadStaticsInstance();

      // Test the copy
      test( _PowerEngineThreadStaticReg::getStaticList( 0 ).size() == testInstance->size(), "Spawned static list has a different size from master copy." );

      // Make sure the size test passed before this is attempted
      if( lastTestPassed() ) 
      {
         // Traverse the list and compare it to the initial value copy (index 0)
         for( int i = 0; i < _PowerEngineThreadStaticReg::getStaticList().size(); i++ )
         {
            _PowerEngineThreadStatic *master = _PowerEngineThreadStaticReg::getStaticList()[i];
            _PowerEngineThreadStatic *cpy = (*testInstance)[i];

            // Make sure it is not the same memory
            test( master != cpy, "Copy not spawned properly." );

            // Make sure the sizes are the same
            test( master->getMemInstSize() == cpy->getMemInstSize(), "Size mismatch between master and copy" );

            // Make sure the initialization occurred properly
            if( lastTestPassed() )
               test( dMemcmp( master->getMemInstPtr(), cpy->getMemInstPtr(), master->getMemInstSize() ) == 0, "Initial value for spawned list is not correct" );
         }
      }

      // Test metrics if enabled
#ifdef POWER_ENABLE_THREAD_STATIC_METRICS
      U32 fooHitCount = (*testInstance)[_gUnitTestFooPowerEngineThreadStatic::getListIndex()]->getHitCount();
#endif

      // Set/get some values (If we test static metrics, this is hit 1)
      ATTS_(gUnitTestFoo, 1) = 55;

      // Test to see that the master copy and the spawned copy differ
      // (If we test metrics, this is hit 2, for the instance, and hit 1 for the master)
      test( ATTS_(gUnitTestFoo, 0) != ATTS_(gUnitTestFoo, 1 ) , "Assignment for spawned instanced memory failed" );

#ifdef POWER_ENABLE_THREAD_STATIC_METRICS
      U32 fooHitCount2 = (*testInstance)[_gUnitTestFooPowerEngineThreadStatic::getListIndex()]->getHitCount();
      test( fooHitCount2 == ( fooHitCount + 2 ), "Thread static metric hit count failed" );
#endif

      // Destroy instances
      _PowerEngineThreadStaticReg::destroyInstance( testInstance );

      // Now test the cleanup
      test( m.check(), "Memory leak in dynamic static allocation stuff." );
   }
};

#endif // POWER_ENABLE_THREAD_STATICS
