//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "UI/dGuiMouseGamePlay.h"
#include "console/consoleTypes.h"
#include "Gameplay/ai/NaviGrid/NavigationBlock.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"

#ifdef NTJ_CLIENT
#include "gfx/primBuilder.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "T3D/gameConnection.h"
#include "T3D/gameFunctions.h"
#include "ts/tsShape.h"
#include "Effects/ScreenFX.h"
#include "windowManager/win32/win32Window.h"
#include "ts/tsShapeInstance.h"
#include "Gameplay/Team/ClientTeam.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/item/PrizeBox.h"
#include "Gameplay/Data/GraphicsProfile.h"
#include "Effects/EffectPacket.h"
#include "Gameplay/GameObjects/AnimationObject.h"
#include "Gameplay/GameObjects/CollectionObject.h"
#include "sceneGraph/lightManager.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "gui/buttons/guiButtonCtrl.h"
#include "T3D/fx/cameraFXMgr.h"
#endif


IMPLEMENT_CONOBJECT(dGuiMouseGamePlay);

#ifdef NTJ_CLIENT

// ========================================================================================================================================
//  castCallback
// ========================================================================================================================================
// 友方玩家、宠物剔除
bool mouseCollideCallback(SceneObject* obj)
{
	if(obj->getTypeMask() & GameObjectType)
	{
		if(((GameObject*)obj)->getGameObjectMask() & (PlayerObjectType | PetObjectType | TotemObjectType))
		{
			if(g_ClientGameplayState->GetControlPlayer() && !g_ClientGameplayState->GetControlPlayer()->CanAttackTarget((GameObject*)obj))
				return false;
		}
	}
	return true;
}

// ========================================================================================================================================
//  CUIMouseGamePlay
// ========================================================================================================================================

dGuiMouseGamePlay* g_UIMouseGamePlay = NULL;

// 摄像机属性
const F32 dGuiMouseGamePlay::m_CameraYawSpeed	= 0.2f;

const bool dGuiMouseGamePlay::m_LockPitch		= true;
const F32 dGuiMouseGamePlay::m_CameraPitchMin	= 0.58f;
const F32 dGuiMouseGamePlay::m_CameraPitchMax	= 0.62f;

const F32 dGuiMouseGamePlay::m_CameraZoomMin	= 7.0f;
const F32 dGuiMouseGamePlay::m_CameraZoomMax	= 32.0f;
const F32 dGuiMouseGamePlay::m_CameraZoomUnit	=
	(dGuiMouseGamePlay::m_CameraZoomMax - dGuiMouseGamePlay::m_CameraZoomMin) * 0.08f;
const F32 dGuiMouseGamePlay::m_CameraZoomTime	= 0.5f;

const F32 dGuiMouseGamePlay::m_OriginalCameraFov= 35.0f;
F32 dGuiMouseGamePlay::m_CameraFov				= dGuiMouseGamePlay::m_OriginalCameraFov;
const F32 dGuiMouseGamePlay::m_StartToPitch		= 32.0f;
const F32 dGuiMouseGamePlay::m_PitchRadian		= 0.30f;

F32 dGuiMouseGamePlay::m_fCameraYaw				= 0;
F32 dGuiMouseGamePlay::m_fCameraPitch			= dGuiMouseGamePlay::m_CameraPitchMax;
F32 dGuiMouseGamePlay::m_fCameraZoom			= 24.0f;
F32 dGuiMouseGamePlay::m_fCurrentZoom			= dGuiMouseGamePlay::m_fCameraZoom;
bool dGuiMouseGamePlay::m_IsFirstPerson			= false;
bool dGuiMouseGamePlay::m_IsZoomingCamera		= true;
bool dGuiMouseGamePlay::m_IsAutoYaw				= false;

const F32 dGuiMouseGamePlay::m_TargetDecalScale	= 2.5f;
// 碰撞相关变量
const F32 dGuiMouseGamePlay::m_ProjectDistance	= 80.f;
const U32 dGuiMouseGamePlay::m_MouseCollisionMask = GameObjectType | ItemObjectType;
const U32 dGuiMouseGamePlay::m_MouseFloorCollisionMask = TerrainObjectType | PlayerStepObjectType;
static dGuiMouseGamePlay::MouseCollisionInfo MouseCollisionObject;

// 自动移动
const SimTime dGuiMouseGamePlay::m_MouseAutoMoveTime = 3000;
const SimTime dGuiMouseGamePlay::m_MouseAutoMoveTimeInterval = 200;

bool dGuiMouseGamePlay::m_IsDragged				= false;


dGuiMouseGamePlay::dGuiMouseGamePlay()
{
	g_UIMouseGamePlay = this;
	m_IsCursorPushed = false;
	m_LastRenderTime = 0;
	m_NextAutoMoveTime = 0xFFFFFFFF;
	m_EnableAutoMoveTime = 0xFFFFFFFF;

	m_MouseState = MouseState_None;
	m_ViewMode = ViewMode_Normal;

	m_TexList.setSize(totalTextureNum);
	for(int i=0;i<totalTextureNum;i++)
		m_TexList[i] = NULL;

	for(int i=0;i<totalStyleNum;i++)
		m_StyleList[i] = NULL;

	m_TargetEP = 0;
	m_PreCastEP = 0;
	mDamageNumberSmallFont	= CommonFontManager::GetFont("Constantia", 48, true, 0, 0, true, false, true);
	mDamageNumberBigFont	= CommonFontManager::GetFont("Constantia", 72, true, 0, 0, true, false, true);
	mDamageTextSmallFont	= CommonFontManager::GetFont("微软雅黑", 24);
	mDamageTextBigFont		= CommonFontManager::GetFont("微软雅黑", 36);
	aniObj = NULL;
    mLastMouseOverObj = NULL;

	mItemNotifyCurFrame = 0;
	mItemNotifyEndPos.set(0,0);
	mItemNotifyIconFile = NULL;
	mItemNotifyIconFrame = NULL;
}

dGuiMouseGamePlay::~dGuiMouseGamePlay()
{
	for(int i=0;i<totalTextureNum;i++)
	{
		m_TexList[i].free();
	}
	mDamageNumberSmallFont = NULL;
	mDamageNumberBigFont = NULL;
	mDamageTextSmallFont = NULL;
	mDamageTextBigFont = NULL;
	if(aniObj)
		aniObj->deleteObject();
    mLastMouseOverObj = NULL;
	if(mItemNotifyIconFile)
		mItemNotifyIconFile = NULL;
	if(mItemNotifyIconFrame)
		mItemNotifyIconFrame = NULL;
}

