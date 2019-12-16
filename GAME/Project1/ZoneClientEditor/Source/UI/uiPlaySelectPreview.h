
//
// PowerEngine V1.0
//
//
// Name:
//		uiPlaySelectPreview.h
//
// Abstract:
//

#pragma once

#include "T3D/GameTSCtrl.h"
#include "platform/platform.h"
#include "Gameplay/GameObjects/PlayerObject.h"

// ========================================================================================================================================
//  uiPlaySelectPreview
// ����ؼ�����Ϸ������ѡ�����
// ========================================================================================================================================
class uiPlaySelectPreview: public GameTSCtrl
{
	typedef GameTSCtrl Parent;

public:
	DECLARE_CONOBJECT(uiPlaySelectPreview);
	static void initPersistFields();

	uiPlaySelectPreview();

	bool onAdd		();
	bool onWake		();
	void onSleep	();

	void onMouseDown			(const GuiEvent& event);
	bool processCameraQuery(CameraQuery *query);

	// ��Ⱦ����
	void		onPreRender();
	void		renderWorld(const RectI &updateRect);
	void		onRender(Point2I offset, const RectI &updateRect);
};

