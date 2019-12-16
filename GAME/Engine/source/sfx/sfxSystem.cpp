//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "sfx/sfxSystem.h"

#include "sfx/sfxProvider.h"
#include "sfx/sfxDevice.h"
#include "console/console.h"
#include "sim/processList.h"
#include "platform/profiler.h"
#include "sfx/sfxWavResource.h"
#include "sfx/sfxMP3Resource.h"
#include "Gameplay/Data/SoundResource.h"

#include "Gameplay/ClientGameplayState.h"

//#ifndef POWER_NO_OGGVORBIS
//   #include "sfx/vorbis/sfxOggResource.h"
//#endif

SFXSystem* SFXSystem::smSingleton = NULL;

void SFXSystem::init()
{
   AssertWarn( smSingleton == NULL, "SFX has already been initialized!" );

   // We register the common resource types 
   // here.  Provider specific resource types
   // should be registered in their constructors.
//   ResourceManager->registerExtension( ".wav", SFXWavResource::create );
//  // ResourceManager->registerExtension( ".mp3", SFXMP3Resource::create );
//   ResourceManager->registerExtension(".mp3",SFXMP3Resource::create);
//#ifndef POWER_NO_OGGVORBIS
//   ResourceManager->registerExtension( ".ogg", SFXOggResource::create );
//#endif

   // Create the system.
   smSingleton = new SFXSystem();
}


void SFXSystem::destroy()
{
   AssertWarn( smSingleton != NULL, "SFX has not been initialized!" );

  //清除所有资源
   if( SFX->mPlayOnceSources.size() > 0)
   {
	   SFXSourceVector::iterator iter = SFX->mPlayOnceSources.begin();
	   if(iter != SFX->mPlayOnceSources.end())
		   SFX->mPlayOnceSources.erase(iter);
	   SFX->mPlayOnceSources.clear();
   }
   if(SFX->mSources.size() > 0)
   {
	   SFXSourceVector::iterator iter = SFX->mSources.begin();
	   if(iter != SFX->mSources.end())
		   SFX->mSources.erase(iter);
	   SFX->mSources.clear();
   }
   // TODO: How do we unregister extensions?
   delete smSingleton;
   smSingleton = NULL;
	
}


SFXSystem::SFXSystem()
   :  mDevice( NULL ),
      mLastTime( 0 ),
      mMasterVolume( 1 ),
      mStatNumSources( 0 ),
      mStatNumPlaying( 0 ),
      mStatNumCulled( 0 ),
      mStatNumVoices( 0 )
{
   mStartTime = 0;
   mSoundID1 = NULL;
   mSoundID2 = NULL;
   // Setup the default channel volumes.
   for ( S32 i=0; i < NumChannels; i++ )
      mChannelVolume[i] = 1.0f;
	
   Con::addVariable( "SFX::numSources", TypeS32, &mStatNumSources );
   Con::addVariable( "SFX::numPlaying", TypeS32, &mStatNumPlaying );
   Con::addVariable( "SFX::numCulled", TypeS32, &mStatNumCulled );
   Con::addVariable( "SFX::numVoices", TypeS32, &mStatNumVoices );
}

SFXSystem::~SFXSystem()
{
   Con::removeVariable( "SFX::numSources" );
   Con::removeVariable( "SFX::numPlaying" );
   Con::removeVariable( "SFX::numCulled" );
   Con::removeVariable( "SFX::numBuffers" );
	
 
   // Cleanup any remaining sources!
   AssertFatal(mSources.size()==0,"sfxSoundSource should be deleted in sfxEmitter");
   AssertFatal(mPlayOnceSources.size()==0,"sfxSoundSource should be deleted in sfxEmitter");
   mChannelMap.clear();
   // If we still have a device... delete it.
   deleteDevice();
}