void dGuiMouseGamePlay::Make3DMouseEvent(Gui3DMouseEvent & gui3DMouseEvent, const GuiEvent & event)
{
	(GuiEvent&)(gui3DMouseEvent) = event;
	gui3DMouseEvent.mousePoint = event.mousePoint;

	MatrixF camera_transform;
	Point3F camera_velocity;
	GameGetCameraTransform(&camera_transform, &camera_velocity);

	// get the eye pos and the mouse vec from that...
	Point3F screenPoint((F32)gui3DMouseEvent.mousePoint.x, (F32)gui3DMouseEvent.mousePoint.y, 1.0f);

	Point3F wp;
	unproject(screenPoint, &wp);

	gui3DMouseEvent.pos = camera_transform.getPosition();
	gui3DMouseEvent.vec = wp - camera_transform.getPosition();
	gui3DMouseEvent.vec.normalize();
}

// 每次鼠标状态改变后，都需更新视角状态
void dGuiMouseGamePlay::UpdateMouseState(const Point2I& pos)
{
	GameConnection* con = GameConnection::getConnectionToServer();
	if(!con)
		return;
	Player* player = dynamic_cast<Player*>(con->getControlObject());
	if(!player)
		return;

	if (ClientGameplayState::m_bMouseOperation)		// 鼠标操作模式
	{
		ClientGameplayState::m_bCameraDragged = true;
		ClientGameplayState::m_bMouseRun = false;
		if(m_MouseState & MouseState_Right)
		{
			MoveManager::mFreeLook = true;
			getRoot()->setCursorON(false);
			Platform::setWindowLocked(true);
			m_ViewMode = ViewMode_Freelook;
		}
		else
		{
			MoveManager::mFreeLook = true;
			getRoot()->setCursorON(true);
			Platform::setWindowLocked(false);
			m_ViewMode = ViewMode_Normal;
		}	
	}
	else	// 键盘操作模式
	{
		switch(m_MouseState)
		{
		case MouseState_None:
			{
				MoveManager::mFreeLook = false;
				getRoot()->setCursorON(true);
				Platform::setWindowLocked(false);
				ClientGameplayState::m_bCameraDragged = false;
				ClientGameplayState::m_bMouseRun = false;
				m_ViewMode = ViewMode_Normal;
			}
			break;
		case MouseState_Left:
			{
				MoveManager::mFreeLook = true;
				getRoot()->setCursorON(false);
				Platform::setWindowLocked(true);
				ClientGameplayState::m_bCameraDragged = true;
				ClientGameplayState::m_bMouseRun = false;
				m_ViewMode = ViewMode_Freelook;
			}
			break;
		case MouseState_Right:
			{
				MoveManager::mFreeLook = false;
				getRoot()->setCursorON(false);
				Platform::setWindowLocked(true);
				ClientGameplayState::m_bCameraDragged = true;
				ClientGameplayState::m_bMouseRun = false;
				player->setRotation(player->getRotation().z + GetCameraYaw(),ClientGameplayState::m_bCameraDragged);
				SetCameraYaw(0);
				m_ViewMode = ViewMode_Follow;
			}
			break;
		default:
			{
				// 设置摄像机与人物方向无关
				MoveManager::mFreeLook = false;
				// 设置鼠标拖曳摄像机
				getRoot()->setCursorON(false);
				Platform::setWindowLocked(true);
				ClientGameplayState::m_bCameraDragged = true;
				// 设置自动前进
				ClientGameplayState::m_bMouseRun = true;
				ClientGameplayState::m_bAutoRun = false;
				// 使人物和摄像机方向一致
				player->setRotation(player->getRotation().z + GetCameraYaw(),ClientGameplayState::m_bCameraDragged);
				SetCameraYaw(0);
				m_ViewMode = ViewMode_Forward;
			}
			break;
		}
	}
	Con::executef("updateTurnLeft");
	Con::executef("updateTurnRight");
}

bool dGuiMouseGamePlay::onAdd()
{
	if(!Parent::onAdd())
		return false;
	return true;
}

bool dGuiMouseGamePlay::onWake()
{
	if(!Parent::onWake())
		return false;

	if(!aniObj)
	{
		aniObj = new AnimationObject;
		aniObj->registerObject();
		aniObj->setTexture("gameres/gui/images/GUIbutton02_1_006.png");
		aniObj->setFrameRate(60);
	}
	
	return true;
}

void dGuiMouseGamePlay::updateData()
{
	if(m_TexList[HpBarFillTexture].isNull())
		m_TexList[HpBarFillTexture].set(GAME_CLIENT_DIRECTORY_IMAGES"GUIWindow30_1_015.png",&GFXDefaultGUIProfile);

	if(m_TexList[HpBarBackgroundTexture].isNull())
		m_TexList[HpBarBackgroundTexture].set(GAME_CLIENT_DIRECTORY_IMAGES"GUIWindow30_1_014.png",&GFXDefaultGUIProfile);

	if(m_TexList[HpBarbottomTexture].isNull())
		m_TexList[HpBarbottomTexture].set(GAME_CLIENT_DIRECTORY_IMAGES"GUIWindow30_1_022.png",&GFXDefaultGUIProfile);

	if(m_TexList[PkstateTexture].isNull())
		m_TexList[PkstateTexture].set(GAME_CLIENT_DIRECTORY_IMAGES"GUIWindow30_1_020.png",&GFXDefaultGUIProfile);

	if(m_TexList[TeamTexture].isNull())
		m_TexList[TeamTexture].set(GAME_CLIENT_DIRECTORY_IMAGES"GUIWindow30_1_018.png",&GFXDefaultGUIProfile);

	if(m_TexList[TeamCaptainTexture].isNull())
		m_TexList[TeamCaptainTexture].set(GAME_CLIENT_DIRECTORY_IMAGES"GUIWindow30_1_016.png",&GFXDefaultGUIProfile);

	if(m_TexList[TeammateTexture].isNull())
		m_TexList[TeammateTexture].set(GAME_CLIENT_DIRECTORY_IMAGES"GUIWindow30_1_017.png",&GFXDefaultGUIProfile);

	if(m_TexList[FamilyTexture].isNull())
		m_TexList[FamilyTexture].set(GAME_CLIENT_DIRECTORY_IMAGES"GUIWindow30_1_021.png",&GFXDefaultGUIProfile);
};

void dGuiMouseGamePlay::onSleep()
{
	Parent::onSleep();
	if(m_TargetEP)
	{
		g_EffectPacketContainer.removeEffectPacket(m_TargetEP);
		m_TargetEP = 0;
	}
	if(m_PreCastEP)
	{
		g_EffectPacketContainer.removeEffectPacket(m_PreCastEP);
		m_PreCastEP = 0;
	}
}

void dGuiMouseGamePlay::onMouseMove(const GuiEvent &event)
{
	Make3DMouseEvent(m_Last3DGuiEvent, event);
	on3DMouseMove(m_Last3DGuiEvent);
}

