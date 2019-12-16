//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
struct VertData
{
   float2 texCoord   : TEXCOORD0;
   float2 waveScale    : TEXCOORD1;
   float2 texCoord2    : TEXCOORD2;
   float3 texCoord3    : TEXCOORD3;
   float2 texCoord4    : TEXCOORD4;
   float3 normal     : NORMAL;
   float4 position   : POSITION;
};


struct ConnectData
{
    float4 hpos        : POSITION;
    float2 TexCoord0   : TEXCOORD0;
    float2 TexCoord1   : TEXCOORD1;
    float2 TexCoord2   : TEXCOORD2;
    float2 TexCoord3   : TEXCOORD3;
    float2 TexCoord4   : TEXCOORD4;
    float2 TexCoord5   : TEXCOORD5;
};


//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
ConnectData main( VertData IN, uniform float4 layer_speed : register(C0),
                  uniform float time_0_X : register(C1),
                  uniform float4x4 world: register(C2),
                  uniform float    GlobalSwayPhase : register(C6),
                  uniform float      SwayMagnitudeSide     : register(C7),
                  uniform float      SwayMagnitudeFront    : register(C8),
                 uniform float4x4 projection             : register(C9)
)
{
   ConnectData OUT = (ConnectData)0;

       float4x4 trans = 0;
    trans[0][0] = 1;
    trans[1][1] = 1;
    trans[2][2] = 1;
    trans[3][3] = 1;
    trans[0][3] = IN.position.x;
    trans[1][3] = IN.position.y;
    trans[2][3] = IN.position.z;
    
    float4x4 o = world;
    o = mul(o, trans);
        
    o[0][0] = 1;
    o[1][0] = 0;
    o[2][0] = 0;
    o[3][0] = 0;
    o[0][1] = 0;
    o[1][1] = 1;
    o[2][1] = 0;
    o[3][1] = 0;
    
    float xSway, ySway;
    float wavePhase = GlobalSwayPhase * IN.waveScale.x;
    sincos(wavePhase, ySway, xSway);
    xSway = xSway * IN.waveScale.y * SwayMagnitudeSide;
    ySway = ySway * IN.waveScale.y * SwayMagnitudeFront;
    float4 p;    
    p = mul(o, float4(IN.normal.x + xSway, ySway, IN.normal.z, 1));
            
    OUT.hpos = mul(projection, p);
        OUT.TexCoord0 = IN.texCoord4;

   float2 tempcoord = IN.texCoord;
   tempcoord.y += IN.texCoord3.z;

   // Base texture coordinates plus scaled time
   OUT.TexCoord1.x = tempcoord.x;
   OUT.TexCoord1.y = tempcoord.y + layer_speed.x * time_0_X;

   // Base texture coordinates plus scaled time
   OUT.TexCoord2.x = tempcoord.x;
   OUT.TexCoord2.y = tempcoord.y + layer_speed.y * time_0_X;

   // Base texture coordinates plus scaled time
   OUT.TexCoord3.x = tempcoord.x;
   OUT.TexCoord3.y = tempcoord.y + layer_speed.z * time_0_X;
   OUT.TexCoord4 = IN.texCoord2;
   OUT.TexCoord5 = IN.texCoord3;

   return OUT;
}