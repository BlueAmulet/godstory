//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "gfx/gfxDevice.h"
#include "gui/core/guiCanvas.h"
#include "gui/buttons/guiRadioCtrl.h"
#include "console/consoleTypes.h"

//---------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiRadioCtrl);

GuiRadioCtrl::GuiRadioCtrl()
{
   mButtonType = ButtonTypeRadio;
}
