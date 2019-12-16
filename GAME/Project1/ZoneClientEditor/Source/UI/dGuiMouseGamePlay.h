//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "T3D/GameTSCtrl.h"

#ifdef NTJ_CLIENT

#include "sceneGraph/sceneObject.h"
#include "gui/controls/guiBitmapCtrl.h"
#include "gui/controls/guiTextCtrl.h"
#include "gui/core/guiCanvas.h"
#include "gui/missionEditor/editTSCtrl.h"
#include "T3D/moveManager.h"
#include "platform/platform.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "sceneGraph/decalManager.h"
#include "T3D/tsStatic.h"

// ==============================================================================================================================
//  Gui3DMouseEvent
// ==============================================================================================================================
/// 鼠标在屏幕上移动引起的3D事件信息.在editTSCtrl.h中已定义
/// 比如, 鼠标在屏幕上移动的时候对应的坐标为2D坐标, 但是需要通过一些操作将这些2D坐标转换成游戏中的3D坐标, 因此设计该类.
///

#endif


class PrizeBox;
class AnimationObject;
class CommonFontEX;

// ========================================================================================================================================
//  CUIMouseGamePlay
// 这个控件是游戏中的主要控件，用于使用鼠标控制游戏世界的物件对象
//
// 鼠标操作控制说明：
//     当某些情况下鼠标按下时，需要隐藏鼠标（setCursorON(false)），此时为了操作的顺畅
// 需要将鼠标移动消息交由脚本处理
//     当隐藏鼠标时，如果鼠标移动了（m_IsDragged == true），则不响应鼠标按键弹起消息
//
// 注：该控件在editor只是有一个简单的声明，只用于UI界面的制作
// ========================================================================================================================================
//客户端图片位置
#define  GAME_CLIENT_DIRECTORY_IMAGES "gameres/gui/images/"
class dGuiMouseGamePlay: public GameTSCtrl
{
	typedef GameTSCtrl Parent;
#ifdef NTJ_CLIENT
public:
	class MouseCollisionInfo
	{
	public:
		SceneObject*		m_Object;
		Point3F				m_Position;
		VectorF				m_Normal;
		F32					m_t;

		MouseCollisionInfo(void)
		{
			m_Object = NULL;
			m_t = 10.0f;
		}
	};

	enum ViewMode
	{
		ViewMode_Normal		= 0,
		ViewMode_Freelook,					// 摄像机自由旋转
		ViewMode_Follow,					// 摄像机与主角一起自由旋转
		ViewMode_Forward,					// 摄像机与主角一起自由旋转，且主角前进
	};

	enum MouseState
	{
		MouseState_None		= 0,
		MouseState_Left		= BIT(0),
		MouseState_Right	= BIT(1),
		MouseState_Middle	= BIT(2),
	};

	enum
	{
		HpBarFillTexture,
		HpBarBackgroundTexture,
		HpBarbottomTexture, 
		PkstateTexture,
		TeamTexture,
		TeamCaptainTexture,
		TeammateTexture,
		FamilyTexture,

		totalTextureNum,
	};

	enum
	{
		StyleOne,
		StyleTwo,
		StyleThree,

		totalStyleNum,
	};

	enum
	{
		HpOffsetY = 15,
	};

	static const U32	m_MouseCollisionMask;		//鼠标可选的物件碰撞类型
	static const U32	m_VisibilityCollisionMask;	//会阻挡视线的物件类型
	static const U32	m_MouseFloorCollisionMask;	//可以点击的物件类型
	static const U32	m_MouseFloorCollisionMask2;

	static const SimTime	m_MouseAutoMoveTime;		//按下鼠标多久后会自动移动
	static const SimTime	m_MouseAutoMoveTimeInterval;//自动移动间隔

	static bool			m_IsDragged;				// 鼠标是否拖曳了，这时不响应鼠标弹起操作
private:

	class CursorInfo
	{
	public:
		S32					m_Index;
		StringTableEntry	m_Name;
	};
	

private:

	static const F32	m_TargetDecalScale;
	static const F32	m_ProjectDistance;
	static const F32	m_CameraYawSpeed;

