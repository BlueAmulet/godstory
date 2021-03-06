//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/consoleTypes.h"
#include "console/console.h"
#include "gui/core/guiCanvas.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gui/controls/guiTextListCtrl.h"
#include "sim/actionMap.h"
#include "gui/editor/guiMenuBar.h"
#include "gfx/gfxDevice.h"
#include "gfx/primBuilder.h"
#include "gfx/gfxFontRenderBatcher.h"

// menu bar:
// basic idea - fixed height control bar at the top of a window, placed and sized in gui editor
// menu text for menus or menu items should not begin with a digit
// all menus can be removed via the clearMenus() console command
// each menu is added via the addMenu(menuText, menuId) console command
// each menu is added with a menu id
// menu items are added to menus via that addMenuItem(menu, menuItemText, menuItemId, accelerator, checkGroup) console command
// each menu item is added with a menu item id and an optional accelerator
// menu items are initially enabled, but can be disabled/re-enabled via the setMenuItemEnable(menu,menuItem,bool)
// menu text can be set via the setMenuText(menu, newMenuText) console method
// menu item text can be set via the setMenuItemText console method
// menu items can be removed via the removeMenuItem(menu, menuItem) console command
// menu items can be cleared via the clearMenuItems(menu) console command
// menus can be hidden or shown via the setMenuVisible(menu, bool) console command
// menu items can be hidden or shown via the setMenuItemVisible(menu, menuItem, bool) console command
// menu items can be check'd via the setMenuItemChecked(menu, menuItem, bool) console command
//    if the bool is true, any other items in that menu item's check group become unchecked.
//
// menu items can have a bitmap set on them via the setMenuItemBitmap(menu, menuItem, bitmapIndex)
//    passing -1 for the bitmap index will result in no bitmap being shown
//    the index paramater is an index into the bitmap array of the associated profile
//    this can be used, for example, to display a check next to a selected menu item
//    bitmap indices are actually multiplied by 3 when indexing into the bitmap
//    since bitmaps have normal, selected and disabled states.
//
// menus can be removed via the removeMenu console command
// specification arguments for menus and menu items can be either the id or the text of the menu or menu item
// adding the menu item "-" will add an un-selectable seperator to the menu
// callbacks:
// when a menu is clicked, before it is displayed, the menu calls its onMenuSelect(menuId, menuText) method -
//    this allows the callback to enable/disable menu items, or add menu items in a context-sensitive way
// when a menu item is clicked, the menu removes itself from display, then calls onMenuItemSelect(menuId, menuText, menuItemId, menuItemText)

// the initial implementation does not support:
//    hierarchal menus
//    keyboard accelerators on menu text (i.e. via alt-key combos)

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(GuiMenuBar);

//------------------------------------------------------------------------------
// console methods
//------------------------------------------------------------------------------

ConsoleMethod(GuiMenuBar, clearMenus, void, 2, 2, "() - clears all the menus from the menu bar.")
{
	object->clearMenus();
}

ConsoleMethod(GuiMenuBar, setMenuMargins, void, 5, 5, "(S32 horizontalMargin, S32 verticalMargin, S32 bitmapToTextSpacing) - Sets the menu rendering margins: horizontal, vertical, bitmap spacing.")
{
	object->mHorizontalMargin = dAtoi(argv[2]);
	object->mVerticalMargin = dAtoi(argv[3]);
	object->mBitmapMargin = dAtoi(argv[4]);
}

ConsoleMethod(GuiMenuBar, addMenu, void, 4, 4, "(string menuText, int menuId) - adds a new menu to the menu bar.")
{
	// <Edit>:[thinking]:????????????????????????????????
	//if(dIsdigit(argv[2][0]))
	//{
	//   Con::errorf("Cannot add menu %s (id = %s).  First character of a menu's text cannot be a digit.", argv[2], argv[3]);
	//   return;
	//}
	object->addMenu(argv[2], dAtoi(argv[3]));
}

ConsoleMethod(GuiMenuBar, addMenuItem, void, 5, 7, "(string menu, string menuItemText, int menuItemId, string accelerator = NULL, int checkGroup = -1) - adds a menu item to the specified menu.  The menu argument can be either the text of a menu or its id.")
{
	//if(dIsdigit(argv[3][0]))
	//{
	//   Con::errorf("Cannot add menu item %s (id = %s).  First character of a menu item's text cannot be a digit.", argv[3], argv[4]);
	//   return;
	//}
	GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
	if(!menu)
	{
		Con::errorf("Cannot find menu %s for addMenuItem.", argv[2]);
		return;
	}
	object->addMenuItem(menu, argv[3], dAtoi(argv[4]), argc == 5 ? "" : argv[5], argc < 7 ? -1 : dAtoi(argv[6]));
}

ConsoleMethod(GuiMenuBar, setMenuItemEnable, void, 5, 5, "(string menu, string menuItem, bool enabled) - sets the menu item to enabled or disabled based on the enable parameter.  The specified menu and menu item can either be text or ids.")
{
	GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
	if(!menu)
	{
		Con::errorf("Cannot find menu %s for setMenuItemEnable.", argv[2]);
		return;
	}
	GuiMenuBar::MenuItem *menuItem = object->findMenuItem(menu, argv[3]);
	if(!menuItem)
	{
		Con::errorf("Cannot find menu item %s for setMenuItemEnable.", argv[3]);
		return;
	}
	menuItem->enabled = dAtob(argv[4]);
}

ConsoleMethod(GuiMenuBar, setCheckmarkBitmapIndex, void, 3, 3, "(S32 bitmapindex) - sets the menu bitmap index for the check mark image.")
{
	object->mCheckmarkBitmapIndex = dAtoi(argv[2]);
}

ConsoleMethod(GuiMenuBar, setMenuItemChecked, void, 5, 5, "(string menu, string menuItem, bool checked) - sets the menu item bitmap to a check mark, which by default is the first element in the bitmap array (although this may be changed with setCheckmarkBitmapIndex()).  Any other menu items in the menu with the same check group become unchecked if they are checked.")
{
	GuiMenuBar::Menu *menu = object->findMenuID(argv[2]);
	if(!menu)
	{
		Con::errorf("Cannot find menu %s for setMenuItemChecked.", argv[2]);
		return;
	}
	GuiMenuBar::MenuItem *menuItem = object->findMenuItemID(menu, argv[3]);
	if(!menuItem)
	{
		Con::errorf("Cannot find menu item %s for setMenuItemChecked.", argv[3]);
		return;
	}
	bool checked = dAtob(argv[4]);
	if(checked && menuItem->checkGroup != -1)
	{
		// first, uncheck everything in the group:
		for(GuiMenuBar::MenuItem *itemWalk = menu->firstMenuItem; itemWalk; itemWalk = itemWalk->nextMenuItem)
			if(itemWalk->checkGroup == menuItem->checkGroup && itemWalk->bitmapIndex == object->mCheckmarkBitmapIndex)
				itemWalk->bitmapIndex = -1;
	}
	menuItem->bitmapIndex = checked ? object->mCheckmarkBitmapIndex : -1;
}

