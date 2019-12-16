//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUISCREENMESSAGE_H_
#define _GUISCREENMESSAGE_H_

#ifndef _GFONT_H_
#include "gfx/gFont.h"
#endif
#ifndef _GUITYPES_H_
#include "gui/core/guiTypes.h"
#endif
#pragma once
#include "gui/shiny/guiTickCtrl.h"

//阶梯式屏幕消息需求:
//1.淡出效果
//2.保留最近三条消息内容
//3.字体描边（20号黑体）

//跑马灯屏幕消息需求:
//1.由右向左滚动显示，长度约300
//2.每条消息有固定保留时间

class CommonFontEX;

class GuiScreenMessage : public GuiTickCtrl
{
	typedef GuiTickCtrl Parent;
public:
	enum Constants
	{
		COMMON_ONETIMES				= 4000,
	};
protected:
	struct MessageInfo
	{
		MessageInfo() { text = NULL; width = 0; overtime = 0; color.set(255,0,0); }
		~MessageInfo() { if(text) dFree(text); }
		char*			text;						// 屏显字符串
		U32				width;						// 屏显字符串宽度
		U32				overtime;					// 超时时间
		ColorI			color;						// 屏显颜色
	};

	typedef Vector<MessageInfo*>	MESSAGELIST;
	MESSAGELIST		mMessageList;		// 屏显信息列表
	CommonFontEX*	mFont;				// 屏显字体对象

protected:
	void interpolateTick( F32 delta );
	void processTick();
	void advanceTime( F32 timeDelta );
public:
	DECLARE_CONOBJECT(GuiScreenMessage);
	GuiScreenMessage();
	virtual ~GuiScreenMessage();

	void onRender(Point2I offset, const RectI &updateRect);
	bool onWake();
	void onSleep();
	void setText(const char *text, ColorI& color);
};

#endif //_GUISCREENMESSAGE_H_
