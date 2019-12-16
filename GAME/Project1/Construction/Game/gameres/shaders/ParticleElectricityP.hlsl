//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
float glowStrength: register(c2);
float height: register(c3);
float glowFallOff: register(c4);
float speed: register(c5);
float sampleDist: register(c6);
float ambientGlow: register(c7);
float ambientGlowHeightScale: register(c8);
float vertNoise: register(c9);
float time_0_X: register(c0);
float translucent  : register(c1);
sampler Noise: register(s0);

struct ConnectData
{
    float2 TexCoord   : TEXCOORD0;
    float4 Col        : COLOR0;
};

struct Fragout
{
     float4 col : COLOR0;
};

//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
Fragout main( ConnectData IN
)
{
    Fragout OutCol = (Fragout)0;

    float time_1_X = time_0_X + (0.299f * IN.Col.r) + (0.587f * IN.Col.g) + (0.114f * IN.Col.b);

    float2 t = float2(speed * time_1_X * 0.5871 - vertNoise * abs(IN.TexCoord.y), speed * time_1_X);

   float xs0 = IN.TexCoord.x - sampleDist;
   float xs1 = IN.TexCoord.x;
   float xs2 = IN.TexCoord.x + sampleDist;

   float noise0 = tex3D(Noise, float3(xs0, t)).r;
   float noise1 = tex3D(Noise, float3(xs1, t)).r;
   float noise2 = tex3D(Noise, float3(xs2, t)).r;

   float mid0 = height * (noise0 * 2 - 1) * (1 - xs0 * xs0);
   float mid1 = height * (noise1 * 2 - 1) * (1 - xs1 * xs1);
   float mid2 = height * (noise2 * 2 - 1) * (1 - xs2 * xs2);

   float dist0 = abs(IN.TexCoord.y - mid0);
   float dist1 = abs(IN.TexCoord.y - mid1);
   float dist2 = abs(IN.TexCoord.y - mid2);

   float glow = 1.0 - pow(0.25 * (dist0 + 2 * dist1 + dist2), glowFallOff);

   float ambGlow = ambientGlow * (1 - xs1 * xs1) * (1 - abs(ambientGlowHeightScale * IN.TexCoord.y));

   OutCol.col = (glowStrength * glow * glow + ambGlow) * IN.
Col;

   OutCol.col.a = (abs(IN.TexCoord.x) > 0.9) ? (1-  abs(IN.TexCoord.x))* 10.0 * OutCol.col.a : OutCol.col.a;
   OutCol.col.a = (abs(IN.TexCoord.y) > 0.9) ? (1-  abs(IN.TexCoord.y))* 10.0 * OutCol.col.a : OutCol.col.a;
   OutCol.col.a *= translucent;

    return OutCol;
}