bool SFXSystem::createDevice( const char* providerName, const char* deviceName, bool useHardware, S32 maxBuffers, bool changeDevice )
{
   // Make sure we don't have a device already.
   if ( mDevice && !changeDevice )
      return false;

   // Lookup the provider.
   SFXProvider* provider = SFXProvider::findProvider( providerName );
   if ( !provider )
      return false;

   // If we have already created this device and are using it then no need to do anything
   if (mDevice &&
       dStricmp(providerName, mDevice->getProvider()->getName()) == 0 &&
       dStricmp(deviceName, mDevice->getName()) == 0 &&
       useHardware == mDevice->getUseHardware())
      return true;

   // If we have an existing device remove it
   if (mDevice)
      deleteDevice();

   // Ok... create the new device.
   mDevice = provider->createDevice( deviceName, useHardware, maxBuffers );
   if ( !mDevice )
      return false;

   SFXDevice::getEventSignal().trigger( mDevice, SFXDeviceEvent_Create );

   return true;
}

const char* SFXSystem::getDeviceInfoString()
{
   // Make sure we have a valid device.
   if ( !mDevice )
      return NULL;

   char* info = Con::getReturnBuffer( 1024 );
   dSprintf( info, 1024,"%s\t%s\t%s\t%d",
      mDevice->getProvider()->getName(),
      mDevice->getName(),
      mDevice->getUseHardware() ? "1" : "0",
      mDevice->getMaxBuffers() );

   return info;
}

void SFXSystem::deleteDevice()
{
   // Make sure we have a valid device.
   if ( !mDevice )
      return;

   // Signal everyone who cares that the
   // device is being deleted.
   SFXDevice::getEventSignal().trigger( mDevice, SFXDeviceEvent_Destroy );

   // Free the device which should delete all
   // the active voices and buffers.
   delete mDevice;
   mDevice = NULL;
}

SFXSource* SFXSystem::createSource( const SFXProfile* profile,
								   const MatrixF* transform, 
								   const VectorF* velocity )
{
	// We sometimes get null profiles... nothing to play without a profile!
	if ( !profile )
		return NULL;

	// Create the source.
	SFXSource *source = SFXSource::_create( profile );
	if ( !source )
	{
		//Con::errorf( 
		//	"SFXSystem::createSource() - Creation failed!\n"
		//	"  Profile: %s\n"
		//	"  Filename: %s",
		//	profile->getName(),
		//	profile->getFilename() );

		return NULL;
	}
	
	mSources.push_back( source );
	
	

	if ( transform )
		source->setTransform( *transform );

	if ( velocity )
		source->setVelocity( *velocity );

	//const U32 channel = source->getChannel();
	//const F32 volume = getChannelVolume( channel ) * mMasterVolume;
	//source->_setModulativeVolume( volume );

	// Update the stats.
	mStatNumSources = mSources.size() ;

	return source;
}

void SFXSystem::_onRemoveSource( SFXSource* source )
{
	SFXSourceVector::iterator iter = find(mSources.begin(), mSources.end(),source);
   AssertFatal( iter != mSources.end(), "Got unknown source!" );
   mSources.erase( iter );

   // Check if it was a play once source...
   iter = find(mPlayOnceSources.begin(), mPlayOnceSources.end(), source );
   if ( iter != mPlayOnceSources.end() )
      mPlayOnceSources.erase( iter );

   // Free the hardware buffer.
   source->_freeVoice( mDevice );

   // Update the stats.
   mStatNumSources = mSources.size();
}

SFXBuffer* SFXSystem::_createBuffer( SFXProfile* profile )
{
   // The buffers are created by the active
   // device... without one we cannot do anything.
   if ( !mDevice )
      return NULL;

   return mDevice->createBuffer( profile );
}

SFXSource* SFXSystem::playOnce(  const StringTableEntry soundId, 
                                 const MatrixF *transform,
                                 const VectorF *velocity )
{
   // We sometimes get null profiles... nothing to play without a profile!
	SFXProfile* profile = g_SoundManager->FindProfile(soundId);
   if ( !profile )
      return NULL;

   SFXSource *source = createSource( profile, transform, velocity );
   if ( source )
   {
      mPlayOnceSources.push_back( source ); 
	  
      source->play();
   }
	
   return source;
}

