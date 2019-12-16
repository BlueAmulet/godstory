//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright © Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

//-----------------------------------------------------------------------------
// Data 
//-----------------------------------------------------------------------------
struct v2f
{
	float4 HPOS : POSITION;
	float2 baseTex    : TEXCOORD0;
};

struct Fragout
{
	float4 col : COLOR0;
};

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------
Fragout main(v2f IN,
			 uniform sampler2D diffuseMap : register(S0))
{
	Fragout OUT;
	
	OUT.col = tex2D(diffuseMap, IN.baseTex);
	OUT.col.rgb = 1.0f;

	return OUT;
}
