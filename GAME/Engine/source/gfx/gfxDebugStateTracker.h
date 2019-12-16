//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFX_DEBUG_STATE_TRACKER_H_
#define _GFX_DEBUG_STATE_TRACKER_H_

#ifdef POWER_GFX_STATE_DEBUG

#include "core/tVector.h"
#include "gfx/gfxStringEnumTranslate.h"

class GFXDevice; // Forward decl

//------------------------------------------------------------------------------
namespace GFXDebugState
{

// Note, not all of these event types are used [5/21/2007 Pat]
enum GFXDebugStateEventType
{
   GFXDebugStateEvent_FIRST = 0,
   GFXNoStateEvent = 0, 
   GFXRenderStateEvent,
   GFXSamplerStateEvent,
   GFXTextureStageStateEvent,
   GFXShaderStateEvent,       // Not used, intended use: pixel/vertex shaders
   GFXDataStateEvent,         // Not used, intended use: texture/stream data
   GFXDebugStateEvent_COUNT
};

extern const char *GFXDebugStateEventTypeNames[GFXDebugStateEvent_COUNT];

//------------------------------------------------------------------------------

/// A storage/helper class to store events for processing and tracking
class GFXDebugStateEvent
{
private:
   /// @name State event data members
   /// @{
   GFXDebugStateEventType mEventType;     ///< Type of event
   S32 mState;                            ///< This value is related to mEventType and represents the state being altered in the event
   const char *mFileName;                 ///< The name of the C++ source file that the event was submitted from
   S32 mLineNumber;                       ///< The line number of the source file that the event was submitted from
   S32 mPrevValue;                        ///< The value of the state prior to this event
   S32 mEventValue;                       ///< The value of the state that this event is setting
   S32 mTextureUnit;                      ///< The texture unit this state change is specific to (applies for texture-stage/sampler states, currently)
   /// @}

   /// @name Helper data members
   /// @{
   U8 mEventMask;                         ///< An internally generated mask which allows this class to be used as a filter, as well as storage
   const char **mStateEnumLookup;         ///< A pointer to the lookup-table for the name of the state represented by mState
   StringValueLookupFn mStateValueLookupFn;///< A pointer to the function which can be used to look up string-values for that state
   /// @}
   
public:
   /// @name Constants
   /// These are used to provide consistent values for uninitialized state-event
   /// data members. The values of state-event data members are compared to these
   /// values when the event-mask is getting created.
   /// @{
   static const GFXDebugStateEventType EventType_UninitVal = GFXNoStateEvent;
   static const S32 State_UninitVal = -1;
   static const S32 FileName_UninitVal = 0; // This is an S32 instead of a const char * because static const values need to be integral
   static const S32 LineNumber_UninitVal = -1;
   static const S32 PrevValue_UninitVal = -1;
   static const S32 EventValue_UninitVal = -1;
   static const S32 TextureUnit_UninitVal = -1;
   /// @}

public:
   /// If a mask bit is set, than it 
   enum EventMaskBits
   {
      EventType = BIT( 0 ),
      State = BIT( 1 ),
      FileName = BIT( 2 ),
      LineNumber = BIT( 3 ),
      PrevValue = BIT( 4 ),
      EventValue = BIT( 5 ),
      TextureUnit = BIT( 6 ),

      // If adding more mask bits, be sure that a) mEventMask is big enough, 
      // and b) UninitMask is proper. Right now, bits 0-6 are mask values, and
      // if bit 7 is set, that means the mask has not been processed
      Uninit = BIT( 7 ),
      UninitMask = 0xFF, 
   };
private:
   
