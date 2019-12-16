//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUIBITMAPCTRL_H_
#define _GUIBITMAPCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#include "UI/guiContainerAdv.h"

/// Renders a bitmap.
class GuiBitmapCtrl : public GuiContainerAdv
{
private:
   typedef GuiContainerAdv Parent;

protected:
   static bool setBitmapName( void *obj, const char *data );
   static const char *getBitmapName( void *obj, const char *data );

   StringTableEntry mBitmapName;
   GFXTexHandle mTextureObject;
   Point2I startPoint;
   bool mWrap;

public:
   //creation methods
   DECLARE_CONOBJECT(GuiBitmapCtrl);
   GuiBitmapCtrl();
   static void initPersistFields();

   //Parental methods
   bool onWake();
   void onSleep();
   void inspectPostApply();

   void setBitmap(const char *name,bool resize = false);
   void setBitmap(GFXTexHandle handle, bool resize = false);


   void updateSizing();

   void onRender(Point2I offset, const RectI &updateRect);
   void setValue(S32 x, S32 y);
};

#endif
