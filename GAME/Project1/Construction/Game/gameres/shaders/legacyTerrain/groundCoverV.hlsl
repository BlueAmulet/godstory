//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) Sickhead Games, LLC
//-----------------------------------------------------------------------------
#define IN_HLSL
#include "groundCover.h"

GroundCoverVOut main( Vert In, 
            uniform float4x4 modelView : register(VC_WORLD_PROJ),
            
	         	         
	         uniform float3 camRight : register(C6),	         
	         //uniform float3 camDir   : register(C7),	         
	         uniform float3 camUp    : register(C8),
	         uniform float3 camPos   : register(C9),

	         uniform float2 fadeParams : register(C10),
	         
	         uniform float3 terrainInfo : register(C11),
	           
	         uniform WindData windData : register(C12),
	         uniform WindEmitter emitters[MAX_WIND_EMITTERS] : register(C40),

	         uniform float4 typeRects[MAX_COVERTYPES] : register(C60) )	         
{

   // Pull some of the parameters for clarity.     
   float    fadeStart      = fadeParams.x;
   float    fadeEnd        = fadeParams.y;
   const float fadeRange   = fadeEnd - fadeStart;

   //float    maxFadeJitter  = ( fadeEnd - fadeStart ) / 2.0f;    
   int      corner      = ( In.ambient.a * 255.0f ) + 0.5f;
   float2   size        = In.params.xy;  
   int      typeIndex   = In.params.z;
           
   float2   terrainPos  = terrainInfo.xy;
   float    terrainSize = terrainInfo.z;    
   
   // The billboarding is based on the camera direction.
   float3 rightVec   = camRight * sCornerRight[corner];
   float3 upVec      = camUp * sCornerUp[corner];               

   // Figure out the corner position.
   float4 outPoint;
   outPoint.xyz = ( upVec * size.y ) + ( rightVec * size.x );
   float len = length( outPoint.xyz );
   
   // We derive the billboard phase used for wind calculations from its position.
   float bbPhase = dot( In.position.xyz, 1 );

   // Get the overall wind gust and turbulence effects.
   float3 wind;
   wind.xy = windEffect( bbPhase, windData );
   wind.z = 0;
   
   // Process the wind emitters!
   for ( int i=0; i < MAX_WIND_EMITTERS; i++ )
   {
      // We set disabled spheres to negative radii!
      const float oneOverRadius = emitters[i].placement.w;
      if ( oneOverRadius > 0 )      
      {
         // Whats the vector between the point and the sphere?
         //
         // Note that by working from the input position we're
         // not effecting the tops of the billboards.  We could
         // probably improve this to use the billboarded position,
         // but we must consider how to do this without
         //
         float3 delta = In.position.xyz - emitters[i].placement.xyz;

         // Now calculate the distance.                    
         float dist = length( delta );
                              
         // Scale the distance to 0 to 1... 0 being the center
         // and 1 being outside the sphere.
         float influence = 1.0 - clamp( dist * oneOverRadius, 0, 1.0 );

         // Pull out some constants.
         const float strength       = emitters[i].effect.x;
         const float turbFrequency  = emitters[i].effect.y;
         const float turbStrength   = emitters[i].effect.z;

         // Calculate the turbulance effect.
         //
         // TODO: This is expensive... how can we fake turbulency
         // in a deterministic way without calculating the full
         // wave accumulation per sphere?
         //
         // Maybe we should do turbulency once outside the loop
         // by averaging the frequency and strength of each sphere?
         //
         float turbulence = windTurbulence( bbPhase, turbFrequency, turbStrength );

         // Get the vector to the center of the sphere scaled by the
         // influence amount... this is how far we push this corner point.
         wind += ( ( delta / dist ) * ( strength + turbulence ) ) * influence;
      }
   }

   // Add the summed wind effect into the point.
   outPoint.xyz += wind.xyz * In.params.w;

   // Do a simple spherical clamp to keep the foliage
   // from stretching too much by wind effect.
   outPoint.xyz = normalize( outPoint.xyz ) * len;

   // Move the point into world space.
   outPoint.xyz += In.position.xyz;
   outPoint.w = 1;

   // Grab the uv set and setup the texture coord.
   float4 uvSet = typeRects[typeIndex]; 
   float2 texCoord;
   texCoord.x = uvSet.x + ( uvSet.z * sUVCornerExtent[corner].x );
   texCoord.y = uvSet.y + ( uvSet.w * sUVCornerExtent[corner].y );

   // Get the alpha fade value.
   float dist = distance( camPos, outPoint.xyz ) - fadeStart;
   float alpha = 1 - clamp( dist / fadeRange, 0, 1 );

   // Setup the shader output data.
   GroundCoverVOut Out;              
   Out.position = mul( modelView, outPoint );
   Out.texCoord = texCoord;
   Out.ambient = float4( In.ambient.rgb, alpha );
   Out.osPos = float4( outPoint.xyz, 1.0 );

   // Set texture space terrain position.
   Out.terrPos = ( In.position.xy - terrainPos ) / terrainSize;
   
   return Out;
}

