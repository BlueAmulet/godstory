//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "T3D/sfxEmitter.h"

#include "sfx/sfxSystem.h"
//#include "T3D/ambientAudioManager.h"
#include "sceneGraph/sceneState.h"
#include "core/bitStream.h"
#ifdef NTJ_CLIENT
#include "Gameplay/LoadMgr/ObjectLoadManager.h"
#endif


extern bool gEditingMission;

IMPLEMENT_CO_NETOBJECT_V1(SFXEmitter);

DECLARE_SERIALIBLE(SFXEmitter);


SFXEmitter::SFXEmitter()
   :  RenderableSceneObject(),
      mSource( NULL ),
	  mSoundID(NULL),
	  mProfile(NULL),
	  mDescription(NULL),
      mPlayOnAdd( true )
{
   mTypeMask |= MarkerObjectType;
   mNetFlags.set( Ghostable | ScopeAlways );

   mDescription = NULL;
  // mDescription->mIsLooping = true;
}

SFXEmitter::~SFXEmitter()
{
}

void SFXEmitter::initPersistFields()
{
   Parent::initPersistFields();
   addGroup("Media");
   addField("soundID",				  TypeString,				 Offset(mSoundID, SFXEmitter));
   endGroup("Media");
}

U64 SFXEmitter::packUpdate( NetConnection *con, U64 mask, BitStream *stream )
{
   U64 retMask = Parent::packUpdate( con, mask, stream );

   if ( stream->writeFlag( mask & InitialUpdateMask ) )
   {
	   // If this is the initial update then all the source
	   // values are dirty and must be transmitted.
	   mask |= TransformUpdateMask;
	   mDirty = AllDirtyMask;

	   // Clear the source masks... they are not
	   // used during an initial update!
	   mask &= ~AllSourceMasks;
   }


   // transform
   if ( stream->writeFlag( mask & TransformUpdateMask ) )
	   stream->writeAffineTransform( mObjToWorld );

   stream->writeFlag( mPlayOnAdd );

   if( stream->writeFlag(mDirty.test(SoundID)))
	   stream->writeString(mSoundID);

   mDirty.clear();

   //// We should never have both source masks 
   //// enabled at the same time!
   //AssertFatal( ( mask & AllSourceMasks ) != AllSourceMasks, 
   //   "SFXEmitter::packUpdate() - Bad source mask!" );

   //// Write the source playback state.
   stream->writeFlag( mask & SourcePlayMask );
   stream->writeFlag( mask & SourceStopMask );

   return retMask;
}

bool SFXEmitter::_readDirtyFlag( BitStream* stream, U32 mask )
{
   bool flag = stream->readFlag();
   if ( flag )
      mDirty.set( mask );

   return flag;
}

void SFXEmitter::unpackUpdate( NetConnection *conn, BitStream *stream )
{
  Parent::unpackUpdate( conn, stream );
   // initial update?
   bool initialUpdate = stream->readFlag();

   // transform
   if ( _readDirtyFlag( stream, Transform ) )
   {
	   MatrixF mat;
	   stream->readAffineTransform(&mat);
	   Parent::setTransform(mat);
   }

   mPlayOnAdd = stream->readFlag();

   if( _readDirtyFlag(stream,  SoundID))
	   mSoundID = stream->readSTString();
//   // update the emitter now?
//   if ( !initialUpdate )
//      _update();
//
//   // Check the source playback masks.
   if ( stream->readFlag() ) // SourcePlayMask
     play();
   if ( stream->readFlag() ) // SourceStopMask
     stop();
}

void SFXEmitter::onStaticModified( const char* slotName, const char* newValue )
{
   // NOTE: The signature for this function is very 
   // misleading... slotName is a StringTableEntry.

   // We don't check for changes on the client side.
   if ( isClientObject() )
      return;

   // Lookup and store the property names once here
   // and we can then just do pointer compares. 
   static StringTableEntry slotPosition   = StringTable->lookup( "position" );
   static StringTableEntry slotRotation   = StringTable->lookup( "rotation" );
   static StringTableEntry slotScale      = StringTable->lookup( "scale" );;
   static StringTableEntry slotSoundID    = StringTable->lookup( "soundID" );

   // Set the dirty flags.
   mDirty.clear();
   if (  slotName == slotPosition ||
         slotName == slotRotation ||
         slotName == slotScale )
      mDirty.set( Transform );
   else if( slotName == slotSoundID)
	   mDirty.set( SoundID );

   //mSoundID = StringTable->insert(newValue);
   if ( mDirty )
      setMaskBits( DirtyUpdateMask );
}

