//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "GamePlay/EditorGameplayState.h"
#include "core/crc.h"
#include "console/console.h"
#include "core/stringTable.h"
#include "core/resManager.h"
#include "ts/TSShapeRepository.h"
#include "Gameplay/GameObjects/NpcObjectData.h"
#include "Gameplay/GameObjects/PlayerData.h"
#include "Gameplay/Data/SoundResource.h"
#include "T3D/trigger.h"
#include "effects/EffectPacket.h"
#include "Gameplay/GameObjects/CollectionObject.h"
#include "util/RichTextDrawer.h"
#include "gfx/reflectPlane.h"
#include "Effects/ScreenFX.h"
#include "ui/dGuiShortCut.h"

#include "util/processMgr.h"
#include "util/aniThread.h"
// ========================================================================================================================================
//	全局对象
// ========================================================================================================================================
EditorGameplayState* g_EditorGameplayState = NULL;
EditorGameplayState gEditorGameplayState;

// ========================================================================================================================================
//	Constructor & Destructor
// ========================================================================================================================================
EditorGameplayState::EditorGameplayState()
{
	g_EditorGameplayState	= this;
}

EditorGameplayState::~EditorGameplayState()
{
}

void EditorGameplayState::clearResource()
{
	SimObject* grp = Sim::findObject("MissionGroup");
	if(grp)
		grp->deleteObject();

	ReflectPlane_SingleZ::freeResource();
}


// 在这里初始化的数据需要在EditorGameplayState::Shutdown()中释放 [3/4/2009 joy]
void EditorGameplayState::Initialize()
{
	// -------------------------------------- 初始化data --------------------------------------

	// 声音数据
	g_SoundManager->Read();
	// 模型数据
	g_TSShapeRepository.Read();
	// 模型数据
	g_ShapesSetRepository.Read();
	// Npc数据
	g_NpcRepository.Read();
	// 玩家数据
	g_PlayerRepository.Read();
	// 触发器数据
	g_TriggerDataManager->ReadTriggerData();
	//触发类物件数据
	gCollectionRepository.Read();

	CMultThreadWorkMgr::init();
	dGuiShortCut::Initialize();
}

void EditorGameplayState::preShutdown()
{
	clearResource();
	dGuiShortCut::Destory();
	CRichTextDrawer::cleanup();
	CMultThreadWorkMgr::shutdown();
}

void EditorGameplayState::Shutdown()
{
	g_SoundManager->Clear();
	g_TSShapeRepository.Clear();
	g_ShapesSetRepository.Clear();
	g_NpcRepository.Clear();
	g_PlayerRepository.Clear();
	g_TriggerDataManager->clearTriggerData();
	gCollectionRepository.Clear();
	g_ScreenFXMgr.clear();	
}

void EditorGameplayState::TimeLoop(S32 timeDelta)
{
	g_EffectPacketContainer.advanceTime(timeDelta);
	g_ScreenFXMgr.AdvanceTime(timeDelta);
}

void EditorGameplayState::prepRenderImage(S32 StateKey)
{
	g_EffectPacketContainer.prepRenderImage(StateKey);
}

void EditorGameplayState::onDisconnect()
{
#ifdef USE_MULTITHREAD_ANIMATE
	g_MultThreadWorkMgr->endProcess();
	g_MultThreadWorkMgr->enable(false);
#endif
}


GameBase* EditorGameplayState::GetControlObject()
{
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(conn) 
		return conn->getControlObject();
	return NULL;
}