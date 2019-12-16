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

	// ������ʾ��Ʒ�Ľ����ı�
	StringTableEntry mText;	

	// ������ʾ��Ʒ�ȽϵĽ����ı�
	StringTableEntry mCompareText;

	// ��Ʒ��Ϣ
	ItemShortcut* mItemShortcut;
	std::string mCompareItemName;
	std::string mCompareItemIcon;

	int mType;
};

#endif