//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/consoleTypes.h"
#include "T3D/gameConnection.h"
#include "gui/missionEditor/editor.h"
#include "gui/missionEditor/editTSCtrl.h"
#include "gui/core/guiCanvas.h"
#include "terrain/terrData.h"
#include "T3D/sphere.h"
#include "T3D/missionArea.h"
#include "gfx/primBuilder.h"
#include "sceneGraph/sceneGraph.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
////#define STATEBLOCK
GFXStateBlock* EditTSCtrl::mSetSB = NULL;
GFXStateBlock* EditTSCtrl::mSetMisSB = NULL;
//cull
GFXStateBlock* EditTSCtrl::mCullNoneSB = NULL;
GFXStateBlock* EditTSCtrl::mCullStoreSB = NULL;
//z enable
GFXStateBlock* EditTSCtrl::mZTrueSB = NULL;
GFXStateBlock* EditTSCtrl::mZFalseSB = NULL;
//alpha blend 
GFXStateBlock* EditTSCtrl::mAlphaBlendFalseSB = NULL;
GFXStateBlock* EditTSCtrl::mAlphaBlendTrueSB = NULL;
GFXStateBlock* EditTSCtrl::mAlphaBlendLerpSB = NULL;
//tex stage color op
GFXStateBlock* EditTSCtrl::mTSSColorOpSB = NULL;
IMPLEMENT_CONOBJECT(EditTSCtrl);

//------------------------------------------------------------------------------

Point3F  EditTSCtrl::smCamPos;
MatrixF  EditTSCtrl::smCamMatrix;
bool     EditTSCtrl::smCamOrtho = false;
F32      EditTSCtrl::smCamFarPlane;
F32      EditTSCtrl::smVisibleDistance = 4000.f;
U32      EditTSCtrl::smSceneBoundsMask = EnvironmentObjectType | TerrainObjectType | WaterObjectType | AtlasObjectType | CameraObjectType | DecalManagerObjectType;
Point3F  EditTSCtrl::smMinSceneBounds = Point3F(500.0f, 500.0f, 500.0f);

EditTSCtrl::EditTSCtrl()
{
   mRenderMissionArea = true;
   mMissionAreaFillColor.set(255,0,0,20);
   mMissionAreaFrameColor.set(255,0,0,128);

   mConsoleFrameColor.set(255,0,0,255);
   mConsoleFillColor.set(255,0,0,120);
   mConsoleSphereLevel = 1;
   mConsoleCircleSegments = 32;
   mConsoleLineWidth = 1;
   mRightMousePassThru = true;

   mConsoleRendering = false;

   mDisplayType = DisplayTypePerspective;
   mOrthoFOV = 50.0f;
   mOrthoCamTrans.set(0.0f, 0.0f, 0.0f);

   mIsoCamAngle = mDegToRad(45.0f);
   mIsoCamRot = EulerF(0, 0, 0);

   mActiveTerrain = NULL;

   mRenderGridPlane = true;
   mGridPlaneOriginColor = ColorI(0, 0, 0, 255);
   mGridPlaneColor = ColorI(0, 0, 0, 255);
   mGridPlaneMinorTickColor = ColorI(102, 102, 102, 255);
   mGridPlaneMinorTicks = 9;
   mGridPlaneSize = 1.0f;
   mGridPlaneSizePixelBias = 2.0f;

   mLastMousePos.set(0, 0);

   mAllowBorderMove = false;
   mMouseMoveBorder = 20;
   mMouseMoveSpeed = 0.1f;
   mLastBorderMoveTime = 0;
   mLeftMouseDown = false;
   mRightMouseDown = false;
   mMouseLeft = false;

}

EditTSCtrl::~EditTSCtrl()
{
}

//------------------------------------------------------------------------------

bool EditTSCtrl::onAdd()
{
   if(!Parent::onAdd())
      return(false);

   // give all derived access to the fields
   setModStaticFields(true);
   return true;
}

bool EditTSCtrl::onWake()
{
   if(!Parent::onWake())
      return(false);

   // Find a terrain block
   SimSet * scopeAlwaysSet = Sim::getGhostAlwaysSet();
   for(SimSet::iterator itr = scopeAlwaysSet->begin(); itr != scopeAlwaysSet->end(); itr++)
   {
      mActiveTerrain = dynamic_cast<TerrainBlock*>(*itr);
      if(mActiveTerrain)
         break;
   }

   return true;
}

void EditTSCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   // Perform possible mouse border move...
   if(mAllowBorderMove && smCamOrtho && !mLeftMouseDown && !mRightMouseDown && !mMouseLeft)
   {
      Point2I ext = getExtent();

      U32 current = Platform::getRealMilliseconds();
      bool update = false;
      F32 movex = 0.0f;
      F32 movey = 0.0f;

      Point2I localMouse = globalToLocalCoord(mLastMousePos);

      if(localMouse.x <= mMouseMoveBorder || localMouse.x >= ext.x - mMouseMoveBorder)
      {
         if(mLastBorderMoveTime != 0)
         {
            U32 dt = current - mLastBorderMoveTime;
            if(localMouse.x <= mMouseMoveBorder)
            {
               movex = mMouseMoveSpeed * dt;
            }
            else
            {
               movex = -mMouseMoveSpeed * dt;
            }
         }
         update = true;
      }

      if(localMouse.y <= mMouseMoveBorder || localMouse.y >= ext.y - mMouseMoveBorder)
      {
         if(mLastBorderMoveTime != 0)
         {
            U32 dt = current - mLastBorderMoveTime;
            if(localMouse.y <= mMouseMoveBorder)
            {
               movey = mMouseMoveSpeed * dt;
            }
            else
            {
               movey = -mMouseMoveSpeed * dt;
            }
         }
         update = true;
      }

      if(update)
      {
         mLastBorderMoveTime = current;
         calcOrthoCamOffset(movex, movey);
      }
      else
      {
         mLastBorderMoveTime = 0;
      }
   }

   updateGuiInfo();
   Parent::onRender(offset, updateRect);

}

//------------------------------------------------------------------------------

void EditTSCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addGroup("Mission Area");	
   addField("renderMissionArea", TypeBool, Offset(mRenderMissionArea, EditTSCtrl));
   addField("missionAreaFillColor", TypeColorI, Offset(mMissionAreaFillColor, EditTSCtrl));
   addField("missionAreaFrameColor", TypeColorI, Offset(mMissionAreaFrameColor, EditTSCtrl));
   endGroup("Mission Area");	

   addGroup("Grid");	
   addField("renderGridPlane", TypeBool, Offset(mRenderGridPlane, EditTSCtrl));
   addField("gridPlaneColor", TypeColorI, Offset(mGridPlaneColor, EditTSCtrl));
   addField("gridPlaneMinorTickColor", TypeColorI, Offset(mGridPlaneMinorTickColor, EditTSCtrl));
   addField("gridPlaneSize", TypeF32, Offset(mGridPlaneSize, EditTSCtrl));
   endGroup("Grid");	

   addGroup("BorderMovement");	
   addField("allowBorderMove", TypeBool, Offset(mAllowBorderMove, EditTSCtrl));
   addField("borderMovePixelSize", TypeS32, Offset(mMouseMoveBorder, EditTSCtrl));
   addField("borderMoveSpeed", TypeF32, Offset(mMouseMoveSpeed, EditTSCtrl));
   endGroup("BorderMovement");	

   addGroup("Misc");	
   addField("consoleFrameColor", TypeColorI, Offset(mConsoleFrameColor, EditTSCtrl));
   addField("consoleFillColor", TypeColorI, Offset(mConsoleFillColor, EditTSCtrl));
   addField("consoleSphereLevel", TypeS32, Offset(mConsoleSphereLevel, EditTSCtrl));
   addField("consoleCircleSegments", TypeS32, Offset(mConsoleCircleSegments, EditTSCtrl));
   addField("consoleLineWidth", TypeS32, Offset(mConsoleLineWidth, EditTSCtrl));
   endGroup("Misc");	
}

