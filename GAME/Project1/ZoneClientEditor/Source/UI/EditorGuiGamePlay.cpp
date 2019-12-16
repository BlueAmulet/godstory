//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once
#include "gui/core/guiTypes.h"
#include "sceneGraph/sceneObject.h"
#include "UI/EditorGuiGamePlay.h"
#include "gui/controls/guiBitmapCtrl.h"
#include "gui/controls/guiTextCtrl.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "Effects/ScreenFX.h"
#include "sceneGraph/lightManager.h"
#include "T3D/fx/cameraFXMgr.h"

EditorGuiGamePlay *gEditorGuiGamePlay = NULL;

IMPLEMENT_CONOBJECT(EditorGuiGamePlay);

EditorGuiGamePlay::EditorGuiGamePlay()
{
	gEditorGuiGamePlay = this;
}

EditorGuiGamePlay::~EditorGuiGamePlay()
{
	gEditorGuiGamePlay = NULL;
}

void EditorGuiGamePlay::renderWorld(const RectI &updateRect)
{
	gClientSceneGraph->getLightManager()->hdrPrepare(updateRect.point, updateRect.extent);

	Parent::renderWorld(updateRect);
#ifdef DEBUG
	drawObjectInfo();
#endif

	gClientSceneGraph->getLightManager()->hdrRender();

	g_ScreenFXMgr.SetRect(updateRect);
	g_ScreenFXMgr.Render();

	gCameraShakeEX.Render();
}

void EditorGuiGamePlay::setControlPlayer(Player * pPlayer)
{
	m_pPlayer = pPlayer;
}
void EditorGuiGamePlay::drawObjectInfo()
{

	Point3F screenPos;
	Point3F objectPos;
	char objectName[256] = {0,};
	Point2I objectNameOffset;

	objectPos = m_pPlayer->getRenderPosition(); 
	objectPos.z += m_pPlayer->getWorldBox().len_z();

	if (project(objectPos,&screenPos))
	{
		dSprintf(objectName,sizeof(objectName),"Ts:%d ,S:%d ,Y:%d, O:%d, DP:%d, Tr:%d, Sd:%d, Sa:%d, Sk:%d, Qr:%d, Rc:%02d",
			gClientSceneGraph->staticNum,
			gClientSceneGraph->getInCameraStatics().size(),
			gClientSceneGraph->getInCameraShadows().size(),
			gClientSceneGraph->getInCameraObjects().size(),
			gClientSceneGraph->getDPtimes(),
			gClientSceneGraph->getTrDPtimes(),
			gClientSceneGraph->getSdDPtimes(),
			gClientSceneGraph->getSaDPtimes(),
			gClientSceneGraph->getSkDPtimes(),
			gClientSceneGraph->getQueryTimes(),
			gClientSceneGraph->getRectCount()
			);

		objectNameOffset.x = screenPos.x - ( mProfile->mFont->getStrWidth(objectName) >> 1 );
		objectNameOffset.y = screenPos.y - ( mProfile->mFont->getHeight() + HpOffsetY );
		GFX->getDrawUtil()->drawTextOutline(mProfile->mFont,objectNameOffset,objectName,&mProfile->mFillColor,true,mProfile->mFontOutline);
	}
}