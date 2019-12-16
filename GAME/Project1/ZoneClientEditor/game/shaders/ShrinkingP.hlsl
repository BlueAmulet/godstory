//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
float4 TexSize    :      register(c2);
float4 Point      :      register(c1);
float4 LightColor :      register(c3);
//static const float2 temp = {1.0f/1024.0f, 1.0f/768.0f};
float4 main(  float2 Tex : TEXCOORD0 ,uniform sampler2D SrcSamp      : register(S0)) : COLOR0
{
   float s00 = tex2D(SrcSamp, Tex + float2(-TexSize.x, -TexSize.y)).r;
   float s01 = tex2D(SrcSamp, Tex + float2( 0,   -TexSize.y)).r;
   float s02 = tex2D(SrcSamp, Tex + float2( TexSize.x, -TexSize.y)).r;

   float s10 = tex2D(SrcSamp, Tex + float2(-TexSize.x,  0)).r;
   float s12 = tex2D(SrcSamp, Tex + float2( TexSize.x,  0)).r;

   float s20 = tex2D(SrcSamp, Tex + float2(-TexSize.x, TexSize.y)).r;
   float s21 = tex2D(SrcSamp, Tex + float2( 0,    TexSize.y)).r;
   float s22 = tex2D(SrcSamp, Tex + float2( TexSize.x,  TexSize.y)).r;

   float4 srccolor = LightColor;

   float sobelX = s00 + 2 * s10 + s20 - s02 - 2 * s12 - s22;
 
   float sobelY = s00 + 2 * s01 + s02 - s20 - 2 * s21 - s22;

   float edgeSqr = (sobelX * sobelX + sobelY * sobelY);
   
   srccolor.a = edgeSqr * Point.x;
   srccolor = saturate(srccolor);
   return srccolor;
}