void EditTSCtrl::consoleInit()
{
   Con::addVariable("pref::Editor::visibleDistance", TypeF32, &EditTSCtrl::smVisibleDistance);
}

//------------------------------------------------------------------------------

void EditTSCtrl::make3DMouseEvent(Gui3DMouseEvent & gui3DMouseEvent, const GuiEvent & event)
{
   (GuiEvent&)(gui3DMouseEvent) = event;
   gui3DMouseEvent.mousePoint = event.mousePoint;

   if(!smCamOrtho)
   {
      // get the eye pos and the mouse vec from that...
      Point3F screenPoint((F32)gui3DMouseEvent.mousePoint.x, (F32)gui3DMouseEvent.mousePoint.y, 1.0f);

      Point3F wp;
      unproject(screenPoint, &wp);

      gui3DMouseEvent.pos = smCamPos;
      gui3DMouseEvent.vec = wp - smCamPos;
      gui3DMouseEvent.vec.normalize();
   }
   else
   {
      // get the eye pos and the mouse vec from that...
      Point3F screenPoint((F32)gui3DMouseEvent.mousePoint.x, (F32)gui3DMouseEvent.mousePoint.y, 0.0f);

      Point3F np, fp;
      unproject(screenPoint, &np);

      gui3DMouseEvent.pos = np;
      smCamMatrix.getColumn( 1, &(gui3DMouseEvent.vec) );
   }
}

//------------------------------------------------------------------------------

void EditTSCtrl::getCursor(GuiCursor *&cursor, bool &visible, const GuiEvent &event)
{
   make3DMouseEvent(mLastEvent, event);
   get3DCursor(cursor, visible, mLastEvent);
}

void EditTSCtrl::get3DCursor(GuiCursor *&cursor, bool &visible, const Gui3DMouseEvent &event)
{
   event;
   cursor = NULL;
   visible = false;
}

//------------------------------------------------------------------------------

void EditTSCtrl::onMouseUp(const GuiEvent & event)
{
   mLeftMouseDown = false;
   make3DMouseEvent(mLastEvent, event);
   on3DMouseUp(mLastEvent);
}

void EditTSCtrl::onMouseDown(const GuiEvent & event)
{
   mLeftMouseDown = true;
   mLastBorderMoveTime = 0;
   make3DMouseEvent(mLastEvent, event);
   on3DMouseDown(mLastEvent);
}

void EditTSCtrl::onMouseMove(const GuiEvent & event)
{
   make3DMouseEvent(mLastEvent, event);
   on3DMouseMove(mLastEvent);

   mLastMousePos = event.mousePoint;
}

void EditTSCtrl::onMouseDragged(const GuiEvent & event)
{
   make3DMouseEvent(mLastEvent, event);
   on3DMouseDragged(mLastEvent);

}

void EditTSCtrl::onMouseEnter(const GuiEvent & event)
{
   mMouseLeft = false;
   make3DMouseEvent(mLastEvent, event);
   on3DMouseEnter(mLastEvent);
}

void EditTSCtrl::onMouseLeave(const GuiEvent & event)
{
   mMouseLeft = true;
   mLastBorderMoveTime = 0;
   make3DMouseEvent(mLastEvent, event);
   on3DMouseLeave(mLastEvent);
}

void EditTSCtrl::onRightMouseDown(const GuiEvent & event)
{
   // always process the right mouse event first...

   mRightMouseDown = true;
   mLastBorderMoveTime = 0;

   make3DMouseEvent(mLastEvent, event);
   on3DRightMouseDown(mLastEvent);

   if(mRightMousePassThru && mProfile->mCanKeyFocus)
   {
      GuiCanvas *pCanvas = getRoot();
      if( !pCanvas )
         return;

      PlatformWindow *pWindow = static_cast<GuiCanvas*>(getRoot())->getPlatformWindow();
      if( !pWindow )
         return;

      PlatformCursorController *pController = pWindow->getCursorController();
      if( !pController )
         return;

      // ok, gotta disable the mouse
      // script functions are lockMouse(true); Canvas.cursorOff();
      pWindow->setMouseLocked(true);
      pCanvas->setCursorON( false );

      if(mDisplayType != DisplayTypePerspective)
      {
         mouseLock();
         mLastMousePos = event.mousePoint;
         pCanvas->setForceMouseToGUI(true);
         mLastMouseClamping = pCanvas->getClampPowerEngineCursor();
         pCanvas->setClampPowerEngineCursor(false);
      }

      if(mDisplayType == DisplayTypeIsometric)
      {
         // Store the screen center point on the terrain for a possible rotation
         if(mActiveTerrain)
         {
            F32 extx, exty;
            if(event.modifier & SI_SHIFT)
            {
               extx = F32(event.mousePoint.x);
               exty = F32(event.mousePoint.y);
            }
            else
            {
               extx = getExtent().x * 0.5;
               exty = getExtent().y * 0.5;
            }
            Point3F sp(extx, exty, 0.0f); // Near plane projection
            Point3F start;
            unproject(sp, &start);

            Point3F end = start + mLastEvent.vec * 4000.0f;
            Point3F tStartPnt, tEndPnt;
            mActiveTerrain->getTransform().mulP(start, &tStartPnt);
            mActiveTerrain->getTransform().mulP(end, &tEndPnt);

            RayInfo info;
            bool result = mActiveTerrain->castRay(tStartPnt, tEndPnt, &info);
            if(result)
            {
               info.point.interpolate(start, end, info.t);
               mIsoCamRotCenter = info.point;
            }
            else
            {
               mIsoCamRotCenter = start;
            }
         }
         else
         {
            F32 extx = getExtent().x * 0.5;
            F32 exty = getExtent().y * 0.5;
            Point3F sp(extx, exty, 0.0f); // Near plane projection
            unproject(sp, &mIsoCamRotCenter);
         }
      }

      setFirstResponder();
   }
}

void EditTSCtrl::onRightMouseUp(const GuiEvent & event)
{
   mRightMouseDown = false;
   make3DMouseEvent(mLastEvent, event);
   on3DRightMouseUp(mLastEvent);
}

