//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SFXFMODVOICE_H_
#define _SFXFMODVOICE_H_

#ifndef _SFXDEVICE_H_
   #include "sfx/sfxDevice.h"
#endif
#ifndef _SFXVOICE_H_
   #include "sfx/sfxVoice.h"
#endif

#include "sfx/sfxResource.h"

#include "fmod/inc/fmod.hpp"
#include <hash_map>

class SFXSource;
class SFXFMODBuffer;
class SFXFMODDevice;

class SFXFMODVoice : public SFXVoice
{

protected:

      SFXFMODDevice *mDevice;

      SFXFMODBuffer *mBuffer;

      FMOD_CHANNEL *mChannel;
	  
	  U32 mPriority;
	 
	 
      ///
	   SFXFMODVoice(  SFXFMODDevice *device, 
                     SFXFMODBuffer *buffer, U32 priority);

      // copy data into buffer
      bool _copyData(U32 offset, const U8* data, U32 length );

      // prep for playback
      bool _assignChannel();
public:
	 
      ///
      static SFXFMODVoice* create(  SFXFMODDevice *device, 
                                    SFXFMODBuffer *buffer , U32 priority);

      ///
      virtual ~SFXFMODVoice();

      /// SFXVoice
      void setPosition( U32 pos );
      void setMinMaxDistance( F32 min, F32 max );
	  void play(U32 looping);
      void stop();
      void pause();
      SFXStatus getStatus() const;
      void setVelocity( const VectorF& velocity );
      void setTransform( const MatrixF& transform );
      void setVolume( F32 volume );
      void setPitch( F32 pitch );
      void setCone( F32 innerAngle, F32 outerAngle, F32 outerVolume );
	  // <Edit> [3/5/2009 icerain] 
	  //FMOD_CHANNEL* getChannelBySoundID(StringTableEntry id);
	  //void clearChannelMap();
	  FMOD_CHANNEL* getChannel() const { return mChannel; }
	  //SFXFMODBuffer* getBuffer() const { return mBuffer; }
};


/// A vector of SFXFMODVoice pointers.
typedef Vector<SFXFMODVoice*> SFXFMODVoiceVector;


#endif // _SFXFMODBUFFER_H_