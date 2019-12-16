//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "sfx/sfxSystem.h"
#include "sfx/fmod/sfxFMODVoice.h"

#include "sfx/fmod/sfxFMODBuffer.h"
#include "sfx/fmod/sfxFMODDevice.h"
#include "core/tAlgorithm.h"
#include "console/console.h"

SFXFMODVoice* SFXFMODVoice::create( SFXFMODDevice *device,
                                    SFXFMODBuffer *buffer, U32 pririty)
{
   AssertFatal( device, "SFXFMODVoice::create() - Got null device!" );
   AssertFatal( buffer, "SFXFMODVoice::create() - Got null buffer!" );

   SFXFMODVoice* voice = new SFXFMODVoice( device, buffer, pririty);

   /*
   // A voice should have a channel assigned 
   // for its entire lifetime.
   if ( !voice->_assignChannel() )
   {
      delete voice;
      return NULL;
   }
   */

   return voice;
}

SFXFMODVoice::SFXFMODVoice(   SFXFMODDevice *device, 
                              SFXFMODBuffer *buffer, U32 priority)
   :  mDevice( device ),
      mBuffer( buffer ),
      mChannel( NULL )
{
   AssertFatal( device, "SFXFMODVoice::SFXFMODVoice() - No device assigned!" );
   AssertFatal( buffer, "SFXFMODVoice::SFXFMODVoice() - No buffer assigned!" );
   AssertFatal( mBuffer->mSound != NULL, "SFXFMODVoice::SFXFMODVoice() - No sound assigned!" );

   mPriority = priority;
   // Assign a channel when the voice is created.
   _assignChannel();
}

SFXFMODVoice::~SFXFMODVoice()
{
	stop();
//	SFX->clearChannelMap();
}