void SFXSystem::stopAll( S32 channel )
{
   AssertFatal( channel < 0 || channel < NumChannels, "Got an invalid channel!" );

   // Go thru the sources and stop them.
   SFXSourceVector::iterator iter = mSources.begin();
   for ( ; iter != mSources.end(); iter++ )
   {
      SFXSource* source = *iter;
      if ( (channel < 0 || source->getChannel() == channel) && (dStrcmp(source->getDesc()->mSoundID, "B_001") != 0))
         source->stop();
   }
}

void SFXSystem::setMasterVolume( F32 volume )
{
   mMasterVolume = mClampF( volume, 0, 1 );

   // Go thru the sources and update the modulative volume.
   SFXSourceVector::iterator iter = mSources.begin();
   for ( ; iter != mSources.end(); iter++ )
   {
      SFXSource* source = *iter;
      U32 channel = source->getChannel();
      F32 volume = getChannelVolume( channel ) * mMasterVolume;
      source->_setModulativeVolume( volume );
   }
}

F32 SFXSystem::getChannelVolume( U32 channel ) const
{
   AssertFatal( channel < NumChannels, "Got an invalid channel!" );
   return mChannelVolume[ channel ];
}

void SFXSystem::setChannelVolume( U32 channel, F32 volume )
{
   AssertFatal( channel < NumChannels, "Got an invalid channel!" );

   volume = mClampF( volume, 0, 1 );
   mChannelVolume[ channel ] = volume;

   // Scale it by the master volume.
   volume *= mMasterVolume;

   // Go thru the sources and update the modulative volume.
   SFXSourceVector::iterator iter = mSources.begin();
   for ( ; iter != mSources.end(); iter++ )
   {
      SFXSource* source = *iter;
      if ( source->getChannel() == channel )
         source->_setModulativeVolume( volume );
   }
}

void SFXSystem::_update()
{
   PROFILE_SCOPE( SFXSystem_Update );
   
   // Every four system ticks.
   const U32 SOURCE_UPDATE_MS = TickMs * 4;

   // The update of the sources can be a bit expensive
   // and it does not need to be updated every frame.
   const U32 time = Platform::getVirtualMilliseconds();
   const U32 elapsed = time - mLastTime;
   if ( elapsed >= SOURCE_UPDATE_MS )
   {
      _updateSources();
      mLastTime = time;
   }

   // If we have a device then update it.
   if ( mDevice )
      mDevice->update( mListener );


   // Update some stats.
   mStatNumSources = mSources.size();
}

void SFXSystem::_updateSources()
{
   PROFILE_SCOPE( SFXSystem_UpdateSources );

   // Check the status of the sources here once.
   mStatNumPlaying = 0;
   SFXSourceVector::iterator iter = mSources.begin();
   for ( ; iter != mSources.end(); ++iter )
   {
	   
      if ( (*iter)->getStatus() == SFXStatusPlaying )
         ++mStatNumPlaying;
   }

   // First check to see if any play once sources have
   // finished playback... delete them.
   iter = mPlayOnceSources.begin();
   for ( ; iter != mPlayOnceSources.end();  )
   {
      SFXSource* source = *iter;

      if ( source->getLastStatus() == SFXStatusStopped )
      {
         int index = iter - mPlayOnceSources.begin();

         // Erase it from the vector first, so that onRemoveSource
         // doesn't do it during cleanup and screw up our loop here!
         mPlayOnceSources.erase( iter );
         source->deleteObject();

         iter = mPlayOnceSources.begin() + index;
         continue;
      }

      iter++;
   }

   // Reassign buffers to the sources.
   _assignVoices();
}

