//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "unit/test.h"
#include "math/mMath.h"
#include "math/mRandom.h"

extern void default_matF_x_matF_C(const F32 *a, const F32 *b, F32 *mresult);
extern void mInstallLibrary_ASM();

// If we're x86 and not Mac, then include these. There's probably a better way to do this.
#if defined(POWER_CPU_X86) && !defined(POWER_OS_MAC)
extern void Athlon_MatrixF_x_MatrixF(const F32 *matA, const F32 *matB, F32 *result);
extern void SSE_MatrixF_x_MatrixF(const F32 *matA, const F32 *matB, F32 *result);
#endif

#if defined( __VEC__ )
extern void vec_MatrixF_x_MatrixF(const F32 *matA, const F32 *matB, F32 *result);
#endif

using namespace UnitTesting;

CreateUnitTest( TestMatrixMul, "Math/Matrix/Multiply" )
{
   // The purpose of this test is to verify that the matrix multiplication operation
   // always agrees with the different implementations of itself within a reasonable
   // epsilon.
   void run()
   {
      F32 m1[16], m2[16], mrC[16];

      // I am not positive that the best way to do this is to use random numbers
      // but I think that using some kind of standard matrix may not always catch
      // all problems.
      for( int i = 0; i < 16; i++ )
      {
         m1[i] = gRandGen.randF();
         m2[i] = gRandGen.randF();
      }

      // C will be the baseline
      default_matF_x_matF_C( m1, m2, mrC );

      // Check the CPU info
      U32 cpuProperties = Platform::SystemInfo.processor.properties;
      bool same = true;

#if defined(POWER_CPU_X86) && !defined(POWER_OS_MAC)
      // Test 3D NOW! if it is available
      F32 mrAMD[16];
      if( cpuProperties & CPU_PROP_3DNOW )
      {
         Athlon_MatrixF_x_MatrixF( m1, m2, mrAMD );

         for( int i = 0; i < 16; i++ )
            same &= isEqual( mrC[i], mrAMD[i] );

         test( same, "Matrix multiplication verification failed. (C vs. 3D NOW!)" );
      }
      else
         warn( "Could not test 3D NOW! matrix multiplication because CPU does not support 3D NOW!." );

      same = true;

      // Test SSE if it is available
      F32 mrSSE[16];
      if( cpuProperties & CPU_PROP_SSE )
      {
         SSE_MatrixF_x_MatrixF( m1, m2, mrSSE );

         for( int i = 0; i < 16; i++ )
            same &= isEqual( mrC[i], mrSSE[i] );

         test( same, "Matrix multiplication verification failed. (C vs. SSE)" );
      }
      else
         warn( "Could not test SSE matrix multiplication because CPU does not support SSE." );

      same = true;
#endif

      // If Altivec exists, test it!
#if defined( __VEC__ )
      F32 mrVEC[16];

      vec_MatrixF_x_MatrixF( m1, m2, mrVEC );

      for( int i = 0; i < 16; i++ )
         same &= isEqual( mrC[i], mrVEC[i] );

      test( same, "Matrix multiplication verification failed. (C vs. Altivec)" );
#else
      warn( "Could not test Altivec matrix multiplication because CPU does not support Altivec." );
#endif
   }
};