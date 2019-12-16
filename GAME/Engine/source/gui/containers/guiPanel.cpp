//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/containers/guiPanel.h"
#include "console/consoleTypes.h"
#include "gfx/primBuilder.h"

//-----------------------------------------------------------------------------
// GuiPanel
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiPanel);

GuiPanel::GuiPanel()
{
   setMinExtent(Point2I(16,16));
   setDocking( Docking::dockNone );
   mColorStart = ColorI(220,220,220);
   mColorEnd   = ColorI(250,250,250);
}

GuiPanel::~GuiPanel()
{
}

void GuiPanel::onRender(Point2I offset, const RectI &updateRect)
{
   RectI ctrlRect = getClientRect();
   ctrlRect.point += offset;

   //*** Draw a gradient left to right, mColor to mColor2
   PrimBuild::begin( GFXTriangleStrip, 4 );
      PrimBuild::color( mColorStart );
      PrimBuild::vertex2i( ctrlRect.point.x, ctrlRect.point.y );
      PrimBuild::vertex2i( ctrlRect.point.x, ctrlRect.point.y + ctrlRect.extent.y );

      PrimBuild::color( mColorEnd );
      PrimBuild::vertex2i( ctrlRect.point.x + ctrlRect.extent.x, ctrlRect.point.y);
      PrimBuild::vertex2i( ctrlRect.point.x + ctrlRect.extent.x, ctrlRect.point.y + ctrlRect.extent.y );
   PrimBuild::end();

   Parent::onRender( offset, updateRect );

}