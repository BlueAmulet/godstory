//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _TSDECAL_H_
#define _TSDECAL_H_

#ifndef _TSMESH_H_
#include "ts/tsMesh.h"
#endif

/// Decals!  The lovely detailing thingies, e.g. bullet hole marks.
/// DEPRECATED: This class is here for compatibility with old files only.
/// Performs no actual rendering.
class TSDecalMesh
{
public:

   /// The mesh that we are decaling
   TSMesh * targetMesh;

   /// @name Topology
   /// @{
   ToolVector<TSDrawPrimitive> primitives;
   ToolVector<U16> indices;
   /// @}

   /// @name Render Data
   /// indexed by decal frame...
   /// @{
   ToolVector<S32> startPrimitive;
   ToolVector<Point4F> texgenS;
   ToolVector<Point4F> texgenT;
   /// @}

   /// We only allow 1 material per decal...
   S32 materialIndex;

   /// DEPRECATED
   // void render(S32 frame, S32 decalFrame, TSMaterialList *);

   void disassemble();
   void assemble(bool skip);
};


#endif