bool SFXEmitter::onAdd()
{
   if ( !Parent::onAdd() )
      return false;


#ifdef NTJ_EDITOR
   if(isClientObject())
   {
	   _update();

	   // Do we need to start playback?
	   if ( mPlayOnAdd && mSource )
		   mSource->play();
   }
#endif

#ifdef NTJ_CLIENT
   
   if(mSoundID != NULL)
   {
	   mProfile = g_SoundManager->FindProfile(mSoundID);

	   if(mProfile)
	   {
		   mDescription = mProfile->mDescription;
	   }
   }

   if(!mDescription)
	   return false;

   Box3F box;

   Box3F worldBox = getWorldBox();
   mEffectBox.min.x = worldBox.min.x - mProfile->getDescription()->mMaxDistance;
   mEffectBox.max.x = worldBox.min.x + mProfile->getDescription()->mMaxDistance;
   mEffectBox.min.y = worldBox.min.y - mProfile->getDescription()->mMaxDistance;
   mEffectBox.max.y = worldBox.max.y + mProfile->getDescription()->mMaxDistance;

   gClientObjectLoadMgr.registObject(this, mEffectBox);
   
#endif


   // Setup the bounds.
   mObjBox.max = mObjScale;
   mObjBox.min = mObjScale;
   mObjBox.min.neg();
   resetWorldBox();
   addToScene();

   return true;
}

void SFXEmitter::onRemove()
{
   SFX_DELETE( mSource );
#ifdef NTJ_CLIENT
   gClientObjectLoadMgr.unregistObject(this, mEffectBox);
#endif
   removeFromScene();
   Parent::onRemove();
}

void SFXEmitter::_update()
{
   AssertFatal( isClientObject(), "SFXEmitter::_update() - This shouldn't happen on the server!" );

   if(!mDescription)
   {
		if(!mProfile)
		{
			if(!mSoundID)
				return;

			mProfile = g_SoundManager->FindProfile(mSoundID);
			if(!mProfile)
				return;
		}

		mDescription = mProfile->mDescription;
		if ( !mDescription )
		{
			Con::errorf("SFXEmitter::_update() missing a description!");
			return;
		}
   }

   // Store the playback status so we
   // we can restore it.
   SFXStatus prevState = mSource ? mSource->getStatus() : SFXStatusNull;

   // Make sure all the settings are valid.
   mDescription->validate();

   const MatrixF &transform   = getTransform();
   const VectorF &velocity    = getVelocity();

   // Did we change the source?
   if ( mDirty.test( SoundID ) )
   {
      SFX_DELETE( mSource );

	  mSource = SFX->createSource( mProfile, &transform, &velocity );
	  // If we're supposed to play when the emitter is 
	  // added to the scene then also restart playback 
	  // when the profile changes.
	  prevState = mPlayOnAdd ? SFXStatusPlaying : prevState;
   }

   // Cheat if the editor is open and the looping state
   // is toggled on a local profile sound.  It makes the
   // editor feel responsive and that things are working.
   if (  gEditingMission && mPlayOnAdd )
      prevState = SFXStatusPlaying;

   // The rest only applies if we have a source.
   if ( mSource )
   {
         mSource->setVolume( mDescription->mVolume );

      // Skip these 3d only settings.
      if ( mDescription->mIs3D )
      {
         if ( mDirty.test( Transform ) )
         {
            mSource->setTransform( transform );
            mSource->setVelocity( velocity );
         }

       mSource->setMinMaxDistance(   mDescription->mReferenceDistance, mDescription->mMaxDistance );

        mSource->setCone( mDegToRad( (F32)mDescription->mConeInsideAngle ),
                              mDegToRad( (F32)mDescription->mConeOutsideAngle ),
                              mDescription->mConeOutsideVolume );
      }     

      // Restore the pre-update playback state.
      if ( prevState == SFXStatusPlaying )
         mSource->play();
   }

   // Clear all the dirty flags.
   mDirty.clear();
}

