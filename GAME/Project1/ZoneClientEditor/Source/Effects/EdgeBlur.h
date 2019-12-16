//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _EDGEBLUR_H_
#define _EDGEBLUR_H_

#ifndef _GAMEBASE_H_
#include "T3D/gameBase.h"
#endif
#ifndef _COLOR_H_
#include "core/color.h"
#endif

#include "gfx/gfxTextureHandle.h"
#include "gfx/gfxDevice.h"
#include "materials/shaderData.h"
class GFXStateBlock;
class CEdgeBlur
{
public:
	CEdgeBlur();
	~CEdgeBlur();
	/**
	@brief		创建

	@return		空
	*/  
	void                                                            Create(U32 BlurNum);

	/**
	@brief		设置纹理

	@return		空
	*/  
	void                                                            SetTexture(StringTableEntry texname);

	/**
	@brief		设置是否开启残影

	@return		空
	*/  
	void                                                            SetOpenBlurState(bool bdata);

	/**
	@brief		获取是否开启残影

	@return		是否开启残影
	*/  
	bool                                                            GetOpenBlurState();

	/**
	@brief		获取纹理

	@return		纹理指针
	*/  
	GFXTexHandle                                                    GetTexture();

	/**
	@brief		帧调用函数

	@return		空
	*/  
	void                                                            FrameMove();

	/**
	@brief		渲染

	@return		空
	*/  
	void                                                            Render(Point3F &point);

	/**
	@brief		添加

	@return		空
	*/  
	void                                                            Add(Point3F min, Point3F max, ColorF color);

	void renderObject( SceneState *state, RenderInst *ri );

protected:
	/**
	@brief		销毁

	@return		空
	*/  
	void                                                            Destory();
private:
	U32                                                             m_wBlurNum;                         //最大残影数量(1个残影带2个顶点)
	GFXVertexPCT*                                                   m_EdgeBlurPointList;
	U32																m_wCurBlurNum;                      //当前残影数量
	bool                                                            m_bOpenBlur;                        //是否开启残影
	GFXTexHandle                                                    m_TextureHandle;                    //纹理
	GFXVertexBufferHandle<GFXVertexPCT>                             mVertBuff;
	GFXPrimitiveBufferHandle                                        primBuffer; 
	bool                                                            m_bFill;
    ShaderData*                                                     m_pShader;
      
	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mCullCCWSB;
public:
	static void init();
	static void shutdown();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};

struct IDirect3DVolumeTexture9;

struct ElectricityData : public SimObject
{
public:
	ColorF                                                          color;
	F32                                                             glowStrength;
	F32                                                             height;
	F32                                                             glowFallOff;
	F32                                                             speed;
	F32                                                             sampleDist;
	F32                                                             ambientGlow;
	F32                                                             ambientGlowHeightScale;
	F32                                                             vertNoise;
	F32                                                             time;
	F32                                                             widthRate;
	StringTableEntry												textureName;

	ElectricityData();
	~ElectricityData();

	DECLARE_CONOBJECT(ElectricityData);
	static void initPersistFields();
};

class Electricity : public RenderableSceneObject
{
	typedef RenderableSceneObject Parent;

public:
	Electricity();
	~Electricity();

	bool onAdd();
	void onRemove();
	void renderObject( SceneState *state, RenderInst *ri );

	bool prepRenderImage( SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState=false);
	void setCarrier(SceneObject* pStart, SceneObject* pEnd);
	void calcBox();
	void setData(ElectricityData* pData);

	/**
	@brief		设置两端的点

	@return		空
	*/  
	void                                                            SetPoints(Point3F start, Point3F end);

	/**
	@brief		获取两端的点

	@return		空
	*/  
	void                                                            GetPoints(Point3F* start, Point3F* end);

	/**
	@brief		设置纹理

	@return		空
	*/  
	void                                                            SetTexture(StringTableEntry texname);

	/**
	@brief		渲染

	@return		空
	*/  
	void                                                            Render();

	/**
	@brief		渲染

	@return		空
	*/  
	void                                                            RenderIns();

	/**
	@brief		设置是否开启

	@return		空
	*/  
	void                                                            SetOpenState(bool bdata);

	/**
	@brief		获取是否开启

	@return		是否开启
	*/  
	bool                                                            GetOpenState();

