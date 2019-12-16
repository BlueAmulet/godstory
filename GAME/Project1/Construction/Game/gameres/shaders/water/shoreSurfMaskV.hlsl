// lzw add: shore surf mask shader                                              

#define IN_HLSL
#include "..\shdrConsts.h"


struct VertData
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD0; // x 面向岸边左端为 0.0f, 反之为 1.0f, y 触岸端为 0.0f, 反之为 1.0f 
    float3 params   : TEXCOORD1; // 初始位置, 移动速度, 透明度
};

struct ConnectData
{
    float4 hpos     : POSITION;
    float3 texCoord : TEXCOORD0; // z 透明度
    float2 fogCoord : TEXCOORD1;
};

ConnectData main( VertData IN,
                  uniform float4x4 modelview : register(C0),
                  uniform float4x4 objTrans  : register(VC_OBJ_TRANS),  
                  uniform float3   eyePos    : register(VC_EYE_POS),
                  uniform float3   fogData   : register(VC_FOGDATA),
                  uniform float    time      : register(C60)        
)
{
    ConnectData OUT;
    OUT.hpos = mul( modelview, float4( IN.position, 1.0f ) );

    OUT.texCoord.x = IN.texCoord.x;
    OUT.texCoord.y = IN.texCoord.y + IN.params.x + IN.params.y * time;
    OUT.texCoord.z = IN.params.z;
    
    float eyeDist = distance( IN.position, eyePos );
    float3 transPos = mul( objTrans, IN.position );
   
    OUT.fogCoord.x = 1.0 - ( eyeDist  / fogData.z );
    OUT.fogCoord.y = ( transPos.z - fogData.x ) * fogData.y;
    
    return OUT;
}

