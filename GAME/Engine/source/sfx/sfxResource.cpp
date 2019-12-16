//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "console/console.h"

#include "sfx/sfxResource.h"
#include "sfx/sfxWavResource.h"
#include "sfx/sfxMP3Resource.h"
//#ifndef POWER_NO_OGGVORBIS
//   #include "sfx/vorbis/sfxOggResource.h"
//#endif


// TODO: This class should support threaded loading of data in 
// the background.  Here is how it could work... 
//
// The header data is always read in the foreground thread when
// the resource is created.
//
// We add a "mThreadedLoad" option to the SFXProfile which 
// triggers the threaded load.
//
// When the data is requested the mThreadedLoad option is
// passed.  
//
// ?????


Resource<SFXResource> SFXResource::load( const char* filename )
{
   // Sound resources have a load on demand feature that
   // isn't directly supported by the resource manager.
   // To put loading under our control we add the resource
   // to the system ourselves giving it just the filename
   // used for loading later.

   // First see if the resource has been cached or can 
   // load directly without us futzing with the extension.
   Resource<SFXResource> buffer = ResourceManager->load( filename );
   if ( (bool)buffer )
      return buffer;

   // We didn't find the file with its given name so we will loop through
   // a list of other valid extensions and see if an alternate file exists
   // First strip off our current extension (validating against a list of
   // known extensions so that we don't strip off the last part of a file
   // name with a dot in it
   const char* noExtension = stripExtension(filename, SOUND_EXT_ARRAY_SIZE, soundExtArray);

   // Then try some of the other possible filenames
   for( U32 i = 0; i < SOUND_EXT_ARRAY_SIZE; i++ ) 
   {
      StringTableEntry testName = addExtension(noExtension, soundExtArray[i]);

      Stream* stream = ResourceManager->openStream( testName );
      if ( stream )
      {
         RESOURCE_CREATE_FN createFunction = ResourceManager->getCreateFunction (soundExtArray[i]);

         if(!createFunction)
         {
			 ResourceManager->closeStream( stream );
             AssertWarn( false, avar("SFXResource::load: NULL resource create function for '%s'.", soundExtArray[i]) );
             Con::errorf("SFXResource::load: NULL resource create function for '%s'.", soundExtArray[i]);
             return NULL;
         }

         ResourceInstance* res = createFunction( *stream, NULL );

         if (res)
         {
            ResourceManager->add( testName, res );
            ResourceManager->closeStream( stream );

            return ResourceManager->load( testName );
         }

		 ResourceManager->closeStream( stream );
      }
   }

   return NULL;
}


bool SFXResource::exists( const char* filename )
{
   // First check to see if the resource manager can find it.
   if ( ResourceManager->getPathOf( filename ) )
      return true;

   // We didn't find the file with its given name so we will loop through
   // a list of other valid extensions and see if an alternate file exists
   // First strip off our current extension (validating against a list of
   // known extensions so that we don't strip off the last part of a file
   // name with a dot in it
   const char* noExtension = stripExtension(filename, SOUND_EXT_ARRAY_SIZE, soundExtArray);

   // Then try some of the other possible filenames
   for( U32 i = 0; i < SOUND_EXT_ARRAY_SIZE; i++ ) 
   {
      StringTableEntry testName = addExtension(noExtension, soundExtArray[i]);

      if ( ResourceManager->getPathOf( testName ) )
         return true;
   }

   return false;
}


SFXResource::SFXResource()
   :  mFormat( SFX_FORMAT_MONO16 ),
      mData( NULL ),
      mSize( 0 ),
      mFrequency( 22050 ),
      mLength( 0 ),
	  mFiletype(0)
{
}


SFXResource::~SFXResource()
{
   delete [] mData;
}

U32 SFXResource::getChannels() const
{
   switch( mFormat )
   {
      case SFX_FORMAT_MONO8:
      case SFX_FORMAT_MONO16:
         return 1;

      case SFX_FORMAT_STEREO8:
      case SFX_FORMAT_STEREO16:
         return 2;
   };

   return 0;
}

U32 SFXResource::getSampleBytes() const
{
   switch( mFormat )
   {
      case SFX_FORMAT_MONO8:
         return 1;

      case SFX_FORMAT_STEREO8:
      case SFX_FORMAT_MONO16:
         return 2;

      case SFX_FORMAT_STEREO16:
         return 4;
   };

   return 0;
}

U32 SFXResource::getPosition( U32 ms ) const
{
   if ( ms > mLength )
      ms = mLength;

   U32 bytes = ( ( ms * mFrequency ) * getSampleBytes() ) / 1000;
   return bytes;
}
