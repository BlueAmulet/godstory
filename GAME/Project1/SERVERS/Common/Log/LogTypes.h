#ifndef _LOGTYPES_H_
#define _LOGTYPES_H_

#include "../dbStruct.h"

/************************************************************************/
/* 定义所有的LOG信息结构
/************************************************************************/

//// Database Struct Sample
//DB_STRUCT( lgPlayerData )
//{
//	DECL_DB_TABLE( lgPlayerData, TB_PLAYERDATA);
//
//	DECL_DB_FIELD( lgPlayerData, mId,		int,	DB_TYPE_INDEX,	索引,		32	);
//};

/************************************************************************/
/* 帐号登入
/************************************************************************/
DB_STRUCT(lg_account_login)
{
    DECL_DB_TABLE(lg_account_login,"帐号登入");

    DECL_DB_FIELD(accountName,std::string, DB_TYPE_STRING,30);      //帐号名称
    DECL_DB_FIELD(ip,         std::string, DB_TYPE_STRING,30);      //IP地址
    DECL_DB_FIELD(area,       int,         DB_TYPE_INT,   10);      //所在区域
    DECL_DB_FIELD(result,     std::string, DB_TYPE_STRING,10);      //登入结果
    //DECL_DB_FIELD(result2,     std::string, DB_TYPE_STRING,30);      //登入结果
    DECL_DB_FIELD(result3,     int, DB_TYPE_INT,14);                //登入结果
};

/************************************************************************/
/* 帐号登出
/************************************************************************/
DB_STRUCT(lg_account_logout)
{
    DECL_DB_TABLE(lg_account_logout,"帐号登出");

    DECL_DB_FIELD(accountName,std::string, DB_TYPE_STRING,30);      //帐号名称
    DECL_DB_FIELD(ip,         std::string, DB_TYPE_STRING,30);      //IP地址
    DECL_DB_FIELD(area,       int,         DB_TYPE_INT,   10);      //所在区域
};

//角色登入
DB_STRUCT(lg_char_login)
{
    DECL_DB_TABLE(lg_char_login,"角色登入");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,32);      //角色名称
    DECL_DB_FIELD(accountName,std::string, DB_TYPE_STRING,32);      //帐号名称
    DECL_DB_FIELD(ip,         std::string, DB_TYPE_STRING,32);      //IP地址
    DECL_DB_FIELD(mapId,      int,         DB_TYPE_INT,   10);      //登录地图
};

//角色登出
DB_STRUCT(lg_char_logout)
{
    DECL_DB_TABLE(lg_char_logout,"角色登出");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,32);      //角色名称
    DECL_DB_FIELD(accountName,std::string, DB_TYPE_STRING,32);      //帐号名称
    DECL_DB_FIELD(ip,         std::string, DB_TYPE_STRING,32);      //IP地址
    DECL_DB_FIELD(mapId,      int,         DB_TYPE_INT,   10);      //登出地图
};

/************************************************************************/
/* 人物创建
/************************************************************************/
DB_STRUCT(lg_char_create)
{
    DECL_DB_TABLE(lg_char_create,"人物创建");
    
    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,32);      //角色名称
    DECL_DB_FIELD(accountName,std::string, DB_TYPE_STRING,32);      //帐号名称
    DECL_DB_FIELD(playerId,   int,         DB_TYPE_INT,   32);      //角色ID
    DECL_DB_FIELD(job,        int,         DB_TYPE_INT,   8);       //职业
    DECL_DB_FIELD(level,      int,         DB_TYPE_INT,   8);       //等级
    DECL_DB_FIELD(sex,        int,         DB_TYPE_INT,   8);       //性别
    DECL_DB_FIELD(race,       int,         DB_TYPE_INT,   8);       //种族
};

