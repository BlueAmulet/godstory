struct VertData
{
   float4 position        : POSITION;
   float2 texCoord        : TEXCOORD0;
};


struct ConnectData
{
   float4 hpos            : POSITION;
   float2 outTexCoord     : TEXCOORD0;
};

//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
ConnectData main( VertData IN )
{
   ConnectData OUT;

   OUT.hpos = IN.position;
   OUT.outTexCoord = IN.texCoord;
   return OUT;
}