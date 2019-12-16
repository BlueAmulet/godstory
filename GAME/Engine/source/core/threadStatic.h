//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _POWERTHREADSTATIC_H_
#define _POWERTHREADSTATIC_H_

#include "core/tVector.h"

//-----------------------------------------------------------------------------
// PowerEngineThreadStatic Base Class
class _PowerEngineThreadStatic
{
   friend class _PowerEngineThreadStaticReg;

private:

#ifdef POWER_ENABLE_THREAD_STATIC_METRICS
   U32 mHitCount;
#endif

protected:
   static U32 mListIndex;
   virtual _PowerEngineThreadStatic *_createInstance() const = 0;

public:
   _PowerEngineThreadStatic()
#ifdef POWER_ENABLE_THREAD_STATIC_METRICS
    :  mHitCount( 0 ) 
#endif
   { };

   static const U32 getListIndex(){ return mListIndex; }

   virtual void *getMemInstPtr() = 0;
   virtual const dsize_t getMemInstSize() const = 0;

#ifdef POWER_ENABLE_THREAD_STATIC_METRICS
   _PowerEngineThreadStatic *_chainHit() { mHitCount++; return this; }
   const U32 &trackHit() { return ++mHitCount; }
   const U32 &getHitCount() const { return mHitCount; }
#endif
};
// Typedef
typedef VectorPtr<_PowerEngineThreadStatic *> PowerEngineThreadStaticList;
typedef PowerEngineThreadStaticList * PowerEngineThreadStaticListHandle;

//-----------------------------------------------------------------------------
// Auto-registration class and manager of the instances
class _PowerEngineThreadStaticReg
{
   // This will manage all of the thread static registrations
   static _PowerEngineThreadStaticReg *smFirst;
   _PowerEngineThreadStaticReg *mNext;

   // This is a vector of vectors which will store instances of thread static
   // variables. mThreadStaticInsances[0] will be the list of the initial values
   // of the statics, and then indexing for instanced versions will start at 1
   // 
   // Note that the list of instances in mThreadStaticInstances[0] does not, and
   // must not get 'delete' called on it, because all members of the list are
   // pointers to statically allocated memory. All other lists will be contain
   // pointers to dynamically allocated memory, and will need to be freed upon
   // termination.
   //
   // So this was originally a static data member, however that caused problems because
   // I was relying on static initialization order to make sure the vector got initialized
   // *before* any static instance of this class was created via macro. By wrapping the
   // static in a function, I can be assured that the static memory will get initialized
   // before it is modified.
   static Vector<PowerEngineThreadStaticList> &getThreadStaticListVector();

public:
   /// Constructor
   _PowerEngineThreadStaticReg( _PowerEngineThreadStatic *ttsInitial )
   {
      // Link this entry into the list
      mNext = smFirst;
      smFirst = this;

      // Create list 0 (initial values) if it doesn't exist
      if( getThreadStaticListVector().empty() )
         getThreadStaticListVector().increment();

      // Set the index of the thread static for lookup
      ttsInitial->mListIndex = getThreadStaticListVector()[0].size();

      // Add the static to the initial value list
      getThreadStaticListVector()[0].push_back( ttsInitial );
   }

   virtual ~_PowerEngineThreadStaticReg();

   // Accessors
   static const PowerEngineThreadStaticList &getStaticList( const U32 idx = 0 ) 
   { 
      AssertFatal( getThreadStaticListVector().size() > idx, "Out of range static list" ); 
      
      return getThreadStaticListVector()[idx]; 
   }

   static void destroyInstances();
   static void destroyInstance( PowerEngineThreadStaticList *instanceList );

   static const _PowerEngineThreadStaticReg *getFirst() { return smFirst; }

   const _PowerEngineThreadStaticReg *getNext() const { return mNext; }

   /// Spawn another copy of the ThreadStatics and pass back the id
   static PowerEngineThreadStaticListHandle spawnThreadStaticsInstance();
};

