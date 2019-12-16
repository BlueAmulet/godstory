//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SFXPROFILE_H_
#define _SFXPROFILE_H_

#ifndef _CONSOLETYPES_H_
   #include "console/consoleTypes.h"
#endif
#ifndef _SIMDATABLOCK_H_
   #include "console/simDataBlock.h"
#endif
#ifndef _SFXDESCRIPTION_H_
   #include "sfx/sfxDescription.h"
#endif
#ifndef _SFXDEVICE_H_
   #include "sfx/sfxDevice.h"
#endif
#ifndef _SFXRESOURCE_H_
   #include "sfx/sfxResource.h"
#endif
#ifndef _SFXBUFFER_H_
   #include "sfx/sfxBuffer.h"
#endif


/// The SFXProfile is used to define a sound for playback.
///
/// A few tips:
///
/// Make sure each of the defined SFXProfile's fileName doesn't specify 
/// an extension. An extension does not need to be specified and by not
/// explicitly saying .ogg or .wav it will allow you to change from one 
/// format to the other without having to change the scripts.
///
/// Make sure that server SFXProfiles are defined with the datablock 
/// keyword, and that client SFXProfiles are defined with the 'new' 
/// keyword.
///
/// Make sure SFXDescriptions exist for your SFXProfiles. Also make sure
/// that SFXDescriptions are defined BEFORE SFXProfiles. This is especially
/// important if your SFXProfiles are located in different files than your
/// SFXDescriptions. In this case, make sure the files containing SFXDescriptions
/// are exec'd before the files containing the SFXProfiles.
///
class SFXProfile : public SimDataBlock
{
   friend class SFXEmitter; // For access to mFilename

   protected:

      typedef SimDataBlock Parent;

      /// Used on the client side during onAdd.
      S32 mDescriptionId;

	 

      /// The sound data.
      Resource<SFXResource> mResource;

      /// The description which controls playback settings.
      SFXDescription *mDescription;

      /// The sound filename.  If no extension is specified
      /// the system will try .wav first then other formats.
      StringTableEntry mFilename;

      /// If true the sound data will be loaded from
      /// disk and possibly cached with the active 
      /// device before the first call for playback.
      bool mPreload;

      /// The device specific data buffer.
      SafePtr<SFXBuffer> mBuffer;

      /// Called when the buffer needs to be preloaded.
      bool _preloadBuffer();

      /// Callback for device events.
      void _onDeviceEvent( SFXDevice *device, SFXDeviceEventType evt );

   public:

	    StringTableEntry mSoundID;
      /// This is only here to allow DECLARE_CONOBJECT 
      /// to create us from script.  You shouldn't use
      /// this constructor from C++.
      explicit SFXProfile();

      /// The constructor.
      SFXProfile( SFXDescription* desc, 
                  StringTableEntry filename = NULL, 
                  bool preload = false );

      /// The destructor.
      virtual ~SFXProfile();

      DECLARE_CONOBJECT( SFXProfile );

      static void initPersistFields();

      // SimObject 
      bool onAdd();
      void onRemove();
      void packData( BitStream* stream );
      void unpackData( BitStream* stream );

      /// Returns the sound filename.
      const char* getFilename() const { return mFilename; }

      /// @note This has nothing to do with mPreload.
      /// @see SimDataBlock::preload
      bool preload( bool server, char errorBuffer[256] );

      /// Returns the description object for this sound profile.
      SFXDescription* getDescription() const { return mDescription; }

      /// Returns the sound resource loading it from
      /// disk if it hasn't been preloaded.
      const Resource<SFXResource>& getResource();

      /// Returns the device buffer for this for this 
      /// sound.  It will load it from disk if it has
      /// not been created already.
      SFXBuffer* getBuffer();

	  StringTableEntry getSoundID() const {return mDescription->mSoundID;}

	  U32 getSoundChannel() const { return mDescription->mSoundChannel; } 
	  bool Insert(SFXDescription* desc);
};

DECLARE_CONSOLETYPE( SFXProfile );


#endif  // _SFXPROFILE_H_