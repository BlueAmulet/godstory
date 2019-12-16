//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------
#define UNITS_PER_TEXTURE 8
#define UNITS_PER_BLEND_TEXTURE 512
#define LIGHTMAP_SIZE_BY_BLOCKSIZE 512
struct VS_OUT
{
    float4 pos               : POSITION;             
    float2 baseTexcoord     : TEXCOORD0;
    float2 blendTexcoord     : TEXCOORD1;
    float2 lightCoord        : TEXCOORD2;
};

VS_OUT   main(
                                    float4        pos                : POSITION,
                                    uniform float4x4 modelView         : register(C0)//,
                                    //uniform float2 offset                            : register(C10)
                                )
{
    VS_OUT output = (VS_OUT)0;
    //顶点变换
    output.pos = mul(modelView, pos);   
     
    //纹理坐标
    float2 temppos = pos.xy;
    output.baseTexcoord = temppos/UNITS_PER_TEXTURE;
    output.lightCoord = temppos / LIGHTMAP_SIZE_BY_BLOCKSIZE;
    //float2 tempOff = clamp((temppos-offset), 0, UNITS_PER_BLEND_TEXTURE);
    float2 tempOff = temppos;
    output.blendTexcoord = tempOff/UNITS_PER_BLEND_TEXTURE;  

    return output;
}