void SFXEmitter::play()
{
   if ( mSource )
      mSource->play();
   else
   {
      // By clearing the playback masks first we
      // ensure the last playback command called 
      // within a single tick is the one obeyed.
      clearMaskBits( AllSourceMasks );

      setMaskBits( SourcePlayMask );
   }
}

void SFXEmitter::stop()
{
   if ( mSource )
      mSource->stop();
   else
   {
      // By clearing the playback masks first we
      // ensure the last playback command called 
      // within a single tick is the one obeyed.
      clearMaskBits( AllSourceMasks );

      setMaskBits( SourceStopMask );
   }
}

void SFXEmitter::setTransform( const MatrixF &mat )
{
   // Set the transform directly from the 
   // matrix created by inspector.
   Parent::setTransform( mat );
   setMaskBits( TransformUpdateMask );
}

void SFXEmitter::setScale( const VectorF &scale )
{
   // We ignore scale... it doesn't effect us.
}

bool SFXEmitter::prepRenderImage(SceneState * state, const U32 stateKey, const U32, const bool)
{
   // TODO: Implement visual feedback of 
   // sound range and directional cone!

   return false;
}

void SFXEmitter::renderObject(SceneState*)
{
   // TODO: Implement visual feedback of 
   // sound range and directional cone!
}

bool SFXEmitter::loadRenderResource()
{
	PROFILE_SCOPE(SFXEmitter_loadRenderResource);

	// Store the playback status so we
	// we can restore it.
	SFXStatus prevState = mSource ? mSource->getStatus() : SFXStatusNull;

	// Make sure all the settings are valid.
	mDescription->validate();

	const MatrixF &transform   = getTransform();
	const VectorF &velocity    = getVelocity();

	// Did we change the source?
	SFX_DELETE( mSource );

	mSource = SFX->createSource( mProfile, &transform, &velocity );
	// If we're supposed to play when the emitter is 
	// added to the scene then also restart playback 
	// when the profile changes.
	prevState = mPlayOnAdd ? SFXStatusPlaying : prevState;

	// Cheat if the editor is open and the looping state
	// is toggled on a local profile sound.  It makes the
	// editor feel responsive and that things are working.
	if (  gEditingMission && mPlayOnAdd )
		prevState = SFXStatusPlaying;

	// The rest only applies if we have a source.
	if ( mSource )
	{
		mSource->setVolume( mDescription->mVolume );

		// Skip these 3d only settings.
		if ( mDescription->mIs3D )
		{
			if ( mDirty.test( Transform ) )
			{
				mSource->setTransform( transform );
				mSource->setVelocity( velocity );
			}

			mSource->setMinMaxDistance(   mDescription->mReferenceDistance, mDescription->mMaxDistance );

			mSource->setCone( mDegToRad( (F32)mDescription->mConeInsideAngle ),
				mDegToRad( (F32)mDescription->mConeOutsideAngle ),
				mDescription->mConeOutsideVolume );
		}     

		// Restore the pre-update playback state.
		//if ( prevState == SFXStatusPlaying )
		mSource->play();
	}

	// Clear all the dirty flags.
	mDirty.clear();

	return Parent::loadRenderResource();
}

void SFXEmitter::freeRenderResource()
{
	//if ( mSource )
	//	mSource->stop();

	//SFX_DELETE( mSource );
	//Parent::freeRenderResource();
}

void SFXEmitter::OnSerialize( CTinyStream& stream )
{
	__super::OnSerialize( stream );

	stream.writeString( mSoundID );
}

void SFXEmitter::OnUnserialize( CTinyStream& stream )
{
	__super::OnUnserialize( stream );

	char buf[1024];
	stream.readString( buf, 1024 );
	mSoundID = StringTable->insert( buf );
}

ConsoleMethod( SFXEmitter, play, void, 2, 2,   
   "SFXEmitter.play()\n"
   "Sends network event to start playback if "
   "the emitter source is not already playing." )
{
   object->play();
}

ConsoleMethod( SFXEmitter, stop, void, 2, 2,   
   "SFXEmitter.stop()\n"
   "Sends network event to stop emitter "
   "playback on all ghosted clients." )
{
   object->stop();
}