ConsoleMethod(GuiMenuBar, setMenuText, void, 4, 4, "(string menu, string newMenuText) - sets the text of the specified menu to the new string.")
{
	//if(dIsdigit(argv[3][0]))
	//{
	//   Con::errorf("Cannot name menu %s to %s.  First character of a menu's text cannot be a digit.", argv[2], argv[3]);
	//   return;
	//}
	GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
	if(!menu)
	{
		Con::errorf("Cannot find menu %s for setMenuText.", argv[2]);
		return;
	}
	dFree(menu->text);
	menu->text = dStrdup(argv[3]);
	object->menuBarDirty = true;
}
ConsoleMethod(GuiMenuBar, setMenuBitmapIndex, void, 6, 6, "(string menu, S32 bitmapindex, bool bitmaponly, bool drawborder) - sets the bitmap index for the menu and toggles rendering only the bitmap.")
{
	GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
	if(!menu)
	{
		Con::errorf("Cannot find menu %s for setMenuBitmapIndex.", argv[2]);
		return;
	}

	menu->bitmapIndex = dAtoi(argv[3]);
	menu->drawBitmapOnly = dAtob(argv[4]);
	menu->drawBorder = dAtob(argv[5]);

	object->menuBarDirty = true;
}

ConsoleMethod(GuiMenuBar, setMenuVisible, void, 4, 4, "(string menu, bool visible) - sets the whether or not to display the specified menu.")
{
	GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
	if(!menu)
	{
		Con::errorf("Cannot find menu %s for setMenuVisible.", argv[2]);
		return;
	}
	menu->visible = dAtob(argv[3]);
	object->menuBarDirty = true;
	object->setUpdate();
}

ConsoleMethod(GuiMenuBar, setMenuItemText, void, 5, 5, "(string menu, string menuItem, string newMenuItemText) - sets the text of the specified menu item to the new string.")
{
	//if(dIsdigit(argv[4][0]))
	//{
	//   Con::errorf("Cannot name menu item %s to %s.  First character of a menu item's text cannot be a digit.", argv[3], argv[4]);
	//   return;
	//}
	GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
	if(!menu)
	{
		Con::errorf("Cannot find menu %s for setMenuItemText.", argv[2]);
		return;
	}
	GuiMenuBar::MenuItem *menuItem = object->findMenuItem(menu, argv[3]);
	if(!menuItem)
	{
		Con::errorf("Cannot find menu item %s for setMenuItemText.", argv[3]);
		return;
	}
	dFree(menuItem->text);
	menuItem->text = dStrdup(argv[4]);
}

ConsoleMethod(GuiMenuBar, setMenuItemVisible, void, 5, 5, "(string menu, string menuItem, bool isVisible) - sets the specified menu item to be either visible or not.")
{
	GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
	if(!menu)
	{
		Con::errorf("Cannot find menu %s for setMenuItemVisible.", argv[2]);
		return;
	}
	GuiMenuBar::MenuItem *menuItem = object->findMenuItem(menu, argv[3]);
	if(!menuItem)
	{
		Con::errorf("Cannot find menu item %s for setMenuItemVisible.", argv[3]);
		return;
	}
	menuItem->visible = dAtob(argv[4]);
}

ConsoleMethod(GuiMenuBar, setMenuItemBitmap, void, 5, 5, "(string menu, string menuItem, int bitmapIndex) - sets the specified menu item bitmap index in the bitmap array.  Setting the item's index to -1 will remove any bitmap.")
{
	GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
	if(!menu)
	{
		Con::errorf("Cannot find menu %s for setMenuItemBitmap.", argv[2]);
		return;
	}
	GuiMenuBar::MenuItem *menuItem = object->findMenuItem(menu, argv[3]);
	if(!menuItem)
	{
		Con::errorf("Cannot find menu item %s for setMenuItemBitmap.", argv[3]);
		return;
	}
	menuItem->bitmapIndex = dAtoi(argv[4]);
}

ConsoleMethod(GuiMenuBar, removeMenuItem, void, 4, 4, "(string menu, string menuItem) - removes the specified menu item from the menu.")
{
	GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
	if(!menu)
	{
		Con::errorf("Cannot find menu %s for removeMenuItem.", argv[2]);
		return;
	}
	GuiMenuBar::MenuItem *menuItem = object->findMenuItem(menu, argv[3]);
	if(!menuItem)
	{
		Con::errorf("Cannot find menu item %s for removeMenuItem.", argv[3]);
		return;
	}
	object->removeMenuItem(menu, menuItem);
}

ConsoleMethod(GuiMenuBar, clearMenuItems, void, 3, 3, "(string menu) - removes all the menu items from the specified menu.")
{
	GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
	if(!menu)
	{
		//Con::errorf("Cannot find menu %s for clearMenuItems.", argv[2]);
		return;
	}
	object->clearMenuItems(menu);
}

ConsoleMethod(GuiMenuBar, removeMenu, void, 3, 3, "(string menu) - removes the specified menu from the menu bar.")
{
	GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
	if(!menu)
	{
		//Con::errorf("Cannot find menu %s for removeMenu.", argv[2]);
		return;
	}
	object->clearMenuItems(menu);
	object->menuBarDirty = true;
}

//------------------------------------------------------------------------------
//  Submenu console methods
//------------------------------------------------------------------------------

ConsoleMethod(GuiMenuBar, setMenuItemSubmenuState, void, 5, 5, "(string menu, string menuItem, bool isSubmenu) - Sets the given menu item to be a submenu")
{

	GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
	if(!menu)
	{
		Con::errorf("Cannot find menu %s for setMenuItemSubmenuState.", argv[2]);
		return;
	}

	GuiMenuBar::MenuItem *menuitem = object->findMenuItem(menu, argv[3]);
	if(!menuitem)
	{
		Con::errorf("Cannot find menuitem %s for setMenuItemSubmenuState.", argv[3]);
		return;
	}

	menuitem->isSubmenu = dAtob(argv[4]);
}

ConsoleMethod(GuiMenuBar, addSubmenuItem, void, 6, 8, "(string menu, string menuItem, string submenuItemText, int submenuItemId, string accelerator = NULL, int checkGroup = -1) - adds a menu item to the specified menu.  The menu argument can be either the text of a menu or its id.")
{
	//if(dIsdigit(argv[4][0]))
	//{
	//   Con::errorf("Cannot add submenu item %s (id = %s).  First character of a menu item's text cannot be a digit.", argv[4], argv[5]);
	//   return;
	//}

	GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
	if(!menu)
	{
		Con::errorf("Cannot find menu %s for addMenuItem.", argv[2]);
		return;
	}

	GuiMenuBar::MenuItem *menuitem = object->findMenuItem(menu, argv[3]);
	if(!menuitem)
	{
		Con::errorf("Cannot find menuitem %s for addSubmenuItem.", argv[3]);
		return;
	}

	object->addSubmenuItem(menu, menuitem, argv[4], dAtoi(argv[5]), argc == 6 ? "" : argv[6], argc < 8 ? -1 : dAtoi(argv[7]));
}

