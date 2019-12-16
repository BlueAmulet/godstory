//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) Sickhead Games, LLC
//-----------------------------------------------------------------------------
#define IN_HLSL
#include "groundCover.h"

struct Frag
{
   float4 col : COLOR0;
};

Frag main(  GroundCoverPIn In,
            uniform sampler2D diffuseMap : register(S0),
            uniform sampler2D damageMask : register(S2),
            uniform PointLight lights[MAX_POINT_LIGHTS] : register(C0) )
{
   // Why the 2x?  In the brightest areas the lightmap value is 2/3s of the
   // sunlight color and in the darkest areas its 1/2 of the ambient value.
   // So the 2x ensures that the ambient doesn't get darker than it should
   // at the expense of a little over brightening in the sunlight.
   float3 diffuse = saturate( In.ambient.rgb * 2 );
   
   for( int i = 0; i < MAX_POINT_LIGHTS; i++ )
   {
      // We set lights that are disabled to a negative radius!
      if( lights[i].oneOverRadius > 0 )
      {
         // Calc the contribution of the diffuse lightmap color
         // vs this dynamic light color... 0 is the light and 1
         // is the lightmap color.   
         float scale = saturate( distance( In.osPos, lights[i].position ) * lights[i].oneOverRadius );
         
         // Now use the scale to interpolate between the two colors.
         diffuse = lerp( lights[i].color, diffuse, scale );
      }
   }
   
   // Get the final color.
   float4 color = tex2D(diffuseMap, In.texCoord) * float4( diffuse, In.ambient.a );

   // Lookup the billboard positition in the damage mask to
   // alpha out destroyed ground cover.
   //float4 damaged = tex2D( damageMask, In.terrPos );   
   //color.w *= ( 1.0f - damaged.w );
   
   Frag Out;
   Out.col = color;
   return Out;
}
