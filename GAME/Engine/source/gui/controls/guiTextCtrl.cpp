//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/consoleTypes.h"
#include "console/console.h"
#include "core/color.h"
#include "gui/controls/guiTextCtrl.h"
#include "i18n/lang.h"
#include "gfx/gfxFontRenderBatcher.h"

// -----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiTextCtrl);

GuiTextCtrl::GuiTextCtrl()
{
	//default fonts
	mInitialText = StringTable->insert("");
	mInitialTextID = StringTable->insert("");
	mText[0] = '\0';
	mMaxStrLen = GuiTextCtrl::MAX_STRING_LENGTH;
	mUseSetColor = false;
}

ConsoleMethod( GuiTextCtrl, setText, void, 3, 3, "obj.setText( newText )" )
{
	argc;
	object->setText( argv[2] );
}

ConsoleMethod( GuiTextCtrl, setTextID, void, 3, 3, "obj.setTextID( newText )" )
{
	argc;
	object->setTextID( argv[2] );
}

ConsoleMethod( GuiTextCtrl, setColor,void, 5, 5, "obj.setTextColor(%R,%G,%B)")
{
    argc;
    ColorI temp(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),255);
	object->SetColor(temp);
}

void GuiTextCtrl::initPersistFields()
{
	Parent::initPersistFields();
	//addField( "text",       TypeCaseString,  Offset( mInitialText, GuiTextCtrl ) );
	addProtectedField("text", TypeCaseString, Offset(mInitialText, GuiTextCtrl), setText, getTextProperty, "");
	addField( "textID",     TypeString,      Offset( mInitialTextID, GuiTextCtrl ) );
	addField( "maxLength",  TypeS32,         Offset( mMaxStrLen, GuiTextCtrl ) );     
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

bool GuiTextCtrl::onAdd()
{
	if(!Parent::onAdd())
		return false;
	dStrcpy(mText,MAX_STRING_LENGTH,(UTF8*)mInitialText);
	mText[MAX_STRING_LENGTH] = '\0';
	return true;
}

void GuiTextCtrl::inspectPostApply()
{
	Parent::inspectPostApply();
	if(mInitialTextID && *mInitialTextID != 0)
		setTextID(mInitialTextID);
	else
		setText(mInitialText);
}

bool GuiTextCtrl::onWake()
{
	if ( !Parent::onWake() )
		return false;

    mFont = mProfile->mFont;
	AssertFatal(mFont, "GuiTextCtrl::onWake: invalid font in profile" );
	if(mInitialTextID && *mInitialTextID != 0)
		setTextID(mInitialTextID);

	if ( mConsoleVariable[0] )
	{
		const char *txt = Con::getVariable( mConsoleVariable );
		if ( txt )
		{
			if ( dStrlen( txt ) > mMaxStrLen )
			{
				char* buf = new char[mMaxStrLen + 1];
				dStrncpy( buf, mMaxStrLen + 1, txt, mMaxStrLen );
				buf[mMaxStrLen] = 0;
				setScriptValue( buf );
				delete [] buf;
			}
			else
				setScriptValue( txt );
		}
	}

	//resize
	autoResize();

	return true;
}

void GuiTextCtrl::autoResize()
{
	Point2I newExtents = getExtent();
	if ( mProfile->mAutoSizeWidth )
		newExtents.x = mFont->getStrWidth(( UTF8 *) mText );
	if ( mProfile->mAutoSizeHeight )
		newExtents.y = mFont->getHeight() + 4;

	if ( mProfile->mAutoSizeWidth || mProfile->mAutoSizeHeight)
		setExtent( newExtents );
}

void GuiTextCtrl::onSleep()
{
	Parent::onSleep();
	mFont = NULL;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiTextCtrl::setText(const char *txt)
{
	//make sure we don't call this before onAdd();
	if( !mProfile )
		return;

	if (txt)
		dStrncpy(mText, sizeof(mText), (UTF8*)txt, MAX_STRING_LENGTH);
	mText[MAX_STRING_LENGTH] = '\0';

	//Make sure we have a font
	mProfile->incRefCount();
    mFont = mProfile->mFont;
	//resize
	autoResize();

	setVariable((char*)mText);
	setUpdate();

	//decrement the profile referrence
	mProfile->decRefCount();
} 

void GuiTextCtrl::setTextID(const char *id)
{
	S32 n = Con::getIntVariable(id, -1);
	if(n != -1)
	{
		mInitialTextID = StringTable->insert(id);
		setTextID(n);
	}
}
void GuiTextCtrl::setTextID(S32 id)
{
	const UTF8 *str = getGUIString(id);
	if(str)
		setText((const char*)str);
	//mInitialTextID = id;
}


void GuiTextCtrl::SetColor(ColorI& col)
{
   mSetColorRGB = col;
   mUseSetColor = true;
}
//------------------------------------------------------------------------------
void GuiTextCtrl::onPreRender()
{
	const char * var = getVariable();
	if(var && var[0] && dStricmp((char*)mText, var))
		setText(var);
	//Con::executef( this,"Prerender");
}

//------------------------------------------------------------------------------
void GuiTextCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	m_DirtyFlag = false;
	if (mUseSetColor)
	{
		GFX->getDrawUtil()->setBitmapModulation( mSetColorRGB );
	} 
	else
	{
		GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColor );
	}	
	renderJustifiedText(offset, getExtent(), (char*)mText);
	//render the child controls
	renderChildControls(offset, updateRect);
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

const char *GuiTextCtrl::getScriptValue()
{
	return getText();
}

void GuiTextCtrl::setScriptValue(const char *val)
{
	setText(val);
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
// EOF //
