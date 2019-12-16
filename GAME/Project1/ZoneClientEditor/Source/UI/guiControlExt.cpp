
#include "UI/guiControlExt.h"
#include "console/consoleTypes.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gui/core/guiCanvas.h"
#include "gfx/gfxCardProfile.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK
GFXStateBlock* GuiControlExt::mSetSB = NULL;
GFXTexHandle	GuiControlExt::m_DepthTex = NULL;

//==================================================================================================================================
//   GuiControlExt
//==================================================================================================================================

IMPLEMENT_CONOBJECT(GuiControlExt);

GuiControlExt::GuiControlExt()
{
	m_BuffTexture = NULL;
	mCanUseSeparateAlphaBlend = GFX->getCardProfiler()->queryProfile( "SeparateAlphaBlend", false );
}

void GuiControlExt::clear()
{
	m_DepthTex = NULL;
}

//bool GuiControlExt::onAdd()
//{
	//static GuiCanvas *pCanvas = NULL;
	//
	//if(!pCanvas)
	//	pCanvas = dynamic_cast<GuiCanvas*>(Sim::findObject("Canvas"));

	//if(pCanvas)
	//{
	//	PlatformWindow *pWin = pCanvas->getPlatformWindow();
	//	if(pWin)
	//		pWin->syncDisplayEvent.notify(this, &GuiControlExt::handleSyncEvent);

	//}

//	return Parent::onAdd();
//}

//void GuiControlExt::onRemove()
//{
	//static GuiCanvas *pCanvas = NULL;

	//if(!pCanvas)
	//	pCanvas = dynamic_cast<GuiCanvas*>(Sim::findObject("Canvas"));

	//if(pCanvas)
	//{
	//	PlatformWindow *pWin = pCanvas->getPlatformWindow();
	//	if(pWin)
	//		pWin->syncDisplayEvent.remove(this, &GuiControlExt::handleSyncEvent);

	//}

//	return Parent::onRemove();
//}

//void GuiControlExt::handleSyncEvent(DeviceId did)
//{
//	m_DirtyFlag = true;
//}

void GuiControlExt::recreateBuff()
{
	Point2I extent = getExtent();
	if(m_BuffTexture.isNull() || extent.x!=m_BuffTexture->getBitmapWidth() || extent.y!=m_BuffTexture->getBitmapHeight() || m_BuffTexture.getPointer()->setDirty())
	{
		m_BuffTexture.set(extent.x, extent.y, GFXFormatR8G8B8A8, &GFXDefaultRenderTargetProfile, 0);
		m_DirtyFlag = true;

		const Point2I &winRect = getRoot()->getBounds().extent;
		if(extent.x>winRect.x || extent.y>winRect.y)
		{
			Point2I bufRect(0,0);

			if(!m_DepthTex.isNull())
			{
				bufRect.x = m_DepthTex.getWidth();
				bufRect.y = m_DepthTex.getHeight();
			}

			if(extent.x>bufRect.x || extent.y>bufRect.y)
			{
				m_DepthTex.set( extent.x>bufRect.x? extent.x :bufRect.x, extent.y>bufRect.y?extent.y:bufRect.y, GFXFormatD24S8, &GFXDefaultZTargetNoMSAAProfile );
			}
		}
	}
}

void GuiControlExt::onPreRender()
{
	if(mCanUseSeparateAlphaBlend && !m_DirtyFlag)
		m_DirtyFlag = isDirty();
}

void GuiControlExt::onRenderExt(Point2I offset, const RectI &updateRect)
{
	RectI ctrlRect(offset, getExtent());

	//if opaque, fill the update rect with the fill color
	if (getControlProfile()->mOpaque)
		GFX->getDrawUtil()->drawRectFill(ctrlRect, getControlProfile()->mFillColor);

	//if there's a border, draw the border
	if (getControlProfile()->mBorder)
		renderBorder(ctrlRect, getControlProfile());

	// Render Children
	renderChildControls(offset, updateRect);
}

