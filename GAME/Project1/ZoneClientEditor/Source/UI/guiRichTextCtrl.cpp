#include "guiRichTextCtrl.h"

#include "console/consoleTypes.h"
IMPLEMENT_CONOBJECT(GuiRichTextCtrl);

GuiRichTextCtrl::GuiRichTextCtrl()
{
	mText = NULL;
	mMouseRightDown = StringTable->insert("");
	mLineHeight = 16;
}

GuiRichTextCtrl::~GuiRichTextCtrl()
{
	if( mText )
		delete[] mText;
}

void GuiRichTextCtrl::addContent( const char* pText )
{
	mDrawer.addContent( pText );

	setHeight( mDrawer.getHeight() );
}

void GuiRichTextCtrl::setContent( const char* pText )
{
	// delete all childrend
	this->clear();

	CRichTextDrawer::Style style;
	strcpy_s( style.fontName, sizeof( style.fontName ), getControlProfile()->mFontType );
	style.fontSize = getControlProfile()->mFontSize;
	style.fontColor = getControlProfile()->mFontColor;
	style.fontIsBitmap = getControlProfile()->mBitmapFont;
	style.outline = getControlProfile()->mFontOutline;
	
	mDrawer.setDefaultStyle( style );
	mDrawer.setParentControl( this );
	mDrawer.setControlOffset( getGlobalBounds().point );
	mDrawer.setDefaultLineHeight( mLineHeight);


	if( mText )
	{
		delete[] mText;
		mText = NULL;
	}

	int len = strlen( pText );
	mText = new char[ len + 1];
	strcpy_s( mText, len + 1, pText );

	mDrawer.setWidth( getWidth() );	
	mDrawer.setContent( mText );

	setHeight( mDrawer.getHeight() );

}

bool GuiRichTextCtrl::onWake()
{
	if (! Parent::onWake())
		return false;

	return mDrawer.onWake();
}

void GuiRichTextCtrl::onSleep()
{
	Parent::onSleep();

	mDrawer.onSleep();
}

void GuiRichTextCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addField("MouseRightDown", TypeString, Offset(mMouseRightDown,GuiRichTextCtrl) );
	addField("LineHeight", TypeS32, Offset(mLineHeight,GuiRichTextCtrl) );

}

void GuiRichTextCtrl::onPreRender()
{
	mDrawer.onPreRender();
}

void GuiRichTextCtrl::onRender( Point2I offset, const RectI &updateRect )
{
	Parent::onRender( offset, updateRect );
	
/*
	GuiControl* pControl = getParent();

	RectI ctrlRect;

	if( pControl )
	{
		ctrlRect.set( pControl->getPosition(), pControl->getExtent() );
	}
	
	GFX->setClipRect( ctrlRect );*/

	// 增加单行解析控件对齐问题 [10/30/2009 LivenHotch]
	int textWidth = mDrawer.getRealLineWidth();

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
		// GuiControlProfile::LeftJustify
		start.set( 0, 0 );
		break;
	}

	start += offset;

	mDrawer.render( start, updateRect );

	//if( mMask.getPointer() != NULL )
	//{
	//	GFX->setTextureStageColorArg1(0,  GFXTACurrent);
	//	GFX->setTextureStageColorArg2(0,  GFXTATexture);
	//	GFX->setTextureStageColorOp(0, GFXTOPModulate);
	//	GFX->setTextureStageAlphaArg1(0, GFXTADiffuse );
	//	GFX->setTextureStageAlphaArg2(0, GFXTATexture );
	//	GFX->setTextureStageAlphaOp(0, GFXTOPModulate);
	//	GFX->setTextureStageColorOp( 1, GFXTOPDisable );
	//	GFX->setTextureStageAlphaOp( 1, GFXTOPDisable );

	//	GFX->setAlphaBlendEnable(false);
	//	GFX->getDrawUtil()->drawBitmap( mMask, offset );
	//}
	
}

void GuiRichTextCtrl::refresh()
{
	mDrawer.setWidth( getWidth() );

	if( mText )
		mDrawer.relayout();
}

CRichTextDrawer* GuiRichTextCtrl::getDrawer()
{
	return &mDrawer;
}

void GuiRichTextCtrl::onMouseUp( const GuiEvent &event )
{
	mDrawer.onMouseUp( event );
}

void GuiRichTextCtrl::onMouseDown( const GuiEvent &event )
{
	mDrawer.onMouseDown( event );
}

void GuiRichTextCtrl::onMouseMove( const GuiEvent &event )
{
	mDrawer.onMouseMove( event );
}

void GuiRichTextCtrl::onMouseLeave( const GuiEvent &event )
{
	mDrawer.onMouseLeave( event );
}

void GuiRichTextCtrl::onMouseDragged( const GuiEvent &event )
{
	mDrawer.onMouseDragged( event );
}

void GuiRichTextCtrl::onRightMouseDown(const GuiEvent &event)
{
   if (mMouseRightDown && mMouseRightDown[0])
   {

	   Con::setVariable("$ThisControl",avar("%d",getId()));
	   Con::evaluate(mMouseRightDown,false);
   }
}

void GuiRichTextCtrl::setDefaultLineHeight( int lineHeight )
{
	mLineHeight = lineHeight;
}

ConsoleMethod( GuiRichTextCtrl, SetContent, void, 3,3, "" )
{
	object->setContent( argv[2] );
}

ConsoleMethod( GuiRichTextCtrl, AddContent, void, 3,3, "" )
{
	object->addContent( argv[2] );
}

ConsoleMethod( GuiRichTextCtrl, Refresh, void, 2, 2, "" )
{
	object->mDrawer.setWidth( object->getWidth() );
	object->mDrawer.relayout();
}

ConsoleMethod( GuiRichTextCtrl, GetNodeAttribute, const char*, 4, 4, "" )
{
	 CTinyXElement* pElement = object->mDrawer.mDocument.GetElementById( argv[2] );
	 if( pElement )
		 return pElement->getAttribute( argv[3] );

	 return NULL;
}

ConsoleMethod( GuiRichTextCtrl, SetNodeAttribute, void, 5, 5, "" )
{
	CTinyXElement* pElement = object->mDrawer.mDocument.GetElementById( argv[2] );
	if( pElement )
		pElement->setAttribute( argv[3], argv[4] );

}