//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "console/consoleTypes.h"
#include "ui/guiClockCtrl.h"
#include "gfx/gfxFontRenderBatcher.h"

IMPLEMENT_CONOBJECT(GuiClockCtrl);

GuiClockCtrl::GuiClockCtrl() : GuiTickCtrl()
{
	mUseSetColor	= false;
	mFont			= NULL;
	mText[0]		= '\0';
	mLifeTime		= 0;
	mLastUpdateTime	= Platform::getVirtualMilliseconds();
}

GuiClockCtrl::~GuiClockCtrl()
{
	mFont			= NULL;
	mText[0]		= '\0';
}

//--------------------------------------------------------------------------
void GuiClockCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	if(mLifeTime <= 0 || !mText[0])
		return;

	m_DirtyFlag = false;
	if (mUseSetColor)
	{
		GFX->getDrawUtil()->setBitmapModulation( mSetColorRGB );
	} 
	else
	{
		GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColor );
	}

	S32 textWidth = mFont->getStrWidth(( UTF8*)mText);
	
	//水平居中
	Point2I start;
	switch( getControlProfile()->mAlignment )
	{
	case GuiControlProfile::RightJustify:
		start.set( getExtent().x - textWidth, 0 );
		break;
	case GuiControlProfile::CenterJustify:
		start.set( ( getExtent().x - textWidth) / 2, 0 );
		break;
	default:
		start.set( 0, 0 );
		break;
	}

	if( textWidth > getExtent().x )
		start.set( 0, 0 );

	// 垂直居中
	if(mFont->getHeight() > getExtent().y)
		start.y = 0 - ((mFont->getHeight() - getExtent().y) / 2) ;
	else
		start.y = ( getExtent().y - mFont->getHeight() ) / 2;

	ColorI color;
	GFX->getDrawUtil()->getBitmapModulation().getColor(&color);

	if (mProfile->mFontOutline == ColorI(0,0,0))
		GFX->getDrawUtil()->drawText( mFont, start + offset, mText, &color );
	else
		GFX->getDrawUtil()->drawTextOutline(mFont, start + offset, mText, &color,true,mProfile->mFontOutline);

	/*renderJustifiedText(offset, getExtent(), (char*)mText);*/
}

bool GuiClockCtrl::onWake()
{
	if ( !Parent::onWake() )
		return false;

	mFont = mProfile->mFont;
	if(!mFont)
		mFont = CommonFontManager::GetFont("宋体", 12);
	return true;
}

void GuiClockCtrl::onSleep()
{
	Parent::onSleep();
	mFont = NULL;
}

void GuiClockCtrl::processTick()
{
	if(mLifeTime <= 0 )
		return;
	U32 uCurTick = Platform::getVirtualMilliseconds();
	if(uCurTick < mLastUpdateTime + 1000)
		return;
	mLifeTime --;
	if(mLifeTime >= 3600)
		dSprintf(mText, 32, "%2d小时", mLifeTime / 3600);
	else if(mLifeTime >= 60)
		dSprintf(mText, 32, "%2d分", mLifeTime / 60);
	else
		dSprintf(mText, 32, "%2d秒", mLifeTime);
	
	GuiControl* pControl = dynamic_cast<GuiControl*>(Sim::findObject("MissionTraceWnd_Tree"));
	if(pControl)
		pControl->setDirtyFlag();
	mLastUpdateTime = uCurTick;	
}

void GuiClockCtrl::SetColor(ColorI& col)
{
	mSetColorRGB = col;
	mUseSetColor = true;
}

void GuiClockCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addField( "lifetime",		TypeS32,      Offset( mLifeTime, GuiClockCtrl ) );
}

ConsoleMethod( GuiClockCtrl, setColor,void, 5, 5, "obj.setTextColor(%R,%G,%B)")
{
	argc;
	ColorI temp(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),255);
	object->SetColor(temp);
}