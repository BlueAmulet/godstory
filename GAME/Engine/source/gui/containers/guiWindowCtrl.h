//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUIWINDOWCTRL_H_
#define _GUIWINDOWCTRL_H_

#ifndef _GUICONTAINER_H_
#include "gui/containers/guiContainer.h"
#endif

/// @addtogroup gui_container_group Containers
///
/// @ingroup gui_group Gui System
/// @{
class GuiWindowCtrl : public GuiContainer
{
   private:
      typedef GuiContainer Parent;

      bool mResizeWidth;
      bool mResizeHeight;
      bool mCanMove;
      bool mCanClose;
      bool mCanMinimize;
      bool mCanMaximize;
	  bool mCanHelp;
      bool mCanDock; ///< Show a docking button on the title bar?
      bool mEdgeSnap; ///< Should this window snap to other windows edges?
	  bool mBoundCheck;  //窗口移动边界检查
      bool mPressClose;
      bool mPressMinimize;
      bool mPressMaximize;
	  bool mPressHelp;
      Point2I mMinSize;

      StringTableEntry mCloseCommand;
	  StringTableEntry mHelpCommand;
      StringTableEntry mText;
      S32 mResizeEdge; ///< Resizing Edges Mask (See Edges Enumeration)

      S32 mTitleHeight;
      F32 mResizeMargin;



      bool mMouseMovingWin;
      bool mMouseResizeWidth;
      bool mMouseResizeHeight;
	  bool mMouseOver;
      bool mMinimized;
      bool mMaximized;

      Point2I mMouseDownPosition;
      RectI mOrigBounds;
      RectI mStandardBounds;

      RectI mCloseButton;
      RectI mMinimizeButton;
      RectI mMaximizeButton;
	  RectI mHelpButton;
      S32 mMinimizeIndex;
      S32 mTabIndex;

      void PositionButtons(void);

   protected:
      enum BitmapIndices
      {
         BmpClose,
         BmpMaximize,
         BmpNormal,
         BmpMinimize,
		 BmpHelp,

         BmpCount
      };
      enum {
         BorderTopLeftKey = 20,
         BorderTopRightKey,
         BorderTopKey,
         BorderTopLeftNoKey,
         BorderTopRightNoKey,
         BorderTopNoKey,
         BorderLeft,
         BorderRight,
         BorderBottomLeft,
         BorderBottom,
         BorderBottomRight,
         NumBitmaps
      };

      enum BitmapStates
      {
         BmpDefault = 0,
         BmpHilite,
         BmpDisabled,
		 BmpOver,

         BmpStates
      };
      RectI *mBitmapBounds;  //bmp is [4*n], bmpHL is [4*n + 1], bmpNA is [4*n + 2]
      GFXTexHandle mTextureObject;


      /// Window Edge Bit Masks
      ///
      /// Edges can be combined to create a mask of multiple edges.  
      /// This is used for hit detection throughout this class.
      enum Edges
      {
         edgeNone   = 0,      ///< No Edge
         edgeTop    = BIT(1), ///< Top Edge
         edgeLeft   = BIT(2), ///< Left Edge
         edgeRight  = BIT(3), ///< Right Edge
         edgeBottom = BIT(4)  ///< Bottom Edge
      };

      void drawWinRect(const RectI &myRect);
   public:
      GuiWindowCtrl();
      DECLARE_CONOBJECT(GuiWindowCtrl);
      static void initPersistFields();

      bool onWake();
      void onSleep();

      bool isMinimized(S32 &index);

      virtual void getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent);

      void setFont(S32 fntTag);

      GuiControl* findHitControl(const Point2I &pt, S32 initialLayer = -1);
      S32 findHitEdges( const Point2I &globalPoint );
      void getSnappableRects( Vector<EdgeRectI> &outVector );
      bool resize(const Point2I &newPosition, const Point2I &newExtent);

      void onMouseDown(const GuiEvent &event);
      void onMouseDragged(const GuiEvent &event);
      void onMouseUp(const GuiEvent &event);
	  void onMouseEnter(const GuiEvent &event);
	  void onMouseLeave(const GuiEvent &event);

      //only cycle tabs through the current window, so overwrite the method
      GuiControl* findNextTabable(GuiControl *curResponder, bool firstCall = true);
      GuiControl* findPrevTabable(GuiControl *curResponder, bool firstCall = true);

      bool onKeyDown(const GuiEvent &event);

      S32 getTabIndex(void) { return mTabIndex; }
      void selectWindow(void);

      void onRender(Point2I offset, const RectI &updateRect);

      ////
      const RectI getClientRect();
};
/// @}

#endif //_GUI_WINDOW_CTRL_H
