//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "core/bitStream.h"
#include "app/game.h"
#include "math/mMath.h"
#include "console/simBase.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "sim/netConnection.h"
#include "T3D/item.h"
#include "collision/boxConvex.h"
#include "collision/earlyOutPolyList.h"
#include "collision/extrudedPolyList.h"
#include "math/mathIO.h"
#include "sceneGraph/lightInfo.h"
#include "sceneGraph/lightManager.h"
//----------------------------------------------------------------------------

const U32 sClientCollisionMask = (AtlasObjectType       |  TerrainObjectType     |
                                  InteriorObjectType    |  StaticShapeObjectType |
                                  VehicleObjectType     |  GameObjectType        | 
                                  StaticTSObjectType);

const U32 sServerCollisionMask = (sClientCollisionMask |
                                  TriggerObjectType);


//----------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(Item);

Item::Item()
{
   mTypeMask |= ItemObjectType;
   mItemMask = 0;
}

