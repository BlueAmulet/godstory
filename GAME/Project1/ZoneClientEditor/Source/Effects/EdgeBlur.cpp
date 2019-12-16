//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include <d3dx9math.h>
#include "EdgeBlur.h"
#include "gfx/D3D9/gfxD3D9Device.h"
#include "gfx/gfxDrawUtil.h"
#include "gfx/primBuilder.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
#include "renderInstance/renderInstMgr.h"
#include <Mmsystem.h>
#include "console/consoleTypes.h"
#include "renderInstance/RenderTranslucentMgr.h"
#include "platform/platformTimer.h"
#include "lightingSystem/synapseGaming/sgFormatManager.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK
GFXStateBlock* CEdgeBlur::mSetSB = NULL;
GFXStateBlock* CEdgeBlur::mCullCCWSB = NULL;

CEdgeBlur::CEdgeBlur()
{
	m_wBlurNum = 0;
	m_EdgeBlurPointList = NULL;
	m_wCurBlurNum = 0;
	m_bOpenBlur = TRUE;
	m_TextureHandle = NULL;
	mVertBuff = NULL;
	primBuffer = NULL;
	m_bFill = FALSE;
	m_pShader = NULL;
}

CEdgeBlur::~CEdgeBlur()
{
	Destory();
}

void CEdgeBlur::Create(U32 BlurNum)
{
	Destory();
	m_wBlurNum = BlurNum;
	U16 num = 2 * BlurNum;
	m_EdgeBlurPointList = new GFXVertexPCT[num];
	float InvBlurNum = 1.0f/float(BlurNum - 1);
	U16* pIndex = new U16[num];
	for (U16 i = 0; i < num; i++)
	{
		pIndex[i] = i;
	}
	for (U16 i = 0; i < BlurNum; i++)
	{
		m_EdgeBlurPointList[i * 2].texCoord = Point2F(InvBlurNum * i ,1.0f);
		m_EdgeBlurPointList[i * 2 + 1].texCoord = Point2F(m_EdgeBlurPointList[i * 2].texCoord.x ,0.0f);
	}
	mVertBuff.set( GFX, num, GFXBufferTypeDynamic );
	primBuffer.set(GFX, num, 0, GFXBufferTypeStatic );
	U16 *ibIndices = NULL;
	primBuffer.lock(&ibIndices);
	dMemcpy( ibIndices, pIndex, num * sizeof(U16) );
	primBuffer.unlock();
	SAFE_DELETE_ARRAY(pIndex);
	m_wCurBlurNum = 0;
}

void CEdgeBlur::Destory()
{
	m_TextureHandle.free();
	SAFE_DELETE_ARRAY(m_EdgeBlurPointList);
	mVertBuff = NULL;
	primBuffer = NULL;
}

void CEdgeBlur::SetTexture(StringTableEntry texname)
{
	if(!texname)
	{
		// 寻找默认贴图
		m_TextureHandle.free();
		return;
	}
	m_TextureHandle = GFXTexHandle(texname, &GFXDefaultStaticDiffuseProfile);
	if (!m_TextureHandle)
	{
		return;
	}
}

void CEdgeBlur::SetOpenBlurState(bool bdata)
{
	m_bOpenBlur = bdata;
	//if (m_bOpenBlur)
	//{
	//	m_wCurBlurNum = 0;
	//	m_bFill = FALSE;
	//}
}

bool CEdgeBlur::GetOpenBlurState()
{
	return m_bOpenBlur;
}

GFXTexHandle CEdgeBlur::GetTexture()
{
	return m_TextureHandle;
}

void CEdgeBlur::FrameMove()
{
	if (m_wCurBlurNum <= 0)
		return;

	if((m_wCurBlurNum >= m_wBlurNum) || (!m_bOpenBlur))
	{
		for (U32 i = 0, j = 1; i < m_wCurBlurNum - 1; i++, j++)
		{
			m_EdgeBlurPointList[i * 2].point = m_EdgeBlurPointList[j * 2].point;
			m_EdgeBlurPointList[i * 2 + 1].point = m_EdgeBlurPointList[j * 2 + 1].point;
			m_EdgeBlurPointList[i * 2].color = m_EdgeBlurPointList[j * 2].color;
			m_EdgeBlurPointList[i * 2 + 1].color = m_EdgeBlurPointList[j * 2 + 1].color;
		}
		m_wCurBlurNum -= 1;
	}
}

void CEdgeBlur::Add(Point3F min, Point3F max, ColorF color)
{
	if(m_bOpenBlur)
	{
		if (m_wCurBlurNum > 1)
		{
			Point3F len = max - m_EdgeBlurPointList[(m_wCurBlurNum - 2) * 2].point;
			if (len.len() > 10.0f)
				return;
			len = min - m_EdgeBlurPointList[(m_wCurBlurNum - 2) * 2 + 1].point;
			if (len.len() > 10.0f)
				return;
		}

		if (m_wCurBlurNum < m_wBlurNum)
			m_wCurBlurNum += 1;
		else
			m_bFill = TRUE;
		m_EdgeBlurPointList[(m_wCurBlurNum - 1) * 2].point = max;
		m_EdgeBlurPointList[(m_wCurBlurNum - 1) * 2 + 1].point = min;
		m_EdgeBlurPointList[(m_wCurBlurNum - 1) * 2].color = color;
		m_EdgeBlurPointList[(m_wCurBlurNum - 1) * 2 + 1].color = color;
	}
}

