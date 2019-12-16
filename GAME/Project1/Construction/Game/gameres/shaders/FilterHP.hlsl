//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
float4 TexSize: register(c2);
//static const float newr = {1.0f/1024.0f};
float4 Point: register(c1);
float4 main( float2 Tex : TEXCOORD0, uniform sampler2D SrcSamp      : register(S0)): COLOR0
{
    float4 Color = 0;
    Color += tex2D( SrcSamp, Tex + float2(-6*TexSize.x,0) ) * 0.002216;
    Color += tex2D( SrcSamp, Tex + float2(-5*TexSize.x,0) ) *  0.008764;
Color += tex2D( SrcSamp, Tex + float2(-4*TexSize.x,0) ) * 0.026995;
Color += tex2D( SrcSamp, Tex + float2(-3*TexSize.x,0) ) * 0.064759;
Color += tex2D( SrcSamp, Tex + float2(-2*TexSize.x,0) ) * 0.120985;
Color += tex2D( SrcSamp, Tex + float2(-1*TexSize.x,0) ) *  0.176033;
Color += tex2D( SrcSamp, Tex + float2(0,0) ) * 0.199471;
Color += tex2D( SrcSamp, Tex + float2(1*TexSize.x,0) ) * 0.176033;
Color += tex2D( SrcSamp, Tex + float2(2*TexSize.x,0) ) * 0.120985;
Color += tex2D( SrcSamp, Tex + float2(3*TexSize.x,0) ) * 0.064759;
Color += tex2D( SrcSamp, Tex + float2(4*TexSize.x,0) ) * 0.026995;
Color += tex2D( SrcSamp, Tex + float2(5*TexSize.x,0) ) * 0.008764;
Color += tex2D( SrcSamp, Tex + float2(6*TexSize.x,0) ) *  0.002216;

    return saturate(Color * Point.y);
}