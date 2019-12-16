// lzw add: shore surf mask shader

#define IN_HLSL
#include "..\shdrConsts.h"


struct ConnectData
{
    float3 texCoord : TEXCOORD0; // z 透明度
    float2 fogCoord : TEXCOORD1;
};

struct Fragout
{
   float4 col : COLOR0;
};

Fragout main( ConnectData IN,
              uniform sampler baseMap : register(S0),
              uniform sampler fogMap  : register(S1)
)
{
   Fragout OUT;   

   OUT.col = tex2D( baseMap, IN.texCoord.xy );
   OUT.col.rgb = float3( 0.0f, 0.0f, 0.0f );
   OUT.col.a *= IN.texCoord.z;
   OUT.col.a *= ( 1.0f - tex2D( fogMap, IN.fogCoord ).a ); // 背景已有雾化时, 透明物体不能重复雾化, 选择直接淡出即可

   return OUT;
}

