//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SFXEMITTER_H_
#define _SFXEMITTER_H_

#ifndef _RENDERABLESCENEOBJECT_H
#include "sceneGraph/RenderableSceneObject.h"
#endif
#ifndef _SFXPROFILE_H_
#include "sfx/sfxProfile.h"
#endif

class SFXSource;


/// The SFXEmitter is used to place 2D or 3D sounds into a 
/// mission.
///
/// If the profile is set then the emitter plays that.  If the
/// profile is null and the filename is set then the local emitter
/// options are used.
///
/// Note that you can call SFXEmitter.play() and SFXEmitter.stop()
/// to control playback from script.
///
class SFXEmitter : public RenderableSceneObject
{
   typedef RenderableSceneObject Parent;

   protected:
      Box3F mEffectBox;

      /// The sound source for the emitter.
      SFXSource *mSource;

      /// The selected profile or null if the local
      /// profile should be used.
      SFXProfile *mProfile;
	  StringTableEntry	mSoundID;

      /// A local profile object used to coax the
      /// sound system to play a custom sound.
      SFXProfile mLocalProfile;

      /// The description used by the local profile.
      SFXDescription* mDescription;

      /// If true playback starts when the emitter
      /// is added to the scene.
      bool mPlayOnAdd;

      /// Network update masks.
      enum UpdateMasks 
      {
         InitialUpdateMask    = BIT(0),
         TransformUpdateMask  = BIT(1),
         DirtyUpdateMask      = BIT(2),

         SourcePlayMask       = BIT(3),
         SourceStopMask       = BIT(4),

         AllSourceMasks = SourcePlayMask | SourceStopMask,
      };

      /// Dirty flags used to handle sound property
      /// updates locally and across the network.
      enum Dirty
      {
         Profile                    = BIT(0),
         Filename                   = BIT(2),
         Volume                     = BIT(4),
         IsLooping                  = BIT(5),
         Is3D                       = BIT(6),
         ReferenceDistance          = BIT(7),
         MaxDistance                = BIT(8),
         ConeInsideAngle            = BIT(9),
         ConeOutsideAngle           = BIT(10),
         ConeOutsideVolume          = BIT(11),
         Transform                  = BIT(12),
         Channel                    = BIT(13),
         OutsideAmbient             = BIT(14),
		 SoundID					= BIT(15),
         AllDirtyMask               = 0xFFFFFFFF,
      };

      /// The current dirty flags.
      BitSet32 mDirty;

      /// Helper which reads a flag from the stream and 
      /// updates the mDirty bits.
      bool _readDirtyFlag( BitStream *stream, U32 flag );

      /// Called when the emitter state has been marked
      /// dirty and the source needs to be updated.
      void _update();

   public:

      SFXEmitter();
      virtual ~SFXEmitter();

      DECLARE_CONOBJECT( SFXEmitter );

      // SimObject
      bool onAdd();
      void onRemove();
      void onStaticModified( const char *slotName, const char *newValue = NULL );
      static void initPersistFields();

	  void OnSerialize( CTinyStream& stream );
	  void OnUnserialize( CTinyStream& stream );

      // NetObject
      U64 packUpdate( NetConnection *conn, U64 mask, BitStream *stream );
      void unpackUpdate( NetConnection *conn, BitStream *stream );

      // SceneObject
	  void setSoundId(StringTableEntry soundId){mSoundID = soundId;}
      void setTransform( const MatrixF &mat );
      void setScale( const VectorF &scale );
      bool prepRenderImage( SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState );

	  //RenderableSceneObject
      void renderObject(SceneState *state);

      /// Sends network event to start playback if 
      /// the emitter source is not already playing.
      void play();

      /// Sends network event to stop emitter 
      /// playback on all ghosted clients.
      void stop();

	  virtual bool loadRenderResource();
	  virtual void freeRenderResource();
};

#endif // _SFXEMITTER_H_
