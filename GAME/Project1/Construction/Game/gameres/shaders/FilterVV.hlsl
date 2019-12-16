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
//static const  float2 temp = {1.0f/1024.0f, 1.0f/768.0f};
float4 main(  float2 Tex : TEXCOORD0 ,uniform sampler2D SrcSamp      : register(S0)) : COLOR0
{
     float4 Color = 0;
     float2 teccd = TexSize * 1.5f;
     float2 teccd1 = TexSize * 0.5f;

           Color += tex2D( SrcSamp, Tex + float2(teccd.x,  -teccd.y) );
Color += tex2D( SrcSamp, Tex + float2(teccd.x,  -teccd1.y) );
Color += tex2D( SrcSamp, Tex + float2(teccd.x,   teccd1.y) );
          Color += tex2D( SrcSamp, Tex + float2(teccd.x,   teccd.y) );
Color += tex2D( SrcSamp, Tex + float2(teccd1.x,  -teccd.y) );
Color += tex2D( SrcSamp, Tex + float2(teccd1.x,  -teccd1.y) );
          Color += tex2D( SrcSamp, Tex + float2(teccd1.x,   teccd1.y) );
Color += tex2D( SrcSamp, Tex + float2(teccd1.x,   teccd.y) );
Color += tex2D( SrcSamp, Tex + float2(-teccd1.x,  -teccd.y) );
          Color += tex2D( SrcSamp, Tex + float2(-teccd1.x,  -teccd1.y) );
Color += tex2D( SrcSamp, Tex + float2(-teccd1.x,   teccd1.y) );
Color += tex2D( SrcSamp, Tex + float2(-teccd1.x,   teccd.y) );
          Color += tex2D( SrcSamp, Tex + float2(-teccd.x,  -teccd.y) );
Color += tex2D( SrcSamp, Tex + float2(-teccd.x,  -teccd1.y) );
Color += tex2D( SrcSamp, Tex + float2(-teccd.x,   teccd1.y) );
Color += tex2D( SrcSamp, Tex + float2(-teccd.x,   teccd.y) );

    return saturate(Color / 16);
}