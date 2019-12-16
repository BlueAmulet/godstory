//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "console/consoleTypes.h"
#include "console/console.h"
#include "core/color.h"
#include "ui/guiScreenMessage.h"
#include <windows.h>
#include "Gameplay/GameplayCommon.h"
#include "gfx/gfxFontRenderBatcher.h"

// -----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiScreenMessage);

GuiScreenMessage::GuiScreenMessage()
{
	mFont = CommonFontManager::GetFont("黑体", 20);
}

GuiScreenMessage::~GuiScreenMessage()
{
	for(S32 i = 0; i < mMessageList.size(); ++i)
	{
		if(mMessageList[i]) delete mMessageList[i];
	}
	mMessageList.clear();
	if(mFont) mFont->ReduceRef();
}

void GuiScreenMessage::processTick() 
{
	U32 size = (U32)mMessageList.size();
	if(size == 0)
	{
		setVisible(false);
		return;
	}
	
	U32 curtime = GetTickCount();
	for(S32 i = 0; i < mMessageList.size(); ++i)
	{
		MessageInfo* pMsg = mMessageList[i];
		S32 dis = pMsg->overtime - curtime;
		if(pMsg && dis <= 0)
		{
			delete pMsg;
			mMessageList.erase_fast(i--);
		}
		else if(dis <= 2000)
		{
			pMsg->color.alpha = dis * 250 / 2000;
		}
	}
}

//------------------------------------------------------------------------------
void GuiScreenMessage::onRender(Point2I offset, const RectI &updateRect)
{
	if(mMessageList.empty() || !mFont)
		return;

	Point2I pos(offset);
	Point2I extents = getExtent();
	pos.y += extents.y;

	mFont->BeginRender();
	for(S32 i = mMessageList.size() - 1; i >= 0; --i)
	{
		MessageInfo* pMsg = mMessageList[i];
		if(pMsg)
		{
			pos.x  = offset.x + (extents.x - pMsg->width) / 2;
			pos.y -= mFont->getHeight();
			mFont->SetText( Point2I( pos.x, pos.y ), pMsg->color, pMsg->text, true, ColorI(0,0,0, pMsg->color.alpha));
			pos.y -= 10;
		}
	}
	mFont->Render();
	renderChildControls(offset, updateRect);
}

void GuiScreenMessage::interpolateTick( F32 delta ) 
{
}

void GuiScreenMessage::advanceTime(F32 timeDelta ) 
{
}

bool GuiScreenMessage::onWake()
{
	if ( !Parent::onWake() )
		return false;
	return true;
}

void GuiScreenMessage::onSleep()
{
	Parent::onSleep();
}

// ----------------------------------------------------------------------------
// 设置屏显消息文本
void GuiScreenMessage::setText(const char *text, ColorI& color)
{
	if(!text || *text == 0 || !mFont)
		return;
	MessageInfo* pMsg = new MessageInfo;
	pMsg->text = dStrdup(text);
	pMsg->color = color;
	pMsg->width = mFont->getStrWidth(pMsg->text);
	pMsg->overtime = GetTickCount() + COMMON_ONETIMES;
	mMessageList.push_back(pMsg);
	if(mMessageList.size() > 3)
	{
		SAFE_DELETE(mMessageList[0]);
		mMessageList.erase_fast((U32)0);
	}
	setVisible(true);
}

// ----------------------------------------------------------------------------
// 设置屏显消息文本脚本方法
ConsoleMethod( GuiScreenMessage, setText, void, 3, 4, "obj.setText(%text, %color)" )
{
	ColorI fontColor(255,0, 0, 255);
	if(argc > 3)
	{
		U32 RGBA = dAtol(argv[3]);
		U8 r = (RGBA >> 16) & 0xFF;
		U8 g = (RGBA >> 8) & 0xFF;
		U8 b = RGBA & 0xFF;
		fontColor.set(r,g,b);
	}
	object->setText(argv[2], fontColor);
}