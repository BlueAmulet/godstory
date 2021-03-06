//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _VEHICLEBLOCKER_H_
#define _VEHICLEBLOCKER_H_

#ifndef _SCENEOBJECT_H_
#include "sceneGraph/sceneObject.h"
#endif
#ifndef _BOXCONVEX_H_
#include "collision/boxConvex.h"
#endif

//--------------------------------------------------------------------------
class VehicleBlocker : public SceneObject
{
   typedef SceneObject Parent;
   friend class VehicleBlockerConvex;

  protected:
   bool onAdd();
   void onRemove();

   // Collision
   void buildConvex(const Box3F& box, Convex* convex);
  protected:
   Convex* mConvexList;

   Point3F mDimensions;

  public:
   VehicleBlocker();
   ~VehicleBlocker();

   DECLARE_CONOBJECT(VehicleBlocker);
   static void initPersistFields();

   U64  packUpdate  (NetConnection *conn, U64 mask, BitStream *stream);
   void unpackUpdate(NetConnection *conn,           BitStream *stream);
};

#endif // _H_VEHICLEBLOCKER
