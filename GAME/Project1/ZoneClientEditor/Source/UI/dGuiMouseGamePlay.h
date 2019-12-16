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
/// �������Ļ���ƶ������3D�¼���Ϣ.��editTSCtrl.h���Ѷ���
/// ����, �������Ļ���ƶ���ʱ���Ӧ������Ϊ2D����, ������Ҫͨ��һЩ��������Щ2D����ת������Ϸ�е�3D����, �����Ƹ���.
///

#endif


class PrizeBox;
class AnimationObject;
class CommonFontEX;

// ========================================================================================================================================
//  CUIMouseGamePlay
// ����ؼ�����Ϸ�е���Ҫ�ؼ�������ʹ����������Ϸ������������
//
// ����������˵����
//     ��ĳЩ�������갴��ʱ����Ҫ������꣨setCursorON(false)������ʱΪ�˲�����˳��
// ��Ҫ������ƶ���Ϣ���ɽű�����
//     ���������ʱ���������ƶ��ˣ�m_IsDragged == true��������Ӧ��갴��������Ϣ
//
// ע���ÿؼ���editorֻ����һ���򵥵�������ֻ����UI���������
// ========================================================================================================================================
//�ͻ���ͼƬλ��
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
		ViewMode_Freelook,					// �����������ת
		ViewMode_Follow,					// �����������һ��������ת
		ViewMode_Forward,					// �����������һ��������ת��������ǰ��
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

	static const U32	m_MouseCollisionMask;		//����ѡ�������ײ����
	static const U32	m_VisibilityCollisionMask;	//���赲���ߵ��������
	static const U32	m_MouseFloorCollisionMask;	//���Ե�����������
	static const U32	m_MouseFloorCollisionMask2;

	static const SimTime	m_MouseAutoMoveTime;		//��������ú���Զ��ƶ�
	static const SimTime	m_MouseAutoMoveTimeInterval;//�Զ��ƶ����

	static bool			m_IsDragged;				// ����Ƿ���ҷ�ˣ���ʱ����Ӧ��굯�����
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

	// �̶��ӽ�ʱ�������ӽ��������ƽ������
	static const F32	m_StartToPitch;				// ��ʼ��ƽ��λ�ã���Zoomȡֵ��Χ�ڣ�
	static const F32	m_PitchRadian;				// ���pitch�Ƕ�

	static F32			m_fCurrentZoom;				// ��ǰzoom
	static const F32	m_CameraZoomMax;
	static const F32	m_CameraZoomMin;
	static const F32	m_CameraZoomUnit;
	static const F32	m_CameraZoomTime;			///< Total time for camera zoom.

	static const F32	m_OriginalCameraFov;
	static F32			m_CameraFov;

	static F32			m_fCameraYaw;
	static F32			m_fCameraPitch;
	static F32			m_fCameraZoom;

	static bool			m_IsFirstPerson;			// ��gameConnection::mFirstPerson����һ��
	static bool			m_IsZoomingCamera;
	static bool			m_IsAutoYaw;				// �Ƿ����Զ���ת

	MouseCollisionInfo	m_MouseCollisionInfo;
protected:
	bool				m_IsCursorPushed;			// ����ƶ���ĳ����ʱʹ��Cursor��ʾ
	SimTime				m_LastRenderTime;
	SimTime				m_NextAutoMoveTime;			// ��һ���Զ����ʱ��
	SimTime				m_EnableAutoMoveTime;		// �����Զ��ƶ�ʱ��
	Gui3DMouseEvent		m_Last3DGuiEvent;

	U32					m_MouseState;				// ��갴����״̬
	ViewMode			m_ViewMode;					// ��ǰ�ӽ�ģʽ

	Vector<GFXTexHandle> m_TexList;
	GuiControlProfile* m_StyleList[totalStyleNum];  

	SimObjectPtr<PrizeBox>	m_pPrizeBox;
	U32					m_TargetEP;					// Ŀ���Ȧ��Ч
	U32					m_PreCastEP;				// ������Ȧ��Ч

    SimObjectPtr<ShapeBase> mLastMouseOverObj;      // ���һ���������ȥ�����Ķ���
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

	// ��������ú���
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

	// �����ײ
	bool		IsMouseCollide(const Gui3DMouseEvent& event, MouseCollisionInfo& collision, U32 mask, bool bUseBox = true, Container::castCallback = NULL);

	// ��Ⱦ����
	void		onPreRender();
	void		renderWorld(const RectI &updateRect);

	// Decal
	void		RefreshTargetDecal();
	void		RefreshPreCastDecal();

	//ͷ����Ϣ
	void drawGameObjectMessages();

	//������Ϣ
	void drawPrizeItem(PrizeBox *pBox);

	CommonFontEX* mDamageNumberSmallFont;	// ��Ⱦ���ֵ�С�������
	CommonFontEX* mDamageNumberBigFont;		// ��Ⱦ���ֵĴ��������
	CommonFontEX* mDamageTextSmallFont;		// ��Ⱦ�ı���С�������
	CommonFontEX* mDamageTextBigFont;		// ��Ⱦ�ı��Ĵ��������
	void addDamageText(GameObject* target,  GFXTexHandle* Tex);
	void addDamageNumber(GameObject* target, S32 number, U32 color, U32 pos, bool zoom = false);
	void addExpText(GameObject* target,  bool flag, U32 number);
	
	AnimationObject* aniObj;

	GFXTexHandle	mItemNotifyIconFile;				// ��Ʒ��ʾ��ICON�ļ�
	GFXTexHandle	mItemNotifyIconFrame;				// ��Ʒ��ʾ��ICON�߿�
	U32				mItemNotifyCurFrame;				// ��Ʒ��ʾ�ĵ�ǰ֡��
	Point2I			mItemNotifyEndPos;					// ��Ʒ��ʾ�յ�λ��
	void setItemNotify(StringTableEntry IconName);		// ������Ʒ��ʾ��ز���
	void drawItemNotify();								// ��Ⱦ��Ʒ��ʾ
#endif

};

#ifdef NTJ_CLIENT

extern dGuiMouseGamePlay* g_UIMouseGamePlay;
#include "dGuiMouseGamePlay_inline.h"

#endif
