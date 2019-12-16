//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"

#include "sfx/sfxProfile.h"
#include "sfx/sfxDescription.h"
#include "sfx/sfxSystem.h"
#include "sim/netConnection.h"
#include "core/bitStream.h"
#include "GamePlay/Data/SoundResource.h"

IMPLEMENT_CO_DATABLOCK_V1( SFXProfile );


SFXProfile::SFXProfile()
   :  mFilename( NULL ),
      mDescription( NULL ),
      mPreload( false ),
      mDescriptionId( 0 ),
	  mSoundID(NULL)
{
}

SFXProfile::SFXProfile( SFXDescription* desc, StringTableEntry filename, bool preload )
   :  mFilename( filename ),
      mDescription( desc ),
      mPreload( preload ),
      mDescriptionId( 0 ),
	  mSoundID(NULL)
{
}

SFXProfile::~SFXProfile()
{
	if(mDescription)
		delete mDescription;
}

IMPLEMENT_CONSOLETYPE( SFXProfile )
IMPLEMENT_GETDATATYPE( SFXProfile )
IMPLEMENT_SETDATATYPE( SFXProfile )


void SFXProfile::initPersistFields()
{
   Parent::initPersistFields();

   //addField( "filename",    TypeFilename,              Offset(mFilename, SFXProfile));
   //addField( "description", TypeSFXDescriptionPtr,     Offset(mDescription, SFXProfile));
   addField( "preload",     TypeBool,                 Offset(mPreload, SFXProfile));
   addField( "SoundID",		TypeString,				Offset(mSoundID, SFXProfile));
}


bool SFXProfile::onAdd()
{
   if ( !Parent::onAdd() )
      return false;

   //if (  mDescription == NULL &&
   //      mDescriptionId != 0 )
   //{
   //   if ( !Sim::findObject( mDescriptionId, mDescription ) )
   //   {
   //      Con::errorf( 
   //         "SFXProfile(%s)::onAdd: Invalid packet, bad description id: %d", 
   //         getName(), mDescriptionId );
   //   }

	 
   //}

   if(mSoundID != NULL)
   {
	   mDescription = g_SoundManager->FindProfile(mSoundID)->getDescription();
		mFilename	= mDescription->getFilename();
   }    

   if ( !mDescription )
   {
      Con::errorf( 
         "SFXProfile(%s)::onAdd: The profile is missing a description!", 
         getName() );
      return false;
   }

   if ( SFX )
   {
      // If preload is enabled we load the resource
      // and device buffer now to avoid a delay on
      // first playback.
      if ( mPreload && !_preloadBuffer() )
         Con::errorf( "SFXProfile(%s)::onAdd: The preload failed!", getName() );

      // We need to get device change notifications.
      SFXDevice::getEventSignal().notify( this, &SFXProfile::_onDeviceEvent );
   }

   return true;
}

bool SFXProfile::Insert(SFXDescription* desc)
{
	mDescription = desc;
	mFilename = mDescription->mFileName;
	mPreload = true;
	//_preloadBuffer();
	return mPreload;
}

void SFXProfile::onRemove()
{
   // Remove us from the signal.
   SFXDevice::getEventSignal().remove( this, &SFXProfile::_onDeviceEvent );

   Parent::onRemove();
}

bool SFXProfile::preload( bool server, char errorBuffer[256] )
{
   if ( !Parent::preload( server, errorBuffer ) )
      return false;

   // TODO: Investigate how NetConnection::filesWereDownloaded()
   // effects the system.

   // Validate the datablock... has nothing to do with mPreload.
   if (  !server &&
         NetConnection::filesWereDownloaded() &&
         ( !mFilename || !SFXResource::exists( mFilename ) ) )
      return false;

   return true;
}


void SFXProfile::packData(BitStream* stream)
{
   Parent::packData( stream );

   // audio description:
   if ( stream->writeFlag( mDescription ) )
   {
      stream->writeRangedU32( mDescription->getId(),  
                              DataBlockObjectIdFirst,
                              DataBlockObjectIdLast );
   }

   //
   char buffer[256];
   if ( !mFilename )
      buffer[0] = 0;
   else
      dStrncpy( buffer, sizeof(buffer), mFilename, 256 );
   stream->writeString( buffer );

   stream->writeFlag( mPreload );
   stream->writeString(mSoundID);
}


void SFXProfile::unpackData(BitStream* stream)
{
   Parent::unpackData( stream );

   // audio datablock:
   if ( stream->readFlag() )
      mDescriptionId = stream->readRangedU32( DataBlockObjectIdFirst, DataBlockObjectIdLast );

   char buffer[256];
   stream->readString( buffer ,256);
   mFilename = StringTable->insert( buffer );

   mPreload = stream->readFlag();

   mSoundID = stream->readSTString();
}

void SFXProfile::_onDeviceEvent( SFXDevice *device, SFXDeviceEventType evt )
{
   switch ( evt )
   {
      case SFXDeviceEvent_Create:
      {
         if ( mPreload && !_preloadBuffer() )
            Con::errorf( "SFXProfile::_onDeviceEvent: The preload failed! %s", getName() );
         break;
      }
   }
}

bool SFXProfile::_preloadBuffer()
{
   mBuffer = NULL;

   if ( !mResource )
      mResource = SFXResource::load( mFilename );

   if ( !mResource.isNull() && SFX )
      mBuffer = SFX->_createBuffer( this );

   return mBuffer;
}

const Resource<SFXResource>& SFXProfile::getResource()
{
   if ( !mResource )
      _preloadBuffer();

   return mResource;
}

SFXBuffer* SFXProfile::getBuffer()
{
   if ( !mBuffer )
      _preloadBuffer();

   return mBuffer;
}
