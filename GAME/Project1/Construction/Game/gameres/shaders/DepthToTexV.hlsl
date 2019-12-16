//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
struct VertData
{
    float4 position   : POSITION;
    float4 baseTex    : TEXCOORD0;
    float4 lmTex      : TEXCOORD1;
    float3 T          : TEXCOORD2;
    float3 B          : TEXCOORD3;
};


struct ConnectData
{
    float4 position   : POSITION;
    float4 position0  : TEXCOORD0;
};


//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
ConnectData main( VertData IN,
                  uniform float4x4 modelview : register(c0),
                  uniform float4x4 modelview1 : register(c4)
)
{
   ConnectData OUT = (ConnectData)0;

   //OUT.position = IN.position;
   OUT.position = mul(modelview, IN.position);
   OUT.position0 =mul(modelview1, IN.position);
   return OUT;
}