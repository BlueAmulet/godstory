//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _OBJECTTYPES_H_
#define _OBJECTTYPES_H_

#include "platform/types.h"

/// Types used for SimObject type masks (SimObject::mTypeMask)
///
/// @note If a new object type is added, don't forget to add it to the
///      consoleInit function in main.cpp.
enum SimObjectTypes
{
   /// @name Types used by the SceneObject class
   /// @{

   ///
   DefaultObjectType =           0,
   StaticObjectType =            BIT(0),

   /// @}

   /// @name Basic Engine Types
   /// @{

   ///
   EnvironmentObjectType =       BIT(1),
   TerrainObjectType =           BIT(2),
   InteriorObjectType =          BIT(3),
   WaterObjectType =             BIT(4),
   TriggerObjectType =           BIT(5),
   MarkerObjectType =            BIT(6),
   AtlasObjectType =             BIT(7),
   // Should we decrement the numbers below?
   DecalManagerObjectType =      BIT(9),

   /// @}

   /// @name Game Types
   /// @{

   GameBaseObjectType =          BIT(10),
   DecalObjectType		=		 BIT(11),
   ShapeBaseObjectType =         BIT(12),
   CameraObjectType =            BIT(13),
   StaticShapeObjectType =       BIT(14),
   GameObjectType =              BIT(15),
   ItemObjectType =              BIT(16),
   VehicleObjectType =           BIT(17),
   VehicleBlockerObjectType =    BIT(18),
   ProjectileObjectType =        BIT(19),
   ExplosionObjectType  =        BIT(20),
   CorpseObjectType =            BIT(21),
   DebrisObjectType =            BIT(22),
   PhysicalZoneObjectType =      BIT(23),
   StaticTSObjectType =          BIT(24),
   AIObjectType =                BIT(25),
   StaticRenderedObjectType =    BIT(26),
   /// @}

   /// @name Other
   /// The following are allowed types that can be set on datablocks for static shapes
   /// @{

   ///
   DamagableItemObjectType =     BIT(27),
   /// @}

   ShadowCasterObjectType =      BIT(28),
   PlayerStepObjectType =        BIT(29),
   SpellObjectType =             BIT(30),
};

enum GameObjectTypes
{
   PlayerObjectType =            BIT(0),
   NpcObjectType =               BIT(1),
   PetObjectType =               BIT(2),
   TotemObjectType =             BIT(3),
   MountObjectType =             BIT(4),
};

enum ItemTypes
{
	PrizeBoxType =               BIT(0),
	CollectionObjectType =       BIT(1),
};

#define STATIC_COLLISION_MASK   (   AtlasObjectType    | TerrainObjectType |  \
                                    InteriorObjectType | StaticObjectType  ) 

#define DAMAGEABLE_MASK  ( GameObjectType          |  \
                           VehicleObjectType       |  \
                           DamagableItemObjectType ) 

#endif