ConsoleMethod(GuiMenuBar, clearSubmenuItems, void, 4, 4, "(string menu, string menuItem) - removes all the menu items from the specified submenu.")
{
	GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
	if(!menu)
	{
		Con::errorf("Cannot find menu %s for clearSubmenuItems.", argv[2]);
		return;
	}

	GuiMenuBar::MenuItem *menuitem = object->findMenuItem(menu, argv[3]);
	if(!menuitem)
	{
		Con::errorf("Cannot find menuitem %s for clearSubmenuItems.", argv[3]);
		return;
	}

	object->clearSubmenuItems(menuitem);
}

ConsoleMethod(GuiMenuBar, setSubmenuItemChecked, void, 6, 6, "(string menu, string menuItem, string submenuItemText, bool checked) - sets the menu item bitmap to a check mark, which by default is the first element in the bitmap array (although this may be changed with setCheckmarkBitmapIndex()).  Any other menu items in the menu with the same check group become unchecked if they are checked.")
{
	// Find the parent menu
	GuiMenuBar::Menu *menu = object->findMenu(argv[2]);
	if(!menu)
	{
		Con::errorf("Cannot find menu %s for setSubmenuItemChecked.", argv[2]);
		return;
	}

	// Find the parent menu item
	GuiMenuBar::MenuItem *menuItem = object->findMenuItem(menu, argv[3]);
	if(!menuItem)
	{
		Con::errorf("Cannot find menu item %s for setSubmenuItemChecked.", argv[3]);
		return;
	}

	// Find the submenu item
	GuiMenuBar::MenuItem *submenuItem = object->findSubmenuItem(menu, argv[3], argv[4]);
	if(!submenuItem)
	{
		Con::errorf("Cannot find submenu item %s for setSubmenuItemChecked.", argv[4]);
		return;
	}

	bool checked = dAtob(argv[5]);
	if(checked && submenuItem->checkGroup != -1)
	{
		// first, uncheck everything in the group:
		for(GuiMenuBar::MenuItem *itemWalk = menuItem->firstSubmenuItem; itemWalk; itemWalk = itemWalk->nextMenuItem)
			if(itemWalk->checkGroup == submenuItem->checkGroup && itemWalk->bitmapIndex == object->mCheckmarkBitmapIndex)
				itemWalk->bitmapIndex = -1;
	}
	submenuItem->bitmapIndex = checked ? object->mCheckmarkBitmapIndex : -1;
}

//------------------------------------------------------------------------------
// menu management methods
//------------------------------------------------------------------------------

void GuiMenuBar::addMenu(const char *menuText, U32 menuId)
{
	// allocate the menu
	Menu *newMenu = new Menu;
	newMenu->text = dStrdup(menuText);
	newMenu->id = menuId;
	newMenu->nextMenu = NULL;
	newMenu->firstMenuItem = NULL;
	newMenu->visible = true;

	// Menu bitmap variables
	newMenu->bitmapIndex = -1;
	newMenu->drawBitmapOnly = false;
	newMenu->drawBorder = true;

	// add it to the menu list
	menuBarDirty = true;
	Menu **walk;
	for(walk = &menuList; *walk; walk = &(*walk)->nextMenu)
		;
	*walk = newMenu;
}

GuiMenuBar::Menu *GuiMenuBar::findMenu(const char *menu)
{
	// <Edit>:[thinking]:ID??????????????
	//if(dIsdigit(menu[0]))
	//{
	//	U32 id = dAtoi(menu);
	//	for(Menu *walk = menuList; walk; walk = walk->nextMenu)
	//		if(id == walk->id)
	//			return walk;
	//	return NULL;
	//}
	//else
	//{
	for(Menu *walk = menuList; walk; walk = walk->nextMenu)
		if(!dStricmp(menu, walk->text))
			return walk;
	return NULL;
	//}
}

GuiMenuBar::MenuItem *GuiMenuBar::findMenuItem(Menu *menu, const char *menuItem)
{
	// <Edit>:[3/10/2009 thinking]:ID??????????????
	//if(dIsdigit(menuItem[0]))
	//{
	//   U32 id = dAtoi(menuItem);
	//   for(MenuItem *walk = menu->firstMenuItem; walk; walk = walk->nextMenuItem)
	//      if(id == walk->id)
	//         return walk;
	//   return NULL;
	//}
	//else
	//{
	for(MenuItem *walk = menu->firstMenuItem; walk; walk = walk->nextMenuItem)
		if(!dStricmp(menuItem, walk->text))
			return walk;
	return NULL;
	// }
}
GuiMenuBar::Menu *GuiMenuBar::findMenuID(const char *menu)
{
	if(dIsdigit(menu[0]))
	{
		U32 id = dAtoi(menu);
		for(Menu *walk = menuList; walk; walk = walk->nextMenu)
			if(id == walk->id)
				return walk;
	}
	return NULL;
}
GuiMenuBar::MenuItem *GuiMenuBar::findMenuItemID(Menu *menu, const char *menuItem)
{
	if(dIsdigit(menuItem[0]))
	{
		U32 id = dAtoi(menuItem);
		for(MenuItem *walk = menu->firstMenuItem; walk; walk = walk->nextMenuItem)
			if(id == walk->id)
				return walk;
	}

	return NULL;
}
void GuiMenuBar::removeMenu(Menu *menu)
{
	menuBarDirty = true;
	clearMenuItems(menu);
	for(Menu **walk = &menuList; *walk; walk = &(*walk)->nextMenu)
	{
		if(*walk == menu)
		{
			*walk = menu->nextMenu;
			break;
		}
	}
	dFree(menu->text);
	delete menu;
}

void GuiMenuBar::removeMenuItem(Menu *menu, MenuItem *menuItem)
{
	for(MenuItem **walk = &menu->firstMenuItem; *walk; walk = &(*walk)->nextMenuItem)
	{
		if(*walk == menuItem)
		{
			*walk = menuItem->nextMenuItem;
			break;
		}
	}

	//  If this is a submenu, then be sure to clear the submenu's items
	if(menuItem->isSubmenu)
	{
		clearSubmenuItems(menuItem);
	}

	dFree(menuItem->text);
	dFree(menuItem->accelerator);
	delete menuItem;
}

void GuiMenuBar::addMenuItem(Menu *menu, const char *text, U32 id, const char *accelerator, S32 checkGroup)
{
	// allocate the new menu item
	MenuItem *newMenuItem = new MenuItem;
	newMenuItem->text = dStrdup(text);
	if(accelerator[0])
		newMenuItem->accelerator = dStrdup(accelerator);
	else
		newMenuItem->accelerator = NULL;
	newMenuItem->id = id;
	newMenuItem->checkGroup = checkGroup;
	newMenuItem->nextMenuItem = NULL;
	newMenuItem->acceleratorIndex = 0;
	newMenuItem->enabled = text[0] != '-';
	newMenuItem->visible = true;
	newMenuItem->bitmapIndex = -1;

	//  Default to not having a submenu
	newMenuItem->isSubmenu = false;
	newMenuItem->firstSubmenuItem = NULL;
	newMenuItem->submenuParentMenu = NULL;

	// link it into the menu's menu item list
	MenuItem **walk = &menu->firstMenuItem;
	while(*walk)
		walk = &(*walk)->nextMenuItem;
	*walk = newMenuItem;

}

void GuiMenuBar::clearMenuItems(Menu *menu)
{
	while(menu->firstMenuItem)
		removeMenuItem(menu, menu->firstMenuItem);
}