void CEdgeBlur::Render(Point3F &point)
{
	if(gClientSceneGraph->isReflectPass())
		return;

	if(m_wCurBlurNum <= 2)
		return;

	//FrameMove();

#ifdef STATEBLOCK
	AssertFatal(mSetSB, "CEdgeBlur::Render -- mSetSB cannot be NULL.");
	mSetSB->apply();
#endif

	GFXVertexPCT *verts = mVertBuff.lock();
	dMemcpy( verts, m_EdgeBlurPointList, 2 * m_wCurBlurNum * sizeof(GFXVertexPCT) );
	mVertBuff.unlock();

#ifdef STATEBLOCK
	AssertFatal(mCullCCWSB, "CEdgeBlur::Render -- mCullCCWSB cannot be NULL.");
	mCullCCWSB->apply();
#endif
	const MatrixF &objTrans = GFX->getWorldMatrix();
	MatrixF proj     = GFX->getProjectionMatrix();

	RenderInst *ri = gRenderInstManager.allocInst();
	ri->worldXform = gRenderInstManager.allocXform();
	*ri->worldXform = proj;

	ri->objXform = gRenderInstManager.allocXform();
	*ri->objXform = objTrans;
	ri->obj = (RenderableSceneObject*)this;

	ri->vertBuff = &mVertBuff;
	ri->primBuff = &primBuffer;
	ri->matInst = NULL;
	ri->translucent = true;
	ri->type = RenderInstManager::RIT_Translucent;
	ri->sortPoint = point;
	ri->particles = RenderTranslucentMgr::RENDERTRANSLUCENT_TYPE_EDGEBLUR;
	ri->fadeFlag = 1;

	ri->primBuffIndex = m_wCurBlurNum;

	ri->miscTex = m_TextureHandle;

	gRenderInstManager.addInst( ri );
}

void CEdgeBlur::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(0, GFXTSSColorArg1, GFXTATexture);
	GFX->setTextureStageState(0, GFXTSSColorArg2, GFXTATexture);
	GFX->setTextureStageState(0, GFXTSSAlphaOp, GFXTOPSelectARG1);
	GFX->setTextureStageState(0, GFXTSSAlphaArg1, GFXTATexture);
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaRef, 0);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreater);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mSetSB);

	//mCullCCWSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
	GFX->endStateBlock(mCullCCWSB);
}


void CEdgeBlur::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mCullCCWSB)
	{
		mCullCCWSB->release();
	}
}

void CEdgeBlur::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		mCullCCWSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void CEdgeBlur::shutdown()
{
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mCullCCWSB);
}

void CEdgeBlur::renderObject( SceneState *state, RenderInst *ri )
{
	if (!m_pShader)
	{
		if ( !Sim::findObject( "EdgeBlurShader", m_pShader ) )
			Con::warnf("EdgeBlurShader - failed to locate EdgeBlurShader shader EdgeBlurShaderBlendData!");
	}

	if(m_pShader)
		GFX->setShader(m_pShader->getShader());

	GFX->setAlphaBlendEnable(false);
	//GFX->setSrcBlend( GFXBlendSrcAlpha);
	//GFX->setDestBlend(GFXBlendOne);

	//GFX->setAlphaTestEnable(true);
	//GFX->setAlphaFunc(GFXCmpGreater);
	//GFX->setAlphaRef(0);

	GFX->setCullMode(GFXCullNone);

	GFX->setTexture(0, ri->miscTex);
	Point2I corner(0, 0);

	GFX->copyBBToSfxBuff();
	GFX->setTexture(1, GFX->getSfxBackBuffer());

	MatrixF prj = GFX->getProjectionMatrix();

	GFX->pushWorldMatrix();
	MatrixF mat = ( *ri->worldXform) * ( *ri->objXform);
	mat.transpose();
	GFX->setVertexShaderConstF(0, (float*)&mat, 4);

	GFX->setVertexBuffer( *ri->vertBuff );
	GFX->setPrimitiveBuffer( *ri->primBuff );
	GFX->drawIndexedPrimitive( GFXTriangleStrip, 0, 2 * ri->primBuffIndex, 0, (ri->primBuffIndex - 1) * 2);

	GFX->popWorldMatrix();
	GFX->setProjectionMatrix(prj);
	GFX->setTexture(0, NULL);
	GFX->setCullMode(GFXCullCCW);
	GFX->disableShaders();
}

//---------------------------------------------------------------------------
// ElectricityData
//---------------------------------------------------------------------------
IMPLEMENT_CO_DATABLOCK_V1(ElectricityData);
ElectricityData::ElectricityData()
{
	color.set(0.686275f, 0.4812f, 1.0f, 1.0f);
	glowStrength = 144.0f;
	height = 0.44f;
	glowFallOff = 0.024f;
	speed = 1.86f;
	sampleDist = 0.0076f;
	ambientGlow = 0.5f;
	ambientGlowHeightScale = 1.68f;
	vertNoise = 0.78f;
	time = 0;
	widthRate = 0.01f;
	textureName = NULL;
}
ElectricityData::~ElectricityData()
{
}

void ElectricityData::initPersistFields()
{
	addField("color",			TypeColorF,		Offset(color,					ElectricityData));
	addField("Height",			TypeF32,		Offset(height,					ElectricityData));
	addField("GlowStrength",	TypeF32,		Offset(glowStrength,			ElectricityData));
	addField("GlowFallOff",		TypeF32,		Offset(glowFallOff,				ElectricityData));
	addField("Speed",			TypeF32,		Offset(speed,					ElectricityData));
	addField("SampleDist",		TypeF32,		Offset(sampleDist,				ElectricityData));
	addField("AmbientGlow",		TypeF32,		Offset(ambientGlow,				ElectricityData));
	addField("AmbientGlowHeightScale",TypeF32,	Offset(ambientGlowHeightScale,	ElectricityData));
	addField("VertNoise",		TypeF32,		Offset(vertNoise,				ElectricityData));
	addField("WidthRate",		TypeF32,		Offset(widthRate,				ElectricityData));
	addField("TextureName",		TypeFilename,	Offset(textureName,				ElectricityData));
}

