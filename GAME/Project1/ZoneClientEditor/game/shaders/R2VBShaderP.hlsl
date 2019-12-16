struct ConnectData
{
   float2 t0     : TEXCOORD0;
};

struct Fragout
{
   float4 col : COLOR0;
};

float   boneCount		: register(C9);
float   xBias   		: register(C10);

float4x4 getMatrix(sampler2D s, float idx,float count)
{
	float4x4 m;

	m[0] = tex2D(s, float2((idx+0.5)/boneCount, 0.5f));
	m[1] = tex2D(s, float2((idx+1.5)/boneCount, 0.5f));
	m[2] = tex2D(s, float2((idx+2.5)/boneCount, 0.5f));
	m[3] = tex2D(s, float2((idx+3.5)/boneCount, 0.5f));

	return m;
}

//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
Fragout main( ConnectData IN,
              uniform sampler2D initVN      : register(S0),
              uniform sampler2D boneIdx     : register(S1),
              uniform sampler2D weightV     : register(S2),
              uniform sampler2D boneMx      : register(S3)
              )
{
   Fragout OUT;
   float4x4 M;

	 IN.t0.x += xBias;
	 
	 float4 Index  = tex2D(boneIdx, IN.t0); // get bone index.
	 float4 Vertex = tex2D(initVN, IN.t0); // get vertex position or normal.
	 float4 Weight = tex2D(weightV, IN.t0); // get vertex weight.
	
	
	 M = getMatrix(boneMx, Index.x,boneCount);
	 OUT.col.xyz = mul(M,Vertex).xyz*Weight.x;
	
	
	 if(Weight.y>0)
	 {
		 M = getMatrix(boneMx, Index.y,boneCount);
		 OUT.col.xyz += mul(M,Vertex).xyz*Weight.y;
	 }

	 if(Weight.z>0)
	 {
		 M = getMatrix(boneMx, Index.z,boneCount);
		 OUT.col.xyz += mul(M,Vertex).xyz*Weight.z;
	 }
	 
	 if(Weight.w>0)
	 {
		 M = getMatrix(boneMx, Index.w,boneCount);
		 OUT.col.xyz += mul(M,Vertex).xyz*Weight.w;
	 }

 	 OUT.col.w = 1.0f;
 	 
 	 
 	 
/*
 	 OUT.col.x = Index.x;
 	 OUT.col.y = M[0].x;
 	 OUT.col.z = Weight.x;
 	 OUT.col.w = Vertex.x;
*/
   return OUT;
}
