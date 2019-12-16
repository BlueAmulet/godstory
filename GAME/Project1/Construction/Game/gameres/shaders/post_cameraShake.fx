struct QuadVertexOutput {
   	float4 Position	: POSITION;
    float2 UV		: TEXCOORD0;
};

#define BSIZE 32
#define FULLSIZE 66
#define NOISEFRAC 0.03125
#define NO_NEG_HACK 10000.0f

float3 s_curve(float3 t) { return t*t*( float3(3,3,3) - float3(2,2,2)*t); }
float2 s_curve(float2 t) { return t*t*( float2(3,3) - float2(2,2)*t); }
float  s_curve(float  t) { return t*t*(3.0-2.0*t); }

float vertex_noise(float3 v,
			const uniform float4 pg[FULLSIZE])
{
    v = v + float3(NO_NEG_HACK, NO_NEG_HACK, NO_NEG_HACK);   // hack to avoid negative numbers
    float3 i = frac(v * NOISEFRAC) * BSIZE;   // index between 0 and BSIZE-1
    float3 f = frac(v);            // fractional position
    // lookup in permutation table
    float2 p;
    p.x = pg[ i[0]     ].w;
    p.y = pg[ i[0] + 1 ].w;
    p = p + i[1];
    float4 b;
    b.x = pg[ p[0] ].w;
    b.y = pg[ p[1] ].w;
    b.z = pg[ p[0] + 1 ].w;
    b.w = pg[ p[1] + 1 ].w;
    b = b + i[2];
    // compute dot products between gradients and vectors
    float4 r;
    r[0] = dot( pg[ b[0] ].xyz, f );
    r[1] = dot( pg[ b[1] ].xyz, f - float3(1.0f, 0.0f, 0.0f) );
    r[2] = dot( pg[ b[2] ].xyz, f - float3(0.0f, 1.0f, 0.0f) );
    r[3] = dot( pg[ b[3] ].xyz, f - float3(1.0f, 1.0f, 0.0f) );
    float4 r1;
    r1[0] = dot( pg[ b[0] + 1 ].xyz, f - float3(0.0f, 0.0f, 1.0f) );
    r1[1] = dot( pg[ b[1] + 1 ].xyz, f - float3(1.0f, 0.0f, 1.0f) );
    r1[2] = dot( pg[ b[2] + 1 ].xyz, f - float3(0.0f, 1.0f, 1.0f) );
    r1[3] = dot( pg[ b[3] + 1 ].xyz, f - float3(1.0f, 1.0f, 1.0f) );
    // interpolate
    f = s_curve(f);
    r = lerp( r, r1, f[2] );
    r = lerp( r.xyyy, r.zwww, f[1] );
    return lerp( r.x, r.y, f[0] );
}

// 2D version
float vertex_noise(float2 v,
			const uniform float4 pg[FULLSIZE])
{
    v = v + float2(NO_NEG_HACK, NO_NEG_HACK);
    float2 i = frac(v * NOISEFRAC) * BSIZE;   // index between 0 and BSIZE-1
    float2 f = frac(v);            // fractional position
    // lookup in permutation table
    float2 p;
    p[0] = pg[ i[0]   ].w;
    p[1] = pg[ i[0]+1 ].w;
    p = p + i[1];
    // compute dot products between gradients and vectors
    float4 r;
    r[0] = dot( pg[ p[0] ].xy,   f);
    r[1] = dot( pg[ p[1] ].xy,   f - float2(1.0f, 0.0f) );
    r[2] = dot( pg[ p[0]+1 ].xy, f - float2(0.0f, 1.0f) );
    r[3] = dot( pg[ p[1]+1 ].xy, f - float2(1.0f, 1.0f) );
    // interpolate
    f = s_curve(f);
    r = lerp( r.xyyy, r.zwww, f[1] );
    return lerp( r.x, r.y, f[0] );
}

// 1D version
float vertex_noise(float v,
		const uniform float4 pg[FULLSIZE])
{
    v = v + NO_NEG_HACK;
    float i = frac(v * NOISEFRAC) * BSIZE;   // index between 0 and BSIZE-1
    float f = frac(v);            // fractional position
    // compute dot products between gradients and vectors
    float2 r;
    r[0] = pg[i].x * f;
    r[1] = pg[i + 1].x * (f - 1.0f);
    // interpolate
    f = s_curve(f);
    return lerp( r[0], r[1], f);
}

