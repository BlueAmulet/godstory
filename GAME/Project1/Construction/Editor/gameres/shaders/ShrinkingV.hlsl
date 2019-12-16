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
    float2 TexCoord0  : TEXCOORD0;
    //float2 TexCoord1  : TEXCOORD1;
};


//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
ConnectData main( VertData IN
)
{
   ConnectData OUT = (ConnectData)0;
    OUT.position = IN.position;
    OUT.TexCoord0 = IN.baseTex.xy;   
   //OUT.TexCoord1 = IN.lmTex.xy;  
    return OUT;
}