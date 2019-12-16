//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include <d3dx9math.h>
#include "console/consoleTypes.h"
#include "gfx/primBuilder.h"
#include "gfx/D3D9/gfxD3D9Device.h"
#include "Gameplay/item/Res.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "UI/guiRichTextCtrl.h"
#include "UI/dGuiObjectIcon.h"

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameObjects/LivingSkillShortcut.h"
#endif
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK
GFXStateBlock* dGuiObjectIcon::mSetCoolSB = NULL;
GFXStateBlock* dGuiObjectIcon::mClearCoolSB = NULL;

#define ICONFILENAME_ITEM	"gameres/data/icon/item/"
#define ICONFILENAME_SKILL	"gameres/data/icon/skill/"
#define ICONNUM				"gameres/gui/images/Uinum.png"
#define UNKNOWICON			"gameres/gui/images/icon_unknown.png"

IMPLEMENT_CONOBJECT(dGuiObjectIcon);

dGuiObjectIcon::dGuiObjectIcon()
{
	mBoundary.set(0,0,32,32);
	//mNumTexture	= NULL;
	//mNum[0]		= 0;
	//mNum[1]		= 0;
	//mNum[2]		= 0;
	mTexture		= NULL;
	mObjectType		= OBJECTICON_ITEM;
	mObjectID		= 0;
	mTextCtrl		= NULL;
}

dGuiObjectIcon::~dGuiObjectIcon()
{
	mTexture			= NULL;
	//mNumTexture		= NULL; 
}

void dGuiObjectIcon::initPersistFields()
{
	Parent::initPersistFields();
	addField("objecttype",			TypeS32,	Offset(mObjectType,			dGuiObjectIcon));
	addField("objectid",			TypeS32,	Offset(mObjectID,			dGuiObjectIcon));
	addField("boudary",				TypeRectI,  Offset(mBoundary,			dGuiObjectIcon));
}

bool dGuiObjectIcon::resize(const Point2I &newPosition, const Point2I &newExtent)
{
	Point2I oldsize = mBoundary.extent;
	Parent::resize(newPosition, oldsize);
	//Point2I newsize = mBoundary.extent;
	//resizeBoundary(mBoundary, oldsize, newsize);
	return true;
}

bool dGuiObjectIcon::onWake()
{
	if(!Parent::onWake())
		return false;

	//if(!mNumTexture)
	//	mNumTexture = GFXTexHandle(ICONNUM, &GFXDefaultGUIProfile);
#ifdef NTJ_CLIENT
	updateObject();
#endif

	if(!mTextCtrl)
		mTextCtrl = dynamic_cast<GuiRichTextCtrl*>(Sim::findObject("ShowItemInfo"));
	return true;
}

void dGuiObjectIcon::onSleep()
{
	//mNumTexture	= NULL;
	mTextCtrl		= NULL;
	mTexture		= NULL;
	Parent::onSleep();
}

void dGuiObjectIcon::resizeBoundary(const RectI& bound, const Point2I& oldsize, const Point2I& newsize)
{
	if(oldsize.x == 0 || oldsize.y == 0)
		return;

	RectI old_boundary = bound;
	mBoundary.point.x	= S32(F32(old_boundary.point.x)	/ F32(oldsize.x)	* F32(newsize.x) + 0.55f);
	mBoundary.point.y	= S32(F32(old_boundary.point.y)	/ F32(oldsize.y)	* F32(newsize.y) + 0.55f);
	mBoundary.extent.x	= S32(F32(old_boundary.extent.x)/ F32(oldsize.x)	* F32(newsize.x) + 0.55f);
	mBoundary.extent.y	= S32(F32(old_boundary.extent.y)/ F32(oldsize.y)	* F32(newsize.y) + 0.55f);
}


//=============================================================================
// 渲染处理
//=============================================================================
void dGuiObjectIcon::onPreRender()
{
	Parent::onPreRender();
}

void dGuiObjectIcon::onRender(Point2I offset, const RectI &updateRect)
{
	GFX->getDrawUtil()->clearBitmapModulation();

#ifdef NTJ_CLIENT
	// 画ICON
	if(mTexture)
	{
		GFX->getDrawUtil()->drawBitmapStretch(mTexture, RectI(offset + mBoundary.point, mBoundary.extent));
		// 画ICON上数字
		//drawIconNum(offset, updateRect);
	}
#endif
#ifdef NTJ_EDITOR
	GFX->getDrawUtil()->drawRectFill(RectI(offset + mBoundary.point, mBoundary.extent), ColorI(255, 255, 255, 255));
#endif
}

#ifdef NTJ_CLIENT

//// ----------------------------------------------------------------------------
//// 渲染ICON上数字
//void dGuiObjectIcon::drawIconNum(Point2I offset, const RectI &updateRect)
//{
//	if(!mIsShowNum || !mTexture || !mNumTexture)
//		return;
//
//	S32 offsetX		= offset.x + mBoundary.point.x + mBoundary.extent.x - 2;
//	S32 offsetY		= offset.y + mBoundary.point.y + mBoundary.extent.y - 3;
//	S32 bmpWidth	= mNumTexture.getWidth() / 10;
//	S32 bmpHeight	= mNumTexture.getHeight();
//
//	U32 count = 1;
//	for(S32 n = 2; n >= 0; n--)
//	{
//		if(mNum[n] >= 0)
//		{
//			RectI renderPos((offsetX - bmpWidth * count), (offsetY - bmpHeight), bmpWidth, bmpHeight);
//			RectI bmpOffset(bmpWidth * mNum[n], 0, bmpWidth, bmpHeight);
//			GFX->getDrawUtil()->drawBitmapStretchSR(mNumTexture, renderPos, bmpOffset);
//			count ++;
//		}
//	}
//}

