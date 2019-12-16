//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _EDITTSCTRL_H_
#define _EDITTSCTRL_H_

#ifndef _GUITSCONTROL_H_
#include "gui/3d/guiTSControl.h"
#endif

#include "terrain/terrData.h"

struct Gui3DMouseEvent : public GuiEvent
{
   Point3F     vec;
   Point3F     pos;
};

class EditManager;
class GFXStateBlock;

#pragma message(ENGINE(编辑器视角控制))
#ifdef NTJ_EDITOR
class dWorldEditor;
#endif
#ifdef NTJ_CLIENT
class dWorldEditor;
#endif

class EditTSCtrl : public GuiTSCtrl
{
   private:
      typedef GuiTSCtrl Parent;
#ifdef NTJ_EDITOR
	  friend class dWorldEditor;
#endif
#ifdef NTJ_CLIENT
	  friend class dWorldEditor;
#endif

      // EditTSCtrl
      void make3DMouseEvent(Gui3DMouseEvent & gui3Devent, const GuiEvent &event);
      virtual void getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent);
      virtual void onMouseUp(const GuiEvent & event);
      virtual void onMouseDown(const GuiEvent & event);
      virtual void onMouseMove(const GuiEvent & event);
      virtual void onMouseDragged(const GuiEvent & event);
      virtual void onMouseEnter(const GuiEvent & event);
      virtual void onMouseLeave(const GuiEvent & event);
      virtual void onRightMouseDown(const GuiEvent & event);
      virtual void onRightMouseUp(const GuiEvent & event);
      virtual void onRightMouseDragged(const GuiEvent & event);
      virtual bool onInputEvent(const InputEventInfo & event);
      virtual bool onMouseWheelUp(const GuiEvent &event);
      virtual bool onMouseWheelDown(const GuiEvent &event);


      virtual void updateGuiInfo() {};
      virtual void renderScene(const RectI &){};
      void renderMissionArea();
      virtual void renderCameraAxis();
      virtual void renderGrid();

      // GuiTSCtrl
      void renderWorld(const RectI & updateRect);

   protected:
      enum DisplayType
      {
         DisplayTypeTop,
         DisplayTypeBottom,
         DisplayTypeFront,
         DisplayTypeBack,
         DisplayTypeLeft,
         DisplayTypeRight,
         DisplayTypePerspective,
         DisplayTypeIsometric,
      };

      S32      mDisplayType;
      F32      mOrthoFOV;
      Point3F  mOrthoCamTrans;
      EulerF   mIsoCamRot;
      Point3F  mIsoCamRotCenter;
      F32      mIsoCamAngle;
      Point3F  mRawCamPos;
      Point2I  mLastMousePos;
      bool     mLastMouseClamping;
      TerrainBlock* mActiveTerrain;

      bool     mAllowBorderMove;
      S32      mMouseMoveBorder;
      F32      mMouseMoveSpeed;
      U32      mLastBorderMoveTime;

      Gui3DMouseEvent   mLastEvent;
      bool              mLeftMouseDown;
      bool              mRightMouseDown;
      bool              mMouseLeft;

   public:

      EditTSCtrl();
      ~EditTSCtrl();

      // SimObject
      bool onAdd();
      bool onWake();

      //
      bool        mRenderMissionArea;
      ColorI      mMissionAreaFillColor;
      ColorI      mMissionAreaFrameColor;

      //
      ColorI            mConsoleFrameColor;
      ColorI            mConsoleFillColor;
      S32               mConsoleSphereLevel;
      S32               mConsoleCircleSegments;
      S32               mConsoleLineWidth;

      static void initPersistFields();
      static void consoleInit();

      //
      bool              mConsoleRendering;
      bool              mRightMousePassThru;

      // all editors will share a camera
      static Point3F    smCamPos;
      static MatrixF    smCamMatrix;
      static bool       smCamOrtho;
      static F32        smCamFarPlane;
      static F32        smVisibleDistance;

      static U32        smSceneBoundsMask;
      static Point3F    smMinSceneBounds;

      bool              mRenderGridPlane;
      ColorI            mGridPlaneColor;
      F32               mGridPlaneSize;
      F32               mGridPlaneSizePixelBias;
      S32               mGridPlaneMinorTicks;
      ColorI            mGridPlaneMinorTickColor;
      ColorI            mGridPlaneOriginColor;

      // GuiTSCtrl
      bool processCameraQuery(CameraQuery * query);

      // guiControl
      virtual void onRender(Point2I offset, const RectI &updateRect);
      virtual void on3DMouseUp(const Gui3DMouseEvent &){};
      virtual void on3DMouseDown(const Gui3DMouseEvent &){};
      virtual void on3DMouseMove(const Gui3DMouseEvent &){};
      virtual void on3DMouseDragged(const Gui3DMouseEvent &){};
      virtual void on3DMouseEnter(const Gui3DMouseEvent &){};
      virtual void on3DMouseLeave(const Gui3DMouseEvent &){};
      virtual void on3DRightMouseDown(const Gui3DMouseEvent &){};
      virtual void on3DRightMouseUp(const Gui3DMouseEvent &){};
      virtual void on3DRightMouseDragged(const Gui3DMouseEvent &){};
      virtual void on3DMouseWheelUp(const Gui3DMouseEvent &){};
      virtual void on3DMouseWheelDown(const Gui3DMouseEvent &){};
      virtual void get3DCursor(GuiCursor *&cursor, bool &visible, const Gui3DMouseEvent &);

      virtual S32 getDisplayType() {return mDisplayType;}
      virtual void setDisplayType(S32 type) {mDisplayType = type;}

      virtual void calcOrthoCamOffset(F32 mousex, F32 mousey, U8 modifier=0);

		DECLARE_CONOBJECT(EditTSCtrl);
private:
	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mSetMisSB;
	//cull
	static GFXStateBlock* mCullNoneSB;
	static GFXStateBlock* mCullStoreSB;
	//z enable
	static GFXStateBlock* mZTrueSB;
	static GFXStateBlock* mZFalseSB;

public:
	//alpha blend 
	static GFXStateBlock* mAlphaBlendFalseSB;
	static GFXStateBlock* mAlphaBlendTrueSB;
	static GFXStateBlock* mAlphaBlendLerpSB;
	//tex stage color op
	static GFXStateBlock* mTSSColorOpSB;

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

#endif
