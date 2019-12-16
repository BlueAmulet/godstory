//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include <time.h>
#include <string>
#include <vector>
#include <list>
#include "gui/controls/guiBitmapCtrl.h"
#include "gui/controls/guiTextCtrl.h"
#include "gui/core/guiCanvas.h"
#include "T3D/moveManager.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/ClientGameplayAction.h"


// ========================================================================================================================================
//	ClientGameplayState
// ========================================================================================================================================
///	Client gameplay state class.
///
struct stServerInfo
{
	int GateId;
	int Ip;
	int Port;
};

struct stServerList
{
	int LineId;
	std::vector<stServerInfo> GateList;
};

class ClientGameplayState;

//GM返回值处理基类
class CGMRespHandler
{
public:
    virtual ~CGMRespHandler(void) {}
    virtual void OnResp(ClientGameplayState* pClient,const char* cmdName,int error,int sessionId,Base::BitStream& resps) = 0;
};

class TCPObject2;
class UserPacketProcess;
class SFXSource;
class VocalStatus;

class ClientGameplayState
{
	//客户端本地资源
	StringTableEntry			mClientMisFileName;
	U32							mClientMisFileCrc;
	Vector<U32>					mClientTerrFileCrc;

	char						m_LoginStatue;							//登录服务器状态
	TCPObject2*					m_pGateLink;							//服务器连接
	int							m_AccountId;
	char						m_AccountName[COMMON_STRING_LENGTH];	//登录帐号
	char						m_PWD[COMMON_STRING_LENGTH];			//密码
	T_UID						m_UID;									//Secation
	int							m_CurrentPlayerId;						//当前选择需要登录的玩家ID
	int							m_CurrentZoneId;						//当前选择需要登录的地图服务器
	char						m_CurrentIpPort[COMMON_STRING_LENGTH];	//当前需要连接的服务器
	int 						m_LoginGateIp;							//当前连接的网关服务器IP
	int 						m_LoginGatePort;						//当前连接的网关服务器Port
	int							m_LoginZoneIp;							//当前连接的地图服务器IP
	int							m_LoginZonePort;						//当前连接的地图服务器PORT
	int							m_CurrentLineId;						//当前线路编号
	int							m_CopyMapZoneIp;						//当前副本地图服务器IP
	int							m_CopyMapZonePort;						//当前副本地图服务器Port
	int							m_CopymapLayer;							//当前副本层

	char						m_CrashReportEmail[COMMON_STRING_LENGTH];// CrashReport汇报的Email地址
	char						m_CrashReportFtp[COMMON_STRING_LENGTH];	// CrashReport汇报的FTP地址
	char						m_GameVersion[COMMON_STRING_LENGTH];	// 游戏主版本

	bool						m_LastFrame;							//是否需要更新一下最终画面
	bool						m_Transporting;							//是否正在传送中
	bool						m_LineSwitching;						//是否正在切换线路
	bool						m_DisconnectFlag;						//是否在下一个循环后断开连接

    typedef stdext::hash_map<std::string,CGMRespHandler*> GM_RESP_HANDLERS;
    GM_RESP_HANDLERS            m_gmRespHandlers;

	bool						m_RefreshPlayerModelView;				//是否刷新玩家模型浏览
public:
	std::vector<stServerList>	m_ServerList;							//登录线程列表
	std::vector<stPlayerSelect> m_PlayerList;							//角色列表
	bool						m_isRobotMode; 
    std::string                 m_sClipboard;                           //剪贴板
    std::list<std::string>      m_BanNamelist;                          //屏蔽列表
	typedef stdext::hash_map<U32,GameObject*>   objectListMap;
	objectListMap m_ObjectList;

	ClientGameplayAction*								mCurrentAction;
	ClientGameplayAction*								mTempAction;					// 用来存储一个将要执行的临时action	

	bool m_isPwdChecked;
public:

