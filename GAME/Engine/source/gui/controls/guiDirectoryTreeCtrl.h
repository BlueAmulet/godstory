//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUI_DIRECTORYTREECTRL_H_
#define _GUI_DIRECTORYTREECTRL_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _RESMANAGER_H_
#include "core/resManager.h"
#endif

#ifndef _GUI_TREEVIEWCTRL_H
#include "gui/controls/guiTreeViewCtrl.h"
#endif

class GuiDirectoryTreeCtrl : public GuiControl
{
private:
   typedef GuiControl Parent;
public:
   GuiDirectoryTreeCtrl();
   bool onWake();
   DECLARE_CONOBJECT(GuiDirectoryTreeCtrl);
};

#endif
