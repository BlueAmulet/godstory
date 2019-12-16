//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SFXDSVOICE_H_
#define _SFXDSVOICE_H_

#ifndef _SFXVOICE_H_
   #include "sfx/sfxVoice.h"
#endif
#ifndef _SFXDSBUFFER_H_
   #include "sfx/dsound/sfxDSBuffer.h"
#endif

#include <dsound.h>

class SFXDSDevice;


class SFXDSVoice : public SFXVoice
{
   protected:

      SFXDSVoice( SFXDSDevice *device,
                  SFXDSBuffer *buffer,
                  IDirectSoundBuffer8 *dsBuffer, 
                  IDirectSound3DBuffer8 *dsBuffer3d );

      /// The device used to commit deferred settings. 
      SFXDSDevice *mDevice;

      SFXDSBuffer *mBuffer;

      IDirectSoundBuffer8 *mDSBuffer;

      IDirectSound3DBuffer8 *mDSBuffer3D;

      U32 mFrequency;

      bool mPaused;

      /// Helper for converting floating point linear volume
      /// to a logrithmic integer volume for dsound.
      static LONG _linearToLogVolume( F32 linVolume );

   public:

      ///
      static SFXDSVoice* create( SFXDSDevice *device,
                                 SFXDSBuffer *buffer );

      ///
      virtual ~SFXDSVoice();

      /// SFXVoice
      void setPosition( U32 pos );
      void setMinMaxDistance( F32 min, F32 max );
      void play( U32 looping );
      void pause();
      void stop();
      SFXStatus getStatus() const;
      void setVelocity( const VectorF& velocity );
      void setTransform( const MatrixF& transform );
      void setVolume( F32 volume );
      void setPitch( F32 pitch );
      void setCone( F32 innerAngle, F32 outerAngle, F32 outerVolume );

};


/// A vector of SFXDSVoice pointers.
typedef Vector<SFXDSVoice*> SFXDSVoiceVector;

#endif // _SFXDSBUFFER_H_