	static bool				m_bMouseOperation;				//是否鼠标点击操作
	static bool             m_bAutoResetCamera;             //摄像机是否自动复位
	static bool				m_bCameraDragged;				//摄像机是否被鼠标拖拽
	static bool				m_bAutoRun;						//是否自动前进
	static bool				m_bMouseRun;					//是否自动前进，有鼠标控制
	static bool				m_bIsPlayerQuit;				//是否是玩家主动退出游戏
	static __time32_t       mServerInitTime;                //服务器发送时间
	static SimTime          mClinetUpdateTime;              //服务器更新到客户端时间
public:
	bool                        m_bShowSearchPath;                      //是否显示寻径路径
	std::list<Point3F>          m_SearchPath;                           //寻径路径 
	bool                        m_bNavigation;                          //是否寻径中
	U32							mFindPathID;							//寻径ID
	std::vector<U32>			mStrideServerPath;						//跨服寻径ID
	bool						mbStrideServer;							//跨服寻径标志
	VocalStatus*                pVocalStatus;                           //吟唱状态
	bool						m_isInCopymap;							//是否在副本中
	bool						m_isGoingCopymap;						//是否进入副本中
	bool                        m_bDarwPath;                            //绘制寻径路径

	void						setStrideServerPathFlag(bool flag) { mbStrideServer = flag; }
	bool						getStrideServerPathFlag() { return mbStrideServer; }

	void                        setPath(std::list<Point3F>& path)  { m_SearchPath = path;}
	void						setStrideServerPath(std::vector<U32>& path) { mStrideServerPath = path; }
	std::vector<U32>&			getStrideServerPath() { return mStrideServerPath; }
	std::list<Point3F>&         getPathList()          {return m_SearchPath;}
	void                        clearPath();                            //清除寻径信息
	bool                        isShowSearchPath()     {return m_bShowSearchPath;}
	void                        setShowSearchPath(bool value = false)    { m_bShowSearchPath = value;}
	void                        setStartNavigation(bool value = false)   { m_bNavigation = value;}
	bool                        isStartNavigation()    { return m_bNavigation; }
	void						setFindPathID(U32 pathID){ mFindPathID = pathID; }
	U32							findFindPathID() { return mFindPathID; }
	void						endFindPath();
	void                        setDarwPath(bool bValue = false)   { m_bDarwPath = bValue;}
	bool                        getDarwPath()  { return m_bDarwPath;}   
    void                        setClipboard(std::string clipstr)   { m_sClipboard = clipstr;}
    std::string                 getClipboard()  { return m_sClipboard;}
	
	SFXSource*			mGlobalSound;
public:

	// ------------------------------------------------------------------------------------------------------------------------------------
	//	Constructor & Destructor
	//
	ClientGameplayState										();
	~ClientGameplayState									();

	void					Initialize						();
	void					Shutdown						();
	void					preShutdown						();
	void					prepRenderImage					(S32 StateKey);
	void					TimeLoop						(S32 timeDelta);
	__time32_t              getSetverTime                   ();
	void                    getGameFormatTime               (Platform::LocalTime &lt);

	// 客户端本地资源
	void					setClientMisFileName			(StringTableEntry MisfileName);
	void					setClientMisFileCRC				(U32 MisCrc)					{mClientMisFileCrc  = MisCrc;		}
	void					setClientTerrFileCRC			(Vector<U32>& TerrCrc)			{mClientTerrFileCrc = TerrCrc;		}
	StringTableEntry		getClientMisFileName			()								{return mClientMisFileName;}
	U32						getClientMisFileCRC				()								{return mClientMisFileCrc; }
	bool					CheckClientTerrFileCRC			(U32 TerrCrc);
	bool					LoadClientIni					();
	StringTableEntry		GetGameVersion					() { return m_GameVersion;}
	void					clearResource					();