//---------------------------------------------------------------------------
// Electricity
//---------------------------------------------------------------------------
Electricity::Electricity()
{
	mTypeMask |= StaticObjectType | StaticRenderedObjectType;
	m_Color = ColorF(0.686275f, 0.4812f, 1.0f, 1.0f);
	m_fGlowStrength = 144.0f;
	m_fHeight = 0.44f;
	m_fGlowFallOff = 0.024f;
	m_fSpeed = 1.86f;
	m_fSampleDist = 0.0076f;
	m_fAmbientGlow = 0.5f;
	m_fAmbientGlowHeightScale = 1.68f;
	m_fVertNoise = 0.78f;
	m_fTime = 0.0f;
	m_vPosStart = Point3F(0.0f, 0.0f, 0.0f);
	m_vPosEnd = Point3F(0.0f, 0.0f, 0.0f);
	m_bOpen = true;
	m_pTexture = NULL;
	m_dwNowTime = timeGetTime();
    m_fWidthRate = 0.1f;
	m_pShader = NULL;
	m_TextureName = NULL;
	m_pDataBlock = NULL;
}

Electricity::~Electricity()
{
	SAFE_RELEASE(m_pTexture);
}

bool Electricity::onAdd()
{
	if(!m_pDataBlock || !Parent::onAdd())
		return true;

	// 加载图片
	if(m_pDataBlock->textureName && m_pDataBlock->textureName[0])
		SetTexture(m_pDataBlock->textureName);

	// 加载Shader
	if (!m_pShader)
	{
		if ( !Sim::findObject( "Electricity", m_pShader ) )
			Con::warnf("Electricity - failed to locate Electricity shader ElectricityBlendData!");
	}

	addToScene();
	return true;
}

void Electricity::onRemove()
{
	removeFromScene();
	Parent::onRemove();
}

bool Electricity::prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState/* =false */)
{
	// 首先更新两点的坐标
	if(gClientSceneGraph->isReflectPass())
		return false;

	if(m_pObjStart)
	{
		m_vPosStart = m_pObjStart->getWorldSphere().center;
	}
	if(m_pObjEnd)
	{
		m_vPosEnd = m_pObjEnd->getWorldSphere().center;
	}
	// 更新自己的位置信息
	SetPoints(m_vPosStart, m_vPosEnd);

	Render();
	return true;
}

void Electricity::setCarrier(SceneObject* pStart, SceneObject* pEnd)
{
	m_pObjStart = pStart;
	m_pObjEnd = pEnd;
	if(m_pObjStart)
		m_vPosStart = m_pObjStart->getWorldSphere().center;
	if(m_pObjEnd)
		m_vPosEnd = m_pObjEnd->getWorldSphere().center;
	// 更新自己的位置信息
	SetPoints(m_vPosStart, m_vPosEnd);
}

void Electricity::calcBox()
{
	Point3F pos = (m_vPosStart + m_vPosEnd)/2.0f;
	setPosition(pos);
	Box3F tmp;
	tmp.max.x = getMax(m_vPosStart.x - pos.x, m_vPosEnd.x - pos.x);
	tmp.max.y = getMax(m_vPosStart.y - pos.y, m_vPosEnd.y - pos.y);
	tmp.max.z = getMax(m_vPosStart.z - pos.z, m_vPosEnd.z - pos.z);
	tmp.min.x = getMin(m_vPosStart.x - pos.x, m_vPosEnd.x - pos.x);
	tmp.min.y = getMin(m_vPosStart.y - pos.y, m_vPosEnd.y - pos.y);
	tmp.min.z = getMin(m_vPosStart.z - pos.z, m_vPosEnd.z - pos.z);
	if(tmp.isValidBox())
		mObjBox = tmp;
}

void Electricity::setData(ElectricityData* pData)
{
	m_pDataBlock = pData;
	if(isProperlyAdded())
		SetTexture(m_pDataBlock->textureName);
}

void Electricity::SetPoints(Point3F start, Point3F end)
{
	m_vPosStart = start;
	m_vPosEnd = end;
	calcBox();
}

void Electricity::GetPoints(Point3F* start, Point3F* end)
{
	*start = m_vPosStart;
	*end = m_vPosEnd;
}

void Electricity::SetTexture(StringTableEntry texname)
{
	SAFE_RELEASE(m_pTexture);
	char filename[1024];
	Platform::makeFullPathName(texname, filename, sizeof(filename));
	GFX->CreateVolumeTexture(filename, &m_pTexture);
}

