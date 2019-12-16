#ifndef GUI_RICHTEXT_CTRL_H
#define GUI_RICHTEXT_CTRL_H

#pragma once

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#include "util/RichTextDrawer.h"
#include "gfx/gfxTextureHandle.h"

class GuiRichTextCtrl : public GuiControl
{
	typedef GuiControl Parent;
public:
	GuiRichTextCtrl();
	virtual ~GuiRichTextCtrl();

	DECLARE_CONOBJECT(GuiRichTextCtrl);

	static void initPersistFields();
	virtual bool onWake();
	virtual void onSleep();
	virtual void onPreRender();
	virtual void onRender(Point2I offset, const RectI &updateRect);

	void onMouseUp(const GuiEvent &event);
	void onMouseDown(const GuiEvent &event);
	void onMouseMove(const GuiEvent &event);
	void onMouseLeave(const GuiEvent &event);
	void onMouseDragged(const GuiEvent &event);

	void onRightMouseDown(const GuiEvent &event);

	void setContent( const char* pText );
	CRichTextDrawer* getDrawer();
	void refresh();
	void addContent( const char* pText );
	CRichTextDrawer mDrawer;
	void setDefaultLineHeight( int lineHeight );

	StringTableEntry mMouseRightDown;

	int mLineHeight;

private:
	char* mText;

};



#endif