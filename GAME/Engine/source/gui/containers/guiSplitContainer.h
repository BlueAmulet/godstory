//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _GUI_SPLTCONTAINER_H_
#define _GUI_SPLTCONTAINER_H_

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#ifndef _GUICONTAINER_H_
#include "gui/containers/guiContainer.h"
#endif

#ifndef _GUI_PANEL_H_
#include "gui/containers/guiPanel.h"
#endif

#ifndef _PLATFORMINPUT_H_
#include "platform/platformInput.h"
#endif


/// @brief
enum SplitterType
{
   splitterHorizontal = 0,
   splitterVertical,
};

/// @addtogroup gui_container_group Containers
///
/// @ingroup gui_group Gui System
/// @{
class  GuiSplitContainer : public GuiContainer
{
   // Parent TypeDef
   typedef GuiContainer Parent;
protected:
   S32         mFixedPanel; 
   S32         mFixedPanelSize;
   S32         mSplitterType;
   S32         mSplitterSize;
   Point2I     mSplitPoint;
   RectI       mSplitRect;
   bool        mDragging;
public:

   enum FixedPanel
   {
      panelFirst = 1,
      panelSecond
   };

   DECLARE_CONOBJECT(GuiSplitContainer);

   // Constructor/Destructor/Conobject Declaration
   GuiSplitContainer();
   ~GuiSplitContainer();

   virtual bool onAdd();
   virtual bool onWake();

   virtual bool layoutControls( RectI &clientRect );

   // Cursor Switching
   virtual void getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent);

   // Splitter
   virtual inline Point2I getSplitPoint() { return mSplitPoint; };

   /// The Splitters entire Client Rectangle, this takes into account padding of this control
   virtual inline RectI   getSplitRect() { return mSplitRect; };

   // GuiControl Inherited
   virtual void onMouseDown(const GuiEvent &event);
   virtual void onMouseUp(const GuiEvent &event);
   virtual void onMouseDragged(const GuiEvent &event); 

   virtual void solvePanelConstraints( Point2I newDragPos, GuiContainer * firstPanel, GuiContainer * secondPanel, RectI clientRect );
   virtual void parentResized(const RectI &oldParentRect, const RectI &newParentRect);
   virtual Point2I   getMinExtent();
   virtual bool resize( const Point2I &newPosition, const Point2I &newExtent );
   static void initPersistFields();

   // Rendering
   void onRender(Point2I offset, const RectI &updateRect);

};
/// @}

#endif