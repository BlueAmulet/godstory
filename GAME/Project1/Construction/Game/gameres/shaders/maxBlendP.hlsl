//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
struct ConnectData
{
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
              uniform sampler2D diffuseMap      : register(S0),
              uniform float4    Color           : register(C9),
	      uniform float     visibility      : register(C6)
)
{
   Fragout OUT;
   float4 color1;
   color1 = tex2D(diffuseMap, IN.texCoord);
   color1.a *= visibility;
   float4 color2 = float4(1.0,1.0,1.0,1.0)-Color;
   OUT.col = color2 * saturate(1.0 - color1.a) + Color;
   OUT.col.a = 1.0;
   return OUT;
}