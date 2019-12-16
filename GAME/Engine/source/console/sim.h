//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SIM_H_
#define _SIM_H_

// Forward Refs
class SimSet;
class SimGroup;
class SimDataBlockGroup;
class SimObject;
class SimEvent;
class Stream;

// Sim Types
typedef U32 SimTime;
typedef U32 SimObjectId;

/// Definition of some basic Sim system constants.
///
/// These constants define the range of ids assigned to datablocks
/// (DataBlockObjectIdFirst - DataBlockObjectIdLast), and the number
/// of bits used to store datablock IDs.
///
/// Normal Sim objects are given the range of IDs starting at
/// DynamicObjectIdFirst and going to infinity. Sim objects use
/// a SimObjectId to represent their ID; this is currently a U32.
///
/// The RootGroupId is assigned to gRootGroup, in which most SimObjects
/// are addded as child members. See simManager.cc for details, particularly
/// Sim::initRoot() and following.
enum SimObjectsConstants
{
   DataBlockObjectIdFirst = 3,
   DataBlockObjectIdBitSize = 12,
   DataBlockObjectIdLast = DataBlockObjectIdFirst + (1 << DataBlockObjectIdBitSize) - 1,

   MessageObjectIdFirst = DataBlockObjectIdLast + 1,
   MessageObjectIdBitSize = 6,
   MessageObjectIdLast = MessageObjectIdFirst + (1 << MessageObjectIdBitSize) - 1,

   DynamicObjectIdFirst = MessageObjectIdLast + 1,
   InvalidEventId = 0,
   RootGroupId = 0xFFFFFFFF,
};

//---------------------------------------------------------------------------

/// @defgroup simbase_helpermacros Helper Macros
///
/// These are used for named sets and groups in the manager.
/// @{
#define DeclareNamedSet(set) extern SimSet *g##set;inline SimSet *get##set() { return g##set; }
#define DeclareNamedGroup(set) extern SimGroup *g##set;inline SimGroup *get##set() { return g##set; }
#define ImplementNamedSet(set) SimSet *g##set;
#define ImplementNamedGroup(set) SimGroup *g##set;
/// @}

//---------------------------------------------------------------------------

namespace Sim
{
   DeclareNamedSet(ActiveActionMapSet)
   DeclareNamedSet(GhostAlwaysSet)
   DeclareNamedSet(LightSet)
   DeclareNamedSet(WayPointSet)
   DeclareNamedSet(fxReplicatorSet)
   DeclareNamedSet(fxFoliageSet)
   DeclareNamedSet(reflectiveSet)
   DeclareNamedSet(BehaviorSet)
   DeclareNamedSet(MaterialSet)
   DeclareNamedSet(SFXSourceSet);
   DeclareNamedGroup(ActionMapGroup)
   DeclareNamedGroup(ClientGroup)
   DeclareNamedGroup(GuiGroup)
   DeclareNamedGroup(GuiDataGroup)
   DeclareNamedGroup(TCPGroup)
   DeclareNamedGroup(ClientConnectionGroup)
   DeclareNamedGroup(ChunkFileGroup);

   DeclareNamedSet(sgMissionLightingFilterSet);

   void init();
   void shutdown();

   SimDataBlockGroup *getUserDataBlockGroup();
   SimDataBlockGroup *getDataBlockGroup();
   SimGroup* getRootGroup();

   SimObject* findObject(SimObjectId);
   SimObject* findObject(const char* name);
   template<class T> inline bool findObject(SimObjectId iD,T*&t)
   {
      t = dynamic_cast<T*>(findObject(iD));
      return t != NULL;
   }
   template<class T> inline bool findObject(const char *objectName,T*&t)
   {
      t = dynamic_cast<T*>(findObject(objectName));
      return t != NULL;
   }

   void advanceToTime(SimTime time);
   void advanceTime(SimTime delta);
   SimTime getCurrentTime();
   SimTime getTargetTime();

   /// a target time of 0 on an event means current event
   U32 postEvent(SimObject*, SimEvent*, U32 targetTime);

   inline U32 postEvent(SimObjectId iD,SimEvent*evt, U32 targetTime)
   {
      return postEvent(findObject(iD), evt, targetTime);
   }
   inline U32 postEvent(const char *objectName,SimEvent*evt, U32 targetTime)
   {
      return postEvent(findObject(objectName), evt, targetTime);
   }
   inline U32 postCurrentEvent(SimObject*obj, SimEvent*evt)
   {
      return postEvent(obj,evt,getCurrentTime());
   }
   inline U32 postCurrentEvent(SimObjectId obj,SimEvent*evt)
   {
      return postEvent(obj,evt,getCurrentTime());
   }
   inline U32 postCurrentEvent(const char *obj,SimEvent*evt)
   {
      return postEvent(obj,evt,getCurrentTime());
   }

   void cancelEvent(U32 eventId);
   bool isEventPending(U32 eventId);
   U32  getEventTimeLeft(U32 eventId);
   U32  getTimeSinceStart(U32 eventId);
   U32  getScheduleDuration(U32 eventId);

   bool saveObject(SimObject *obj, Stream *stream);
   SimObject *loadObjectStream(Stream *stream);

   bool saveObject(SimObject *obj, const char *filename);
   SimObject *loadObjectStream(const char *filename);
}

//----------------------------------------------------------------------------

#define DECLARE_CONSOLETYPE(T) \
   DefineConsoleType( Type##T##Ptr, T * )

#define IMPLEMENT_CONSOLETYPE(T) \
   DatablockConsoleType( T##Ptr, Type##T##Ptr, sizeof(T*), T )

#define IMPLEMENT_SETDATATYPE(T) \
   ConsoleSetType( Type##T##Ptr ) \
{                                                                                                 \
   volatile SimDataBlock* pConstraint = static_cast<SimDataBlock*>((T*)NULL);                     \
   pConstraint;                                                                                   \
   if (argc == 1) {                                                                               \
   *reinterpret_cast<T**>(dptr) = NULL;                                                        \
   if (argv[0] && argv[0][0] && !Sim::findObject(argv[0],*reinterpret_cast<T**>(dptr)))        \
   Con::printf("Object '%s' is not a member of the '%s' data block class", argv[0], #T);    \
   }                                                                                              \
      else                                                                                           \
      Con::printf("Cannot set multiple args to a single pointer.");                               \
}

#define IMPLEMENT_GETDATATYPE(T) \
   ConsoleGetType( Type##T##Ptr ) \
{                                                                                   \
   volatile SimDataBlock* pConstraint = static_cast<SimDataBlock*>((T*)NULL);       \
   pConstraint;                                                                     \
   T** obj = reinterpret_cast<T**>(dptr);                                           \
   char* returnBuffer = Con::getReturnBuffer(128);                                   \
   dSprintf(returnBuffer, 128, "%s", *obj && (*obj)->getName() ? (*obj)->getName() : "");\
   return returnBuffer;                                                             \
}

#endif // _SIM_H_
