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
   StringTableEntry	mRightMouseDownCommand;		//��Ӧ����Ҽ�����ʱ�����Ľű�
   StringTableEntry mMouseDownCommand;          //��Ӧ����������ʱ�����Ľű�
   StringTableEntry mMouseUpCommand;            //��Ӧ�������ɿ�ʱ�����Ľű�

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
