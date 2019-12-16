
//
// NTianJi Game Engine V1.0
//
//
// Name:
//		UIMouseGamePlay_inline.h
//
// Abstract:
//

#pragma once

#include "dGuiMouseGamePlay.h"


//inline CDecalData* dGuiMouseGamePlay::GetMouseDownDecal()
//{
//	return m_MouseDownDecal;
//}

inline F32 dGuiMouseGamePlay::GetCameraFov()
{
	return m_CameraFov;
}

inline F32 dGuiMouseGamePlay::GetCameraZoomMin()
{
	return m_CameraZoomMin;
}

inline F32 dGuiMouseGamePlay::GetCameraZoomMax()
{
	return m_CameraZoomMax;
}

inline F32 dGuiMouseGamePlay::GetCameraPitchMin()
{
	return m_CameraPitchMin;
}

inline F32 dGuiMouseGamePlay::GetCameraPitchMax()
{
	return m_CameraPitchMax;
}

