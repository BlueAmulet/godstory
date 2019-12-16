#ifndef GUI_CONTAINERADV_H
#define GUI_CONTAINERADV_H

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#ifndef _GUITYPES_H_
#include "gui/core/guiTypes.h"
#endif

class GuiContainerAdv : public GuiControl
{
	typedef GuiControl Parent;
public:	
	int mAlpha;

	DECLARE_CONOBJECT( GuiContainerAdv );

	GuiContainerAdv();
	static void initPersistFields();

	void onRender(Point2I offset, const RectI &updateRect);

	void onPreRender();

	void fadeout( float time );
	void fadein( float time );

	void setVisible( bool value );
	bool isVisible();

private:
	int mFallType;
	U32 mLastTime;
	float mFallTime;
	bool mUseFadeout;
	bool mIsVisible;
};

#endif