void GuiMenuBar::clearMenus()
{
	while(menuList)
		removeMenu(menuList);
}

//------------------------------------------------------------------------------
//  Submenu methods
//------------------------------------------------------------------------------

//  This method will return the MenuItem class of of a submenu's menu item given
// its parent menu and parent menuitem.  If the menuitem ID is used, then the submenu
// ID must also be used.
GuiMenuBar::MenuItem *GuiMenuBar::findSubmenuItem(Menu *menu, const char *menuItem, const char *submenuItem)
{
	if(dIsdigit(menuItem[0]))
	{
		//  Search by ID
		U32 id = dAtoi(menuItem);
		for(MenuItem *walk = menu->firstMenuItem; walk; walk = walk->nextMenuItem)
			if(id == walk->id)
		 {
			 if(walk->isSubmenu)
			 {
				 U32 subid = dAtoi(submenuItem);
				 for(MenuItem *subwalk = walk->firstSubmenuItem; subwalk; subwalk = subwalk->nextMenuItem)
				 {
					 if(subid == walk->id)
				  {
					  return subwalk;
				  }
				 }
			 }
			 return NULL;
		 }
			return NULL;
	}
	else
	{
		//  Search by name
		for(MenuItem *walk = menu->firstMenuItem; walk; walk = walk->nextMenuItem)
			if(!dStricmp(menuItem, walk->text))
		 {
			 if(walk->isSubmenu)
			 {
				 for(MenuItem *subwalk = walk->firstSubmenuItem; subwalk; subwalk = subwalk->nextMenuItem)
				 {
					 if(!dStricmp(submenuItem, subwalk->text))
						 return subwalk;
				 }
			 }
			 return NULL;
		 }
			return NULL;
	}
}

//  Add a menuitem to the given submenu
void GuiMenuBar::addSubmenuItem(Menu *menu, MenuItem *submenu, const char *text, U32 id, const char *accelerator, S32 checkGroup)
{
	// Check that the given menu item supports a submenu
	if(submenu && !submenu->isSubmenu)
	{
		Con::errorf("GuiMenuBar::addSubmenuItem: Attempting to add menuitem '%s' to an invalid submenu",text);
		return;
	}

	// allocate the new menu item
	MenuItem *newMenuItem = new MenuItem;
	newMenuItem->text = dStrdup(text);
	if(accelerator[0])
		newMenuItem->accelerator = dStrdup(accelerator);
	else
		newMenuItem->accelerator = NULL;
	newMenuItem->id = id;
	newMenuItem->checkGroup = checkGroup;
	newMenuItem->nextMenuItem = NULL;
	newMenuItem->acceleratorIndex = 0;
	newMenuItem->enabled = text[0] != '-';
	newMenuItem->visible = true;
	newMenuItem->bitmapIndex = -1;

	//  Default to not having a submenu
	newMenuItem->isSubmenu = false;
	newMenuItem->firstSubmenuItem = NULL;

	//  Point back to the submenu's menu
	newMenuItem->submenuParentMenu = menu;

	// link it into the menu's menu item list
	MenuItem **walk = &submenu->firstSubmenuItem;
	while(*walk)
		walk = &(*walk)->nextMenuItem;
	*walk = newMenuItem;

}

//  Remove a submenu item
void GuiMenuBar::removeSubmenuItem(MenuItem *menuItem, MenuItem *submenuItem)
{
	// Check that the given menu item supports a submenu
	if(menuItem && !menuItem->isSubmenu)
	{
		Con::errorf("GuiMenuBar::removeSubmenuItem: Attempting to remove submenuitem '%s' from an invalid submenu",submenuItem->text);
		return;
	}

	for(MenuItem **subwalk = &menuItem->firstSubmenuItem; *subwalk; subwalk = &(*subwalk)->nextMenuItem)
	{
		if(*subwalk == submenuItem)
		{
			*subwalk = submenuItem->nextMenuItem;
			break;
		}
	}
	dFree(submenuItem->text);
	dFree(submenuItem->accelerator);
	delete submenuItem;
}

//  Clear all menuitems from a submenu
void GuiMenuBar::clearSubmenuItems(MenuItem *menuitem)
{
	// Check that the given menu item supports a submenu
	if(menuitem && !menuitem->isSubmenu)
	{
		Con::errorf("GuiMenuBar::clearSubmenuItems: Attempting to clear an invalid submenu");
		return;
	}

	while(menuitem->firstSubmenuItem)
		removeSubmenuItem(menuitem, menuitem->firstSubmenuItem);
}

//------------------------------------------------------------------------------
// initialization, input and render methods
//------------------------------------------------------------------------------

GuiMenuBar::GuiMenuBar()
{
	menuList = NULL;
	menuBarDirty = true;
	mouseDownMenu = NULL;
	mouseOverMenu = NULL;
	mCurAcceleratorIndex = 0;
	mBackground = NULL;
	mPadding = 0;

	mCheckmarkBitmapIndex = 0; // Default to the first image in the bitmap array for the check mark

	mHorizontalMargin = 6; // Default number of pixels on the left and right side of a manu's text
	mVerticalMargin = 1;   // Default number of pixels on the top and bottom of a menu's text
	mBitmapMargin = 2;     // Default number of pixels between a menu's bitmap and text

	//  Added:
	mouseDownSubmenu = NULL;
	mouseOverSubmenu = NULL;
	mSubmenuBackground = NULL;
	mSubmenuTextList = NULL;
	mMouseOverCounter = 0;
	mCountMouseOver = false;
	mMouseHoverAmount = 30;
	setProcessTicks(false);
}

void GuiMenuBar::initPersistFields()
{
	Parent::initPersistFields();

	addField("Padding", TypeS32, Offset( mPadding, GuiMenuBar ) );
}

bool GuiMenuBar::onWake()
{
	if(!Parent::onWake())
		return false;
	mProfile->constructBitmapArray();  // if a bitmap was specified...
	maxBitmapSize.set(0,0);
	S32 numBitmaps = mProfile->mBitmapArrayRects.size();
	if(numBitmaps)
	{
		RectI *bitmapBounds = mProfile->mBitmapArrayRects.address();
		for(S32 i = 0; i < numBitmaps; i++)
		{
			if(bitmapBounds[i].extent.x > maxBitmapSize.x)
				maxBitmapSize.x = bitmapBounds[i].extent.x;
			if(bitmapBounds[i].extent.y > maxBitmapSize.y)
				maxBitmapSize.y = bitmapBounds[i].extent.y;
		}
	}
	return true;
}

GuiMenuBar::Menu *GuiMenuBar::findHitMenu(Point2I mousePoint)
{
	Point2I pos = globalToLocalCoord(mousePoint);

	for(Menu *walk = menuList; walk; walk = walk->nextMenu)
		if(walk->visible && walk->bounds.pointInRect(pos))
			return walk;
	return NULL;
}