void SFXSystem::_assignVoices()
{
   PROFILE_SCOPE( SFXSystem_AssignVoices );

   mStatNumVoices = 0;
   mStatNumCulled = 0;

   if ( !mDevice )
      return;

   // Now let the listener prioritize the sounds for us 
   // before we go off and assign buffers.
   mListener.sortSources( mSources );

   // We now make sure that the sources closest to the 
   // listener, the ones at the top of the source list,
   // have a device buffer to play thru.
   mStatNumCulled = 0;
   SFXSourceVector::iterator iter = mSources.begin(); 
   for ( ; iter != mSources.end(); ++iter )
   {
      SFXSource* source = *iter;

      // Non playing sources (paused or stopped) are at the
      // end of the vector, so when i encounter one i know 
      // that nothing else in the vector needs buffer assignment.
		if ( !source->isPlaying() )
         break;

      // If the source is outside it's max range we can
      // skip it as well, so that we don't waste cycles
      // setting up a buffer for something we won't hear.
      /*if ( source->getAttenuatedVolume() <= 0.0f )
      {
         mStatNumCulled++;
         continue;
      }*/

      // If the source has a voice then we can skip it.
      if ( source->hasVoice() )
         continue;

      // Ok let the device try to assign a new voice for 
      // this source... this may fail if we're out of voices.
      if ( source->_allocVoice( mDevice ) )
         continue;

      // The device couldn't assign a new voice, so we look for
      // the last source in the list with a voice and free it.
      SFXSourceVector::iterator hijack = mSources.end() - 1;
      for ( ; hijack != iter; hijack-- )
      {
         if ( (*hijack)->hasVoice() )
         {
            (*hijack)->_freeVoice( mDevice );
            break;
         }
		}

      // Ok try to assign a voice once again!
      if ( source->_allocVoice( mDevice ) )
         continue;

      // If the source still doesn't have a buffer... well
      // tough cookies.  It just cannot be heard yet, maybe
      // it can in the next update.
      mStatNumCulled++;
	}

   // Update the buffer count stat.
   mStatNumVoices = mDevice->getVoiceCount();
}

// <Edit> [3/4/2009 icerain] channelGroup音量控制
FMOD_RESULT SFXSystem::setChannelGroupVolumeByIndex(CHANNEL_GROUP_INDEX index, F32 volume)
{
	FMOD_RESULT result;
	SFXFMODDevice* device = dynamic_cast<SFXFMODDevice*>(mDevice);		
	if(!device)
		return FMOD_ERR_CDDA_NODEVICES;
	result = SFXFMODDevice::smFunc->FMOD_ChannelGroup_SetVolume(device->getChannelGroup(index), volume);
	return result;
}
// <Edit> [3/4/2009 icerain] channelGroup是否暂停
FMOD_RESULT SFXSystem::setChannelGroupPaused(CHANNEL_GROUP_INDEX index, FMOD_BOOL pause)
{
	SFXFMODDevice* dev = dynamic_cast<SFXFMODDevice*>(mDevice);
	if(!dev)
		return FMOD_ERR_CDDA_NODEVICES;
	return (SFXFMODDevice::smFunc->FMOD_ChannelGroup_SetPaused(dev->getChannelGroup(index), pause));
}


FMOD_CHANNEL* SFXSystem::getChannelBySoundID(StringTableEntry id)
{
	//ChannelMap::iterator it1 = mChannelMap.begin();
	/*for(; it1 != mChannelMap.end(); ++it1)
	{
		StringTableEntry text = it1->first;
		FMOD_CHANNEL* channle = it1->second;
	}*/
	ChannelMap::iterator it = mChannelMap.find(id);
		if(it != mChannelMap.end())
			return it->second;
		
		return NULL;
}

void SFXSystem::clearChannelMap()
{
	ChannelMap::iterator it = mChannelMap.begin();
	for(; it != mChannelMap.end(); ++it)
	{
		if(it->second != mGlobal)
			mChannelMap.erase(it);
	}
	//mChannelMap.clear();
}
// <Edit> [3/19/2009 iceRain] 两种背景音乐更替时的渐变效果
static  U32 counter = 0;
void SFXSystem::controlSoundVolume(U32 time)
{
	U32 usedTime = time - mStartTime;
	if(usedTime>100)
	{
		counter++;
		mStartTime = Sim::getCurrentTime();
	}
	else
	{
		return;
	}
	
	F32 volume;
	SFXFMODDevice* device = dynamic_cast<SFXFMODDevice*>(mDevice);
	if(!device)
		return;
	FMOD_CHANNELGROUP* group = device->getChannelGroup(BACKGROUND_MUSIC_CHANNELGROUP);
	if(group)
	{
		SFXFMODDevice::smFunc->FMOD_ChannelGroup_GetVolume(group, &volume);
	}
	else
		return;
	if(counter<20)
	    volume -= 0.05f;
	else if(counter == 20)
		
	{
		SFXFMODDevice::smFunc->FMOD_Channel_SetPaused(getChannelBySoundID(mSoundID1), 1);
		SFXFMODDevice::smFunc->FMOD_Channel_SetPaused(getChannelBySoundID(mSoundID2), 0);
	}
	else if(counter<40)
		volume  += 0.05f;
	else
	{
		volume = 1.0f;
		mStartTime = 0;
		counter = 0;
	}
	

	SFXFMODDevice::smFunc->FMOD_ChannelGroup_SetVolume(group, volume);
}

