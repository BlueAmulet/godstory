//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/3d/guiTSControl.h"
#include "console/consoleTypes.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/lightManager.h"
#include "gfx/debugDraw.h"
#include "math/mathUtils.h"

IMPLEMENT_CONOBJECT(GuiTSCtrl);

U32 GuiTSCtrl::smFrameCount = 0;

GuiTSCtrl::GuiTSCtrl()
{
   mCameraZRot = 0;
   mForceFOV = 0;

   mSaveModelview.identity();
   mSaveProjection.identity();

   mLastCameraQuery.ortho = false;
}
void GuiTSCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addField("cameraZRot", TypeF32, Offset(mCameraZRot, GuiTSCtrl));
   addField("forceFOV",   TypeF32, Offset(mForceFOV,   GuiTSCtrl));
}

void GuiTSCtrl::consoleInit()
{
   Con::addVariable("$TSControl::frameCount", TypeS32, &smFrameCount);
}

void GuiTSCtrl::onPreRender()
{
   setUpdate();
}

bool GuiTSCtrl::processCameraQuery(CameraQuery *)
{
   return false;
}

void GuiTSCtrl::renderWorld(const RectI& /*updateRect*/)
{
}

// -------------------------------------------------------------------
// World -> Screen
// Input z coord is depth, from 0 to 1
// -------------------------------------------------------------------
bool GuiTSCtrl::project(const Point3F &pt, Point3F *dest)
{
   return MathUtils::projectWorldToScreen(pt,*dest,mSaveViewport,mSaveModelview,mSaveProjection);
}

// -------------------------------------------------------------------
// Screen -> World
// Ouput z coord is depth, from 0 to 1
// -------------------------------------------------------------------
bool GuiTSCtrl::unproject(const Point3F &pt, Point3F *dest)
{
   MathUtils::projectScreenToWorld(pt,*dest,mSaveViewport,mSaveModelview,mSaveProjection,mLastCameraQuery.farPlane,mLastCameraQuery.nearPlane);
   return true;
}

void GuiTSCtrl::setupCamera(Point2I offset, const RectI &updateRect)
{
	if(!processCameraQuery(&mLastCameraQuery))
		return;

	// Clear the zBuffer so GUI doesn't hose object rendering accidentally
	GFX->clear( GFXClearZBuffer , ColorI(20,20,20), 1.0f, 0 );

	if(mForceFOV != 0)
		mLastCameraQuery.fov = mDegToRad(mForceFOV);

	if(mCameraZRot)
	{
		MatrixF rotMat(EulerF(0, 0, mDegToRad(mCameraZRot)));
		mLastCameraQuery.cameraMatrix.mul(rotMat);
	}

	// set up the camera and viewport stuff:
	F32 wwidth;
	F32 wheight;
	if(!mLastCameraQuery.ortho)
	{
		wwidth = mLastCameraQuery.nearPlane * mTan(mLastCameraQuery.fov / 2);
		wheight = F32(getHeight()) / F32(getWidth()) * wwidth;
	}
	else
	{
		wwidth = mLastCameraQuery.fov;
		wheight = F32(getHeight()) / F32(getWidth()) * wwidth;
	}

	F32 hscale = wwidth * 2 / F32(getWidth());
	F32 vscale = wheight * 2 / F32(getHeight());

	F32 left = (updateRect.point.x - offset.x) * hscale - wwidth;
	F32 right = (updateRect.point.x + updateRect.extent.x - offset.x) * hscale - wwidth;
	F32 top = wheight - vscale * (updateRect.point.y - offset.y);
	F32 bottom = wheight - vscale * (updateRect.point.y + updateRect.extent.y - offset.y);

	GFX->setViewport( updateRect );

	if(!mLastCameraQuery.ortho)
	{
		GFX->setFrustum( left, right, bottom, top,
			mLastCameraQuery.nearPlane, mLastCameraQuery.farPlane );
	}
	else
	{
		GFX->setOrtho(left, right, bottom, top, mLastCameraQuery.nearPlane, mLastCameraQuery.farPlane, true);

		mOrthoWidth = right - left;
		mOrthoHeight = top - bottom;
	}

	// We're going to be displaying this render at size of this control in
	// pixels - let the scene know so that it can calculate e.g. reflections
	// correctly for that final display result.
	gClientSceneGraph->setDisplayTargetResolution(getExtent());

	gClientSceneGraph->setVisibleDistance( mLastCameraQuery.farPlane );
	gClientSceneGraph->setStaticObjDistance( mLastCameraQuery.staticDist );

	gClientSceneGraph->getLightManager()->hdrPrepare(updateRect.point, updateRect.extent);

	// save the world matrix before attempting to draw anything
	//GFX->pushWorldMatrix();

	// Set the GFX world matrix to the world-to-camera transform, but don't 
	// change the cameraMatrix in mLastCameraQuery. This is because 
	// mLastCameraQuery.cameraMatrix is supposed to contain the camera-to-world
	// transform. In-place invert would save a copy but mess up any GUIs that
	// depend on that value.
	MatrixF worldToCamera = mLastCameraQuery.cameraMatrix;
	worldToCamera.inverse();
	GFX->setWorldMatrix( worldToCamera );

	mSaveProjection = GFX->getProjectionMatrix();
	mSaveModelview = GFX->getWorldMatrix();

	mSaveViewport = updateRect;
}

