//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "core/tVector.h"
#include "core/tAlgorithm.h"

#ifndef _SIMOBJECTLIST_H_
#define _SIMOBJECTLIST_H_

// Forward Refs
class SimObject;

/// A vector of SimObjects.
///
/// As this inherits from VectorPtr, it has the full range of vector methods.
class SimObjectList: public VectorPtr<SimObject*>
{
   static S32 QSORT_CALLBACK compareId(const void* a,const void* b);
public:
   void pushBack(SimObject*);       ///< Add the SimObject* to the end of the list, unless it's already in the list.
   void pushBackForce(SimObject*);  ///< Add the SimObject* to the end of the list, moving it there if it's already present in the list.
   void pushFront(SimObject*);      ///< Add the SimObject* to the start of the list.
   void remove(SimObject*);         ///< Remove the SimObject* from the list; may disrupt order of the list.

   SimObject* at(S32 index) const {  if(index >= 0 && index < size()) return (*this)[index]; return NULL; }
   /// Remove the SimObject* from the list; guaranteed to preserve list order.
   void removeStable(SimObject* pObject);

   void sortId();                   ///< Sort the list by object ID.
};

#endif // _SIMOBJECTLIST_H_
