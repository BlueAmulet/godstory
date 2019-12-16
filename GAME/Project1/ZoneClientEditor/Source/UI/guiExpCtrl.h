#ifndef _GUIEXPCTRL_H_
#define _GUIEXPCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

class GuiExpCtrl : public GuiControl
{
private:
	typedef GuiControl Parent;

	char m_szExpDescription[256];
	char m_szExpPercent[32];
	char m_szExp[32] ;

	RectI mExpBoundary;
	StringTableEntry mExpImageFile[3];
	GFXTexHandle mExpTexture[3];

	RectI mExpBackBoundary;
	StringTableEntry mExpBackImageFile;
	GFXTexHandle mExpBackTexture;

	RectI mExpTopBoundary;
	StringTableEntry mExpTopImageFile;
	GFXTexHandle mExpTopTexture;

	F32 mExpPercentum;

public:

	GuiExpCtrl();
	DECLARE_CONOBJECT(GuiExpCtrl);

	static void initPersistFields();

	bool onWake();
	void onRender(Point2I offset, const RectI &updateRect);
	bool UpdateDate();
	void setPercentum(F32 percentum);

protected:
};

#endif