//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "console/consoleTypes.h"
#include "console/console.h"
#include "Util/StrLexer.h"
#include "GuiNumericEditCtrl.h"
#include "gui/core/guiDefaultControlRender.h"
#include <string>

IMPLEMENT_CONOBJECT(GuiNumericEditCtrl);

GuiNumericEditCtrl::GuiNumericEditCtrl()
{
	mbUseValueSize = false;
	mValueSize     = 0; 
	mpBindTarget   = NULL;
	mNumberCount   = 0;
	mErrorTypeNum  = 0;
	mTargetStep    = 0;

}

GuiNumericEditCtrl::~GuiNumericEditCtrl()
{

}

void GuiNumericEditCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addField("pBindTarget",         TypeSimObjectPtr,    Offset(mpBindTarget    ,GuiNumericEditCtrl));
	addField("TargetStep",          TypeS32,             Offset(mTargetStep,     GuiNumericEditCtrl));
	addField("bUseValueSize",       TypeBool,            Offset(mbUseValueSize,  GuiNumericEditCtrl));
	addField("ValueSize",           TypeS32,             Offset(mValueSize,      GuiNumericEditCtrl));
	
}

void GuiNumericEditCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	RectI ctrlRect( offset, getExtent() );
	
	if(mErrorTypeNum == NONE_NUM_ERROR)
	{
		Parent::onRender(offset,updateRect);
		return;
	}
	else if (mErrorTypeNum == RANGE_NUM_ERROR)
	{
		GFX->getDrawUtil()->drawRectFill( ctrlRect,ColorI(179,204,187,255) );
	}
	else if (mErrorTypeNum == VALUESIZE_NUM_ERROR)
	{
		GFX->getDrawUtil()->drawRectFill( ctrlRect, ColorI(213,170,179,255) );
	}

	drawText( ctrlRect, isFirstResponder() );
}

bool GuiNumericEditCtrl::onKeyDown(const GuiEvent &event)
{
	if(! isActive())
		return false;

	Parent::onKeyDown(event);
	mErrorTypeNum = parseNumber(mTextBuffer.getPtr8());

	if(mErrorTypeNum == NONE_NUM_ERROR && !checkValueSize())
		mErrorTypeNum = VALUESIZE_NUM_ERROR;

	setUpdate();
	return true;
}

S32 GuiNumericEditCtrl::parseNumber(const char* str)
{
	stStrLexerToken lt;
	CStrLexer lexer(str);
	stStrLexerToken templt;
	std::string tempStr;
	mNumberCount = 0;
	
	while (true)
	{
		int nIndex = 0;
		tempStr.clear();
		while( lexer.GetNext( lt ) )
		{
			if( lt.nTokenType == STR_LEXER_TOKEN_TYPE_SYMBOL )
			{
				if( (nIndex == 0) && *lt.szBuffer != '-')
					break;

				if( nIndex > 0 && *lt.szBuffer != '.' )
					break;

				tempStr += (char*)lt;
				nIndex++;

				continue;
			}

			if( lt.nTokenType == STR_LEXER_TOKEN_TYPE_NUM )
			{
				tempStr += (char*)lt;
				if(lexer.PreGetNext(templt) == STR_LEXER_TOKEN_TYPE_SYMBOL)
				{
					nIndex++;
					continue;
				}
			}	
			
			else if(lt.nTokenType == STR_LEXER_TOKEN_TYPE_BLANK) //忽略空格
				break;
			else
				return RANGE_NUM_ERROR;

			nIndex++;
			mNumberCount++;
		}

		if(nIndex != 0)
		{
			//过滤非法数字
			if(dStricmp(tempStr.c_str(),"0.") == 0 || dStricmp(tempStr.c_str(),"-") == 0 || dStrnicmp(tempStr.c_str(),"00",2) == 0
				|| dStrnicmp(tempStr.c_str(),"..",2) == 0 || dStrnicmp(tempStr.c_str(),"--",2) == 0)
				return RANGE_NUM_ERROR;
			//不是有效的数字
			if(!checkRange(atof(tempStr.c_str())))
				return RANGE_NUM_ERROR;  
		}
		else
			break;
	}

	return NONE_NUM_ERROR;
}

bool GuiNumericEditCtrl::checkRange(F32 number)
{
	if(number >= mMinNum && number <= mMaxNum)
		return true;
	else
		return false;
}

bool GuiNumericEditCtrl::checkValueSize()
{
	if(mValueSize <= 0)
		return true;

	if(NULL == mpBindTarget)
	{
		if(mValueSize == mNumberCount)
			return true;
		else
			return false;
	}

	char strBuff[MAX_STRING_LENGTH] = {0,};
	mpBindTarget->getText(strBuff );
	S32 Value = atoi(strBuff);

	if(mNumberCount == (Value+mTargetStep) * mValueSize)
		return true;

	return false;
}


ConsoleMethod(GuiNumericEditCtrl,checkNumberValid,bool,2,2,"")
{
	char strBuff[1024] = {0,};
	object->getText(strBuff );

	object->mErrorTypeNum = object->parseNumber(strBuff);
	bool ret = object->checkValueSize();

	if(object->mErrorTypeNum == GuiNumericEditCtrl::NONE_NUM_ERROR && !ret)
		object->mErrorTypeNum = GuiNumericEditCtrl::VALUESIZE_NUM_ERROR;

	return ret;
}