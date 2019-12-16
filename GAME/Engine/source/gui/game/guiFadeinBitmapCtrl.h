//-----------------------------------------------------------------------------
// PowerEngine
//
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/consoleTypes.h"


#include "gui/controls/guiBitmapCtrl.h"

class GuiFadeinBitmapCtrl : public GuiBitmapCtrl
{
   typedef GuiBitmapCtrl Parent;
public:
   U32 wakeTime;
   bool done;
   U32 fadeinTime;
   U32 waitTime;
   U32 fadeoutTime;

   GuiFadeinBitmapCtrl()
   {
      wakeTime    = 0;
      fadeinTime  = 1000;
      waitTime    = 2000;
      fadeoutTime = 1000;
      done        = false;
   }
   void onPreRender()
   {
      Parent::onPreRender();
      setUpdate();
   }
   void onMouseDown(const GuiEvent &)
   {
      Con::executef(this, "click");
   }
   bool onKeyDown(const GuiEvent &)
   {
      Con::executef(this, "click");
      return true;
   }
   DECLARE_CONOBJECT(GuiFadeinBitmapCtrl);
   bool onWake()
   {
      if(!Parent::onWake())
         return false;
      wakeTime = Platform::getRealMilliseconds();
      return true;
   }
   void onRender(Point2I offset, const RectI &updateRect);
   static void initPersistFields()
   {
      Parent::initPersistFields();
      addField("fadeinTime", TypeS32, Offset(fadeinTime, GuiFadeinBitmapCtrl));
      addField("waitTime", TypeS32, Offset(waitTime, GuiFadeinBitmapCtrl));
      addField("fadeoutTime", TypeS32, Offset(fadeoutTime, GuiFadeinBitmapCtrl));
      addField("done", TypeBool, Offset(done, GuiFadeinBitmapCtrl));
   }

private:
	static GFXStateBlock* mSetSB;
public:
	static void init();
	static void shutdown();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};


