	//登录管理
	void					setAccountId					(int accountId)					{m_AccountId = accountId;}
	int						getAccountId					()								{return m_AccountId;}
	void					setAccountName					(const char *name)				{dStrcpy(m_AccountName, sizeof(m_AccountName), name);}
	const char*				getAccountName					()								{return m_AccountName;}
	void					setAccountPWD					(const char *pwd)				{dStrcpy(m_PWD, sizeof(m_PWD), pwd);}
	const char*				getAccountPWD					()								{return m_PWD;}
	void					setAccountUID					(T_UID UID)						{m_UID=UID;}
	T_UID					getAccountUID					()								{return m_UID;}
	void					setSelectedPlayerId				(int PlayerId)					{m_CurrentPlayerId = PlayerId;}
	int						getSelectedPlayerId				()								{return m_CurrentPlayerId;}
	void					setCurrentZoneId				(int ZoneId)					{m_CurrentZoneId = ZoneId;}
	int						getCurrentZoneId				()								{return m_CurrentZoneId;}
	void					setLoginStatus					(char Status)					{m_LoginStatue=Status;}
	char					getLoginStatus					()								{return m_LoginStatue;}
	void					setCurrentNetAddr				(const char *addr)				{dStrcpy(m_CurrentIpPort, sizeof(m_CurrentIpPort), addr);}
	const char*				getCurrentNetAddr				()								{return m_CurrentIpPort;}
	void					setGateIP						(int IP)						{m_LoginGateIp = IP;}
	int						getGateIP						()								{return m_LoginGateIp;}
	void					setGatePort						(int Port)						{m_LoginGatePort = Port;}
	int						getGatePort						()								{return m_LoginGatePort;}
	void					setZoneIP						(int IP)						{m_LoginZoneIp = IP;}
	int						getZoneIP						()								{return m_LoginZoneIp;}
	void					setZonePort						(int Port)						{m_LoginZonePort = Port;}
	int						getZonePort						()								{return m_LoginZonePort;}
	void					setCopymapZoneIp				(int Ip)						{m_CopyMapZoneIp = Ip;}
	int						getCopymapZoneIp				()								{return m_CopyMapZoneIp;}
	void					setCopymapZonePort				(int Port)						{m_CopyMapZonePort= Port;}
	int						getCopymapZonePort				()								{return m_CopyMapZonePort;}
	void					setCurrentLineId				(int LineId)					{m_CurrentLineId = LineId;}
	int						getCurrentLineId				()								{return m_CurrentLineId;}
	bool					isPlayerQuit					()								{return m_bIsPlayerQuit;}
	void					setPlayerQuit					(bool flag)						{m_bIsPlayerQuit = flag;}
	void					setDisconnectZone				(bool flag)						{m_DisconnectFlag = flag;}
	void					setGoingCopyMap					(bool flag)						{m_isGoingCopymap = flag;}
	bool					isGoingCopyMap					()								{return m_isGoingCopymap;}
	int						getCopymapLayer					()								{return m_CopymapLayer;}
	void					setCopymapLayer					(int layer)						{m_CopymapLayer = layer;}
	bool					isInCopymap						()								{return m_isInCopymap;}
	void					setInCopymap					(bool flag)						{m_isInCopymap = flag;}
	
	void					ConnectServer					(const char *addr,char Status);
	void					DisconnectServer				();
	void					onDisconnect					();
	UserPacketProcess*		GetPacketProcess				();

	//随机取线
	void					setTransporting					(bool flag=true)				{m_Transporting = flag; m_LastFrame=false;}
	bool					isTransporting					()								{return m_Transporting;}
	void					setLastFrameOk					()								{m_LastFrame = true;}
	bool					isLastFrameOk					()								{return m_LastFrame;}

	void					setLineSwitching				(bool flag=true);
	bool					isLineSwitching					()								{return m_LineSwitching;}
	void					RandomGate						(int LineId)  ;

	bool					isTick;

	// 取得所控制的对象
	GameBase*				GetControlObject				();
	Player*					GetControlPlayer				();

	GameObject*				GetTarget						();
	SceneObject*			GetInteraction					();

    objectListMap&          getObjectList                   ()                              {return m_ObjectList;}
	void                    setObjectList                   (U32 ID, GameObject* object);
	void                    removeObjectList                (U32 ID);
	void					clearObjectList();
	Player*					findPlayer						(U32 PlayerID);

	// 模型浏览
	void inline				setPlayerModelView				() { m_RefreshPlayerModelView = true;}
	void					refreshPlayerModelView			();
	void					refreshPlayerModelAction		();

	void					setTryEquipOk					(S32 index);

