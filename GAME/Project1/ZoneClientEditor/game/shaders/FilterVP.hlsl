//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
float4 TexSize: register(c2);
static const float newr = {1.0f/768.0f};
float4 Point: register(c1);
float4 main( float2 Tex : TEXCOORD0, uniform sampler2D SrcSamp : register(S0)): COLOR0
{
    float4 Color = 0;
    
    Color += tex2D( SrcSamp, Tex + float2(0,(-6) * newr) ) * 0.002216;
    Color += tex2D( SrcSamp, Tex + float2(0,(-5) * newr) ) *  0.008764;
    Color += tex2D( SrcSamp, Tex + float2(0,(-4) * newr) ) * 0.026995;
    Color += tex2D( SrcSamp, Tex + float2(0,(-3) * newr) ) * 0.064759;
    Color += tex2D( SrcSamp, Tex + float2(0,(-2) * newr) ) * 0.120985;
    Color += tex2D( SrcSamp, Tex + float2(0,(-1) * newr) ) *  0.176033;
    Color += tex2D( SrcSamp, Tex + float2(0,0) ) * 0.199471;
    Color += tex2D( SrcSamp, Tex + float2(0,1 * newr) ) * 0.176033;
    Color += tex2D( SrcSamp, Tex + float2(0,2 * newr) ) * 0.120985;
    Color += tex2D( SrcSamp, Tex + float2(0,3 * newr) ) * 0.064759;
    Color += tex2D( SrcSamp, Tex + float2(0,4 * newr) ) * 0.026995;
    Color += tex2D( SrcSamp, Tex + float2(0,5 * newr) ) * 0.008764;
    Color += tex2D( SrcSamp, Tex + float2(0,6 * newr) ) *  0.002216;

    return saturate(Color * Point.y);
}