//-----------------------------------------------------------------------------
// Template class that will get used as a base for the thread statics
template<class T>
class PowerEngineThreadStatic : public _PowerEngineThreadStatic
{
   // The reg object will want access to mInstance
   friend class _PowerEngineThreadStaticReg;

private:
   T mInstance;

public:
   PowerEngineThreadStatic( T instanceVal ) : mInstance( instanceVal ) {}
   virtual void *getMemInstPtr() { return &mInstance; }

   // I am not sure these are needed, and I don't want to create confusing-to-debug code
#if 0
   // Operator overloads
   operator T*() { return &mInstance; }
   operator T*() const { return &mInstance; }
   operator const T*() const { return &mInstance; }

   bool operator ==( const T &l ) const { return mInstance == l; }
   bool operator !=( const T &l ) const { return mInstance != l; }

   T &operator =( const T &l ) { mInstance = l; return mInstance; }
#endif // if0
};

//-----------------------------------------------------------------------------
// If ThreadStatic behavior is not enabled, than the macros will resolve
// to regular, static memory
#ifndef POWER_ENABLE_THREAD_STATICS

#define DITTS( type, name, initialvalue ) static type name = initialvalue
#define ATTS( name ) name

#else // POWER_ENABLE_THREAD_STATICS is defined

//-----------------------------------------------------------------------------
// Declare PowerEngineThreadStatic, and initialize it's value
//
// This macro would be used in a .cpp file to declare a ThreadStatic
#define DITTS(type, name, initalvalue) \
class _##name##PowerEngineThreadStatic : public PowerEngineThreadStatic<type> \
{ \
protected:\
   virtual _PowerEngineThreadStatic *_createInstance() const { return new _##name##PowerEngineThreadStatic; } \
public: \
   _##name##PowerEngineThreadStatic() : PowerEngineThreadStatic<type>( initalvalue ) {} \
   virtual const dsize_t getMemInstSize() const { return sizeof( type ); } \
   type &_cast() { return *reinterpret_cast<type *>( getMemInstPtr() ); } \
   const type &_const_cast() const { return *reinterpret_cast<const type *>( getMemInstPtr() ); } \
}; \
static _##name##PowerEngineThreadStatic name##PowerEngineThreadStatic; \
static _PowerEngineThreadStaticReg _##name##TTSReg( reinterpret_cast<_PowerEngineThreadStatic *>( & name##PowerEngineThreadStatic ) )

//-----------------------------------------------------------------------------
// Access PowerEngineThreadStatic

// NOTE: TEMPDEF is there as a temporary place holder for however we want to get the index of the currently running
// thread or whatever.
#define TEMPDEF 0

#ifdef POWER_ENABLE_THREAD_STATIC_METRICS
// Access counting macro
#  define ATTS_(name, idx) \
   (reinterpret_cast< _##name##PowerEngineThreadStatic *>( _PowerEngineThreadStaticReg::getStaticList( idx )[ _##name##PowerEngineThreadStatic::getListIndex() ]->_chainHit() )->_cast() )
// Const access counting macro
#  define CATTS_(name, idx) \
   (reinterpret_cast< _##name##PowerEngineThreadStatic *>( _PowerEngineThreadStaticReg::getStaticList( idx )[ _##name##PowerEngineThreadStatic::getListIndex() ]->_chainHit() )->_const_cast() )
#else
// Regular access macro
#  define ATTS_(name, idx) \
   (reinterpret_cast< _##name##PowerEngineThreadStatic *>( _PowerEngineThreadStaticReg::getStaticList( idx )[ _##name##PowerEngineThreadStatic::getListIndex() ] )->_cast() )
// Const access macro
#  define CATTS_(name, idx) \
   (reinterpret_cast< _##name##PowerEngineThreadStatic *>( _PowerEngineThreadStaticReg::getStaticList( idx )[ _##name##PowerEngineThreadStatic::getListIndex() ] )->_const_cast() )
#endif // POWER_ENABLE_THREAD_STATIC_METRICS

#define ATTS(name) ATTS_(name, TEMPDEF)
#define CATTS(name) CATTS_(name, TEMPDEF)

#endif // POWER_ENABLE_THREAD_STATICS

#endif