void Electricity::renderObject( SceneState *state, RenderInst *ri )
{
	U32 nowtime = timeGetTime();
	m_fTime += (float)(nowtime - m_dwNowTime)/1000.0f; 
	m_dwNowTime = nowtime;
	if(m_pShader)
		GFX->setShader(m_pShader->getShader());
	GFX->setTextureStageMagFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageMinFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageMipFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageAddressModeU(0, GFXAddressWrap);
	GFX->setTextureStageAddressModeV(0, GFXAddressWrap);
	GFX->setTextureStageAddressModeW(0, GFXAddressWrap);

	GFX->setAlphaBlendEnable(true);
	GFX->setSrcBlend( GFXBlendSrcAlpha);
	GFX->setDestBlend(GFXBlendOne);
	GFX->setAlphaTestEnable(true);
	GFX->setAlphaFunc(GFXCmpGreater);
	GFX->setAlphaRef(0);
	GFX->setCullMode(GFXCullNone);

	GFX->setPixelShaderConstF(1, m_Color, 1);
	Point4F cons = Point4F(m_fGlowStrength, 0.0f, 0.0f, 0.0f);
	GFX->setPixelShaderConstF(2, cons, 1);
	cons = Point4F(m_fHeight, 0.0f, 0.0f, 0.0f);
	GFX->setPixelShaderConstF(3, cons, 1);
	cons = Point4F(m_fGlowFallOff, 0.0f, 0.0f, 0.0f);
	GFX->setPixelShaderConstF(4, cons, 1);
	cons = Point4F(m_fSpeed, 0.0f, 0.0f, 0.0f);
	GFX->setPixelShaderConstF(5, cons, 1);
	cons = Point4F(m_fSampleDist, 0.0f, 0.0f, 0.0f);
	GFX->setPixelShaderConstF(6, cons, 1);
	cons = Point4F(m_fAmbientGlow, 0.0f, 0.0f, 0.0f);
	GFX->setPixelShaderConstF(7, cons, 1);
	cons = Point4F(m_fAmbientGlowHeightScale, 0.0f, 0.0f, 0.0f);
	GFX->setPixelShaderConstF(8, cons, 1);
	cons = Point4F(m_fVertNoise, 0.0f, 0.0f, 0.0f);
	GFX->setPixelShaderConstF(9, cons, 1);
	cons = Point4F(m_fTime, 0.0f, 0.0f, 0.0f);
	GFX->setPixelShaderConstF(0, cons, 1);

	GFX->SetVolumeTexture(0, m_pTexture);

	MatrixF mat = GFX->getProjectionMatrix() * GFX->getWorldMatrix();
	mat.transpose();
	GFX->setVertexShaderConstF(0, (float*)&mat, 4);

	float xdis = m_vPosEnd.x - m_vPosStart.x;
	float ydis = m_vPosEnd.y - m_vPosStart.y;
	float x,y;
	if ((xdis >= 0) && (ydis >= 0))
	{
		x = - ydis;
		y = xdis;
	}
	else if ((xdis >= 0) && (ydis < 0))
	{
		x = ydis;
		y = -xdis;
	}
	else if ((xdis < 0) && (ydis >= 0))
	{
		x = - ydis;
		y = xdis;
	}
	else //if ((xdis < 0) && (ydis < 0))
	{
		x = -ydis;
		y = xdis;
	}
	x *= m_fWidthRate;
	y *= m_fWidthRate;
	PrimBuild::color(ColorF(1.0f, 1.0f, 1.0f));
	PrimBuild::begin(GFXTriangleStrip, 4);
	PrimBuild::texCoord2f(-1.0f, 1.0f);
	PrimBuild::vertex3fv(m_vPosStart + Point3F(x, y, 0.0f));
	PrimBuild::texCoord2f(-1.0f, -1.0f);
	PrimBuild::vertex3fv(m_vPosStart + Point3F(-x, -y, 0.0f));
	PrimBuild::texCoord2f(1.0f, 1.0f);
	PrimBuild::vertex3fv(m_vPosEnd + Point3F(x, y, 0.0f));
	PrimBuild::texCoord2f(1.0f, -1.0f);
	PrimBuild::vertex3fv(m_vPosEnd + Point3F(-x, -y, 0.0f));
	PrimBuild::end(false);

	/*RectI rect(-1, 1, 1, -1);
	PrimBuild::color(ColorF(1.0f, 1.0f, 1.0f));
	PrimBuild::begin(GFXTriangleStrip, 4);
	PrimBuild::texCoord2f(-1.0f, -1.0f);
	PrimBuild::vertex2i(rect.point.x, rect.point.y);
	PrimBuild::texCoord2f(1.0f, -1.0f);
	PrimBuild::vertex2i(rect.extent.x, rect.point.y);
	PrimBuild::texCoord2f(-1.0f, 1.0f);
	PrimBuild::vertex2i(rect.point.x, rect.extent.y);
	PrimBuild::texCoord2f(1.0f, 1.0f);
	PrimBuild::vertex2i(rect.extent.x, rect.extent.y);
	PrimBuild::end(false);*/

	GFX->setTextureStageMagFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageMinFilter(0, GFXTextureFilterLinear);
	GFX->setTextureStageMipFilter(0, GFXTextureFilterNone);
	GFX->SetVolumeTexture(0, NULL);
	GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
	GFX->setTextureStageAddressModeV(0, GFXAddressClamp);
	GFX->setTextureStageAddressModeW(0, GFXAddressClamp);
	GFX->setAlphaBlendEnable(false);
	GFX->setSrcBlend( GFXBlendSrcAlpha);
	GFX->setDestBlend(GFXBlendInvSrcAlpha);
	GFX->setAlphaTestEnable(false);
	GFX->setCullMode(GFXCullCCW);
	GFX->disableShaders();
}

void Electricity::Render()
{
	if(m_bOpen)
	{
		const MatrixF &objTrans = GFX->getWorldMatrix();
		MatrixF proj     = GFX->getProjectionMatrix();

		RenderInst *ri = gRenderInstManager.allocInst();
		ri->worldXform = gRenderInstManager.allocXform();
		*ri->worldXform = proj;

		ri->obj = this;

		ri->translucent = true;
		ri->type = RenderInstManager::RIT_Translucent;
		ri->sortPoint = (m_vPosStart + m_vPosEnd)/2;
		ri->particles = RenderTranslucentMgr::RENDERTRANSLUCENT_TYPE_ELECTRICITY;

		gRenderInstManager.addInst( ri );
	}
}