void dGuiMouseGamePlay::on3DMouseMove(const Gui3DMouseEvent& event)
{
	Player* control_player = g_ClientGameplayState->GetControlPlayer();
	if (!control_player) 
		return;

    if ( mLastMouseOverObj )
    {
        mLastMouseOverObj->setColorExposure(1.0f);
        mLastMouseOverObj = NULL;
    }

	m_MouseCollisionInfo.m_Object = NULL;
	// 有碰撞到物体
	if(IsMouseCollide(event, m_MouseCollisionInfo, m_MouseCollisionMask))
	{
		On3DMouseMove_OnObject(event, control_player, m_MouseCollisionInfo.m_Object);		
	}
	else
	{
		if(m_IsCursorPushed)
			g_ClientGameplayState->popCursor();
		m_IsCursorPushed = false;
		m_pPrizeBox = NULL;
	}
}

void dGuiMouseGamePlay::On3DMouseMove_OnObject(const Gui3DMouseEvent& event, Player* control_player, SceneObject* pSceneObject)
{	
	U32 type = pSceneObject->getType();
	if(type & GameObjectType)
	{
		/*if(g_ClientGameplayState->isShortcutSlotBeSelect())
			return;*/
		
		ClientGameplayAction* currentAction = g_ClientGameplayState->getCurrentAction();
		if(currentAction)
			return;
		GameObject* target = dynamic_cast<GameObject*>(pSceneObject);
		if(target->getGameObjectMask() & NpcObjectType)
		{
			NpcObject* npc = dynamic_cast<NpcObject*>(target);
			if(npc)
			{

				if(!npc->isCombative())
				{
					g_ClientGameplayState->setCurrentCursor("dialog");
				}
				else if(npc->canTame())
				{
					g_ClientGameplayState->setCurrentCursor("tame");
				}
				else
				{
					g_ClientGameplayState->setCurrentCursor("attack");
				}
				m_IsCursorPushed = true;
			}
		}
        mLastMouseOverObj = (ShapeBase*)target;
        mLastMouseOverObj->setColorExposure(1.4f);
	}
	else
	{
		PrizeBox *pPrizeBox = NULL;
		CollectionObject *pCollectionObj = NULL;
		if ( (pPrizeBox = dynamic_cast<PrizeBox *>(pSceneObject)) != NULL)
		{
			if (!pPrizeBox->m_IsBox)
			{
				m_pPrizeBox = pPrizeBox;		
			}
			g_ClientGameplayState->setCurrentCursor("PickUp");
			m_IsCursorPushed = true;

            mLastMouseOverObj = (ShapeBase*)pPrizeBox;
            mLastMouseOverObj->setColorExposure(1.4f);
		}
		else if ( (pCollectionObj = dynamic_cast<CollectionObject *>(pSceneObject)) != NULL)
		{
			g_ClientGameplayState->setCurrentCursor("PickUp");
			m_IsCursorPushed = true;

            mLastMouseOverObj = (ShapeBase*)pCollectionObj;
            mLastMouseOverObj->setColorExposure(1.4f);
		}
	}		
}

void dGuiMouseGamePlay::On3DMouseMove_SpellSelect(const Gui3DMouseEvent& event, Player* control_player, SceneObject* pSceneObject)
{
}

void dGuiMouseGamePlay::onMouseLeave(const GuiEvent &event)
{
	if(m_IsCursorPushed)
		g_ClientGameplayState->popCursor();
	m_IsCursorPushed = false;
}

void dGuiMouseGamePlay::onMouseEnter(const GuiEvent &event)
{
	// 刷新鼠标状态
	static GuiCanvas* pCanvas = getRoot();
	if(pCanvas->mouseButtonDown())
		m_MouseState |= MouseState_Left;
	else
		m_MouseState &= ~MouseState_Left;
	if(pCanvas->mouseRightButtonDown())
		m_MouseState |= MouseState_Right;
	else
		m_MouseState &= ~MouseState_Right;
	UpdateMouseState(event.mousePoint);
}

void dGuiMouseGamePlay::onMouseDown(const GuiEvent& event)
{
	mouseLock();
	m_MouseState |= MouseState_Left;
	UpdateMouseState(event.mousePoint);

	m_EnableAutoMoveTime = Platform::getVirtualMilliseconds() + m_MouseAutoMoveTime;
	Make3DMouseEvent(m_Last3DGuiEvent, event);
	on3DMouseDown(m_Last3DGuiEvent);
}

void dGuiMouseGamePlay::on3DMouseDown(const Gui3DMouseEvent& event)
{
	Player* control_player = g_ClientGameplayState->GetControlPlayer();
	if (!control_player) 
		return;

	// 可以判断是否在自动移动中
	bool isAutoMove = (m_NextAutoMoveTime <= Platform::getVirtualMilliseconds());

	m_MouseCollisionInfo.m_Object = NULL;
	m_MouseCollisionInfo.m_t = 10.0f;
	MouseCollisionInfo collision;
	//鼠标点击取消寻径
	{
		g_ClientGameplayState->getStrideServerPath().clear();
		Con::executef("closeStrideFindPath");
		if(g_ClientGameplayState->getStrideServerPathFlag())
			g_ClientGameplayState->setStrideServerPathFlag(false);
	}

	if(!isAutoMove && IsMouseCollide(event, collision, m_MouseCollisionMask, true, mouseCollideCallback)) //鼠标点击了m_MouseCollisionMask中的物体
		m_MouseCollisionInfo = collision;
	if(IsMouseCollide(event, collision, m_MouseFloorCollisionMask, false)) //鼠标点击了m_MouseFloorCollisionMask中的物体
	{
		if(collision.m_t < m_MouseCollisionInfo.m_t)
			m_MouseCollisionInfo = collision;
	}

	ClientGameplayAction* currentAction = g_ClientGameplayState->getCurrentAction();
	if(currentAction)
	{
		if(currentAction->getActionType() == INFO_NPCSHOP || currentAction->getActionType() == INFO_ITEM_BATCHBUY)
			Con::executef("clearBuyFlag");
		g_ClientGameplayState->cancelCurrentAction();
	}
		
	// 有碰撞到物体且物体为GameObject对象
	if(m_MouseCollisionInfo.m_Object && (m_MouseCollisionInfo.m_Object->getType() & m_MouseCollisionMask))
	{
		On3DLeftMouseDown_OnObject(event, control_player, m_MouseCollisionInfo.m_Object);
	}
	// 是鼠标操作
	else if (ClientGameplayState::m_bMouseOperation)
	{
		// 没有碰撞到物体
		if(!m_MouseCollisionInfo.m_Object)
		{
			// 和人物差不多高的点
			m_MouseCollisionInfo.m_Position = event.pos + event.vec * GetCurrentZoom();
		}
		m_MouseCollisionInfo.m_Position.z += 0.2f;
		static MatrixF mat;
		mat.identity();
		mat.setPosition(m_MouseCollisionInfo.m_Position);
		// 以最终点为准 [7/17/2009 joy]
		bool newMove = g_NavigationManager->CanReach(m_MouseCollisionInfo.m_Position);
		g_EffectPacketContainer.addEffectPacket(newMove ? EP_MouseMobile/*可移动*/ : EP_MouseImmobile/*不可移动*/, g_ClientGameplayState->GetControlPlayer(), mat, NULL, m_MouseCollisionInfo.m_Position);

		SimTime tTime = m_EnableAutoMoveTime;
		control_player->SetTarget(m_MouseCollisionInfo.m_Position);
		m_EnableAutoMoveTime = tTime;
		// 记录下次自动移动的时间
		if(ClientGameplayState::m_bMouseOperation)
			m_NextAutoMoveTime = Platform::getVirtualMilliseconds() + m_MouseAutoMoveTimeInterval;
	}

	if(m_MouseCollisionInfo.m_Object)
	{
		if(m_MouseCollisionInfo.m_Object->getType() & GameObjectType)
			control_player->PreCastSpellDone((GameObject*)m_MouseCollisionInfo.m_Object, &m_MouseCollisionInfo.m_Object->getPosition());
		else
			control_player->PreCastSpellDone(NULL, &m_MouseCollisionInfo.m_Position);
	}
}