void GuiControlExt::onRender(Point2I offset, const RectI &updateRect)
{
	if(!mCanUseSeparateAlphaBlend)
		onRenderExt(offset,updateRect);
	else
	{
		recreateBuff();

		if(m_DirtyFlag || GuiCanvas::mGlobalDirty)
		{
			//建立缓存
			m_BuffBinding =true;
			GFX->getDrawUtil()->setBlendTyps(GFXDrawUtil::SeparateAlphaBlend);

			GFX->pushActiveRenderTarget();
			GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
			myTarg->attachTexture(GFXTextureTarget::Color0, m_BuffTexture );
			if(m_DepthTex.isNull())
				myTarg->attachTexture(GFXTextureTarget::DepthStencil, GFXTextureTarget::sDefaultDepthStencil);
			else
				myTarg->attachTexture(GFXTextureTarget::DepthStencil, m_DepthTex);
			GFX->setActiveRenderTarget( myTarg );
			GFX->clear( GFXClearZBuffer | GFXClearTarget , ColorI( 0, 0, 0, 0), 1.0f, 0 );

			RectI localUpdateRect = updateRect;
			localUpdateRect.point -= offset;
			GFX->setClipPos(offset);
			GFX->setClipRect( localUpdateRect );

			onRenderExt(Point2I(0,0),localUpdateRect);

			GFX->popActiveRenderTarget();
			GFX->setClipRect( updateRect );
			GFX->setClipPos(Point2I(0,0));
			GFX->getDrawUtil()->setBlendTyps(GFXDrawUtil::LerpAlphaBlend);

			m_DirtyFlag = false;
			m_BuffBinding =false;
		}

		RectI rect(offset, getExtent());
		drawBuff(m_BuffTexture, rect);
	}
}

void GuiControlExt::drawBuff(GFXTextureObject *texture, const RectI &dstRect)
{
	// Sanity if no texture is specified.
	if(!texture)
		return;

	RectF srcRect( 0, 0, texture->mBitmapSize.x, texture->mBitmapSize.y );

	GFX->setBaseRenderState();

	GFXVertexBufferHandle<GFXVertexPCT> verts(GFX, 4, GFXBufferTypeVolatile );
	verts.lock();

	F32 texLeft   = F32(srcRect.point.x)                    / F32(texture->mTextureSize.x);
	F32 texRight  = F32(srcRect.point.x + srcRect.extent.x) / F32(texture->mTextureSize.x);
	F32 texTop    = F32(srcRect.point.y)                    / F32(texture->mTextureSize.y);
	F32 texBottom = F32(srcRect.point.y + srcRect.extent.y) / F32(texture->mTextureSize.y);

	F32 screenLeft   = (F32)dstRect.point.x;
	F32 screenRight  = (F32)(dstRect.point.x + dstRect.extent.x);
	F32 screenTop    = (F32)dstRect.point.y;
	F32 screenBottom = (F32)(dstRect.point.y + dstRect.extent.y);

	const F32 fillConv = GFX->getFillConventionOffset();
	verts[0].point.set( screenLeft  - fillConv, screenTop    - fillConv, 0.f );
	verts[1].point.set( screenRight - fillConv, screenTop    - fillConv, 0.f );
	verts[2].point.set( screenLeft  - fillConv, screenBottom - fillConv, 0.f );
	verts[3].point.set( screenRight - fillConv, screenBottom - fillConv, 0.f );

	verts[0].color = verts[1].color = verts[2].color = verts[3].color = ColorI(255,255,255);

	verts[0].texCoord.set( texLeft,  texTop );
	verts[1].texCoord.set( texRight, texTop );
	verts[2].texCoord.set( texLeft,  texBottom );
	verts[3].texCoord.set( texRight, texBottom );

	verts.unlock();

	GFX->setVertexBuffer( verts );
#ifdef STATEBLOCK
	AssertFatal(mSetSB, "GuiControlExt::drawBuff -- mSetSB cannot be NULL.");
	mSetSB->apply();
#else
	GFX->setCullMode( GFXCullNone );
	GFX->setLightingEnable( false );
	GFX->setAlphaBlendEnable( true );
	GFX->setSrcBlend( GFXBlendSrcAlpha );
	GFX->setDestBlend( GFXBlendInvSrcAlpha );
	GFX->setTextureStageColorOp( 0, GFXTOPModulate );
	GFX->setTextureStageColorOp( 1, GFXTOPDisable );
#endif

	GFX->setTexture( 0, texture );

	GFX->drawPrimitive( GFXTriangleStrip, 0, 2 );

	//GFX->setAlphaBlendEnable( false );
}


void GuiControlExt::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPDisable );
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mSetSB);
}


void GuiControlExt::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}
}

void GuiControlExt::init()
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

void GuiControlExt::shutdown()
{
	SAFE_DELETE(mSetSB);
}

ConsoleMethod(GuiControlExt,setDirty,void,2,2,"")
{
	object->SetDirty(true);
}


