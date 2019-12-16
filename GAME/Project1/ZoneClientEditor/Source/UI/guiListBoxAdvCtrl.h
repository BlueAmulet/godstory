#ifndef GUI_LIST_BOX_ADV_CTRL_H
#define GUI_LIST_BOX_ADV_CTRL_H

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#ifndef _GUITYPES_H_
#include "gui/core/guiTypes.h"
#endif

#include <string>
#include <hash_map>

#include "guiRichTextCtrl.h"

#include "guiContainerAdv.h"

class GuiListBoxAdvCtrl;

class GuiListBoxAdvCtrlListItem : public GuiContainerAdv
{
	friend class GuiListBoxAdvCtrl;
	typedef GuiContainerAdv Parent;
public:

	DECLARE_CONOBJECT(GuiListBoxAdvCtrlListItem);
	
	GuiListBoxAdvCtrlListItem();
	virtual ~GuiListBoxAdvCtrlListItem();
	static void initPersistFields();

	void setKey( const char* keyString );
	void setTag( const int tagId );

	void addColumn( const char* text, int totalWidth, int width );
	void onPreRender();

	void setNewPosition( Point2I newPoint, float time );

	void onMouseDown(const GuiEvent &event);

private:
	std::string mKey;
	int mTag;
	Vector<S32> mSubItems;
	Point2I mNewPoint;
	Point2I mOldPoint;
	float mMoveTime;
	bool mIsMoving;
	U32 mLastTime;
} ;

class GuiListBoxAdvCtrlSubListItem : public GuiRichTextCtrl
{
	friend class GuiListBoxAdvCtrl;
	typedef GuiRichTextCtrl Parent;
public:

	DECLARE_CONOBJECT(GuiListBoxAdvCtrlSubListItem);

	GuiListBoxAdvCtrlSubListItem();
	static void initPersistFields();

	void onMouseDown(const GuiEvent &event);
};

class GuiListBoxAdvCtrl : public GuiControl
{
	typedef GuiControl Parent;

public:
	GuiListBoxAdvCtrl();
	virtual ~GuiListBoxAdvCtrl();

	DECLARE_CONOBJECT(GuiListBoxAdvCtrl);
	typedef std::list< int > ItemList;
	typedef stdext::hash_map< std::string, int > ItemMap;
	typedef stdext::hash_map< std::string, std::string> listBoxTagMap;
	ItemMap mItemMap;
	listBoxTagMap mTagMap;


	static void initPersistFields();

	void onPreRender();
	void onRender( Point2I offset, const RectI& updateRect );	

	void setSelectedItem( const char* key );
	int addItem( const char* text, const char* key );
	void removeItem( int listItemId );
	void removeItem( const char* key );
	ItemMap& getItemMap();
	int find( const char* keyString );
	std::string getSelectedItem();
	std::string getListBoxTagText(const char* keyString);
	bool  setListBoxTagText(const char* text, const char* key);

	bool  setCommand(const char* cmdString,const char* key);
	bool  setAltCommand(const char* cmdString,const char* key);
	

	void resetLayout();
	std::string getColumn( const char* text, int i);
	void clearItems();
private:
	ItemList mListItems;
	Vector<S32> mColumnWidth;
	int mLineHeight;
	std::string mSelectedKey;
};


#endif