//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
struct ConnectData
{
    float4  tex       : TEXCOORD0;
    float4  tex1      : TEXCOORD1;
    float4  Col        : COLOR0;
};

struct Fragout
{
   float4 col : COLOR0;
};

//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
Fragout main( ConnectData IN,
              uniform sampler2D Tex0           : register(S0),
               uniform sampler2D Back          : register(S1)
)
{
   Fragout OutCol = (Fragout)0;
   float4 color = tex2D( Tex0, IN.tex.xy );
   OutCol.col = IN.Col * color;
   OutCol.col = OutCol.col * color.a + tex2D(Back, IN.tex1.xy);
   return OutCol;
 }