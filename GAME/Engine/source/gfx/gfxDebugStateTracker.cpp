//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gfx/gfxDevice.h"

#ifdef POWER_GFX_STATE_DEBUG

#include "gfx/gfxDebugStateTracker.h"

namespace GFXDebugState
{

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// Static initialization
const char *GFXDebugStateEventTypeNames[GFXDebugStateEvent_COUNT] = { 
   "GFXNoStateEvent", 
   "GFXRenderStateEvent",
   "GFXSamplerStateEvent",
   "GFXTextureStageStateEvent",
   "GFXShaderStateEvent",
   "GFXDataStateEvent",
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void GFXDebugStateWatch::issueWatch()
{
   // Clear tracked state events
   mTrackedStateEvents.clear();
}

//------------------------------------------------------------------------------

void GFXDebugStateWatch::processStateEvent( const StateEventRef &stateEventRef )
{
   bool different = false;
   bool interested = false;

   // Bail if we are looking at event type, and this event isn't of that type
   if( mWatchedStateEvent.getEventMask() & GFXDebugStateEvent::EventType )
   {
      if( stateEventRef->getEventType() != mWatchedStateEvent.getEventType() )
         return;

      interested = true;
   }


   // Bail if we are looking at state id, and this isn't the right state id
   if( mWatchedStateEvent.getEventMask() & GFXDebugStateEvent::State )
   {
      if( stateEventRef->getState() != mWatchedStateEvent.getState() )
         return;

      interested = true;
   }

   // Go through the mask values, and figure out if the states are different and
   // if we even care about this event
   if( mWatchedStateEvent.getEventMask() & GFXDebugStateEvent::FileName  )
   { 
      different |= ( dStrcmp( stateEventRef->getFileName(), mWatchedStateEvent.getFileName() ) != 0 ); 
      interested = true; 
   }

#define _MASK_TEST_HELPER( maskBit, getFxnName ) if( mWatchedStateEvent.getEventMask() & maskBit ) { different |= ( stateEventRef->getFxnName() != mWatchedStateEvent.getFxnName() ); interested = true; }

   _MASK_TEST_HELPER( GFXDebugStateEvent::LineNumber, getLineNumber );
   _MASK_TEST_HELPER( GFXDebugStateEvent::PrevValue, getPrevValue );
   _MASK_TEST_HELPER( GFXDebugStateEvent::EventValue, getEventValue );
   _MASK_TEST_HELPER( GFXDebugStateEvent::TextureUnit, getTextureUnit );

#undef _MASK_TEST_HELPER

   // If we don't care about this event, bail
   if( !interested )
      return;

   switch( mBreakTrigger )
   {
      // If this watch should break as soon as something sets the value away from
      // the value contained in the watched state, than break now
      case BreakOnChangeFrom:
         if( different )
            Platform::debugBreak();
         break;

      // If the watch should break as soon as something sets the value to the
      // value in the watched state, break now
      case BreakOnChangeTo:
         if( !different )
            Platform::debugBreak();
         break;
   }

   // Track the state event
   mTrackedStateEvents.push_back( stateEventRef );
}

//------------------------------------------------------------------------------

void GFXDebugStateWatch::processWatch()
{
   // Get some info from GFX
   S32 curValue = GFXDebugStateEvent::EventValue_UninitVal;

   switch( mWatchedStateEvent.getEventType() )
   {
      case GFXRenderStateEvent:
         AssertFatal( mWatchedStateEvent.getState() != GFXDebugStateEvent::State_UninitVal, "Lame functionality" );
         curValue = mGFXDevice->mStateTracker[mWatchedStateEvent.getState()].newValue;
         break;

      case GFXSamplerStateEvent:
         AssertFatal( mWatchedStateEvent.getState() != GFXDebugStateEvent::State_UninitVal, "Lame functionality" );
         AssertFatal( mWatchedStateEvent.getTextureUnit() != GFXDebugStateEvent::TextureUnit_UninitVal, "Lame functionality" );
         curValue = mGFXDevice->mSamplerStateTracker[mWatchedStateEvent.getTextureUnit()][mWatchedStateEvent.getState()].newValue;
         break;

      case GFXTextureStageStateEvent:
         AssertFatal( mWatchedStateEvent.getState() != GFXDebugStateEvent::State_UninitVal, "Lame functionality" );
         AssertFatal( mWatchedStateEvent.getTextureUnit() != GFXDebugStateEvent::TextureUnit_UninitVal, "Lame functionality" );
         curValue = mGFXDevice->mTextureStateTracker[mWatchedStateEvent.getTextureUnit()][mWatchedStateEvent.getState()].newValue;

      default:
         AssertFatal( false, "Hacky, I know, but only render, sampler and texture-stage states are supported right now" );
   }

   // Now compare to the state that this watch wants
   bool dumpStateHistory = false;

   switch( mBreakTrigger )
   {
      case BreakIfNot:
         if( curValue != mWatchedStateEvent.getEventValue() )
         {
            dumpStateHistory = true;
            //Platform::debugBreak();
         }
         
         break;
      case BreakIf:
         if( curValue == mWatchedStateEvent.getEventValue() )
         {
            dumpStateHistory = true;
            //Platform::debugBreak();
         }
         break;

      default:
         AssertFatal( false, "How did we get here?" );
   }

   // Also lame as hell...reporting...
   if( dumpStateHistory )
   {
      Con::printf( "State history dump for watch:\n\tFile: %s\n\tLine: %d\n\t%s -- %s\n\tValue Should Be: %s\n\tValue Is: %s", 
         mWatchFileName, mWatchLine, mWatchedStateEvent.getEventTypeName(), 
         mWatchedStateEvent.getStateName(), 
         mWatchedStateEvent.getEventValueString(), 
         mWatchedStateEvent.unsafeEventValueStringLookup( curValue ) );
      for( VectorPtr<StateEventRef>::iterator i = mTrackedStateEvents.begin(); i != mTrackedStateEvents.end(); i++ )
      {
         //Con::printf( )
         Con::printf( "%s :: %d -- Value set to: %s from %s", (*i)->getFileName(), (*i)->getLineNumber(), (*i)->getEventValueString(), (*i)->getPrevValueString() );
      }
      Platform::debugBreak();
   }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void GFXDebugStateManager::submitStateChange( GFXDebugStateEventType eventType, U32 state, U32 prevValue, U32 newValue, S32 textureUnit /* = -1  */ )
{
   mActiveStateChangeList.increment();
   mActiveStateChangeList.last().set( eventType, state, mGFXDevice->getLastGFXCallFileName(), 
      mGFXDevice->getLastGFXCallLine(), prevValue, newValue, textureUnit );

   // Create event ref (the watches will make their own copies in the vectors)
   StateEventRef stateEventRef( &mActiveStateChangeList, mActiveStateChangeList.size() -1 );

   // Process watches
   for( VectorPtr<GFXDebugStateWatch *>::iterator i = mActiveWatches.begin(); i != mActiveWatches.end(); i++ )
      (*i)->processStateEvent( stateEventRef );
}

//------------------------------------------------------------------------------

void GFXDebugStateManager::startFrame()
{
   // Clear out of frame state change list, and set active list to in-frame state change list
   mOutOfFrameStateChangeEvents.clear();
   mActiveStateChangeList = mFrameStateChangeEvents;

   // Add the watches for the next frame
   while( mWatchesToAddNextFrame.size() > 0 )
   {
      GFXDebugStateWatch *watch = mWatchesToAddNextFrame.last();
      mWatchesToAddNextFrame.pop_back();
      mActiveWatches.push_back( watch );
      watch->issueWatch();
   }
}

//------------------------------------------------------------------------------

void GFXDebugStateManager::endFrame()
{
   // Clear in-frame state change list and set active list to out-of-frame event list
   mFrameStateChangeEvents.clear();
   mActiveStateChangeList = mOutOfFrameStateChangeEvents;

   // Delete one-frame watches
   for( VectorPtr<GFXDebugStateWatch *>::iterator i = mActiveWatches.begin(); i != mActiveWatches.end(); i++ )
   {
      AssertFatal( (*i)->mWatchType != GFXDebugStateWatch::OneShot, "Functionality not implemented yet, sorry." );

      // If this is a no-break watch, than give the GFX Device a callback to allow
      // it to process and/or report the state history
      if( (*i)->mBreakTrigger == GFXDebugStateWatch::NoBreak )
         mGFXDevice->processStateWatchHistory( (*i) );

      // If this is a one-frame watch, delete it at end of frame
      if( (*i)->mWatchType == GFXDebugStateWatch::OneFrame )
         delete *i;
   }

   mActiveWatches.clear();
}

//------------------------------------------------------------------------------

void GFXDebugStateManager::createWatch( const GFXDebugStateEvent &watchEvent, GFXDebugStateWatch::WatchBreakTrigger breakTrigger, GFXDebugStateWatch::WatchType watchType  )
{
   // Ok this is hackey and lame. I have some ideas on how to make this not nearly
   // so lame, but I am kind of rushing implementation to use this tool to fix
   // the problem I created it for.
   for( VectorPtr<GFXDebugStateWatch *>::iterator i = mActiveWatches.begin(); i != mActiveWatches.end(); i++ )
   {
      // It would be really lame if two watches were created on the same file and the same line
      // and I'm not positive if that is even possible, even with macros
      if( watchType == GFXDebugStateWatch::Persistent &&
          (*i)->mWatchType == GFXDebugStateWatch::Persistent &&
          (*i)->mWatchLine == mGFXDevice->getLastGFXCallLine() && // Test line number first to early out of strcmp
          ( dStrcmp( (*i)->mWatchFileName, mGFXDevice->getLastGFXCallFileName() ) == 0 ) )
      {
         // Ok call processWatch here if applicable
         switch( (*i)->mBreakTrigger )
         {
            case GFXDebugStateWatch::BreakIf:
            case GFXDebugStateWatch::BreakIfNot:
               (*i)->processWatch();
               break;
         }

         // This watch already exists, and it is a persistant watch. Don't create a new one,
         // just re-submit the old one, and bail.
         mWatchesToAddNextFrame.push_back( *i );
         return;
      }
   }

   // Create the new watch
   GFXDebugStateWatch *watch = new GFXDebugStateWatch( mGFXDevice, watchEvent, breakTrigger, watchType, mGFXDevice->getLastGFXCallFileName(), mGFXDevice->getLastGFXCallLine() );

   // Figure out where to put it
   switch( breakTrigger )
   {
      // In these cases, we want the watch to be inserted into the active list
      // right away, and call issueWatch so it knows that it is starting now
      case GFXDebugStateWatch::BreakOnChangeFrom:
      case GFXDebugStateWatch::BreakOnChangeTo:
         mActiveWatches.push_back( watch );
         watch->issueWatch();
         break;

      // Rest of them should be added at the start of the next frame so they can
      // gather history
      default:
         mWatchesToAddNextFrame.push_back( watch );
         break;
   }
}

//------------------------------------------------------------------------------

GFXDebugStateManager::~GFXDebugStateManager()
{
   // Need to delete watches, but also make sure not to double delete them
   VectorPtr<GFXDebugStateWatch *> freeList;

   for( VectorPtr<GFXDebugStateWatch *>::iterator activeWatch = mActiveWatches.begin(); activeWatch != mActiveWatches.end(); activeWatch++ )
   {
      // Check the watches to add next frame list, and remove entries in that list
      // which are in this list, so when we delete the elements of that list it
      // won't be a double delete
      bool found = false;
      for( VectorPtr<GFXDebugStateWatch *>::iterator nextFrameWatch = mWatchesToAddNextFrame.begin(); nextFrameWatch != mWatchesToAddNextFrame.end(); nextFrameWatch++ )
      {
         if( *activeWatch == *nextFrameWatch )
         {
            found = true;
            break;
         }
      }

      if( !found )
         freeList.push_back( *activeWatch );
   }
   mActiveWatches.clear();

   for( VectorPtr<GFXDebugStateWatch *>::iterator nextFrameWatch = mWatchesToAddNextFrame.begin(); nextFrameWatch != mWatchesToAddNextFrame.end(); nextFrameWatch++ )
      freeList.push_back( *nextFrameWatch );

   mWatchesToAddNextFrame.clear();

   for( VectorPtr<GFXDebugStateWatch *>::iterator delWatch = freeList.begin(); delWatch != freeList.end(); delWatch++ )
      delete *delWatch;
}

//------------------------------------------------------------------------------

void GFXDebugStateManager::BreakOnChangeTo( const GFXDebugStateEventType eventType, const U32 state, const U32 value, const S32 textureUnit /* = GFXDebugStateEvent::TextureUnit_UninitVal */ )
{
   createWatch( GFXDebugStateEvent( eventType, state, mGFXDevice->getLastGFXCallFileName(), 
      mGFXDevice->getLastGFXCallLine(), 
      GFXDebugStateEvent::PrevValue_UninitVal,
      value, textureUnit ), GFXDebugStateWatch::BreakOnChangeTo, GFXDebugStateWatch::OneFrame );
}

//------------------------------------------------------------------------------

void GFXDebugStateManager::BreakOnChangeFrom( const GFXDebugStateEventType eventType, const U32 state, const U32 value, const S32 textureUnit /* = GFXDebugStateEvent::TextureUnit_UninitVal */ )
{
   createWatch( GFXDebugStateEvent( eventType, state, mGFXDevice->getLastGFXCallFileName(), 
      mGFXDevice->getLastGFXCallLine(), 
      GFXDebugStateEvent::PrevValue_UninitVal,
      value, textureUnit ), GFXDebugStateWatch::BreakOnChangeFrom, GFXDebugStateWatch::OneFrame );
}

//------------------------------------------------------------------------------

void GFXDebugStateManager::BreakIfNot( const GFXDebugStateEventType eventType, const U32 state, const U32 value, const S32 textureUnit /* = GFXDebugStateEvent::TextureUnit_UninitVal */ )
{
   createWatch( GFXDebugStateEvent( eventType, state, mGFXDevice->getLastGFXCallFileName(), 
      mGFXDevice->getLastGFXCallLine(), 
      GFXDebugStateEvent::PrevValue_UninitVal,
      value, textureUnit ), GFXDebugStateWatch::BreakIfNot, GFXDebugStateWatch::Persistent );
}

//------------------------------------------------------------------------------

void GFXDebugStateManager::BreakIf( const GFXDebugStateEventType eventType, const U32 state, const U32 value, const S32 textureUnit /* = GFXDebugStateEvent::TextureUnit_UninitVal */ )
{
   createWatch( GFXDebugStateEvent( eventType, state, mGFXDevice->getLastGFXCallFileName(), 
      mGFXDevice->getLastGFXCallLine(), 
      GFXDebugStateEvent::PrevValue_UninitVal,
      value, textureUnit ), GFXDebugStateWatch::BreakIf, GFXDebugStateWatch::Persistent );
}

//------------------------------------------------------------------------------

void GFXDebugStateManager::WatchState( const GFXDebugStateEventType eventType, const U32 state, const S32 textureUnit /* = GFXDebugStateEvent::TextureUnit_UninitVal */ )
{
   createWatch( GFXDebugStateEvent( eventType, state, mGFXDevice->getLastGFXCallFileName(), 
      mGFXDevice->getLastGFXCallLine(), 
      GFXDebugStateEvent::PrevValue_UninitVal,
      GFXDebugStateEvent::EventValue_UninitVal, textureUnit ), GFXDebugStateWatch::NoBreak, GFXDebugStateWatch::Persistent );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

}; // namespace GFXDebugState

#endif // POWER_GFX_STATE_DEBUG
