//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"

#include "console/sim.h"
#include "console/simObject.h"
#include "console/simObjectList.h"

void SimObjectList::pushBack(SimObject* obj)
{
   if (find(begin(),end(),obj) == end())
      push_back(obj);
}

void SimObjectList::pushBackForce(SimObject* obj)
{
   iterator itr = find(begin(),end(),obj);
   if (itr == end())
   {
      push_back(obj);
   }
   else
   {
      // Move to the back...
      //
      SimObject* pBack = *itr;
      removeStable(pBack);
      push_back(pBack);
   }
}

void SimObjectList::pushFront(SimObject* obj)
{
   if (find(begin(),end(),obj) == end())
      push_front(obj);
}

void SimObjectList::remove(SimObject* obj)
{
   iterator ptr = find(begin(),end(),obj);
   if (ptr != end())
      erase(ptr);
}

void SimObjectList::removeStable(SimObject* obj)
{
   iterator ptr = find(begin(),end(),obj);
   if (ptr != end())
      erase(ptr);
}

S32 QSORT_CALLBACK SimObjectList::compareId(const void* a,const void* b)
{
   return (*reinterpret_cast<const SimObject* const*>(a))->getId() -
      (*reinterpret_cast<const SimObject* const*>(b))->getId();
}

void SimObjectList::sortId()
{
   dQsort(address(),size(),sizeof(value_type),compareId);
}
