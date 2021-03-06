// The purpose of this file is to get all of our HLSL structures into one place.
// Please use the structures here instead of redefining input and output structures
// in each shader file. If structures are added, please adhere to the naming convention.

//------------------------------------------------------------------------------
// Vertex Input Structures
//
// These structures map to FVFs/Vertex Declarations in PowerEngine. See gfxStructs.h
//------------------------------------------------------------------------------

// Notes
//
// Position should be specified as a float4. Right now our vertex structures in 
// the engine output float3s for position. This does NOT mean that the POSITION
// binding should be float3, because it will assign 0 to the w coordinate, which
// results in the vertex not getting translated when it is transformed. 

struct VertexIn_P
{
   float4 pos        : POSITION;
};

struct VertexIn_PT
{
   float4 pos        : POSITION;
   float2 uv0        : TEXCOORD0;
};

struct VertexIn_PTTT
{
   float4 pos        : POSITION;
   float2 uv0        : TEXCOORD0;
   float2 uv1        : TEXCOORD1;
   float2 uv2        : TEXCOORD2;
};

struct VertexIn_PC
{
   float4 pos        : POSITION;
   float4 color      : DIFFUSE;
};

struct VertexIn_PNC
{
   float4 pos        : POSITION;
   float3 normal     : NORMAL;
   float4 color      : DIFFUSE;
};

struct VertexIn_PCT
{
   float4 pos        : POSITION;
   float4 color      : DIFFUSE;
   float2 uv0        : TEXCOORD0;
};

struct VertexIn_PN
{
   float4 pos        : POSITION;
   float3 normal     : NORMAL;
};

struct VertexIn_PNT
{
   float4 pos        : POSITION;
   float3 normal     : NORMAL;
   float2 uv0        : TEXCOORD0;
};

struct VertexIn_PNCT
{
   float4 pos        : POSITION;
   float3 normal     : NORMAL;
   float4 color      : DIFFUSE;
   float2 uv0        : TEXCOORD0;
};

struct VertexIn_PNTTTB
{
   float4 pos        : POSITION;
   float3 normal     : NORMAL; 
   float2 uv0        : TEXCOORD0;
   float2 uv1        : TEXCOORD1;   
   float3 T          : TEXCOORD2;
   float3 B          : TEXCOORD3;
};