// -------------------------------------------------------------------
// onRender
// -------------------------------------------------------------------
void GuiTSCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   if(!processCameraQuery(&mLastCameraQuery))
      return;

   // Clear the zBuffer so GUI doesn't hose object rendering accidentally
   GFX->clear( GFXClearZBuffer , ColorI(20,20,20), 1.0f, 0 );

   if(mForceFOV != 0)
      mLastCameraQuery.fov = mDegToRad(mForceFOV);

   if(mCameraZRot)
   {
      MatrixF rotMat(EulerF(0, 0, mDegToRad(mCameraZRot)));
      mLastCameraQuery.cameraMatrix.mul(rotMat);
   }

   // set up the camera and viewport stuff:
   F32 wwidth;
   F32 wheight;
   if(!mLastCameraQuery.ortho)
   {
      wwidth = mLastCameraQuery.nearPlane * mTan(mLastCameraQuery.fov / 2);
      wheight = F32(getHeight()) / F32(getWidth()) * wwidth;
   }
   else
   {
      wwidth = mLastCameraQuery.fov;
      wheight = F32(getHeight()) / F32(getWidth()) * wwidth;
   }

   F32 hscale = wwidth * 2 / F32(getWidth());
   F32 vscale = wheight * 2 / F32(getHeight());

   F32 left = (updateRect.point.x - offset.x) * hscale - wwidth;
   F32 right = (updateRect.point.x + updateRect.extent.x - offset.x) * hscale - wwidth;
   F32 top = wheight - vscale * (updateRect.point.y - offset.y);
   F32 bottom = wheight - vscale * (updateRect.point.y + updateRect.extent.y - offset.y);

   GFX->setViewport( updateRect );

   if(!mLastCameraQuery.ortho)
   {
      GFX->setFrustum( left, right, bottom, top,
                       mLastCameraQuery.nearPlane, mLastCameraQuery.farPlane );
   }
   else
   {
      GFX->setOrtho(left, right, bottom, top, mLastCameraQuery.nearPlane, mLastCameraQuery.farPlane, true);

      mOrthoWidth = right - left;
      mOrthoHeight = top - bottom;
   }

   // We're going to be displaying this render at size of this control in
   // pixels - let the scene know so that it can calculate e.g. reflections
   // correctly for that final display result.
   gClientSceneGraph->setDisplayTargetResolution(getExtent());

   gClientSceneGraph->setVisibleDistance( mLastCameraQuery.farPlane );
   gClientSceneGraph->setStaticObjDistance( mLastCameraQuery.staticDist );

   // save the world matrix before attempting to draw anything
   //GFX->pushWorldMatrix();

   // Set the GFX world matrix to the world-to-camera transform, but don't 
   // change the cameraMatrix in mLastCameraQuery. This is because 
   // mLastCameraQuery.cameraMatrix is supposed to contain the camera-to-world
   // transform. In-place invert would save a copy but mess up any GUIs that
   // depend on that value.
   MatrixF worldToCamera = mLastCameraQuery.cameraMatrix;
   worldToCamera.inverse();
   GFX->setWorldMatrix( worldToCamera );

   mSaveProjection = GFX->getProjectionMatrix();
   mSaveModelview = GFX->getWorldMatrix();

   mSaveViewport = updateRect;

   renderWorld(updateRect);

   // restore the world matrix so the GUI will render correctly
   //GFX->popWorldMatrix();

   renderChildControls(offset, updateRect);
   smFrameCount++;
}

void GuiTSCtrl::onRemove()
{
   Parent::onRemove();
}