void dGuiMouseGamePlay::On3DLeftMouseDown_OnObject(const Gui3DMouseEvent& event, Player* control_player, SceneObject* pSceneObject)
{
	if (!control_player || !pSceneObject)
		return ;

	Point3F	player_position = control_player->getPosition();
	VectorF	distance = pSceneObject->getPosition() - player_position;
	//bool doubleClick = (event.nMouseClickCount > 1);    
	GameObject* obj = NULL;
	PrizeBox *pPrizeBox = NULL;

	U32 nTypeMask = pSceneObject->getTypeMask();
	if (nTypeMask & ItemObjectType)
	{
		//宝箱
		PrizeBox *pPrizeBox = dynamic_cast<PrizeBox *>(pSceneObject);
		if (pPrizeBox)
		{
			control_player->setPickupPath(pPrizeBox, Player::Type_OpenDropList);
			return;
		}
		//采集
		CollectionObject *pCollectionObj = dynamic_cast<CollectionObject *>(pSceneObject);
		if (pCollectionObj)
		{
			control_player->setCollectionPath(pCollectionObj);
			return;
		}
	}
	else if(nTypeMask & GameObjectType)
	{
		GameObject* obj = (GameObject*)(pSceneObject);
		// 玩家
		if(obj->getGameObjectMask() & PlayerObjectType)
		{
			if(control_player->CanAttackTarget(obj))
				control_player->SetTarget(obj);
			else
				control_player->SetTarget(obj->getPosition());
		}
		// 宠物
		else if(obj->getGameObjectMask() & PetObjectType)
		{
			if(control_player->CanAttackTarget(obj))
				control_player->SetTarget(obj);
			else
				control_player->SetTarget(obj->getPosition());
		}
		// NPC
		else if(obj->getGameObjectMask() & NpcObjectType)
		{
			control_player->SetTarget(obj);
			if(obj->canTame())
			{
				// 自动使用捕捉技能
				control_player->PreCastSpell(200030001, obj, NULL);
			}
		}
	}
}

void dGuiMouseGamePlay::On3DLeftMouseDown_SpellSelect(const Gui3DMouseEvent& event, Player* control_player, SceneObject* pSceneObject)
{
}

void dGuiMouseGamePlay::onMouseUp(const GuiEvent& event)
{
	mouseUnlock();
	m_MouseState &= ~MouseState_Left;
	UpdateMouseState(event.mousePoint);

	// 如果未到开启自动移动的时间
	if(m_EnableAutoMoveTime > Platform::getVirtualMilliseconds())
	{
		m_NextAutoMoveTime = 0xFFFFFFFF;
		m_EnableAutoMoveTime = 0xFFFFFFFF;
	}

	ClientGameplayAction* currentAction = g_ClientGameplayState->getCurrentAction();
	if(!currentAction)
		return;
	if(currentAction->getActionType() == INFO_SHORTCUT_EXCHANGE)
	{
		ClientGameplayParam* param = currentAction->getParam(0);
		if(param->getIntArg(0) == SHORTCUTTYPE_PANEL)
		{
			ClientGameplayParam* param1 = new ClientGameplayParam;
			param1->setIntArgValues(2, 0, 0);
			g_ClientGameplayState->setCurrentActionParam(param1);
		}
		else if(param->getIntArg(0) == SHORTCUTTYPE_IDENTIFY)
		{
			U32 uSlotIndex = param->getIntArg(1);
			ClientGameplayAction* action = new ClientGameplayAction(1, INFO_ITEM_DROP);
			g_ClientGameplayState->setCurrentAction(action);
			ClientGameplayParam* param1 = new ClientGameplayParam;
			param1->setIntArgValues(2, SHORTCUTTYPE_IDENTIFY, uSlotIndex);
			g_ClientGameplayState->setCurrentActionParam(param1);
		}
		else if ( param->getIntArg(0) == SHORTCUTTYPE_STRENGTHEN )
		{
			U32 uSlotIndex = param->getIntArg(1);
			ClientGameplayAction* action = new ClientGameplayAction(1, INFO_ITEM_DROP);
			g_ClientGameplayState->setCurrentAction(action);
			ClientGameplayParam* param1 = new ClientGameplayParam;
			param1->setIntArgValues(2, SHORTCUTTYPE_STRENGTHEN, uSlotIndex);
			g_ClientGameplayState->setCurrentActionParam(param1);
		}
		else if ( param->getIntArg(0) == SHORTCUTTYPE_EQUIP_PUNCHHOLE )
		{
			U32 uSlotIndex = param->getIntArg(1);
			ClientGameplayAction* action = new ClientGameplayAction(1, INFO_ITEM_DROP);
			g_ClientGameplayState->setCurrentAction(action);
			ClientGameplayParam* param1 = new ClientGameplayParam;
			param1->setIntArgValues(2, SHORTCUTTYPE_EQUIP_PUNCHHOLE, uSlotIndex);
			g_ClientGameplayState->setCurrentActionParam(param1);
		}
		else if(param->getIntArg(0) == SHORTCUTTYPE_STALL_PET)
		{
			ClientGameplayParam* param1 = new ClientGameplayParam;
			param1->setIntArgValues(2, 0, 0);
			g_ClientGameplayState->setCurrentActionParam(param1);
		}
		else if (param->getIntArg(0) == SHORTCUTTYPE_SKILL_HINT)
		{
			ClientGameplayParam* param1 = new ClientGameplayParam;
			param1->setIntArgValues(2, 0, 0);
			g_ClientGameplayState->setCurrentActionParam(param1);
		}
		else if(param->getIntArg(0) == SHORTCUTTYPE_INVENTORY)
		{
			Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
			U32 uSlotIndex = param->getIntArg(1);
			ShortcutObject* pSlot = g_ItemManager->getShortcutSlot(pPlayer, SHORTCUTTYPE_INVENTORY, uSlotIndex);
			if(!pSlot)
			{
				g_ClientGameplayState->cancelCurrentAction();
				return;
			}
			ItemShortcut* pItem = (ItemShortcut*)pSlot;
			ClientGameplayAction* action = new ClientGameplayAction(1, INFO_ITEM_DROP);
			g_ClientGameplayState->setCurrentAction(action);

			Res* pRes = pItem->getRes();
			if(!pRes)
				return;
			char buff[256] = "\0";
			char tempbuffer[128] = "\0";
			dSprintf(buff, sizeof(buff), "<t  m='0'  c='0xffffffff'>%s</t>", "你确定要销毁【");
			pRes->getItemName(tempbuffer, 128);
			dStrcat(buff, sizeof(buff), tempbuffer);
			U32 iCount = pRes->getQuantity();
			dSprintf(tempbuffer, sizeof(tempbuffer), "<t  m='0' c='0xffffffff'>%s%d%s</t>", "】×", iCount, " 吗?");
			dStrcat(buff, sizeof(buff), tempbuffer);

			Con::executef("DorpItemPopUp", buff, Con::getIntArg(uSlotIndex));
		}		
		else if(param->getIntArg(0) == SHORTCUTTYPE_REPAIR)
		{
			S32 iIndex = param->getIntArg(1);
			if(iIndex != 0)
				return;
			Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
			if(!pPlayer)
				return;
			GameConnection* conn = pPlayer->getControllingClient();
			if(!conn)
				return;
			ClientGameNetEvent *event = new ClientGameNetEvent(INFO_SHORTCUT_EXCHANGE);
			event->SetIntArgValues(4, SHORTCUTTYPE_REPAIR, 0, SHORTCUTTYPE_NONE, 0);
			event->SetInt32ArgValues(1, 0);
			conn->postNetEvent(event);
			g_ClientGameplayState->cancelCurrentAction();
		}
	}
	
	if(m_IsDragged)		// 如果鼠标在按下期间有拖动，则不响应弹起事件
	{
		m_IsDragged = false;
		return;
	}
	
}

