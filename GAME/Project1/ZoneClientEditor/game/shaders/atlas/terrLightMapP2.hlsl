
struct PS_INPUT
{
    float2 tex : TEXCOORD0;
    float2 fade : TEXCOORD1;
};

struct PS_OUTPUT
{
    float4 Color:COLOR;
};

PS_OUTPUT main(PS_INPUT psInput,
			  			uniform sampler2D lightmap	: register(S0),
              uniform sampler2D bakedColor  : register(S1),
              uniform float4 key1			: register(C0),
              uniform float4 key2			: register(C1),
              uniform float4 key3			: register(C2)
)
{
	PS_OUTPUT Out_ps = (PS_OUTPUT) 0;
	float4 color = (float4) 0;
	float4 bakedColorScale = tex2D(bakedColor, psInput.tex);
	float delta = 1.0/1023;

   	color =  tex2D(lightmap, float2(psInput.tex.x-delta, psInput.tex.y-delta))		* key1.x;
   	color += tex2D(lightmap, float2(psInput.tex.x-delta, psInput.tex.y)) 	 		* key1.y;
   	color += tex2D(lightmap, float2(psInput.tex.x-delta, psInput.tex.y+delta))		* key1.z;
   	color += tex2D(lightmap, float2(psInput.tex.x, 	 	 	 psInput.tex.y-delta))		* key1.w;
   	color += tex2D(lightmap, float2(psInput.tex.x, 	 	 	 psInput.tex.y)) 			* key2.x;
   	color += tex2D(lightmap, float2(psInput.tex.x, 	 	 	 psInput.tex.y+delta))		* key2.y;
   	color += tex2D(lightmap, float2(psInput.tex.x+delta, psInput.tex.y-delta))		* key2.z;
   	color += tex2D(lightmap, float2(psInput.tex.x+delta, psInput.tex.y)) 			* key2.w;
   	color += tex2D(lightmap, float2(psInput.tex.x+delta, psInput.tex.y+delta))		* key3.x;
  	color /= key3.y;
  	
  color += bakedColorScale;
	color = clamp(color, 0.0f, 1.0f);
	Out_ps.Color = color;
	return Out_ps;
}
