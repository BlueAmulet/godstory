//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------
#   define LM_BLEND_FACTOR   2.0
struct ConnectData
{
    float4 pos               : POSITION;             
    float2 baseTexcoord      : TEXCOORD0;
    float2 blendTexcoord     : TEXCOORD1;
    float2 lightCoord        : TEXCOORD2;
};

struct Fragout
{
    float4 color               : COLOR0;
};

Fragout main(
                                    ConnectData input,
                                    uniform sampler baseTex0        : register(S0),
                                    uniform sampler baseTex1        : register(S1),
                                    uniform sampler baseTex2        : register(S2),
                                    uniform sampler baseTex3        : register(S3),
                                    uniform sampler blendTex        : register(S4),
                                    uniform sampler lightTex        : register(S5)
                                )
{
    Fragout   output = (Fragout)0;
 		//纹理采样
    float4   baseTexColor0 = tex2D(baseTex0, input.baseTexcoord);
    float4   baseTexColor1 = tex2D(baseTex1, input.baseTexcoord);
    float4   baseTexColor2 = tex2D(baseTex2, input.baseTexcoord);
    float4   baseTexColor3 = tex2D(baseTex3, input.baseTexcoord);
    float4   BlendTexColor = tex2D(blendTex, input.blendTexcoord);
    //纹理混合
    float4   TexColor = 0.0f;
    TexColor += baseTexColor0 * BlendTexColor.r;
    TexColor += baseTexColor1 * BlendTexColor.g;
    TexColor += baseTexColor2 * BlendTexColor.b;
    TexColor += baseTexColor3 * BlendTexColor.a;
 		//光照
    float4 LightMapColor = tex2D(lightTex, input.lightCoord);
    TexColor = LM_BLEND_FACTOR * LightMapColor * TexColor;

    output.color = TexColor;
    return output;
}