void EditTSCtrl::onRightMouseDragged(const GuiEvent & event)
{
   make3DMouseEvent(mLastEvent, event);
   on3DRightMouseDragged(mLastEvent);

   // Handle translation of orthographic views
   if(mDisplayType != DisplayTypePerspective)
   {
      calcOrthoCamOffset((event.mousePoint.x - mLastMousePos.x), (event.mousePoint.y - mLastMousePos.y), event.modifier);

      mLastMousePos = event.mousePoint;
   }
}

bool EditTSCtrl::onMouseWheelUp( const GuiEvent &event )
{
   if(mDisplayType != DisplayTypePerspective && !event.modifier)
   {
      mOrthoFOV -= 2.0f;
      if(mOrthoFOV < 1.0f)
         mOrthoFOV = 1.0f;
      return true;
   }

   make3DMouseEvent(mLastEvent, event);
   on3DMouseWheelUp(mLastEvent);

   return true;
}

bool EditTSCtrl::onMouseWheelDown( const GuiEvent &event )
{
   if(mDisplayType != DisplayTypePerspective && !event.modifier)
   {
      mOrthoFOV += 2.0f;
      return true;
   }

   make3DMouseEvent(mLastEvent, event);
   on3DMouseWheelDown(mLastEvent);

   return true;
}


bool EditTSCtrl::onInputEvent(const InputEventInfo & event)
{
   if(mRightMousePassThru && event.deviceType == MouseDeviceType &&
      event.objInst == KEY_BUTTON1 && event.action == SI_BREAK)
   {
      // if the right mouse pass thru is enabled,
      // we want to reactivate mouse on a right mouse button up
      GuiCanvas *pCanvas = getRoot();
      if( !pCanvas )
         return false;

      PlatformWindow *pWindow = static_cast<GuiCanvas*>(getRoot())->getPlatformWindow();
      if( !pWindow )
         return false;

      PlatformCursorController *pController = pWindow->getCursorController();
      if( !pController )
         return false;

      pWindow->setMouseLocked(false);
      pCanvas->setCursorON( true );

      if(mDisplayType != DisplayTypePerspective)
      {
         mouseUnlock();
         pCanvas->setForceMouseToGUI(false);
         pCanvas->setClampPowerEngineCursor(mLastMouseClamping);
      }
   }
   // we return false so that the canvas can properly process the right mouse button up...
   return false;
}

//------------------------------------------------------------------------------

void EditTSCtrl::calcOrthoCamOffset(F32 mousex, F32 mousey, U8 modifier)
{
   F32 camScale = 0.01f;

   switch(mDisplayType)
   {
      case DisplayTypeTop:
         mOrthoCamTrans.x -= mousex * mOrthoFOV * camScale;
         mOrthoCamTrans.y += mousey * mOrthoFOV * camScale;
         break;

      case DisplayTypeBottom:
         mOrthoCamTrans.x -= mousex * mOrthoFOV * camScale;
         mOrthoCamTrans.y -= mousey * mOrthoFOV * camScale;
         break;

      case DisplayTypeFront:
         mOrthoCamTrans.x += mousex * mOrthoFOV * camScale;
         mOrthoCamTrans.z += mousey * mOrthoFOV * camScale;
         break;

      case DisplayTypeBack:
         mOrthoCamTrans.x -= mousex * mOrthoFOV * camScale;
         mOrthoCamTrans.z += mousey * mOrthoFOV * camScale;
         break;

      case DisplayTypeLeft:
         mOrthoCamTrans.y += mousex * mOrthoFOV * camScale;
         mOrthoCamTrans.z += mousey * mOrthoFOV * camScale;
         break;

      case DisplayTypeRight:
         mOrthoCamTrans.y -= mousex * mOrthoFOV * camScale;
         mOrthoCamTrans.z += mousey * mOrthoFOV * camScale;
         break;

      case DisplayTypeIsometric:
         if(modifier & SI_CTRL)
         {
            // NOTE: Maybe move the center of rotation code to right mouse down to avoid compound errors?
            F32 rot = mDegToRad(mousex);

            Point3F campos = (mRawCamPos + mOrthoCamTrans) - mIsoCamRotCenter;
            MatrixF mat(EulerF(0, 0, rot));
            mat.mulP(campos);
            mOrthoCamTrans = (campos + mIsoCamRotCenter) - mRawCamPos;
            mIsoCamRot.z += rot;

         }
         else
         {
            mOrthoCamTrans.x -= mousex * mOrthoFOV * camScale * mCos(mIsoCamRot.z) - mousey * mOrthoFOV * camScale * mSin(mIsoCamRot.z);
            mOrthoCamTrans.y += mousex * mOrthoFOV * camScale * mSin(mIsoCamRot.z) + mousey * mOrthoFOV * camScale * mCos(mIsoCamRot.z);
         }
         break;
   }
}

//------------------------------------------------------------------------------

void EditTSCtrl::renderWorld(const RectI & updateRect)
{
#ifdef STATEBLOCK
	AssertFatal(mSetSB, "EditTSCtrl::renderWorld -- mSetSB cannot be NULL.");
	mSetSB->apply();
#else
	GFX->setZEnable( true );
	GFX->setZFunc( GFXCmpLessEqual );
	GFX->setCullMode( GFXCullNone );
#endif

   GFX->setBaseRenderState();

   gClientSceneGraph->setDisplayTargetResolution(getExtent());

   gClientSceneGraph->renderScene();

   // render the mission area...
   if(mRenderMissionArea)
      renderMissionArea();

#ifdef STATEBLOCK
	AssertFatal(mZFalseSB, "EditTSCtrl::renderWorld -- mZFalseSB cannot be NULL.");
	mZFalseSB->apply();
#else
   GFX->setZEnable( false );
#endif


   // render through console callbacks
   SimSet * missionGroup = static_cast<SimSet*>(Sim::findObject("MissionGroup"));
   if(missionGroup)
   {
      mConsoleRendering = true;
#ifdef STATEBLOCK
		AssertFatal(mZTrueSB, "EditTSCtrl::renderWorld -- mZTrueSB cannot be NULL.");
		mZTrueSB->apply();
#else
		GFX->setZEnable( true );
#endif
      for(SimSetIterator itr(missionGroup); *itr; ++itr)
      {
         char buf[2][16];
         dSprintf(buf[0], 16, (*itr)->isSelected() ? "true" : "false");
         dSprintf(buf[1], 16, (*itr)->isExpanded() ? "true" : "false");
         Con::executef(*itr, "onEditorRender", getIdString(), buf[0], buf[1]);
      }
#ifdef STATEBLOCK
		AssertFatal(mZFalseSB, "EditTSCtrl::renderWorld -- mZFalseSB cannot be NULL.");
		mZFalseSB->apply();
#else
		GFX->setZEnable( false );
#endif
      mConsoleRendering = false;
   }

   GFX->disableShaders();

	// Draw the grid
   //GFX->setClipRect(updateRect);
#ifdef STATEBLOCK
	AssertFatal(mCullNoneSB, "EditTSCtrl::renderWorld -- mCullNoneSB cannot be NULL.");
	mCullNoneSB->apply();
#else
   GFX->setCullMode( GFXCullNone );
#endif

   renderGrid();

   // render the editor stuff
   renderScene(updateRect);

	// Draw the camera axis
   GFX->setClipRect(updateRect);
#ifdef STATEBLOCK
	AssertFatal(mCullNoneSB, "EditTSCtrl::renderWorld -- mCullNoneSB cannot be NULL.");
	mCullNoneSB->apply();
#else
	GFX->setCullMode( GFXCullNone );
#endif
   renderCameraAxis();

   GFX->setClipRect(updateRect);
#ifdef STATEBLOCK
	AssertFatal(mZTrueSB, "EditTSCtrl::renderWorld -- mZTrueSB cannot be NULL.");
	mZTrueSB->apply();
#else
	GFX->setZEnable( true );
#endif

}

