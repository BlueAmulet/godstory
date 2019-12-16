//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#include "lightingSystem/synapseGaming/sgLightManager.h"
#include "lightingSystem/synapseGaming/terrain/sgTerrainLightmap.h"
#include "lightingSystem/synapseGaming/sgLightingModel.h"
#include "terrain/terrdata.h"

void sgTerrainLightMap::sgCalculateLighting(sgLightInfo *light)
{
   // setup zone info...
   bool isinzone = (light->sgZone[0] == 0) || (light->sgZone[1] == 0);

   // allow what?
   bool allowdiffuse = (!light->sgDiffuseRestrictZone) || isinzone;
   bool allowambient = (!light->sgAmbientRestrictZone) || isinzone;

   // should I bother?
   if((!allowdiffuse) && (!allowambient))
      return;


   AssertFatal((sgTerrain), "Member 'sgTerrain' must be populated.");

   // setup constants...
   F32 terrainlength = (F32)(sgTerrain->getSquareSize() * TerrainBlock::BlockSize);
   const F32 halfterrainlength = terrainlength * 0.5f;


   U32 time = Platform::getRealMilliseconds();

   Point2F s, t;
   s[0] = sgLightMapSVector[0];
   s[1] = sgLightMapSVector[1];
   t[0] = sgLightMapTVector[0];
   t[1] = sgLightMapTVector[1];
   Point2F run = t * (F32)sgWidth;

   Point2F start;
   start[0] = sgWorldPosition[0] + halfterrainlength;
   start[1] = sgWorldPosition[1] + halfterrainlength;

   sgLightingModel &model = sgLightingModelManager::sgGetLightingModel(light);   
   model.sgInitStateLM();

   Point2I lmindexmin, lmindexmax;
   if(light->mType == LightInfo::Vector)
   {
      lmindexmin.x = 0;
      lmindexmin.y = 0;
      lmindexmax.x = (sgWidth - 1);
      lmindexmax.y = (sgHeight - 1);
   }
   else
   {
      F32 maxrad = model.sgGetMaxRadius();
      Box3F worldbox = Box3F(light->mPos, light->mPos);
      worldbox.min -= Point3F(maxrad, maxrad, maxrad);
      worldbox.max += Point3F(maxrad, maxrad, maxrad);

      lmindexmin.x = (S32)((worldbox.min.x - sgWorldPosition.x) / s.x);
      lmindexmin.y = (S32)((worldbox.min.y - sgWorldPosition.y) / t.y);
      lmindexmax.x = (S32)((worldbox.max.x - sgWorldPosition.x) / s.x);
      lmindexmax.y = (S32)((worldbox.max.y - sgWorldPosition.y) / t.y);

      lmindexmin.x = getMax(lmindexmin.x, S32(0));
      lmindexmin.y = getMax(lmindexmin.y, S32(0));
      lmindexmax.x = getMin(lmindexmax.x, S32(sgWidth - 1));
      lmindexmax.y = getMin(lmindexmax.y, S32(sgHeight - 1));
   }

   S32 lmx, lmy, lmindex;
   Point3F lexelworldpos;
   Point3F normal;
   ColorF diffuse = ColorF(0.0, 0.0, 0.0);
   ColorF ambient = ColorF(0.0, 0.0, 0.0);
   Point3F lightingnormal = Point3F(0.0, 0.0, 0.0);

   Point2F point = ((t * (F32)lmindexmin.y) + start + (s * (F32)lmindexmin.x));

   for(lmy=lmindexmin.y; lmy<lmindexmax.y; lmy++)
   {
      for(lmx=lmindexmin.x; lmx<lmindexmax.x; lmx++)
      {
         lmindex = (lmx + (lmy * sgWidth));

         // get lexel 2D world pos...
         lexelworldpos[0] = point[0] - halfterrainlength;
         lexelworldpos[1] = point[1] - halfterrainlength;

         // use 2D terrain space pos to get the world space z and normal...
         sgTerrain->getNormalAndHeight(point, &normal, &lexelworldpos.z, false);

         // too often unset, must do these here...
         ambient = diffuse = ColorF(0.0f, 0.0f, 0.0f);
         lightingnormal = VectorF(0.0f, 0.0f, 0.0f);
         model.sgLightingLM(lexelworldpos, normal, diffuse, ambient, lightingnormal);

         if(allowdiffuse && ((diffuse.red > SG_MIN_LEXEL_INTENSITY) ||
            (diffuse.green > SG_MIN_LEXEL_INTENSITY) || (diffuse.blue > SG_MIN_LEXEL_INTENSITY)))
         {
            // step four: check for shadows...

            bool shadowed = false;
            RayInfo info;
            if(light->sgCastsShadows && sgLightManager::sgAllowShadows())
            {
               // set light pos for shadows...
               Point3F lightpos = light->mPos;
               if(light->mType == LightInfo::Vector)
               {
                  lightpos = SG_STATIC_LIGHT_VECTOR_DIST * light->mDirection * -1;
                  lightpos = lexelworldpos + lightpos;
               }

               // make texels terrain space coord into a world space coord...
               RayInfo info;
               if(sgTerrain->getContainer()->castRay(lightpos, (lexelworldpos + (lightingnormal * 0.5)),
                  ShadowCasterObjectType, &info))
               {
                  shadowed = true;
               }
            }

            if(!shadowed)
            {
               // step five: apply the lighting to the light map...
               sgTexels->sgData[lmindex] += diffuse;
            }
         }

         if(allowambient && ((ambient.red > 0.0f) || (ambient.green > 0.0f) || (ambient.blue > 0.0f)))
         {
            //sgTexels->sgData[lmindex] += ambient;
         }

         /*if((lmx & 0x1) == (lmy & 0x1))
         sgTexels[lmindex] += ColorF(1.0, 0.0, 0.0);
         else
         sgTexels[lmindex] += ColorF(0.0, 1.0, 0.0);*/


         // stats...
         sgStatistics::sgTerrainLexelCount++;


         point += s;
      }

      point = ((t * (F32)lmy) + start + (s * (F32)lmindexmin.x));
   }

   model.sgResetState();


   // stats...
   sgStatistics::sgTerrainLexelTime += Platform::getRealMilliseconds() - time;
}

void sgTerrainLightMap::sgMergeLighting(ColorF *lightmap)
{
   sgTexels->sgBlur();

   U32 y, x, index;
   for(y=0; y<sgHeight; y++)
   {
      for(x=0; x<sgWidth; x++)
      {
         index = (y * sgWidth) + x;
         ColorF &pixel = lightmap[index];
         pixel += sgTexels->sgData[index];
      }
   }
}
