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

//����ʽ��Ļ��Ϣ����:
//1.����Ч��
//2.�������������Ϣ����
//3.������ߣ�20�ź��壩

//�������Ļ��Ϣ����:
//1.�������������ʾ������Լ300
//2.ÿ����Ϣ�й̶�����ʱ��

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
		char*			text;						// �����ַ���
		U32				width;						// �����ַ������
		U32				overtime;					// ��ʱʱ��
		ColorI			color;						// ������ɫ
	};

	typedef Vector<MessageInfo*>	MESSAGELIST;
	MESSAGELIST		mMessageList;		// ������Ϣ�б�
	CommonFontEX*	mFont;				// �����������

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
