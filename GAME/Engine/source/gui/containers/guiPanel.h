//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _GUI_PANEL_H_
#define _GUI_PANEL_H_

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#ifndef _GUITICKCTRL_H_
#include "gui/shiny/guiTickCtrl.h"
#endif

#ifndef _GUICONTAINER_H_
#include "gui/containers/guiContainer.h"
#endif

/// @addtogroup gui_container_group Containers
///
/// @ingroup gui_group Gui System
/// @{
class  GuiPanel : public GuiContainer
{
private:
   // Parent TypeDef
   typedef GuiContainer Parent;
protected:
   ColorI		mColorStart;
   ColorI		mColorEnd;
public:
   DECLARE_CONOBJECT(GuiPanel);

   // Constructor/Destructor/Conobject Declaration
   GuiPanel();
   virtual ~GuiPanel();

   void onRender(Point2I offset, const RectI &updateRect);

   void setVisible(bool value) { Parent::setVisible(value); setUpdateLayout( updateParent );};
};
/// @}

#endif