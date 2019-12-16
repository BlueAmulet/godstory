#ifndef _GMRECVER_H_
#define _GMRECVER_H_

#include "base/Locker.h"
#include "lua/LuaWarp.h"
#include "GMRespondDef.h"
#include "AuthManager.h"
#include <hash_map>
#include <vector>


/************************************************************************/
/* GM�û���װ
/************************************************************************/
class CGMUser
{
public:
    CGMUser(int accountId) : m_accountId(accountId),m_lastGetTime(0) {}

    int GetAccountId(void) const {return m_accountId;}

    /************************************************************************/
    /* ��ȡ�����Ϣ�������Ϣ���Դ����ݿ��ZONE�ϻ�ȡ�����������ߣ�Ĭ�ϵ����
       ��Ϣ��ˢ��ʱ����30s
    /************************************************************************/
    stPlayerStruct* GetPlayerInfo(int playerId,bool isPlayerDeleted);
    stPlayerStruct* GetPlayerInfo(const char* playerName,bool isPlayerDeleted);
    stPlayerStruct* GetPlayerInfo(int playerId)           {return GetPlayerInfo(playerId,m_isPlayerDeleted);}
    stPlayerStruct* GetPlayerInfo(const char* playerName) {return GetPlayerInfo(playerName,m_isPlayerDeleted);}
private:
    bool                m_isPlayerDeleted;
    //��ǰGM���ʺ�ID
    int                 m_accountId;    
    
    //��ǰ�����������Ϣ
    int                 m_lastGetTime;
    stPlayerStruct      m_playerInfo;
};

/************************************************************************/
/* ����GM����
/************************************************************************/
class CGMRecver
{
public:
    CGMRecver(void) {}

    bool Initialize(void);

	static CGMRecver* Instance(void)
	{
		static CGMRecver local;
		return &local;
	}

	//���÷���ֵ
	template<typename _Ty>
	void	PushResp(const _Ty& resp) 
	{
		_Ty* pCurResp  = new _Ty;
		*pCurResp = resp;

        m_resps.push_back(pCurResp);
	}

    bool  HandleCommand(int sessionId,int SocketHandle,int accountId,char gmFlag,const char* fun,Base::BitStream* pack);
    
    /************************************************************************/
    /* ��ȡ��ǰ������GM����
    /************************************************************************/
    CGMUser*    GetGM(int accountId);
    CGMUser*    GetGM(void) {return GetGM(m_curAccountId);}

    /************************************************************************/
    /* ���ô���ֵ
    /************************************************************************/
    void        SetError(int error) {m_error = error;}

    int   GetSocketHandle(void) {return m_curSocketHandle;}
    
    CGMAuthManager& GetAuthMgr(void) {return m_auth;}

    const char* GetGMName(int accountId);
    const char* GetCurGMName(void) {return GetGMName(m_curAccountId);}
    int         GetCurGMFlag(void) {return m_curGMFlag;}
    const char* GetCurCmd(void)    {return m_curCMD.c_str();}

    const char* Format(const char* fmt,...)
    {
        static char buffer[4096];

        va_list args;
        va_start(args,fmt);
        _vsnprintf_s(buffer,sizeof(buffer),sizeof(buffer),fmt,(char*)args);

        return buffer;
    }

    Base::BitStream* GetPack(void) {return m_curPack;}
private:
    CMyCriticalSection m_cs;

    std::vector<CGMRespBase*> m_resps;
    std::vector<CGMUser>      m_gms;

    int             m_curSocketHandle;
    int             m_curAccountId;
    int             m_curGMFlag;
    int             m_error;
	CLuaWarp        m_lua;
    CGMAuthManager  m_auth;
    std::string     m_curCMD;
    Base::BitStream*m_curPack;
};

#endif /*_GMRECVER_H_*/