void dGuiMouseGamePlay::on3DMouseUp(const Gui3DMouseEvent& event)
{
	//g_ClientGameplayState->cancelCurrentAction();
}

void dGuiMouseGamePlay::onMouseDragged(const GuiEvent& event)
{
	if(ClientGameplayState::m_bMouseOperation)
		Make3DMouseEvent(m_Last3DGuiEvent, event);
}

void dGuiMouseGamePlay::on3DMouseDragged(const Gui3DMouseEvent& event)
{
}

void dGuiMouseGamePlay::onRightMouseDown(const GuiEvent& event)
{
	m_MouseState |= MouseState_Right;	
	UpdateMouseState(event.mousePoint);

	Make3DMouseEvent(m_Last3DGuiEvent, event);
	on3DRightMouseDown(m_Last3DGuiEvent);
}

void dGuiMouseGamePlay::on3DRightMouseDown(const Gui3DMouseEvent& event)
{
	Player* control_player = g_ClientGameplayState->GetControlPlayer();
	if (!control_player) 
		return;

	// 取消施法范围选择
	if(control_player && control_player->GetSpell().GetPreSpell().pSkillData)
		control_player->GetSpell().ResetPreSpell();

//	m_MouseCollisionInfo.m_Object = NULL;
//	MouseCollisionInfo collision;
//
//	if(IsMouseCollide(event, collision, m_MouseCollisionMask)) //鼠标点击了m_MouseCollisionMask中的物体
//		m_MouseCollisionInfo = collision;
//
//	if(!m_MouseCollisionInfo.m_Object)
//		return;
//
//	GameObject* pGameObject = dynamic_cast<GameObject*>(m_MouseCollisionInfo.m_Object);
//	if(!pGameObject)
//		return;
//	
//	control_player->SetTarget(pGameObject);
}

void dGuiMouseGamePlay::onRightMouseUp(const GuiEvent& event)
{
	m_MouseState &= ~MouseState_Right;
	UpdateMouseState(event.mousePoint);

	if(m_IsDragged)		// 如果鼠标在按下期间有拖动，则不响应弹起事件
	{
		m_IsDragged = false;
		return;
	}

	Make3DMouseEvent(m_Last3DGuiEvent, event);
	on3DRightMouseUp(m_Last3DGuiEvent);
}

void dGuiMouseGamePlay::on3DRightMouseUp(const Gui3DMouseEvent& event)
{
	Player* control_player = g_ClientGameplayState->GetControlPlayer();
	if (!control_player) 
		return;

	// 取消选中目标
	//control_player->SetTarget(NULL);

	m_MouseCollisionInfo.m_Object = NULL;
	MouseCollisionInfo collision;

	if(IsMouseCollide(event, collision, m_MouseCollisionMask)) //鼠标点击了m_MouseCollisionMask中的物体
		m_MouseCollisionInfo = collision;

	if(!m_MouseCollisionInfo.m_Object)
	{
		// to do: 清除弹出菜单
		return;
	}

	U32 nTypeMask = m_MouseCollisionInfo.m_Object->getTypeMask();
	// item对象
	if (nTypeMask & ItemObjectType)
	{
		PrizeBox *pPrizeBox = dynamic_cast<PrizeBox*>(m_MouseCollisionInfo.m_Object);
		if (pPrizeBox)
		{
			control_player->setPickupPath(pPrizeBox, Player::Type_AutoPickup);
			return;
		}
	}
	// GameObject对象
	else if(nTypeMask & GameObjectType)
	{
		// 设定目标
		GameObject* pGameObject = (GameObject*)(m_MouseCollisionInfo.m_Object);
		control_player->SetTarget(pGameObject,false);

		// 玩家对象
		if(pGameObject->getGameObjectMask() & PlayerObjectType)
		{
			Player* target = (Player*)(pGameObject);
			// 交互距离太远
			if(!control_player->canInteraction(target))
			{
				MessageEvent::show(SHOWTYPE_NOTIFY, MSG_PLAYER_TOOFAR);	
				return;
			}
			GameConnection* conn = GameConnection::getConnectionToServer();
			if(conn)
			{
				ClientGameNetEvent* event = new ClientGameNetEvent(INFO_STALL);
				event->SetInt32ArgValues(3, TRADE_ASKFOR, target->getPlayerID(),stall_individual);
				conn->postNetEvent(event);
			}	
		}
	}
}

void dGuiMouseGamePlay::onRightMouseDragged(const GuiEvent& event)
{
}

void dGuiMouseGamePlay::onMiddleMouseDown(const GuiEvent& event)
{
	m_MouseState |= MouseState_Middle;
	UpdateMouseState(event.mousePoint);
}