/************************************************************************/
/* 人物删除
/************************************************************************/
DB_STRUCT(lg_char_delete)
{
    DECL_DB_TABLE(lg_char_delete,"人物删除");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,32);      //角色名称
    DECL_DB_FIELD(accountName,std::string, DB_TYPE_STRING,32);      //帐号名称
    DECL_DB_FIELD(playerId,   int,         DB_TYPE_INT,   32);      //角色ID
    DECL_DB_FIELD(job,        int,         DB_TYPE_INT,   8);       //职业
    DECL_DB_FIELD(level,      int,         DB_TYPE_INT,   8);       //等级
    DECL_DB_FIELD(sex,        int,         DB_TYPE_INT,   8);       //性别
    DECL_DB_FIELD(race,       int,         DB_TYPE_INT,   8);       //种族
};

/************************************************************************/
/* 技能学习
/************************************************************************/
DB_STRUCT(lg_skill_learn)
{
    DECL_DB_TABLE(lg_skill_learn,"技能学习");

    DECL_DB_FIELD(skillName, std::string, DB_TYPE_STRING,30);      //技能名称
    DECL_DB_FIELD(costGold,  int,         DB_TYPE_INT,32);         //消耗的金钱
    DECL_DB_FIELD(costExp,   int,         DB_TYPE_INT,32);         //消耗的经验
    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,30);      //角色名称
};

/************************************************************************/
/* 生活技能学习
/************************************************************************/
DB_STRUCT(lg_livingskill_learn)
{
    DECL_DB_TABLE(lg_livingskill_learn,"生活技能学习");

    DECL_DB_FIELD(skillName, std::string, DB_TYPE_STRING,30);      //技能名称
    DECL_DB_FIELD(costGold,  int,         DB_TYPE_INT,32);         //消耗的金钱
    DECL_DB_FIELD(costExp,   int,         DB_TYPE_INT,32);         //消耗的经验
    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,30);      //角色名称
};

/************************************************************************/
/* 技能升级
/************************************************************************/
DB_STRUCT(lg_skill_update)
{
    DECL_DB_TABLE(lg_skill_update,"技能升级");

    DECL_DB_FIELD(skillName, std::string, DB_TYPE_STRING,30);      //技能名称
    DECL_DB_FIELD(level,     int,         DB_TYPE_INT,8);          //等级
    DECL_DB_FIELD(costGold,  int,         DB_TYPE_INT,32);         //消耗的金钱
    DECL_DB_FIELD(costExp,   int,         DB_TYPE_INT,32);         //消耗的经验
    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,30);      //角色名称
};

/************************************************************************/
/* 商品购买
/************************************************************************/
DB_STRUCT(lg_shopping_buy)
{
    DECL_DB_TABLE(lg_shopping_buy,"道具购买");

    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,30);      //角色名称
    DECL_DB_FIELD(buyfrom,   std::string, DB_TYPE_STRING,30);      //来源(商店,或商店回购)
    DECL_DB_FIELD(itemName,  std::string, DB_TYPE_STRING,30);      //道具名称
    DECL_DB_FIELD(itemCount, int,         DB_TYPE_INT,   8);       //道具数量
    DECL_DB_FIELD(costType,  int,         DB_TYPE_STRING,30);      //消耗类型(金币,元宝)
    DECL_DB_FIELD(costVal,   int,         DB_TYPE_INT,   32);      //消耗值
    DECL_DB_FIELD(posX,      float,       DB_TYPE_FLOAT, 32);      //坐标X
    DECL_DB_FIELD(posY,      float,       DB_TYPE_FLOAT, 32);      //坐标Y
};

DB_STRUCT(lg_shopping_rebuy)
{
    DECL_DB_TABLE(lg_shopping_rebuy,"道具回购");

    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,30);      //角色名称
    DECL_DB_FIELD(buyfrom,   std::string, DB_TYPE_STRING,30);      //来源(商店,或商店回购)
    DECL_DB_FIELD(itemName,  std::string, DB_TYPE_STRING,30);      //道具名称
    DECL_DB_FIELD(itemCount, int,         DB_TYPE_INT,   8);       //道具数量
    DECL_DB_FIELD(costType,  int,         DB_TYPE_STRING,30);      //消耗类型(金币,元宝)
    DECL_DB_FIELD(costVal,   int,         DB_TYPE_INT,   32);      //消耗值
    DECL_DB_FIELD(posX,      float,       DB_TYPE_FLOAT, 32);      //坐标X
    DECL_DB_FIELD(posY,      float,       DB_TYPE_FLOAT, 32);      //坐标Y
};

