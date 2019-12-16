//*****************************************************************************
// TSE -- HLSL procedural shader                                               
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
struct VertData
{
   float4 position        : POSITION;
};


struct ConnectData
{
   float4 hpos            : POSITION;
};


//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
ConnectData main( VertData IN, uniform float4x4 modelview )
{
   ConnectData OUT;

   OUT.hpos = mul(modelview, IN.position);
   
   return OUT;
}