   /// This method will construct an event mask which will allow the object to be
   /// used in the GFXDebugStateWatch class. 
   void _buildEventMask()
   {
      // Clear mask
      mEventMask = 0;

#define _EVT_MASK_HELPER( var ) if( m##var != var##_UninitVal ) { mEventMask |= var; }
      _EVT_MASK_HELPER( EventType );
      _EVT_MASK_HELPER( State );
      _EVT_MASK_HELPER( FileName );
      _EVT_MASK_HELPER( LineNumber );
      _EVT_MASK_HELPER( PrevValue );
      _EVT_MASK_HELPER( EventValue );
      _EVT_MASK_HELPER( TextureUnit );
#undef _EVT_MASK_HELPER

      // Set up the string lookup tables for the output's sake
      switch( mEventType )
      {
         case GFXRenderStateEvent:
            mStateEnumLookup = GFXStringRenderState;
            mStateValueLookupFn = GFXStringRenderStateValueLookup[mState];
            break;
         case GFXSamplerStateEvent:
            mStateEnumLookup = GFXStringSamplerState;
            mStateValueLookupFn = GFXStringSamplerStateValueLookup[mState];
            break;
         case GFXTextureStageStateEvent:
            mStateEnumLookup = GFXStringTextureStageState;
            mStateValueLookupFn = GFXStringTextureStageStateValueLookup[mState];
            break;
      }
   }

public:
   /// Constructor
   GFXDebugStateEvent( GFXDebugStateEventType eventType = EventType_UninitVal,
                       S32 state = State_UninitVal,
                       const char *fileName = FileName_UninitVal,
                       S32 lineNumber = LineNumber_UninitVal,
                       S32 prevValue = PrevValue_UninitVal,
                       S32 eventValue = EventValue_UninitVal,
                       S32 textureUnit = TextureUnit_UninitVal ) : 
                       mEventType( eventType ), mState( state ), 
                       mFileName( fileName ), mLineNumber( lineNumber ),
                       mPrevValue( prevValue ), mEventValue( eventValue ),
                       mTextureUnit( textureUnit ), mEventMask( UninitMask ),
                       mStateEnumLookup( NULL ), mStateValueLookupFn( NULL )
   {
      // Build Event mask
      _buildEventMask();
   }

   /// Copy constructor with masking behavior so you can do something like
   /// set a watch on the last state that got set (to find out when it changes next)
   /// and mask out things like the file name, line number, or whatever else you want
#define _MASK_CPY_CTOR_HELPER( var ) m##var( copyMask & var ? copy.m##var : var##_UninitVal )
   GFXDebugStateEvent( const GFXDebugStateEvent &copy, U8 copyMask = UninitMask ) :
      _MASK_CPY_CTOR_HELPER( EventType ), _MASK_CPY_CTOR_HELPER( State ), 
      _MASK_CPY_CTOR_HELPER( FileName ), _MASK_CPY_CTOR_HELPER( LineNumber ),
      _MASK_CPY_CTOR_HELPER( PrevValue ), _MASK_CPY_CTOR_HELPER( EventValue ),
      _MASK_CPY_CTOR_HELPER( TextureUnit ),
#undef _MASK_CPY_CTOR_HELPER
      mStateEnumLookup( copy.mStateEnumLookup ), mStateValueLookupFn( copy.mStateValueLookupFn )
   {
      // Build new event mask since copy may have been masked
      _buildEventMask();
   }


   /// Destructor
   /// Unlink this from any lists it is a member of
   virtual ~GFXDebugStateEvent()
   {
      // TODO: Lists if needed
   }

   void set( GFXDebugStateEventType eventType = EventType_UninitVal,
               S32 state = State_UninitVal,
               const char *fileName = FileName_UninitVal,
               S32 lineNumber = LineNumber_UninitVal,
               S32 prevValue = PrevValue_UninitVal,
               S32 eventValue = EventValue_UninitVal,
               S32 textureUnit = TextureUnit_UninitVal )
   {
      mEventType = eventType;
      mState = state;
      mFileName = fileName;
      mLineNumber = lineNumber;
      mPrevValue = prevValue;
      mEventValue = eventValue;
      mTextureUnit = textureUnit;

      _buildEventMask();
   }

   /// @name Accessors
   /// @{
   const GFXDebugStateEventType getEventType() const { return mEventType; }
   const S32 getState() const { return mState; }
   const char *getFileName() const { return mFileName; }
   const S32 getLineNumber() const { return mLineNumber; }
   const S32 getPrevValue() const { return mPrevValue; }
   const S32 getEventValue() const { return mEventValue; }
   const S32 getTextureUnit() const { return mTextureUnit; }
   
   const U8 getEventMask() const { return mEventMask; }
   /// @}

