//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright © Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
#define IN_HLSL
#include "../shdrConsts.h"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
struct Appdata
{
   float2 texCoord        : TEXCOORD0;
   float2 lmCoord         : TEXCOORD1;
   float2 inTex0          : TEXCOORD2;
   float2 inTex1          : TEXCOORD3;
   float2 inTex2          : TEXCOORD4;
   float2 inTex3          : TEXCOORD5;
   float4 T4              : TEXCOORD6;
   float3 N               : TEXCOORD7;
   float3 org_normal      : NORMAL;
   float4 org_position    : POSITION;
};

struct Conn
{
	float4 HPOS : POSITION;
	float2 baseTex    : TEXCOORD0;
};

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------
Conn main(Appdata IN,
	uniform float4x4 lightspace       : register(C0),
	uniform float4   projectioninfo   : register(C4),
  uniform sampler2D vertexMap       : register(S0),
  uniform float    boneCount       : register(C48))
{
	Conn Out;
	
	 float4 position = IN.org_position;
   float4x4 boneMx;
   if ( boneCount>0 ) {
		position = float4(0,0,0,0);
		if(IN.inTex0.y>0){
			boneMx[0] = tex2Dlod(vertexMap, float4( (IN.inTex0.x + 0.5)/boneCount,0.5,0,0) );
			boneMx[1] = tex2Dlod(vertexMap, float4( (IN.inTex0.x + 1.5)/boneCount,0.5,0,0) );
			boneMx[2] = tex2Dlod(vertexMap, float4( (IN.inTex0.x + 2.5)/boneCount,0.5,0,0) );
			boneMx[3] = tex2Dlod(vertexMap, float4( (IN.inTex0.x + 3.5)/boneCount,0.5,0,0) );
			position += mul(boneMx, IN.org_position) * IN.inTex0.y;
		}

		if(IN.inTex1.y>0){
			boneMx[0] = tex2Dlod(vertexMap, float4( (IN.inTex1.x + 0.5)/boneCount,0.5,0,0) );
			boneMx[1] = tex2Dlod(vertexMap, float4( (IN.inTex1.x + 1.5)/boneCount,0.5,0,0) );
			boneMx[2] = tex2Dlod(vertexMap, float4( (IN.inTex1.x + 2.5)/boneCount,0.5,0,0) );
			boneMx[3] = tex2Dlod(vertexMap, float4( (IN.inTex1.x + 3.5)/boneCount,0.5,0,0) );
			position += mul(boneMx, IN.org_position) * IN.inTex1.y;
		}

		if(IN.inTex2.y>0){
			boneMx[0] = tex2Dlod(vertexMap, float4( (IN.inTex2.x + 0.5)/boneCount,0.5,0,0) );
			boneMx[1] = tex2Dlod(vertexMap, float4( (IN.inTex2.x + 1.5)/boneCount,0.5,0,0) );
			boneMx[2] = tex2Dlod(vertexMap, float4( (IN.inTex2.x + 2.5)/boneCount,0.5,0,0) );
			boneMx[3] = tex2Dlod(vertexMap, float4( (IN.inTex2.x + 3.5)/boneCount,0.5,0,0) );
			position += mul(boneMx, IN.org_position) * IN.inTex2.y;
		}

		if(IN.inTex3.y>0){
			boneMx[0] = tex2Dlod(vertexMap, float4( (IN.inTex3.x + 0.5)/boneCount,0.5,0,0) );
			boneMx[1] = tex2Dlod(vertexMap, float4( (IN.inTex3.x + 1.5)/boneCount,0.5,0,0) );
			boneMx[2] = tex2Dlod(vertexMap, float4( (IN.inTex3.x + 2.5)/boneCount,0.5,0,0) );
			boneMx[3] = tex2Dlod(vertexMap, float4( (IN.inTex3.x + 3.5)/boneCount,0.5,0,0) );
			position += mul(boneMx, IN.org_position) * IN.inTex3.y;
		}

		position.w = 1.0;
   }
	Out.HPOS = mul(lightspace, position);
	Out.HPOS.y = Out.HPOS.z;
	Out.HPOS.z = 0.5;
	
	Out.baseTex = IN.texCoord;
	
	return Out;
}

