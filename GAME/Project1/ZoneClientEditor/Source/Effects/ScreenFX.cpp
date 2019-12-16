//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "gfx/D3D9/gfxD3D9Device.h"
#include "gfx/gfxDrawUtil.h"
#include "Effects/ScreenFX.h"
#include "gfx/primBuilder.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK

#define DEF_TIME 500

GFXStateBlock* CScreenFXMgr::mSetSB = NULL;

CScreenFXMgr g_ScreenFXMgr;

GFXVertexPT* CScreenFXMgr::mtempBuff = NULL;
bool CScreenFXMgr::mbInit = false;

IScreenFX::IScreenFX()
{
	m_TextureHandle = NULL;
	mAdded = false;
}

IScreenFX::~IScreenFX()
{
	m_TextureHandle = NULL;
}


CScreenFX::CScreenFX()
{
	mDuration = 0.0f;
	mStartAlpha = 0.0f;
	mEndAlpha = 1.0f;
	mAlphaStep = 0.0f;
	mElapsedTime = 0.0f;
	mFadeIn = true;
}

CScreenFX::CScreenFX(StringTableEntry texname, F32 Duration, F32 StartAlpha, F32 EndAlpha)
{
	m_TextureHandle = GFXTexHandle(texname, &GFXDefaultStaticDiffuseProfile);
	if (!m_TextureHandle)
	{
		return;
	}
	mDuration = Duration;

	if(mEndAlpha > mStartAlpha)
	{
		mStartAlpha = StartAlpha;
		mEndAlpha = EndAlpha;
		mFadeIn = true;
	}
	else
	{
		mStartAlpha = EndAlpha;
		mEndAlpha = StartAlpha;
		mFadeIn = false;
	}

	mAlphaStep = (mEndAlpha - mStartAlpha) / mDuration;
	mElapsedTime = 0.0f;
} 

void CScreenFX::setFadeIn(bool flag)
{
	mFadeIn = flag;
	mElapsedTime = 0.0f;

	if(mFadeIn)
	{
		mDuration = 0.0f;
	}
	else
	{
		mDuration = DEF_TIME;
	}
}

bool CScreenFX::AdvanceTime(S32 timeDelta)
{
	mElapsedTime += (F32)timeDelta;

	if(mDuration==0.0f)
		return true;

	return (mElapsedTime > mDuration) ? false : true;
}

F32 CScreenFX::Render()
{
	if(mFadeIn)
		nowalpha = mClampF(mStartAlpha + mAlphaStep * mElapsedTime,mStartAlpha,mEndAlpha);
	else
		nowalpha = mClampF(mEndAlpha - mAlphaStep * mElapsedTime,mStartAlpha,mEndAlpha);

	return nowalpha;
}

void CScreenFX::onRemove()
{
	IScreenFX::onRemove();
	delete this;
}

CScreenMask::CScreenMask()
{
	reset();
}

void CScreenMask::onRemove()
{
	IScreenFX::onRemove();
}

void CScreenMask::reset()
{
	mDuration = 0.0f;
	mElapsedTime = 0.0f;
	mStartAlpha = 0.0f;
	mEndAlpha = 1.0f;
	mAlphaStep = 1.0f/DEF_TIME;
	nowalpha = 0.0f;
	mFadeIn = true;
}

void CScreenMask::setMask(StringTableEntry texname)
{
	if(texname)
	{
		reset();
		m_TextureHandle = GFXTexHandle(texname, &GFXDefaultStaticDiffuseProfile);
	}
	else
		m_TextureHandle = NULL;
}

CScreenFXMgr::CScreenFXMgr()
{
	m_CurRect = RectI(0,0,1,1);

    mtempBuff = new GFXVertexPT[4];
	mtempBuff[0].texCoord = Point2F(0.0f ,0.0f);
	mtempBuff[1].texCoord = Point2F(0.0f ,1.0f);
	mtempBuff[2].texCoord = Point2F(1.0f ,1.0f);
	mtempBuff[3].texCoord = Point2F(1.0f ,0.0f);

	m_maskAdded = false;
	m_curScreenMaskFX = new CScreenMask;
	m_preScreenMaskFX = new CScreenMask;
}

void CScreenFXMgr::clear()
{
	delete[]  mtempBuff;
	mtempBuff = NULL;

	CScreenFXPtr *cur = NULL;
	for( cur = mFXList.next( cur ); cur; cur = mFXList.next( cur ) )
	{
		IScreenFX *  curFX = *cur;
		CScreenFXPtr * prev = mFXList.prev( cur );
		(*cur)->onRemove();
		mFXList.free( cur );
		cur = prev;
	}

	mFXList.free();
	delete m_curScreenMaskFX;
	delete m_preScreenMaskFX;
}

void CScreenFXMgr::SetRect(const RectI &rect)
{
	if(m_CurRect!=rect)
	{
		mtempBuff[0].point = Point3F(rect.point.x, rect.point.y, 0.0f); 
		mtempBuff[1].point = Point3F(rect.point.x, rect.extent.y, 0.0f);
		mtempBuff[2].point = Point3F(rect.extent.x, rect.extent.y, 0.0f);
		mtempBuff[3].point = Point3F(rect.extent.x, rect.point.y, 0.0f);
		m_CurRect = rect;
	}
}

