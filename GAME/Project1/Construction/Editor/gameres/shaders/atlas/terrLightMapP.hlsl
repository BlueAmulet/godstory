struct ConnectData
{
   float4 hpos      : POSITION;
   float2 detCoord  : TEXCOORD0;
};

struct Fragout
{
   float4 col : COLOR0;
};

Fragout main( ConnectData IN,
              //uniform sampler2D lightmap          : register(S0),
              uniform sampler2D bakedColor               : register(S0),
              uniform float4 sunAmbient                 : register(C9),
              uniform float4 sunColor                        : register(C10),
              uniform float4 backColor                        : register(C12)
              )
{
   Fragout OUT;
   float4 bakedColorScale = tex2D(bakedColor, IN.detCoord);
   float4 color;
  
  //ˢ�ϵĹ�����ɫ
  if(bakedColorScale.a > 0.6)
  {
  	color = bakedColorScale;//ֱ�Ӳ���ˢ�ϵ���ɫ��alphaֵû�ã�������͸����
  }
  else //����Ĺ�����ɫ
  {
    color = sunAmbient + sunColor*bakedColorScale.g + backColor * (1 - bakedColorScale.g);
    color /= 2;   
  }             

   color = clamp(color, 0.0f, 1.0f);
   OUT.col = color;
   return OUT;
}
