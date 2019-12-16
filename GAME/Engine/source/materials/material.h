//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "materials/miscShdrDat.h"
#include "gfx/gfxTextureHandle.h"
#include "gfx/gfxStructs.h"
#include "gfx/gfxCubemap.h"

class CubemapData;
struct SceneGraphData;
class GFXCubemap;

//**************************************************************************
// Basic Material
//**************************************************************************
class Material : public SimObject
{
   typedef SimObject Parent;

public:
   static GFXCubemap *getNormalizeCube();

   //-----------------------------------------------------------------------
   // Enums
   //-----------------------------------------------------------------------
   enum Constants
   {
      MAX_TEX_PER_PASS = 8,         // number of textures per pass
      MAX_STAGES = 4,
   };


   enum TexType
   {
      NoTexture = 0,
      Standard = 1,
      Detail,
      Bump,
      Env,
      Cube,
      SGCube,  // scene graph cube - probably dynamic
      Lightmap,
      NormLightmap,
      Mask,
      Fog,
      BackBuff,
      ReflectBuff,
      Misc,
      DynamicLight,
      DynamicLightSecondary,
      DynamicLightMask,
      NormalizeCube
   };

   enum BlendOp
   {
      None = 0,
      Mul,
      Add,
      AddAlpha,      // add modulated with alpha channel
      Sub,
      LerpAlpha,     // linear interpolation modulated with alpha channel
      NumBlendTypes
   };

   enum AnimType
   {
      Scroll = 1,
      Rotate = 2,
      Wave   = 4,
      Scale  = 8,
      Sequence = 16,
   };

   enum WaveType
   {
      Sin = 0,
      Triangle,
      Square,
   };

   enum MaterialType
   {
      Base = 0,
      Custom = 1,
   };

   enum GameStatus
   {
	   NoneStatus	  = 0,
	   DisableColor	  = 1,
	   ForceTranslate = 2,
	   SunLightOnly	  = 4,
   };

   struct StageData
   {
      GFXTexHandle      tex[GFXShaderFeatureData::NumFeatures];
      GFXCubemap *      cubemap;
      bool              lightmap;

      StageData()
      {
         cubemap = NULL;
         lightmap = false;
      }
   };


private:
   static GFXCubemapHandle normalizeCube;   
public:

   //-----------------------------------------------------------------------
   // Data
   //-----------------------------------------------------------------------
   StringTableEntry  baseTexFilename[MAX_STAGES];							//���������ļ���
   StringTableEntry  detailFilename[MAX_STAGES];							//ϸ�������ļ���
   StringTableEntry  bumpFilename[MAX_STAGES];								//bump�����ļ���
   StringTableEntry  envFilename[MAX_STAGES];								//���������ļ���
   StringTableEntry  SpecularFilename[MAX_STAGES];							//�߹������ļ���
   StageData         stages[MAX_STAGES];									//
   ColorF            diffuse[MAX_STAGES];									//��������ɫ
   ColorF            specular[MAX_STAGES];									//�߹���ɫ
   ColorF            colorMultiply[MAX_STAGES];								//����color��ɫ
   F32               specularPower[MAX_STAGES];								//�߹�ϵ��
   bool              pixelSpecular[MAX_STAGES];								//����Ƿ�������ؼ��߹�
   bool              vertexSpecular[MAX_STAGES];							//����Ƿ���ö��㼶�߹�

   // yes this should be U32 - we test for 2 or 4...
   U32               exposure[MAX_STAGES];									//

   U32               animFlags[MAX_STAGES];									//�������ͼ�AnimType����
   Point2F           scrollDir[MAX_STAGES];									
   F32               scrollSpeed[MAX_STAGES];
   Point2F           scrollOffset[MAX_STAGES];

   F32               rotSpeed[MAX_STAGES];
   Point2F           rotPivotOffset[MAX_STAGES];
   F32               rotPos[MAX_STAGES];
   
   F32               wavePos[MAX_STAGES];
   F32               waveFreq[MAX_STAGES];
   F32               waveAmp[MAX_STAGES];
   U32               waveType[MAX_STAGES];
   
   F32               seqFramePerSec[MAX_STAGES];
   F32               seqSegSize[MAX_STAGES];
   
   bool              glow[MAX_STAGES];          // entire stage glows		//����Ƿ��glowЧ��
   bool              emissive[MAX_STAGES];									//����Ƿ��Է���

   bool				 noFog;													//Ray: ����
   bool				 effectLight;											//Ray: ��Ч��Ч
   bool              castsShadow;
   bool              breakable;
   bool              doubleSided;
   bool              enableVertexColor;
   bool              attenuateBackFace;
   bool              preload;

   const char      * cubemapName;

   CubemapData     * mCubemapData;
   bool              dynamicCubemap;

   bool              translucent;											//����Ƿ�Ϊ͸������
   bool              subPassTranslucent;
   BlendOp           translucentBlendOp;
   bool              translucentZWrite;
   bool              translucentZEnable;

   bool				 enableFilterColor;											//Ray: �˾�
   ColorF			 filterColor;

   bool              alphaTest;
   U32               alphaRef;

   bool              planarReflection;										//����Ƿ��ƽ�淴��Ч��


   const char      * mapTo; // map Material to this texture name			

public:
   // Static material wide time params
   static F32 mDt;
   static F32 mAccumTime;

   char mPath[128]; // [����ǰ·��]  ��������������,�������̼߳���ʱ·�����������,��·����������󴴽�ʱ����ֵ [7/2/2009 LivenHotch]


protected:
   // Static material wide time params
   static U32 mLastTime;
   
   // Per material animation parameters
   U32 mLastUpdateTime;

   static SimSet *gMatSet;

   bool  hasSetStageData;   
   MaterialType mType;
   bool  mIsIFL;



   static EnumTable mBlendOpTable;
   static EnumTable mWaveTypeTable;

   virtual void mapMaterial();

   bool onAdd();
   void onRemove();

public:
   Material();

   static void initPersistFields();
   static void updateTime();
   static SimSet * getMaterialSet();

   inline bool haveFilterColor(){return enableFilterColor;}
   inline ColorF &getFilterColor(){return filterColor;}
   inline float getColorExposure(){return filterColor.alpha;}
   inline MaterialType getType(){ return mType; }
   inline bool isIFL(){ return mIsIFL; }
   inline bool isTranslucent() { return translucent || subPassTranslucent; }
   inline char* getPath() { return mPath; }

   void updateTimeBasedParams();

   DECLARE_CONOBJECT(Material);
};

#endif _MATERIAL_H_
