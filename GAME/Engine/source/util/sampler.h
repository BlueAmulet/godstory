//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _UTILSAMPLER_H_
#define _UTILSAMPLER_H_

#include "platform/types.h"

/// The sampling framework.
///
/// Sampling allows per-frame snaphots of specific values to be logged.  For
/// each value that you want to have sampled, you define a sampling key and
/// then simply call the sample function at an appropriate place.  If you
/// want to sample the same value multiple times within a single frame, simply
/// register several keys for it.
///
/// The easiest way to use this facility is with the SAMPLE macro.
///
/// @code
/// SAMPLE( "my/sample/value", my.sample->val );
/// @endcode
///
/// @section Using the Sampler
///
/// Before you use the sampler it is important that you let your game run for
/// some frames and make sure that all relevant code paths have been touched (i.e.
/// if you want to sample Atlas data, have an Atlas instance on screen).  This
/// will ensure that sampling keys are registered with the sampler.
///
/// Then use the console to first enable the keys you are interested in.  For
/// example, to enable sampling for all Atlas keys:
///
/// @code
/// enableSamples( "atlas/*" );
/// @endcode
///
/// Finally, you have to start the actual sampling.  This is achieved with the
/// beginSampling console function that takes a string informing the backend
/// where to store sample data and optionally a name of the specific logging backend
/// to use.  The default is the CSV backend.  In most cases, the logging store
/// will be a file name.
///
/// @code
/// beginSampling( "mysamples.csv" );
/// @endcode
///
/// To stop sampling, use:
///
/// @code
/// stopSampling();
/// @endcode
///
/// @section Sample Keys
///
/// Sample key name should generally follow the pattern "path/to/group/samplename".
/// This allows to very easily enable or disable specific sets of keys using
/// wildcards.
///
/// Note that sampling keys are case-insensitive.

namespace Sampler
{
   void init();
   void destroy();

   void beginFrame();
   void endFrame();

   void sample( U32 key, bool value );
   void sample( U32 key, S32 value );
   void sample( U32 key, F32 value );
   void sample( U32 key, const char* value );

   /// Register a new sample key.
   ///
   /// @note Note that all keys are disabled by default.
   U32 registerKey( const char* name );

   /// Enable sampling for all keys that match the given name
   /// pattern.  Slashes are treated as separators.
   void enableKeys( const char* pattern, bool state = true );
};

#ifdef POWER_ENABLE_SAMPLING
#  define SAMPLE( name, value )           \
{                                         \
   static U32 key;                        \
   if( !key )                             \
      key = Sampler::registerKey( name ); \
   Sampler::sample( key, value );         \
}
#else
#  define SAMPLE( name, value )
#endif

#endif // _UTILSAMPLER_H_
