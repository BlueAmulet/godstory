#ifndef _ACCOUNTHANDLER_H_
#define _ACCOUNTHANDLER_H_

#include <string>
#include <hash_map>
#include <set>
#include <time.h>
#include "Common/TimerStateMachine.h"
#include "Common/PlayerStruct.h"
#include "base/memPool.h"

#include <WinSock2.h>
#include <windows.h>
#include "WorldServer.h"
#include "Common/RefHandle.h"

struct stPlayerData : public RefObject< stPlayerData >
{
	stPlayerBase		BaseData;
	stPlayerDisp		DispData;

    stPlayerMain*		m_pMainData;
    
	stPlayerData()
    {
        memset(&BaseData,0,sizeof(stPlayerBase));
		memset(&DispData,0,sizeof(stPlayerDisp));
		m_pMainData = NULL;
        
		BaseData.InitData();
		DispData.InitData();
    }

	~stPlayerData()
	{
		if( m_pMainData )
			MEMPOOL->FreeObj<stPlayerMain>( m_pMainData );
	}

	template<class T>
	bool WriteData(T* pStream)
	{
		return BaseData.WriteData<T>(pStream) && DispData.WriteData<T>(pStream);
	}

	template<class T>
	bool ReadData(T* pStream)
	{
		return BaseData.ReadData<T>(pStream) && DispData.ReadData<T>(pStream);
	}
};

typedef RefHandle<stPlayerData> PlayerDataRef;

//---------------------------------------------------------------
// stPlayerData的引用计数封装，解决多线程删除问题
//---------------------------------------------------------------


enum ePlayerEvent
{
    EVENT_PLAYER_OFFLINE,
};

#define PLAYER_TIMEOUT 400

struct stPacketHead;
class Base::BitStream;

/************************************************************************/
/* 帐号和帐号对应玩家的封装
/************************************************************************/
class stAccountInfo
{
public:
    //帐号的状态
    enum Status
    {
        STATUS_IDEL              = 0, //空闲状态
        STATUS_LOGIN_ACCOUNT,         //客户端登入,包括帐号验证和game验证
        STATUS_IN_SELECT,             //角色选择
        STATUS_LOGIN_GAME,            //登入游戏
        STATUS_IN_GAME,               //在游戏世界里
        STATUS_MAP_SWITCH,            //切换地图
        STATUS_DELAY_DEL,             //等待帐号删除，帐号退出后，会在过1段时间后被删除

		STATUS_ID_CODE,				  //验证码状态

        STATUS_COUNT,
    };

	char mIdCode[COMMON_STRING_LENGTH];   // 验证码

    //玩家数据封装
    struct Player
    {
    public:
        //帐号对象
        stAccountInfo*  m_pAccount;

        //玩家数据库中的展开数据
        stPlayerData *	pPlayerData;

        //以下是一些临时控制变量
        bool			isDispDataInited;
        bool			isMainDataInited;

        ////以下为一些逻辑对象，可能和pPlayerData中的有重复
        //int				m_TeamRule;			//队伍分配规则
        //int				m_TeamId;			//队伍id
        //bool			m_bTeamSwitch;		//组队开关
        //std::set<int>	m_TeamRPendingList;	//组队收到的申请列表
        //std::set<int>	m_TeamBPendingList;	//组队发起的申请列表

        //玩家当前的位置
        float           m_curX,m_curY,m_curZ;

        Player(void)
        {
            m_pAccount       = 0;
            pPlayerData      = 0;
            isDispDataInited = false;
            isMainDataInited = false;
            //m_TeamId         = 0;
            //m_bTeamSwitch    = true;
            m_curX = m_curY = m_curZ = 0;

            //m_TeamRPendingList.clear();
            //m_TeamBPendingList.clear();
        }

        Player &operator=(const stAccountInfo::Player &Other)
        {
            pPlayerData		= Other.pPlayerData;
            isDispDataInited = Other.isDispDataInited;
            isMainDataInited = Other.isMainDataInited;
            //m_TeamRule		= Other.m_TeamRule;
            //m_TeamId		= Other.m_TeamId;
            //m_bTeamSwitch	= Other.m_bTeamSwitch;
            m_pAccount      = Other.m_pAccount;

            //m_TeamRPendingList = Other.m_TeamRPendingList;
            //m_TeamBPendingList = Other.m_TeamBPendingList;

            m_curX = Other.m_curX;
            m_curY = Other.m_curY;
            m_curZ = Other.m_curZ;

            return *this;
        }
    };
public:
    stAccountInfo():
        m_status(this)
    {
        LastTime		= (int)time(NULL);
        isAdult			= true;
        UID				= 0;
        AccountId		= 0;
        socketId		= 0;
        LineId			= 0;
        CurPlayerPos	= -1;
        PointNum		= 0;
        PlayerNum		= 0;
        mode            = 0;
        status          = 0;
        totalOnlineTime = 0;
        AccountName[0]	= 0;
        isGM			= 0;
        accountSocketId = 0;
        memset(PlayerIdList,0,sizeof(PlayerIdList));
        memset(Password1,0,MD5_STRING_LENGTH);
        memset(Password2,0,MD5_STRING_LENGTH);
        memset(Password3,0,MD5_STRING_LENGTH);

        __InitStatus();

        m_status.SetRawState(0);
    }