ConsoleFunctionGroupBegin( SFX, 
   "Functions dealing with the SFX audio layer." );

ConsoleFunction( sfxGetAvailableDevices, const char*, 1, 1, 
   "Returns a list of available devices in the form:\n\n"
   "provider1 [tab] device1 [tab] hasHardware1 [tab] maxBuffers1 [nl] provider2 ... etc." )
{
   char* deviceList = Con::getReturnBuffer( 2048 );
   deviceList[0] = 0;

   SFXProvider* provider = SFXProvider::getFirstProvider();
   while ( provider )
   {
      // List the devices in this provider.
      const SFXDeviceInfoVector& deviceInfo = provider->getDeviceInfo();
      for ( S32 d=0; d < deviceInfo.size(); d++ )
      {
         const SFXDeviceInfo* info = deviceInfo[d];
         dStrcat( deviceList, 2048, provider->getName() );
         dStrcat( deviceList, 2048, "\t" );
         dStrcat( deviceList, 2048, info->name );
         dStrcat( deviceList, 2048, "\t" );
         dStrcat( deviceList, 2048, info->hasHardware ? "1" : "0" );
         dStrcat( deviceList, 2048, "\t" );
         dStrcat( deviceList, 2048, Con::getIntArg( info->maxBuffers ) );         
         dStrcat( deviceList, 2048, "\n" );
      }

      provider = provider->getNextProvider();
   }

   return deviceList;
}

ConsoleFunction( sfxCreateDevice, bool, 5, 5,  
                  "sfxCreateDevice( string provider, string device, bool useHardware, S32 maxBuffers )\n"
                  "Initializes the requested device.  This must be called successfully before any sounds will be heard.\n"
                  "@param provider The provider name.\n"
                  "@param device The device name.\n"
                  "@param useHardware A boolean which toggles the use of hardware processing when available.\n"
                  "@param maxBuffers The maximum buffers for this device to use or -1 for the device to pick its own reasonable default.")
{
   return SFX->createDevice( argv[1], argv[2], dAtob( argv[3] ), dAtoi( argv[4] ), true );
}

ConsoleFunction( sfxDeleteDevice, void, 1, 1, 
   "Destroys the currently initialized device.  Sounds will still play, but not be heard.")
{
   SFX->deleteDevice();
}

ConsoleFunction( sfxGetDeviceInfo, const char*, 1, 1,  
      "Returns a newline delimited string containing information on the current device." )
{
   return SFX->getDeviceInfoString();
}

ConsoleFunction( sfxPlay, S32, 2, 5, "sfxPlay( source )\n"
				"sfxPlay( soundID, <x, y, z> )\n" )
{
	if ( argc == 2 )
	{
		SFXSource* source = dynamic_cast<SFXSource*>( Sim::findObject( argv[1] ) );
		if ( source )
		{
			source->play();
			return source->getId();
		}
	}

	SFXProfile *profile = g_SoundManager->FindProfile(argv[1]);
	if ( !profile )
	{
		Con::printf( "Unable to locate sfx profile '%s'", argv[1] );
		return 0;
	}

	Point3F pos(0.f, 0.f, 0.f);
	if ( argc == 3 )
		dSscanf( argv[2], "%g %g %g", &pos.x, &pos.y, &pos.z );
	else if(argc == 5)
		pos.set( dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]) );

	MatrixF transform;
	transform.set( EulerF(0,0,0), pos );

	SFXSource* source = SFX->playOnce( argv[1], &transform );
	if ( source )
		return source->getId();

	return 0;
}


