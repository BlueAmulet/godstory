//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/controls/guiDirectoryTreeCtrl.h"

IMPLEMENT_CONOBJECT(GuiDirectoryTreeCtrl);

GuiDirectoryTreeCtrl::GuiDirectoryTreeCtrl()
{
}

bool GuiDirectoryTreeCtrl::onWake()
{
   AssertWarn(false,"This Control has been deprecated in favor of OS provided dialog functionality\n\nIf you are seeing this message, please note what you did before you encountered it and report it on TDN Mantis and let justin and adam know where the bug is");
   // Deprecated
   return false;
}