   /// @name String accessors
   /// These are used to get string versions of the values in this event. It
   /// should be noted that there is no bounds-checking done on these array
   /// accesses. Because this code is so debug-build focused, I don't see this
   /// as a problem, but it is worth mentioning. 
   /// @{
   const char *getEventTypeName() const { return GFXDebugStateEventTypeNames[mEventType]; }
   const char *getStateName() const { AssertFatal( mStateEnumLookup != NULL, "State-enum lookup table never set." ); return mStateEnumLookup[mState]; }

   const char *getPrevValueString() const { AssertFatal( mStateValueLookupFn != NULL, "State value lookup function pointer never set." ); return mStateValueLookupFn( mPrevValue ); }
   const char *getEventValueString() const { AssertFatal( mStateValueLookupFn != NULL, "State value lookup function pointer never set." ); return mStateValueLookupFn( mEventValue ); }

   const char *unsafeEventValueStringLookup( const U32 value ) const { AssertFatal( mStateValueLookupFn != NULL, "State value lookup function pointer never set." ); return mStateValueLookupFn( value ); }
   /// @}

}; // class GFXDebugStateChangeEvent

//------------------------------------------------------------------------------

/// This class was created because a vector is not the ideal data storage structure
/// for what I am using it for. I did not want to have multiple copies of state
/// events getting created, so I, instead, push_back all state events into a vector.
/// When that vector needs to be re-sized, a pointer to an entry in that vector
/// may become invalid. This gets around that problem.
class StateEventRef
{
private:
   Vector<GFXDebugStateEvent> *mVector;   ///< Pointer to a vector which has an entry that this reference represents
   U32 mIndex;                            ///< Index of the entry this reference represents

public:
   StateEventRef( Vector<GFXDebugStateEvent> *vector = NULL, U32 idx = 0 ) : mVector( vector ), mIndex( idx ) {};
   StateEventRef( const StateEventRef &cpy ) : mVector( cpy.mVector ), mIndex( cpy.mIndex ) {};

   GFXDebugStateEvent *operator->() const { AssertFatal( mVector != NULL, "StateEventRef never set up properly." ); return &(*mVector)[mIndex]; }
   operator GFXDebugStateEvent &() { AssertFatal( mVector != NULL, "StateEventRef never set up properly." ); return (*mVector)[mIndex]; }
   StateEventRef &operator=( const StateEventRef &cpy ) { mVector = cpy.mVector; mIndex = cpy.mIndex; return *this; }
};

//------------------------------------------------------------------------------

/// Internally used class to represent the different types of watches which can
/// be created with the debug state tracker
class GFXDebugStateWatch
{
   friend class GFXDebugStateManager;

   // CodeReview: Should this be sub-classed or should these triggers and types
   // be kept in here? [5/17/2007 Pat]
public:
   // The break trigger for the watch. 
   enum WatchBreakTrigger
   {
      NoBreak = 0,         ///< This watch is simply observing values and should not break execution
      BreakOnChangeFrom,   ///< Break into the debugger when values change from the masked values in the watch state event
      BreakOnChangeTo,     ///< Break into the debugger when the values are changed to masked values in the watch state event
      BreakIfNot,          ///< Break into the debugger if the masked values are not the ones specified at watch insertion time
      BreakIf,             ///< Break into the debugger if the masked values equal the ones specified at watch insertion time
   };

   enum WatchType
   {
      OneShot = 0,         ///< This watch gets inserted by code and should be removed after check. An example would be a 'BreakIfNot' watch to catch stray states.
      OneFrame,            ///< This watch should stay around the entire frame. An example would be a 'BreakOnChangeFrom' watch.
      Persistent,          ///< This watch should not get removed by the manager. A 'NoBreak' watch would be an example of this.
   };

private:
   GFXDevice *mGFXDevice;
   GFXDebugStateEvent mWatchedStateEvent; ///< This uses the GFXDebugStateEvent class to store data it is watching for, and the mask bits resulting from that data
   WatchBreakTrigger mBreakTrigger;
   
   WatchType mWatchType; 


   // CodeReview: Do we need these? [5/17/2007 Pat]
   const char *mWatchFileName;            ///< File name this watch is declared at
   S32 mWatchLine;                        ///< Line in the file this watch is declared at