bool SFXFMODVoice::_assignChannel()
{
   AssertFatal( mBuffer->mSound != NULL, "SFXFMODVoice::_assignChannel() - No sound assigned!" );

   // we start playing it now in the paused state, so that we can immediately set attributes that
   // depend on having a channel (position, volume, etc).  According to the FMod docs
   // it is ok to do this.
   FMOD_RESULT result;
   result = SFXFMODDevice::smFunc->FMOD_System_PlaySound(
      SFXFMODDevice::smSystem, 
	  FMOD_CHANNEL_FREE, 
      mBuffer->mSound, 
      true, 
      &mChannel );
   

   SFXSystem::ChannelMap::iterator it = SFX->mChannelMap.find(mBuffer->mSoundID);
   if(it == SFX->mChannelMap.end() && mBuffer->getSoundChannel() == 0)
   {
	   SFX->mChannelMap[mBuffer->mSoundID] = mChannel;
	   if(dStrcmp(mBuffer->mSoundID, StringTable->insert("B_001")) == 0)
		   SFX->mGlobal = mChannel;
   }
   // <Edit> [3/4/2009 icerain] 优先级，group，声音设置
   F32 volume;
   FMOD_BOOL isTrue = false;
   SFXFMODDevice::smFunc->FMOD_Channel_IsPlaying(mChannel, &isTrue);
  
   if(isTrue)
   {
	   SFXFMODDevice::smFunc->FMOD_Channel_SetPriority(mChannel, mPriority);
   }
    
   switch (mBuffer->getSoundChannel())
   {
   case 0:
		SFXFMODDevice::smFunc->FMOD_Channel_SetChannelGroup(mChannel, mDevice->getChannelGroup(BACKGROUND_MUSIC_CHANNELGROUP));
		SFXFMODDevice::smFunc->FMOD_ChannelGroup_GetVolume(mDevice->getChannelGroup(BACKGROUND_MUSIC_CHANNELGROUP), &volume);
		SFXFMODDevice::smFunc->FMOD_ChannelGroup_SetVolume(mDevice->getChannelGroup(BACKGROUND_MUSIC_CHANNELGROUP), volume);
		SFXFMODDevice::smFunc->FMOD_ChannelGroup_SetPaused(mDevice->getChannelGroup(BACKGROUND_MUSIC_CHANNELGROUP),0);
	   break;
   case 1:
	   SFXFMODDevice::smFunc->FMOD_Channel_SetChannelGroup(mChannel, mDevice->getChannelGroup(UI_SOUND_CHANNELGROUP));
	   SFXFMODDevice::smFunc->FMOD_ChannelGroup_GetVolume(mDevice->getChannelGroup(UI_SOUND_CHANNELGROUP), &volume);
	   SFXFMODDevice::smFunc->FMOD_ChannelGroup_SetVolume(mDevice->getChannelGroup(UI_SOUND_CHANNELGROUP), volume);
	   SFXFMODDevice::smFunc->FMOD_ChannelGroup_SetPaused(mDevice->getChannelGroup(UI_SOUND_CHANNELGROUP),0);
	   break;
   case 2:
	   SFXFMODDevice::smFunc->FMOD_Channel_SetChannelGroup(mChannel, mDevice->getChannelGroup(AMBIENT_SOUND_CHANNELGROUP));
	   SFXFMODDevice::smFunc->FMOD_ChannelGroup_GetVolume(mDevice->getChannelGroup(AMBIENT_SOUND_CHANNELGROUP), &volume);
	   SFXFMODDevice::smFunc->FMOD_ChannelGroup_SetVolume(mDevice->getChannelGroup(AMBIENT_SOUND_CHANNELGROUP), volume);
	   SFXFMODDevice::smFunc->FMOD_ChannelGroup_SetPaused(mDevice->getChannelGroup(AMBIENT_SOUND_CHANNELGROUP),0);
	   break;
   case 3:
	   SFXFMODDevice::smFunc->FMOD_Channel_SetChannelGroup(mChannel, mDevice->getChannelGroup(DYNAMIC_SOUND_CHANNELGROUP));
	   SFXFMODDevice::smFunc->FMOD_ChannelGroup_GetVolume(mDevice->getChannelGroup(DYNAMIC_SOUND_CHANNELGROUP), &volume);
	   SFXFMODDevice::smFunc->FMOD_ChannelGroup_SetVolume(mDevice->getChannelGroup(DYNAMIC_SOUND_CHANNELGROUP), volume);
	   SFXFMODDevice::smFunc->FMOD_ChannelGroup_SetPaused(mDevice->getChannelGroup(DYNAMIC_SOUND_CHANNELGROUP),0);
	   break;
   }
   //有群组管理，pause不起作用
   //pause();

   return result;
   
}

void SFXFMODVoice::setPosition( U32 pos )
{
	if ( !mChannel )
		return;

	// Position is in bytes.
	//FModAssert(SFXFMODDevice::smFunc->FMOD_Channel_SetPosition(mChannel, pos, FMOD_TIMEUNIT_PCMBYTES), "SFXFMODBuffer::setPosition - Failed to set position in buffer!");
	SFXFMODDevice::smFunc->FMOD_Channel_SetPosition(mChannel, pos, FMOD_TIMEUNIT_PCMBYTES);
}

void SFXFMODVoice::setMinMaxDistance( F32 min, F32 max )
{
	if ( !mChannel || !( mBuffer->mMode & FMOD_3D ) )
		return;

   FModAssert(SFXFMODDevice::smFunc->FMOD_Channel_Set3DMinMaxDistance(mChannel, min, max), "SFXFMODBuffer::setMinMaxDistance - Failed to set min/max distance!");
	//FModAssert(SFXFMODDevice::smFunc->FMOD_Sound_Set3DMinMaxDistance(mSound, min, max), "SFXFMODBuffer::setMinMaxDistance - Failed to set min/max distance!");
}