    /************************************************************************/
    /* 发送GM命令
    /************************************************************************/
    bool SendGMCommand(int sessionId,Base::BitStream* pack,const char* cmdName,const char* fmt,...);
    bool SendGMCommand(int sessionId,Base::BitStream* pack,const char* fun);

    //处理GM返回结果
	bool HandleGMCommandResp(int sessionId,Base::BitStream &RecvPacket);

    //注册GM返回值处理
    template<typename _Ty>
    void RegisterGMRespHandler(const char* cmdName)
    {
        static _Ty local;

        if (0 == cmdName)
            return;

        m_gmRespHandlers[cmdName] = &local;
    }

    void ClearGMRespHandlers(void);

	static ClientGameplayState* Instance(void);
public:
	//dGuiShortCut栏位对象的操作
	enum MouseEventType
	{
		SLOT_NONE,								// 无动作
		SLOT_LEFTMOUSEDOWN,						// 鼠标左键按下
		SLOT_LEFTMOUSEDRAG,						// 鼠标左键拖动
		SLOT_LEFTMOUSEUP,						// 鼠标左键抬起
		SLOT_SHIFT_LEFTMOUSEDOWN,				// SHIFT + 鼠标左键按下
		SLOT_SHIFT_LEFTMOUSEUP,					// SHIFT + 鼠标左键抬起
		SLOT_RIGHTMOUSEDOWN,					// 鼠标右键按下
		SLOT_RIGHTMOUSEDRAG,					// 鼠标右键拖动
		SLOT_RIGHTMOUSEUP,						// 鼠标右键抬起
		SLOT_SHIFT_RIGHTMOUSEDOWN,				// SHIFT + 鼠标右键按下
	};

	struct SelectShortcutInfo
	{
		S32 type;
		//S32 row;
		S32 col;
		MouseEventType event;
		S32 nums;	// 物品的数量
		S32 price;	// 物品的价格		
	};

	SelectShortcutInfo	mSelectShortcut;

	void	setSelectShortcutSlot	(S32 type, S32 col, MouseEventType event, S32 num = 0, S32 price = 0);
	bool	isSelectShortcutSlot	(S32 type,  S32 col);
	void	clearSelectShortcutSlot	();
	bool	isShortcutSlotBeSelect	();
	S32		getSelectShortcutType	();
	S32		getSelectShortcutCol	();

	MouseEventType	getSelectShortcutMouseEvent	();
	S32		getSelectShortcutNum	();
	S32		getSelectShortcutPrice	();

	//Action
	void	setCurrentAction(ClientGameplayAction* action);
	ClientGameplayAction* getCurrentAction() const;
	void	setCurrentActionParam(ClientGameplayParam* param);
	void	cancelCurrentAction();
	void	doneCurrentAction();

	ClientGameplayAction* getTempAction();
	void    setTempAction(ClientGameplayAction* action);
	bool	setTempAcrionParam(ClientGameplayParam* param);
	void	clearTempAction();

	void	setTempToCurrentAction();
	void	setCurrentCursor(StringTableEntry iconName);
	void	popCursor();
	//全局声音
	void					setGlobalSound					(SFXSource* source);
	void					deleteGlobalSound				() ;

	struct stMsgParam
	{
		char			 message[128];
		U32				 mode;			// 消息模式(0-普通, 1-跑马灯 2-闪动 3-渐隐 4-爆炸)
		U32				 limit;			// 限制类型(0-限时 1-限次)
		U32				 time;			// 限时秒数或限次次数
		U32				 fonttype;		// 字体类型(0-宋体 1-黑体 2-楷体_GB2312 3-隶书)
		U32				 fontcolor;		// 字体颜色
		U32				 fontsize;		// 字体大小
		U32				 posx;			// 屏幕位置x坐标
		U32				 posy;			// 屏幕位置y坐标
		stMsgParam()
		{
			message[0]  = 0;
			mode		= 0;
			limit		= 0;
			time		= 10;
			fonttype	= 1;
			fontcolor	= 16711680;
			fontsize	= 16;
		}
	};

	void OutputScreenMessage(stMsgParam& param);
	void Cleanup();
};

//定义游戏世界状态全局对象
extern ClientGameplayState* g_ClientGameplayState;

