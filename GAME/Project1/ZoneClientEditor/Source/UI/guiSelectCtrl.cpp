#include "guiSelectCtrl.h"
#include "gfx/gfxFontRenderBatcher.h"

IMPLEMENT_CONOBJECT(GuiSelectCtrl);


GuiSelectCtrl::GuiSelectCtrl()
{
	mItemHeight = 20;
	mMaxCount  = 10;

	mSelectIndex = -1;
}

GuiSelectCtrl::~GuiSelectCtrl()
{

}

void GuiSelectCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addGroup("GuiSelectCtrl");
	addField("ItemHeight" ,  TypeS32,  Offset(mItemHeight, GuiSelectCtrl));
	addField("MaxCount",	TypeS32,   Offset(mMaxCount,GuiSelectCtrl));
	endGroup("GuiSelectCtrl");
}

bool GuiSelectCtrl::onKeyDown( const GuiEvent &event )
{
	if( mSelectIndex < 0 )
		return false;

	if( event.keyCode == KEY_DOWN )
		mSelectIndex++;

	if( event.keyCode == KEY_UP )
		mSelectIndex--;

	if( mSelectIndex < 0 )
		mSelectIndex = mMaxCount - 1;

	mSelectIndex %= mMaxCount;

	return true;
}

const char* GuiSelectCtrl::getSelectItem()
{
	StringList::iterator it;	
	int ind = 0;
	for( it = mStringList.begin(); it != mStringList.end(); it++ )
	{
		if( ind == mSelectIndex )
			return it->c_str();

		ind++;
	}
	return "";
}

void GuiSelectCtrl::addItem( const char* string )
{
	std::string str = string;
	mStringList.push_back( str );

	mSelectIndex = mStringList.size() - 1;
}

void GuiSelectCtrl::setCount( int nCount )
{
	mMaxCount = nCount;
}

void GuiSelectCtrl::drawBackGround( Point2I& offset )
{
	RectI rect( offset, getExtent() );
	GFX->getDrawUtil()->drawRectFill( rect, ColorI( 0, 0, 0, 100 ) );
}

void GuiSelectCtrl::drawSelectBox( Point2I& offset )
{
	if( mSelectIndex < 0 || mSelectIndex >= mMaxCount )
		return ;

	RectI rect( offset + Point2I( 0, mItemHeight * mSelectIndex ), Point2I( getWidth(), mItemHeight ) );
	GFX->getDrawUtil()->drawRectFill( rect, ColorI( 0, 0, 0, 200 ) );
}

void GuiSelectCtrl::drawAllItemString( Point2I& offset )
{
	CommonFontEX* pFont = mProfile->mFont;

	StringList::iterator it;
	GFX->getDrawUtil()->clearBitmapModulation();

	int i = 0;

	int offsetY = ( mItemHeight - pFont->getHeight() ) / 2 ; // ÐÐ¾ÓÖÐ»æÖÆ

	for(it = mStringList.begin(); it != mStringList.end(); it++ )
	{
		GFX->getDrawUtil()->drawText( pFont, offset + Point2I(0, i * mItemHeight), (*it).c_str() );
		i++;
	}
}

void GuiSelectCtrl::onRender( Point2I offset, const RectI &updateRect )
{
	drawBackGround( offset );
	drawSelectBox( offset );
	drawAllItemString( offset );

	Parent::onRender( offset, updateRect );
}

ConsoleMethod( GuiSelectCtrl, addItem, void, 3, 3, "" )
{
	object->addItem( argv[2] );
}

void GuiSelectCtrl::onMouseDown( const GuiEvent& event )
{
	return ;
}