	static const bool	m_LockPitch;
	static const F32	m_CameraPitchMin;
	static const F32	m_CameraPitchMax;

	// 固定视角时，拉近视角摄像机渐平的设置
	static const F32	m_StartToPitch;				// 开始变平的位置（在Zoom取值范围内）
	static const F32	m_PitchRadian;				// 最大pitch角度

	static F32			m_fCurrentZoom;				// 当前zoom
	static const F32	m_CameraZoomMax;
	static const F32	m_CameraZoomMin;
	static const F32	m_CameraZoomUnit;
	static const F32	m_CameraZoomTime;			///< Total time for camera zoom.

	static const F32	m_OriginalCameraFov;
	static F32			m_CameraFov;

	static F32			m_fCameraYaw;
	static F32			m_fCameraPitch;
	static F32			m_fCameraZoom;

	static bool			m_IsFirstPerson;			// 与gameConnection::mFirstPerson保持一致
	static bool			m_IsZoomingCamera;
	static bool			m_IsAutoYaw;				// 是否在自动旋转

	MouseCollisionInfo	m_MouseCollisionInfo;
protected:
	bool				m_IsCursorPushed;			// 鼠标移动到某对象时使用Cursor提示
	SimTime				m_LastRenderTime;
	SimTime				m_NextAutoMoveTime;			// 下一次自动点击时间
	SimTime				m_EnableAutoMoveTime;		// 开启自动移动时间
	Gui3DMouseEvent		m_Last3DGuiEvent;

	U32					m_MouseState;				// 鼠标按键的状态
	ViewMode			m_ViewMode;					// 当前视角模式

	Vector<GFXTexHandle> m_TexList;
	GuiControlProfile* m_StyleList[totalStyleNum];  

	SimObjectPtr<PrizeBox>	m_pPrizeBox;
	U32					m_TargetEP;					// 目标光圈特效
	U32					m_PreCastEP;				// 法术光圈特效

    SimObjectPtr<ShapeBase> mLastMouseOverObj;      // 最后一个鼠标移上去高亮的对象
#endif
public:
	DECLARE_CONOBJECT(dGuiMouseGamePlay);

#ifdef NTJ_CLIENT
	static void initPersistFields();
	dGuiMouseGamePlay();
	~dGuiMouseGamePlay();

	MouseCollisionInfo* GetMouseCollisionInfo(void)  { return &m_MouseCollisionInfo;}
	void ClearMouseCollisionInfo(void) { m_MouseCollisionInfo.m_Object = NULL;}

	void Make3DMouseEvent(Gui3DMouseEvent & gui3DMouseEvent, const GuiEvent & event);

	void UpdateMouseState	(const Point2I& pos);
	inline U32 getMouseState() { return m_MouseState;}

	void updateData();

	/// @name		Event Handlers
	/// @{
	bool onAdd		();
	bool onWake		();
	void onSleep	();

	void onMouseMove			(const GuiEvent& event);
	void onMouseEnter			(const GuiEvent& event);
	void onMouseLeave			(const GuiEvent& event);
	void onMouseDown			(const GuiEvent& event);
	void onMouseUp				(const GuiEvent& event);
	void onMouseDragged			(const GuiEvent& event);
	void onRightMouseDown		(const GuiEvent& event);
	void onRightMouseUp			(const GuiEvent& event);
	void onRightMouseDragged	(const GuiEvent& event);
	void onMiddleMouseDown		(const GuiEvent& event);
	void onMiddleMouseUp		(const GuiEvent& event);
	void onMiddleMouseDragged	(const GuiEvent& event);
	bool onMouseWheelUp			(const GuiEvent& event);
	bool onMouseWheelDown		(const GuiEvent& event);

	void on3DMouseMove			(const Gui3DMouseEvent& event);
	void on3DMouseDown			(const Gui3DMouseEvent& event);
	void on3DMouseUp			(const Gui3DMouseEvent& event);
	void on3DMouseDragged		(const Gui3DMouseEvent& event);
	void on3DRightMouseDown		(const Gui3DMouseEvent& event);
	void on3DRightMouseUp		(const Gui3DMouseEvent& event);