void dGuiMouseGamePlay::onMiddleMouseUp(const GuiEvent& event)
{
	m_MouseState &= ~MouseState_Middle;
	UpdateMouseState(event.mousePoint);
}

void dGuiMouseGamePlay::onMiddleMouseDragged(const GuiEvent& event)
{
}

bool dGuiMouseGamePlay::onMouseWheelUp(const GuiEvent& event)
{
	ZoomCamera(true);
	return true;
}

bool dGuiMouseGamePlay::onMouseWheelDown(const GuiEvent& event)
{
	ZoomCamera(false);
	return true;
}

// 获取摄像机角度
F32 dGuiMouseGamePlay::GetCameraPitch()
{
	return m_fCurrentZoom > m_StartToPitch ? m_fCameraPitch : m_fCameraPitch - (m_StartToPitch-m_fCurrentZoom)/(m_StartToPitch-m_CameraZoomMin)*m_PitchRadian;
}

// 获取摄像机焦点高度
F32 dGuiMouseGamePlay::GetCurrentFocus(F32 height)
{
	return mClampF((m_CameraZoomMax-m_fCurrentZoom)/(m_CameraZoomMax-m_CameraZoomMin), 0, 1)*height*0.35f;
}

// 设置摄像机角度
void dGuiMouseGamePlay::SetCameraPitch(F32 val)
{
	if(m_LockPitch)
		return;
	m_fCameraPitch = mClampF(val, m_CameraPitchMin, m_CameraPitchMax);
}
void dGuiMouseGamePlay::SetCameraYaw(F32 val)
{
	m_fCameraYaw = val;
	//确定最短旋转角度
	while(m_fCameraYaw > M_PI_F)
		m_fCameraYaw -= M_2PI_F;
	while(m_fCameraYaw < -M_PI_F)
		m_fCameraYaw += M_2PI_F;
}

void dGuiMouseGamePlay::SetAutoYaw(bool val)
{
	m_IsAutoYaw = val;
}

void dGuiMouseGamePlay::UpdateAutoYaw(F32 dt)
{
	if(!m_IsAutoYaw)
		return;

	if(!(bool)m_fCameraYaw || ClientGameplayState::m_bCameraDragged)
	{
		SetAutoYaw(false);
		return;
	}

	if(m_fCameraYaw > 0)
		m_fCameraYaw -= mClampF(m_CameraYawSpeed * dt, 0, m_fCameraYaw);
	else
		m_fCameraYaw += mClampF(m_CameraYawSpeed * dt, 0, -m_fCameraYaw);
}

/// Zoom the camera.
///		@param		in_out			Zoom in - ture; zoom out - false.
void dGuiMouseGamePlay::ZoomCamera(bool in_out)
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

void dGuiMouseGamePlay::SetCameraZoom(F32 zoom_camera)
{
	if(m_IsFirstPerson)
		return;
	m_IsZoomingCamera = true;
	m_fCameraZoom = mClampF(zoom_camera, m_CameraZoomMin, m_CameraZoomMax);
}

void dGuiMouseGamePlay::SetCameraFov(Point2I& size)
{
	F32 tmp = ((F32)size.x/(F32)size.y) / ((F32)Win32Window::getMinimum().x/(F32)Win32Window::getMinimum().y);
	m_CameraFov = m_OriginalCameraFov * tmp;
}

void dGuiMouseGamePlay::UpdateZoom(F32 dt, F32& CurrentZoom)
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

// 鼠标碰撞
bool dGuiMouseGamePlay::IsMouseCollide(const Gui3DMouseEvent& event, MouseCollisionInfo& collision, U32 mask, bool bUseBox /* = true */, Container::castCallback callback/* = NULL */)
{
	// 忽略对自己的射线碰撞
	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(player)
		player->disableCollision();

	Point3F start_point	= event.pos;
	Point3F end_point	= event.pos + event.vec * m_ProjectDistance;

	RayInfo ri;
	bool hit;

	if(bUseBox)
		hit = gClientContainer.collideBox(start_point,
		end_point,
		mask,
		&ri,
		player->getLayerID(),
		callback);
	else
		hit = gClientContainer.castRay(start_point,
		end_point,
		mask,
		&ri,
		player->getLayerID(),
		callback);

	if(hit)
	{
		collision.m_Position		= ri.point;
		collision.m_Object			= ri.object;
		collision.m_Normal			= ri.normal;
		collision.m_t				= ri.t;

		AssertFatal(collision.m_Object, "Error! Invalid object.");
	}

	if(player)
		player->enableCollision();

	return hit;
}

void dGuiMouseGamePlay::UpdateAutoMove()
{
	if(!ClientGameplayState::m_bMouseOperation || m_NextAutoMoveTime > Platform::getVirtualMilliseconds())
		return;

	Make3DMouseEvent(m_Last3DGuiEvent, m_Last3DGuiEvent);
	on3DMouseDown(m_Last3DGuiEvent);
}

// ====================================================================================
//  渲染函数
// ====================================================================================

void dGuiMouseGamePlay::onPreRender()
{
	Parent::onPreRender();

	m_LastRenderTime = Platform::getVirtualMilliseconds();

	UpdateAutoMove();
	updateData();
}

bool dGuiMouseGamePlay::processCameraQuery(CameraQuery *camq)
{
	GameUpdateCameraFov();
	bool ret = GameProcessCameraQuery(camq);

	if(!ret && g_ClientGameplayState->isTransporting())
		return true;
	
	return ret;
}

void dGuiMouseGamePlay::renderWorld(const RectI &updateRect)
{
	gClientSceneGraph->getLightManager()->hdrPrepare(updateRect.point, updateRect.extent);

	if(g_ClientGameplayState->isTransporting() && g_ClientGameplayState->isLastFrameOk())
	{
		GFX->setClipRect(updateRect);

		g_ScreenFXMgr.DrawBuff(updateRect);
	}
	else
	{
		// God forgives me! We will never get the answer until now
		RefreshPreCastDecal();

		GameRenderWorld();

		gClientSceneGraph->getLightManager()->hdrRender();

		GFX->setClipRect(updateRect);

		drawGameObjectMessages();
		drawPrizeItem(m_pPrizeBox);
		drawItemNotify();

        g_ScreenFXMgr.SetRect(updateRect);
        g_ScreenFXMgr.Render();

		if(g_ClientGameplayState->isTransporting() && !g_ClientGameplayState->isLastFrameOk())
		{
			g_ScreenFXMgr.updateBuff(updateRect);
			g_ClientGameplayState->setLastFrameOk();
		}

		gCameraShakeEX.Render();
	}
}

// ====================================================================================
//  Decal
// ====================================================================================

