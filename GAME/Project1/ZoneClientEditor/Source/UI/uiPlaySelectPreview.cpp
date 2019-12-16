//
// PowerEngine V1.0
//
//
// Name:
//		uiPlaySelectPreview.cpp
//
// Abstract:

#include "ts/tsShape.h"
#include "console/consoleTypes.h"
#include "T3D/gameFunctions.h"
#include "UI/uiPlaySelectPreview.h"
#include "GamePlay/ClientGameplayState.h"


IMPLEMENT_CONOBJECT(uiPlaySelectPreview);


uiPlaySelectPreview::uiPlaySelectPreview()
{
}

bool uiPlaySelectPreview::onAdd()
{
	if(!Parent::onAdd())
		return false;

	return true;
}

bool uiPlaySelectPreview::onWake()
{
	if(!Parent::onWake())
		return false;
	return true;
}

void uiPlaySelectPreview::onSleep()
{
	Parent::onSleep();
}

void uiPlaySelectPreview::onMouseDown(const GuiEvent& event)
{
}

// ====================================================================================
//  äÖÈ¾º¯Êý
// ====================================================================================
bool uiPlaySelectPreview::processCameraQuery(CameraQuery *query)
{
	return true;
}

void uiPlaySelectPreview::onRender(Point2I offset, const RectI &updateRect)
{

}

void uiPlaySelectPreview::onPreRender()
{
	Parent::onPreRender();
}

void uiPlaySelectPreview::renderWorld(const RectI &updateRect)
{
	GameRenderWorld();

	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(!player)
		return;

	GFX->setClipRect(updateRect);
}

void uiPlaySelectPreview::initPersistFields()
{
	Parent::initPersistFields();
}

