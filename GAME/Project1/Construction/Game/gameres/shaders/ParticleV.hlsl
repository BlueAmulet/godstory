//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
struct VertData
{
   float2 texCoord        : TEXCOORD0;
   float4 position        : POSITION;
   float4 Col             : COLOR0;
};


struct ConnectData
{
   float4 hpos            : POSITION;
   float2 outTexCoord     : TEXCOORD0;
   float4 Col             : COLOR0;
};


//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
ConnectData main( VertData IN,
                  uniform float4x4 modelview       : register(C0)
)
{
   ConnectData OUT;

   OUT.hpos = mul(modelview, IN.position);
   OUT.outTexCoord = IN.texCoord;
   OUT.Col = IN.Col;
   return OUT;
}