/************************************************************************/
/* 商品出售
/************************************************************************/
DB_STRUCT(lg_shopping_sell)
{
    DECL_DB_TABLE(lg_shopping_sell,"道具出售");

    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,30);      //角色名称
    DECL_DB_FIELD(itemName,  std::string, DB_TYPE_STRING,30);      //道具名称
    DECL_DB_FIELD(itemUID,   U64,         DB_TYPE_BIGINT,32);      //道具UID
    DECL_DB_FIELD(itemCount, int,         DB_TYPE_INT,   8);       //道具数量
    DECL_DB_FIELD(itemPrice, int,         DB_TYPE_INT,   8);       //价格
};

/************************************************************************/
/* 拾取掉物
/************************************************************************/
DB_STRUCT(lg_pickup_item)
{
    DECL_DB_TABLE(lg_pickup_item,"掉物拾取");

    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,30);      //角色名称
    DECL_DB_FIELD(monsterName,std::string,DB_TYPE_STRING,30);      //怪物名称
    DECL_DB_FIELD(itemUID,   U64,         DB_TYPE_BIGINT,32);      //道具UID
    DECL_DB_FIELD(itemName,  std::string, DB_TYPE_STRING,30);      //道具名称
    DECL_DB_FIELD(itemCount, int,         DB_TYPE_INT,   8);       //道具数量
    DECL_DB_FIELD(map,       int,         DB_TYPE_INT,   32);      //MAPID
    DECL_DB_FIELD(posX,      float,       DB_TYPE_FLOAT, 32);      //坐标X
    DECL_DB_FIELD(posY,      float,       DB_TYPE_FLOAT, 32);      //坐标Y
};

/************************************************************************/
/* 丢弃物品
/************************************************************************/
DB_STRUCT(lg_drop_item)
{
    DECL_DB_TABLE(lg_drop_item,"物品丢弃");

    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,  30);      //角色名称
    DECL_DB_FIELD(itemName,  std::string, DB_TYPE_STRING,30);        //道具名称
    DECL_DB_FIELD(itemCount, int,         DB_TYPE_INT,   8);         //道具数量
};

/************************************************************************/
/* 物品交易得到的
/************************************************************************/
DB_STRUCT(lg_trade_itemGet)
{
    DECL_DB_TABLE(lg_trade_itemGet,"交易获得物品");

    DECL_DB_FIELD(fromPlayer,std::string, DB_TYPE_STRING,  30);      //对方交易的玩家名称
    DECL_DB_FIELD(toPlayer,  std::string, DB_TYPE_STRING,  30);      //当前玩家名称
    DECL_DB_FIELD(itemName,  std::string, DB_TYPE_STRING,  30);      //道具名称
    DECL_DB_FIELD(itemCount, int,         DB_TYPE_INT,     8);       //道具数量
};

/************************************************************************/
/* 物品交易出去的
/************************************************************************/
DB_STRUCT(lg_trade_itemLost)
{
    DECL_DB_TABLE(lg_trade_itemLost,"交易失去物品");

    DECL_DB_FIELD(fromPlayer,std::string, DB_TYPE_STRING,  30);      //当前玩家名称
    DECL_DB_FIELD(toPlayer,  std::string, DB_TYPE_STRING,  30);      //收取物品的玩家名称
    DECL_DB_FIELD(itemName,  std::string, DB_TYPE_STRING,  30);      //道具名称
    DECL_DB_FIELD(itemCount, int,         DB_TYPE_INT,     8);       //道具数量
};

