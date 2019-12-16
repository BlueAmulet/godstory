//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _ITEM_H_
#define _ITEM_H_

#ifndef _SHAPEBASE_H_
#include "T3D/shapeBase.h"
#endif
#ifndef _BOXCONVEX_H_
#include "collision/boxConvex.h"
#endif

//----------------------------------------------------------------------------

struct ItemData: public ShapeBaseData {
   typedef ShapeBaseData Parent;
};


//----------------------------------------------------------------------------

class Item: public ShapeBase
{
   typedef ShapeBase Parent;

  protected:
   U32 mItemMask;

  public:
   DECLARE_CONOBJECT(Item);

   Item();
   U32 getItemMask() { return mItemMask;}
};

#endif
