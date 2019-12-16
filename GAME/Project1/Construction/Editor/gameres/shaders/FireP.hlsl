//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
float distortion_amount2    : register(c0);
float distortion_amount1    : register(c2);
float distortion_amount0    : register(c3);
float4 myColor              : register(c4);
float bumpscale             : register(c5);

struct ConnectData
{
    float2 TexCoord0   : TEXCOORD0;
    float2 TexCoord1   : TEXCOORD1;
    float2 TexCoord2   : TEXCOORD2;
    float2 TexCoord3   : TEXCOORD3;
    float2 TexCoord4   : TEXCOORD4;
    float2 TexCoord5   : TEXCOORD5;
};

struct Fragout
{
   float4 col : COLOR0;
};

float4 bx2(float x)
{
   return 2.0f * x - 1.0f;
}

//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
Fragout main( ConnectData IN,
              uniform sampler2D fire_base           : register(S0),
              uniform sampler2D fire_distortion     : register(S1)
)
{
   Fragout OutCol = (Fragout)0;

   float4 noise0 = tex2D(fire_distortion, IN.TexCoord1);
   float4 noise1 = tex2D(fire_distortion, IN.TexCoord2);
   float4 noise2 = tex2D(fire_distortion, IN.TexCoord3);

   float4 noiseSum = bx2(noise0.a) * distortion_amount0 + bx2(noise1.a) * distortion_amount1 + bx2(noise2.a) * distortion_amount2;

   float2 perturbedBaseCoords = IN.TexCoord0 + bumpscale * (IN.TexCoord4.x - IN.TexCoord0.y) * noiseSum; 
   
   float4 base = tex2D(fire_base, perturbedBaseCoords);

perturbedBaseCoords = IN.TexCoord5 + bumpscale * (IN.TexCoord4.y - IN.TexCoord5.y) * noiseSum; 
   float4 opacity = tex2D(fire_base, perturbedBaseCoords);

   OutCol.col = (base + opacity) / 2;
   OutCol.col.rgb += myColor.rgb;
   OutCol.col.a += (OutCol.col.a > 0) ? (OutCol.col.a + myColor.a) : 0.0;
  
    return OutCol;
}