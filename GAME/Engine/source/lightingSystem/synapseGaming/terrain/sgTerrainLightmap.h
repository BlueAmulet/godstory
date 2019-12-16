//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef __SGTERRAINLIGHTMAP_H
#define __SGTERRAINLIGHTMAP_H

#ifndef _SGLIGHTMAP_H_
#include "lightingSystem/synapseGaming/sgLightMap.h"
#endif

class TerrainBlock;

/**
* Used to generate terrain light maps.
*/
class sgTerrainLightMap : public sgLightMap
{
public:
   TerrainBlock *sgTerrain;
   sgTerrainLightMap(U32 width, U32 height, TerrainBlock *terrain)
      : sgLightMap(width, height)
   {
      sgTerrain = terrain;
   }
   void sgMergeLighting(ColorF *lightmap);
   /// See: sgLightMap::sgGetBoundingBox.
   virtual void sgCalculateLighting(sgLightInfo *light);
};

#endif