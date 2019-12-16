//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "console/consoleTypes.h"
#include "console/console.h"
#include "core/color.h"
#include "ui/guiRollMessage.h"
#include <windows.h>
#include "Gameplay/GameplayCommon.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "util/RichTextDrawer.h"

IMPLEMENT_CONOBJECT(GuiRollMessage);

GuiRollMessage::GuiRollMessage()
{
}

GuiRollMessage::~GuiRollMessage()
{
	for(S32 i = 0; i < mMessageList.size(); ++i)
	{
		if(mMessageList[i])
		{
			delete mMessageList[i]->pDraw;
			delete mMessageList[i];
		}
	}
	mMessageList.clear();
}

void GuiRollMessage::processTick() 
{
	if(mMessageList.empty())
	{
		GuiControl* parentCtrl = getParent();
		if(parentCtrl && parentCtrl->isVisible())
			parentCtrl->setVisible(false);
		return;
	}

	U32 curtime = GetTickCount();
	bool nextCanShow = false;
	for(S32 i = 0; i < mMessageList.size(); ++i)
	{
		MessageInfo* pMsg = mMessageList[i];
		if((pMsg->limetype == 0 && pMsg->time <= curtime) ||
			(pMsg->limetype == 1 && pMsg->time <= 0))
		{
			delete pMsg->pDraw;
			delete pMsg;
			mMessageList.erase_fast(i--);
			continue;
		}

		if(pMsg->show)
			pMsg->delta++;
		if(pMsg->delta > pMsg->width + 100)
		{
			S32 j = (i+1) >= mMessageList.size()? 0 : i + 1;
			if(j != i && !mMessageList[j]->show)
			{
				mMessageList[j]->show = true;
				mMessageList[j]->delta = 0;
			}
		}

		if(pMsg->delta > pMsg->width + getWidth())
		{
			pMsg->show = false;			
			pMsg->delta = 0;
			if(mMessageList.size() == 1)
				pMsg->show = true;
		}
	}
}

//------------------------------------------------------------------------------
void GuiRollMessage::onRender(Point2I offset, const RectI &updateRect)
{
	if(mMessageList.empty())
		return;

	Point2I pos;
	for(S32 i = 0; i < mMessageList.size(); ++i)
	{
		MessageInfo* pMsg = mMessageList[i];
		if(!pMsg || !pMsg->show)
			continue;
		pos.x = offset.x + getWidth() - pMsg->delta;
		pos.y = offset.y;
		pMsg->pDraw->render(pos, updateRect);
	}
	renderChildControls(offset, updateRect);
}

void GuiRollMessage::interpolateTick( F32 delta ) 
{
}

void GuiRollMessage::advanceTime(F32 timeDelta ) 
{
}

bool GuiRollMessage::onWake()
{
	if ( !Parent::onWake() )
		return false;
	return true;
}

void GuiRollMessage::onSleep()
{
	Parent::onSleep();
}

// ----------------------------------------------------------------------------
// 设置屏显消息文本
void GuiRollMessage::setText(const char *text, U32 limetype, S32 time)
{
	if(!text || *text == 0)
		return;
	MessageInfo* pMsg = new MessageInfo;
	pMsg->pDraw = new CRichTextDrawer;
	pMsg->limetype = limetype;
	pMsg->time = limetype == 0 ? GetTickCount() + time * 1000 : time;

	CRichTextDrawer::Style style;
	strcpy( style.fontName, mProfile->mFontType );
	style.fontSize = mProfile->mFontSize;
	style.fontColor = mProfile->mFontColor;
	style.fontIsBitmap = mProfile->mBitmapFont;
	style.outline = mProfile->mFontOutline;

	pMsg->pDraw->setDefaultStyle( style );
	pMsg->pDraw->setParentControl( this );
	pMsg->pDraw->setControlOffset( getGlobalBounds().point );
	pMsg->pDraw->setWidth( 2048);
	pMsg->pDraw->setContent( text );
	pMsg->width = pMsg->pDraw->getRealLineWidth();
	pMsg->show	= mMessageList.empty();
	pMsg->delta = 0;
	mMessageList.push_back(pMsg);

	GuiControl* parentCtrl = getParent();
	if(parentCtrl)
		parentCtrl->setVisible(true);
}

// ----------------------------------------------------------------------------
// 设置屏显消息文本脚本方法
ConsoleMethod( GuiRollMessage, setText, void, 5, 5, "obj.setText(%text, %limittype,%time)" )
{
	object->setText(argv[2], dAtol(argv[3]), dAtol(argv[4]));
}