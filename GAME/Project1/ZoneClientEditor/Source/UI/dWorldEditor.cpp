//
// NTianJi Game Engine V1.0
//
//
// Name:
//		dWorldEditor.cpp
//
// Abstract:

#include "UI/dWorldEditor.h"
#include "T3D/gameConnection.h"
#pragma message(ENGINE(原Player类改成GameObject类))
#include "Gameplay/GameObjects/PlayerObject.h"
#include "windowManager/win32/win32Window.h"
#include "Effects/EdgeBlur.h"

IMPLEMENT_CONOBJECT(dWorldEditor);

// 摄像机属性
bool dWorldEditor::m_LockPitch				= true;
F32 dWorldEditor::m_CameraPitchMin			= 0.18f;
F32 dWorldEditor::m_CameraPitchMax			= 0.72f;

F32 dWorldEditor::m_CameraZoomMin			= 7.0f;
F32 dWorldEditor::m_CameraZoomMax			= 32.0f;
F32 dWorldEditor::m_CameraZoomUnit			=
	(dWorldEditor::m_CameraZoomMax - dWorldEditor::m_CameraZoomMin) * 0.08f;
F32 dWorldEditor::m_CameraZoomTime			= 0.5f;

F32 dWorldEditor::m_OriginalCameraFov		= 35.0f;
F32 dWorldEditor::m_CameraFov				= dWorldEditor::m_OriginalCameraFov;	// 根据屏幕长宽比会自动调节

F32 dWorldEditor::m_StartToPitch			= 32.0f;
F32 dWorldEditor::m_PitchRadian				= 0.40f;

F32 dWorldEditor::m_fCameraYaw				= 0;
F32 dWorldEditor::m_fCameraPitch			= dWorldEditor::m_CameraPitchMax;
F32 dWorldEditor::m_fCameraZoom				= 20.0f;
F32 dWorldEditor::m_fCurrentZoom			= dWorldEditor::m_fCameraZoom;
bool dWorldEditor::m_IsFirstPerson			= false;
bool dWorldEditor::m_IsZoomingCamera		= true;
bool dWorldEditor::m_enableRenderShadows	= false;

dWorldEditor::dWorldEditor()
{
}

bool dWorldEditor::onMouseWheelUp(const GuiEvent& event)
{
	Parent::onMouseWheelUp(event);
	ZoomCamera(true);
	return true;
}

bool dWorldEditor::onMouseWheelDown(const GuiEvent& event)
{
	Parent::onMouseWheelDown(event);
	ZoomCamera(false);
	return true;
}

// 设置摄像机角度
void dWorldEditor::SetCameraPitch(F32 val)
{
	if(m_LockPitch)
		return;
	m_fCameraPitch = mClampF(val, m_CameraPitchMin, m_CameraPitchMax);
}
void dWorldEditor::SetCameraYaw(F32 val)
{
	m_fCameraYaw = val;
	//确定最短旋转角度
	while(m_fCameraYaw > M_PI_F)
		m_fCameraYaw -= M_2PI_F;
	while(m_fCameraYaw < -M_PI_F)
		m_fCameraYaw += M_2PI_F;
}

// 获取摄像机角度
F32 dWorldEditor::GetCameraPitch()
{
	return m_fCurrentZoom > m_StartToPitch ? m_fCameraPitch : m_fCameraPitch - (m_StartToPitch-m_fCurrentZoom)/(m_StartToPitch-m_CameraZoomMin)*m_PitchRadian;
}

// 获取摄像机焦点高度
F32 dWorldEditor::GetCurrentFocus(F32 height)
{
	//static bool bb = false;
	//static CEdgeBlur blur;
	//if (!bb)
	//{
	//	blur.Create(100);
	//	blur.SetTexture("~/data/environments/particles/00000");
	//	bb = true;
	//}
	//POINT ptCursor;
	//GetCursorPos( &ptCursor );
	//Point3F min = Point3F(ptCursor.x - 350, ptCursor.y - 150, 0.0f);
	//Point3F max = Point3F(ptCursor.x - 350, ptCursor.y, 0.0f);
	//blur.FrameMove(min, max);
	//blur.Render();
	return mClampF((m_CameraZoomMax-m_fCurrentZoom)/(m_CameraZoomMax-m_CameraZoomMin), 0, 1)*height*0.35f;
}

/// Zoom the camera.
///		@param		in_out			Zoom in - ture; zoom out - false.
void dWorldEditor::ZoomCamera(bool in_out)
{
	if(m_IsFirstPerson)
		return;
	m_IsZoomingCamera = true;

	if(in_out)
		m_fCameraZoom -= m_CameraZoomUnit;
	else
		m_fCameraZoom += m_CameraZoomUnit;

	m_fCameraZoom = mClampF(m_fCameraZoom, m_CameraZoomMin, m_CameraZoomMax);
}

void dWorldEditor::SetCameraZoom(F32 zoom_camera)
{
	if(m_IsFirstPerson)
		return;
	m_IsZoomingCamera = true;
	m_fCameraZoom = mClampF(zoom_camera, m_CameraZoomMin, m_CameraZoomMax);
}