void dGuiMouseGamePlay::RefreshTargetDecal()
{
	static MatrixF mat(true);
	static Point3F pos(0,0,0);
	static F32 rad;
	static ColorI color(255,255,255);

	if(m_TargetEP)
		g_EffectPacketContainer.removeEffectPacket(m_TargetEP);

	Player* player = g_ClientGameplayState->GetControlPlayer();
	if (!player || !player->GetTarget())
		return;

	m_TargetEP = g_EffectPacketContainer.addEffectPacket(player->CanAttackTarget(player->GetTarget()) ? EP_TargetCombative : EP_Target, player->GetTarget(), mat, NULL, pos);
	rad = player->GetTarget()->getWorldSphere().radius * m_TargetDecalScale;
	g_EffectPacketContainer.refreshDecal(m_TargetEP, &rad, &pos, &color);
}

void dGuiMouseGamePlay::RefreshPreCastDecal()
{
	static MatrixF mat(true);
	static F32 rad;
	static bool inRange;
	static ColorI colorValid(COLORI_NPC_FRIEND);
	static ColorI colorUnknown(255,0,255);
	static GuiCanvas* pCanvas = getRoot();
	static GuiEvent guiEvent;

	Player* player = g_ClientGameplayState->GetControlPlayer();
	if (!player || !pCanvas || !player->GetSpell().GetPreSpell().pSkillData)
	{
		if(m_PreCastEP)
		{
			g_EffectPacketContainer.removeEffectPacket(m_PreCastEP);
			m_PreCastEP = 0;
		}
		return;
	}

	guiEvent.mousePoint = pCanvas->getCursorPos();
	Make3DMouseEvent(m_Last3DGuiEvent, guiEvent);
	if(!IsMouseCollide(m_Last3DGuiEvent, m_MouseCollisionInfo, m_MouseFloorCollisionMask, false))
	{
		if(m_PreCastEP)
		{
			g_EffectPacketContainer.removeEffectPacket(m_PreCastEP);
			m_PreCastEP = 0;
		}
		return;
	}

	if(!m_PreCastEP)
	{
		mat.setPosition(m_MouseCollisionInfo.m_Position);
		m_PreCastEP = g_EffectPacketContainer.addEffectPacket(EP_PreCast, player, mat, NULL, m_MouseCollisionInfo.m_Position);
	}

	SkillData::EffectLimit& effectLimit = player->GetSpell().GetPreSpell().pSkillData->GetEffectLimit();
	rad = (SkillData::Region_Circle == effectLimit.region) ? effectLimit.regionRect.x * 2 : getMin(effectLimit.regionRect.x, effectLimit.regionRect.y);
	rad = mClampF(rad, 1.0f, 20.0f);
	inRange = player->GetSpell().GetPreSpell().pSkillData->InRangeSquared((m_MouseCollisionInfo.m_Position - player->getPosition()).lenSquared());
	g_EffectPacketContainer.refreshDecal(m_PreCastEP, &rad, &m_MouseCollisionInfo.m_Position, inRange ? &colorValid : &colorUnknown);
}

//====================================================================================
//            头顶信息
//====================================================================================

// ----------------------------------------------------------------------------
// 添加待渲染经验荣誉文本内容
// target	目标对象
// flag		0-经验值  1-荣誉值
// Number   经验值

void dGuiMouseGamePlay::addExpText(GameObject* target,  bool flag, U32 number)
{
    if(target)
        target->addExpText(flag, number);
}


// ----------------------------------------------------------------------------
// 添加待渲染伤害文本内容
// target	目标对象
// tex		伤害文本贴图

void dGuiMouseGamePlay::addDamageText(GameObject* target,  GFXTexHandle* Tex)
{
	if(target)
		target->addDamageText(Tex);
}

// ----------------------------------------------------------------------------
// 添加待渲染伤害的数字值
// target	    目标对象
// text		    伤害数字值
// colorEnum	伤害数字颜色的枚举值
// pos		    渲染位置(0-中间 1-左边 2-右边)
// zoom		    是否放大显示(用于暴击数值)
void dGuiMouseGamePlay::addDamageNumber(GameObject* target, S32 number, U32 color, U32 pos, bool zoom)
{
	if(target)
		target->addDamageNumber(number, color, pos,zoom);
}

void dGuiMouseGamePlay::drawGameObjectMessages()
{
	Player* control_player = g_ClientGameplayState->GetControlPlayer();
	if(!control_player)
		return;

	Vector<SceneObject*>::iterator itr;
	itr = gClientSceneGraph->getInCameraObjects().begin();
	for (; itr != gClientSceneGraph->getInCameraObjects().end();++itr)
	{
		if ((*itr)->getTypeMask() & GameObjectType)
		{
			GameObject* pGameObject = (GameObject*)(*itr);
			if(pGameObject)
			{
				//pGameObject->drawDamageInfo();
				pGameObject->drawObjectInfo(control_player,&m_TexList,m_StyleList);
				pGameObject->drawObjectWord(m_StyleList);
			}
		}
		else if ((*itr)->getTypeMask() & ItemObjectType)
		{
			CollectionObject* pCollectionObject = dynamic_cast<CollectionObject*>(*itr);
			if(pCollectionObject)
				pCollectionObject->drawObjectInfo(control_player,&m_TexList,m_StyleList);
		}
	}

	GameObject* pTarObject = control_player->GetTarget();
	if(pTarObject)
    {
        if((pTarObject != control_player && (pTarObject->getGameObjectMask() & PlayerObjectType)) || 
            ((pTarObject->getGameObjectMask() & NpcObjectType) && pTarObject->isCombative()) || 
            (pTarObject->getGameObjectMask() & PetObjectType))
        {
            pTarObject->drawHpBar(pTarObject,&m_TexList,&m_StyleList);
        }
    }
    //伤害数字最后画
    itr = gClientSceneGraph->getInCameraObjects().begin();
    for (; itr != gClientSceneGraph->getInCameraObjects().end();++itr)
    {
        if ((*itr)->getTypeMask() & GameObjectType)
        {
            GameObject* pGameObject = (GameObject*)(*itr);
            if(pGameObject)
            {
                pGameObject->drawDamageInfo();              
            }
        }       
    }

	if(!g_GraphicsProfile.getDrawNameState() && pTarObject)
		pTarObject->drawObjectInfo(control_player,&m_TexList,&m_StyleList);
}

void dGuiMouseGamePlay::drawPrizeItem(PrizeBox *pBox)
{
	if (pBox && !pBox->m_IsBox && pBox->m_pItemShortcut)
	{
		Point3F screenPosition;
		Point3F position = pBox->getRenderPosition();
		position.z += pBox->getWorldBox().len_z();
		g_UIMouseGamePlay->project(position, &screenPosition);
		StringTableEntry strItemName = pBox->m_pItemShortcut->getRes()->getItemName();
		Point2I textPos;
		textPos.x = screenPosition.x;
		textPos.y = screenPosition.y - 25;
		GFX->getDrawUtil()->drawText(m_StyleList[StyleOne]->mFont, textPos, strItemName);
	}
}

