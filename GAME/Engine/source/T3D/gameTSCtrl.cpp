//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "T3D/gameTSCtrl.h"
#include "console/consoleTypes.h"
#include "T3D/gameBase.h"
#include "T3D/gameConnection.h"
#include "T3D/shapeBase.h"
#include "T3D/gameFunctions.h"
#include "UI/dWorldEditor.h"

//---------------------------------------------------------------------------
// Debug stuff:
Point3F lineTestStart = Point3F(0, 0, 0);
Point3F lineTestEnd =   Point3F(0, 1000, 0);
Point3F lineTestIntersect =  Point3F(0, 0, 0);
bool gSnapLine = false;

//----------------------------------------------------------------------------
// Class: GameTSCtrl
//----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GameTSCtrl);

GameTSCtrl::GameTSCtrl()
{
}


//---------------------------------------------------------------------------
bool GameTSCtrl::processCameraQuery(CameraQuery *camq)
{
   GameUpdateCameraFov();
   return GameProcessCameraQuery(camq);
}

//---------------------------------------------------------------------------
void GameTSCtrl::renderWorld(const RectI &updateRect)
{
   GameRenderWorld();
   GFX->setClipRect(updateRect);
}

//---------------------------------------------------------------------------
bool GameTSCtrl::onMouseWheelUp(const GuiEvent& event)
{
	Parent::onMouseWheelUp(event);
#ifdef NTJ_EDITOR
	dWorldEditor::ZoomCamera(true);
#endif
	return true;
}

bool GameTSCtrl::onMouseWheelDown(const GuiEvent& event)
{
	Parent::onMouseWheelDown(event);
#ifdef NTJ_EDITOR
	dWorldEditor::ZoomCamera(false);
#endif
	return true;
}

void GameTSCtrl::onMouseMove(const GuiEvent &evt)
{
   if(gSnapLine)
      return;

   MatrixF mat;
   Point3F vel;
   if ( GameGetCameraTransform(&mat, &vel) )
   {
      Point3F pos;
      mat.getColumn(3,&pos);
      Point3F screenPoint((F32)evt.mousePoint.x, (F32)evt.mousePoint.y, -1.0f);
      Point3F worldPoint;
      if (unproject(screenPoint, &worldPoint)) {
         Point3F vec = worldPoint - pos;
         lineTestStart = pos;
         vec.normalizeSafe();
         lineTestEnd = pos + vec * 1000;
      }
   }
}

void GameTSCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   // check if should bother with a render
   GameConnection * con = GameConnection::getConnectionToServer();
   bool skipRender = !con || (con->getWhiteOut() >= 1.f) || (con->getDamageFlash() >= 1.f) || (con->getBlackOut() >= 1.f);

   if(!skipRender || true)
      Parent::onRender(offset, updateRect);

   GFX->setViewport( updateRect );
   CameraQuery camq;
   if(GameProcessCameraQuery(&camq))
      GameRenderFilters(camq);
}

//--------------------------------------------------------------------------
ConsoleFunction( snapToggle, void, 1, 1, "()" )
{
   gSnapLine = !gSnapLine;
}