ConsoleFunction( sfxCreateSource, S32, 2, 6,
				"sfxCreateSource(soundID)\n"
				"sfxCreateSource(soundID, x,y,z)\n"
				"sfxCreateSource(soundID, filename)\n"
				"sfxCreateSource(soundID, filename, x,y,z)\n"
				"\n"
				"Creates a new paused sound source using a profile or a description "
				"and filename.  The return value is the source which must be "
				"released by delete()." )
{
	SFXDescription* description = NULL;
	SFXProfile* profile = g_SoundManager->FindProfile(argv[1]);
	if ( !profile )
	{
		description = g_SoundManager->getDescriptionByID( argv[1]  );
		if ( !description )
		{
			Con::printf( "Unable to locate sound profile/description '%s'", argv[1] );
			return 0;
		}
	}

	SFXSource* source = NULL;

	if ( profile )
	{
		if ( argc == 2 )
		{
			source = SFX->createSource( profile );
		}
		else
		{
			MatrixF transform;
			transform.set( EulerF(0,0,0), Point3F( dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4])) );
			source = SFX->createSource( profile, &transform );
		}
	}
	else if ( description )
	{
		SFXProfile* tempProfile = new SFXProfile( description, StringTable->insert( argv[2] ), true );
		if( !tempProfile->registerObject() )
		{
			Con::errorf( "sfxCreateSource - unable to create profile" );
			delete tempProfile;
		}
		else
		{
			if ( argc == 3 )
			{
				source = SFX->createSource( tempProfile );
			}
			else
			{
				MatrixF transform;
				transform.set(EulerF(0,0,0), Point3F( dAtof(argv[3]),dAtof(argv[4]),dAtof(argv[5]) ));
				source = SFX->createSource( tempProfile, &transform );
			}

			tempProfile->setAutoDelete( true );
		}
	}

	if ( source )
		return source->getId();

	return 0;
}

ConsoleFunction( sfxPlayOnce, S32, 2, 6,
				"sfxPlayOnce(profile)\n"
				"sfxPlayOnce(profile, x,y,z)\n"
				"sfxPlayOnce(description, filename)\n"
				"sfxPlayOnce(description, filename, x,y,z)\n"
				"\n"
				"Creates a new sound source using a profile or a description "
				"and filename and plays it once.  Once playback is finished the "
				"source is deleted.  The return value is the temporary source id." )
{
	/*SFXDescription* description = NULL;
	SFXProfile* profile = dynamic_cast<SFXProfile*>( Sim::findObject( argv[1] ) );
	if ( !profile )
	{
		description = dynamic_cast<SFXDescription*>( Sim::findObject( argv[1] ) );
		if ( !description )
		{
			Con::errorf( "sfxPlayOnce - Unable to locate sound profile/description '%s'", argv[1] );
			return 0;
		}
	}

	SFXSource* source = NULL;

	if ( profile )
	{
		if ( argc == 2 )
			source = SFX->playOnce( profile );
		else
		{
			MatrixF transform;
			transform.set(EulerF(0,0,0), Point3F( dAtof(argv[2]),dAtof(argv[3]),dAtof(argv[4]) ));
			source = SFX->playOnce( profile, &transform );
		}
	}

	else if ( description )
	{
		SFXProfile* tempProfile = new SFXProfile( description, StringTable->insert( argv[2] ), true );
		if( !tempProfile->registerObject() )
		{
			Con::errorf( "sfxPlayOnce - unable to create profile" );
			delete tempProfile;
		}
		else
		{
			if ( argc == 3 )
				source = SFX->playOnce( tempProfile );
			else
			{
				MatrixF transform;
				transform.set(EulerF(0,0,0), Point3F( dAtof(argv[3]),dAtof(argv[4]),dAtof(argv[5]) ));
				source = SFX->playOnce( tempProfile, &transform );
			}

			tempProfile->setAutoDelete( true );
		}
	}

	if ( source )
		return source->getId();*/
	SFXSource* source = NULL;
	if(argc == 2)
		source = SFX->playOnce(argv[1]);
	else if(argc == 3)
	{
		MatrixF transform;
		transform.set(EulerF(0,0,0), Point3F( dAtof(argv[3]),dAtof(argv[4]),dAtof(argv[5]) ));
		source = SFX->playOnce(argv[1], &transform);
	}
	if(source)
		return source->getId();

	return 0;
}

