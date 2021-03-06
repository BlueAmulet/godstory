//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "unit/test.h"
#include "unit/memoryTester.h"
#include "util/swizzle.h"
#include "math/mRandom.h"

using namespace UnitTesting;

class TestStruct
{
private:
   static U32 smIdx;
   U32 mIdx;
   U32 mData;
   
public:
   TestStruct( const S32 data = -1 ) : mData( data ), mIdx( smIdx++ ) {};

   dsize_t Idx() const { return mIdx; }

   U32 Data() const { return mData; }
   void Data(U32 val) { mData = val; }
};

U32 TestStruct::smIdx = 0;

CreateUnitTest(TestSwizzle, "Utils/Swizzle")
{
   void run()
   {
      //------------------------------------------------------------------------
      // Debugger-Observable Functionality Tests
      //------------------------------------------------------------------------
      U8 simpleBuffer[] = { 0, 1, 2, 3 };
      U8 simpleTest[] = { 0, 1, 2, 3 };

#define RESET_SIMPLE() dMemcpy( simpleTest, simpleBuffer, sizeof( simpleBuffer ) )

      //------------------------------------------------------------------------
      // No-switch test
      dsize_t noSwzl4[] = { 0, 1, 2, 3 };
      Swizzle<U8,4> noSwizzle4( noSwzl4 );

      noSwizzle4.InPlace( simpleTest, sizeof( simpleTest ) );
      test( dMemcmp( simpleTest, simpleBuffer, sizeof( simpleBuffer ) ) == 0, "No-switch test failed!" );
      RESET_SIMPLE();

      //------------------------------------------------------------------------
      // No-brainer RGBA->BGRA test
      dsize_t bgraSwzl[] = { 2, 1, 0, 3 };
      Swizzle<U8,4> bgraSwizzle( bgraSwzl );

      U8 bgraTest[] = { 2, 1, 0, 3 };
      bgraSwizzle.InPlace( simpleTest, sizeof( simpleTest ) );
      test( dMemcmp( simpleTest, bgraTest, sizeof( bgraTest ) ) == 0, "U8 RGBA->BGRA test failed" );
      
      //------------------------------------------------------------------------
      // Reverse test
      bgraSwizzle.InPlace( simpleTest, sizeof( simpleTest ) );
      test( dMemcmp( simpleTest, simpleBuffer, sizeof( simpleBuffer ) ) == 0, "U8 RGBA->BGRA reverse test failed" );

      RESET_SIMPLE();

      //------------------------------------------------------------------------
      // Object support test
      Swizzle<TestStruct,4> bgraObjSwizzle( bgraSwzl );
      {
         U32 objIdx[] = { 0, 1, 2, 3 };

         FrameTemp<TestStruct> objTest( sizeof( objIdx ) / sizeof( U32 ) );
         FrameTemp<U32> randData( sizeof( objIdx ) / sizeof( U32 ) );

         bool same = true;

         for( int i = 0; i < sizeof( objIdx ) / sizeof( U32 ); i++ )
         {
            // Make random data and assign it
            randData[i] = gRandGen.randI();
            objTest[i].Data( randData[i] );

            // Continue object sanity check
            same &= ( objTest[i].Idx() == objIdx[i] );
         }

         test( same, "Test object failed to be competent" );

         bgraObjSwizzle.InPlace( ~objTest, sizeof( TestStruct ) * ( sizeof( objIdx ) / sizeof( U32 ) ) );
         same = true;

         for( U32 i = 0; i < sizeof( objIdx ) / sizeof( U32 ); i++ )
            same &= ( objTest[i].Idx() == bgraTest[i] ) && ( objTest[i].Data() == randData[ (U32)bgraTest[ i ] ] );

         test( same, "Object RGBA->BGRA test failed." );

         bgraObjSwizzle.InPlace( ~objTest, sizeof( TestStruct ) * ( sizeof( objIdx ) / sizeof( U32 ) ) );
         same = true;

         for( int i = 0; i < sizeof( objIdx ) / sizeof( U32 ); i++ )
            same &= ( objTest[i].Idx() == objIdx[i] ) && ( objTest[i].Data() == randData[i] );

         test( same, "Object RGBA->BGRA reverse test failed." );
      }
   }
};