#define SELECTFILENAME		"gameres/gui/images/slotselect.png"
// 设置获取物品时给予界面表现
void dGuiMouseGamePlay::setItemNotify(StringTableEntry IconName)
{
	if(IconName && *IconName != 0)
		mItemNotifyIconFile = GFXTexHandle(IconName, &GFXDefaultGUIProfile);
	else
		mItemNotifyIconFile = NULL;
	if(mItemNotifyIconFile)
		mItemNotifyIconFrame = GFXTexHandle(SELECTFILENAME, &GFXDefaultGUIProfile);
	mItemNotifyCurFrame = 0;
	GuiButtonCtrl* pButton = (GuiButtonCtrl*)Sim::findObject("ItemBagButton");
	if(pButton)
	{
		RectI globalCoord = pButton->getGlobalBounds();
		Point2I wh(globalCoord.len_x()/2, globalCoord.len_y()/2);
		mItemNotifyEndPos = globalCoord.point + wh;
	}
}

// 渲染获取物品提示的ICON
void dGuiMouseGamePlay::drawItemNotify()
{
	if(!mItemNotifyIconFile)
		return;
	Point2I offset;
	RectI size;
	if(mItemNotifyCurFrame <= 40)
	{
		static U32 changeSize = 0; 		
		size.point.x = size.point.y = 0;
		if(changeSize < 10)
		{
			offset.x = mItemNotifyEndPos.x - 150;
			offset.y = mItemNotifyEndPos.y - 120;
			size.extent.x = 48;
			size.extent.y = 48;
		}
		else
		{
			offset.x = mItemNotifyEndPos.x - 158;
			offset.y = mItemNotifyEndPos.y - 128;
			size.extent.x = 64;
			size.extent.y = 64;
		}
		if(changeSize++ >= 20)
			changeSize = 0;
	}
	else
	{
		U32 mFrames = mItemNotifyCurFrame - 40;
		offset.x = mItemNotifyEndPos.x - (150-mFrames*10);
		offset.y = mItemNotifyEndPos.y - (120-mFrames*8);
		size.point.x = 0;
		size.point.y = 0;
		size.extent.x = 64 - (mFrames + 1) * 4;
		size.extent.y = 64 - (mFrames + 1) * 4;
	}
	
	GFX->getDrawUtil()->drawBitmapSR(mItemNotifyIconFile, offset, size);
	if(mItemNotifyIconFile)
	{
		RectI bmpOffset(48 * 8, 0, 48,48);
		GFX->getDrawUtil()->drawBitmapStretchSR(mItemNotifyIconFrame, RectI(offset, size.extent), bmpOffset);
	}	
	mItemNotifyCurFrame ++;

	// 最多55帧
	if(mItemNotifyCurFrame >= 55)
	{
		mItemNotifyIconFile = NULL;
		mItemNotifyIconFrame = NULL;
	}
}

// ====================================================================================
//  导出脚本函数
// ====================================================================================

ConsoleFunction(GetCameraPitch, F32, 1, 1, "obj.GetCameraPitch();")
{
	return dGuiMouseGamePlay::GetCameraPitch();
}

ConsoleFunction(SetCameraPitch, void, 2, 2, "obj.SetCameraPitch();")
{
	dGuiMouseGamePlay::SetCameraPitch(dAtof(argv[1]));
}

ConsoleFunction(GetCameraYaw, F32, 1, 1, "obj.GetCameraYaw();")
{
	return dGuiMouseGamePlay::GetCameraYaw();
}

ConsoleFunction(SetCameraYaw, void, 2, 2, "obj.SetCameraYaw();")
{
	dGuiMouseGamePlay::SetCameraYaw(dAtof(argv[1]));
}

ConsoleFunction(CameraZoomIn, void, 1, 1, "CameraZoomIn()")
{		
	if(g_UIMouseGamePlay)
		g_UIMouseGamePlay->ZoomCamera(true);
}

ConsoleFunction(CameraZoomOut, void, 1, 1, "CameraZoomOut()")
{
	if(g_UIMouseGamePlay)
		g_UIMouseGamePlay->ZoomCamera(false);
}

ConsoleFunction(addDamageNumber, void, 5, 6, "addDamageNumber(%tag, %num, %colorEnum, %pos, %zoom)")//此处颜色为枚举值
{
	if(g_UIMouseGamePlay)
	{
		GameObject* tag = (GameObject*)Sim::findObject(dAtol(argv[1]));
		bool zoom = false;
		if(argc > 5)
			zoom = dAtob(argv[5]);
		g_UIMouseGamePlay->addDamageNumber(tag, dAtol(argv[2]), dAtol(argv[3]), dAtol(argv[4]), zoom);
	}
}

// ConsoleFunction(addDamageText, void, 5, 5, "addDamageText(%tag, %num, %color, %pos)")
// {
// 	if(g_UIMouseGamePlay)
// 	{
// 		GameObject* tag = (GameObject*)Sim::findObject(dAtol(argv[1]));
// 		g_UIMouseGamePlay->addDamageText(tag, argv[2], dAtol(argv[3]), dAtol(argv[4]));
// 	}
// }

ConsoleFunction(setItemNotify, void, 2, 2, "setItemNotify(%iconname)")
{
	if(g_UIMouseGamePlay)
		g_UIMouseGamePlay->setItemNotify(argv[1]);
}

void dGuiMouseGamePlay::initPersistFields()
{
	Parent::initPersistFields();

	addField("StyleName",           TypeGuiProfile,     Offset(m_StyleList[StyleOne],           dGuiMouseGamePlay));
	addField("StyleOhter",          TypeGuiProfile,     Offset(m_StyleList[StyleTwo],           dGuiMouseGamePlay));
	addField("StyleChat",           TypeGuiProfile,     Offset(m_StyleList[StyleThree],         dGuiMouseGamePlay));

	Con::addVariable("IsDragged",	TypeBool,			&m_IsDragged);
}

ConsoleFunction(processPitch, void, 2, 2, "processPitch(%val);")
{
	F32 val = atof(argv[1]);
	dGuiMouseGamePlay::m_IsDragged = val;
	val *= (GameGetCameraFov() / 90) * 0.005;
	dGuiMouseGamePlay::SetCameraPitch( dGuiMouseGamePlay::GetCameraPitch()+val);
}

ConsoleFunction(processYaw, void, 2, 2, "obj.processYaw(%val);")
{
	F32 val = atof(argv[1]);
	val *= (GameGetCameraFov() / 90) * 0.005;

	if(MoveManager::mFreeLook == 0)
		MoveManager::mYaw += val;
	else
		dGuiMouseGamePlay::SetCameraYaw( dGuiMouseGamePlay::GetCameraYaw()+val);
}

#endif