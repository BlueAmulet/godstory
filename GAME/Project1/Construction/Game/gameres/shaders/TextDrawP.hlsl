//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
struct ConnectData
{
   float4 color           : COLOR;
   float2 texCoord        : TEXCOORD0; 
};


struct Fragout
{
   float4 col : COLOR0;
};


//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
Fragout main( ConnectData IN,
              uniform sampler2D diffuseMap      : register(S0)
)
{
   Fragout OUT;
   OUT.col = tex2D(diffuseMap, IN.texCoord);
   OUT.col.rgb += IN.color.rgb;
   OUT.col.a += (OUT.col.a > 0) ? 0.1 : 0.0;
   return OUT;
}