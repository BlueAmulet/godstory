//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
struct VertData
{
    float4 position   : POSITION;
    float4 baseTex    : TEXCOORD0;
    float4 lmTex      : TEXCOORD1;
    float3 T          : TEXCOORD2;
    float3 B          : TEXCOORD3;
};


struct ConnectData
{
    float4 position   : POSITION;
    float4  tex       : TEXCOORD0;
    float4  tex1      : TEXCOORD1;
    float4  tex2      : TEXCOORD2;
    float4  tex3      : TEXCOORD3;
};


//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
ConnectData main( VertData IN,
                  uniform float4x4 modelview : register(c0),
                 uniform float4 layer_speed : register(c4),
               uniform float time_0_X : register(c5)
)
{
   ConnectData OUT = (ConnectData)0;

   //OUT.position = IN.position;
   OUT.position = mul(modelview, IN.position);
   OUT.tex.x = IN.baseTex.x + layer_speed.x * time_0_X;
   OUT.tex.y = IN.baseTex.y + layer_speed.y * time_0_X;
   OUT.tex.z = IN.baseTex.x + 0.33 + (-layer_speed.x) * time_0_X;
   OUT.tex.w = IN.baseTex.y + 0.33 + (-layer_speed.y) * time_0_X;
   OUT.tex1.x = IN.baseTex.x + 0.66 + (-layer_speed.x) * time_0_X;
   OUT.tex1.y = IN.baseTex.y + 0.66 + layer_speed.y * time_0_X;
   OUT.tex1.z = IN.baseTex.x + 0.9 + layer_speed.x * time_0_X;
   OUT.tex1.w = IN.baseTex.y + 0.9 + (-layer_speed.y) * time_0_X;
   OUT.tex2 = IN.position;
    OUT.tex3 = OUT.position;
   return OUT;
}