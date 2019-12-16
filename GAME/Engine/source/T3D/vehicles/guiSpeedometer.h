//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/controls/guiBitmapCtrl.h"
//#include "console/consoleTypes.h"
//#include "T3D/gameConnection.h"
//#include "T3D/vehicles/vehicle.h"
//#include "gfx/primBuilder.h"

//-----------------------------------------------------------------------------
/// A Speedometer control.
/// This gui displays the speed of the current Vehicle based
/// control object. This control only works if a server
/// connection exists and it's control object is a vehicle. If
/// either of these requirements is false, the control is not rendered.
class GuiSpeedometerHud : public GuiBitmapCtrl
{
   typedef GuiBitmapCtrl Parent;

   F32   mSpeed;        ///< Current speed
   F32   mMaxSpeed;     ///< Max speed at max need pos
   F32   mMaxAngle;     ///< Max pos of needle
   F32   mMinAngle;     ///< Min pos of needle
   Point2F mCenter;     ///< Center of needle rotation
   ColorF mColor;       ///< Needle Color
   F32   mNeedleLength;
   F32   mNeedleWidth;
   F32   mTailLength;

public:
   GuiSpeedometerHud();

   void onRender( Point2I, const RectI &);
   static void initPersistFields();
   DECLARE_CONOBJECT( GuiSpeedometerHud );
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