SFXStatus SFXFMODVoice::getStatus() const
{
   // No channel... then we're stopped!
   if ( !mChannel )
      return SFXStatusStopped;

	FMOD_BOOL isTrue = false;
   SFXFMODDevice::smFunc->FMOD_Channel_GetPaused( mChannel, &isTrue );
   if ( isTrue )
      return SFXStatusPaused;

   SFXFMODDevice::smFunc->FMOD_Channel_IsPlaying( mChannel, &isTrue );
   if ( isTrue )
      return SFXStatusPlaying;

   return SFXStatusStopped;
}
void SFXFMODVoice::play( U32 looping)
{
	bool loopMode = looping != 0 ? true : false;

	if ( !mChannel )
		_assignChannel();

	// set channel props and unpause the sound
	FMOD_MODE mode = mDevice->get3dRollOffMode();
	mode |= (loopMode ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

	// <Edit> [3/5/2009 icerain] 设置loopcount需要循环每个通道组，得到该通道组中的通道，分别设置，如果不循环，设置不起作用
	int nums;
	FMOD_CHANNEL* channel = NULL;
	switch (mBuffer->getSoundChannel())
	{
		
		case 0:
			SFXFMODDevice::smFunc->FMOD_ChannelGroup_GetNumChannels(mDevice->getChannelGroup(BACKGROUND_MUSIC_CHANNELGROUP), &nums);
			for (int i=0; i<nums; ++i)
			{

				SFXFMODDevice::smFunc->FMOD_ChannelGroup_GetChannel(mDevice->getChannelGroup(BACKGROUND_MUSIC_CHANNELGROUP), i, &channel);
				if(channel == mChannel)
				{
					SFXFMODDevice::smFunc->FMOD_Channel_SetMode(mChannel, mode);
					SFXFMODDevice::smFunc->FMOD_Channel_SetLoopCount(mChannel, looping);
					SFXFMODDevice::smFunc->FMOD_Channel_SetPaused(mChannel, false);

				}
			}
			break;
		case 1:
			SFXFMODDevice::smFunc->FMOD_ChannelGroup_GetNumChannels(mDevice->getChannelGroup(UI_SOUND_CHANNELGROUP), &nums);
			for (int i=0; i<nums; ++i)
			{
				
				SFXFMODDevice::smFunc->FMOD_ChannelGroup_GetChannel(mDevice->getChannelGroup(UI_SOUND_CHANNELGROUP), i, &channel);
				if(channel == mChannel)
				{
					SFXFMODDevice::smFunc->FMOD_Channel_SetMode(mChannel, mode);
					SFXFMODDevice::smFunc->FMOD_Channel_SetLoopCount(mChannel, looping);
					SFXFMODDevice::smFunc->FMOD_Channel_SetPaused(mChannel, false);

				}
			}
			break;
		case 2:
			SFXFMODDevice::smFunc->FMOD_ChannelGroup_GetNumChannels(mDevice->getChannelGroup(AMBIENT_SOUND_CHANNELGROUP), &nums);
			for (int i=0; i<nums; ++i)
			{
				
				SFXFMODDevice::smFunc->FMOD_ChannelGroup_GetChannel(mDevice->getChannelGroup(AMBIENT_SOUND_CHANNELGROUP), i, &channel);
				if(channel == mChannel)
				{
					SFXFMODDevice::smFunc->FMOD_Channel_SetMode(mChannel, mode);
					SFXFMODDevice::smFunc->FMOD_Channel_SetLoopCount(mChannel, looping);
					SFXFMODDevice::smFunc->FMOD_Channel_SetPaused(mChannel, false);

				}
			}
			break;
		case 3:
			SFXFMODDevice::smFunc->FMOD_ChannelGroup_GetNumChannels(mDevice->getChannelGroup(DYNAMIC_SOUND_CHANNELGROUP), &nums);
			for (int i=0; i<nums; ++i)
			{
				
				SFXFMODDevice::smFunc->FMOD_ChannelGroup_GetChannel(mDevice->getChannelGroup(DYNAMIC_SOUND_CHANNELGROUP), i, &channel);
				if(channel == mChannel)
				{
					SFXFMODDevice::smFunc->FMOD_Channel_SetMode(mChannel, mode);
					SFXFMODDevice::smFunc->FMOD_Channel_SetLoopCount(mChannel, looping);
					SFXFMODDevice::smFunc->FMOD_Channel_SetPaused(mChannel, false);

				}
			break;

	  }
	
	
	}
	//FModAssert(SFXFMODDevice::smFunc->FMOD_Channel_SetMode(mChannel, mode), "SFXFMODBuffer::play - failed to set mode on sound.");
	//FModAssert(SFXFMODDevice::smFunc->FMOD_Channel_SetLoopCount(mChannel, looping), "SFXFMODBuffer::play - Failed to set looping!");

	/*FModAssert(
		(SFXFMODDevice::smFunc->FMOD_Channel_SetPaused(mChannel, false)), 
		"SFXFMODBuffer::play - Failed to unpause sound!");*/
}



void SFXFMODVoice::stop()
{
	if ( !mChannel )
		return;

	// Failing on this is OK as it can happen if the sound has already 
	// stopped and in other non-critical cases. Note the commented version
	// with assert, in case you want to re-enable it:
	//FModAssert(SFXFMODDevice::smFunc->FMOD_Channel_Stop(mChannel), "SFXFMODBuffer::stop - failed to stop channel!");
	SFXFMODDevice::smFunc->FMOD_Channel_Stop(mChannel);
	mChannel = NULL;
}

void SFXFMODVoice::pause()
{
	if ( !mChannel )
		return;

	SFXFMODDevice::smFunc->FMOD_Channel_SetPaused( mChannel, true );
}

void SFXFMODVoice::setVelocity( const VectorF& velocity )
{
	if ( !mChannel || !( mBuffer->mMode & FMOD_3D ) )
		return;

	// Note we have to do a handedness swap; see the 
   // listener update code in SFXFMODDevice for details.
	FMOD_VECTOR vel;
	vel.x = velocity.x;
	vel.y = velocity.z;
	vel.z = velocity.y;

	SFXFMODDevice::smFunc->FMOD_Channel_Set3DAttributes( mChannel, NULL, &vel );
}

void SFXFMODVoice::setTransform( const MatrixF& transform )
{
	if ( !mChannel || !( mBuffer->mMode & FMOD_3D ) )
		return;

   FMOD_VECTOR pos;
   FMOD_VECTOR dir;
   transform.getColumn( 3, (Point3F*)&pos );
   transform.getColumn( 1, (Point3F*)&dir );

   // Note we have to do a handedness swap; see the 
   // listener update code in SFXFMODDevice for details.
   _swap( pos.y, pos.z );
   _swap( dir.y, dir.z );

	// This can fail safe, so don't assert if it fails.
	SFXFMODDevice::smFunc->FMOD_Channel_Set3DAttributes( mChannel, &pos, NULL );
   SFXFMODDevice::smFunc->FMOD_Channel_Set3DConeOrientation( mChannel, &dir );
}

void SFXFMODVoice::setVolume( F32 volume )
{
	if ( !mChannel )
		return;

	// This can fail safe, so don't assert if it fails.
	//FModAssert(SFXFMODDevice::smFunc->FMOD_Channel_SetVolume(mChannel, volume), "SFXFMODBuffer::setVolume - failed to set channel volume!");
	SFXFMODDevice::smFunc->FMOD_Channel_SetVolume(mChannel, volume);
}

void SFXFMODVoice::setPitch( F32 pitch )
{
	if ( !mChannel )
		return;

   // if we do not know the frequency, we cannot change the pitch
   F32 frequency = mBuffer->mResource->getFrequency();
   if ( frequency == 0 )
      return;

	// Scale the original frequency by the pitch factor.
	//FModAssert(SFXFMODDevice::smFunc->FMOD_Channel_SetFrequency(mChannel, mFrequency * pitch), "SFXFMODBuffer::setPitch - failed to set channel frequency!");
	SFXFMODDevice::smFunc->FMOD_Channel_SetFrequency(mChannel, frequency * pitch);
}

void SFXFMODVoice::setCone( F32 innerAngle, F32 outerAngle, F32 outerVolume )
{
	if ( !mChannel )
		return;

   SFXFMODDevice::smFunc->FMOD_Channel_Set3DConeSettings( 
      mChannel, 
      mRadToDeg( innerAngle ), 
      mRadToDeg( outerAngle ), 
      outerVolume );
}