	void On3DMouseMove_OnObject(const Gui3DMouseEvent& event, Player* control_player, SceneObject* pSceneObject);
	void On3DMouseMove_SpellSelect(const Gui3DMouseEvent& event, Player* control_player, SceneObject* pSceneObject);
	void On3DLeftMouseDown_OnObject(const Gui3DMouseEvent& event, Player* control_player, SceneObject* pSceneObject);
	void On3DLeftMouseDown_SpellSelect(const Gui3DMouseEvent& event, Player* control_player, SceneObject* pSceneObject);

	/// @}


	/// @name Query Functions
	/// @{
	static F32	GetCameraZoomMin();
	static F32	GetCameraZoomMax();
	static F32	GetCameraPitchMin();
	static F32	GetCameraPitchMax();
	static F32	GetCameraFov();
	/// @}

	// 摄像机设置函数
	static F32	GetCameraPitch();
	static F32	GetCameraYaw()				{return m_fCameraYaw;}
	static F32	GetCameraZoom()				{return m_fCameraZoom;}
	static F32	GetCurrentZoom()			{return m_fCurrentZoom;}
	static F32	GetCurrentFocus(F32 height);
	static void	SetCameraPitch(F32 val);
	static void	SetCameraYaw(F32 val);
	static void	SetCameraZoom(F32 zoom_camera);
	static void	SetCameraFov(Point2I& size);

	static bool	IsFirstPerson()				{return m_IsFirstPerson;}
	static void	SetFirstPerson(bool val)	{m_IsFirstPerson = val;}

	static bool	IsZoomingCamera()			{return m_IsZoomingCamera;}
	static void	SetZoomingCamera(bool val)	{m_IsZoomingCamera = val;}

	bool		processCameraQuery(CameraQuery *camq);
	void		SetAutoYaw(bool val);
	void		UpdateAutoYaw(F32 dt);
	void		UpdateAutoMove();
	void		DisableAutoMove() { m_NextAutoMoveTime = 0xFFFFFFFF; m_EnableAutoMoveTime = 0xFFFFFFFF;}
	inline void        setMouseState(MouseState state) { m_MouseState = state; }
	/// @name zoom
	/// @{
	void		ZoomCamera(bool in_out);
	void		UpdateZoom(F32 dt, F32& CurrentZoom);
	/// @}

	// 鼠标碰撞
	bool		IsMouseCollide(const Gui3DMouseEvent& event, MouseCollisionInfo& collision, U32 mask, bool bUseBox = true, Container::castCallback = NULL);

	// 渲染函数
	void		onPreRender();
	void		renderWorld(const RectI &updateRect);

	// Decal
	void		RefreshTargetDecal();
	void		RefreshPreCastDecal();

	//头顶信息
	void drawGameObjectMessages();

	//掉物信息
	void drawPrizeItem(PrizeBox *pBox);

	CommonFontEX* mDamageNumberSmallFont;	// 渲染数字的小字体对象
	CommonFontEX* mDamageNumberBigFont;		// 渲染数字的大字体对象
	CommonFontEX* mDamageTextSmallFont;		// 渲染文本的小字体对象
	CommonFontEX* mDamageTextBigFont;		// 渲染文本的大字体对象
	void addDamageText(GameObject* target,  GFXTexHandle* Tex);
	void addDamageNumber(GameObject* target, S32 number, U32 color, U32 pos, bool zoom = false);
	void addExpText(GameObject* target,  bool flag, U32 number);
	
	AnimationObject* aniObj;

	GFXTexHandle	mItemNotifyIconFile;				// 物品提示的ICON文件
	GFXTexHandle	mItemNotifyIconFrame;				// 物品提示的ICON边框
	U32				mItemNotifyCurFrame;				// 物品提示的当前帧数
	Point2I			mItemNotifyEndPos;					// 物品提示终点位置
	void setItemNotify(StringTableEntry IconName);		// 设置物品提示相关参数
	void drawItemNotify();								// 渲染物品提示
#endif

};

#ifdef NTJ_CLIENT

extern dGuiMouseGamePlay* g_UIMouseGamePlay;
#include "dGuiMouseGamePlay_inline.h"

#endif
