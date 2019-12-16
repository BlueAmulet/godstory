//*****************************************************************************
// PowerEngine -- HLSL procedural shader                                              
//*****************************************************************************
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
float4 myColor              : register(c4);
float4  amount              : register(c0);

struct ConnectData
{
    float4  tex       : TEXCOORD0;
    float4  tex1      : TEXCOORD1;
    float4   tex2      : TEXCOORD2;
    float4  tex3      : TEXCOORD3;
};

struct Fragout
{
   float4 col : COLOR0;
};

//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
Fragout main( ConnectData IN,
              uniform sampler2D DepthTex0           : register(S0),
             uniform sampler2D BackTex           : register(S1),
                uniform float2 radius : register(c6)
)
{
   Fragout OutCol = (Fragout)0;

   float Depth0 = tex2D(DepthTex0, IN.tex.xy).r;
   Depth0 += tex2D(DepthTex0, IN.tex.zw).r;
    Depth0 += tex2D(DepthTex0, IN.tex1.xy).r;
   Depth0 += tex2D(DepthTex0, IN.tex1.zw).r;
Depth0 /= 4.0;
   
   OutCol.col = myColor;
   OutCol.col.a = Depth0 * amount.x + amount.y; 
   float rr = length(IN.tex2.xyz);
    float alpha = (rr > radius.y) ? (1- (rr - radius.y)/(radius.x - radius.y)) : 1;
   alpha = (alpha > 0) ? alpha : 0;

   OutCol.col.a *= alpha;
   OutCol.col.a = (OutCol.col.a > amount.z) ? OutCol.col.a : amount.z;
   OutCol.col.a = (OutCol.col.a < amount.w) ? OutCol.col.a : amount.w;

   float2 temp;
   temp.x = IN.tex3.x/IN.tex3.w * 0.5 + 0.5;
   temp.y = (-IN.tex3.y/IN.tex3.w) * 0.5 + 0.5;

   OutCol.col.rgb = (OutCol.col.a != 0.0) ? OutCol.col.a * OutCol.col.rgb + (1- OutCol.col.a) * tex2D(BackTex, temp).rgb  : tex2D(BackTex, temp).rgb;
   OutCol.col.a = 1.0;
   return OutCol;
 }