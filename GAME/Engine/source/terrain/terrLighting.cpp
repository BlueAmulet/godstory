//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "terrain/terrData.h"
#include "math/mMath.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "gfx/gBitmap.h"
#include "terrain/terrRender.h"

U32 TerrainRender::testSquareLights(GridSquare *sq, S32 level, Point2I pos, U32 lightMask)
{
   U32 retMask = 0;
   F32 blockX = pos.x * mSquareSize + mBlockPos.x;
   F32 blockY = pos.y * mSquareSize + mBlockPos.y;
   F32 blockZ = fixedToFloat(sq->minHeight);

   F32 blockSize = F32(mSquareSize * (1 << level));
   F32 blockHeight = fixedToFloat(sq->maxHeight - sq->minHeight);

   Point3F vec;

   for(S32 i = 0; (lightMask >> i) != 0; i++)
   {

      if(lightMask & (1 << i))
      {
         Point3F *pos = &mTerrainLights[i].pos;
         // test the visibility of this light to box
         // find closest point on box to light and test

         if(pos->z < blockZ)
            vec.z = blockZ - pos->z;
         else if(pos->z > blockZ + blockHeight)
            vec.z = pos->z - (blockZ + blockHeight);
         else
            vec.z = 0;

         if(pos->x < blockX)
            vec.x = blockX - pos->x;
         else if(pos->x > blockX + blockSize)
            vec.x = pos->x - (blockX + blockSize);
         else
            vec.x = 0;

         if(pos->y < blockY)
            vec.y = blockY - pos->y;
         else if(pos->y > blockY + blockSize)
            vec.y = pos->y - (blockY + blockSize);
         else
            vec.y = 0;
         F32 dist = vec.len();
         if(dist < mTerrainLights[i].radius)
            retMask |= (1 << i);
      }
   }
   return retMask;
}

void TerrainRender::buildLightArray()
{
   mDynamicLightCount = 0;
   if ((mTerrainLighting == NULL) || (!TerrainRender::mEnableTerrainDynLights))
      return;

   static LightInfoList lights;
   lights.clear();   

   gClientSceneGraph->getLightManager()->getBestLights(lights);
   // create terrain lights from these...
   U32 curIndex = 0;
   for(U32 i = 0; i < lights.size(); i++)
   {
      LightInfo* light = lights[i];
      if((light->mType != LightInfo::Point) && (light->mType != LightInfo::Spot))
         continue;

      // set the 'fo
      TerrLightInfo & info = mTerrainLights[curIndex++];
      mCurrentBlock->getWorldTransform().mulP(light->mPos, &info.pos);
      info.radius = light->getRadius();
      info.light = light;
   }

   mDynamicLightCount = curIndex;
}


