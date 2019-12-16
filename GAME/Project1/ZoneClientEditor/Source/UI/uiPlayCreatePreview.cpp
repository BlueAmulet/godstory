//
// PowerEngine V1.0
//
//
// Name:
//		uiPlayCreatePreview.cpp
//
// Abstract:

#include "ts/tsShape.h"
#include "console/consoleTypes.h"
#include "T3D/gameFunctions.h"
#include "UI/uiPlayCreatePreview.h"
#include "GamePlay/ClientGameplayState.h"


IMPLEMENT_CONOBJECT(uiPlayCreatePreview);


uiPlayCreatePreview::uiPlayCreatePreview()
{
}

bool uiPlayCreatePreview::onAdd()
{
	if(!Parent::onAdd())
		return false;

	return true;
}

bool uiPlayCreatePreview::onWake()
{
	if(!Parent::onWake())
		return false;
	return true;
}

void uiPlayCreatePreview::onSleep()
{
	Parent::onSleep();
}

void uiPlayCreatePreview::onMouseDown(const GuiEvent& event)
{
}

// ====================================================================================
//  äÖÈ¾º¯Êý
// ====================================================================================
bool uiPlayCreatePreview::processCameraQuery(CameraQuery *query)
{
	return true;
}

void uiPlayCreatePreview::onRender(Point2I offset, const RectI &updateRect)
{

}

void uiPlayCreatePreview::onPreRender()
{
	Parent::onPreRender();
}

void uiPlayCreatePreview::renderWorld(const RectI &updateRect)
{
	GameRenderWorld();

	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;

	GFX->setClipRect(updateRect);
}

void uiPlayCreatePreview::initPersistFields()
{
	Parent::initPersistFields();
}