void GuiMenuBar::onPreRender()
{
	Parent::onPreRender();
	if(menuBarDirty)
	{
		menuBarDirty = false;
		U32 curX = mPadding;
		for(Menu *walk = menuList; walk; walk = walk->nextMenu)
		{
			if(!walk->visible)
				continue;

			// Bounds depends on if there is a bitmap to be drawn or not
			if(walk->bitmapIndex == -1)
		 {
			 // Text only
			 walk->bounds.set(curX, 0, mProfile->mFont->getStrWidth(walk->text) + (mHorizontalMargin * 2), getHeight() - (mVerticalMargin * 2));

			} else
		 {
			 // Will the bitmap and text be draw?
			 if(!walk->drawBitmapOnly)
			 {
				 // Draw the bitmap and the text
				 RectI *bitmapBounds = mProfile->mBitmapArrayRects.address();
				 walk->bounds.set(curX, 0, bitmapBounds[walk->bitmapIndex].extent.x + mProfile->mFont->getStrWidth(walk->text) + (mHorizontalMargin * 2), getHeight() + (mVerticalMargin * 2));

			 } else
			 {
				 // Only the bitmap will be drawn
				 RectI *bitmapBounds = mProfile->mBitmapArrayRects.address();
				 walk->bounds.set(curX, 0, bitmapBounds[walk->bitmapIndex].extent.x + mBitmapMargin + (mHorizontalMargin * 2), getHeight() + (mVerticalMargin * 2));
			 }
		 }

			curX += walk->bounds.extent.x;
		}
		mouseOverMenu = NULL;
		mouseDownMenu = NULL;
	}
}

void GuiMenuBar::checkMenuMouseMove(const GuiEvent &event)
{
	Menu *hit = findHitMenu(event.mousePoint);
	if(hit && hit != mouseDownMenu)
	{
		// gotta close out the current menu...
		mTextList->setSelectedCell(Point2I(-1, -1));
		closeMenu();
		mouseOverMenu = mouseDownMenu = hit;
		setUpdate();
		onAction();
	}
}

void GuiMenuBar::onMouseMove(const GuiEvent &event)
{
	Menu *hit = findHitMenu(event.mousePoint);
	if(hit != mouseOverMenu)
	{
		//  If we need to, reset the mouse over menu counter and indicate
		// that we should track it.
		if(hit)
			mMouseOverCounter = 0;
		if(!mCountMouseOver)
		{
			//  We've never started the counter, so start it.
			if(hit)
				mCountMouseOver = true;
		}

		mouseOverMenu = hit;
		setUpdate();
	}
}

void GuiMenuBar::onMouseLeave(const GuiEvent &event)
{
	if(mouseOverMenu)
		setUpdate();
	mouseOverMenu = NULL;

	//  As we've left the control, don't track how long the mouse has been
	// within it.
	if(mCountMouseOver && mMouseOverCounter >= mMouseHoverAmount)
	{
		Con::executef( this, "onMouseInMenu", "0"); // Last parameter indicates if we've entered or left the menu
	}
	mCountMouseOver = false;
	mMouseOverCounter = 0;
}

void GuiMenuBar::onMouseDragged(const GuiEvent &event)
{
	Menu *hit = findHitMenu(event.mousePoint);

	if(hit != mouseOverMenu)
	{
		//  If we need to, reset the mouse over menu counter and indicate
		// that we should track it.
		if(hit)
			mMouseOverCounter = 0;
		if(!mCountMouseOver)
		{
			//  We've never started the counter, so start it.
			if(hit)
				mCountMouseOver = true;
		}

		mouseOverMenu = hit;
		mouseDownMenu = hit;
		setUpdate();
		onAction();
	}
}

void GuiMenuBar::onMouseDown(const GuiEvent &event)
{
	mouseDownMenu = mouseOverMenu = findHitMenu(event.mousePoint);
	setUpdate();
	onAction();
}

void GuiMenuBar::onMouseUp(const GuiEvent &event)
{
	mouseDownMenu = NULL;
	setUpdate();
}

void GuiMenuBar::onRender(Point2I offset, const RectI &updateRect)
{
	m_DirtyFlag = false;

	RectI ctrlRect(offset, getExtent());

	//if opaque, fill the update rect with the fill color
	if (mProfile->mOpaque)
		GFX->getDrawUtil()->drawRectFill(RectI(offset, getExtent()), mProfile->mFillColor);

	//if there's a border, draw the border
	if (mProfile->mBorder)
		renderBorder(ctrlRect, mProfile);

	for(Menu *walk = menuList; walk; walk = walk->nextMenu)
	{
		if(!walk->visible)
			continue;
		ColorI fontColor = mProfile->mFontColor;
		RectI bounds = walk->bounds;
		bounds.point += offset;

		Point2I start;

		start.x = walk->bounds.point.x + mHorizontalMargin;
		start.y = walk->bounds.point.y + ( walk->bounds.extent.y - mProfile->mFont->getHeight() ) / 2;

		// Draw the border
		if(walk->drawBorder)
		{
			RectI highlightBounds = bounds;
			highlightBounds.inset(1,1);
			if(walk == mouseDownMenu)
				renderFilledBorder(highlightBounds, mProfile->mBorderColorHL, mProfile->mFillColorHL );
			else if(walk == mouseOverMenu && mouseDownMenu == NULL)
				renderFilledBorder(highlightBounds, mProfile->mBorderColor, mProfile->mFillColor );
		}

		// Do we draw a bitmap?
		if(walk->bitmapIndex != -1)
		{
			S32 index = walk->bitmapIndex * 3;
			if(walk == mouseDownMenu)
				++index;
			else if(walk == mouseOverMenu && mouseDownMenu == NULL)
				index += 2;

			RectI rect = mProfile->mBitmapArrayRects[index];

			Point2I bitmapstart(start);
			bitmapstart.y = walk->bounds.point.y + ( walk->bounds.extent.y - rect.extent.y ) / 2;

			GFX->getDrawUtil()->clearBitmapModulation();
			GFX->getDrawUtil()->drawBitmapSR( mProfile->mTextureObject, offset + bitmapstart, rect);

			// Should we also draw the text?
			if(!walk->drawBitmapOnly)
			{
				start.x += mBitmapMargin;
				GFX->getDrawUtil()->setBitmapModulation( fontColor );
				GFX->getDrawUtil()->drawText( mProfile->mFont, start + offset, walk->text, mProfile->mFontColors );
			}
		} else
		{
			GFX->getDrawUtil()->setBitmapModulation( fontColor );
			GFX->getDrawUtil()->drawText( mProfile->mFont, start + offset, walk->text, mProfile->mFontColors );
		}
	}

	renderChildControls( offset, updateRect );
}

void GuiMenuBar::buildAcceleratorMap()
{
	Parent::buildAcceleratorMap();
	// ok, accelerator map is cleared...
	// add all our keys:
	mCurAcceleratorIndex = 1;

	for(Menu *menu = menuList; menu; menu = menu->nextMenu)
	{
		for(MenuItem *item = menu->firstMenuItem; item; item = item->nextMenuItem)
		{
			if(!item->accelerator)
			{
				item->accelerator = 0;
				continue;
			}
			EventDescriptor accelEvent;
			ActionMap::createEventDescriptor(item->accelerator, &accelEvent);

			//now we have a modifier, and a key, add them to the canvas
			GuiCanvas *root = getRoot();
			if (root)
				root->addAcceleratorKey(this, mCurAcceleratorIndex, accelEvent.eventCode, accelEvent.flags);
			item->acceleratorIndex = mCurAcceleratorIndex;
			mCurAcceleratorIndex++;
		}
	}
}

