//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUITSCONTROL_H_
#define _GUITSCONTROL_H_

#ifndef _GUICONTAINER_H_
#include "gui/containers/guiContainer.h"
#endif
#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

struct CameraQuery
{
   SimObject*  object;
   F32         nearPlane;
   F32         farPlane;
   F32         fov;
   F32         staticDist;
   bool        ortho;
   MatrixF     cameraMatrix;
};

class GuiTSCtrl : public GuiContainer
{
   typedef GuiContainer Parent;

   MatrixF     mSaveModelview;
   MatrixF     mSaveProjection;
   RectI       mSaveViewport;


   static U32     smFrameCount;
   F32            mCameraZRot;
   F32            mForceFOV;

protected:
   F32            mOrthoWidth;
   F32            mOrthoHeight;

public:
   CameraQuery    mLastCameraQuery;
   GuiTSCtrl();

   void onPreRender();
   void onRender(Point2I offset, const RectI &updateRect);
   virtual bool processCameraQuery(CameraQuery *query);
   virtual void renderWorld(const RectI &updateRect);
   void setupCamera(Point2I offset, const RectI &updateRect);

   static void initPersistFields();
   static void consoleInit();

   virtual void onRemove();

   bool project(const Point3F &pt, Point3F *dest); // returns screen space X,Y and Z for world space point
   bool unproject(const Point3F &pt, Point3F *dest); // returns world space point for X, Y and Z

   DECLARE_CONOBJECT(GuiTSCtrl);
};

#endif
