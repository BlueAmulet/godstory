#ifndef _GMRECVER_H_
#define _GMRECVER_H_

#include "base/Locker.h"
#include "lua/LuaWarp.h"
#include "GMRespondDef.h"
#include "AuthManager.h"
#include <hash_map>
#include <vector>


/************************************************************************/
/* GM用户封装
/************************************************************************/
class CGMUser
{
public:
    CGMUser(int accountId) : m_accountId(accountId),m_lastGetTime(0) {}

    int GetAccountId(void) const {return m_accountId;}

    /************************************************************************/
    /* 获取玩家信息，玩家信息可以从数据库或ZONE上获取，如果玩家在线，默认的玩家
       信息的刷新时间是30s
    /************************************************************************/
    stPlayerStruct* GetPlayerInfo(int playerId,bool isPlayerDeleted);
    stPlayerStruct* GetPlayerInfo(const char* playerName,bool isPlayerDeleted);
    stPlayerStruct* GetPlayerInfo(int playerId)           {return GetPlayerInfo(playerId,m_isPlayerDeleted);}
    stPlayerStruct* GetPlayerInfo(const char* playerName) {return GetPlayerInfo(playerName,m_isPlayerDeleted);}
private:
    bool                m_isPlayerDeleted;
    //当前GM的帐号ID
    int                 m_accountId;    
    
    //当前操作的玩家信息
    int                 m_lastGetTime;
    stPlayerStruct      m_playerInfo;
};

/************************************************************************/
/* 处理GM命令
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

	//设置返回值
	template<typename _Ty>
	void	PushResp(const _Ty& resp) 
	{
		_Ty* pCurResp  = new _Ty;
		*pCurResp = resp;

        m_resps.push_back(pCurResp);
	}

    bool  HandleCommand(int sessionId,int SocketHandle,int accountId,char gmFlag,const char* fun,Base::BitStream* pack);
    
    /************************************************************************/
    /* 获取当前操作的GM对象
    /************************************************************************/
    CGMUser*    GetGM(int accountId);
    CGMUser*    GetGM(void) {return GetGM(m_curAccountId);}

    /************************************************************************/
    /* 设置错误值
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