//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
#define WT_0 1.0
#define WT_1 0.8
#define WT_2 0.6
#define WT_3 0.4
#define WT_4 0.2
#define WT_NORMALIZE (WT_0+2.0*(WT_1+WT_2+WT_3+WT_4))
#define KW_0 (WT_0/WT_NORMALIZE)
#define KW_1 (WT_1/WT_NORMALIZE)
#define KW_2 (WT_2/WT_NORMALIZE)
#define KW_3 (WT_3/WT_NORMALIZE)
#define KW_4 (WT_4/WT_NORMALIZE)

struct ConnectData
{
    float2 UV    : TEXCOORD0;
    float4 UV1   : TEXCOORD1; 
    float4 UV2   : TEXCOORD2; 
    float4 UV3   : TEXCOORD3; 
    float4 UV4   : TEXCOORD4; 
};

struct Fragout
{
   float4 col : COLOR0;
};

//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
Fragout main( ConnectData IN,
              uniform sampler2D SrcSamp      : register(S0)
)
{
    Fragout OutCol = (Fragout)0;
    OutCol.col = tex2D(SrcSamp, IN.UV4.zw) * KW_4;
    OutCol.col += tex2D(SrcSamp, IN.UV3.zw) * KW_3;
    OutCol.col += tex2D(SrcSamp, IN.UV2.zw) * KW_2;
    OutCol.col += tex2D(SrcSamp, IN.UV1.zw) * KW_1;
    OutCol.col += tex2D(SrcSamp, IN.UV) * KW_0;
    OutCol.col += tex2D(SrcSamp, IN.UV1.xy) * KW_1;
    OutCol.col += tex2D(SrcSamp, IN.UV2.xy) * KW_2;
    OutCol.col += tex2D(SrcSamp, IN.UV3.xy) * KW_3;
    OutCol.col += tex2D(SrcSamp, IN.UV4.xy) * KW_4;
    //OutCol.col = float4(0.0, 0.0, 0.0, 1.0);
    return OutCol;
}