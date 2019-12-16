//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gfx/genericShaders.h"

//------------------------------------------------------------------------------

const char *gGSColorP = \
"struct ConnectData\n" \
"{\n" \
"   float4 color : COLOR0;\n" \
"};\n" \
"struct Fragout\n" \
"{\n" \
"   float4 col : COLOR0;\n" \
"};\n" \
"Fragout main( ConnectData IN, uniform sampler2D diffuseMap : register(S0) )\n" \
"{\n" \
"   Fragout OUT;\n" \
"   OUT.col = IN.color;\n" \
"   return OUT;\n" \
"}";

//------------------------------------------------------------------------------

const char *gGSColorV = \
"#define IN_HLSL\n" \
"#include \"shdrConsts.h\"\n" \
"struct Appdata\n" \
"{\n" \
"	float4 position   : POSITION;\n" \
"	float4 color      : COLOR;\n" \
"};\n" \
"struct Conn\n" \
"{\n" \
"   float4 HPOS             : POSITION;\n" \
"   float4 color            : COLOR;\n" \
"};\n" \
"Conn main( Appdata In, uniform float4x4 modelview : register( VC_WORLD_PROJ ) )\n" \
"{\n" \
"   Conn Out;\n" \
"   Out.HPOS = mul( modelview, In.position );\n" \
"   Out.color = In.color;\n" \
"   return Out;\n" \
"}";

//------------------------------------------------------------------------------

const char *gGSModColorTextureV = \
"#define IN_HLSL\n" \
"#include \"shdrConsts.h\"\n" \
"struct Appdata\n" \
"{\n" \
"	float4 position   : POSITION;\n" \
"	float4 color      : COLOR;\n" \
"	float2 texCoord   : TEXCOORD;\n" \
"};\n" \
"struct Conn\n" \
"{\n" \
"   float4 HPOS             : POSITION;\n" \
"   float4 color            : COLOR;\n" \
"   float2 texCoord         : TEXCOORD0;\n" \
"};\n" \
"Conn main( Appdata In, uniform float4x4 modelview : register( VC_WORLD_PROJ ) )\n" \
"{\n" \
"   Conn Out;\n" \
"   Out.HPOS = mul( modelview, In.position );\n" \
"   Out.color = In.color;\n" \
"   Out.texCoord = In.texCoord;\n" \
"   return Out;\n" \
"}";


//------------------------------------------------------------------------------

const char *gGSModColorTextureP = \
"struct ConnectData\n" \
"{\n" \
"   float4 color      : COLOR0;\n" \
"   float2 texCoord   : TEXCOORD0;\n" \
"};\n" \
"struct Fragout\n" \
"{\n" \
"   float4 col : COLOR0;\n" \
"};\n" \
"Fragout main( ConnectData IN, uniform sampler2D diffuseMap : register(S0) )\n" \
"{\n" \
"   Fragout OUT;\n" \
"   OUT.col = tex2D( diffuseMap, IN.texCoord ) * IN.color;\n" \
"   return OUT;\n" \
"}";

//------------------------------------------------------------------------------

const char *gGSAddColorTextureV = \
"#define IN_HLSL\n" \
"#include \"shdrConsts.h\"\n" \
"struct Appdata\n" \
"{\n" \
"	float4 position   : POSITION;\n" \
"	float4 color      : COLOR;\n" \
"	float2 texCoord   : TEXCOORD;\n" \
"};\n" \
"struct Conn\n" \
"{\n" \
"   float4 HPOS             : POSITION;\n" \
"   float4 color            : COLOR;\n" \
"   float2 texCoord         : TEXCOORD0;\n" \
"};\n" \
"Conn main( Appdata In, uniform float4x4 modelview : register( VC_WORLD_PROJ ) )\n" \
"{\n" \
"   Conn Out;\n" \
"   Out.HPOS = mul( modelview, In.position );\n" \
"   Out.color = In.color;\n" \
"   Out.texCoord = In.texCoord;\n" \
"   return Out;\n" \
"}\n";

//------------------------------------------------------------------------------

const char *gGSAddColorTextureP = \
"struct ConnectData\n" \
"{\n" \
"   float4 color      : COLOR0;\n" \
"   float2 texCoord   : TEXCOORD0;\n" \
"};\n" \
"struct Fragout\n" \
"{\n" \
"   float4 col : COLOR0;\n" \
"};\n" \
"Fragout main( ConnectData IN, uniform sampler2D diffuseMap : register(S0) )\n" \
"{\n" \
"   Fragout OUT;\n" \
"   OUT.col = tex2D( diffuseMap, IN.texCoord );\n" \
"   OUT.col.rgb = IN.color.rgb;\n" \
"   OUT.col.a *= IN.color.a;\n" \
"   return OUT;\n" \
"}";

//------------------------------------------------------------------------------

const char *gSSSPT_P = \
"struct ConnectData\n" \
"{\n" \
"   float2 texCoord   : TEXCOORD0;\n" \
"};\n" \
"struct Fragout\n" \
"{\n" \
"   float4 col : COLOR0;\n" \
"   float1 ds  : DEPTH0;\n" \
"};\n" \
"Fragout main( ConnectData IN, uniform sampler2D diffuseMap : register(S0), uniform sampler2D depthStencilMap : register(S1) )\n" \
"{\n" \
"   Fragout OUT;\n" \
"   OUT.col = tex2D( diffuseMap, IN.texCoord );\n" \
"   OUT.ds = tex2D( depthStencilMap, IN.texCoord ).x;\n" \
"   return OUT;\n" \
"}";

//------------------------------------------------------------------------------

const char *gSSSPT_V = \
"struct Appdata\n" \
"{\n" \
"	float3 position   : POSITION;\n" \
"	float2 texCoord   : TEXCOORD;\n" \
"};\n" \
"struct Conn\n" \
"{\n" \
"   float4 HPOS             : POSITION;\n" \
"   float2 texCoord         : TEXCOORD0;\n" \
"};\n" \
"Conn main( Appdata In )\n" \
"{\n" \
"   Conn Out;\n" \
"   Out.HPOS = float4( In.position, 1 );\n" \
"   Out.texCoord = In.texCoord;\n" \
"   return Out;\n" \
"}\n";


const char *gGSTextureV = \
"#define IN_HLSL\n" \
"#include \"shdrConsts.h\"\n" \
"struct Appdata\n" \
"{\n" \
"	float4 position   : POSITION;\n" \
"	float2 texCoord   : TEXCOORD;\n" \
"};\n" \
"struct Conn\n" \
"{\n" \
"   float4 HPOS             : POSITION;\n" \
"   float2 texCoord         : TEXCOORD0;\n" \
"};\n" \
"Conn main( Appdata In, uniform float4x4 modelview : register( VC_WORLD_PROJ ) )\n" \
"{\n" \
"   Conn Out;\n" \
"   Out.HPOS = mul( modelview, In.position );\n" \
"   Out.texCoord = In.texCoord;\n" \
"   return Out;\n" \
"}";


//------------------------------------------------------------------------------

const char *gGSTextureP = \
"struct ConnectData\n" \
"{\n" \
"   float2 texCoord   : TEXCOORD0;\n" \
"};\n" \
"struct Fragout\n" \
"{\n" \
"   float4 col : COLOR0;\n" \
"};\n" \
"Fragout main( ConnectData IN, uniform sampler2D diffuseMap : register(S0) )\n" \
"{\n" \
"   Fragout OUT;\n" \
"   OUT.col = tex2D( diffuseMap, IN.texCoord );\n" \
"   return OUT;\n" \
"}";
