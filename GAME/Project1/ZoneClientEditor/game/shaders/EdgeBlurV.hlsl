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
    float4 Col        : COLOR0;
    float4 lmTex      : TEXCOORD1;
    float3 T          : TEXCOORD2;
    float3 B          : TEXCOORD3;
};


struct ConnectData
{
    float4 position   : POSITION;
    float4  tex       : TEXCOORD0;
    float4  tex1      : TEXCOORD1;
    float4 Col        : COLOR0;
};


//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
ConnectData main( VertData IN,
                  uniform float4x4 modelview : register(c0)
)
{
   ConnectData OUT = (ConnectData)0;
   OUT.position = mul(modelview, IN.position);
   OUT.tex1.x = OUT.position.x/OUT.position.w * 0.5 + 0.5;
   OUT.tex1.y = (-OUT.position.y/OUT.position.w) * 0.5 + 0.5;
   OUT.tex = IN.baseTex;
   OUT.Col = IN.Col;
   return OUT;
}