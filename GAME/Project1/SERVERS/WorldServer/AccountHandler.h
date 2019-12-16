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
// stPlayerData�����ü�����װ��������߳�ɾ������
//---------------------------------------------------------------


enum ePlayerEvent
{
    EVENT_PLAYER_OFFLINE,
};

#define PLAYER_TIMEOUT 400

struct stPacketHead;
class Base::BitStream;

/************************************************************************/
/* �ʺź��ʺŶ�Ӧ��ҵķ�װ
/************************************************************************/
class stAccountInfo
{
public:
    //�ʺŵ�״̬
    enum Status
    {
        STATUS_IDEL              = 0, //����״̬
        STATUS_LOGIN_ACCOUNT,         //�ͻ��˵���,�����ʺ���֤��game��֤
        STATUS_IN_SELECT,             //��ɫѡ��
        STATUS_LOGIN_GAME,            //������Ϸ
        STATUS_IN_GAME,               //����Ϸ������
        STATUS_MAP_SWITCH,            //�л���ͼ
        STATUS_DELAY_DEL,             //�ȴ��ʺ�ɾ�����ʺ��˳��󣬻��ڹ�1��ʱ���ɾ��

		STATUS_ID_CODE,				  //��֤��״̬

        STATUS_COUNT,
    };

	char mIdCode[COMMON_STRING_LENGTH];   // ��֤��

    //������ݷ�װ
    struct Player
    {
    public:
        //�ʺŶ���
        stAccountInfo*  m_pAccount;

        //������ݿ��е�չ������
        stPlayerData *	pPlayerData;

        //������һЩ��ʱ���Ʊ���
        bool			isDispDataInited;
        bool			isMainDataInited;

        ////����ΪһЩ�߼����󣬿��ܺ�pPlayerData�е����ظ�
        //int				m_TeamRule;			//����������
        //int				m_TeamId;			//����id
        //bool			m_bTeamSwitch;		//��ӿ���
        //std::set<int>	m_TeamRPendingList;	//����յ��������б�
        //std::set<int>	m_TeamBPendingList;	//��ӷ���������б�

        //��ҵ�ǰ��λ��
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
    /* ������ǰ�ʺŵ��¼����ƶ�״̬��
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
	// ������֤��
	//---------------------------------------------------------------
	void genIdCode();

    /************************************************************************/
    /* ��������ߣ������Ҫ��ѡ��������Ϸ����
    /************************************************************************/
    bool    Kick(const char* reason, int& error, bool isModifyState = false);

    /************************************************************************/
    /* �������
    /************************************************************************/
    int     TransportPlayer(int playerId,int mapId,float x,float y,float z);

    /************************************************************************/
    /* ˢ��world�ϵ�������ݣ�����GM��ѯ
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
	int		dTotalOnlineTime;	// �����ԣ��ۼ�����ʱ��
	int		dTotalOfflineTime;	// �����ԣ��ۼ�����ʱ��
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
    //��ʼ��״̬
    void    __InitStatus(void);

    //�¼�����
    void    __OnIdleStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);
    void    __OnLoginAccountStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);
    void    __OnInSelectStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);
    void    __OnLoginGameStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);
    void    __OnInGameStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);
    void    __OnSwitchMapStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);
    void    __OnDelayStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);
	void	__OnIdCodeHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet);

    //����״̬
    void    __OnEnterLoginGameHandler(void);
    void    __OnEnterDelayDelHandler(void);
    void    __OnEnterGameHandler(void);

    //��ʱ����
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


    //��ʱ����״̬�й��ɵ�����
    int  m_t_PlayerId;
    bool m_t_isSwitch;

    int              m_curOpcode;
    int              m_curSocketHandle;
    stPacketHead*    m_curPHead;
    Base::BitStream* m_curPacket;
};

#endif /*_ACCOUNTHANDLER_H_*/