//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SFXSYSTEM_H_
#define _SFXSYSTEM_H_


#ifndef _SFXSOURCE_H_
   #include "sfx/sfxSource.h"
#endif
#ifndef _SFXPROFILE_H_
   #include "sfx/sfxProfile.h"
#endif
#ifndef _SFXDEVICE_H_
   #include "sfx/sfxDevice.h"
#endif
#ifndef _SFXLISTENER_H_
   #include "sfx/sfxListener.h"
#endif
#ifndef _SIGNAL_H_
   #include "util/tSignal.h"
#endif

#ifndef _SFXFMODDEVICE_H_
   #include "sfx/fmod/sfxFMODDevice.h"
#endif
#include "Gameplay/Data/SoundResource.h"

#include "sfx/fmod/sfxFMODVoice.h"
#include <string>
// ============================================================================
// 音效系统管理类
//  $pref::SFX::frequency 提供设定声音采样频率，一般是44100
//  $pref::SFX::bitrate 提供设定声音数据位长度，一般是16或32 
class SFXSystem
{
   friend class SFXSource;		// for _onRemoveSource.
   friend class SFXProfile;     // for _createBuffer.
  
   public:   
      enum
      {
         NumChannels = 32,			//有效的声音通道最大个数
         NumChannelBits = 6,		//用于pack/unpack声音通道最大数的bit位
      };

   protected:
      static SFXSystem* smSingleton;
	  SFXSystem();
      ~SFXSystem();

      SFXDevice* mDevice;
      SFXSourceVector mSources;					//所有音源数据
      SFXSourceVector mPlayOnceSources;			//仅一次播放后释放的音源数据
      SFXListener mListener;					//接听者的位置和方向

      U32 mLastTime;							//最近一次更新时间(update())

      F32 mChannelVolume[NumChannels];			//所有声道的音量控制
      F32 mMasterVolume;						//主声道的音量控制

      //为跟踪性能汇报一些状态数据到控制台
      S32 mStatNumSources;
      S32 mStatNumPlaying;
      S32 mStatNumCulled;
      S32 mStatNumVoices;
	  
      //对于音源的状态，音量控制变化及接听者位置方向变化而需要更新
	  //播放优先次序或再分配声音缓冲内存，看SFXSource::_update()方法
      void _updateSources();
      void _assignVoices();
      void _onRemoveSource( SFXSource* source );  

   public:	  
	   // <Edit> [3/18/2009 iceRain] 
	   //控制声音减弱的开始时间
	   U32 mStartTime;
	   StringTableEntry mSoundID1;
	   StringTableEntry mSoundID2;
	   void controlSoundVolume(U32 time);
	   /// Called from SFXProfile to create a device specific
	   /// sound buffer used in conjunction with a voice in playback.
	   SFXBuffer*  _createBuffer( SFXProfile* profile );

      static SFXSystem* getSingleton() { return smSingleton; }
      static void init();
      static void destroy();
      void _update();					//帧循环更新声音数据

      /// This initializes a new device.
      ///
      /// @param providerName    The name of the provider.
      /// @param deviceName      The name of the provider device.
      /// @param useHardware     Toggles the use of hardware processing when available.
      /// @param maxBuffers      The maximum buffers for this device to use or -1 
      ///                        for the device to pick its own reasonable default.
      /// @param changeDevice    Allows this to change the current device to a new one
      /// @return Returns true if the device was created.
      ///
      bool createDevice(   const char* providerName, 
                           const char* deviceName, 
                           bool useHardware,
                           S32 maxBuffers,
                           bool changeDevice = false);


      /// Returns the current device information or NULL if no
      /// device is present.  The information string is in the
      /// following format:
      /// 
      /// Provider Name\tDevice Name\tUse Hardware\tMax Buffers
      ///
      const char* getDeviceInfoString();

      /// This destroys the current device.  All sources loose their
      /// playback buffers, but otherwise continue to function.
      void deleteDevice();

      /// Returns true if a device is allocated.
      bool hasDevice() const { return mDevice != NULL; }

      /// Used to create new sound sources from a sound profile.  The
      /// returned source is in a stopped state and ready for playback.
      /// Use the SFX_DELETE macro to free the source when your done.
      ///
      /// @param profile   The sound profile for the created source.
      /// @param transform The optional transform if creating a 3D source.
      /// @param velocity  The optional doppler velocity if creating a 3D source.
      ///
      /// @return The sound source or NULL if an error occured.
      ///
      SFXSource*  createSource(  const SFXProfile* profile, 
                                 const MatrixF* transform = NULL, 
                                 const VectorF* velocity = NULL );

      /// Creates a source which when it finishes playing will auto delete
      /// itself.  Be aware that the returned SFXSource pointer should only
      /// be used for error checking or immediate setting changes.  It may
      /// be deleted as soon as the next system tick.
      ///
      /// @param profile   The sound profile for the created source.
      /// @param transform The optional transform if creating a 3D source.
      /// @param velocity  The optional doppler velocity if creating a 3D source.
      ///
      /// @return The sound source or NULL if an error occured.
      ///
      SFXSource* playOnce( const StringTableEntry soundID, 
                           const MatrixF *transform = NULL,
                           const VectorF *velocity = NULL );

      /// Returns the one and only listener object.
      SFXListener& getListener() { return mListener; }

      /// Stops all the sounds in a particular channel or across 
      /// all channels if the channel is -1.
      ///
      /// @param channel The channel index less than NumChannels or -1.
      ///
      /// @see NumChannels
      ///
      void stopAll( S32 channel = -1 );

      /// Returns the volume for the specified sound channel.
      ///
      /// @param channel The channel index less than NumChannels.
      ///
      /// @return The channel volume.
      ///
      /// @see NumChannels
      ///
      F32 getChannelVolume( U32 channel ) const;

      /// Sets the volume on the specified sound channel, changing
      /// the volume on all sources in that channel.
      ///
      /// @param channel The channel index less than NumChannels.
      /// @param volume The channel volume to set.
      ///
      /// @see NumChannels
      ///
      void setChannelVolume( U32 channel, F32 volume );

      /// Returns the system master volume level.
      ///
      /// @return The channel volume.
      ///
      F32 getMasterVolume() const { return mMasterVolume; }

      /// Sets the master volume level, changing the
      /// volume of all sources.
      ///
      /// @param volume The channel volume to set.
      ///
      void setMasterVolume( F32 volume );
	  
	  FMOD_RESULT setChannelGroupVolumeByIndex(CHANNEL_GROUP_INDEX index, F32 volume);
	  FMOD_RESULT setChannelGroupPaused(CHANNEL_GROUP_INDEX          index, FMOD_BOOL);
	  
	  SFXDevice* getDevice() const { return mDevice; }

	  typedef stdext::hash_map<StringTableEntry, FMOD_CHANNEL*>	ChannelMap;
	  ChannelMap mChannelMap;
	  FMOD_CHANNEL* mGlobal;
	  FMOD_CHANNEL* getChannelBySoundID(StringTableEntry id);
	 // void addChannelMap();
	  void clearChannelMap();
	 
	  SFXSourceVector getVector() const { return mSources; }

};

#define SFX SFXSystem::getSingleton()

#undef  SFX_DELETE
#define SFX_DELETE( source )  \
   if ( source )              \
   {                          \
      source->deleteObject(); \
      source = NULL;          \
   }                          \

#endif // _SFXSYSTEM_H_
