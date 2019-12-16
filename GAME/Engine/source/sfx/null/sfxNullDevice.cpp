//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"

#include "sfx/null/sfxNullDevice.h"
#include "sfx/null/sfxNullBuffer.h"
#include "sfx/sfxListener.h"


SFXNullDevice::SFXNullDevice( SFXProvider* provider, 
                              const char* name, 
                              bool useHardware, 
                              S32 maxBuffers )

   :  SFXDevice( provider, useHardware, maxBuffers ),
      mName( StringTable->insert( name ) )
{
   mMaxBuffers = getMax( maxBuffers, 8 );
}

SFXNullDevice::~SFXNullDevice()
{
   SFXNullBufferVector::iterator buffer = mBuffers.begin();
   for ( ; buffer != mBuffers.end(); buffer++ )
      delete (*buffer);
   mBuffers.clear();

   SFXNullVoiceVector::iterator voice = mVoices.begin();
   for ( ; voice != mVoices.end(); voice++ )
      delete (*voice);
   mVoices.clear();
}

SFXBuffer* SFXNullDevice::createBuffer( SFXProfile *profile )
{
   AssertFatal( profile, "SFXNullDevice::createBuffer() - Got null profile!" );

   SFXNullBuffer* buffer = new SFXNullBuffer();
   if ( !buffer )
      return NULL;

   mBuffers.push_back( buffer );
   return buffer;
}

SFXVoice* SFXNullDevice::createVoice( bool is3D, SFXBuffer *buffer, U32 priority=0 )
{
   // Don't bother going any further if we've 
   // exceeded the maximum voices.
   if ( mVoices.size() >= mMaxBuffers )
      return NULL;

   AssertFatal( buffer, "SFXNullDevice::createVoice() - Got null buffer!" );

   SFXNullBuffer* nullBuffer = dynamic_cast<SFXNullBuffer*>( buffer );
   AssertFatal( nullBuffer, "SFXNullDevice::createVoice() - Got bad buffer!" );

   SFXNullVoice* voice = new SFXNullVoice();
   if ( !voice )
      return NULL;

   mVoices.push_back( voice );
	return voice;
}

void SFXNullDevice::deleteVoice( SFXVoice* voice )
{
   AssertFatal( voice, "SFXNullDevice::deleteVoice() - Got null voice!" );

   SFXNullVoice* nullVoice = dynamic_cast<SFXNullVoice*>( voice );
   AssertFatal( nullVoice, "SFXNullDevice::deleteVoice() - Got bad voice!" );

	// Find the buffer...
	SFXNullVoiceVector::iterator iter = find( mVoices.begin(), mVoices.end(), nullVoice );
	AssertFatal( iter != mVoices.end(), "SFXNullDevice::deleteVoice() - Got unknown voice!" );

	mVoices.erase( iter );
	delete nullVoice;
}

void SFXNullDevice::update( const SFXListener& listener )
{
}