	/**
	@brief		设置宽度比例

	@return		空
	*/  
	void                                                            SetWidthRate(F32 data);

	/**
	@brief		获取宽度比例

	@return		宽度比例
	*/  
	F32                                                             GetWidthRate();

protected:
private:
	ColorF                                                          m_Color;
    F32                                                             m_fGlowStrength;
	F32                                                             m_fHeight;
    F32                                                             m_fGlowFallOff;
    F32                                                             m_fSpeed;
	F32                                                             m_fSampleDist;
	F32                                                             m_fAmbientGlow;
	F32                                                             m_fAmbientGlowHeightScale;
	F32                                                             m_fVertNoise;
	F32                                                             m_fTime;
	IDirect3DVolumeTexture9*                                        m_pTexture;
	Point3F                                                         m_vPosStart;
	Point3F                                                         m_vPosEnd; 
	ShaderData*                                                     m_pShader;
	bool                                                            m_bOpen;
    U32                                                             m_dwNowTime;
	F32                                                             m_fWidthRate;
	StringTableEntry												m_TextureName;
	ElectricityData*												m_pDataBlock;

	SimObjectPtr<SceneObject>										m_pObjStart;
	SimObjectPtr<SceneObject>										m_pObjEnd;
};

#define DEFAULT_THICKNESS_TO_COLOR_SCALE	424.0f
#define DEFAULT_NEARCLIP					0.1f
#define DEFAULT_FARCLIP						50.0f
#include "ts/tsShapeInstance.h"

class ThicknessRenderProperties
{
public:
	GFXBlend  	    m_dwVolumeColorToScreenSrcBlend;
	GFXBlend	    m_dwVolumeColorToScreenDestBlend;
	GFXBlendOp	    m_dwVolumeColorToScreenBlendOp;

	GFXTexHandle *	m_ppTexFogThicknessToColor;

	F32	    m_fTexCrdScale;					
	F32	    m_fTexCrdPrecisionFactor;	

	F32     m_fNearClip;			
	F32     m_fFarClip;

	F32	    m_fScale;

	bool SetParameters( F32 fThicknessToColorScale, F32 fNearClip, F32 fFarClip );
	bool SetClipPlanes( F32 fNearClip, F32 fFarClip );

	virtual void	SetThicknessToColorTexCoordScale( F32 fTexCrdScale );
	virtual F32   	GetThicknessToColorTexCoordScale() { return( m_fTexCrdScale ); }; 

	ThicknessRenderProperties();


protected:
	Point4F m_NormalizeWDepth;

};

class ThicknessRenderProperties8BPC : public ThicknessRenderProperties
{
public:
	enum DepthValuesPerChannel
	{
		PVF_256,	//  1 surfaces      24-bit
		PVF_128,	//  2 surfaces		21-bit
		PVF_64,		//  4 surfaces		18-bit
		PVF_32,		//  8 surfaces		15-bit = 32768
		PVF_16,		//  16 surfaces		12-bit = 4096
		PVF_8,		//  32 surfaces		9-bit  = 512
		PVF_4,		//  64 surfaces		6-bit  = 64
		PVF_2,		//  128 surfaces	3-bit  = 8
		PVF_UNSET
	};

	enum MaxFogDepthComplexity
	{
		PVF_1_SURFACES_24_BIT_DEPTH,
		PVF_2_SURFACES_21_BIT_DEPTH,
		PVF_4_SURFACES_18_BIT_DEPTH,
		PVF_8_SURFACES_15_BIT_DEPTH,
		PVF_16_SURFACES_12_BIT_DEPTH,
		PVF_32_SURFACES_9_BIT_DEPTH,
		PVF_64_SURFACES_6_BIT_DEPTH,
		PVF_128_SURFACES_3_BIT_DEPTH,
		PVF_MAXDEPTHUNSET
	};

	MaxFogDepthComplexity	m_MaxFogDepthComplexity;
	DepthValuesPerChannel	m_DepthValuesPerColorChannel;

	bool SetParameters( F32 fThicknessToColorScale,
		F32 fNearClip,
		F32 fFarClip,
		MaxFogDepthComplexity depth_relation,
		bool bUsePS20);

	virtual void	SetThicknessToColorTexCoordScale( F32 fTexCrdScale );

	F32 m_fBitReduce;