void EditTSCtrl::renderMissionArea()
{
   MissionArea * obj = dynamic_cast<MissionArea*>(Sim::findObject("MissionAreaObject"));

   if( obj == NULL )
      return;
   //EditTSCTRL has a pointer to the currently active terrain. Going to use it
   //TerrainBlock * terrain = dynamic_cast<TerrainBlock*>(Sim::findObject("Terrain"));

   F32 height = 0.f;

   if( mActiveTerrain != NULL )
   {
      GridSquare * gs = mActiveTerrain->findSquare(TerrainBlock::BlockShift, Point2I(0,0));
      height = F32(gs->maxHeight) * 0.03125f + 10.f;
   }

   //
   const RectI &area = obj->getArea();
#ifdef STATEBLOCK
	Point2F min;
	Point2F max;
	min.set(area.point.x, area.point.y);
	max.set(area.point.x+area.extent.x, area.point.y+area.extent.y);
	AssertFatal(mCullStoreSB, "EditTSCtrl::renderMissionArea -- mCullStoreSB cannot be NULL.");
	mCullStoreSB->capture();
	AssertFatal(mSetMisSB, "EditTSCtrl::renderMissionArea -- mSetMisSB cannot be NULL.");
	mSetMisSB->apply();
#else
	Point2F min;
	Point2F max;
	min.set(area.point.x, area.point.y);
	max.set(area.point.x+area.extent.x, area.point.y+area.extent.y);
	//Point2F min((F32)area.point.x, (F32)area.point.y);
	//Point2F max((F32)(area.point.x + area.extent.x), (F32)(area.point.y + area.extent.y));
	GFXCullMode oldCull = GFX->getCullMode();
	GFX->setCullMode( GFXCullNone );
	GFX->setAlphaBlendEnable( true );
	GFX->setSrcBlend( GFXBlendSrcAlpha );
	GFX->setDestBlend( GFXBlendInvSrcAlpha );
#endif


   // This code block was mega-lame in the last incarnation of editTSCtrl. This
   // is less lame, but still pretty lame. -patw

   PrimBuild::color( mMissionAreaFillColor );
   bool bDoingFill = true;
   U32 numVertsPer = 5;
   GFXPrimitiveType primType = GFXTriangleFan;
   
renderStart:
   PrimBuild::begin( primType, numVertsPer );
      PrimBuild::vertex3f( min.x, min.y, 0.f );
      PrimBuild::vertex3f( max.x, max.y, 0.f );
      PrimBuild::vertex3f( max.x, min.y, height ); 
      PrimBuild::vertex3f( min.x, min.y, height );

      if( bDoingFill )
         PrimBuild::vertex3f( min.x, min.y, 0.f );
   PrimBuild::end();

   //
   PrimBuild::begin( primType, numVertsPer );
      PrimBuild::vertex3f( min.x, max.y, 0.f );
      PrimBuild::vertex3f( max.x, max.y, 0.f );
      PrimBuild::vertex3f( max.x, max.y, height );
      PrimBuild::vertex3f( min.x, max.y, height );

      if( bDoingFill )
         PrimBuild::vertex3f( min.x, max.y, 0.f );
   PrimBuild::end();

   //
   PrimBuild::begin( primType, numVertsPer );
      PrimBuild::vertex3f( min.x, min.y, 0.f );
      PrimBuild::vertex3f( min.x, max.y, 0.f );
      PrimBuild::vertex3f( min.x, max.y, height );
      PrimBuild::vertex3f( min.x, min.y, height );

      if( bDoingFill )
         PrimBuild::vertex3f( min.x, min.y, 0.f );
   PrimBuild::end();

   //
   PrimBuild::begin( primType, numVertsPer );
      PrimBuild::vertex3f( max.x, min.y, 0.f );
      PrimBuild::vertex3f( max.x, max.y, 0.f );
      PrimBuild::vertex3f( max.x, max.y, height );
      PrimBuild::vertex3f( max.x, min.y, height );

      if( bDoingFill )
         PrimBuild::vertex3f( max.x, min.y, 0.f );
   PrimBuild::end();

   // Now do the outline
   if( bDoingFill )
   {
      bDoingFill = false;
      numVertsPer = 4;
      primType = GFXLineStrip;
      goto renderStart;
   }
#ifdef STATEBLOCK
	AssertFatal(mCullStoreSB, "EditTSCtrl::renderMissionArea -- mCullStoreSB cannot be NULL.");
	mCullStoreSB->apply();
	AssertFatal(mAlphaBlendFalseSB, "EditTSCtrl::renderMissionArea -- mAlphaBlendFalseSB cannot be NULL.");
	mAlphaBlendFalseSB->apply();
#else
	GFX->setAlphaBlendEnable( false );
	GFX->setCullMode( oldCull );
#endif

}

void EditTSCtrl::renderCameraAxis()
{
   static MatrixF sRotMat(EulerF( (M_PI_F / -2.0f), 0.0f, 0.0f));

   MatrixF camMat = mLastCameraQuery.cameraMatrix;
   camMat.mul(sRotMat);
   camMat.inverse();

   MatrixF axis;
   axis.setColumn(0, Point3F(1, 0, 0));
   axis.setColumn(1, Point3F(0, 0, 1));
   axis.setColumn(2, Point3F(0, -1, 0));
   axis.mul(camMat);

   Point3F forwardVec, upVec, rightVec;
	axis.getColumn( 2, &forwardVec );
	axis.getColumn( 1, &upVec );
	axis.getColumn( 0, &rightVec );

   Point2I pos = getPosition();
	F32 offsetx = pos.x + 20.0;
	F32 offsety = pos.y + getExtent().y - 20.0;
	F32 scale = 15.0;

   // Generate correct drawing order
   ColorI c1(255,0,0);
   ColorI c2(0,255,0);
   ColorI c3(0,0,255);
	ColorI tc;
	Point3F *p1, *p2, *p3, *tp;
	p1 = &rightVec;
	p2 = &upVec;
	p3 = &forwardVec;
	if(p3->y > p2->y)
	{
		tp = p2; tc = c2;
		p2 = p3; c2 = c3;
		p3 = tp; c3 = tc;
	}
	if(p2->y > p1->y)
	{
		tp = p1; tc = c1;
		p1 = p2; c1 = c2;
		p2 = tp; c2 = tc;
	}

   PrimBuild::begin( GFXLineList, 6 );
		//*** Axis 1
		PrimBuild::color(c1);
		PrimBuild::vertex3f(offsetx, offsety, 0);
		PrimBuild::vertex3f(offsetx+p1->x*scale, offsety-p1->z*scale, 0);

		//*** Axis 2
		PrimBuild::color(c2);
		PrimBuild::vertex3f(offsetx, offsety, 0);
		PrimBuild::vertex3f(offsetx+p2->x*scale, offsety-p2->z*scale, 0);

		//*** Axis 3
		PrimBuild::color(c3);
		PrimBuild::vertex3f(offsetx, offsety, 0);
		PrimBuild::vertex3f(offsetx+p3->x*scale, offsety-p3->z*scale, 0);
   PrimBuild::end();
}