ConsoleFunction(sfxStop, void, 2, 2, "(S32 id): stop a source, equivalent to id.stop()")
{
	S32 id = dAtoi(argv[1]);
	SFXSource * obj;
	if (Sim::findObject<SFXSource>(id, obj))
		obj->stop();
}


ConsoleFunction(sfxStopAll, void, 1, 2, "(S32 channel = -1)\n\n"
				"@param channel The optional channel index of which sources to stop.\n"
				"@return The volume of the channel.")
{
	U32 channel = -1;

	if ( argc > 1 )
	{
		channel = dAtoi( argv[1] );

		if ( channel >= SFXSystem::NumChannels )
		{
			Con::errorf( ConsoleLogEntry::General, "sfxStopAll: invalid channel '%d'", dAtoi( argv[1] ) );
			return;
		}
	}
	SFX->stopAll( channel );
}

ConsoleFunction(sfxGetChannelVolume, F32, 2, 2, "(S32 channel)\n\n"
				"@param channel The channel index to fetch the volume from.\n"
				"@return The volume of the channel.")
{
	U32 channel = dAtoi( argv[1] );

	if ( channel >= SFXSystem::NumChannels )
	{
		Con::errorf( ConsoleLogEntry::General, "sfxGetChannelVolume: invalid channel '%d'", dAtoi( argv[1] ) );
		return 0.0f;
	}

	return SFX->getChannelVolume( channel );
}

ConsoleFunction(sfxSetChannelVolume, bool, 3, 3, "(S32 channel, F32 volume)\n\n"
				"@param channel The channel index to set volume on.\n"
				"@param volume New 0 to 1 channel volume."
				)
{
	U32 channel = dAtoi( argv[1] );

	F32 volume = mClampF( dAtof( argv[2] ), 0, 1 );

	if ( channel >= SFXSystem::NumChannels )
	{
		Con::errorf( ConsoleLogEntry::General, "sfxSetChannelVolume: invalid channel '%d'", dAtoi( argv[1] ) );
		return false;
	}

	SFX->setChannelVolume( channel, volume );
	return true;
}

ConsoleFunction(sfxGetMasterVolume, F32, 1, 1, "()\n\n"
				"@return The sound system master volume." )
{
	return SFX->getMasterVolume();
}

