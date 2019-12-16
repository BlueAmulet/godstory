//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PROCESSLIST_H_
#define _PROCESSLIST_H_

#include "platform/platform.h"
#include "console/sim.h"

//----------------------------------------------------------------------------

#define TickMs      32
#define TickSec     (F32(TickMs) / 1000.0f)

//----------------------------------------------------------------------------

class ProcessObject
{
   friend class ProcessList;
   friend class ProcessList;
   friend class ClientProcessList;
   friend class ServerProcessList;

public:

   ProcessObject() { mProcessTag = 0; mProcessLink.next=mProcessLink.prev=this; mOrderGUID=0; mDeleteInNextTick = false;}
   virtual ProcessObject * getAfterObject() { return NULL; }
   inline void safeDeleteObject() { mDeleteInNextTick = true;}

protected:

   struct Link
   {
      ProcessObject *next;
      ProcessObject *prev;
   };

   // Processing interface
   void plUnlink();
   void plLinkAfter(ProcessObject*);
   void plLinkBefore(ProcessObject*);
   void plJoin(ProcessObject*);

   U32 mProcessTag;                       // Tag used during sort
   U32 mOrderGUID;                        // UID for keeping order synced (e.g., across network or runs of sim)
   Link mProcessLink;                     // Ordered process queue

   // safe delete
   bool mDeleteInNextTick;
};

//----------------------------------------------------------------------------

/// List of ProcessObjects.
class ProcessList
{

public:
   ProcessList();

   void markDirty()  { mDirty = true; }
   bool isDirty()  { return mDirty; }

   virtual void addObject(ProcessObject * obj);

   SimTime getLastTime() { return mLastTime; }
   F32 getLastDelta() { return mLastDelta; }
   F32 getLastInterpDelta() { return mLastDelta / F32(TickMs); }
   U32 getTotalTicks() { return mTotalTicks; }
   void dumpToConsole();

   /// @name Advancing Time
   /// The advance time functions return true if a tick was processed.
   /// @{

   bool advanceTime(SimTime timeDelta);

protected:

   ProcessObject mHead;

   U32 mCurrentTag;
   bool mDirty;

   U32 mTotalTicks;
   SimTime mLastTick;
   SimTime mLastTime;
   F32 mLastDelta;

   void orderList();
   virtual void advanceObjects();
   virtual void onAdvanceObjects() { advanceObjects(); }
   virtual void onTickObject(ProcessObject *) {}
};

#endif