void EditTSCtrl::renderGrid()
{
   if(!mRenderGridPlane)
      return;

   if(mDisplayType == DisplayTypePerspective || mDisplayType == DisplayTypeIsometric)
      return;
#ifdef STATEBLOCK
	AssertFatal(mTSSColorOpSB, "EditTSCtrl::renderGrid -- mTSSColorOpSB cannot be NULL.");
	mTSSColorOpSB->apply();
#else
   GFX->setTextureStageColorOp( 0, GFXTOPDisable );
#endif


	// Set up based on the view
   S32 x,y,z;
   if(mDisplayType == DisplayTypeTop || mDisplayType == DisplayTypeBottom)
   {
      x=0;
      y=1;
      z=2;
   }
   else if(mDisplayType == DisplayTypeFront || mDisplayType == DisplayTypeBack)
   {
      x=0;
      y=2;
      z=1;
   }
   else // Left and right
   {
      x=1;
      y=2;
      z=0;
   }

   // Calculate the displayed grid size based on view
   F32 drawnGridSize = mGridPlaneSize;
   F32 gridPixelSize = GFX->projectRadius(1.0f, mGridPlaneSize);
   if(gridPixelSize < mGridPlaneSizePixelBias)
   {
      U32 counter = 1;
      while(gridPixelSize < mGridPlaneSizePixelBias)
      {
         drawnGridSize = mGridPlaneSize * counter * 10.0f;
         gridPixelSize = GFX->projectRadius(1.0f, drawnGridSize);

         ++counter;

         // No infinite loops here
         if(counter > 1000)
            break;
      }
   }

   F32 minorTickSize = 0;
   F32 gridSize = drawnGridSize;
   U32 minorTickMax = mGridPlaneMinorTicks + 1;
   if(minorTickMax > 0)
   {
      minorTickSize = drawnGridSize;
      gridSize = drawnGridSize * minorTickMax;
   }

   // Build the view-based origin
   F32 origin[3];
   F32 gridPos[3];
   gridPos[0] = smCamPos.x;
   gridPos[1] = smCamPos.y;
   gridPos[2] = smCamPos.z;
   origin[x] = gridPos[x] - fmod(gridPos[x], gridSize);
   origin[y] = gridPos[y] - fmod(gridPos[y], gridSize);
   VectorF dir;
   smCamMatrix.getColumn( 1, &dir );
   Point3F gridPlanePos = smCamPos + (dir * smCamFarPlane) / 2.0f; // Place grid between camera and far plane
   origin[z] = gridPlanePos[z];

   Point2F start(origin[x] - mOrthoWidth / 2, origin[y] - mOrthoHeight / 2);
   Point2F size(mOrthoWidth + 2 * gridSize, mOrthoHeight + 2 * gridSize);

   U32 xSteps = (U32)mCeil(mOrthoWidth / gridSize) + 2; // Additional two grid lines to ensure proper coverage
   F32 xHashStart = mFloor(start.x / gridSize) * gridSize;
   U32 ySteps = (U32)mCeil(mOrthoHeight / gridSize) + 2; // Additional two grid lines to ensure proper coverage
   F32 yHashStart = mFloor(start.y / gridSize) * gridSize;

   F32 points[3];

   PrimBuild::begin( GFXLineList, xSteps*2 + xSteps * (mGridPlaneMinorTicks * 2) + ySteps*2 + ySteps * (mGridPlaneMinorTicks * 2) );

   // Draw all minor ticks
   PrimBuild::color(mGridPlaneMinorTickColor);
   for(U32 i = 0; i < xSteps; i++)
   {
      F32 xStart = xHashStart + gridSize * i;
      for(U32 j = 1; j < minorTickMax; ++j)
      {
         points[x] = xStart + minorTickSize * j;
         points[y] = start.y;
         points[z] = origin[z];
         PrimBuild::vertex3f(points[0], points[1], points[2]);
         points[y] = start.y + size.y;
         PrimBuild::vertex3f(points[0], points[1], points[2]);
      }
   }
   for(U32 i = 0; i < ySteps; i++)
   {
      F32 yStart = yHashStart + gridSize * i;
      for(U32 j = 1; j < minorTickMax; ++j)
      {
         points[x] = start.x;
         points[y] = yStart + minorTickSize * j;
         points[z] = origin[z];
         PrimBuild::vertex3f(points[0], points[1], points[2]);
         points[x] = start.x + size.x;
         PrimBuild::vertex3f(points[0], points[1], points[2]);
      }
   }

   // Draw all major ticks
   PrimBuild::color(mGridPlaneColor);
   for(U32 i = 0; i < xSteps; i++)
   {
      points[x] = xHashStart + gridSize * i;
      points[y] = start.y;
      points[z] = origin[z];
      PrimBuild::vertex3f(points[0], points[1], points[2]);
      points[y] = start.y + size.y;
      PrimBuild::vertex3f(points[0], points[1], points[2]);
   }
   for(U32 i = 0; i < ySteps; i++)
   {
      points[x] = start.x;
      points[y] = yHashStart + gridSize * i;
      points[z] = origin[z];
      PrimBuild::vertex3f(points[0], points[1], points[2]);
      points[x] = start.x + size.x;
      PrimBuild::vertex3f(points[0], points[1], points[2]);
   }

   PrimBuild::end();
}

//------------------------------------------------------------------------------

#include "terrain/sky.h"

struct SceneBoundsInfo 
{
   bool  mValid;
   Box3F mBounds;

   SceneBoundsInfo()
   {
      mValid = false;
      mBounds.min.set(1e10, 1e10, 1e10);
      mBounds.max.set(-1e10, -1e10, -1e10);
   }
};

static void sceneBoundsCalcCallback(SceneObject* obj, void *key)
{
   // Early out for those objects that slipped through the mask check
   // because they belong to more than one type.
   if((obj->getType() & EditTSCtrl::smSceneBoundsMask) != 0)
      return;

   if(obj->isGlobalBounds())
      return;

   SceneBoundsInfo* bounds = (SceneBoundsInfo*)key;

   Point3F min = obj->getWorldBox().min;
   Point3F max = obj->getWorldBox().max;

   if (min.x <= -5000.0f || min.y <= -5000.0f || min.z <= -5000.0f ||
       min.x >=  5000.0f || min.y >=  5000.0f || min.z >=  5000.0f)
       Con::errorf("SceneObject %d (%s : %s) has a bounds that could cause problems with a non-perspective view", obj->getId(), obj->getClassName(), obj->getName());

   bounds->mBounds.min.setMin(min);
   bounds->mBounds.min.setMin(max);
   bounds->mBounds.max.setMax(min);
   bounds->mBounds.max.setMax(max);

   bounds->mValid = true;
}