	F32		m_fGrnRamp;
	F32		m_fBluRamp;
	Point4F	m_vTexAddrWeights;
	bool		m_bUsePS20;

	GFXTexHandle *	m_ppTexRedGreenRamp;
	GFXTexHandle *	m_ppTexBlueRamp;
	bool		CreateRampTextures();
	bool		FreeRampTextures();

	ThicknessRenderProperties8BPC();
	~ThicknessRenderProperties8BPC();
	void	SetAllNull();

protected:
	
	GFXTexHandle		m_pTexRedGreenRamp;
	GFXTexHandle		m_pTexBlueRamp;

	void CreateGradientDithered( GFXTexHandle* ppTex,
		S32 width, S32 height,
		char lower, char upper,
		Point3F color_mask,
		const U32 * pDither_upper );

	void CreateGradient2D( GFXTexHandle* ppTex2D,
		S32 width, S32 height, char lower, char upper,
		Point3F col_mask_u, Point3F col_mask_v );

	friend class ThicknessRenderPS20_8bpc;
};

#define DEFAULT_DEPTHRELATION				ThicknessRenderProperties8BPC::PVF_8_SURFACES_15_BIT_DEPTH

class ThicknessRenderTargetsPS20_8bpc
{
public:
	GFXTexHandle		    m_pTexOccludersDepth;

	GFXTexHandle		    m_pTexFrontFacesDepth;
	GFXTextureTargetRef		m_pSurfFrontFacesDepth;
	GFXTexHandle		    m_pTexBackFacesDepth;
	GFXTextureTargetRef		m_pSurfBackFacesDepth;

	Point4F				    m_HalfTexelSize;

	bool		Initialize( U32 x_resolution, U32 y_resolution );
	bool		Free();

	bool		SetToDefaultBackbuffers();
	bool		SetToFrontFacesDepth();
	bool		SetToBackFacesDepth();

	ThicknessRenderTargetsPS20_8bpc();
	~ThicknessRenderTargetsPS20_8bpc();
	void	SetAllNull()
	{
		m_pTexOccludersDepth		= NULL;
		m_pTexFrontFacesDepth		= NULL;
		m_pSurfFrontFacesDepth		= NULL;
		m_pTexBackFacesDepth		= NULL;
		m_pSurfBackFacesDepth		= NULL;
	}
};

class FogTombShaders8bpc
{
public:
	FogTombShaders8bpc();
	~FogTombShaders8bpc();
	void LoadShaders();
	ShaderData*                                                     m_pShader;
    ShaderData*                                                     m_pShader2;
    ShaderData*                                                     m_pShader3;
};

class ThicknessRenderPS20_8bpc
{
public:
	ThicknessRenderProperties8BPC	m_RenderProperties;

	virtual bool SetToRenderFrontFaceDepths( ThicknessRenderTargetsPS20_8bpc * pTargets,
		FogTombShaders8bpc * pShaders );
	virtual bool SetToRenderBackFaceDepths( ThicknessRenderTargetsPS20_8bpc * pTargets );

	virtual bool SetToRenderFogSubtractConvertAndBlend( ThicknessRenderTargetsPS20_8bpc * pTargets,
		FogTombShaders8bpc * pShaders,
		GFXTexHandle pThicknessToColorTexture );
};

class FogTombScene
{
public:
	ThicknessRenderProperties * m_pProperties;

	Resource<TSShape> mShape;
    TSShapeInstance *mShapeInstance;

	F32		m_fThicknessToColorScaleFactor;

	GFXTexHandle*	m_ppFogColorRamp;

	MatrixF				m_matViewProj;					
	MatrixF				m_matWorldViewProjTranspose;	

	MatrixF * CalculateViewProjMatrix();
	MatrixF * GetViewProjMatrix()					{ return( & m_matViewProj ); };
	MatrixF * ApplyWorldToViewProjMatrixAndTranspose( const MatrixF * pWorld );
	MatrixF * GetWVPTransMatrix()					{ return( & m_matWorldViewProjTranspose ); };

public:
	virtual void Init();

	FogTombScene();
	virtual ~FogTombScene();
	void SetAllNull()
	{
		m_ppFogColorRamp		= NULL;
		m_pProperties	= NULL;
		m_fThicknessToColorScaleFactor = 204.0f;
		mShapeInstance    = NULL;
	}
};

#endif