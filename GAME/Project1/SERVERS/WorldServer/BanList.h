#ifndef _BANLIST_H_
#define _BANLIST_H_

#include <hash_map>

/************************************************************************/
/* 保存IP冻结和帐号角色的冻结信息
/************************************************************************/
class CBanlist
{
    enum TargetType
    {
        TARGET_TYPE_ACC = 0,   //帐号
        TARGET_TYPE_ACTOR,     //角色
        TARGET_TYPE_IP,        //地址

        TARGET_TYPE_COUNT
    };

    enum BanType
    {
        BAN_TYPE_CHAT,
        BAN_TYPE_LOGIN,

        BAN_TYPE_COUNT
    };
public:
    //冻结的信息
    struct BanInfo
    {
        std::string name;
        int         type;
        int         loginBanExpire;
        int         chatBanExpire;
        std::string loginReason;
        std::string chatReason;

        bool IsLoginBaned(int curTime)
        {
            if (curTime >= loginBanExpire)
            {
                loginBanExpire = 0;
                return false;
            }

            return true;
        }

        bool IsChatBaned(int curTime)
        {
            if (curTime >= chatBanExpire)
            {
                chatBanExpire = 0;
                return false;
            }

            return true;
        }
    };

    CBanlist(void) {}
    
    //从数据库读取所有的数据
    bool    Init(void);

    static CBanlist* Instance(void)
    {
         static CBanlist local;
        return &local;
    }

    //ban now!
    void    BanIpLogin(const char* name,int lastTime,const char* reason);
    void    BanIpChat(const char* name,int lastTime,const char* reason);

    void    BanAccLogin(const char* name,int lastTime,const char* reason);
    void    BanAccChat(const char* name,int lastTime,const char* reason);

    void    BanActorLogin(const char* name,int lastTime,const char* reason);
    void    BanActorChat(const char* name,int lastTime,const char* reason);
    
    //unban
    void    UnBanIpLogin(const char* name);
    void    UnBanIpChat(const char* name);

    void    UnBanAccLogin(const char* name);
    void    UnBanAccChat(const char* name);

    void    UnBanActorLogin(const char* name);
    void    UnBanActorChat(const char* name);
    
    //query
    bool    IsIpLoginBan(const char* name,BanInfo* pInfo = 0);
    bool    IsIpChatBan(const char* name,BanInfo* pInfo = 0);

    bool    IsAccLoginBan(const char* name,BanInfo* pInfo = 0);
    bool    IsAccChatBan(const char* name,BanInfo* pInfo = 0);

    bool    IsActorLoginBan(const char* name,BanInfo* pInfo = 0);
    bool    IsActorChatBan(const char* name,BanInfo* pInfo = 0);
private:
    typedef stdext::hash_map<std::string,BanInfo> BAN_MAP;

    const char* __TranslateName(const char* name);
    void        __SaveBanToDB(BanInfo& info);
    bool        __IsBaned(BAN_MAP& banMap,const char* name,int type,BanInfo* pInfo);
    void        __DelBan(BAN_MAP& banMap,const char* name,int type);
    void        __DoBan(BAN_MAP& banMap,int target,const char* name,int banType,int banTime,const char* reason);
    bool        __HasBaned(BanInfo& info);
    
    BAN_MAP  m_accBans;
    BAN_MAP  m_actorBans;
    BAN_MAP  m_ipBans;
};

#endif /*_BANLIST_H_*/