bool EditTSCtrl::processCameraQuery(CameraQuery * query)
{
   if(mDisplayType == DisplayTypePerspective)
   {
      query->ortho = false;
   }
   else
   {
      query->ortho = true;
   }

   GameConnection* connection = dynamic_cast<GameConnection *>(NetConnection::getConnectionToServer());
   if (connection)
   {
      if (connection->getControlCameraTransform(0.032f, &query->cameraMatrix)) 
      {
         query->farPlane = getMax(smVisibleDistance, 50.0f);
         query->nearPlane = query->farPlane/5000.0f;
         query->fov = mDegToRad(90.0f);
		 query->staticDist = query->farPlane;

         if(query->ortho)
         {
            MatrixF camRot(true);
            SceneBoundsInfo sceneBounds;
            const F32 camBuffer = 1.0f;
            Point3F camPos = query->cameraMatrix.getPosition();

            F32 isocamplanedist = 0.0f;
            if(mDisplayType == DisplayTypeIsometric)
            {
               const RectI& vp = GFX->getViewport();
               isocamplanedist = 0.25 * vp.extent.y * mSin(mIsoCamAngle);
            }

            // Calculate the scene bounds
            gClientContainer.findObjects(~(smSceneBoundsMask), sceneBoundsCalcCallback, &sceneBounds);

            if(!sceneBounds.mValid)
            {
               sceneBounds.mBounds.max = camPos + smMinSceneBounds;
               sceneBounds.mBounds.min = camPos - smMinSceneBounds;
            }
            else
            {
               query->farPlane = getMax(smMinSceneBounds.x * 2.0f, (sceneBounds.mBounds.max - sceneBounds.mBounds.min).len() + camBuffer * 2.0f + isocamplanedist);
            }

            mRawCamPos = camPos;
            camPos += mOrthoCamTrans;

            switch(mDisplayType)
            {
               case DisplayTypeTop:
                  camRot.setColumn(0, Point3F(1.0, 0.0,  0.0));
                  camRot.setColumn(1, Point3F(0.0, 0.0, -1.0));
                  camRot.setColumn(2, Point3F(0.0, 1.0,  0.0));
                  camPos.z = getMax(camPos.z + smMinSceneBounds.z, sceneBounds.mBounds.max.z + camBuffer);
                  break;

               case DisplayTypeBottom:
                  camRot.setColumn(0, Point3F(1.0,  0.0,  0.0));
                  camRot.setColumn(1, Point3F(0.0,  0.0,  1.0));
                  camRot.setColumn(2, Point3F(0.0, -1.0,  0.0));
                  camPos.z = getMin(camPos.z - smMinSceneBounds.z, sceneBounds.mBounds.min.z - camBuffer);
                  break;

               case DisplayTypeFront:
                  camRot.setColumn(0, Point3F(-1.0,  0.0,  0.0));
                  camRot.setColumn(1, Point3F( 0.0, -1.0,  0.0));
                  camRot.setColumn(2, Point3F( 0.0,  0.0,  1.0));
                  camPos.y = getMax(camPos.z + smMinSceneBounds.z, sceneBounds.mBounds.max.y + camBuffer);
                  break;

               case DisplayTypeBack:
                  //camRot.setColumn(0, Point3F(1.0,  0.0,  0.0));
                  //camRot.setColumn(1, Point3F(0.0,  1.0,  0.0));
                  //camRot.setColumn(2, Point3F(0.0,  0.0,  1.0));
                  camPos.y = getMin(camPos.y - smMinSceneBounds.y, sceneBounds.mBounds.min.y - camBuffer);
                  break;

               case DisplayTypeLeft:
                  camRot.setColumn(0, Point3F( 0.0, -1.0,  0.0));
                  camRot.setColumn(1, Point3F( 1.0,  0.0,  0.0));
                  camRot.setColumn(2, Point3F( 0.0,  0.0,  1.0));
                  camPos.x = getMin(camPos.x - smMinSceneBounds.x, sceneBounds.mBounds.min.x - camBuffer);
                  break;

               case DisplayTypeRight:
                  camRot.setColumn(0, Point3F( 0.0,  1.0,  0.0));
                  camRot.setColumn(1, Point3F(-1.0,  0.0,  0.0));
                  camRot.setColumn(2, Point3F( 0.0,  0.0,  1.0));
                  camPos.x = getMax(camPos.x + smMinSceneBounds.x, sceneBounds.mBounds.max.x + camBuffer);
                  break;

               case DisplayTypeIsometric:
                  camPos.z = sceneBounds.mBounds.max.z + camBuffer + isocamplanedist;
                  MatrixF angle(EulerF(mIsoCamAngle, 0, 0));
                  MatrixF rot(mIsoCamRot);
                  camRot.mul(rot, angle);
                  break;
            }

            query->cameraMatrix = camRot;
            query->cameraMatrix.setPosition(camPos);
            query->fov = mOrthoFOV;
         }

         smCamMatrix = query->cameraMatrix;
         smCamMatrix.getColumn(3,&smCamPos);
         smCamOrtho = query->ortho;
         smCamFarPlane = query->farPlane;

		 gClientSceneGraph->setLastView2WorldMX(query->cameraMatrix);
         return true;
      }
   }
   return false;

}

//------------------------------------------------------------------------------
ConsoleMethod(EditTSCtrl, getDisplayType, S32, 2, 2, "")
{
   return object->getDisplayType();
}

ConsoleMethod(EditTSCtrl, setDisplayType, void, 3, 3, "(int displayType)")
{
   object->setDisplayType(dAtoi(argv[2]));
}

//------------------------------------------------------------------------------
// sort the surfaces: not correct when camera is inside sphere but not
// inside tesselated representation of sphere....
struct SortInfo 
{
   U32 idx;
   F32 dot;
};

static int QSORT_CALLBACK alphaSort(const void* p1, const void* p2)
{
   const SortInfo* ip1 = (const SortInfo*)p1;
   const SortInfo* ip2 = (const SortInfo*)p2;

   if(ip1->dot > ip2->dot)
      return(1);
   if(ip1->dot == ip2->dot)
      return(0);
   return(-1);
}