void GuiMenuBar::acceleratorKeyPress(U32 index)
{
	// loop through all the menus
	// and find the item that corresponds to the accelerator index
	for(Menu *menu = menuList; menu; menu = menu->nextMenu)
	{
		if(!menu->visible)
			continue;

		for(MenuItem *item = menu->firstMenuItem; item; item = item->nextMenuItem)
		{
			if(item->acceleratorIndex == index)
			{
				// first, call the script callback for menu selection:
				Con::executef( this, "onMenuSelect", Con::getIntArg(menu->id), 
					menu->text);
				if(item->visible)
					menuItemSelected(menu, item);
				return;
			}
		}
	}
}

//------------------------------------------------------------------------------
// Menu display class methods
//------------------------------------------------------------------------------

GuiMenuBackgroundCtrl::GuiMenuBackgroundCtrl(GuiMenuBar *ctrl, GuiMenuTextListCtrl *textList)
{
	mMenuBarCtrl = ctrl;
	mTextList = textList;
}

void GuiMenuBackgroundCtrl::onMouseDown(const GuiEvent &event)
{
	mTextList->setSelectedCell(Point2I(-1,-1));
	mMenuBarCtrl->closeMenu();
}

void GuiMenuBackgroundCtrl::onMouseMove(const GuiEvent &event)
{
	GuiCanvas *root = getRoot();
	GuiControl *ctrlHit = root->findHitControl(event.mousePoint, mLayer - 1);
	if(ctrlHit == mMenuBarCtrl)  // see if the current mouse over menu is right...
		mMenuBarCtrl->checkMenuMouseMove(event);
}

void GuiMenuBackgroundCtrl::onMouseDragged(const GuiEvent &event)
{
	GuiCanvas *root = getRoot();
	GuiControl *ctrlHit = root->findHitControl(event.mousePoint, mLayer - 1);
	if(ctrlHit == mMenuBarCtrl)  // see if the current mouse over menu is right...
		mMenuBarCtrl->checkMenuMouseMove(event);
}

GuiMenuTextListCtrl::GuiMenuTextListCtrl(GuiMenuBar *ctrl)
{
	mMenuBarCtrl = ctrl;
	isSubMenu = false; //  Added
}

void GuiMenuTextListCtrl::onRenderCell(Point2I offset, Point2I cell, bool selected, bool mouseOver)
{
	if(dStrcmp(mList[cell.y].text + 3, "-\t")) //  Was: dStrcmp(mList[cell.y].text + 2, "-\t")) but has been changed to take into account the submenu flag
		Parent::onRenderCell(offset, cell, selected, mouseOver);
	else
	{
		S32 yp = offset.y + mCellSize.y / 2;
		GFX->getDrawUtil()->drawLine(offset.x, yp, offset.x + mCellSize.x, yp, ColorI(128,128,128));
		GFX->getDrawUtil()->drawLine(offset.x, yp+1, offset.x + mCellSize.x, yp+1, ColorI(255,255,255));
	}
	// now see if there's a bitmap...
	U8 idx = mList[cell.y].text[0];
	if(idx != 1)
	{
		// there's a bitmap...
		U32 index = U32(idx - 2) * 3;
		if(!mList[cell.y].active)
			index += 2;
		else if(selected || mouseOver)
			index ++;

		RectI rect = mProfile->mBitmapArrayRects[index];
		Point2I off = mMenuBarCtrl->maxBitmapSize - rect.extent;
		off /= 2;

		GFX->getDrawUtil()->clearBitmapModulation();
		GFX->getDrawUtil()->drawBitmapSR(mProfile->mTextureObject, offset + off, rect);
	} 

	//  Check if this is a submenu
	idx = mList[cell.y].text[1];
	if(idx != 1)
	{
		// This is a submenu, so draw an arrow
		S32 left = offset.x + mCellSize.x - 12;
		S32 right = left + 8;
		S32 top = mCellSize.y / 2 + offset.y - 4;
		S32 bottom = top + 8;
		S32 middle = top + 4;

		PrimBuild::begin( GFXTriangleList, 3 );
		if( selected || mouseOver )
			PrimBuild::color( mProfile->mFontColorHL );
		else
			PrimBuild::color( mProfile->mFontColor );

		PrimBuild::vertex2i( left,  top );
		PrimBuild::vertex2i( right, middle );
		PrimBuild::vertex2i( left, bottom );
		PrimBuild::end();
	}
}

bool GuiMenuTextListCtrl::onKeyDown(const GuiEvent &event)
{
	//if the control is a dead end, don't process the input:
	if ( !mVisible || !mActive || !mAwake )
		return false;

	//see if the key down is a <return> or not
	if ( event.modifier == 0 )
	{
		if ( event.keyCode == KEY_RETURN )
		{
			mMenuBarCtrl->closeMenu();
			return true;
		}
		else if ( event.keyCode == KEY_ESCAPE )
		{
			mSelectedCell.set( -1, -1 );
			mMenuBarCtrl->closeMenu();
			return true;
		}
	}

	//otherwise, pass the event to it's parent
	return Parent::onKeyDown(event);
}

void GuiMenuTextListCtrl::onMouseDown(const GuiEvent &event)
{
	Parent::onMouseDown(event);
	mMenuBarCtrl->closeMenu();
}

void GuiMenuTextListCtrl::onMouseUp(const GuiEvent &event)
{
	// ok, this is kind of strange... but!
	// here's the deal: if we get a mouse up in this control
	// it means the mouse was dragged from the initial menu mouse click
	// so: activate the menu result as though this event were,
	// instead, a mouse down.

	onMouseDown(event);
}

void GuiMenuTextListCtrl::onCellHighlighted(Point2I cell)
{
	// If this text list control is part of a submenu, then don't worry about
	// passing this along
	if(!isSubMenu)
	{
		RectI globalbounds(getBounds());
		Point2I globalpoint = localToGlobalCoord(globalbounds.point);
		globalbounds.point = globalpoint;
		mMenuBarCtrl->highlightedMenuItem(cell.y, globalbounds, mCellSize);
	}
}

//------------------------------------------------------------------------------
// Submenu display class methods
//------------------------------------------------------------------------------

GuiSubmenuBackgroundCtrl::GuiSubmenuBackgroundCtrl(GuiMenuBar *ctrl, GuiMenuTextListCtrl *textList) : GuiMenuBackgroundCtrl(ctrl, textList)
{
}

void GuiSubmenuBackgroundCtrl::onMouseDown(const GuiEvent &event)
{
	mTextList->setSelectedCell(Point2I(-1,-1));
	mMenuBarCtrl->closeMenu();
}

bool GuiSubmenuBackgroundCtrl::pointInControl(const Point2I& parentCoordPoint)
{
	S32 xt = parentCoordPoint.x - getLeft();
	S32 yt = parentCoordPoint.y - getTop();

	if(findHitControl(Point2I(xt,yt)) == this)
		return false;
	else
		return true;
	//   return xt >= 0 && yt >= 0 && xt < getWidth() && yt < getHeight();
}

