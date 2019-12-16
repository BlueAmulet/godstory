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
	bool checkRange(F32 number);          // 检查值是否有效区间
	bool checkValueSize();                // 校验控件尺寸是否合法
	bool checkNumberValid();
	S32  mErrorTypeNum;             // 数字的错误类型

	
protected:
	GuiNumericEditCtrl* mpBindTarget;  
	bool mbUseValueSize;            // 是否使用单元值的尺寸
	S32  mValueSize;                // 单元值的尺寸
	S32  mNumberCount;              // 统计数字个数
    S32  mTargetStep;               // 当前控件与指针指向的控件的数值项对应关系,如数值项　N对应N-1 或 N对应N+1等等之类的关系
	
};