//老特效淡出，新特效淡入
void CScreenFXMgr::setScreenMask(StringTableEntry texname)
{
	CScreenMask *pTmp = m_preScreenMaskFX;
	m_preScreenMaskFX = m_curScreenMaskFX;
	m_curScreenMaskFX = pTmp;
	m_curScreenMaskFX->setMask(texname);
	m_preScreenMaskFX->setFadeIn(false);

	if(m_curScreenMaskFX->isReady())
	{
		if(!m_curScreenMaskFX->isAdded())
			Add(m_curScreenMaskFX);
	}
	else
	{
		if(m_curScreenMaskFX->isAdded())
			Remove(m_curScreenMaskFX);
	}
}

CScreenFXMgr::~CScreenFXMgr()
{
}

void CScreenFXMgr::AdvanceTime(S32 timeDelta)
{
	CScreenFXPtr *cur = NULL;
	bool bb;

	for( cur = mFXList.next( cur ); cur; cur = mFXList.next( cur ) )
	{
		IScreenFX *  curFX = *cur;
		bb = curFX->AdvanceTime( timeDelta );

		if( !bb )
		{
			CScreenFXPtr * prev = mFXList.prev( cur );
			(*cur)->onRemove();
			mFXList.free( cur );
			cur = prev;
		}
	}
}

void CScreenFXMgr::Add(IScreenFX* newone)
{
	mFXList.link( newone );
	newone->onAdd();
}

void CScreenFXMgr::Remove(IScreenFX *oldone)
{
	CScreenFXPtr *cur = NULL;

	for( cur = mFXList.next( cur ); cur; cur = mFXList.next( cur ) )
	{
		if(*cur == oldone)
		{
			CScreenFXPtr * prev = mFXList.prev( cur );
			(*cur)->onRemove();
			mFXList.free( cur );
			cur = prev;
			return;
		}
	}
}

void CScreenFXMgr::Render()
{
#ifdef STATEBLOCK
	AssertFatal(mSetSB, "CScreenFXMgr::Render -- mSetSB cannot be NULL.");
	mSetSB->apply();
#else
	GFX->setTextureStageColorArg1(0,  GFXTACurrent);
	GFX->setTextureStageColorArg2(0,  GFXTATexture);
	GFX->setTextureStageColorOp(0, GFXTOPModulate);
	GFX->setTextureStageAlphaArg1(0, GFXTADiffuse );
	GFX->setTextureStageAlphaArg2(0, GFXTATexture );
	GFX->setTextureStageAlphaOp(0, GFXTOPModulate);
	GFX->setTextureStageColorOp( 1, GFXTOPDisable );
	GFX->setTextureStageAlphaOp( 1, GFXTOPDisable );

	GFX->setAlphaBlendEnable(true);
	GFX->setSrcBlend( GFXBlendSrcAlpha);
	GFX->setDestBlend(GFXBlendInvSrcAlpha);

	GFX->setAlphaTestEnable(true);
	GFX->setAlphaFunc(GFXCmpGreater);
	GFX->setAlphaRef(0);

	GFX->setCullMode( GFXCullNone );

#endif

	CScreenFXPtr *cur = NULL;
	for( cur = mFXList.next( cur ); cur; cur = mFXList.next( cur ) )
	{
        IScreenFX *temp = *cur;

		GFX->setTexture(0, temp->GetTexture());

		PrimBuild::begin(GFXTriangleFan, 4);

        F32 alpha = temp->Render();
		PrimBuild::color(ColorF(1.0f, 1.0f, 1.0f, alpha));

		for (S32 i=0; i<4; i++)
		{
			PrimBuild::texCoord2f(mtempBuff[i].texCoord.x, mtempBuff[i].texCoord.y);
            PrimBuild::vertex2f(mtempBuff[i].point.x, mtempBuff[i].point.y);			
		}
		PrimBuild::end();
	}
}

void CScreenFXMgr::updateBuff(const RectI &updateRect)
{
	GFX->copyBBToSfxBuff();
}

void CScreenFXMgr::DrawBuff(const RectI &updateRect)
{
	GFX->getDrawUtil()->drawBitmapStretch(GFX->getSfxBackBuffer(),updateRect);
}


void CScreenFXMgr::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 0, GFXTSSColorArg1, GFXTACurrent );
	GFX->setTextureStageState( 0, GFXTSSColorArg2, GFXTATexture );
	GFX->setTextureStageState( 0, GFXTSSAlphaOp, GFXTOPModulate );
	GFX->setTextureStageState( 0, GFXTSSAlphaArg1, GFXTACurrent );
	GFX->setTextureStageState( 0, GFXTSSAlphaArg2, GFXTATexture );
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreater);
	GFX->setRenderState(GFXRSAlphaRef, 0);
	GFX->endStateBlock(mSetSB);
}


void CScreenFXMgr::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}
}

void CScreenFXMgr::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		resetStateBlock();
	}
}

void CScreenFXMgr::shutdown()
{
	SAFE_DELETE(mSetSB);
}


