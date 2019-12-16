#ifndef GUI_SELECT_CTRL_H
#define GUI_SELECT_CTRL_H

#include "gui/core/guiControl.h"
#include "gfx/gfxDevice.h"
#include "console/consoleTypes.h"

#include <list>
#include <string>

class GuiSelectCtrl : public GuiControl
{
	typedef GuiControl Parent;
public:
	GuiSelectCtrl();
	~GuiSelectCtrl();

	void addItem( const char* string );
	void setCount( int nCount );

	const char* getSelectItem();

	bool onKeyDown(const GuiEvent &event);
	void onMouseDown(const GuiEvent& event);

	static void initPersistFields();

	void drawBackGround( Point2I& offset );
	void drawSelectBox( Point2I& offset );

	void drawAllItemString( Point2I& offset );

	void onRender(Point2I offset, const RectI &updateRect);

	DECLARE_CONOBJECT(GuiSelectCtrl);

private:
	typedef std::list< std::string > StringList;

	StringList mStringList;
	S32 mMaxCount;
	S32 mSelectIndex;
	S32 mItemHeight;		

	
};



#endif