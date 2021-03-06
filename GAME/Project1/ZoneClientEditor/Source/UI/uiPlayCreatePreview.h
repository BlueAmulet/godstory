
//
// PowerEngine V1.0
//
//
// Name:
//		uiPlayCreatePreview.h
//
// Abstract:
//

#pragma once

#include "T3D/GameTSCtrl.h"
#include "platform/platform.h"
#include "Gameplay/GameObjects/PlayerObject.h"

// ========================================================================================================================================
//  uiPlaySelectPreview
// 这个控件是游戏中人物选择界面
// ========================================================================================================================================
class uiPlayCreatePreview: public GameTSCtrl
{
	typedef GameTSCtrl Parent;

public:
	DECLARE_CONOBJECT(uiPlayCreatePreview);
	static void initPersistFields();

	uiPlayCreatePreview();

	bool onAdd		();
	bool onWake		();
	void onSleep	();

	void onMouseDown			(const GuiEvent& event);
	bool processCameraQuery(CameraQuery *query);

	// 渲染函数
	void		onPreRender();
	void		renderWorld(const RectI &updateRect);
	void		onRender(Point2I offset, const RectI &updateRect);
};