   Vector<StateEventRef> mTrackedStateEvents;
   
public:
   /// Constructor
   GFXDebugStateWatch( GFXDevice *gfxDevice, const GFXDebugStateEvent &watchEvent, WatchBreakTrigger breakTrigger, 
                       WatchType watchType, const char *fileName = NULL, S32 line = -1 ) :
                       mGFXDevice( gfxDevice ), mWatchedStateEvent( watchEvent ), mBreakTrigger( breakTrigger ), 
                       mWatchType( watchType ), mWatchFileName( fileName ), mWatchLine( line )
   {
      // Nothing now
   }
   
   /// Accessor methods so reporting can be done by the GFXDevice
   /// @{

   /// Get the event history for the watched state
   const Vector<StateEventRef> *getTrackedStateEventList() const { return &mTrackedStateEvents; }

   /// Get the break trigger of the watch
   /// @see WatchBreakTrigger
   WatchBreakTrigger getBreakTrigger() const { return mBreakTrigger; }

   /// Get the state event this watch is using to compare and filter data with
   const GFXDebugStateEvent &getWatchedStateEvent() const { return mWatchedStateEvent; }

   /// Get the type of watch this is
   /// @see WatchType
   WatchType getWatchType() const { return mWatchType; }

   /// Get the file name of the file where this watch was requested
   const char *getWatchFileName() const { return mWatchFileName; }

   /// Get the line number in the file that this watch was requested on
   S32 getWatchLine() const { return mWatchLine; }
   /// @}

   // These methods will be used by the manager class to notify active watches
   // of events that they may be interested in. Protected because they should be
   // called only by the manager.
protected:
   /// Process a state event 
   virtual void processStateEvent( const StateEventRef &stateEvent );

   /// This occurs when the watch is activated by the manager and will start getting
   /// sent state events to process. This is when the tracked state event list is
   /// cleared by this implementation
   virtual void issueWatch();

   /// This occurs whenever the watch needs to do processing to determine an action
   /// to take (break, no-break etc)
   virtual void processWatch();
   
}; // class GFXDebugStateWatchBase

//------------------------------------------------------------------------------

// one per GFXDevice instance
class GFXDebugStateManager
{
private:
   // CodeReview: For now, I am going to process queries by just traversing 
   // this list, however I would like to have a better storage structure,
   // at some point. [5/17/2007 Pat]
   Vector<GFXDebugStateEvent> mFrameStateChangeEvents; ///< Order not modified, so last() is the last state change event that occurred this frame
   Vector<GFXDebugStateEvent> mOutOfFrameStateChangeEvents; // Same as above, only this enumerates state changes which occur outside of start/end frame
   Vector<GFXDebugStateEvent> &mActiveStateChangeList;
   
   /// Watch list. Once a watch is submitted the manager takes ownership of the 
   /// memory. When the watch is set for deletion, it will be deleted by the manager.
   VectorPtr<GFXDebugStateWatch *> mActiveWatches;

   /// Some watch types want to know the history of a state throughout the frame,
   /// so adding them exactly when they are submitted is not really applicable.
   /// this list contains watches to add at the start of the next frame.
   VectorPtr<GFXDebugStateWatch *> mWatchesToAddNextFrame;

   GFXDevice *mGFXDevice; ///< Pointer to the GFXDevice this instance is attached to

   // Changing this to an internal method and making accessors to make it easier
   // to understand and use this whole thing[5/21/2007 Pat]
   void createWatch( const GFXDebugStateEvent &watchEvent, GFXDebugStateWatch::WatchBreakTrigger breakTrigger, GFXDebugStateWatch::WatchType watchType );
   // CodeReview: Return a watch handle so that NoBreak watches (among others)
   // can be queried for what their story is for a GUI display or something. 
   // [5/17/2007 Pat]

public:
   /// A single instance of a GFXDebugStateManager should exist on each GFXDevice
   /// and state events will be sent to that instance via submitStateChange. The
   /// GFXDebugStateManager queries the device it is attached to for values in
   /// the state cache.
   ///
   /// @param  device   The GFXDevice this state manager is attached to. 
   GFXDebugStateManager( GFXDevice *device ) : mGFXDevice( device ), mActiveStateChangeList( mOutOfFrameStateChangeEvents )
   {
      GFXStringEnumTranslate::init();
   }
   
   /// Destructor
   /// This will delete the watches which were created with this state debugger.
   ~GFXDebugStateManager();