void dGuiObjectIcon::updateObject()
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;

	if(mObjectType == OBJECTICON_ITEM)
	{
		ItemBaseData* pData = g_ItemRepository->getItemData(mObjectID);
		if(!pData)
			return;

		char cTempPath[128] = "\0";
		dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_ITEM, pData->getIconName());
		mTexture.set(cTempPath, &GFXDefaultGUIProfile);
		if(!mTexture)
			mTexture.set(UNKNOWICON, &GFXDefaultGUIProfile);
	}
	else if(mObjectType == OBJECTICON_SKILL)
	{
		SkillData* pData = g_SkillRepository.GetSkill(mObjectID);
		if(!pData)
			return;
		char cTempPath[128] = "\0";
		dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_SKILL, pData->GetIcon());
		mTexture.set(cTempPath, &GFXDefaultGUIProfile);
		if(!mTexture)
			mTexture.set(UNKNOWICON, &GFXDefaultGUIProfile);
	}
	else if(mObjectType == OBJECTICON_STATUS)
	{
		
	}
	else if(mObjectType == OBJECTICON_LIVINGSKILL)
	{
		LivingSkillData* pData = g_LivingSkillRespository.getLivingSkillData(mObjectID);
		char cTempPath[128] = "\0";
		dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_SKILL, pData->getIconName());
		mTexture.set(cTempPath, &GFXDefaultGUIProfile);
		if(!mTexture)
			mTexture.set(UNKNOWICON, &GFXDefaultGUIProfile);
	}
}

void dGuiObjectIcon::onMouseEnter(const GuiEvent &event)
{
	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if(!pPlayer)
		return;	

	char cDesc[4096];
	dMemset(cDesc, 0, 4096);
	char cTempPath[128] = "\0";

	if(mObjectType == OBJECTICON_ITEM)
	{
		Res* pRes = new Res;
		if(pRes->setBaseData(mObjectID))
		{
			g_ItemManager->insertText(pPlayer, pRes, cDesc,0, 0);
			dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_ITEM, pRes->getIconName());
		}
		delete pRes;
	}
	else if(mObjectType == OBJECTICON_SKILL)
	{
		SkillData* pData = g_SkillRepository.GetSkill(mObjectID);
		if (pData)
		{
			g_ItemManager->insertText(pPlayer, pData, cDesc, 0, 0);
			dSprintf(cTempPath, sizeof(cTempPath), "%s%s", ICONFILENAME_SKILL, pData->GetIcon());
		}
	}
	else if(mObjectType == OBJECTICON_STATUS)
	{
		
	}

	if(mTextCtrl && cDesc[0])
	{
		mTextCtrl->getDrawer()->setDefaultLineHeight(16);
		mTextCtrl->setContent(cDesc);
		S32 iHight = mTextCtrl->getHeight() + 10;
		Point2I	mMousePos = event.mousePoint;

		Point2I newPos;
		if(mMousePos.x - 32 - mTextCtrl->getWidth() < 0)
			newPos.x = mMousePos.x + 32 ;
		else
			newPos.x = mMousePos.x - 32 - mTextCtrl->getWidth();

		GuiCanvas* pCanvas = getRoot();
		S32 iWindowHight = 600;
		if(pCanvas)
			iWindowHight = pCanvas->getHeight();
		if((mMousePos.y + mTextCtrl->getHeight()/2) > iWindowHight)
			newPos.y = iWindowHight - mTextCtrl->getHeight() - 15;
		else
			newPos.y = mMousePos.y - mTextCtrl->getHeight()/2;

		Con::executef("ShowItemHot", Con::getIntArg(newPos.x), Con::getIntArg(newPos.y), Con::getIntArg(iHight), cTempPath);
	}	
}

void dGuiObjectIcon::onMouseLeave(const GuiEvent &event)
{
	m_DirtyFlag = true;

	//清除ShortCut的热感提示
	Con::executef("HideItemHot");
	if(mTextCtrl)
	{
		mTextCtrl->setContent("");
		mTextCtrl->refresh();
	}
}

ConsoleMethod( dGuiObjectIcon, setObject, void, 4, 4, "obj.setObject(%type, %id)" )
{
	object->setObject( dAtol(argv[2]), dAtol(argv[3]));
	object->updateObject();
}

#endif

void dGuiObjectIcon::resetStateBlock()
{
	//mSetCoolSB
	GFX->beginStateBlock();
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPDisable );
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mSetCoolSB);

	//mClearCoolSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSLighting, true);
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPModulate);
	GFX->endStateBlock(mClearCoolSB);
}

void dGuiObjectIcon::releaseStateBlock()
{
	if (mSetCoolSB)
		mSetCoolSB->release();

	if (mClearCoolSB)
		mClearCoolSB->release();
}

void dGuiObjectIcon::init()
{
	if (mSetCoolSB == NULL)
	{
		mSetCoolSB = new GFXD3D9StateBlock;
		mSetCoolSB->registerResourceWithDevice(GFX);
		mSetCoolSB->mZombify = &releaseStateBlock;
		mSetCoolSB->mResurrect = &resetStateBlock;

		mClearCoolSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void dGuiObjectIcon::shutdown()
{
	SAFE_DELETE(mSetCoolSB);
	SAFE_DELETE(mClearCoolSB);
}