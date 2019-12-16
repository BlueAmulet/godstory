//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
struct ConnectData
{
    float2 UV    : TEXCOORD0;
};

struct Fragout
{
   float4 col : COLOR0;
};

//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
Fragout main( ConnectData IN,
              uniform sampler2D SrcSamp      : register(S0),
              uniform sampler2D GlowSamp     : register(S1),
              uniform float4    stride       : register(c0)
)
{
    Fragout OutCol = (Fragout)0;
    float4 scn = stride.x * tex2D(SrcSamp, IN.UV);
    float3 glow = stride.y * tex2D(GlowSamp, IN.UV).xyz;
    OutCol.col = float4(scn.xyz+glow,scn.z);
    return OutCol;
}