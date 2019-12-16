//*****************************************************************************
// TSE -- water shader
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
#define IN_HLSL
#include "..\shdrConsts.h"

#define UNDERWATER_DISTORTION_SCALE 0

struct ConnectData
{
   float4 texCoord   : TEXCOORD0;
   float2 texCoord2  : TEXCOORD2;
   float4 texCoord3  : TEXCOORD3;
   float2 fogCoord   : TEXCOORD4;
   float3 pos        : TEXCOORD6;
   float3 eyePos     : TEXCOORD7;
   float4 vertColor  : COLOR;
};

struct Fragout
{
   float4 col : COLOR0;
};

//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
Fragout main( ConnectData IN,
              uniform sampler    bumpMap           : register(S0),
              uniform sampler    refractBuff       : register(S1),
              uniform float3     miscParams		   : register(PC_USERDEF1)
)
{
   Fragout OUT;

                       

   float3 bumpNorm = tex2D(bumpMap, IN.texCoord.xy) * 2.0 - 1.0;
   bumpNorm += tex2D(bumpMap, IN.texCoord.zw) * 2.0 - 1.0;
   
   // This large scale texture has 1/3 the influence as the other two.
   // Its purpose is to break up the repetitive patterns of the other two textures.
   bumpNorm += (tex2D(bumpMap, IN.texCoord2) * 2.0 - 1.0) * 0.3;
   
   // calc distortion, place in projected tex coords
   float distortion = (length( IN.eyePos - IN.pos ) / 200.0) + 0.15;
   //IN.texCoord3.xy += bumpNorm.xy *  distortion;
   IN.texCoord3.xy += bumpNorm.xy * miscParams[UNDERWATER_DISTORTION_SCALE];
   
   OUT.col = tex2Dproj( refractBuff, IN.texCoord3 );
   OUT.col *= IN.vertColor;

   return OUT;
}

