//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUIROLLMESSAGE_H_
#define _GUIROLLMESSAGE_H_

#ifndef _GFONT_H_
#include "gfx/gFont.h"
#endif
#ifndef _GUITYPES_H_
#include "gui/core/guiTypes.h"
#endif
#pragma once
#include "gui/shiny/guiTickCtrl.h"

//跑马灯屏幕消息需求:
//1.每条消息有固定保留时间
//2.消息文本可以由RichTextControl来支持
class CRichTextDrawer;

class GuiRollMessage : public GuiTickCtrl
{
	typedef GuiTickCtrl Parent;
protected:
	struct MessageInfo
	{
		S32					width;			// 消息文本内容宽度
		U32					limetype;		// 限时/限次类型 0-限时 1-限次
		S32					time;			// 限时秒数/限次次数
		CRichTextDrawer*	pDraw;			// 文本描述对象
		S32					delta;			// 渲染时屏幕坐标步进值
		bool				show;			// 是否渲染
	};

	typedef Vector<MessageInfo*>	MESSAGELIST;
	MESSAGELIST		mMessageList;	//屏显信息列表

protected:
	void interpolateTick( F32 delta );
	void processTick();
	void advanceTime( F32 timeDelta );
public:
	DECLARE_CONOBJECT(GuiRollMessage);
	GuiRollMessage();
	virtual ~GuiRollMessage();
	void onRender(Point2I offset, const RectI &updateRect);
	bool onWake();
	void onSleep();
	void setText(const char *text, U32, S32);
};

#endif //_GUIROLLMESSAGE_H_