//------------------------------------------------------------------------------
ConsoleMethod(EditTSCtrl, renderSphere, void, 4, 5, "(Point3F pos, float radius, int subdivisions=NULL)")
{
   if(!object->mConsoleRendering)
      return;

   static Sphere sphere(Sphere::Icosahedron);

   if(!object->mConsoleFrameColor.alpha && !object->mConsoleFillColor.alpha)
      return;

   S32 sphereLevel = object->mConsoleSphereLevel;
   if(argc == 5)
      sphereLevel = dAtoi(argv[4]);

   const Sphere::TriangleMesh * mesh = sphere.getMesh(sphereLevel);

   Point3F pos;
   dSscanf(argv[2], "%f %f %f", &pos.x, &pos.y, &pos.z);

   F32 radius = dAtof(argv[3]);

   // sort the surfaces back->front
   Vector<SortInfo> sortInfos;

   Point3F camNormal = object->smCamPos - pos;
   camNormal.normalize();

   sortInfos.setSize(mesh->numPoly);
   for(U32 i = 0; i < mesh->numPoly; i++)
   {
      sortInfos[i].idx = i;
      sortInfos[i].dot = mDot(camNormal, mesh->poly[i].normal);
   }
   dQsort(sortInfos.address(), sortInfos.size(), sizeof(SortInfo), alphaSort);

#ifdef STATEBLOCK
	AssertFatal(EditTSCtrl::mAlphaBlendLerpSB, "ConsoleMethod -- EditTSCtrl-- renderSphere -- EditTSCtrl::mAlphaBlendLerpSB cannot be NULL.");
	EditTSCtrl::mAlphaBlendLerpSB->apply();
#else
	GFX->setAlphaBlendEnable( true );
	GFX->setSrcBlend( GFXBlendSrcAlpha );
	GFX->setDestBlend( GFXBlendInvSrcAlpha );
#endif


   /*
   // TODO: I am not sure GFX supports what is needed for this. Isn't glPolygonMode
   // a pretty old-school function? I will look this up. -patw

   // frame
   if(object->mConsoleFrameColor.alpha)
   {
   PrimBuild::color( object->mConsoleFrameColor );

   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

   // TODO: Set GFX line width (when it exists) to the value of 'object->mConsoleLineWidth'

   PrimBuild::begin( GFXTriangleList, mesh->numPoly * 3 );

   for(U32 i = 0; i < mesh->numPoly; i++)
   {
   Sphere::Triangle & tri = mesh->poly[sortInfos[i].idx];

   for( S32 j = 2; j >= 0; j-- )
   PrimBuild::vertex3fv( ( tri.pnt[j] * radius ) + pos );
   }

   PrimBuild::end();

   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

   // TODO: Reset GFX line width here
   }
   */

   // fill
   if(object->mConsoleFillColor.alpha)
   {
      PrimBuild::color( object->mConsoleFillColor );

      PrimBuild::begin( GFXTriangleList, mesh->numPoly * 3 );

      for(U32 i = 0; i < mesh->numPoly; i++)
      {
         Sphere::Triangle & tri = mesh->poly[sortInfos[i].idx];

         for( S32 j = 2; j >= 0; j-- )
            PrimBuild::vertex3fv( ( tri.pnt[j] * radius ) + pos );
      }

      PrimBuild::end();
   }
#ifdef STATEBLOCK
	AssertFatal(EditTSCtrl::mAlphaBlendFalseSB, "ConsoleMethod -- EditTSCtrl-- renderSphere -- EditTSCtrl::mAlphaBlendFalseSB cannot be NULL.");
	EditTSCtrl::mAlphaBlendFalseSB->apply();
#else
	GFX->setAlphaBlendEnable( false );
#endif
}

ConsoleMethod( EditTSCtrl, renderCircle, void, 5, 6, "(Point3F pos, Point3F normal, float radius, int segments=NULL)")
{
   if(!object->mConsoleRendering)
      return;

   if(!object->mConsoleFrameColor.alpha && !object->mConsoleFillColor.alpha)
      return;

   Point3F pos, normal;
   dSscanf(argv[2], "%g %g %g", &pos.x, &pos.y, &pos.z);
   dSscanf(argv[3], "%g %g %g", &normal.x, &normal.y, &normal.z);

   F32 radius = dAtof(argv[4]);

   S32 segments = object->mConsoleCircleSegments;
   if(argc == 6)
      segments = dAtoi(argv[5]);

   normal.normalize();

   AngAxisF aa;
   mCross(normal, Point3F(0,0,1), &aa.axis);
   aa.axis.normalizeSafe();
   aa.angle = mAcos(mClampF(mDot(normal, Point3F(0,0,1)), -1.f, 1.f));

   if(aa.angle == 0.f)
      aa.axis.set(0,0,1);

   MatrixF mat;
   aa.setMatrix(&mat);

   F32 step = M_2PI / segments;
   F32 angle = 0.f;

   Vector<Point3F> points;
   segments--;
   for(U32 i = 0; i < segments; i++)
   {
      Point3F pnt(mCos(angle), mSin(angle), 0.f);

      mat.mulP(pnt);
      pnt *= radius;
      pnt += pos;

      points.push_back(pnt);
      angle += step;
   }

#ifdef STATEBLOCK
	AssertFatal(EditTSCtrl::mAlphaBlendLerpSB, "ConsoleMethod -- EditTSCtrl-- renderCircle -- EditTSCtrl::mAlphaBlendLerpSB cannot be NULL.");
	EditTSCtrl::mAlphaBlendLerpSB->apply();
#else
	GFX->setAlphaBlendEnable( true );
	GFX->setSrcBlend( GFXBlendSrcAlpha );
	GFX->setDestBlend( GFXBlendInvSrcAlpha );
#endif
   // framed
   if(object->mConsoleFrameColor.alpha)
   {
      // TODO: Set GFX line width (when it exists) to the value of 'object->mConsoleLineWidth'

      PrimBuild::color( object->mConsoleFrameColor );

      PrimBuild::begin( GFXLineStrip, points.size() + 1 );

      for( int i = 0; i < points.size(); i++ )
         PrimBuild::vertex3fv( points[i] );

      // GFX does not have a LineLoop primitive, so connect the last line
      if( points.size() > 0 )
         PrimBuild::vertex3fv( points[0] );

      PrimBuild::end();

      // TODO: Reset GFX line width here
   }

   // filled
   if(object->mConsoleFillColor.alpha)
   {
      PrimBuild::color( object->mConsoleFillColor );

      PrimBuild::begin( GFXTriangleFan, points.size() + 1 );

      // Center point
      PrimBuild::vertex3fv( pos );

      // Edge verts
      for( int i = 0; i < points.size(); i++ )
         PrimBuild::vertex3fv( points[i] );

      PrimBuild::end();
      /*
      // I left this code in here because this is how it was done originally. I 
      // see no reason to do it with a triangle list, since this is what the 
      // triangle-fan primitive is for. -patw
      glBegin(GL_TRIANGLES);

      for(S32 i = 0; i < points.size(); i++)
      {
      S32 j = (i + 1) % points.size();
      glVertex3f(points[i].x, points[i].y, points[i].z);
      glVertex3f(points[j].x, points[j].y, points[j].z);
      glVertex3f(pos.x, pos.y, pos.z);
      }

      glEnd();
      */
   }

#ifdef STATEBLOCK
	AssertFatal(EditTSCtrl::mAlphaBlendFalseSB, "ConsoleMethod -- EditTSCtrl-- renderCircle -- EditTSCtrl::mAlphaBlendFalseSB cannot be NULL.");
	EditTSCtrl::mAlphaBlendFalseSB->apply();
#else
	GFX->setAlphaBlendEnable( false );
#endif
}