void Electricity::SetOpenState(bool bdata)
{
	m_bOpen = bdata;
}

bool Electricity::GetOpenState()
{
	return m_bOpen;
}

void Electricity::SetWidthRate(F32 data)
{
    m_fWidthRate = data;
}

F32 Electricity::GetWidthRate()
{
	return m_fWidthRate;
}

ThicknessRenderProperties::ThicknessRenderProperties()
{
	m_dwVolumeColorToScreenSrcBlend		=	GFXBlendOne;
	m_dwVolumeColorToScreenDestBlend	=	GFXBlendInvSrcColor;
	m_dwVolumeColorToScreenBlendOp		=	GFXBlendOpAdd;

	SetParameters( DEFAULT_THICKNESS_TO_COLOR_SCALE, DEFAULT_NEARCLIP, DEFAULT_FARCLIP );

	m_ppTexFogThicknessToColor				= NULL;
	m_fTexCrdPrecisionFactor				= 1.0f;
}

bool ThicknessRenderProperties::SetClipPlanes( F32 fNearClip, F32 fFarClip )
{
	if( fFarClip > fNearClip )
	{
		m_fFarClip = fFarClip;
		m_fNearClip = fNearClip;
	}
	else
	{
		m_fFarClip = fNearClip;
		m_fNearClip = fFarClip;		
	}

	m_NormalizeWDepth = Point4F(	1.0f / ( m_fFarClip - m_fNearClip ),
		- m_fNearClip / ( m_fFarClip - m_fNearClip ),
		0.0f,
		1.0f );
	return( true );
}


bool ThicknessRenderProperties::SetParameters( F32 fThicknessToColorScale,
												 F32 fNearClip,
												 F32 fFarClip )
{
	bool hr = true;
	SetClipPlanes( fNearClip, fFarClip );
	SetThicknessToColorTexCoordScale( fThicknessToColorScale );
	m_fScale = 1.0f;
	m_fTexCrdPrecisionFactor	= 1.0f;
	return( true );
}

void ThicknessRenderProperties::SetThicknessToColorTexCoordScale( F32 fTexCrdScale )
{
	m_fTexCrdScale = fTexCrdScale;
}

void ThicknessRenderProperties8BPC::SetAllNull()
{
	m_pTexRedGreenRamp			= NULL;
	m_pTexBlueRamp				= NULL;
	m_ppTexRedGreenRamp			= NULL;
	m_ppTexBlueRamp				= NULL;
}

ThicknessRenderProperties8BPC::ThicknessRenderProperties8BPC()
{
	SetAllNull();
}

ThicknessRenderProperties8BPC::~ThicknessRenderProperties8BPC()
{
	FreeRampTextures();
	SetAllNull();
}

bool ThicknessRenderProperties8BPC::FreeRampTextures()
{
	bool hr = true;
	m_pTexRedGreenRamp = NULL;
	m_pTexBlueRamp = NULL;
	return( true );
}

bool ThicknessRenderProperties8BPC::SetParameters( F32 fThicknessToColorScale,
													 F32 fNearClip, F32 fFarClip, MaxFogDepthComplexity depth_relation, bool bUsePS20)
{
	bool hr = true;

	ThicknessRenderProperties::SetParameters( fThicknessToColorScale, fNearClip, fFarClip );

	m_bUsePS20 = bUsePS20;

	m_MaxFogDepthComplexity = depth_relation;
	F32 base_scale = 10.0f;
	F32 diff = 2.0f;

	switch( m_MaxFogDepthComplexity )
	{
	case PVF_1_SURFACES_24_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_256;
		m_fTexCrdPrecisionFactor = base_scale / (diff*diff*diff*diff);
		break;
	case PVF_2_SURFACES_21_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_128;
		m_fTexCrdPrecisionFactor = base_scale / (diff*diff*diff);
		break;
	case PVF_4_SURFACES_18_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_64;
		m_fTexCrdPrecisionFactor = base_scale / (diff*diff);
		break;
	case PVF_8_SURFACES_15_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_32;
		m_fTexCrdPrecisionFactor = base_scale / diff;
		break;
	case PVF_16_SURFACES_12_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_16;
		m_fTexCrdPrecisionFactor = base_scale;
		break;
	case PVF_32_SURFACES_9_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_8;
		m_fTexCrdPrecisionFactor = base_scale * diff;
		break;
	case PVF_64_SURFACES_6_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_4;
		m_fTexCrdPrecisionFactor = base_scale * diff * diff;
		break;
	case PVF_128_SURFACES_3_BIT_DEPTH :
		m_DepthValuesPerColorChannel = PVF_2;
		m_fTexCrdPrecisionFactor = base_scale * diff * diff * diff;
		break;

	default :
		return( false );
		break;
	}

	float little_bands;
	switch( m_DepthValuesPerColorChannel )
	{
	case PVF_256 :
		m_fBitReduce = 1.0f;
		little_bands = 256.0f;
		break;
	case PVF_128 :
		m_fBitReduce = 0.5f;
		little_bands = 128.0f;
		break;
	case PVF_64 :
		m_fBitReduce = 0.25f;
		little_bands = 64.0f;
		break;
	case PVF_32 :
		m_fBitReduce = 0.125f;
		little_bands = 32.0f;
		break;
	case PVF_16 :
		m_fBitReduce = 0.0625f;
		little_bands = 16.0f;
		break;
	case PVF_8 :
		m_fBitReduce = 0.03125f;
		little_bands = 8.0f;
		break;
	case PVF_4 :
		m_fBitReduce = 0.015625;
		little_bands = 4.0f;
		break;
	case PVF_2 :
		m_fBitReduce = 0.0078125;
		little_bands = 2.0f;
		break;

	default:
		m_fBitReduce = 0.0625f;
		little_bands = 16.0f;
	}

	m_fGrnRamp = m_fBitReduce * 256.0f;		
	m_fBluRamp = m_fGrnRamp * m_fGrnRamp;
	m_fGrnRamp = little_bands;
	m_fBluRamp = little_bands * little_bands;

	SetThicknessToColorTexCoordScale( fThicknessToColorScale );

	CreateRampTextures();

	return( true );
}