//------------------------------------------------------------------------------

void GuiMenuBar::menuItemSelected(GuiMenuBar::Menu *menu, GuiMenuBar::MenuItem *item)
{
	if(item->enabled)
		Con::executef( this, "onMenuItemSelect", Con::getIntArg(menu->id), 
		menu->text, Con::getIntArg(item->id), item->text);
}

void GuiMenuBar::onSleep()
{
	if(mBackground) // a menu is up?
	{
		mTextList->setSelectedCell(Point2I(-1, -1));
		closeMenu();
	}
	Parent::onSleep();
}

void GuiMenuBar::closeMenu()
{
	//  First close any open submenu
	closeSubmenu();

	// Get the selection from the text list:
	S32 selectionIndex = mTextList->getSelectedCell().y;

	// Pop the background:
	if( getRoot() )
		getRoot()->popDialogControl(mBackground);
	else
		return;

	// Kill the popup:
	//mBackground->deleteObject();
	//mBackground = NULL;

	//[2009/12/31][thinking]:??????????????????????
	//??????onMouseDown??????????????????????????????????????????
	mBackground->setVisible(false);
	

	// Now perform the popup action:
	if ( selectionIndex != -1 )
	{
		MenuItem *list = mouseDownMenu->firstMenuItem;

		while(selectionIndex && list)
		{
			list = list->nextMenuItem;
			selectionIndex--;
		}
		if(list)
			menuItemSelected(mouseDownMenu, list);
	}
	mouseDownMenu = NULL;
}

//  Called when a menu item is highlighted by the mouse
void GuiMenuBar::highlightedMenuItem(S32 selectionIndex, RectI bounds, Point2I cellSize)
{
	S32 selstore = selectionIndex;

	// Now perform the popup action:
	if ( selectionIndex != -1 )
	{
		MenuItem *list = mouseDownMenu->firstMenuItem;

		while(selectionIndex && list)
		{
			list = list->nextMenuItem;
			selectionIndex--;
		}

		if(list)
		{
			// If the highlighted item has changed...
			if(mouseOverSubmenu != list)
		 {
			 closeSubmenu();
			 mouseOverSubmenu = NULL;

			 // Check if this is a submenu.  If so, open the submenu.
			 if(list->isSubmenu)
			 {
				 // If there are submenu items, then open the submenu
				 if(list->firstSubmenuItem)
				 {
					 mouseOverSubmenu = list;
					 onSubmenuAction(selstore, bounds, cellSize);
				 }
			 }
		 }
		}
	}
}

//------------------------------------------------------------------------------
void GuiMenuBar::onAction()
{
	if(mBackground)
	{
		mBackground->deleteObject();
		mBackground = NULL;
	}

	if(!mouseDownMenu)
		return;

	// first, call the script callback for menu selection:
	Con::executef( this, "onMenuSelect", Con::getIntArg(mouseDownMenu->id), 
		mouseDownMenu->text);

	MenuItem *visWalk = mouseDownMenu->firstMenuItem;
	while(visWalk)
	{
		if(visWalk->visible)
			break;
		visWalk = visWalk->nextMenuItem;
	}
	if(!visWalk)
	{
		mouseDownMenu = NULL;
		return;
	}

	mTextList = new GuiMenuTextListCtrl(this);
	mTextList->setControlProfile(mProfile);

	mBackground = new GuiMenuBackgroundCtrl(this, mTextList);

	GuiCanvas *root = getRoot();
	Point2I windowExt = root->getExtent();

	mBackground->resize( Point2I(0,0), root->getExtent());
	S32 textWidth = 0, width = 0;
	S32 acceleratorWidth = 0;

	CommonFontEX *font = mProfile->mFont;

	for(MenuItem *walk = mouseDownMenu->firstMenuItem; walk; walk = walk->nextMenuItem)
	{
		if(!walk->visible)
			continue;

		S32 iTextWidth = font->getStrWidth(walk->text);
		S32 iAcceleratorWidth = walk->accelerator ? font->getStrWidth(walk->accelerator) : 0;

		if(iTextWidth > textWidth)
			textWidth = iTextWidth;
		if(iAcceleratorWidth > acceleratorWidth)
			acceleratorWidth = iAcceleratorWidth;
	}
	width = textWidth + acceleratorWidth + maxBitmapSize.x * 2 + 2 + 4;

	mTextList->setCellSize(Point2I(width, font->getHeight()+2));
	mTextList->clearColumnOffsets();
	mTextList->addColumnOffset(-1); // add an empty column in for the bitmap index.
	mTextList->addColumnOffset(maxBitmapSize.x + 1);
	mTextList->addColumnOffset(maxBitmapSize.x + 1 + textWidth + 4);

	U32 entryCount = 0;

	for(MenuItem *walk = mouseDownMenu->firstMenuItem; walk; walk = walk->nextMenuItem)
	{
		if(!walk->visible)
			continue;

		char buf[512];

		//  If this menu item is a submenu, then set the isSubmenu to 2 to indicate
		// an arrow should be drawn.  Otherwise set the isSubmenu normally.
		char isSubmenu = 1;
		if(walk->isSubmenu)
			isSubmenu = 2;

		char bitmapIndex = 1;
		if(walk->bitmapIndex >= 0 && (walk->bitmapIndex * 3 <= mProfile->mBitmapArrayRects.size()))
			bitmapIndex = walk->bitmapIndex + 2;
		dSprintf(buf, sizeof(buf), "%c%c\t%s\t%s", bitmapIndex, isSubmenu, walk->text, walk->accelerator ? walk->accelerator : "");
		mTextList->addEntry(entryCount, buf);

		if(!walk->enabled)
			mTextList->setEntryActive(entryCount, false);

		entryCount++;
	}
	Point2I menuPoint = localToGlobalCoord(mouseDownMenu->bounds.point);
	menuPoint.y += mouseDownMenu->bounds.extent.y; //  Used to have this at the end: + 2;

	GuiControl *ctrl = new GuiControl;
	RectI ctrlBounds(  menuPoint, mTextList->getExtent() + Point2I(6, 6));

	ctrl->setControlProfile(mProfile);
	mTextList->setPosition( mTextList->getPosition() + Point2I(3,3) );

	//  Make sure the menu doesn't go beyond the Canvas' bottom edge.
	if((ctrlBounds.point.y + ctrlBounds.extent.y) > windowExt.y)
	{
		//  Pop the menu above the menu bar
		Point2I menuBar = localToGlobalCoord(mouseDownMenu->bounds.point);
		ctrlBounds.point.y = menuBar.y - ctrl->getHeight();
	}

	ctrl->resize(ctrlBounds.point, ctrlBounds.extent);
	//mTextList->setPosition(Point2I(3,3));

	mTextList->registerObject();
	mBackground->registerObject();
	ctrl->registerObject();

	mBackground->addObject( ctrl );
	ctrl->addObject( mTextList );

	root->pushDialogControl(mBackground, mLayer + 1);
	mTextList->setFirstResponder();
}