/************************************************************************/
/* 交易得到的金钱 
/************************************************************************/
DB_STRUCT(lg_trade_goldGet)
{
    DECL_DB_TABLE(lg_trade_goldGet,"交易获得金钱");

    DECL_DB_FIELD(fromPlayer,std::string, DB_TYPE_STRING,  30);      //对方交易的玩家名称
    DECL_DB_FIELD(toPlayer,  std::string, DB_TYPE_STRING,  30);      //收取金币的玩家名称
    DECL_DB_FIELD(gold,      int,         DB_TYPE_INT,     32);      //金币
};

/************************************************************************/
/* 交易失去的金钱
/************************************************************************/
DB_STRUCT(lg_trade_goldLost)
{   
    DECL_DB_TABLE(lg_trade_goldLost,"交易失去金钱");

    DECL_DB_FIELD(fromPlayer,std::string, DB_TYPE_STRING,  30);      //当前玩家名称
    DECL_DB_FIELD(toPlayer,  std::string, DB_TYPE_STRING,  30);      //收取金币的玩家名称
    DECL_DB_FIELD(gold,      int,         DB_TYPE_INT,     32);      //金币
};

/************************************************************************/
/* 邮寄
/************************************************************************/
DB_STRUCT(lg_mail)
{
    DECL_DB_TABLE(lg_mail,"邮寄");

    DECL_DB_FIELD(sendName, std::string, DB_TYPE_STRING,  30);      //发信人
    DECL_DB_FIELD(recvName, std::string, DB_TYPE_STRING,  30);      //发信人
    DECL_DB_FIELD(itemName, std::string, DB_TYPE_STRING,  30);      //道具名称
    DECL_DB_FIELD(itemCount,int,         DB_TYPE_INT,     8);       //道具数量
    DECL_DB_FIELD(gold,     int,         DB_TYPE_INT,     32);      //金币
};

/************************************************************************/
/* 邮寄获取物品
/************************************************************************/
DB_STRUCT(lg_mail_itemGet)
{
    DECL_DB_TABLE(lg_mail_itemGet,"邮件获得物品");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(sendName,   std::string, DB_TYPE_STRING,  30);      //发信人
    DECL_DB_FIELD(itemName,   std::string, DB_TYPE_STRING,  30);        //道具名称
    DECL_DB_FIELD(itemCount,  int,         DB_TYPE_INT,     8);         //道具数量
};

/************************************************************************/
/* 邮寄获得金钱
/************************************************************************/
DB_STRUCT(lg_mail_goldGet)
{
    DECL_DB_TABLE(lg_mail_goldGet,"邮件获得金钱");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(sendName,   std::string, DB_TYPE_STRING,  30);      //发信人
    DECL_DB_FIELD(gold,       int,         DB_TYPE_INT,     32);      //金币
};

/************************************************************************/
/* 往银行存入物品
/************************************************************************/
DB_STRUCT(lg_bank_saveItem)
{
    DECL_DB_TABLE(lg_bank_saveItem,"银行存物");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(itemName,   std::string, DB_TYPE_STRING,  30);      //道具名称
    DECL_DB_FIELD(itemCount,  int,         DB_TYPE_INT,     8);       //道具数量
};

/************************************************************************/
/* 银行取出物品
/************************************************************************/
DB_STRUCT(lg_bank_getItem)
{
    DECL_DB_TABLE(lg_bank_getItem,"银行取物");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(itemName,   std::string, DB_TYPE_STRING,  30);      //道具名称
    DECL_DB_FIELD(itemCount,  int,         DB_TYPE_INT,     8);       //道具数量
};

/************************************************************************/
/* 银行存钱
/************************************************************************/
DB_STRUCT(lg_bank_saveGold)
{
    DECL_DB_TABLE(lg_bank_saveGold,"银行存钱");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(gold,       int,         DB_TYPE_INT,     32);      //金币
};

/************************************************************************/
/* 银行取钱
/************************************************************************/
DB_STRUCT(lg_bank_getGold)
{
    DECL_DB_TABLE(lg_bank_getGold,"银行取钱");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(gold,       int,         DB_TYPE_INT,     32);      //金币
};