const float4 NTab[66] <string UIWidget="None";> = {-0.854611,-0.453029,0.25378,0,
		-0.84528,-0.456307,-0.278002,1,
		-0.427197,0.847095,-0.316122,2,
		0.670266,-0.496104,0.551928,3,
		-0.675674,-0.713842,0.184102,4,
		-0.0373602,-0.600265,0.798928,5,
		-0.939116,-0.119538,0.322135,6,
		0.818521,0.278224,0.502609,7,
		0.105335,-0.765291,0.635007,8,
		-0.634436,-0.298693,0.712933,9,
		-0.532414,-0.603311,-0.593761,10,
		0.411375,0.0976618,0.906219,11,
		0.798824,-0.416379,-0.434175,12,
		-0.691156,0.585681,-0.423415,13,
		0.612298,0.0777332,0.786797,14,
		0.264612,-0.262848,0.927842,15,
		-0.70809,0.0548396,-0.703989,16,
		0.933195,-0.294222,-0.206349,17,
		0.788936,-0.466718,-0.399692,18,
		-0.540183,-0.824413,0.168954,19,
		0.469322,-0.184125,0.863617,20,
		-0.84773,0.292229,-0.44267,21,
		0.450832,0.650314,-0.611427,22,
		0.906378,-0.247125,-0.342647,23,
		-0.995052,0.0271277,-0.0955848,24,
		-0.0252277,-0.778349,0.627325,25,
		0.991428,0.128623,0.0229457,26,
		-0.842581,-0.290688,0.453384,27,
		-0.662511,-0.500545,-0.557256,28,
		0.650245,-0.692099,-0.313338,29,
		0.636901,0.768918,-0.0558766,30,
		-0.437006,0.872104,-0.220138,31,
		-0.854611,-0.453029,0.25378,0,
		-0.84528,-0.456307,-0.278002,1,
		-0.427197,0.847095,-0.316122,2,
		0.670266,-0.496104,0.551928,3,
		-0.675674,-0.713842,0.184102,4,
		-0.0373602,-0.600265,0.798928,5,
		-0.939116,-0.119538,0.322135,6,
		0.818521,0.278224,0.502609,7,
		0.105335,-0.765291,0.635007,8,
		-0.634436,-0.298693,0.712933,9,
		-0.532414,-0.603311,-0.593761,10,
		0.411375,0.0976618,0.906219,11,
		0.798824,-0.416379,-0.434175,12,
		-0.691156,0.585681,-0.423415,13,
		0.612298,0.0777332,0.786797,14,
		0.264612,-0.262848,0.927842,15,
		-0.70809,0.0548396,-0.703989,16,
		0.933195,-0.294222,-0.206349,17,
		0.788936,-0.466718,-0.399692,18,
		-0.540183,-0.824413,0.168954,19,
		0.469322,-0.184125,0.863617,20,
		-0.84773,0.292229,-0.44267,21,
		0.450832,0.650314,-0.611427,22,
		0.906378,-0.247125,-0.342647,23,
		-0.995052,0.0271277,-0.0955848,24,
		-0.0252277,-0.778349,0.627325,25,
		0.991428,0.128623,0.0229457,26,
		-0.842581,-0.290688,0.453384,27,
		-0.662511,-0.500545,-0.557256,28,
		0.650245,-0.692099,-0.313338,29,
		0.636901,0.768918,-0.0558766,30,
		-0.437006,0.872104,-0.220138,31,
		-0.854611,-0.453029,0.25378,0,
		-0.84528,-0.456307,-0.278002,1};

float2 TimeDelta = {1,.2};
float Speed;
float Time;
float Shake;
float Sharpness;
texture  g_txScene;

sampler2D SceneSampler =
sampler_state
{
    Texture = <g_txScene>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

QuadVertexOutput ShakerVS(
        float3 Position : POSITION, 
        float2 TexCoord : TEXCOORD0
) {
    QuadVertexOutput OUT;
    OUT.Position = float4(Position, 1);
    float2 dn = Speed*Time*TimeDelta;
    float2 off = 0;//float2(QuadTexOffset/(QuadScreenSize.x),QuadTexOffset/(QuadScreenSize.y));
    //float2 noisePos = TexCoord+off+dn;
    float2 noisePos = (float2)(0.5)+off+dn;
    float2 i = Shake*float2(vertex_noise(noisePos, NTab),
                            vertex_noise(noisePos.yx, NTab));
    i = sign(i) * pow(i,Sharpness);
    OUT.UV = TexCoord.xy+i;
    return OUT;
}
float4 TexQuadPS(QuadVertexOutput IN,uniform sampler2D InputSampler) : COLOR
{   
	float4 texCol = tex2D(InputSampler, IN.UV);
	return texCol;
}  
technique CamShake 
 {
    pass p0 
    {
        VertexShader = compile vs_2_0 ShakerVS();
        PixelShader  = compile ps_2_0 TexQuadPS(SceneSampler);
    }
}
