#include "guiListBoxAdvCtrl.h"
#include "console\consoleTypes.h"
#include <windows.h>

IMPLEMENT_CONOBJECT(GuiListBoxAdvCtrl);

IMPLEMENT_CONOBJECT(GuiListBoxAdvCtrlListItem);

IMPLEMENT_CONOBJECT(GuiListBoxAdvCtrlSubListItem);

void GuiListBoxAdvCtrl::initPersistFields()
{
	__super::initPersistFields();

	addField("columns",                 TypeS32Vector, Offset(mColumnWidth, GuiListBoxAdvCtrl));
	addField("lineHeight",				TypeS32, Offset(mLineHeight,GuiListBoxAdvCtrl));
} 

void GuiListBoxAdvCtrl::onRender( Point2I offset, const RectI& updateRect )
{
	Parent::onRender( offset, updateRect );
}

GuiListBoxAdvCtrl::GuiListBoxAdvCtrl()
{
	mLineHeight = 20; // 默认行高20px
}

GuiListBoxAdvCtrl::~GuiListBoxAdvCtrl()
{
	// 删除掉所有控件
	for each( int controlId in mListItems )
	{
		SimObject* pObj = Sim::findObject( controlId );
		if( pObj )
			pObj->deleteObject();
	}
}
 
int GuiListBoxAdvCtrl::find( const char* keyString )
{
	return mItemMap[keyString];
}

std::string GuiListBoxAdvCtrl::getColumn( const char* text, int i )
{
	char* pText = (char*)text;
	for(U32 index = 0; index < mColumnWidth.size(); index++)
	{
		const char *nextCol = dStrchr(pText, '\t');

		if( nextCol == NULL )
			return pText;

		dsize_t slen;

		if(nextCol)
			slen = nextCol - pText;
		else
			slen = dStrlen(text);

		std::string str = pText;

		str = str.substr( 0, slen );
		if( i == index )
			return str;

		pText = (char*)nextCol;
		pText++;
	}

	return "";
}

int GuiListBoxAdvCtrl::addItem( const char* text, const char* key )
{
	GuiListBoxAdvCtrlListItem* pNewItem = new GuiListBoxAdvCtrlListItem();
	
	pNewItem->registerObject();
	//pNewItem->setControlProfile( (GuiControlProfile*)Sim::findObject( "GuiModelessDialogProfile" ) );

	addObject( pNewItem );
	pNewItem->setKey( key );
	pNewItem->fadein( 0.5 );
	pNewItem->setExtent( getExtent().x, mLineHeight );
	int totalWidth = 0;
	for( int i=0; i < mColumnWidth.size(); i++ )
	{
		pNewItem->addColumn( getColumn( text, i ).c_str(), totalWidth, mColumnWidth[i] );
		totalWidth += mColumnWidth[i];
	}
 
	mItemMap[key] = pNewItem->getId();

	mListItems.push_back( pNewItem->getId() );

	resetLayout();

	return pNewItem->getId();
}

void GuiListBoxAdvCtrl::removeItem( int listItemId )
{
	GuiListBoxAdvCtrlListItem* pItem = (GuiListBoxAdvCtrlListItem*)Sim::findObject( listItemId );

	if( pItem )
	{
		mItemMap.erase( pItem->mKey );
		ItemList::iterator it = mListItems.begin();
		for( ;it != mListItems.end();it++ )
			if( *it == listItemId )
			{
				break;
			}

		pItem->fadeout( 0.5 );
	}
}

void GuiListBoxAdvCtrl::clearItems()
{
	ItemList::iterator it = mListItems.begin();
	for( ;it != mListItems.end();it++ )
	{
		GuiListBoxAdvCtrlListItem* pItem = (GuiListBoxAdvCtrlListItem*)Sim::findObject( *it );
		if( pItem )
			pItem->deleteObject();
	}

	mListItems.clear();
	mItemMap.clear();

	// 清空选择
	mSelectedKey = "";
}

