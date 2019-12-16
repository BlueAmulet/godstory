
#pragma once
#ifndef _GUIBITMAPWINDOWCTRL_H_
#define  _GUIBITMAPWINDOWCTRL_H_

#ifndef _GUIBITMAPCTRL_H_
#include "gui/controls/guiBitmapCtrl.h"
#endif

class GuiBitmapWindowCtrl :public GuiBitmapCtrl
{
private:
    typedef GuiBitmapCtrl Parent;

protected:
	RectI		m_MoveHotspot;			// Hotspot region for moving action.
	RectI		m_OriginalBoundary;		// Original image boundary.

	bool		m_IsMoving;				// Is this image moving.
	Point2I	    m_MouseDownPosition;	// Mouse pressed position.

public:
    GuiBitmapWindowCtrl();
	DECLARE_CONOBJECT(GuiBitmapWindowCtrl);

	static void initPersistFields();
    
	bool   onWake();
	virtual void onMouseDown(const GuiEvent &event);
	virtual void onMouseDragged(const GuiEvent &event);
	virtual void onMouseUp(const GuiEvent &event);
	virtual void onMouseEnter(const GuiEvent &event);
	virtual void onMouseLeave(const GuiEvent &event);
	virtual void onRightMouseDown(const GuiEvent &event);
	void onRender(Point2I offset, const RectI &updateRect);
	void intoRound();
};

#endif