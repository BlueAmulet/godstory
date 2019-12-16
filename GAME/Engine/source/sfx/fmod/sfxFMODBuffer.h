//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SFXFMODBUFFER_H_
#define _SFXFMODBUFFER_H_

#ifndef _SFXBUFFER_H_
   #include "sfx/sfxBuffer.h"
#endif
#ifndef _SFXRESOURCE_H_
   #include "sfx/sfxResource.h"
#endif
#ifndef _FMOD_H
   #include "fmod/inc/fmod.hpp"
#endif


class SFXProfile;


class SFXFMODBuffer : public SFXBuffer
{
   friend class SFXFMODDevice;
   friend class SFXFMODVoice;

   protected:

      Resource<SFXResource> mResource;

      FMOD_SOUND *mSound;	   
      FMOD_MODE mMode;

	  static StringTableEntry mSoundID;
	  static U32 mSoundChannel; 
      // 
      SFXFMODBuffer( const Resource<SFXResource> &resource,
                     FMOD_SOUND *sound );

      // copy data into buffer
      bool _copyData( U32 offset, const U8* data, U32 length);

      /// Copy the data to the buffer.
      void _fillBuffer();
	  // <editor>:iceRain 得到声音通道
	  U32 getSoundChannel() const { return mSoundChannel; }
	  

      virtual ~SFXFMODBuffer();

   public:

      ///
      static SFXFMODBuffer* create( SFXProfile *profile );
	  StringTableEntry getSoundID() const { return mSoundID; }

};

//extern FMOD_SOUND *sound;
/// A vector of SFXDSBuffer pointers.
typedef Vector<SFXFMODBuffer*> SFXFMODBufferVector;

#endif // _SFXFMODBUFFER_H_