//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "T3D/GameTSCtrl.h"

class Player;

class EditorGuiGamePlay: public GameTSCtrl
{
	typedef GameTSCtrl Parent;

	//enum
	//{
	//	HpBarFillTexture,
	//	HpBarBackgroundTexture,
	//	HpBarbottomTexture, 
	//	PkstateTexture,
	//	TeamTexture,
	//	TeamCaptainTexture,
	//	TeammateTexture,
	//	FamilyTexture,

	//	totalTextureNum,
	//};

	//enum
	//{
	//	StyleOne,
	//	StyleTwo,

	//	totalStyleNum,
	//};

	enum
	{
		HpOffsetY = 15,
	};

	Player *m_pPlayer;

public:
	DECLARE_CONOBJECT(EditorGuiGamePlay);

	void		renderWorld(const RectI &updateRect);
	void		setControlPlayer(Player *);
	void        drawObjectInfo();

	EditorGuiGamePlay();
	~EditorGuiGamePlay();
};

extern EditorGuiGamePlay *gEditorGuiGamePlay;