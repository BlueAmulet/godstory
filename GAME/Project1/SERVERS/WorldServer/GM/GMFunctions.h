#ifndef _GMFUNCTIONS_H_
#define _GMFUNCTIONS_H_

/************************************************************************/
/* 角色查询
   isById:true表示通过帐号编号查询，否则通过帐号名称查询
/************************************************************************/
extern void    GM_AccountQuery(const char* name,int mode);

/************************************************************************/
/* 冻结帐号,time = 0表示永久冻结
/************************************************************************/
extern void    GM_AccountFreeze(const char* accountName,int time,int isIPFreeze,const char* reason);
extern void    GM_AccountUnFreeze(const char* accountName,const char* reason);
extern void    GM_ActorFreeze(const char* actorName,int time,const char* reason);
extern void    GM_ActorUnFreeze(const char* actorName,const char* reason);

/************************************************************************/
/* 禁言,time = 0表示帐号永久禁言
/************************************************************************/
extern void    GM_AccountChatMute(const char* accountName,int time,const char* reason);
extern void    GM_AccountChatUnMute(const char* accountName,int time,const char* reason);
extern void    GM_ActorChatMute(const char* actorName,int time,const char* reason);
extern void    GM_ActorChatUnMute(const char* actorName,int time,const char* reason);

/************************************************************************/
/* 踢线,帐号和玩家角色踢线
/************************************************************************/
extern void    GM_Kick(const char* accountName);
extern void    GM_ActorKick(const char* actorName);

/************************************************************************/
/* 角色相关信息查询
/************************************************************************/
extern  void    GM_ActorBaseInfo(int accountId,const char* actorName);
extern  void    GM_ActorDeletedBaseInfo(int accountId,const char* actorName);
extern  void    GM_ActorSkill(int playerId);
extern  void    GM_ActorSkillAdd(int playerId,unsigned int skillId);
extern  void    GM_ActorSkillDel(int playerId,unsigned int skillId);
extern  void    GM_ActorSkillMdf(int playerId,unsigned int oldId,unsigned int newSkillId);
extern  void    GM_ActorLivingSkill(int playerId);
extern  void    GM_ActorQuestFinished(int actorId);
extern  void    GM_ActorQuestCycle(int actorId);
extern  void    GM_ActorQuestAccepted(int actorId);
extern  void    GM_ActorFriend(int playerId);
extern  void	GM_ActorRide(int playerId);
extern  void    GM_ActorMail(int accountId,int actorId);
extern  void    GM_ActorItem(int accountId,int actorId,int slot);
extern  void    GM_ActorItemDetail(int actorId,const char* itemUID);
extern  void    GM_ActorTransport(int playerId,int mapId,float x,float y,float z);
extern  void    GM_ActorLevelMdf(int accountId,int playerId,int newLevel);
extern  void    GM_ActorExpMdf(int accountId,int playerId,int newExp);
extern  void    GM_ActorGoldMdf(int accountId,int playerId,int newGold);
extern  void    GM_ActorItemModify(int actorId,const char* UID);
extern  void    GM_ActorItemDel(int actorId,const char* UID);
extern  void    GM_ActorPos(int accountId,int playerId);
extern  void    GM_ActorRename(int playerId,const char* newName);
extern  void	GM_ActorPet(int playerId);
extern  void	GM_ActorPetInfo(int playerId,int petId);

/************************************************************************/
/* GM权限查询
/************************************************************************/
extern  void    GM_GMRole(int gmFlag);
extern  void    GM_GMAddRole(int gmFlag,const char* role);
extern  void    GM_GMDelRole(int gmFlag,const char* role);
extern  void    GM_GMQueryAccount(int gmFlag);
extern  void    GM_GMMdfAccount(const char* accountName,int gmFlag);

/************************************************************************/
/* 修改GM密码
/************************************************************************/
extern  void    GM_ChangePassword(const char* accountName,const char* oldPassword,const char* newPassword);

/************************************************************************/
/* 发送GM消息
/************************************************************************/
extern  void    GM_Chat(const char* playerName,int channel);
extern  void    GM_ChatAuto(const char* playerName,int channel,int isSysMsg);

/************************************************************************/
/* 查询被删除的角色
/************************************************************************/
extern  void    GM_QueryDelActor(int accountId);
extern  void    GM_ActorDelete(int accountId,int playerId);
extern  void    GM_ActorUnDelete(int accountId,int playerId);

#endif /*_GMFUNCTIONS_H_*/