void GuiListBoxAdvCtrl::removeItem( const char* key )
{
	removeItem( find( key ) );
}
// 重新布局
void GuiListBoxAdvCtrl::resetLayout()
{

	ItemList::iterator it;
	
	int i = 0;

	int lineHeight = 0;
	
	for( it = mListItems.begin(); it != mListItems.end(); it++, i++ )
	{
		int controlId = (*it);
		GuiListBoxAdvCtrlListItem* pControl = (GuiListBoxAdvCtrlListItem*)Sim::findObject( controlId );

		if( pControl )
		{
			pControl->setNewPosition( Point2I(0, lineHeight), 0.2f );
			lineHeight += pControl->getExtent().y;
		}
	}

	setExtent( getExtent().x, mListItems.size() * lineHeight );
}

void GuiListBoxAdvCtrl::onPreRender()
{
	bool isDirty = false;

	ItemList::iterator it;
	GuiListBoxAdvCtrlListItem* pItem;

	for( it = mListItems.begin(); it != mListItems.end(); it++ )
	{
		int controlId = (*it);
		pItem = (GuiListBoxAdvCtrlListItem*)Sim::findObject( controlId );
		if( pItem )
		{
			if( pItem->mAlpha == 0 )
			{
				isDirty = true;
				it++;
				removeObject( pItem );
				pItem->deleteObject();
				continue;
			}
		}
	}
	if( isDirty )
		resetLayout();
}

GuiListBoxAdvCtrl::ItemMap& GuiListBoxAdvCtrl::getItemMap()
{
	return mItemMap;
}

void GuiListBoxAdvCtrl::setSelectedItem( const char* key )
{
	mSelectedKey = key;
}

std::string GuiListBoxAdvCtrl::getSelectedItem()
{
	return mSelectedKey;
}

std::string GuiListBoxAdvCtrl::getListBoxTagText(const char* keyString)
{
	listBoxTagMap::iterator  it = mTagMap.find(keyString);
	if(it != mTagMap.end())
		return it->second;

	return "";
}

bool GuiListBoxAdvCtrl::setListBoxTagText(const char* text, const char* key)
{
	if(!text || !key)
		return false;

	mTagMap.insert(listBoxTagMap::value_type(key,text));

	return true;
}

bool GuiListBoxAdvCtrl::setCommand(const char* cmdString,const char* key)
{
	int iListId = find(key);
	GuiListBoxAdvCtrlListItem* pItem = (GuiListBoxAdvCtrlListItem*)Sim::findObject( iListId );
	if(!pItem)
		return false;

	if(cmdString)
		pItem->mConsoleCommand = StringTable->insert(cmdString);
	else
		pItem->mConsoleCommand = StringTable->insert(cmdString);

	return true;

}

bool GuiListBoxAdvCtrl::setAltCommand(const char* cmdString,const char* key)
{
	int iListId = find(key);
	GuiListBoxAdvCtrlListItem* pItem = (GuiListBoxAdvCtrlListItem*)Sim::findObject( iListId );
	if(!pItem)
		return false;

	if(cmdString)
		pItem->mAltConsoleCommand = StringTable->insert(cmdString);
	else 
		pItem->mAltConsoleCommand = StringTable->insert("");

	return true;
}

GuiListBoxAdvCtrlListItem::~GuiListBoxAdvCtrlListItem()
{
	// 删除掉所有控件
	for each( int controlId in mSubItems )
	{
		SimObject* pObj = Sim::findObject( controlId );
		if( pObj )
			pObj->deleteObject();
	}
}


void GuiListBoxAdvCtrlListItem::initPersistFields()
{
	__super::initPersistFields();
}

void GuiListBoxAdvCtrlListItem::addColumn( const char* text, int totalWidth, int width )
{
	GuiListBoxAdvCtrlSubListItem* pItem = new GuiListBoxAdvCtrlSubListItem();
	pItem->registerObject();
	//pItem->setControlProfile( (GuiControlProfile*)Sim::findObject( "GuiModelessDialogProfile" ) );

	pItem->setPosition( Point2I( totalWidth, 0 ) );
	pItem->setExtent( width, 20 );
	pItem->setContent( text );
	addObject( pItem );
	GuiListBoxAdvCtrlListItem* pParent = (GuiListBoxAdvCtrlListItem*)getParent();
}

void GuiListBoxAdvCtrlListItem::setKey( const char* keyString )
{
	mKey = keyString;
}

GuiListBoxAdvCtrlListItem::GuiListBoxAdvCtrlListItem()
{
	mIsMoving = false;
}

void GuiListBoxAdvCtrlListItem::setNewPosition( Point2I newPoint, float time )
{
	mIsMoving = true;
	mOldPoint = getPosition();
	mNewPoint = newPoint;
	mMoveTime = time;
	mLastTime = ::GetTickCount();
}

