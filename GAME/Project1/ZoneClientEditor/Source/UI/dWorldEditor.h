
//
// NTianJi Game Engine V1.0
//
//
// Name:
//		dWorldEditor.h
//
// Abstract:
//

#pragma once

#include "sceneGraph/sceneObject.h"
#include "gui/core/guiCanvas.h"
#include "gui/missionEditor/WorldEditor.h"
#include "T3D/moveManager.h"
#include "platform/platform.h"
#include "math/mPoint.h"


// ========================================================================================================================================
// 主要用于视角控制，方便场景制作，注意应和dGuiMouseGamePlay保持一致
// ========================================================================================================================================
class dWorldEditor: public WorldEditor
{
	typedef WorldEditor Parent;
private:
	static bool			m_LockPitch;
	static F32			m_CameraPitchMin;
	static F32			m_CameraPitchMax;

	// 固定视角时，拉近视角摄像机渐平的设置
	static F32			m_StartToPitch;				// 开始变平的位置（在Zoom取值范围内）
	static F32			m_PitchRadian;				// 最大pitch角度

	static F32			m_fCurrentZoom;				// 当前zoom
	static F32			m_CameraZoomMax;
	static F32			m_CameraZoomMin;
	static F32			m_CameraZoomUnit;
	static F32			m_CameraZoomTime;			///< Total time for camera zoom.

	static F32			m_OriginalCameraFov;
	static F32			m_CameraFov;

	static F32			m_fCameraYaw;
	static F32			m_fCameraPitch;
	static F32			m_fCameraZoom;

	static bool			m_IsFirstPerson;			// 与gameConnection::mFirstPerson保持一致
	static bool			m_IsZoomingCamera;
public:
	static bool			m_enableRenderShadows;

public:
	DECLARE_CONOBJECT(dWorldEditor);
	static void initPersistFields();

	dWorldEditor();

	void UpdateMouseState	(const Point2I& pos);

	/// @name		Event Handlers
	/// @{

	bool onMouseWheelUp			(const GuiEvent& event);
	bool onMouseWheelDown		(const GuiEvent& event);

	/// @}


	/// @name Query Functions
	/// @{
	static F32	GetCameraZoomMin()			{return m_CameraZoomMin;}
	static F32	GetCameraZoomMax()			{return m_CameraZoomMax;}
	static F32	GetCameraPitchMin()			{return m_CameraPitchMax;}
	static F32	GetCameraPitchMax()			{return m_CameraPitchMin;}
	static F32	GetCameraFov()				{return m_CameraFov;}
	/// @}

	// 摄像机设置函数
	static F32	GetCameraPitch();
	static F32	GetCameraYaw()				{return m_fCameraYaw;}
	static F32	GetCameraZoom()				{return m_fCameraZoom;}
	static F32	GetCurrentZoom()			{return m_fCurrentZoom;}
	static F32	GetCurrentFocus(F32 height);
	static void	SetCameraPitch(F32 val);
	static void	SetCameraYaw(F32 val);
	static void	SetCameraZoom(F32 zoom_camera);
	static void	SetCameraFov(Point2I& size);

	static bool	IsFirstPerson()				{return m_IsFirstPerson;}
	static void	SetFirstPerson(bool val)	{m_IsFirstPerson = val;}

	static bool	IsZoomingCamera()			{return m_IsZoomingCamera;}
	static void	SetZoomingCamera(bool val)	{m_IsZoomingCamera = val;}

	/// @name zoom
	/// @{
	static void	ZoomCamera(bool in_out);
	static void	UpdateZoom(F32 dt, F32& CurrentZoom);
	/// @}

};

