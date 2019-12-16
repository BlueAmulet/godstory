#ifndef GUI_RICHEDIT_CTRL_H
#define GUI_RICHEDIT_CTRL_H

#include "math\mPoint.h"
#include "Util\RichTextDrawer.h"

#include <string>

class GuiRichEditCtrl : public GuiControl
{
	typedef GuiControl Parent;
public:
	GuiRichEditCtrl();
	virtual ~GuiRichEditCtrl();

	static void initPersistFields();
	DECLARE_CONOBJECT(GuiRichEditCtrl);

	void onRender(Point2I offset, const RectI &updateRect);
	void setCursorPosition( Point2I position );
	Point2I getCursorPosition();

	void insertChar( const wchar_t Char );
	void insertPic( const char* pPicPath );
	bool onKeyDown(const GuiEvent &event);
private:
	void drawCursor(const Point2I& offset);
	void buildText();
	void clearItemList();

private:

	struct Item
	{
		virtual std::string getText() = 0;
	};

	struct CharItem : Item
	{
		wchar_t Char[2];
		std::string getText();
	};

	struct PicItem : Item
	{
		std::string picPath;
		std::string getText();
	};

	struct ReturnItem : Item
	{
		std::string getText();
	};

	Point2I mCurrosPosition;

	typedef std::list< Item* > ItemList;
	ItemList mList;
	Item* mCurrentItem;

	CRichTextDrawer mDrawer;

	int mLastCursorTime;
	bool mShowCursor;

};

#endif /*GUI_RICHEDIT_CTRL_H*/