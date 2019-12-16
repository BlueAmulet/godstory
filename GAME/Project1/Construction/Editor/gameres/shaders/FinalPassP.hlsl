//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
float4 Point: register(c1);
float  Point1: register(c3);
float4 main( float2 Tex : TEXCOORD0,uniform sampler2D SrcSamp: register(S0), uniform sampler2D SrcSamp1:register(S1)): COLOR0
{
     float4 ColorOrig = tex2D( SrcSamp, Tex*Point1 );

    float4 ColorOrig2 = tex2D( SrcSamp1, Tex );

    ColorOrig.rgb = (ColorOrig2.rgb * Point.w) + Point.z * ColorOrig.rgb;
    //ColorOrig += ColorOrig2;
    ColorOrig.a = 1.0;
    return ColorOrig;
}