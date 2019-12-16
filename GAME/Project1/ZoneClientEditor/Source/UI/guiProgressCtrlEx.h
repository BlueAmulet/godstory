//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _GUIPROGRESSCTRLEX_H_
#define _GUIPROGRESSCTRLEX_H_
#include "console/sim.h"
class GuiProgressCtrl;
class GuiProgressCtrlEx :public GuiProgressCtrl
{
	typedef GuiProgressCtrl Parent;
public:
	enum styleConstants
	{
		STYLE_INCREMENT = 0,     // ¥””“≤‡µ›‘ˆ
		STYLE_DEGRESSION,        // ¥””“≤‡µ›ºı
		STYLE_BREAK,             // ÷’÷π
	};

public:
	DECLARE_CONOBJECT(GuiProgressCtrlEx);
	GuiProgressCtrlEx();
	static void initPersistFields();

	void onPreRender     ();
	bool UpdateDate      ();
	void onRender        (Point2I offset, const RectI &updateRect);
	void startProgress   (SimTime time);
	void endProgress     ();
	void setFadeOutTime  (SimTime time);

	inline  U8       getProgressStyle()           { return mProgressStyle;}
	inline  void     setProgressStyle(U8 value)   { mProgressStyle = value;}

private: 
	U8          mProgressStyle;
	SimTime		mUpdateInterval;				///< Unit: ms.
	SimTime		mLastUpdateTime;
	SimTime		mCurrentTime;
	SimTime     mIntervalTime;
	SimTime     mLapseTime;
	SimTime     mFadeOutTime;
};

#endif