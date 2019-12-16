//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SFXNULLVOICE_H_
#define _SFXNULLVOICE_H_

#ifndef _SFXVOICE_H_
   #include "sfx/sfxVoice.h"
#endif
#ifndef _SFXSTATUS_H_
   #include "sfx/sfxStatus.h"
#endif


class SFXNullVoice : public SFXVoice
{
   friend class SFXNullDevice;

   protected:

      SFXNullVoice();

      SFXStatus mStatus;

   public:

      virtual ~SFXNullVoice();

      /// SFXVoice
      void setPosition( U32 pos );
      void setMinMaxDistance( F32 min, F32 max );
      SFXStatus getStatus() const;
      void play( U32 looping );
      void pause();
      void stop();
      void setVelocity( const VectorF& velocity );
      void setTransform( const MatrixF& transform );
      void setVolume( F32 volume );
      void setPitch( F32 pitch );
      void setCone( F32 innerAngle, F32 outerAngle, F32 outerVolume );
};


/// A vector of SFXNullBuffer pointers.
typedef Vector<SFXNullVoice*> SFXNullVoiceVector;

#endif // _SFXNULLVOICE_H_