void ThicknessRenderProperties8BPC::SetThicknessToColorTexCoordScale( F32 fTexCrdScale )
{
	m_fTexCrdScale = fTexCrdScale;

	m_vTexAddrWeights = Point4F(	1.0f, 
		1.0f / m_fGrnRamp,
		1.0f / m_fBluRamp,
		-1.0f / m_fGrnRamp );
	m_vTexAddrWeights.x *= m_fTexCrdScale * m_fTexCrdPrecisionFactor;
	m_vTexAddrWeights.y *= m_fTexCrdScale * m_fTexCrdPrecisionFactor;
	m_vTexAddrWeights.z *= m_fTexCrdScale * m_fTexCrdPrecisionFactor;
	m_vTexAddrWeights.w *= m_fTexCrdScale * m_fTexCrdPrecisionFactor;

	if( !m_bUsePS20 )
	{
		m_vTexAddrWeights.x *= 0.5f;
		m_vTexAddrWeights.y *= 0.5f;
		m_vTexAddrWeights.z *= 0.5f;
		m_vTexAddrWeights.w *= 0.5f;
	}
}


bool	ThicknessRenderProperties8BPC::CreateRampTextures()
{
	bool hr = true;

	S32 size_divide = (S32)( 1.0f / m_fBitReduce );

	CreateGradient2D( & m_pTexRedGreenRamp,
		256 / size_divide, 256 / size_divide,
		0, (byte)( 255.0f * m_fBitReduce ),
		Point3F( 1.0f, 0.0f, 0.0f ),	
		Point3F( 0.0f, 1.0f, 0.0f )  );	


	U32 upper_color;
	upper_color = D3DCOLOR_ARGB( 0, 0, 1, 0 );	 

	S32 width = 8 * 256 / size_divide;

	if( width > 1024 )
		width = 1024;

	CreateGradientDithered( & m_pTexBlueRamp,
		width, 64,
		0, (byte)( 255.0f * m_fBitReduce ),
		Point3F( 0.0f, 0.0f, 1.0f ),	
		& upper_color );

	m_ppTexRedGreenRamp = &m_pTexRedGreenRamp;
	m_ppTexBlueRamp		= &m_pTexBlueRamp;


	return( hr );
}

void ThicknessRenderProperties8BPC::CreateGradient2D( GFXTexHandle* ppTex2D,
													 S32 width, S32 height, char lower, char upper,
													 Point3F col_mask_u, Point3F col_mask_v )
{
	*ppTex2D = NULL;

	*ppTex2D = GFXTexHandle(width, height, GFXFormatR8G8B8A8, &GFXDefaultStaticDiffuseProfile);

	GFXTexHandle pTex = *ppTex2D;
	GFXLockedRect* d3dlr = pTex.lock(0);
	byte * pixel;
	byte * pBase = (BYTE*) d3dlr->bits;
	S32 bytes_per_pixel = 4;

	byte colu;
	byte colv;
	S32 i,j;
	Point3F sum;

	for( j=0; j < height; j++ )
	{
		for( i=0; i < width; i++ )
		{
			pixel = pBase + j * d3dlr->pitch + i * bytes_per_pixel;

			colu = (byte)( lower + (upper-lower) * (F32)i / ((F32)width - 1.0f));
			colv = (byte)( lower + (upper-lower) * (F32)j / ((F32)height - 1.0f));
			sum = colu * col_mask_u + colv * col_mask_v;

			*((U32*)pixel) = D3DCOLOR_ARGB( 0, (BYTE)( sum.x ), (BYTE)( sum.y ), (BYTE)( sum.z ) );
		}	
	}

	pTex->unlock();
}



