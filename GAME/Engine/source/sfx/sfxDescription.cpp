//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"

#include "sfx/sfxDescription.h"
#include "sfx/sfxSystem.h"
#include "sim/netConnection.h"
#include "core/bitStream.h"


IMPLEMENT_CO_DATABLOCK_V1( SFXDescription );


SFXDescription::SFXDescription()
   :  SimDataBlock(),
      mVolume( 1 ),
      mLoopcount( 0 ),
      mIsStreaming( false ),
      mIs3D( false ),
      mReferenceDistance( 1 ),
      mMaxDistance( 100 ),
      mConeInsideAngle( 360 ),
      mConeOutsideAngle( 360 ),
      mConeOutsideVolume( 1 ),
      mChannel( 0 ),
	  mSoundID( "" ),
	  mPriority(1),
	  mSoundChannel(0)
{
}

SFXDescription::SFXDescription( const SFXDescription& desc )
   :  SimDataBlock(),
      mVolume( desc.mVolume ),
      mLoopcount( desc.mLoopcount ),
      mIsStreaming( desc.mIsStreaming ),
      mIs3D( desc.mIs3D ),
      mReferenceDistance( desc.mReferenceDistance ),
      mMaxDistance( desc.mMaxDistance ),
      mConeInsideAngle( desc.mConeInsideAngle ),
      mConeOutsideAngle( desc.mConeOutsideAngle ),
      mConeOutsideVolume( desc.mConeOutsideVolume ),
      mChannel( desc.mChannel ),
	  mSoundID( desc.mSoundID),
	  mPriority( desc.mPriority ),
	  mSoundChannel(desc.mSoundChannel)
{
}

IMPLEMENT_CONSOLETYPE( SFXDescription )
IMPLEMENT_GETDATATYPE( SFXDescription )
IMPLEMENT_SETDATATYPE( SFXDescription )


void SFXDescription::initPersistFields()
{
   Parent::initPersistFields();

   addField( "volume",            TypeF32,     Offset(mVolume, SFXDescription));
   addField( "loopcount",         TypeBool,    Offset(mLoopcount, SFXDescription));
   addField( "isStreaming",       TypeBool,    Offset(mIsStreaming, SFXDescription));
   addField( "is3D",              TypeBool,    Offset(mIs3D, SFXDescription));
   addField( "referenceDistance", TypeF32,     Offset(mReferenceDistance, SFXDescription));
   addField( "maxDistance",       TypeF32,     Offset(mMaxDistance, SFXDescription));
   addField( "coneInsideAngle",   TypeS32,     Offset(mConeInsideAngle, SFXDescription));
   addField( "coneOutsideAngle",  TypeS32,     Offset(mConeOutsideAngle, SFXDescription));
   addField( "coneOutsideVolume", TypeF32,     Offset(mConeOutsideVolume, SFXDescription));
   addField( "channel",           TypeS32,     Offset(mChannel, SFXDescription));
   addField( "soundID",			  TypeString,  Offset(mSoundID, SFXDescription));
   addField( "priority",		  TypeS32,	   Offset(mPriority, SFXDescription));
}


bool SFXDescription::onAdd()
{
   if ( !Parent::onAdd() )
      return false;

   // Validate the data we'll be passing to 
   // the audio layer.
   validate();

   return true;
}

void SFXDescription::validate()
{
   // Validate the data we'll be passing to the audio layer.
   mVolume = mClampF( mVolume, 0, 1 );

   mReferenceDistance = mClampF( mReferenceDistance, 0, mReferenceDistance );

   if ( mMaxDistance <= mReferenceDistance )
      mMaxDistance = mReferenceDistance + 0.01f;

   mConeInsideAngle     = mClamp( mConeInsideAngle, 0, 360 );
   mConeOutsideAngle    = mClamp( mConeOutsideAngle, mConeInsideAngle, 360 );
   mConeOutsideVolume   = mClampF( mConeOutsideVolume, 0, 1 );

   mChannel = mClamp( mChannel, 0, SFXSystem::NumChannels - 1 );
}

void SFXDescription::packData( BitStream *stream )
{
   Parent::packData( stream );

   stream->writeFloat( mVolume, 6 );

   stream->writeInt( mLoopcount, 9 );

   stream->writeFlag( mIsStreaming );
   stream->writeFlag( mIs3D );
   
   if ( mIs3D )
   {
      stream->write( mReferenceDistance );
      stream->write( mMaxDistance );

      stream->writeInt( mConeInsideAngle, 9 );
      stream->writeInt( mConeOutsideAngle, 9 );

      stream->writeFloat( mConeOutsideVolume, 6 );
   }

   stream->writeInt( mChannel, SFXSystem::NumChannelBits );
   stream->writeString(mSoundID);
}


void SFXDescription::unpackData( BitStream *stream )
{
   Parent::unpackData( stream );

   mVolume    = stream->readFloat( 6 );
   mLoopcount = stream->readInt(9);

   mIsStreaming   = stream->readFlag();
   mIs3D          = stream->readFlag();

   if ( mIs3D )
   {
      stream->read( &mReferenceDistance );
      stream->read( &mMaxDistance );

      mConeInsideAngle     = stream->readInt( 9 );
      mConeOutsideAngle    = stream->readInt( 9 );

      mConeOutsideVolume   = stream->readFloat( 6 );
   }

   mChannel = stream->readInt( SFXSystem::NumChannelBits );
   mSoundID = stream->readSTString();
}

//const Resource<SFXResource>& SFXDescription::getResource()
//{
//	if ( !mResource )
//		mResource = SFXResource::load( mFileName );
//
//	if ( !mResource.isNull() && SFX )
//		mBuffer = SFX->_createBuffer( this );
//
//	return mResource;
//}
//
//SFXBuffer* SFXDescription::getBuffer()
//{
//	if ( !mBuffer )
//	{
//		if ( !mResource )
//			mResource = SFXResource::load( mFileName );
//
//		if ( !mResource.isNull() && SFX )
//			mBuffer = SFX->_createBuffer( this );
//	}
//
//	return mBuffer;
//}





