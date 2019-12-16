//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "guiSpeedometer.h"
#include "gui/controls/guiBitmapCtrl.h"
#include "console/consoleTypes.h"
#include "T3D/gameConnection.h"
#include "T3D/vehicles/vehicle.h"
#include "gfx/primBuilder.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
////#define STATEBLOCK
GFXStateBlock* GuiSpeedometerHud::mSetSB = NULL;
//-----------------------------------------------------------------------------
/// A Speedometer control.
/// This gui displays the speed of the current Vehicle based
/// control object. This control only works if a server
/// connection exists and it's control object is a vehicle. If
/// either of these requirements is false, the control is not rendered.
//class GuiSpeedometerHud : public GuiBitmapCtrl
//{
//   typedef GuiBitmapCtrl Parent;
//
//   F32   mSpeed;        ///< Current speed
//   F32   mMaxSpeed;     ///< Max speed at max need pos
//   F32   mMaxAngle;     ///< Max pos of needle
//   F32   mMinAngle;     ///< Min pos of needle
//   Point2F mCenter;     ///< Center of needle rotation
//   ColorF mColor;       ///< Needle Color
//   F32   mNeedleLength;
//   F32   mNeedleWidth;
//   F32   mTailLength;
//
//public:
//   GuiSpeedometerHud();
//
//   void onRender( Point2I, const RectI &);
//   static void initPersistFields();
//   DECLARE_CONOBJECT( GuiSpeedometerHud );
//};


//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT( GuiSpeedometerHud );

GuiSpeedometerHud::GuiSpeedometerHud()
{
   mSpeed = 0;
   mMaxSpeed = 100;
   mMaxAngle = 0;
   mMinAngle = 200;
   mCenter.set(0,0);
   mNeedleWidth = 3;
   mNeedleLength = 10;
   mTailLength = 5;
   mColor.set(1,0,0,1);
}

void GuiSpeedometerHud::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Needle");
   addField("maxSpeed", TypeF32, Offset( mMaxSpeed, GuiSpeedometerHud ) );
   addField("minAngle", TypeF32, Offset( mMinAngle, GuiSpeedometerHud ) );
   addField("maxAngle", TypeF32, Offset( mMaxAngle, GuiSpeedometerHud ) );
   addField("color", TypeColorF, Offset( mColor, GuiSpeedometerHud ) );
   addField("center", TypePoint2F, Offset( mCenter, GuiSpeedometerHud ) );
   addField("length", TypeF32, Offset( mNeedleLength, GuiSpeedometerHud ) );
   addField("width", TypeF32, Offset( mNeedleWidth, GuiSpeedometerHud ) );
   addField("tail", TypeF32, Offset( mTailLength, GuiSpeedometerHud ) );
   endGroup("Needle");
}


//-----------------------------------------------------------------------------
/**
   Gui onRender method.
   Renders a health bar with filled background and border.
*/
void GuiSpeedometerHud::onRender(Point2I offset, const RectI &updateRect)
{
   // Must have a connection and player control object
   GameConnection* conn = GameConnection::getConnectionToServer();
   if (!conn)
      return;
   Vehicle* control = dynamic_cast<Vehicle*>(conn->getControlObject());
   if (!control)
      return;

   Parent::onRender(offset,updateRect);

   // Use the vehicle's velocity as it's speed...
   mSpeed = control->getVelocity().len();
   if (mSpeed > mMaxSpeed)
      mSpeed = mMaxSpeed;

   // Render the needle
   GFX->pushWorldMatrix();   // glPushMatrix();
   Point2F center = mCenter;
   if (isZero(center.x) && isZero(center.y))
   {
      center.x = getExtent().x / 2.0f;
      center.y = getExtent().y / 2.0f;
   }
   MatrixF newMat(1);

   newMat.setPosition(Point3F(getLeft() + center.x, getTop() + center.y, 0.0f));

   F32 rotation = mMinAngle + (mMaxAngle - mMinAngle) * (mSpeed / mMaxSpeed);
   AngAxisF newRot(Point3F(0.0f,0.0f,-1.0f), rotation);

   newRot.setMatrix(&newMat);
#ifdef STATEBLOCK
	AssertFatal(mSetSB, "GuiSpeedometerHud::onRender -- mSetSB cannot be NULL.");
	mSetSB->apply();
#else
	GFX->setAlphaBlendEnable( true );
	GFX->setSrcBlend(GFXBlendSrcAlpha);
	GFX->setDestBlend(GFXBlendInvSrcAlpha);

	GFX->setTextureStageColorOp(0, GFXTOPDisable);
#endif

   GFX->setTexture(0, NULL);

   PrimBuild::begin(GFXLineStrip, 5);
   PrimBuild::color4f(mColor.red, mColor.green, mColor.blue, mColor.alpha);

   PrimBuild::vertex2f(+mNeedleLength,-mNeedleWidth);
   PrimBuild::vertex2f(+mNeedleLength,+mNeedleWidth);
   PrimBuild::vertex2f(-mTailLength  ,+mNeedleWidth);
   PrimBuild::vertex2f(-mTailLength  ,-mNeedleWidth);

   // Get back to the start!
   PrimBuild::vertex2f(+mNeedleLength,-mNeedleWidth);

   PrimBuild::end();
}


void GuiSpeedometerHud::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPDisable );
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->endStateBlock(mSetSB);
}


void GuiSpeedometerHud::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}
}

void GuiSpeedometerHud::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		resetStateBlock();
	}
}

void GuiSpeedometerHud::shutdown()
{
	SAFE_DELETE(mSetSB);
}