void ThicknessRenderProperties8BPC::CreateGradientDithered( GFXTexHandle* ppTex,
														   S32 width, S32 height,
														   char lower, char upper,
														   Point3F color_mask,
														   const U32 * pDither_upper )
{	
	bool bSaveTexture = false;

	bool bDitherLowerToBlueAndAlpha = false;
	if( m_bUsePS20 )
	{
		bDitherLowerToBlueAndAlpha = true;
	}

	*ppTex = NULL;

	*ppTex = GFXTexHandle(width, height, GFXFormatR8G8B8A8, &GFXDefaultStaticDiffuseProfile);

	GFXTexHandle pTex = *ppTex;
	GFXLockedRect* d3dlr = pTex.lock(0);

	byte * pixel;
	byte * pBase = (BYTE*) d3dlr->bits;

	S32 bytes_per_pixel = 4;

	S32 val;
	S32 dithered_val;
	U32 color;
	S32 noiseinc;
	F32 fcol;
	F32 frand;
	S32 i,j;

	for( j=0; j < height; j++ )
	{
		for( i=0; i < width; i++ )
		{
			pixel = pBase + j * d3dlr->pitch + i * bytes_per_pixel;

			val = (S32)( lower + (upper+1-lower) * ((F32)i / ((F32)width)));
			fcol = lower + (upper+1-lower) * ((F32)i / ((F32)width));

			fcol = (F32) fmod( fcol, 1.0f );
			frand = ((F32)rand()) / ((F32)RAND_MAX);
			if( fcol < 0.5f )
			{
				fcol = 0.5f - fcol;
				if( frand < fcol )
					noiseinc = -1;
				else
					noiseinc = 0;
			}
			else
			{
				fcol = fcol - 0.5f;			
				if( frand > fcol )
					noiseinc = 0;
				else
					noiseinc = 1;
			}

			dithered_val = val + noiseinc;
			if( dithered_val < 0 )
			{
				if( bDitherLowerToBlueAndAlpha )
				{
					color = D3DCOLOR_ARGB( 1, 
						(BYTE)( upper * color_mask.x ),
						(BYTE)( upper * color_mask.y ),
						(BYTE)( upper * color_mask.z ) );
				}
				else
				{
					color = 0x00;
				}
			}
			else if( dithered_val > upper )
			{
				if( pDither_upper != NULL )
				{
					color = *pDither_upper;
				}
				else
				{
					dithered_val = dithered_val - 1;
					color = D3DCOLOR_ARGB( 0, 
						(BYTE)( dithered_val * color_mask.x),
						(BYTE)( dithered_val * color_mask.y),
						(BYTE)( dithered_val * color_mask.z) );
				}
			}
			else
			{
				color = D3DCOLOR_ARGB( 0, 
					(BYTE)( dithered_val * color_mask.x),
					(BYTE)( dithered_val * color_mask.y),
					(BYTE)( dithered_val * color_mask.z) );
			}

			if( j == 0 )
			{
				color = D3DCOLOR_ARGB( 0,
					(BYTE)( val * color_mask.x),
					(BYTE)( val * color_mask.y),
					(BYTE)( val * color_mask.z) );
			}


			*((U32*)pixel) = color;
		}	
	}

	pTex->unlock();
}

ThicknessRenderTargetsPS20_8bpc::ThicknessRenderTargetsPS20_8bpc()
{
	SetAllNull();
}

ThicknessRenderTargetsPS20_8bpc::~ThicknessRenderTargetsPS20_8bpc()
{
	Free();
	SetAllNull();
}

bool ThicknessRenderTargetsPS20_8bpc::Free()
{
	bool hr = true;
	m_pTexOccludersDepth = NULL;
	m_pTexFrontFacesDepth = NULL;
	m_pSurfFrontFacesDepth = NULL;
	m_pTexBackFacesDepth = NULL;
	m_pSurfBackFacesDepth = NULL;
	return( hr );
}

bool ThicknessRenderTargetsPS20_8bpc::Initialize( U32 x_resolution, U32 y_resolution )
{
	bool hr = true;
	Free();

	m_pTexOccludersDepth = GFXTexHandle(x_resolution, y_resolution, GFXFormatR8G8B8A8, &GFXDefaultRenderTargetProfile);
	
	m_pTexFrontFacesDepth = GFXTexHandle(x_resolution, y_resolution, 
		GFXFormatR8G8B8A8, &GFXDefaultRenderTargetProfile);
	m_pSurfFrontFacesDepth = GFX->allocRenderToTextureTarget();
	m_pSurfFrontFacesDepth->attachTexture(GFXTextureTarget::Color0, m_pTexFrontFacesDepth );

	m_pTexBackFacesDepth = GFXTexHandle(x_resolution, y_resolution, GFXFormatR8G8B8A8, &GFXDefaultRenderTargetProfile);
	m_pSurfBackFacesDepth = GFX->allocRenderToTextureTarget();
	m_pSurfBackFacesDepth->attachTexture(GFXTextureTarget::Color0, m_pTexBackFacesDepth );

	F32 half_w = 0.50f * 1.0f / ((F32)x_resolution);
	F32 half_h = 0.50f * 1.0f / ((F32)y_resolution);
	m_HalfTexelSize = Point4F( half_w, half_h, 0.0f, 0.0f );

	return( hr );
}


bool ThicknessRenderTargetsPS20_8bpc::SetToDefaultBackbuffers()
{
	bool hr = true;
	GFX->popActiveRenderTarget();
	return( hr );
}

bool	ThicknessRenderTargetsPS20_8bpc::SetToFrontFacesDepth()
{
	bool hr = true;
	GFX->setActiveRenderTarget( m_pSurfFrontFacesDepth );
	return( hr );
}

bool	ThicknessRenderTargetsPS20_8bpc::SetToBackFacesDepth()
{
	bool hr = true;
	GFX->setActiveRenderTarget( m_pSurfBackFacesDepth );
	return( hr );
}

FogTombShaders8bpc::FogTombShaders8bpc()
{
    m_pShader = NULL;
    m_pShader2 = NULL;
    m_pShader3 = NULL;
}

FogTombShaders8bpc::~FogTombShaders8bpc()
{

}

void FogTombShaders8bpc::LoadShaders()
{
	if (!m_pShader)
	{
		if ( !Sim::findObject( "FogPolygonVolumes1", m_pShader ) )
			Con::warnf("FogPolygonVolumes1 - failed to locate FogPolygonVolumes1 shader");
	}
	if (!m_pShader2)
	{
		if ( !Sim::findObject( "FogPolygonVolumes2", m_pShader2 ) )
			Con::warnf("FogPolygonVolumes2 - failed to locate FogPolygonVolumes2 shader");
	}
	if (!m_pShader3)
	{
		if ( !Sim::findObject( "FogPolygonVolumes3", m_pShader3 ) )
			Con::warnf("FogPolygonVolumes3 - failed to locate FogPolygonVolumes3 shader");
	}
}

