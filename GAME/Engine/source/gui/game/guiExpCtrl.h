#ifndef _GUIEXPCTRL_H_
#define _GUIEXPCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#ifndef _GUITEXTCTRL_H_
#include "gui/controls/guiTextCtrl.h"
#endif

#ifndef _GUIPROGRESSCTRL_H_
#include "gui/game/guiProgressCtrl.h"
#endif

class GuiExpCtrl : public GuiProgressCtrl
{
private:
	typedef GuiProgressCtrl Parent;

	char m_szExpDescription[256];
	char m_szExpPercent[32];
	char m_szExp[32] ;

public:

	GuiExpCtrl();
	DECLARE_CONOBJECT(GuiExpCtrl);

	static void initPersistFields();

	bool onWake();
	void onPreRender();
	void onRender(Point2I offset, const RectI &updateRect);
	void UpdateDate();

protected:
};

#endif