   /// @name GFX Interface
   /// These methods are called from GFX Device to inform the state debug tool
   /// about events occurring on the device. 
   /// @{

   /// This method is called at the beginning of a frame. This method signals the
   /// GFXDebugStateManager to submit watches which need to be started at the
   /// beginning of a frame (Such as BreakIfNot) so that the event histories of
   /// states can be observed.
   void startFrame();

   /// This method signals the end of a frame. Watches are removed from the active
   /// watch list, and deleted if they are not persistent watches. 
   void endFrame();

   /// This method notifies the state debugger of changes in the GFX device state
   /// which are relevant. When the GFXDebugStateManager gets a call to this method
   /// it will create a GFXDebugStateEvent, store it in the state-history for the frame
   /// and pass a reference to all active watches so that they can ignore it or
   /// track it, as needed.
   ///
   /// @param  eventType   The type of event being sent
   /// @param  state       The state which is changing. This will depend on eventType,
   ///                     for example, if the eventType is a render state change
   ///                     than the state argument should be a value in GFXRenderState
   /// @param  prevValue   The previous value of this state
   /// @param  newValue    The new value of this state
   /// @param  textureUnit If the state change event is particular to a texture unit, this argument will be non-negative
   void submitStateChange( const GFXDebugStateEventType eventType, const U32 state, const U32 prevValue, const U32 newValue, const S32 textureUnit = GFXDebugStateEvent::TextureUnit_UninitVal );

   /// @}

   /// @name Debugging Interface
   /// These methods are used by the programmer to aid in state debugging. 
   /// @{

   /// This method will create a watch which becomes active immediately, and lasts
   /// until 'endFrame' is called. It will break program execution if a state
   /// change event of 'eventType' on 'state' and 'textureUnit' where the new 
   /// value is 'value'. 
   void BreakOnChangeTo( const GFXDebugStateEventType eventType, const U32 state, const U32 value, const S32 textureUnit = GFXDebugStateEvent::TextureUnit_UninitVal );

   /// This method will create a watch which becomes active immediately, and lasts
   /// until 'endFrame' is called. It will break program execution if a state
   /// change event of 'eventType' on 'state' and 'textureUnit' where the new
   /// value is not 'value'
   void BreakOnChangeFrom( const GFXDebugStateEventType eventType, const U32 state, const U32 value, const S32 textureUnit = GFXDebugStateEvent::TextureUnit_UninitVal );

   /// This method creates a watch which will become active upon the next call to
   /// startFrame. Execution will proceed normally until it reaches this call again.
   /// Upon reaching this call again, the manager will query it's attached GFXDevice
   /// for the value of 'state' as it relates to 'eventType' on 'textureUnit'. Execution
   /// break if the queried value does not match 'value'. It will then re-submit
   /// the same watch so that it is added at the start of the next frame. Note that
   /// a _new_ watch is not created by this method, the old one is simply re-submitted.
   void BreakIfNot( const GFXDebugStateEventType eventType, const U32 state, const U32 value, const S32 textureUnit = GFXDebugStateEvent::TextureUnit_UninitVal );

   /// This method creates a watch which will become active upon the next call to
   /// startFrame. Execution will proceed normally until it reaches this call again.
   /// Upon reaching this call again, the manager will query it's attached GFXDevice
   /// for the value of 'state' as it relates to 'eventType' on 'textureUnit'. Execution
   /// break if the queried value matches 'value'. It will then re-submit
   /// the same watch so that it is added at the start of the next frame. Note that
   /// a _new_ watch is not created by this method, the old one is simply re-submitted.
   void BreakIf( const GFXDebugStateEventType eventType, const U32 state, const U32 value, const S32 textureUnit = GFXDebugStateEvent::TextureUnit_UninitVal );

   /// This method creates a watch which will become active at the start of the 
   /// next frame, and simply monitor the state changes made. Watches created
   /// with this method will get passed to the GFXDevice at the end of every
   /// frame.
   void WatchState( const GFXDebugStateEventType eventType, const U32 state, const S32 textureUnit = GFXDebugStateEvent::TextureUnit_UninitVal );

   /// @}
};

//------------------------------------------------------------------------------

}; // namespace GFXDebugState

#endif // POWER_GFX_STATE_DEBUG

#endif // _GFX_DEBUG_STATE_TRACKER_H_