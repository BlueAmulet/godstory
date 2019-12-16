//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GAMETSCTRL_H_
#define _GAMETSCTRL_H_

#ifndef _GAME_H_
#include "app/game.h"
#endif
#ifndef _GUITSCONTROL_H_
#include "gui/3d/guiTSControl.h"
#endif

class ProjectileData;
class GameBase;

//----------------------------------------------------------------------------
class GameTSCtrl : public GuiTSCtrl
{
private:
   typedef GuiTSCtrl Parent;

public:
   GameTSCtrl();

   bool processCameraQuery(CameraQuery *query);
   void renderWorld(const RectI &updateRect);

   void onMouseMove(const GuiEvent &evt);
   bool onMouseWheelUp(const GuiEvent& event);
   bool onMouseWheelDown(const GuiEvent& event);
   void onRender(Point2I offset, const RectI &updateRect);

   DECLARE_CONOBJECT(GameTSCtrl);
};

#endif