    stAccountInfo &operator=(const stAccountInfo &Other)
    {
        LastTime		= (int)time(NULL);
        isGM			= Other.isGM;
        isAdult			= Other.isAdult;
        UID				= Other.UID;
        AccountId		= Other.AccountId;
        socketId		= Other.socketId;
        LineId			= Other.LineId;
        CurPlayerPos	= Other.CurPlayerPos;
        PointNum		= Other.PointNum;
        PlayerNum		= Other.PointNum;
        mode            = Other.mode;
        status          = Other.status;
        totalOnlineTime = Other.totalOnlineTime;
        accountSocketId = Other.accountSocketId;
        strcpy_s(AccountName,sizeof(AccountName),Other.AccountName);
        memcpy(PlayerIdList,Other.PlayerIdList,sizeof(PlayerIdList));
        memcpy(Password1,Other.Password1,MD5_STRING_LENGTH);
        memcpy(Password2,Other.Password2,MD5_STRING_LENGTH);
        memcpy(Password3,Other.Password3,MD5_STRING_LENGTH);

        return *this;
    }
    
    const char* GetCurPlayerName(void);

	int getCurrentState() {return m_status.GetState();}

    /************************************************************************/
    /* 触发当前帐号的事件，推动状态机
    /************************************************************************/
    void    TriggerEvent(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet)
    {
        m_curOpcode       = opcode;
        m_curSocketHandle = SocketHandle;
        m_curPHead        = pHead;
        m_curPacket       = Packet;

        m_status.Trigger(opcode,SocketHandle,pHead,Packet);
    }
   
    void    Update(int curTime)
    {
        m_status.Update(curTime);
    }
    
    bool    IsIdle(void)     const {return m_status.GetState() == STATUS_IDEL;}
    bool    IsInGame(void)   const {return m_status.GetState() == STATUS_IN_GAME;}  
    bool    IsDeleting(void) const {return m_status.GetState() == STATUS_DELAY_DEL;}  


	//---------------------------------------------------------------
	// 生成验证码
	//---------------------------------------------------------------
	void genIdCode();

    /************************************************************************/
    /* 踢玩家下线，玩家需要在选择界面或游戏里面
    /************************************************************************/
    bool    Kick(const char* reason, int& error, bool isModifyState = false);

    /************************************************************************/
    /* 传送玩家
    /************************************************************************/
    int     TransportPlayer(int playerId,int mapId,float x,float y,float z);

    /************************************************************************/
    /* 刷新world上的玩家数据，给予GM查询
    /************************************************************************/
    void    RefreshData(void) {}
public:
    int		LastTime;
    T_UID	UID;
    int		AccountId;
    int		socketId;
    int		accountSocketId;
    int		LineId;
    int		CurPlayerPos;
    int		PlayerIdList[MAX_ACTOR_COUNT];
    int		PointNum;
    int     mode;
    int     status;
    int     loginStatus;
    int     loginTime;
    int     logoutTime;
	int		dTotalOnlineTime;	// 防沉迷，累计在线时间
	int		dTotalOfflineTime;	// 防沉迷，累计离线时间
    std::string loginIP;
    int     totalOnlineTime;
    char	AccountName[COMMON_STRING_LENGTH];
    char	Password1[MD5_STRING_LENGTH];
    char	Password2[MD5_STRING_LENGTH];
    char	Password3[MD5_STRING_LENGTH];
    char	PlayerNum;
    bool	isAdult;
    char	isGM;

	typedef void(stAccountInfo::*EVENT_CALLBACK)(int,int,stPacketHead*,Base::BitStream*);

	CTimerStateMachine<stAccountInfo,STATUS_COUNT,EVENT_CALLBACK> m_status;

    
private:
    //初始化状态
    void    __InitStatus(void);

    //事件处理
    void    __OnIdleStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);
    void    __OnLoginAccountStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);
    void    __OnInSelectStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);
    void    __OnLoginGameStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);
    void    __OnInGameStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);
    void    __OnSwitchMapStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);
    void    __OnDelayStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);
	void	__OnIdCodeHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);

    //进入状态
    void    __OnEnterLoginGameHandler(void);
    void    __OnEnterDelayDelHandler(void);
    void    __OnEnterGameHandler(void);

    //超时处理
    void    __OnDelayStatusExpire(void);
    void    __OnLoginAccountExpire(void);
	void	__OnIdCodeExpire(void);


	void ClearPlayer( int PlayerId );
    
    void    __SendClientLoginAccountResponse(int Error, int SocketHandle);
    bool    __SendWorldZoneTickPlayerRequest(int LineId, PlayerDataRef pPlayerData, int PlayerId, bool bTickNow = true);
    bool    __SendZoneGameLogin(int SocketHandle, T_UID UID, int LineId, int AccountId, int PlayerId, bool isFirst);
	void    __SendClientGameLoginResponse(int Error,int SocketHandle, T_UID UID, int LineId, int AccountId, int PlayerId);
    void    __SendClientLogout(int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);

    void    __OnPlayerLeaveGame(void);


    //临时用在状态中过渡的数据
    int  m_t_PlayerId;
    bool m_t_isSwitch;

    int              m_curOpcode;
    int              m_curSocketHandle;
    stPacketHead*    m_curPHead;
    Base::BitStream* m_curPacket;
};

#endif /*_ACCOUNTHANDLER_H_*/