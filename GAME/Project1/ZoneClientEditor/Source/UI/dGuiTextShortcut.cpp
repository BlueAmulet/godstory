#include "dGuiTextShortcut.h"
#include "Gameplay\ClientGameplayState.h"
#include "Gameplay\item\Player_Item.h"
#include "console\consoleTypes.h"
#include "Gameplay\item\Res.h"

IMPLEMENT_CONOBJECT( GuiTextShortcut );

void GuiTextShortcut::initPersistFields()
{
	Parent::initPersistFields();

	addField( "Text", TypeString, Offset( mText, GuiTextShortcut ) );
}


void GuiTextShortcut::onMouseDown( const GuiEvent &event )
{
	Parent::onMouseDown( event );

	if( !mItemShortcut )
		return ;


	static GuiRichTextCtrl* pCtrl = (GuiRichTextCtrl*)Sim::findObject( "ChatLink_ItemInfo" );
	static GuiRichTextCtrl* pCompareCtrl = (GuiRichTextCtrl*)Sim::findObject( "CompareChat_ItemInfo" );
	
	if( !pCtrl || !pCompareCtrl )
		return ;

	std::string iconPath = "gameres/data/icon/item/";
	iconPath += mItemShortcut->getIconName();
	
	pCtrl->setDefaultLineHeight(16);
	generateDescText();
	pCtrl->setContent( mText );

	int height = pCtrl->getExtent().y;

	if( dStrcmp( mCompareText, "" ) != 0 )
	{
		pCompareCtrl->setDefaultLineHeight( 16 );
		pCompareCtrl->setContent( mCompareText );
	}

	if( pCompareCtrl->getExtent().y + 24 > height && dStrcmp( mCompareText, "" ) != 0 )
		height = pCompareCtrl->getExtent().y + 24;

	Con::executef("ShowChatLink_Item", Con::getIntArg( event.mousePoint.x ), Con::getIntArg( event.mousePoint.y - height - 20 ), Con::getIntArg( height ), iconPath.c_str() );
	
	if( dStrcmp( mCompareText, "" ) != 0 )
		Con::executef("ShowCompareChatLink_Item", Con::getIntArg( event.mousePoint.x ), Con::getIntArg( event.mousePoint.y - height - 20 ) , Con::getIntArg( height ), mCompareItemIcon.c_str() );
}

GuiTextShortcut::GuiTextShortcut()
{
	mText = StringTable->insert( "" );

	mCompareText = StringTable->insert( "" );

	mItemShortcut = NULL;
}

void GuiTextShortcut::setItemInfo( ItemShortcut* pItem )
{
	if( !pItem )
		return ;

	mItemShortcut = pItem;

	Res* pRes = pItem->getRes();

	char name[128];
	
	if( pRes )
	{
		sprintf_s( name, "<t o='0x121212ff' c='0xffffc0'>【%s】</t>", pRes->getItemName() );
		setText( name );
	}
}

void GuiTextShortcut::generateDescText()
{
	if( !mItemShortcut )
		return ;

	static char text[4096] = { 0, };

#pragma region 生成物品解析字符串

#ifdef NTJ_CLIENT
	text[0] = 0;
	g_ItemManager->insertText( g_ClientGameplayState->GetControlPlayer(), mItemShortcut->getRes(), text, SHORTCUTTYPE_INVENTORY, 0 );
	mText = StringTable->insert( text );

	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if( pPlayer )
	{
		ItemShortcut* pObject = (ItemShortcut*)pPlayer->equipList.GetSlot( mItemShortcut->getRes()->getEquipPos() );
		if( pObject )
		{
			text[0] = 0;
			g_ItemManager->insertText( pPlayer, pObject->getRes(), text, SHORTCUTTYPE_INVENTORY, 0 );
			mCompareText = StringTable->insert( text );

			mCompareItemName = pObject->getRes()->getItemName();
			mCompareItemIcon = "gameres/data/icon/item/";
			mCompareItemIcon += pObject->getRes()->getIconName();
		}
		else
		{
			mCompareText = StringTable->insert( "" );	
		}
	}
#endif
	
#pragma endregion 生成物品解析字符串
}

GuiTextShortcut::~GuiTextShortcut()
{
	if( mItemShortcut )
	{
		delete mItemShortcut;
	}
}