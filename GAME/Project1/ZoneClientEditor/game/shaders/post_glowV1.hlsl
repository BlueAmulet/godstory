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
    float2 UV         : TEXCOORD0;
    float4 UV1        : TEXCOORD1; 
    float4 UV2        : TEXCOORD2; 
    float4 UV3        : TEXCOORD3; 
    float4 UV4        : TEXCOORD4; 
};


//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
ConnectData main( VertData IN,
                  uniform float4   stride            : register(C0))

{
   ConnectData OUT = (ConnectData)0;

   OUT.position = IN.position;
   
   float TexelIncrement = stride.x;
   float2 Coord = float2(IN.baseTex.xy + stride.zw);
   OUT.UV = Coord;
   OUT.UV1 = float4(Coord.x + TexelIncrement, Coord.y,
		     Coord.x - TexelIncrement, Coord.y);
   TexelIncrement += TexelIncrement;
    OUT.UV2 = float4(Coord.x + TexelIncrement, Coord.y,
		     Coord.x - TexelIncrement, Coord.y);
   TexelIncrement += TexelIncrement;
    OUT.UV3 = float4(Coord.x + TexelIncrement, Coord.y,
		     Coord.x - TexelIncrement, Coord.y);
   TexelIncrement += TexelIncrement;
    OUT.UV4 = float4(Coord.x + TexelIncrement, Coord.y,
		     Coord.x - TexelIncrement, Coord.y);

   return OUT;
}