//------------------------------------------------------------------------------
//  Performs an action when a menu item that is a submenu is selected/highlighted
void GuiMenuBar::onSubmenuAction(S32 selectionIndex, RectI bounds, Point2I cellSize)
{
	if(!mouseOverSubmenu)
		return;

	// first, call the script callback for menu selection:
	Con::executef( this, "onSubmenuSelect", Con::getIntArg(mouseOverSubmenu->id), 
		mouseOverSubmenu->text);

	MenuItem *visWalk = mouseOverSubmenu->firstSubmenuItem;
	while(visWalk)
	{
		if(visWalk->visible)
			break;
		visWalk = visWalk->nextMenuItem;
	}
	if(!visWalk)
	{
		mouseOverSubmenu = NULL;
		return;
	}

	mSubmenuTextList = new GuiMenuTextListCtrl(this);
	mSubmenuTextList->setControlProfile(mProfile);
	mSubmenuTextList->isSubMenu = true; // Indicate that this text list is part of a submenu

	mSubmenuBackground = new GuiSubmenuBackgroundCtrl(this, mSubmenuTextList);

	GuiCanvas *root = getRoot();
	Point2I windowExt = root->getExtent();

	mSubmenuBackground->resize( Point2I(0,0), root->getExtent());
	S32 textWidth = 0, width = 0;
	S32 acceleratorWidth = 0;

	CommonFontEX *font = mProfile->mFont;

	for(MenuItem *walk = mouseOverSubmenu->firstSubmenuItem; walk; walk = walk->nextMenuItem)
	{
		if(!walk->visible)
			continue;

		S32 iTextWidth = font->getStrWidth(walk->text);
		S32 iAcceleratorWidth = walk->accelerator ? font->getStrWidth(walk->accelerator) : 0;

		if(iTextWidth > textWidth)
			textWidth = iTextWidth;
		if(iAcceleratorWidth > acceleratorWidth)
			acceleratorWidth = iAcceleratorWidth;
	}
	width = textWidth + acceleratorWidth + maxBitmapSize.x * 2 + 2 + 4;

	mSubmenuTextList->setCellSize(Point2I(width, font->getHeight()+3));
	mSubmenuTextList->clearColumnOffsets();
	mSubmenuTextList->addColumnOffset(-1); // add an empty column in for the bitmap index.
	mSubmenuTextList->addColumnOffset(maxBitmapSize.x + 1);
	mSubmenuTextList->addColumnOffset(maxBitmapSize.x + 1 + textWidth + 4);

	U32 entryCount = 0;

	for(MenuItem *walk = mouseOverSubmenu->firstSubmenuItem; walk; walk = walk->nextMenuItem)
	{
		if(!walk->visible)
			continue;

		char buf[512];

		//  Can't have submenus within submenus.
		char isSubmenu = 1;

		char bitmapIndex = 1;
		if(walk->bitmapIndex >= 0 && (walk->bitmapIndex * 3 <= mProfile->mBitmapArrayRects.size()))
			bitmapIndex = walk->bitmapIndex + 2;
		dSprintf(buf, sizeof(buf), "%c%c\t%s\t%s", bitmapIndex, isSubmenu, walk->text, walk->accelerator ? walk->accelerator : "");
		mSubmenuTextList->addEntry(entryCount, buf);

		if(!walk->enabled)
			mSubmenuTextList->setEntryActive(entryCount, false);

		entryCount++;
	}
	Point2I menuPoint = bounds.point; //localToGlobalCoord(bounds.point);
	menuPoint.x += bounds.extent.x;
	menuPoint.y += cellSize.y * selectionIndex - 6;

	GuiControl *ctrl = new GuiControl;
	RectI ctrlBounds(menuPoint, mSubmenuTextList->getExtent() + Point2I(6, 6));
	ctrl->setControlProfile(mProfile);
	mSubmenuTextList->setPosition( getPosition() + Point2I(3,3));

	//  Make sure the menu doesn't go beyond the Canvas' bottom edge.
	if((ctrlBounds.point.y + ctrlBounds.extent.y ) > windowExt.y)
	{
		//  Pop the menu above the menu bar
		ctrlBounds.point.y -= mSubmenuTextList->getHeight() - cellSize.y - 6 - 3;
	}

	//  And the same for the right edge
	if((ctrlBounds.point.x + ctrlBounds.extent.x) > windowExt.x)
	{
		//  Pop the submenu to the left of the menu
		ctrlBounds.point.x -= mSubmenuTextList->getWidth() + cellSize.x + 6;
	}
	ctrl->resize(ctrlBounds.point, ctrlBounds.extent);

	//mSubmenuTextList->setPosition(Point2I(3,3));

	mSubmenuTextList->registerObject();
	mSubmenuBackground->registerObject();
	ctrl->registerObject();

	mSubmenuBackground->addObject( ctrl );
	ctrl->addObject( mSubmenuTextList );

	root->pushDialogControl(mSubmenuBackground, mLayer + 1);
	mSubmenuTextList->setFirstResponder();
}

//  Close down the submenu controls
void GuiMenuBar::closeSubmenu()
{
	if(!mSubmenuBackground || !mSubmenuTextList)
		return;

	// Get the selection from the text list:
	S32 selectionIndex = mSubmenuTextList->getSelectedCell().y;

	// Pop the background:
	if( getRoot() )
		getRoot()->popDialogControl(mSubmenuBackground);

	// Kill the popup:
	mSubmenuBackground->deleteObject();
	mSubmenuBackground = NULL;
	mSubmenuTextList = NULL;

	// Now perform the popup action:
	if ( selectionIndex != -1 )
	{
		MenuItem *list = NULL;
		if(mouseOverSubmenu)
		{
			list = mouseOverSubmenu->firstSubmenuItem;

			while(selectionIndex && list)
			{
				list = list->nextMenuItem;
				selectionIndex--;
			}
		}
		if(list)
			menuItemSelected(list->submenuParentMenu, list);
	}
	mouseOverSubmenu = NULL;
}

//  Find if the mouse pointer is within a menu item
GuiMenuBar::MenuItem *GuiMenuBar::findHitMenuItem(Point2I mousePoint)
{
	Point2I pos = globalToLocalCoord(mousePoint);

	//   for(Menu *walk = menuList; walk; walk = walk->nextMenu)
	//      if(walk->visible && walk->bounds.pointInRect(pos))
	//         return walk;
	return NULL;
}

//  Checks if the mouse has been moved to a new menu item
void GuiMenuBar::checkSubmenuMouseMove(const GuiEvent &event)
{
	MenuItem *hit = findHitMenuItem(event.mousePoint);
	if(hit && hit != mouseOverSubmenu)
	{
		// gotta close out the current menu...
		mSubmenuTextList->setSelectedCell(Point2I(-1, -1));
		//      closeSubmenu();
		setUpdate();
	}
}

//  Process a tick
void GuiMenuBar::processTick()
{
	// If we are to track a tick, then do so.
	if(mCountMouseOver)
	{
		//  If we're at a particular number of ticks, notify the script function
		if(mMouseOverCounter < mMouseHoverAmount)
		{
			++mMouseOverCounter;

		} else if(mMouseOverCounter == mMouseHoverAmount)
		{
			++mMouseOverCounter;
			Con::executef( this, "onMouseInMenu", "1"); // Last parameter indicates if we've entered or left the menu
		}
	}
}
