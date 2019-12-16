//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "console/simBase.h"
#include "core/tVector.h"
#include "util/messaging/dispatcher.h"
#include "gui/core/guiCanvas.h"

#ifndef _POPUPMENU_H_
#define _POPUPMENU_H_

// Forward ref used by the platform code
struct PlatformPopupMenuData;
class MenuBar;

// PopupMenu represents a menu.
// You can add menu items to the menu, but there is no PowerEngine object associated
// with these menu items, they exist only in a  platform specific manner.
class PopupMenu : public SimObject, public virtual Dispatcher::IMessageListener
{
   typedef SimObject Parent;

   friend class MenuBar;

private:
   /// Used by MenuBar to attach the menu to the menu bar. Do not use anywhere else.
   void attachToMenuBar(GuiCanvas *owner, S32 pos);

protected:
   PlatformPopupMenuData *mData;
   
   SimSet *mSubmenus;
   SimObjectPtr<GuiCanvas> mCanvas;

   StringTableEntry mBarTitle;
   
   bool mIsPopup;

public:
   PopupMenu();
   virtual ~PopupMenu();
   void createPlatformPopupMenuData();
   void deletePlatformPopupMenuData();
   
   DECLARE_CONOBJECT(PopupMenu);

   static void initPersistFields();

   virtual bool onAdd();
   virtual void onRemove();
   
   /// Creates the platform specific menu object, a peer to this object.
   /// The platform menu *must* exist before calling any method that manipulates
   /// menu items or displays the menu.
   /// implementd on a per-platform basis.
   void createPlatformMenu();

   void setBarTitle(const char * val) { mBarTitle = StringTable->insert(val, true); }	
   StringTableEntry getBarTitle() const { return mBarTitle; }
   
   /// pass NULL for @p title to insert a separator
   /// returns the menu item's ID, or -1 on failure.
   /// implementd on a per-platform basis.
   /// TODO: factor out common code
   S32 insertItem(S32 pos, const char *title, const char* accelerator);

   /// pass NULL for @p title to insert a separator
   /// returns the menu item's ID, or -1 on failure.
   /// adds the submenu to the mSubmenus vector.
   /// implemented on a per-platform basis.
   /// TODO: factor out common code
   S32 insertSubMenu(S32 pos, const char *title, PopupMenu *submenu);
   
   /// remove the menu item at @p itemPos
   /// if the item has a submenu, it is removed from the mSubmenus list.
   /// implemented on a per-platform basis.
   /// TODO: factor out common code
   void removeItem(S32 itemPos);

   /// implemented on a per-platform basis.
   void enableItem(S32 pos, bool enable);
   /// implemented on a per-platform basis.
   void checkItem(S32 pos, bool checked);

   /// All items at positions firstPos through lastPos are unchecked, and the
   /// item at checkPos is checked.
   /// implemented on a per-platform basis.
   void checkRadioItem(S32 firstPos, S32 lastPos, S32 checkPos);
   bool isItemChecked(S32 pos);

   //-----------------------------------------------------------------------------
   // New code should not use these methods directly, use the menu bar instead.
   //
   // They remain for compatibility with old code and will be changing/going away
   // once the existing code is moved over to the menu bar.
   //-----------------------------------------------------------------------------

   /// Places this menu in the menu bar of the application's main window.
   /// @param owner The GuiCanvas that owns the PlatformWindow that this call is associated with
   /// @param pos The relative position at which to place the menu.
   /// @param title The name of the menu
   void attachToMenuBar(GuiCanvas *owner, S32 pos, const char *title);
   
   /// Removes this menu from the menu bar.
   void removeFromMenuBar();

   //-----------------------------------------------------------------------------

   /// Called when the menu has been attached to the menu bar
   void onAttachToMenuBar(GuiCanvas *canvas, S32 pos, const char *title);
   
   /// Called when the menu has been removed from the menu bar
   void onRemoveFromMenuBar(GuiCanvas *canvas);

   /// Returns the position index of this menu on the bar.
   S32 getPosOnMenuBar();

   /// Returns true if this menu is attached to the menu bar
   bool isAttachedToMenuBar()       { return mCanvas != NULL; }

   /// Displays this menu as a popup menu and blocks until the user has selected
   /// an item.
   /// @param canvas the owner to show this popup associated with
   /// @param x window local x coordinate at which to display the popup menu
   /// @param y window local y coordinate at which to display the popup menu
   /// implemented on a per-platform basis.
   void showPopup(GuiCanvas *owner, S32 x = -1, S32 y = -1);

   /// Returns true iff this menu contains an item that matches @p iD.
   /// implemented on a per-platform basis.
   /// TODO: factor out common code
   bool canHandleID(U32 iD);

   /// A menu item in this menu has been selected by id.
   /// Submenus are given a chance to respond to the command first.
   /// If no submenu can handle the command id, this menu handles it.
   /// The script callback this::onSelectItem( position, text) is called.
   /// If @p text is null, then the text arg passed to script is the text of
   /// the selected menu item.
   /// implemented on a per-platform basis.
   /// TODO: factor out common code
   bool handleSelect(U32 command, const char *text = NULL);

   void onMenuSelect();

   virtual bool onMessageReceived(StringTableEntry queue, const char* event, const char* data );
   virtual bool onMessageObjectReceived(StringTableEntry queue, Message *msg );
};

#endif // _POPUPMENU_H_
