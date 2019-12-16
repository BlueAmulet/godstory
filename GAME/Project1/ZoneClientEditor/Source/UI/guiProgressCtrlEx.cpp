//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "console/consoleTypes.h"
#include "gui/game/guiProgressCtrl.h"
#include "ui/guiProgressCtrlEx.h"

IMPLEMENT_CONOBJECT(GuiProgressCtrlEx);

GuiProgressCtrlEx::GuiProgressCtrlEx() : mProgressStyle(STYLE_INCREMENT),
                                         mUpdateInterval(10),
										 mLastUpdateTime(0),
										 mCurrentTime(0),
                                         mIntervalTime(1000),
										 mLapseTime(0),
										 mFadeOutTime(0)
{

}

void GuiProgressCtrlEx::initPersistFields()
{
	Parent::initPersistFields();

	addGroup("GuiProgressCtrlEx");
	addField("Style",              TypeS8,         Offset(mProgressStyle,GuiProgressCtrlEx));
	addField("UpdateInterval",     TypeS32,        Offset(mUpdateInterval,GuiProgressCtrlEx));
	addField("IntervalTime",       TypeS32,        Offset(mFadeOutTime,GuiProgressCtrlEx));
	addField("FadeOutTime",        TypeS32,        Offset(mFadeOutTime,GuiProgressCtrlEx));
	endGroup("GuiProgressCtrlEx");
}

void GuiProgressCtrlEx::onPreRender()
{
	//m_DirtyFlag = UpdateDate();
	UpdateDate();
}

bool GuiProgressCtrlEx::UpdateDate()
{
	mCurrentTime = Platform::getRealMilliseconds();
	if((mCurrentTime - mLastUpdateTime) < mUpdateInterval)
		return false;
		
	if(mProgressStyle != STYLE_BREAK && mIntervalTime >0)
	{
		mLapseTime += mCurrentTime - mLastUpdateTime;
		if(mLapseTime >= mIntervalTime)
		{
			mLastUpdateTime = mCurrentTime;
			return false;
		}

		F32 tempValue = (F32)mLapseTime / (F32)mIntervalTime;
		if(mProgressStyle == STYLE_DEGRESSION)
			tempValue = 1.0f - tempValue;

		setProgressValue(tempValue);
	}

	mLastUpdateTime = mCurrentTime;
	return true;
}

void GuiProgressCtrlEx::onRender(Point2I offset, const RectI &updateRect)
{
	//m_DirtyFlag = false;
	if(mIntervalTime > mLapseTime || mFadeOutTime ==0)
	{
		Parent::onRender(offset,updateRect);
	}
	else if((GFXTextureObject*)m_ProgressEffectTexture)
	{
		SimTime tempTime =  mIntervalTime + mFadeOutTime - mLapseTime;

		 if(mFadeOutTime >0 && tempTime >0)//µ­Èë
		 {
			  S32 alpha = mClamp(tempTime * (255.0f / mFadeOutTime),0,255);
			  GFX->getDrawUtil()->setBitmapModulation( ColorI(255,255,255,alpha) );
			  GFX->getDrawUtil()->drawBitmapStretch(m_ProgressEffectTexture,RectI(offset + m_ProgressEffectRect.point,m_ProgressEffectRect.extent));
		 }
	}
}

void GuiProgressCtrlEx::startProgress(SimTime time)
{
	 mIntervalTime = time;
	 mLapseTime = 0;
	 mLastUpdateTime = Platform::getRealMilliseconds();
}

void GuiProgressCtrlEx::endProgress()
{
	mIntervalTime = 0;
	mLapseTime = 0;
}
void GuiProgressCtrlEx::setFadeOutTime(SimTime time)
{
	mFadeOutTime = time;
}

ConsoleMethod(GuiProgressCtrlEx,setProgressStyle,void,3,3,"obj.setProgressStyle(%style)")
{
	object->setProgressStyle(atoi(argv[2]));
}

ConsoleMethod(GuiProgressCtrlEx,getProgressStyle,S32,2,2,"obj.getProgressStyle()")
{
	return object->getProgressStyle();
}

ConsoleMethod(GuiProgressCtrlEx,startProgress,void,3,3,"obj.startProgress(%time)")
{
	object->startProgress(atoi(argv[2]));
}

ConsoleMethod(GuiProgressCtrlEx,endProgress,void,2,2,"obj.endProgress()")
{
	object->endProgress();
	object->setProgressValue(0);
}

ConsoleMethod(GuiProgressCtrlEx,setFadeOutTime,void,3,3,"obj.setFadeOutTime(%time)")
{
	object->setFadeOutTime(atoi(argv[2]));
}