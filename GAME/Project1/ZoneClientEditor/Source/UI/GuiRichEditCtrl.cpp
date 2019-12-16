#include "GuiRichEditCtrl.h"
#include "core\stringBuffer.h"
#include <windows.h>
#include "core\unicode.h"

IMPLEMENT_CONOBJECT(GuiRichEditCtrl);

void GuiRichEditCtrl::onRender( Point2I offset, const RectI &updateRect )
{
	mDrawer.render( offset, updateRect );

	if( ::GetTickCount() - mLastCursorTime > 500 )
	{
		mShowCursor = !mShowCursor;
		mLastCursorTime = ::GetTickCount();
	}
	if( mShowCursor )
		drawCursor( offset );
}

GuiRichEditCtrl::GuiRichEditCtrl()
{
	mCurrentItem = NULL;

	mLastCursorTime = 0;

}

GuiRichEditCtrl::~GuiRichEditCtrl()
{
	clearItemList();
}

void GuiRichEditCtrl::initPersistFields()
{
	Parent::initPersistFields();
}

void GuiRichEditCtrl::setCursorPosition( Point2I position )
{

}

Point2I GuiRichEditCtrl::getCursorPosition()
{
	return Point2I( 0, 0 );
}

void GuiRichEditCtrl::insertChar( const wchar_t Char )
{
	CharItem* pItem = new CharItem();
	pItem->Char[0] = Char;
	pItem->Char[1] = 0;

	mList.push_back( pItem );
}

void GuiRichEditCtrl::insertPic( const char* pPicPath )
{

}

void GuiRichEditCtrl::drawCursor( const Point2I& offset )
{
	
}

void GuiRichEditCtrl::buildText()
{
	StringBuffer sb;
	ItemList::iterator it;
	sb.append( "<t>" );
	for( it = mList.begin(); it != mList.end(); it++ )
	{
		sb.append( (*it)->getText().c_str() );
	}

	sb.append( "</t>" );
	mDrawer.setContent( sb.getPtr8() );
}

void GuiRichEditCtrl::clearItemList()
{
	for each( Item* pItem in mList )
	{
		delete pItem;
	}

	mList.clear();
}

bool GuiRichEditCtrl::onKeyDown( const GuiEvent &event )
{
	// ����
	if( event.keyCode == KEY_C && event.modifier & SI_CTRL )
	{
		return true;
	}

	// ճ��
	if( event.keyCode == KEY_V && event.modifier & SI_CTRL )
	{
		return true;
	}

	// �˸�
	if( event.keyCode == KEY_BACKSPACE )
	{
		if( mCurrentItem != NULL )
			mList.remove( mCurrentItem );

		return true;
	}

	//// ���A��
	//if( event.keyCode == KEY_UP )
	//{
	//	return ;
	//}

	//// �������
	//if( event.keyCode == KEY_DOWN )
	//{
	//	return ;
	//}

	// �������
	if( event.keyCode == KEY_LEFT )
	{
		return true;
	}

	// �������
	if( event.keyCode == KEY_RIGHT )
	{
		return true;
	}

	Item* pItem;

	// ����
	if( event.keyCode == KEY_RETURN )
	{
		pItem = new ReturnItem();
	}
	else
	{
		pItem = new CharItem();
		((CharItem*)pItem)->Char[0] = event.ascii;
		((CharItem*)pItem)->Char[1] = 0;
	}

	if( mCurrentItem )
	{
		ItemList::iterator it;
		for( it = mList.begin(); it != mList.end(); it++ )
		{
			if( *it == mCurrentItem )
				break;
		}

		if( it != mList.end() )
			mList.insert( it, pItem );   // ���뵽���λ��
	}
	else
	{
		mList.push_back( pItem );
	}

	mCurrentItem = pItem;

	// �����ı�������
	buildText();

	return true;
}

std::string GuiRichEditCtrl::CharItem::getText()
{
	char* pRet = (char *)convertUTF16toUTF8( Char );
	std::string ret( pRet );

	if (pRet)
		delete [] pRet;
	return ret;
}

std::string GuiRichEditCtrl::PicItem::getText()
{
	return "</t><t>";
}

std::string GuiRichEditCtrl::ReturnItem::getText()
{
	return "</t><b/><t>";
}