void dWorldEditor::SetCameraFov(Point2I& size)
{
	F32 tmp = ((F32)size.x/(F32)size.y) / ((F32)Win32Window::getMinimum().x/(F32)Win32Window::getMinimum().y);
	m_CameraFov = m_OriginalCameraFov * tmp;
}

void dWorldEditor::UpdateZoom(F32 dt, F32& CurrentZoom)
{
	if(!m_IsZoomingCamera)
		return;

	if(mFabs(CurrentZoom - m_fCameraZoom) <= 0.03f)
	{
		m_IsZoomingCamera = false;
	}
	else
	{
		F32 zoom_speed = mClampF(dt / m_CameraZoomTime, 0, 1) * (m_fCameraZoom - CurrentZoom);
		CurrentZoom += zoom_speed;
		m_fCurrentZoom = CurrentZoom;
	}
}

void dWorldEditor::initPersistFields()
{
	Parent::initPersistFields();

	Con::addVariable("$Editor::CameraFov", TypeF32, &dWorldEditor::m_OriginalCameraFov);
	Con::addVariable("$Editor::LockPitch", TypeBool, &dWorldEditor::m_LockPitch);
	Con::addVariable("$Editor::CameraPitchMin", TypeF32, &dWorldEditor::m_CameraPitchMin);
	Con::addVariable("$Editor::CameraPitchMax", TypeF32, &dWorldEditor::m_CameraPitchMax);
	Con::addVariable("$Editor::CameraPitch", TypeF32, &dWorldEditor::m_fCameraPitch);
	Con::addVariable("$Editor::StartToPitch", TypeF32, &dWorldEditor::m_StartToPitch);
	Con::addVariable("$Editor::PitchRadian", TypeF32, &dWorldEditor::m_PitchRadian);
	Con::addVariable("$Editor::CameraZoomMax", TypeF32, &dWorldEditor::m_CameraZoomMax);
	Con::addVariable("$Editor::CameraZoomMin", TypeF32, &dWorldEditor::m_CameraZoomMin);
	Con::addVariable("$Editor::CameraZoomUnit", TypeF32, &dWorldEditor::m_CameraZoomUnit);
	Con::addVariable("$Editor::CameraZoomTime", TypeF32, &dWorldEditor::m_CameraZoomTime);
	Con::addVariable("$Editor::enableRenderShadows", TypeBool, &dWorldEditor::m_enableRenderShadows);
	
}

// ====================================================================================
//  导出脚本函数
// ====================================================================================

ConsoleFunction(GetCameraPitch, F32, 1, 1, "obj.GetCameraPitch();")
{
	return dWorldEditor::GetCameraPitch();
}

ConsoleFunction(SetCameraPitch, void, 2, 2, "obj.SetCameraPitch();")
{
	dWorldEditor::SetCameraPitch(dAtof(argv[1]));
}

ConsoleFunction(GetCameraYaw, F32, 1, 1, "obj.GetCameraYaw();")
{
	return dWorldEditor::GetCameraYaw();
}

ConsoleFunction(SetCameraYaw, void, 2, 2, "obj.SetCameraYaw();")
{
	dWorldEditor::SetCameraYaw(dAtof(argv[1]));
}

ConsoleFunction(GetCameraZoom,F32,1,1,"obj.GetCameraZoom();")
{
	return dWorldEditor::GetCameraZoom();
};

ConsoleFunction(ShowAllPlayer, void, 1, 1, "ShowAllPlayer()")
{
	S32 iCount = Sim::getClientGroup()->size();
	Con::warnf("----------- Player Info (Total: %d Players)-----------\n", iCount);
	Con::warnf("ConnID	PlayObjID	PlayerID	PlayerName	  Level   Exp   EnterGameTime\n");
	for(S32 i = 0; i < iCount; i++)
	{
		GameConnection* client = dynamic_cast<GameConnection*> ((*Sim::getClientGroup())[i]);
		GameBase*  controlobject = NULL;
		Player* player = NULL;
		if(client && (controlobject = client->getControlObject()) && (player = dynamic_cast<Player*>(controlobject)))
		{			
			Con::warnf(" %d    %d		%d		%s********\n",
				client->getId(), controlobject->getId(), player->getPlayerID(), player->getName()/*,player->GetLevel(), player->GetExp()*/);
		}
	}
}

extern F32 GameGetCameraFov();
ConsoleFunction(processPitch, void, 2, 2, "processPitch(%val);")
{
	F32 val = atof(argv[1]);
	val *= (GameGetCameraFov() / 90) * 0.01;
	MoveManager::mPitch += val;
	dWorldEditor::SetCameraPitch( dWorldEditor::GetCameraPitch()+val);
}

ConsoleFunction(processYaw, void, 2, 2, "obj.processYaw(%val);")
{
	F32 val = atof(argv[1]);
	val *= (GameGetCameraFov() / 90) * 0.01;

	if(MoveManager::mFreeLook == 0)
      MoveManager::mYaw += val;
   else
      dWorldEditor::SetCameraYaw( dWorldEditor::GetCameraYaw()+val);
}