/*
** Lua binding: GMExport
** Generated automatically by tolua++-1.0.92 on 01/28/10 11:19:35.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_GMExport_open (lua_State* tolua_S);

#include "GMFunctions.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
}

/* function: GM_AccountQuery */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_AccountQuery00
static int tolua_GMExport_GM_AccountQuery00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* name = ((const char*)  tolua_tostring(tolua_S,1,0));
  int mode = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   GM_AccountQuery(name,mode);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_AccountQuery'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_AccountFreeze */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_AccountFreeze00
static int tolua_GMExport_GM_AccountFreeze00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* accountName = ((const char*)  tolua_tostring(tolua_S,1,0));
  int time = ((int)  tolua_tonumber(tolua_S,2,0));
  int isIPFreeze = ((int)  tolua_tonumber(tolua_S,3,0));
  const char* reason = ((const char*)  tolua_tostring(tolua_S,4,0));
  {
   GM_AccountFreeze(accountName,time,isIPFreeze,reason);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_AccountFreeze'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_AccountUnFreeze */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_AccountUnFreeze00
static int tolua_GMExport_GM_AccountUnFreeze00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* accountName = ((const char*)  tolua_tostring(tolua_S,1,0));
  const char* reason = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   GM_AccountUnFreeze(accountName,reason);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_AccountUnFreeze'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorFreeze */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorFreeze00
static int tolua_GMExport_GM_ActorFreeze00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* actorName = ((const char*)  tolua_tostring(tolua_S,1,0));
  int time = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* reason = ((const char*)  tolua_tostring(tolua_S,3,0));
  {
   GM_ActorFreeze(actorName,time,reason);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorFreeze'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorUnFreeze */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorUnFreeze00
static int tolua_GMExport_GM_ActorUnFreeze00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* actorName = ((const char*)  tolua_tostring(tolua_S,1,0));
  const char* reason = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   GM_ActorUnFreeze(actorName,reason);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorUnFreeze'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_AccountChatMute */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_AccountChatMute00
static int tolua_GMExport_GM_AccountChatMute00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* accountName = ((const char*)  tolua_tostring(tolua_S,1,0));
  int time = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* reason = ((const char*)  tolua_tostring(tolua_S,3,0));
  {
   GM_AccountChatMute(accountName,time,reason);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_AccountChatMute'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_AccountChatUnMute */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_AccountChatUnMute00
static int tolua_GMExport_GM_AccountChatUnMute00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* accountName = ((const char*)  tolua_tostring(tolua_S,1,0));
  int time = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* reason = ((const char*)  tolua_tostring(tolua_S,3,0));
  {
   GM_AccountChatUnMute(accountName,time,reason);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_AccountChatUnMute'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorChatMute */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorChatMute00
static int tolua_GMExport_GM_ActorChatMute00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* actorName = ((const char*)  tolua_tostring(tolua_S,1,0));
  int time = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* reason = ((const char*)  tolua_tostring(tolua_S,3,0));
  {
   GM_ActorChatMute(actorName,time,reason);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorChatMute'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorChatUnMute */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorChatUnMute00
static int tolua_GMExport_GM_ActorChatUnMute00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* actorName = ((const char*)  tolua_tostring(tolua_S,1,0));
  int time = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* reason = ((const char*)  tolua_tostring(tolua_S,3,0));
  {
   GM_ActorChatUnMute(actorName,time,reason);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorChatUnMute'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_Kick */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_Kick00
static int tolua_GMExport_GM_Kick00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* accountName = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   GM_Kick(accountName);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_Kick'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorKick */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorKick00
static int tolua_GMExport_GM_ActorKick00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* actorName = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   GM_ActorKick(actorName);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorKick'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorBaseInfo */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorBaseInfo00
static int tolua_GMExport_GM_ActorBaseInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int accountId = ((int)  tolua_tonumber(tolua_S,1,0));
  const char* actorName = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   GM_ActorBaseInfo(accountId,actorName);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorBaseInfo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorDeletedBaseInfo */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorDeletedBaseInfo00
static int tolua_GMExport_GM_ActorDeletedBaseInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int accountId = ((int)  tolua_tonumber(tolua_S,1,0));
  const char* actorName = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   GM_ActorDeletedBaseInfo(accountId,actorName);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorDeletedBaseInfo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorSkill */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorSkill00
static int tolua_GMExport_GM_ActorSkill00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int playerId = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   GM_ActorSkill(playerId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorSkill'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorSkillAdd */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorSkillAdd00
static int tolua_GMExport_GM_ActorSkillAdd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int playerId = ((int)  tolua_tonumber(tolua_S,1,0));
  unsigned int skillId = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  {
   GM_ActorSkillAdd(playerId,skillId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorSkillAdd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorSkillDel */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorSkillDel00
static int tolua_GMExport_GM_ActorSkillDel00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int playerId = ((int)  tolua_tonumber(tolua_S,1,0));
  unsigned int skillId = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  {
   GM_ActorSkillDel(playerId,skillId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorSkillDel'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorSkillMdf */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorSkillMdf00
static int tolua_GMExport_GM_ActorSkillMdf00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int playerId = ((int)  tolua_tonumber(tolua_S,1,0));
  unsigned int oldId = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int newSkillId = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  {
   GM_ActorSkillMdf(playerId,oldId,newSkillId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorSkillMdf'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorLivingSkill */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorLivingSkill00
static int tolua_GMExport_GM_ActorLivingSkill00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int playerId = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   GM_ActorLivingSkill(playerId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorLivingSkill'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorQuestFinished */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorQuestFinished00
static int tolua_GMExport_GM_ActorQuestFinished00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int actorId = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   GM_ActorQuestFinished(actorId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorQuestFinished'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorQuestCycle */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorQuestCycle00
static int tolua_GMExport_GM_ActorQuestCycle00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int actorId = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   GM_ActorQuestCycle(actorId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorQuestCycle'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorQuestAccepted */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorQuestAccepted00
static int tolua_GMExport_GM_ActorQuestAccepted00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int actorId = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   GM_ActorQuestAccepted(actorId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorQuestAccepted'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorFriend */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorFriend00
static int tolua_GMExport_GM_ActorFriend00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int playerId = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   GM_ActorFriend(playerId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorFriend'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorRide */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorRide00
static int tolua_GMExport_GM_ActorRide00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int playerId = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   GM_ActorRide(playerId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorRide'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorMail */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorMail00
static int tolua_GMExport_GM_ActorMail00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int accountId = ((int)  tolua_tonumber(tolua_S,1,0));
  int actorId = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   GM_ActorMail(accountId,actorId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorMail'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorItem */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorItem00
static int tolua_GMExport_GM_ActorItem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int accountId = ((int)  tolua_tonumber(tolua_S,1,0));
  int actorId = ((int)  tolua_tonumber(tolua_S,2,0));
  int slot = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   GM_ActorItem(accountId,actorId,slot);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorItem'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorItemDetail */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorItemDetail00
static int tolua_GMExport_GM_ActorItemDetail00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int actorId = ((int)  tolua_tonumber(tolua_S,1,0));
  const char* itemUID = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   GM_ActorItemDetail(actorId,itemUID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorItemDetail'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorItemModify */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorItemModify00
static int tolua_GMExport_GM_ActorItemModify00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int actorId = ((int)  tolua_tonumber(tolua_S,1,0));
  const char* UID = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   GM_ActorItemModify(actorId,UID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorItemModify'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorItemDel */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorItemDel00
static int tolua_GMExport_GM_ActorItemDel00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int actorId = ((int)  tolua_tonumber(tolua_S,1,0));
  const char* UID = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   GM_ActorItemDel(actorId,UID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorItemDel'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorPet */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorPet00
static int tolua_GMExport_GM_ActorPet00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int playerId = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   GM_ActorPet(playerId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorPet'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorPetInfo */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorPetInfo00
static int tolua_GMExport_GM_ActorPetInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int playerId = ((int)  tolua_tonumber(tolua_S,1,0));
  int petId = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   GM_ActorPetInfo(playerId,petId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorPetInfo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorTransport */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorTransport00
static int tolua_GMExport_GM_ActorTransport00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int playerId = ((int)  tolua_tonumber(tolua_S,1,0));
  int mapId = ((int)  tolua_tonumber(tolua_S,2,0));
  float x = ((float)  tolua_tonumber(tolua_S,3,0));
  float y = ((float)  tolua_tonumber(tolua_S,4,0));
  float z = ((float)  tolua_tonumber(tolua_S,5,0));
  {
   GM_ActorTransport(playerId,mapId,x,y,z);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorTransport'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorLevelMdf */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorLevelMdf00
static int tolua_GMExport_GM_ActorLevelMdf00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int accountId = ((int)  tolua_tonumber(tolua_S,1,0));
  int playerId = ((int)  tolua_tonumber(tolua_S,2,0));
  int newLevel = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   GM_ActorLevelMdf(accountId,playerId,newLevel);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorLevelMdf'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorExpMdf */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorExpMdf00
static int tolua_GMExport_GM_ActorExpMdf00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int accountId = ((int)  tolua_tonumber(tolua_S,1,0));
  int playerId = ((int)  tolua_tonumber(tolua_S,2,0));
  int newExp = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   GM_ActorExpMdf(accountId,playerId,newExp);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorExpMdf'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorGoldMdf */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorGoldMdf00
static int tolua_GMExport_GM_ActorGoldMdf00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int accountId = ((int)  tolua_tonumber(tolua_S,1,0));
  int playerId = ((int)  tolua_tonumber(tolua_S,2,0));
  int newGold = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   GM_ActorGoldMdf(accountId,playerId,newGold);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorGoldMdf'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorPos */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorPos00
static int tolua_GMExport_GM_ActorPos00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int accountId = ((int)  tolua_tonumber(tolua_S,1,0));
  int playerId = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   GM_ActorPos(accountId,playerId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorPos'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorRename */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorRename00
static int tolua_GMExport_GM_ActorRename00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int playerId = ((int)  tolua_tonumber(tolua_S,1,0));
  const char* newName = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   GM_ActorRename(playerId,newName);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorRename'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorDelete */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorDelete00
static int tolua_GMExport_GM_ActorDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int accountId = ((int)  tolua_tonumber(tolua_S,1,0));
  int playerId = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   GM_ActorDelete(accountId,playerId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorDelete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ActorUnDelete */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ActorUnDelete00
static int tolua_GMExport_GM_ActorUnDelete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int accountId = ((int)  tolua_tonumber(tolua_S,1,0));
  int playerId = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   GM_ActorUnDelete(accountId,playerId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ActorUnDelete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_QueryDelActor */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_QueryDelActor00
static int tolua_GMExport_GM_QueryDelActor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int accountId = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   GM_QueryDelActor(accountId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_QueryDelActor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_GMRole */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_GMRole00
static int tolua_GMExport_GM_GMRole00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int gmFlag = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   GM_GMRole(gmFlag);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_GMRole'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_GMAddRole */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_GMAddRole00
static int tolua_GMExport_GM_GMAddRole00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int gmFlag = ((int)  tolua_tonumber(tolua_S,1,0));
  const char* role = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   GM_GMAddRole(gmFlag,role);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_GMAddRole'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_GMDelRole */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_GMDelRole00
static int tolua_GMExport_GM_GMDelRole00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int gmFlag = ((int)  tolua_tonumber(tolua_S,1,0));
  const char* role = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   GM_GMDelRole(gmFlag,role);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_GMDelRole'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_GMQueryAccount */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_GMQueryAccount00
static int tolua_GMExport_GM_GMQueryAccount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int gmFlag = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   GM_GMQueryAccount(gmFlag);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_GMQueryAccount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_GMMdfAccount */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_GMMdfAccount00
static int tolua_GMExport_GM_GMMdfAccount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* accountName = ((const char*)  tolua_tostring(tolua_S,1,0));
  int gmFlag = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   GM_GMMdfAccount(accountName,gmFlag);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_GMMdfAccount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ChangePassword */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ChangePassword00
static int tolua_GMExport_GM_ChangePassword00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* accountName = ((const char*)  tolua_tostring(tolua_S,1,0));
  const char* oldPassword = ((const char*)  tolua_tostring(tolua_S,2,0));
  const char* newPassword = ((const char*)  tolua_tostring(tolua_S,3,0));
  {
   GM_ChangePassword(accountName,oldPassword,newPassword);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ChangePassword'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_Chat */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_Chat00
static int tolua_GMExport_GM_Chat00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* playerName = ((const char*)  tolua_tostring(tolua_S,1,0));
  int channel = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   GM_Chat(playerName,channel);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_Chat'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GM_ChatAuto */
#ifndef TOLUA_DISABLE_tolua_GMExport_GM_ChatAuto00
static int tolua_GMExport_GM_ChatAuto00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* playerName = ((const char*)  tolua_tostring(tolua_S,1,0));
  int channel = ((int)  tolua_tonumber(tolua_S,2,0));
  int isSysMsg = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   GM_ChatAuto(playerName,channel,isSysMsg);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GM_ChatAuto'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_GMExport_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_function(tolua_S,"GM_AccountQuery",tolua_GMExport_GM_AccountQuery00);
  tolua_function(tolua_S,"GM_AccountFreeze",tolua_GMExport_GM_AccountFreeze00);
  tolua_function(tolua_S,"GM_AccountUnFreeze",tolua_GMExport_GM_AccountUnFreeze00);
  tolua_function(tolua_S,"GM_ActorFreeze",tolua_GMExport_GM_ActorFreeze00);
  tolua_function(tolua_S,"GM_ActorUnFreeze",tolua_GMExport_GM_ActorUnFreeze00);
  tolua_function(tolua_S,"GM_AccountChatMute",tolua_GMExport_GM_AccountChatMute00);
  tolua_function(tolua_S,"GM_AccountChatUnMute",tolua_GMExport_GM_AccountChatUnMute00);
  tolua_function(tolua_S,"GM_ActorChatMute",tolua_GMExport_GM_ActorChatMute00);
  tolua_function(tolua_S,"GM_ActorChatUnMute",tolua_GMExport_GM_ActorChatUnMute00);
  tolua_function(tolua_S,"GM_Kick",tolua_GMExport_GM_Kick00);
  tolua_function(tolua_S,"GM_ActorKick",tolua_GMExport_GM_ActorKick00);
  tolua_function(tolua_S,"GM_ActorBaseInfo",tolua_GMExport_GM_ActorBaseInfo00);
  tolua_function(tolua_S,"GM_ActorDeletedBaseInfo",tolua_GMExport_GM_ActorDeletedBaseInfo00);
  tolua_function(tolua_S,"GM_ActorSkill",tolua_GMExport_GM_ActorSkill00);
  tolua_function(tolua_S,"GM_ActorSkillAdd",tolua_GMExport_GM_ActorSkillAdd00);
  tolua_function(tolua_S,"GM_ActorSkillDel",tolua_GMExport_GM_ActorSkillDel00);
  tolua_function(tolua_S,"GM_ActorSkillMdf",tolua_GMExport_GM_ActorSkillMdf00);
  tolua_function(tolua_S,"GM_ActorLivingSkill",tolua_GMExport_GM_ActorLivingSkill00);
  tolua_function(tolua_S,"GM_ActorQuestFinished",tolua_GMExport_GM_ActorQuestFinished00);
  tolua_function(tolua_S,"GM_ActorQuestCycle",tolua_GMExport_GM_ActorQuestCycle00);
  tolua_function(tolua_S,"GM_ActorQuestAccepted",tolua_GMExport_GM_ActorQuestAccepted00);
  tolua_function(tolua_S,"GM_ActorFriend",tolua_GMExport_GM_ActorFriend00);
  tolua_function(tolua_S,"GM_ActorRide",tolua_GMExport_GM_ActorRide00);
  tolua_function(tolua_S,"GM_ActorMail",tolua_GMExport_GM_ActorMail00);
  tolua_function(tolua_S,"GM_ActorItem",tolua_GMExport_GM_ActorItem00);
  tolua_function(tolua_S,"GM_ActorItemDetail",tolua_GMExport_GM_ActorItemDetail00);
  tolua_function(tolua_S,"GM_ActorItemModify",tolua_GMExport_GM_ActorItemModify00);
  tolua_function(tolua_S,"GM_ActorItemDel",tolua_GMExport_GM_ActorItemDel00);
  tolua_function(tolua_S,"GM_ActorPet",tolua_GMExport_GM_ActorPet00);
  tolua_function(tolua_S,"GM_ActorPetInfo",tolua_GMExport_GM_ActorPetInfo00);
  tolua_function(tolua_S,"GM_ActorTransport",tolua_GMExport_GM_ActorTransport00);
  tolua_function(tolua_S,"GM_ActorLevelMdf",tolua_GMExport_GM_ActorLevelMdf00);
  tolua_function(tolua_S,"GM_ActorExpMdf",tolua_GMExport_GM_ActorExpMdf00);
  tolua_function(tolua_S,"GM_ActorGoldMdf",tolua_GMExport_GM_ActorGoldMdf00);
  tolua_function(tolua_S,"GM_ActorPos",tolua_GMExport_GM_ActorPos00);
  tolua_function(tolua_S,"GM_ActorRename",tolua_GMExport_GM_ActorRename00);
  tolua_function(tolua_S,"GM_ActorDelete",tolua_GMExport_GM_ActorDelete00);
  tolua_function(tolua_S,"GM_ActorUnDelete",tolua_GMExport_GM_ActorUnDelete00);
  tolua_function(tolua_S,"GM_QueryDelActor",tolua_GMExport_GM_QueryDelActor00);
  tolua_function(tolua_S,"GM_GMRole",tolua_GMExport_GM_GMRole00);
  tolua_function(tolua_S,"GM_GMAddRole",tolua_GMExport_GM_GMAddRole00);
  tolua_function(tolua_S,"GM_GMDelRole",tolua_GMExport_GM_GMDelRole00);
  tolua_function(tolua_S,"GM_GMQueryAccount",tolua_GMExport_GM_GMQueryAccount00);
  tolua_function(tolua_S,"GM_GMMdfAccount",tolua_GMExport_GM_GMMdfAccount00);
  tolua_function(tolua_S,"GM_ChangePassword",tolua_GMExport_GM_ChangePassword00);
  tolua_function(tolua_S,"GM_Chat",tolua_GMExport_GM_Chat00);
  tolua_function(tolua_S,"GM_ChatAuto",tolua_GMExport_GM_ChatAuto00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_GMExport (lua_State* tolua_S) {
 return tolua_GMExport_open(tolua_S);
};
#endif

