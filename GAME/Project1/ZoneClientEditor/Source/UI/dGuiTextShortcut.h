#ifndef GUI_TEXT_SHORTCUT_H
#define GUI_TEXT_SHORTCUT_H

#include "guiRichButtonCtrl.h"
#include "Gameplay\item\ItemShortcut.h"

class GuiTextShortcut : public GuiRichButtonCtrl
{
public:
	typedef GuiRichButtonCtrl Parent;

	GuiTextShortcut();
	virtual ~GuiTextShortcut();

	static void initPersistFields();
	
	void onMouseDown(const GuiEvent &event );
	
	void setItemInfo( ItemShortcut* pItem );

	void generateDescText();

	DECLARE_CONOBJECT( GuiTextShortcut );

	enum TextShortcutType
	{
		TST_NONE,
		TST_ITEM,
		TST_SKILL,
		TST_PET,
		TST_PLAYERINFO,
		TST_PATH,

		TST_COUNT,
	} ;

protected:

	// 用于显示物品的解析文本
	StringTableEntry mText;	

	// 用于显示物品比较的解析文本
	StringTableEntry mCompareText;

	// 物品信息
	ItemShortcut* mItemShortcut;
	std::string mCompareItemName;
	std::string mCompareItemIcon;

	int mType;
};

#endif