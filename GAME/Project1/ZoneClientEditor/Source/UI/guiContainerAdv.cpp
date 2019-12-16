#include "guiContainerAdv.h"
#include "console\consoleTypes.h"
#include <windows.h>

IMPLEMENT_CONOBJECT(GuiContainerAdv);

void GuiContainerAdv::initPersistFields()
{
	Parent::initPersistFields();

	addField("alpha",                 TypeS32, Offset(mAlpha, GuiContainerAdv));
	addField("useFadeout",                 TypeBool, Offset(mUseFadeout, GuiContainerAdv));
}

GuiContainerAdv::GuiContainerAdv()
{
	mAlpha = 255;
	mFallType = 0;
	mUseFadeout = true;
	mIsVisible = true;
}

void GuiContainerAdv::onRender( Point2I offset, const RectI &updateRect )
{
	if( GFX->getDrawUtil()->isAlphaLocked() )
		Parent::onRender( offset, updateRect );
	else
	{
		GFX->getDrawUtil()->lockAlpha( mAlpha );
		Parent::onRender( offset, updateRect );
		GFX->getDrawUtil()->unlockAlpha();
	}
}

void GuiContainerAdv::fadein( float time )
{
	mFallType = 1;
	mFallTime = time;
	mLastTime = ::GetTickCount();
}

void GuiContainerAdv::fadeout( float time )
{
	mFallType = 2;
	mFallTime = time;
	mLastTime = ::GetTickCount();
}

void GuiContainerAdv::onPreRender()
{
	Parent::onPreRender();

	float elapsedTime = ( ::GetTickCount() - mLastTime ) / 1000.0f;
	if( mFallType == 1 )
		mAlpha = 1 + int( ( elapsedTime / mFallTime ) * 255 );

	if( mAlpha > 255 )
		mAlpha = 255;
	
	if( mFallType == 2)
		mAlpha = 255 - int( ( elapsedTime / mFallTime ) * 255 );

	if( mAlpha < 0 )
		mAlpha = 0;

	if( mAlpha == 0 && mUseFadeout )
		Parent::setVisible( false );

	if( elapsedTime > mFallTime )
	{
		if(mFallType == 2)
		{
			mAlpha = 0;
			parentSetVisible(true);
		}
		else if(mFallType == 1)
		{
			mAlpha = 255;
			parentSetVisible(true);
		}
		mFallType = 0;
	}
	else
	{
		parentSetVisible(true);
	}
}

void GuiContainerAdv::setVisible( bool value )
{
	if( !mUseFadeout )
	{

		Parent::setVisible( value );
		return ;
	}

	if( value )
	{
		Parent::setVisible( value );	
		fadein( 0.2 );
	}
	else
	{
		if( isVisible() && !GFX->getDrawUtil()->isAlphaLocked() )
			fadeout( 0.2 );
	}

	mIsVisible = value;
}

bool GuiContainerAdv::isVisible()
{
	return mIsVisible;
}

ConsoleMethod( GuiContainerAdv, fallIn, void, 3, 3, "" )
{
	object->fadein( atof( argv[2] ) );
}

ConsoleMethod( GuiContainerAdv, fallOut, void, 3, 3, "" )
{
	object->fadeout( atof( argv[2] ) );
}

ConsoleMethod( GuiContainerAdv, isVisible, bool, 2, 2, "" )
{
	return object->isVisible();
}



