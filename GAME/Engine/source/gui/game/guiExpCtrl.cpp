#include "console/console.h"
#include "console/consoleTypes.h"

#include "gui/game/guiExpCtrl.h"

IMPLEMENT_CONOBJECT(GuiExpCtrl);

GuiExpCtrl::GuiExpCtrl()
{
	dSprintf(m_szExpDescription,sizeof(m_szExpDescription),"");
	dSprintf(m_szExpPercent,sizeof(m_szExpPercent),"");
	dSprintf(m_szExp,sizeof(m_szExp),"");
}

void GuiExpCtrl::initPersistFields()
{
	Parent::initPersistFields();
}

bool GuiExpCtrl::onWake()
{
	if (!Parent::onWake())
		return false;

	setActive(true);

	return true;
}

void GuiExpCtrl::onPreRender()
{

}

void GuiExpCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	Parent::onRender( offset, updateRect );

	UpdateDate();

	//render the children
	renderChildControls(offset, updateRect);
}

void GuiExpCtrl::UpdateDate()
{

}