ConsoleMethod( EditTSCtrl, renderTriangle, void, 5, 5, "(Point3F a, Point3F b, Point3F c)")
{

   if(!object->mConsoleRendering)
      return;

   if(!object->mConsoleFrameColor.alpha && !object->mConsoleFillColor.alpha)
      return;

   Point3F pnts[3];
   for(U32 i = 0; i < 3; i++)
      dSscanf(argv[i+2], "%f %f %f", &pnts[i].x, &pnts[i].y, &pnts[i].z);
#ifdef STATEBLOCK
	AssertFatal(EditTSCtrl::mAlphaBlendLerpSB, "ConsoleMethod -- EditTSCtrl-- renderTriangle -- EditTSCtrl::mAlphaBlendLerpSB cannot be NULL.");
	EditTSCtrl::mAlphaBlendLerpSB->apply();
#else
	GFX->setAlphaBlendEnable( true );
	GFX->setSrcBlend( GFXBlendSrcAlpha );
	GFX->setDestBlend( GFXBlendInvSrcAlpha );
#endif

   // frame
   if( object->mConsoleFrameColor.alpha )
   {
      PrimBuild::color( object->mConsoleFrameColor );

      // TODO: Set GFX line width (when it exists) to the value of 'object->mConsoleLineWidth'

      PrimBuild::begin( GFXLineStrip, 4 );
         PrimBuild::vertex3fv( pnts[0] );
         PrimBuild::vertex3fv( pnts[1] );
         PrimBuild::vertex3fv( pnts[2] );
         PrimBuild::vertex3fv( pnts[0] );
      PrimBuild::end();

      // TODO: Reset GFX line width here
   }

   // fill
   if( object->mConsoleFillColor.alpha )
   {
      PrimBuild::color( object->mConsoleFillColor );

      PrimBuild::begin( GFXTriangleList, 3 );
         PrimBuild::vertex3fv( pnts[0] );
         PrimBuild::vertex3fv( pnts[1] );
         PrimBuild::vertex3fv( pnts[2] );
      PrimBuild::end();
   }
#ifdef STATEBLOCK
	AssertFatal(EditTSCtrl::mAlphaBlendFalseSB, "ConsoleMethod -- EditTSCtrl-- renderTriangle -- EditTSCtrl::mAlphaBlendFalseSB cannot be NULL.");
	EditTSCtrl::mAlphaBlendFalseSB->apply();
#else
   GFX->setAlphaBlendEnable( false );
#endif

}

ConsoleMethod( EditTSCtrl, renderLine, void, 4, 5, "(Point3F start, Point3F end, int width)")
{

   if(!object->mConsoleRendering)
      return;

   if(!object->mConsoleFrameColor.alpha)
      return;

   Point3F start, end;
   dSscanf(argv[2], "%f %f %f", &start.x, &start.y, &start.z);
   dSscanf(argv[3], "%f %f %f", &end.x, &end.y, &end.z);

   S32 lineWidth = object->mConsoleLineWidth;
   if(argc == 5)
      lineWidth = dAtoi(argv[4]);

   // TODO: Set GFX line width (when it exists) to the value of 'lineWidth'

#ifdef STATEBLOCK
	AssertFatal(EditTSCtrl::mAlphaBlendLerpSB, "ConsoleMethod -- EditTSCtrl-- renderLine -- EditTSCtrl::mAlphaBlendLerpSB cannot be NULL.");
	EditTSCtrl::mAlphaBlendLerpSB->apply();
#else
	GFX->setAlphaBlendEnable( true );
	GFX->setSrcBlend( GFXBlendSrcAlpha );
	GFX->setDestBlend( GFXBlendInvSrcAlpha );
#endif

   PrimBuild::color( object->mConsoleFrameColor );
   PrimBuild::begin( GFXLineStrip, 2 );
      PrimBuild::vertex3fv( &start );
      PrimBuild::vertex3fv( &end );
   PrimBuild::end();
#ifdef STATEBLOCK
	AssertFatal(EditTSCtrl::mAlphaBlendTrueSB, "ConsoleMethod -- EditTSCtrl-- renderLine -- EditTSCtrl::mAlphaBlendTrueSB cannot be NULL.");
	EditTSCtrl::mAlphaBlendTrueSB->apply();
#else
	GFX->setAlphaBlendEnable( true );
#endif

   // TODO: Reset GFX line width here
}

void EditTSCtrl::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZFunc, GFXCmpLessEqual);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mSetSB);

	//mSetMisSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->endStateBlock(mSetMisSB);

	//mCullNoneSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mCullNoneSB);

	//mCullStoreSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mCullStoreSB);

	//mZTrueSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->endStateBlock(mZTrueSB);

	//mZFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->endStateBlock(mZFalseSB);

	//mAlphaBlendFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mAlphaBlendFalseSB);

	//mAlphaBlendTrueSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->endStateBlock(mAlphaBlendTrueSB);

	//mAlphaBlendLerpSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->endStateBlock(mAlphaBlendLerpSB);

	//mTSSColorOpSB
	GFX->beginStateBlock();
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPDisable );
	GFX->endStateBlock(mTSSColorOpSB);
}


void EditTSCtrl::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mSetMisSB)
	{
		mSetMisSB->release();
	}

	if (mCullNoneSB)
	{
		mCullNoneSB->release();
	}

	if (mCullStoreSB)
	{
		mCullStoreSB->release();
	}

	if (mZTrueSB)
	{
		mZTrueSB->release();
	}

	if (mZFalseSB)
	{
		mZFalseSB->release();
	}

	if (mAlphaBlendFalseSB)
	{
		mAlphaBlendFalseSB->release();
	}

	if (mAlphaBlendTrueSB)
	{
		mAlphaBlendTrueSB->release();
	}

	if (mAlphaBlendLerpSB)
	{
		mAlphaBlendLerpSB->release();
	}

	if (mTSSColorOpSB)
	{
		mTSSColorOpSB->release();
	}
}

void EditTSCtrl::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		mSetMisSB = new GFXD3D9StateBlock;
		mCullNoneSB = new GFXD3D9StateBlock;
		mCullStoreSB = new GFXD3D9StateBlock;
		mZTrueSB = new GFXD3D9StateBlock;
		mZFalseSB = new GFXD3D9StateBlock;
		mAlphaBlendFalseSB = new GFXD3D9StateBlock;
		mAlphaBlendTrueSB = new GFXD3D9StateBlock;
		mAlphaBlendLerpSB = new GFXD3D9StateBlock;
		mTSSColorOpSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}


}

void EditTSCtrl::shutdown()
{
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mSetMisSB);
	SAFE_DELETE(mCullNoneSB);
	SAFE_DELETE(mCullStoreSB);
	SAFE_DELETE(mZTrueSB);
	SAFE_DELETE(mZFalseSB);
	SAFE_DELETE(mAlphaBlendFalseSB);
	SAFE_DELETE(mAlphaBlendTrueSB);
	SAFE_DELETE(mAlphaBlendLerpSB);
	SAFE_DELETE(mTSSColorOpSB);
}