void GuiListBoxAdvCtrlListItem::onPreRender()
{
	Parent::onPreRender();

	if( mIsMoving )
	{
		U32 nowTime = ::GetTickCount();

		float elapsedTime = ( nowTime - mLastTime ) / 1000.0f;
		float delta = elapsedTime / mMoveTime;
		setPosition( Point2I( mOldPoint.x + ( mNewPoint.x - mOldPoint.x ) * delta, mOldPoint.y + ( mNewPoint.y - mOldPoint.y ) * delta ) );

		if( elapsedTime > mMoveTime )
		{
			mIsMoving = false;
			setPosition( mNewPoint );
		}
	}
}

void GuiListBoxAdvCtrlListItem::onMouseDown( const GuiEvent &event )
{
	GuiListBoxAdvCtrl* pControl = (GuiListBoxAdvCtrl*)getParent();	
	if( !pControl )
		return ;

	// 设置选中
	pControl->setSelectedItem( mKey.c_str() );

	Con::executef(this,"onMouseDown");

	if(event.mouseClickCount>1)
		execAltConsoleCallback();
	else
		execConsoleCallback();
}

void GuiListBoxAdvCtrlSubListItem::initPersistFields()
{
	__super::initPersistFields();
}

GuiListBoxAdvCtrlSubListItem::GuiListBoxAdvCtrlSubListItem()
{
	
}

void GuiListBoxAdvCtrlSubListItem::onMouseDown( const GuiEvent &event )
{
	__super::onMouseDown(event);

	
	// 继续执行选中操作
	mClickOut = true;
}

ConsoleMethod( GuiListBoxAdvCtrl, AddItem, void, 4,4, "obj.addItem(%text,%key)")
{
	if(dStrcmp(argv[3],"") == 0)
	{
		AssertFatal(false,"AddItem:: Key is NULL");
		return;
	}

	object->addItem( argv[2], argv[3] );
}

ConsoleMethod( GuiListBoxAdvCtrl, RemoveItem, void, 3,3, "obj.addItem(%key)")
{
	object->removeItem( argv[2] );
}

ConsoleMethod( GuiListBoxAdvCtrl, ClearItems, void, 2, 2, "" )
{
	object->clearItems();
}

ConsoleMethod( GuiListBoxAdvCtrlListItem, setNewPosition, void, 5, 5, "" )
{
	Point2I lPos(dAtoi(argv[2]), dAtoi(argv[3]));
	object->setNewPosition(lPos, atof(argv[4]) );
}

ConsoleMethod( GuiListBoxAdvCtrl,getSelectedId,const char*, 2, 2, "object.getselectedId()")
{		
	std::string s = object->getSelectedItem();
	char* buf = Con::getReturnBuffer(128);
	dStrcpy(buf,128,s.c_str());	
	return  buf;
}

ConsoleMethod(GuiListBoxAdvCtrl,getListBoxTagText,const char*,3,3,"obj.getListBoxTagText(%key)")
{
	std::string str = object->getListBoxTagText(argv[2]);
	char* buff = Con::getReturnBuffer(512);
	dStrcpy(buff,512,str.c_str());
	return buff;
}

ConsoleMethod(GuiListBoxAdvCtrl,setListBoxTagText,void,4,4,"obj.setListBoxTagText(%text,%key)")
{
	if(dStrcmp(argv[3],"") == 0)
	{
		AssertFatal(false,"setListBoxTagText:: Key is NULL");
		return;
	}

	object->setListBoxTagText(argv[2],argv[3]);
}

ConsoleMethod(GuiListBoxAdvCtrl,setCommand,void,4,4,"obj.setCommand(%strCmd,%key)")
{
	if(dStrcmp(argv[3],"") == 0)
	{
		AssertFatal(false,"setCommand:: Key is NULL");
		return;
	}

	object->setCommand(argv[2],argv[3]);
}

ConsoleMethod(GuiListBoxAdvCtrl,setAltCommand,void,4,4,"obj.setAltCommand(%strCmd,%key)")
{
	if(dStrcmp(argv[3],"") == 0)
	{
		AssertFatal(false,"setAltCommand:: Key is NULL");
		return;
	}

	object->setAltCommand(argv[2],argv[3]);
}