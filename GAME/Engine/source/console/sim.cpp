//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"

#include "console/sim.h"
#include "console/simEvents.h"
#include "console/simObject.h"
#include "console/simSet.h"

namespace Sim
{
   // Don't forget to InstantiateNamed* in simManager.cc - DMM
   ImplementNamedSet(ActiveActionMapSet)
   ImplementNamedSet(GhostAlwaysSet)
   ImplementNamedSet(LightSet)
   ImplementNamedSet(WayPointSet)
   ImplementNamedSet(fxReplicatorSet)
   ImplementNamedSet(fxFoliageSet)
   ImplementNamedSet(BehaviorSet)
   ImplementNamedSet(reflectiveSet)
   ImplementNamedSet(MaterialSet)
   ImplementNamedSet(SFXSourceSet)
   ImplementNamedGroup(ActionMapGroup)
   ImplementNamedGroup(ClientGroup)
   ImplementNamedGroup(GuiGroup)
   ImplementNamedGroup(GuiDataGroup)
   ImplementNamedGroup(TCPGroup)

   //groups created on the client
   ImplementNamedGroup(ClientConnectionGroup)
   ImplementNamedGroup(ChunkFileGroup)
   ImplementNamedSet(sgMissionLightingFilterSet)
}

//-----------------------------------------------------------------------------
// Console Functions
//-----------------------------------------------------------------------------

ConsoleFunctionGroupBegin ( SimFunctions, "Functions relating to Sim.");

ConsoleFunction(nameToID, S32, 2, 2, "nameToID(object)")
{
   argc;
   SimObject *obj = Sim::findObject(argv[1]);
   if(obj)
      return obj->getId();
   else
      return -1;
}

ConsoleFunction(isObject, bool, 2, 2, "isObject(object)")
{
   argc;
   if (!dStrcmp(argv[1], "0") || !dStrcmp(argv[1], ""))
      return false;
   else
      return (Sim::findObject(argv[1]) != NULL);
}

ConsoleFunction(cancel,void,2,2,"cancel(eventId)")
{
   argc;
   Sim::cancelEvent(dAtoi(argv[1]));
}

ConsoleFunction(isEventPending, bool, 2, 2, "isEventPending(%scheduleId);")
{
   argc;
   return Sim::isEventPending(dAtoi(argv[1]));
}

ConsoleFunction(getEventTimeLeft, S32, 2, 2, "getEventTimeLeft(scheduleId) Get the time left in ms until this event will trigger.")
{
   return Sim::getEventTimeLeft(dAtoi(argv[1]));
}

ConsoleFunction(getScheduleDuration, S32, 2, 2, "getScheduleDuration(%scheduleId);")
{
   argc;   S32 ret = Sim::getScheduleDuration(dAtoi(argv[1]));
   return ret;
}

ConsoleFunction(getTimeSinceStart, S32, 2, 2, "getTimeSinceStart(%scheduleId);")
{
   argc;   S32 ret = Sim::getTimeSinceStart(dAtoi(argv[1]));
   return ret;
}

ConsoleFunction(schedule, S32, 4, 0, "schedule(time, refobject|0, command, <arg1...argN>)")
{
   U32 timeDelta = U32(dAtof(argv[1]));
   SimObject *refObject = Sim::findObject(argv[2]);
   if(!refObject)
   {
      if(argv[2][0] != '0')
         return 0;

      refObject = Sim::getRootGroup();
   }
   SimConsoleEvent *evt = new SimConsoleEvent(argc - 3, argv + 3, false);

   S32 ret = Sim::postEvent(refObject, evt, Sim::getCurrentTime() + timeDelta);
   // #ifdef DEBUG
   //    Con::printf("ref %s schedule(%s) = %d", argv[2], argv[3], ret);
   //    Con::executef( "backtrace");
   // #endif
   return ret;
}

ConsoleFunctionGroupEnd( SimFunctions );
