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

//�������Ļ��Ϣ����:
//1.ÿ����Ϣ�й̶�����ʱ��
//2.��Ϣ�ı�������RichTextControl��֧��
class CRichTextDrawer;

class GuiRollMessage : public GuiTickCtrl
{
	typedef GuiTickCtrl Parent;
protected:
	struct MessageInfo
	{
		S32					width;			// ��Ϣ�ı����ݿ��
		U32					limetype;		// ��ʱ/�޴����� 0-��ʱ 1-�޴�
		S32					time;			// ��ʱ����/�޴δ���
		CRichTextDrawer*	pDraw;			// �ı���������
		S32					delta;			// ��Ⱦʱ��Ļ���경��ֵ
		bool				show;			// �Ƿ���Ⱦ
	};

	typedef Vector<MessageInfo*>	MESSAGELIST;
	MESSAGELIST		mMessageList;	//������Ϣ�б�

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
