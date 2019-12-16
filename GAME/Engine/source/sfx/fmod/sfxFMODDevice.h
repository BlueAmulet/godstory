//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SFXFMODDEVICE_H_
#define _SFXFMODDEVICE_H_

#ifndef _SFXDEVICE_H_
   #include "sfx/sfxDevice.h"
#endif
#ifndef _SFXFMODVOICE_H_
   #include "sfx/fmod/sfxFMODVoice.h"
#endif
#ifndef _SFXFMODBUFFER_H_
   #include "sfx/fmod/sfxFMODBuffer.h"
#endif

#include "sfx/sfxResource.h"
#include "core/tDictionary.h"

#include "fmod.hpp"

#include "platform/platformDlibrary.h"
//#include "Gameplay/Data//SoundResource.h"

// This doesn't appear to exist in some contexts, so let's just add it.
#ifdef POWER_OS_WIN32
#ifndef WINAPI
#define WINAPI __stdcall
#endif
#else
#define WINAPI
#endif

#define FModAssert(x, msg) AssertISV(x==FMOD_OK, msg);

#ifdef POWER_OS_MAC
#define FMOD_FN_FILE "sfx/fmod/fmodFunctions.mac.h"
#else
#define FMOD_FN_FILE "sfx/fmod/fmodFunctions.h"
#endif

// Typedefs
#define FMOD_FUNCTION(fn_name, fn_args) \
   typedef FMOD_RESULT (WINAPI *FMODFNPTR##fn_name)fn_args;
#include FMOD_FN_FILE
#undef FMOD_FUNCTION

// Function table
struct FModFNTable
{
   FModFNTable() : isLoaded( false ) {};
   bool isLoaded;
   DLibraryRef dllRef;

#define FMOD_FUNCTION(fn_name, fn_args) \
   FMODFNPTR##fn_name fn_name;
#include FMOD_FN_FILE
#undef FMOD_FUNCTION
};

class SFXProvider;

enum CHANNEL_GROUP_INDEX
{
	BACKGROUND_MUSIC_CHANNELGROUP					= 0,
	UI_SOUND_CHANNELGROUP							= 1,
	AMBIENT_SOUND_CHANNELGROUP						= 2,
	DYNAMIC_SOUND_CHANNELGROUP						= 3,
};

class SFXFMODDevice : public SFXDevice
{
   typedef SFXDevice Parent;

   public:

      explicit SFXFMODDevice();

      SFXFMODDevice( SFXProvider* provider, FModFNTable *fmodFnTbl, int deviceIdx, const char* name );

      virtual ~SFXFMODDevice();

   protected:

      const StringTableEntry mName;

      /// The active voices.
      SFXFMODVoiceVector mVoices;

      /// The allocated buffers.
      SFXFMODBufferVector mBuffers;

      FMOD_MODE m3drolloffmode;

	  FMOD_CHANNELGROUP *mChannelGroup[4], *masterGroup;

   public:

      const char* getName() const { return mName; }

      SFXBuffer* createBuffer( SFXProfile *profile );

      ///
      SFXVoice* createVoice( bool is3D, SFXBuffer* buffer, U32 priority);

      ///
      void deleteVoice( SFXVoice* buffer );

      U32 getVoiceCount() const { return mVoices.size(); }

      void update( const SFXListener& listener );
      
      virtual bool supportsResource( const SFXResource *resource )
      {
         if (resource == NULL)
            return false;
         return true;
      }
      
      //virtual SFXBuffer* preload(const SFXPreloadedBuffer* r);

      FMOD_MODE get3dRollOffMode() { return m3drolloffmode; }

	  FMOD_CHANNELGROUP* getChannelGroup(CHANNEL_GROUP_INDEX index);
	  
	  SFXFMODVoiceVector getVoice() const { return mVoices; }

	  static FMOD_SYSTEM *smSystem;
	  static FModFNTable *smFunc;
};

#endif // _SFXFMODDEVICE_H_
