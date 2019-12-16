//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUIBUTTONCTRL_H_
#define _GUIBUTTONCTRL_H_

#ifndef _GUIBUTTONBASECTRL_H_
#include "gui/buttons/guiButtonBaseCtrl.h"
#endif

class GuiButtonCtrl : public GuiButtonBaseCtrl
{
   typedef GuiButtonBaseCtrl Parent;
protected:
   bool mHasTheme;
   StringTableEntry	mRightMouseDownCommand;		//响应鼠标右键按下时触发的脚本
   StringTableEntry mMouseDownCommand;          //响应鼠标左键按下时触发的脚本
   StringTableEntry mMouseUpCommand;            //响应鼠标左键松开时触发的脚本

public:
   DECLARE_CONOBJECT(GuiButtonCtrl);
   GuiButtonCtrl();
   static void initPersistFields();
   void onRightMouseDown(const GuiEvent &event);
   void onMouseDown(const GuiEvent &event);
   void onMouseUp(const GuiEvent &event);

   bool onWake();
   void onRender(Point2I offset, const RectI &updateRect);
};

#endif //_GUI_BUTTON_CTRL_H