/************************************************************************/
/* 任务完成情况
/************************************************************************/
DB_STRUCT(lg_quest_finishInfo)
{
    DECL_DB_TABLE(lg_quest_finishInfo,"任务完成情况");

    DECL_DB_FIELD(questName,  std::string, DB_TYPE_STRING,  30);      //任务名称
    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(playerLevel,int,         DB_TYPE_INT,     32);      //玩家等级
    DECL_DB_FIELD(playerRace, int,         DB_TYPE_INT,     32);      //玩家职业
};

/************************************************************************/
/* 任务获得的物品
/************************************************************************/
DB_STRUCT(lg_quest_itemGet)
{
    DECL_DB_TABLE(lg_quest_itemGet,"任务物品获取");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(questName,  std::string, DB_TYPE_STRING,  30);      //任务名称
    DECL_DB_FIELD(itemName,   std::string, DB_TYPE_STRING,  30);      //道具名称
    DECL_DB_FIELD(itemCount,  int,         DB_TYPE_INT,     8);       //道具数量
};

/************************************************************************/
/* 任务获得金币
/************************************************************************/
DB_STRUCT(lg_quest_goldGet)
{
    DECL_DB_TABLE(lg_quest_goldGet,"任务金币获取");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(questName,  std::string, DB_TYPE_STRING,  30);      //任务名称
    DECL_DB_FIELD(gold,       int,         DB_TYPE_INT,     32);      //金币
};

/************************************************************************/
/* 任务获得经验
/************************************************************************/
DB_STRUCT(lg_quest_expGet)
{
    DECL_DB_TABLE(lg_quest_expGet,"任务经验获取");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(questName,  std::string, DB_TYPE_STRING,  30);      //任务名称
    DECL_DB_FIELD(exp,        int,         DB_TYPE_INT,     32);      //金币
};

/************************************************************************/
/* 玩家死亡信息
/************************************************************************/
DB_STRUCT(lg_death)
{
    DECL_DB_TABLE(lg_death,"死亡信息");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(killerName, std::string, DB_TYPE_STRING,  30);      //杀死玩家的人或怪物名称
};




/************************************************************************/
/* 死亡丢失的物品
/************************************************************************/
DB_STRUCT(lg_death_itemLost)
{
    DECL_DB_TABLE(lg_death_itemLost,"死亡掉物");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(itemName,   std::string, DB_TYPE_STRING,  30);      //道具名称
    DECL_DB_FIELD(itemCount,  int,         DB_TYPE_INT,     8);       //道具数量
};

/************************************************************************/
/* 死亡时丢失的金钱
/************************************************************************/
DB_STRUCT(lg_death_goldLost)
{
    DECL_DB_TABLE(lg_death_goldLost,"死亡掉钱");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(gold,       int,         DB_TYPE_INT,     32);      //金币
};

/************************************************************************/
/* 道具使用
/************************************************************************/
DB_STRUCT(lg_item_use)
{
    DECL_DB_TABLE(lg_item_use,"道具使用");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(itemName,   std::string, DB_TYPE_STRING,  30);      //道具名称
    DECL_DB_FIELD(useName,    std::string, DB_TYPE_STRING,  30);      //用途
};


/************************************************************************/
/* 打怪获得的经验
/************************************************************************/
DB_STRUCT(lg_monster_exp)
{
    DECL_DB_TABLE(lg_monster_exp,"杀怪经验");

    DECL_DB_FIELD(playerName,  std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(playerLevel, int,         DB_TYPE_INT,     32);      //玩家等级
    DECL_DB_FIELD(playerRace,  int,         DB_TYPE_INT,     32);      //玩家职业
    DECL_DB_FIELD(monsterName, std::string, DB_TYPE_STRING,  30);      //当前玩家
    DECL_DB_FIELD(exp,         int,         DB_TYPE_DATETIME,32);      //exp
};

#endif /*_LOGTYPES_H_*/