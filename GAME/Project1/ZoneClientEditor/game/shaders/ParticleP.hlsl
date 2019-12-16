//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
struct ConnectData
{
   float2 outTexCoord     : TEXCOORD0;
   float4 Col             : COLOR0;
};

struct Fragout
{
   float4 col : COLOR0;
};

//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
Fragout main( ConnectData IN,
              uniform sampler2D Samp         : register(S0),
              uniform float4    Glow         : register(c0),
              uniform float     self_mult    : register(c1),
              uniform float     translucent  : register(c2)
)
{
    Fragout OutCol = (Fragout)0;
    OutCol.col = IN.Col * tex2D( Samp, IN.outTexCoord );

    OutCol.col.rgb = OutCol.col.rgb * Glow.a + Glow.rgb;

    OutCol.col.rgb = pow( OutCol.col.rgb, self_mult );
    OutCol.col.a *= translucent;
    return OutCol;
}