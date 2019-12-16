//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _GUINUMERICEDITCTRL_H_
#define _GUINUMERICEDITCTRL_H_

#ifndef _GUITYPES_H_
#include "gui/core/guiTypes.h"
#endif
#ifndef _SIMBASE_H_
#include "console/simBase.h"
#endif
#ifndef _GUITEXTEDITCTRL_H_
#include "gui/controls/guiTextEditCtrl.h"
#endif
#ifndef _STRINGBUFFER_H_
#include "core/stringBuffer.h"
#endif

#endif

class GuiNumericEditCtrl : public GuiTextEditCtrl
{
private:
	typedef GuiTextEditCtrl Parent;

public:
	enum errType
	{
		NONE_NUM_ERROR = 0,
		RANGE_NUM_ERROR,
		VALUESIZE_NUM_ERROR
	};

	GuiNumericEditCtrl();
	~GuiNumericEditCtrl();

	DECLARE_CONOBJECT(GuiNumericEditCtrl);
	static void initPersistFields();

	void onRender(Point2I offset, const RectI &updateRect);
	bool onKeyDown(const GuiEvent &event);
	S32  parseNumber(const char* str);     
	bool checkRange(F32 number);          // ���ֵ�Ƿ���Ч����
	bool checkValueSize();                // У��ؼ��ߴ��Ƿ�Ϸ�
	bool checkNumberValid();
	S32  mErrorTypeNum;             // ���ֵĴ�������

	
protected:
	GuiNumericEditCtrl* mpBindTarget;  
	bool mbUseValueSize;            // �Ƿ�ʹ�õ�Ԫֵ�ĳߴ�
	S32  mValueSize;                // ��Ԫֵ�ĳߴ�
	S32  mNumberCount;              // ͳ�����ָ���
    S32  mTargetStep;               // ��ǰ�ؼ���ָ��ָ��Ŀؼ�����ֵ���Ӧ��ϵ,����ֵ�N��ӦN-1 �� N��ӦN+1�ȵ�֮��Ĺ�ϵ
	
};