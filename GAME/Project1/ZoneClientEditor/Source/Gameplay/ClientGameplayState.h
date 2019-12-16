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

//GM����ֵ�������
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
	//�ͻ��˱�����Դ
	StringTableEntry			mClientMisFileName;
	U32							mClientMisFileCrc;
	Vector<U32>					mClientTerrFileCrc;

	char						m_LoginStatue;							//��¼������״̬
	TCPObject2*					m_pGateLink;							//����������
	int							m_AccountId;
	char						m_AccountName[COMMON_STRING_LENGTH];	//��¼�ʺ�
	char						m_PWD[COMMON_STRING_LENGTH];			//����
	T_UID						m_UID;									//Secation
	int							m_CurrentPlayerId;						//��ǰѡ����Ҫ��¼�����ID
	int							m_CurrentZoneId;						//��ǰѡ����Ҫ��¼�ĵ�ͼ������
	char						m_CurrentIpPort[COMMON_STRING_LENGTH];	//��ǰ��Ҫ���ӵķ�����
	int 						m_LoginGateIp;							//��ǰ���ӵ����ط�����IP
	int 						m_LoginGatePort;						//��ǰ���ӵ����ط�����Port
	int							m_LoginZoneIp;							//��ǰ���ӵĵ�ͼ������IP
	int							m_LoginZonePort;						//��ǰ���ӵĵ�ͼ������PORT
	int							m_CurrentLineId;						//��ǰ��·���
	int							m_CopyMapZoneIp;						//��ǰ������ͼ������IP
	int							m_CopyMapZonePort;						//��ǰ������ͼ������Port
	int							m_CopymapLayer;							//��ǰ������

	char						m_CrashReportEmail[COMMON_STRING_LENGTH];// CrashReport�㱨��Email��ַ
	char						m_CrashReportFtp[COMMON_STRING_LENGTH];	// CrashReport�㱨��FTP��ַ
	char						m_GameVersion[COMMON_STRING_LENGTH];	// ��Ϸ���汾

	bool						m_LastFrame;							//�Ƿ���Ҫ����һ�����ջ���
	bool						m_Transporting;							//�Ƿ����ڴ�����
	bool						m_LineSwitching;						//�Ƿ������л���·
	bool						m_DisconnectFlag;						//�Ƿ�����һ��ѭ����Ͽ�����

    typedef stdext::hash_map<std::string,CGMRespHandler*> GM_RESP_HANDLERS;
    GM_RESP_HANDLERS            m_gmRespHandlers;

	bool						m_RefreshPlayerModelView;				//�Ƿ�ˢ�����ģ�����
public:
	std::vector<stServerList>	m_ServerList;							//��¼�߳��б�
	std::vector<stPlayerSelect> m_PlayerList;							//��ɫ�б�
	bool						m_isRobotMode; 
    std::string                 m_sClipboard;                           //������
    std::list<std::string>      m_BanNamelist;                          //�����б�
	typedef stdext::hash_map<U32,GameObject*>   objectListMap;
	objectListMap m_ObjectList;

	ClientGameplayAction*								mCurrentAction;
	ClientGameplayAction*								mTempAction;					// �����洢һ����Ҫִ�е���ʱaction	

	bool m_isPwdChecked;
public:

	static bool				m_bMouseOperation;				//�Ƿ����������
	static bool             m_bAutoResetCamera;             //������Ƿ��Զ���λ
	static bool				m_bCameraDragged;				//������Ƿ������ק
	static bool				m_bAutoRun;						//�Ƿ��Զ�ǰ��
	static bool				m_bMouseRun;					//�Ƿ��Զ�ǰ������������
	static bool				m_bIsPlayerQuit;				//�Ƿ�����������˳���Ϸ
	static __time32_t       mServerInitTime;                //����������ʱ��
	static SimTime          mClinetUpdateTime;              //���������µ��ͻ���ʱ��
public:
	bool                        m_bShowSearchPath;                      //�Ƿ���ʾѰ��·��
	std::list<Point3F>          m_SearchPath;                           //Ѱ��·�� 
	bool                        m_bNavigation;                          //�Ƿ�Ѱ����
	U32							mFindPathID;							//Ѱ��ID
	std::vector<U32>			mStrideServerPath;						//���Ѱ��ID
	bool						mbStrideServer;							//���Ѱ����־
	VocalStatus*                pVocalStatus;                           //����״̬
	bool						m_isInCopymap;							//�Ƿ��ڸ�����
	bool						m_isGoingCopymap;						//�Ƿ���븱����
	bool                        m_bDarwPath;                            //����Ѱ��·��

	void						setStrideServerPathFlag(bool flag) { mbStrideServer = flag; }
	bool						getStrideServerPathFlag() { return mbStrideServer; }

	void                        setPath(std::list<Point3F>& path)  { m_SearchPath = path;}
	void						setStrideServerPath(std::vector<U32>& path) { mStrideServerPath = path; }
	std::vector<U32>&			getStrideServerPath() { return mStrideServerPath; }
	std::list<Point3F>&         getPathList()          {return m_SearchPath;}
	void                        clearPath();                            //���Ѱ����Ϣ
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

	// �ͻ��˱�����Դ
	void					setClientMisFileName			(StringTableEntry MisfileName);
	void					setClientMisFileCRC				(U32 MisCrc)					{mClientMisFileCrc  = MisCrc;		}
	void					setClientTerrFileCRC			(Vector<U32>& TerrCrc)			{mClientTerrFileCrc = TerrCrc;		}
	StringTableEntry		getClientMisFileName			()								{return mClientMisFileName;}
	U32						getClientMisFileCRC				()								{return mClientMisFileCrc; }
	bool					CheckClientTerrFileCRC			(U32 TerrCrc);
	bool					LoadClientIni					();
	StringTableEntry		GetGameVersion					() { return m_GameVersion;}
	void					clearResource					();

	//��¼����
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

	//���ȡ��
	void					setTransporting					(bool flag=true)				{m_Transporting = flag; m_LastFrame=false;}
	bool					isTransporting					()								{return m_Transporting;}
	void					setLastFrameOk					()								{m_LastFrame = true;}
	bool					isLastFrameOk					()								{return m_LastFrame;}

	void					setLineSwitching				(bool flag=true);
	bool					isLineSwitching					()								{return m_LineSwitching;}
	void					RandomGate						(int LineId)  ;

	bool					isTick;

	// ȡ�������ƵĶ���
	GameBase*				GetControlObject				();
	Player*					GetControlPlayer				();

	GameObject*				GetTarget						();
	SceneObject*			GetInteraction					();

    objectListMap&          getObjectList                   ()                              {return m_ObjectList;}
	void                    setObjectList                   (U32 ID, GameObject* object);
	void                    removeObjectList                (U32 ID);
	void					clearObjectList();
	Player*					findPlayer						(U32 PlayerID);

	// ģ�����
	void inline				setPlayerModelView				() { m_RefreshPlayerModelView = true;}
	void					refreshPlayerModelView			();
	void					refreshPlayerModelAction		();

	void					setTryEquipOk					(S32 index);

    /************************************************************************/
    /* ����GM����
    /************************************************************************/
    bool SendGMCommand(int sessionId,Base::BitStream* pack,const char* cmdName,const char* fmt,...);
    bool SendGMCommand(int sessionId,Base::BitStream* pack,const char* fun);

    //����GM���ؽ��
	bool HandleGMCommandResp(int sessionId,Base::BitStream &RecvPacket);

    //ע��GM����ֵ����
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
	//dGuiShortCut��λ����Ĳ���
	enum MouseEventType
	{
		SLOT_NONE,								// �޶���
		SLOT_LEFTMOUSEDOWN,						// ����������
		SLOT_LEFTMOUSEDRAG,						// �������϶�
		SLOT_LEFTMOUSEUP,						// ������̧��
		SLOT_SHIFT_LEFTMOUSEDOWN,				// SHIFT + ����������
		SLOT_SHIFT_LEFTMOUSEUP,					// SHIFT + ������̧��
		SLOT_RIGHTMOUSEDOWN,					// ����Ҽ�����
		SLOT_RIGHTMOUSEDRAG,					// ����Ҽ��϶�
		SLOT_RIGHTMOUSEUP,						// ����Ҽ�̧��
		SLOT_SHIFT_RIGHTMOUSEDOWN,				// SHIFT + ����Ҽ�����
	};

	struct SelectShortcutInfo
	{
		S32 type;
		//S32 row;
		S32 col;
		MouseEventType event;
		S32 nums;	// ��Ʒ������
		S32 price;	// ��Ʒ�ļ۸�		
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
	//ȫ������
	void					setGlobalSound					(SFXSource* source);
	void					deleteGlobalSound				() ;

	struct stMsgParam
	{
		char			 message[128];
		U32				 mode;			// ��Ϣģʽ(0-��ͨ, 1-����� 2-���� 3-���� 4-��ը)
		U32				 limit;			// ��������(0-��ʱ 1-�޴�)
		U32				 time;			// ��ʱ�������޴δ���
		U32				 fonttype;		// ��������(0-���� 1-���� 2-����_GB2312 3-����)
		U32				 fontcolor;		// ������ɫ
		U32				 fontsize;		// �����С
		U32				 posx;			// ��Ļλ��x����
		U32				 posy;			// ��Ļλ��y����
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

//������Ϸ����״̬ȫ�ֶ���
extern ClientGameplayState* g_ClientGameplayState;