bool ThicknessRenderPS20_8bpc::SetToRenderFrontFaceDepths( ThicknessRenderTargetsPS20_8bpc * pTargets,
															 FogTombShaders8bpc * pShaders )
{
	bool hr = true;

	GFX->setZEnable(false); 
	GFX->setZWriteEnable( false );
	GFX->setZFunc(GFXCmpAlways);

	GFX->setAlphaBlendEnable(true);
	GFX->setSrcBlend( GFXBlendOne);
	GFX->setDestBlend(GFXBlendOne);
	GFX->setBlendOp(GFXBlendOpAdd);

	pTargets->SetToFrontFacesDepth();

    const int proj_tex_stage = 3;

	GFXTexHandle pTexOccludersDepth;
	pTexOccludersDepth = pTargets->m_pTexOccludersDepth;
	GFX->setTexture( proj_tex_stage, pTexOccludersDepth );

	GFX->clear( GFXClearTarget,
		ColorI( 0, 0, 0 ,0), 1.0, 0 );

	if(pShaders->m_pShader2)
		GFX->setShader(pShaders->m_pShader2->getShader());

	GFX->setPixelShaderConstF( 4, (float*)&m_RenderProperties.m_vTexAddrWeights, 1 );
	return( hr );
}

bool ThicknessRenderPS20_8bpc::SetToRenderBackFaceDepths( ThicknessRenderTargetsPS20_8bpc * pTargets )
{
	bool hr = true;

	pTargets->SetToBackFacesDepth();
	GFX->clear( GFXClearTarget,
		ColorI( 0, 0, 0 ,0), 1.0, 0 );

	return( hr );
}

bool ThicknessRenderPS20_8bpc::SetToRenderFogSubtractConvertAndBlend( ThicknessRenderTargetsPS20_8bpc * pTargets,
																		FogTombShaders8bpc * pShaders,
																		GFXTexHandle pThicknessToColorTexture )
{
	bool hr = true;
	
	GFX->setZEnable(false); 
	GFX->setCullMode( GFXCullNone );
	GFX->setAlphaBlendEnable(true);

	GFX->setTextureStageMagFilter( 0, GFXTextureFilterPoint );
	GFX->setTextureStageMinFilter( 0, GFXTextureFilterPoint );
	GFX->setTextureStageMipFilter( 0, GFXTextureFilterNone );

	GFX->setTextureStageMagFilter( 1, GFXTextureFilterPoint );
	GFX->setTextureStageMinFilter( 1, GFXTextureFilterPoint );
	GFX->setTextureStageMipFilter( 1, GFXTextureFilterNone );

	GFX->setTextureStageMagFilter( 2, GFXTextureFilterLinear );
	GFX->setTextureStageMinFilter( 2, GFXTextureFilterLinear );
	GFX->setTextureStageMipFilter( 2, GFXTextureFilterNone );

	GFX->setTextureStageAddressModeU(0, GFXAddressWrap);
	GFX->setTextureStageAddressModeV(0, GFXAddressWrap);
	GFX->setTextureStageAddressModeU(1, GFXAddressWrap);
	GFX->setTextureStageAddressModeV(1, GFXAddressWrap);
	GFX->setTextureStageAddressModeU(2, GFXAddressClamp);
	GFX->setTextureStageAddressModeV(2, GFXAddressClamp);

	pTargets->SetToDefaultBackbuffers();

	GFXTexHandle		pTexFrontFaceDepths;
	pTexFrontFaceDepths = pTargets->m_pTexFrontFacesDepth;

	GFXTexHandle		pTexBackFaceDepths;
	pTexBackFaceDepths = pTargets->m_pTexBackFacesDepth;

	GFX->setTexture( 0, pTexBackFaceDepths );			
	GFX->setTexture( 1, pTexFrontFaceDepths );		
	GFX->setTexture( 2, pThicknessToColorTexture );

	GFX->setPixelShaderConstF( 4, (float*)&m_RenderProperties.m_vTexAddrWeights, 1 );

	if(pShaders->m_pShader3)
		GFX->setShader(pShaders->m_pShader3->getShader());

	GFX->setSrcBlend( m_RenderProperties.m_dwVolumeColorToScreenSrcBlend);
	GFX->setDestBlend(m_RenderProperties.m_dwVolumeColorToScreenDestBlend);
	GFX->setBlendOp(m_RenderProperties.m_dwVolumeColorToScreenBlendOp);

	return( hr );
}

FogTombScene::FogTombScene()
{

	SetAllNull();
}

FogTombScene::~FogTombScene()
{
	SAFE_DELETE( mShapeInstance );
}

MatrixF * FogTombScene::CalculateViewProjMatrix()
{
	m_matViewProj = GFX->getProjectionMatrix() * GFX->getWorldMatrix();
	return( & m_matViewProj );
}

MatrixF * FogTombScene::ApplyWorldToViewProjMatrixAndTranspose( const MatrixF * pWorld )
{
	MatrixF * pViewProj = GetViewProjMatrix();
	m_matWorldViewProjTranspose = (*pWorld) * (*pViewProj);
	m_matWorldViewProjTranspose.transpose();
	return( & m_matWorldViewProjTranspose );
}

void FogTombScene::Init()
{
    *m_ppFogColorRamp = GFXTexHandle("VolumeFogRamp_03tomb.tga", &GFXDefaultStaticDiffuseProfile);
	if (!mShape)
	{
		mShape = ResourceManager->load("");
	}
    mShapeInstance = new TSShapeInstance(mShape, true);

	if( m_pProperties != NULL )
	{
		m_pProperties->SetClipPlanes( 0.1f, 50.0f );
	}
}

