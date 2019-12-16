//*****************************************************************************
// TSE -- water shader                                               
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
#define IN_HLSL
#include "..\shdrConsts.h"

struct VertData
{
   float4 position        : POSITION;
   float4 vertColor		  : COLOR;
};


struct ConnectData
{
   float4 hpos             : POSITION;
   float4 texCoord         : TEXCOORD0;
   float2 texCoord2        : TEXCOORD2;
   float4 texCoord3        : TEXCOORD3;
   float3 pos              : TEXCOORD6;
   float3 eyePos           : TEXCOORD7;
   float4 vertColor        : COLOR;
};


//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
ConnectData main( VertData IN,
                  uniform float4x4 modelview       : register(C0),
                  uniform float3x3 cubeTrans       : register(VC_CUBE_TRANS),
                  uniform float3   cubeEyePos      : register(VC_CUBE_EYE_POS),
                  uniform float3   eyePos          : register(VC_EYE_POS),
                  uniform float3   lightVec        : register(VC_LIGHT_DIR1),
                  uniform float3   fogData         : register(VC_FOGDATA),
                  uniform float4x4 objTrans        : register(VC_OBJ_TRANS),
                  uniform float4   waveData[4]     : register(C50),
                  uniform float4   timeData        : register(C54),
                  uniform float2   waveTexScale[4] : register(C55),
                  uniform float    reflectTexSize  : register(C59)
)
{
   ConnectData OUT;
   OUT.hpos = mul(modelview, IN.position);

   // set up tex coordinates for the 3 interacting normal maps
   OUT.texCoord.xy = IN.position.xy * waveTexScale[0];
   OUT.texCoord.xy += waveData[0].xy * timeData[0];

   OUT.texCoord.zw = IN.position.xy * waveTexScale[1];
   OUT.texCoord.zw += waveData[1].xy * timeData[1];

   OUT.texCoord2.xy = IN.position.xy * waveTexScale[2];
   OUT.texCoord2.xy += waveData[2].xy * timeData[2];

   // send misc data to pixel shaders
   OUT.pos = IN.position;
   OUT.eyePos = eyePos;
   
   // use projection matrix for reflection / refraction texture coords
   float4x4 texGen = { 0.5,  0.0,  0.0,  0.5 + 0.5 / reflectTexSize,
                       0.0, -0.5,  0.0,  0.5 + 1.0 / reflectTexSize,
                       0.0,  0.0,  1.0,  0.0,
                       0.0,  0.0,  0.0,  1.0 };

   OUT.texCoord3 = mul( texGen, OUT.hpos );


   
   OUT.vertColor = IN.vertColor;
   
   return OUT;

}