ConsoleFunction(sfxSetMasterVolume, void, 2, 2, "(F32 volume)\n\n"
				"@param volume The new 0 to 1 sound system master volume." )
{
	F32 volume = mClampF( dAtof( argv[1] ), 0, 1 );
	SFX->setMasterVolume( volume );
}
// <Edit> [3/4/2009 icerain] channelGroup音量控制脚本函数
ConsoleFunction(sfxSetBackChannelGroupVolume, void, 3, 3, "(S32 inde, F32 volume)\n\n" )
{
	S32 index = dAtoi(argv[1]);
	F32 volume = mClampF( dAtof( argv[2] ), 0, 1 );
	switch (index)
	{
	case 0:
		SFX->setChannelGroupVolumeByIndex(BACKGROUND_MUSIC_CHANNELGROUP, volume);
		break;
	case 1:
		SFX->setChannelGroupVolumeByIndex(UI_SOUND_CHANNELGROUP, volume);
		break;
	case 2:
		SFX->setChannelGroupVolumeByIndex(AMBIENT_SOUND_CHANNELGROUP, volume);
		break;
	case 3:
		SFX->setChannelGroupVolumeByIndex(DYNAMIC_SOUND_CHANNELGROUP, volume);
		break;
	default:
		return;
	}
	
}
// <Edit> [3/4/2009 icerain] channelGroup是否暂停脚本函数
ConsoleFunction(sfxSetChanbelGroupPause, void, 3, 3,"(S32 index, bool pause)\n\n" )
{
	S32 index = dAtoi(argv[1]);
	bool pause = dAtob(argv[2]);
	switch (index)
	{
	case 0:
		SFX->setChannelGroupPaused(BACKGROUND_MUSIC_CHANNELGROUP, pause);
		break;
	case 1:
		SFX->setChannelGroupPaused(UI_SOUND_CHANNELGROUP, pause);
		break;
	case 2:
		SFX->setChannelGroupPaused(AMBIENT_SOUND_CHANNELGROUP, pause);
		break;
	case 3:
		SFX->setChannelGroupPaused(DYNAMIC_SOUND_CHANNELGROUP, pause);
		break;
	default:
		return;
	}
	
}
// <Edit> [3/6/2009 icerain] 停止播放
ConsoleFunction(ChannelStop, void, 2, 2, "(StringTableEntry id)")
{
	SFXFMODDevice* device = dynamic_cast<SFXFMODDevice*>(SFX->getDevice());
	if(!device)
		return;
	FMOD_CHANNEL* channel = SFX->getChannelBySoundID(argv[1]); 
	SFXFMODDevice::smFunc->FMOD_Channel_Stop(channel);
	channel	= NULL;
	/*SFXSourceVector::iterator it = SFX->getVector().begin();
	for(; it != SFX->getVector().end(); ++it)
	{
		SFXSource* source = *it;
		if (source && (source->getDesc()->mSoundID == argv[1]))
		{
			SFX->getVector().erase(it);
		}
	}*/
	
}
// <Edit> [3/6/2009 icerain] 暂停
ConsoleFunction(ChannelPauseByID, void, 3, 3, "(StringTableEntry id, Bool paused)")
{
	SFXFMODDevice* device = dynamic_cast<SFXFMODDevice*>(SFX->getDevice());
	if(!device)
		return;
	SFXFMODDevice::smFunc->FMOD_Channel_SetPaused(SFX->getChannelBySoundID(argv[1]), dAtob(argv[2]));

}
ConsoleFunction(PauseSound, void, 2, 2, "")
{
	SFXFMODDevice* device = dynamic_cast<SFXFMODDevice*>(SFX->getDevice());
	if(!device)
		return;
	SFXFMODDevice::smFunc->FMOD_Channel_SetPaused(SFX->mGlobal, dAtob(argv[1]));
}
ConsoleFunction(SoundFadeInByID, void, 3, 3, "(soundID1, soundID2)")
{
	SFX->mStartTime = Sim::getCurrentTime();
	SFX->mSoundID1 = argv[1];
	SFX->mSoundID2 = argv[2];
	
}

ConsoleFunction(PlaySound, void, 2, 2, "PlaySound(StringTableEntry id)")
{
	SFXProfile* file = g_SoundManager->FindProfile(argv[1]);
	if(file)
	{
		SFXSource* source = SFX->createSource(file);
		if(source)
		{
			source->play();
#ifdef NTJ_CLIENT
		g_ClientGameplayState->setGlobalSound(source);
#endif
			
		}
			
	}
}

ConsoleFunction(PlayOnceSound, void, 2, 2, "PlayOnceSound(StringTableEntry id)")
{
	SFXSource* source = SFX->playOnce(argv[1]);
}



//ConsoleFunction(StopSound, void, 2, 2, "(StringTableEntry id)")
//{
//	SFXProfile* file = g_SoundManager->FindProfile(argv[1]);
//	SFXFMODDevice* device = dynamic_cast<SFXFMODDevice*>(SFX->getDevice());
//	SFXDescription* desc = file->getDescription();
//	if(file)
//	{
//		SFXSource* source = new SFXSource(file);
//		SFXBuffer* buffer = file->getBuffer();
//		if(device)
//		{
//			SFXVoice* voice = device->createVoice(false, buffer, desc->mPriority);
//			if(voice)
//				voice->stop();
//		}
//	}
//}

//ConsoleFunction(SetPosition, void, 2, 2, "(StringTableEntry id)")
//{
//	SFXFMODDevice* device = dynamic_cast<SFXFMODDevice*>(SFX->getDevice());
//	SFXFMODDevice::smFunc->FMOD_Channel_SetPosition(SFX->getChannelBySoundID(argv[1]), 0, FMOD_TIMEUNIT_MODROW);
//}
ConsoleFunctionGroupEnd( SFX );


