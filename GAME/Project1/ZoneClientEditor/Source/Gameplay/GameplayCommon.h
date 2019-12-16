//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef __GAMEPLAYCOMMON_H__
#define __GAMEPLAYCOMMON_H__

#pragma once
#include "common/PlayerStruct.h"

struct Stats;
// ========================================================================================================================================
//  枚举 & 结构
// ========================================================================================================================================

enum
{
	MaxPlayerLevel			= 120,
	MAX_PET_LEVEL			= 161,
	MAX_SPIRIT_LEVEL		= 60,
	MAX_SPIRIT_JINGJIE		= 9,
	EnablePlayerLevel		= 90,
};

enum enSex
{
	SEX_BOTH	= 0,			//无要求
	SEX_MALE,					//男性
	SEX_FEMALE,					//女性
};

// 职业，注意与技能类型一致
enum enClass
{
	Class_Null,
	Class_Mu,
	Class_Huo,
	Class_Tu,
	Class_Jin,
	Class_Shui,
	MaxClasses,
};

// 五行加成
enum enWuXingMaster
{
	_Mu,
	_Huo,
	_Tu,
	_Jin,
	_Shui,
	MaxWuXingMasters,
};

// 声望
enum enFame
{
	Fame_Bottom,				// 最低声望，玩家无法达到
	Fame_Enmity,				// 仇恨
	Fame_Rivalry,				// 敌对
	Fame_Neutrality,			// 中立
	Fame_Amity,					// 友好
	Fame_Honor,					// 尊敬
	Fame_Adoration,				// 崇拜
	Fame_Top,					// 最高声望，玩家无法达到

	MaxFame,
};

enum InfoTypes
{
	INFO_NONE			= 0,					//未知的事件类型
	INFO_SET_TARGET,							//设置目标
	INFO_SET_TARGET_TOCLIENT,					//强制设置目标到客户端
	INFO_SET_AUTOCAST,							//设置自动施放
	INFO_BREAKCAST,								//打断施放
	INFO_QUEST_TRIGGER_NPC,						//触发NPC
	INFO_QUEST_TRIGGER_OTHER,					//其它方式的触发任务
	INFO_SHORTCUT_EXCHANGE,						//交换栏位对象
	INFO_SHORTCUT_MOVEITEMOBJECT,				//移动栏位对象

	INFO_ITEM_DROP,								//丢弃物品
	INFO_ITEM_USE,								//使用物品
	INFO_ITEM_SPLIT,							//拆分物品
	INFO_ITEM_COMPACT,							//整理物品
	INFO_ITEM_ADD,								//增加或者扣除物品
	INFO_ITEM_BUY,								//购买物品
	INFO_ITEM_BATCHBUY,							//批量购买
	INFO_ITEM_REPAIR,							//修理物品
	INFO_ITEM_IDENTIFY,							//鉴定
	INFO_ITEM_IMPRESS,							//铭刻
	INFO_ITEM_STRENGTHEN,						//强化
	INFO_ITEM_PUNCHHOLE,						//装备打孔
	INFO_ITEM_MOUNTGEM,							//镶嵌宝石
	INFO_ITEM_CLOSE_MOUNTGEM,					//关闭镶嵌界面
	INFO_ITEM_UNMOUNTGEM,						//摘取宝石
	INFO_ITEM_SOULLINK,							//灵魂链接
	INFO_ITEM_WUXINGLINK,						//五行链接

	INFO_NPCSHOP,								//与NPC交易
	INFO_TRADE,									//玩家间交易
	INFO_STALL,									//普通摆摊
	INFO_BANK,									//银行交易
	INFO_MAIL,									//邮件交易
	INFO_CHANGEMAXSLOTS,						//改变栏位上限

	INFO_SWITCH,								//设置开关状态
	INFO_RISE_REQUEST,							//复活

	INFO_MISSION,								//任务处理

	INFO_LEVEL_UP,								//升级
	INFO_TRANSPORT,                             //传送
	INFO_PET_OPERATE,							//宠物
	INFO_MOUNT_PET_OPERATE,						//骑宠
	INFO_SPIRIT_OPERATE,						//元神
	INFO_SKILL_OPERATE,                         //技能操作
	INFO_COOLDOWN,								//冷却时间
	INFO_LIVINGSKILL_LEARN,                     //生活技能学习
	INFO_PRESCRIPTION_PRODUCE,                  //制造
	INFO_CANCELVOCAL,                           //取消吟唱
	INFO_FAME,									//声望
	INFO_FLY,									//飞行
	INFO_TALENT,								//天赋

	INFO_COLLECTION_ITEM,						//采集对象消息
	INFO_TIMER_TRIGGER,							//时间触发器消息
	INFO_TIMER_TRIGGER_PET_STUDY,				//宠物修行时间触发器消息

	INFO_ENTER_TRANSPORT,						// 进入传送
	INFO_UPDATEOTHERFLAG,						//

	INFO_GETENCOURAGE,							// 玩家卜卦

	INFO_INITIALIZCOMPLETE,						// 角色初始化数据PACK发送完毕


	INFO_QUICKCHANGE,							// 切换时装
	INFO_TEMPLIST,								// 改变TempList类型
	INFO_CANCELBUFF,							// 自主取消buff

	INFO_MPMANAGER,                             // 建立师徒关系
	INFO_MPMCHANGETEXT,                         // 师傅公告信息
	INFO_ITEMSPLIT,                             // 物品分解
	INFO_ITEMCOMPOSE,                           // 物品合成

	MAIL_REQUEST,								// 接收邮件申请
	MAIL_SEND,									// 发送邮件
	MAIL_READ,									// 读取邮件
	MAIL_DELETE,								// 删除邮件
	MAIL_DELETEALL,								// 删除所有邮件
	MAIL_GETITEMS,								// 收取包裹
	MAIL_GETMONEY,								// 收取金钱
	MAIL_SEND_OK,								// 发送邮件成功

	ZONE_CHAT,									// 玩家附近聊天
	FRIEND_REQUEST,								// 请求加为好友

	INFO_SUPERMARKET,							// 商城

	INFO_HELPDIRECT,                            // 请求帮助指引
};

enum
{
	COLOR_WHITE			= 0,					// 白色
	COLOR_BLACK			= 1,					// 黑色
	COLOR_RED			= 2, 					// 红色
	COLOR_GREEN			= 3,					// 绿色
	COLOR_BLUE			= 4,					// 蓝色
	COLOR_YELLOW		= 5,					// 黄色
	COLOR_PURPLE		= 6,					// 紫色
	COLOR_CYAN			= 7,					// 青色
	COLOR_GREY			= 8,					// 灰色
	COLOR_DEEPRED		= 9,					// 暗红
	COLOR_DEEPGREEN		= 10,					// 暗绿
	COLOR_DEEPBLUE		= 11,					// 暗蓝
	COLOR_DEEPYELLOW	= 12,					// 暗黄
	COLOR_DEEPPURPLE	= 13,					// 暗紫
	COLOR_DEEPCYAN		= 14,					// 暗青
	COLOR_FUCHSIN		= 15,					// 品红色
	COLOR_SHINEGREEN	= 16,					// 亮绿色
	COLOR_LAKEBLUE		= 17,					// 湖蓝色
	COLOR_ORANGE		= 18,					// 橙黄色
	COLOR_BROWNPURPLE	= 19,					// 褐紫色
	COLOR_NEWGREEN		= 20,					// 新绿色
	COLOR_UNKNOWN		= 24,					// 用户自定义颜色
	COLOR_NOTIFY		= COLOR_WHITE,			// 系统提示颜色（白色）
	COLOR_WARN			= COLOR_YELLOW,			// 系统警告颜色（黄色）
	COLOR_DANGER		= COLOR_RED,			// 系统错误颜色（红色）

	FONT_SONG			= 0,					// 宋体
	FONT_HEI			= 1,					// 黑体
	FONT_KAI			= 2,					// 楷体
	FONT_LI				= 3,					// 隶书
	FONT_YH				= 4,					// 微软雅黑

	SHOWTYPE_NOTIFY		= 0,					// 系统提示
	SHOWTYPE_WARN		= 1,					// 系统警告
	SHOWTYPE_ERROR		= 2,					// 系统错误

	SHOWPOS_SCREEN		= 0,					// 屏幕中央
	SHOWPOS_CHAT		= 1,					// 聊天框
	SHOWPOS_SCRANDCHAT	= 2,					// 屏幕中央且聊天框

	SHOWLIMIT_TIME		= 0,					// 限时
	SHOWLIMIT_TIMES		= 1,					// 限次
};



// ========================================================================================================================================
//  全局常量 & 变量
// ========================================================================================================================================
extern const F32 GAMEPLAY_PERCENT_SCALE;
extern const F32 MAX_GAMEPLAY_PERCENT_SCALE;
extern const F32 MIN_GAMEPLAY_PERCENT_SCALE;
extern const F32 MAX_GAMEPLAY_PERCENT;
extern const F32 MIN_GAMEPLAY_PERCENT;
extern const F32 MAX_FORWARD_SPEED;
extern const F32 MAX_WALK_SPEED;
extern const U32 MAX_ATTACK_SPEED;
extern const U32 MIN_ATTACK_SPEED;
extern const U32 MAX_PET_GIFT;
extern const U32 MIN_PET_GIFT;
extern const U32 MAX_PET_QUALITY;
extern const U32 PET_STATSPOINTS_PERLEVEL;

extern const U32 g_FamilyBuff[];
extern const S32 g_FameMax[];
extern const S32 g_LevelExp[];
extern const S32 g_PetLevelExp[];
extern const S32 g_SpiritLevelExp[];
extern const U32 g_LivingLevelExp[];
extern U32 g_Color[];

extern F32 g_PetQualityFactor[];
extern U32 g_PetGrowthVal[];
extern F32 g_PetGrowthFactor[];

extern const enFame g_FameRelationship[TotalInfluence][TotalInfluence];
// ========================================================================================================================================
//  全局函数
// ========================================================================================================================================

extern bool isNullString(StringTableEntry string);
extern StringTableEntry filterNullString(StringTableEntry string);
extern F32 getDamageReducePhy(F32 val);
extern F32 getDamageReduceSpl(F32 val);
extern F32 getDamageReducePneuma(F32 val);
extern F32 getCriticalRate(F32 val);
extern F32 getAntiCriticalRate(F32 val);
extern F32 getDodgeRate(F32 val);
extern F32 getPetGrowthFactor(U32 val);
extern void InitScriptVariable();
extern void GetStatsDesc(Stats* stats, char* color, char buff[1024]);
extern void ParsePlayerScript(char buff[1024], StringTableEntry str, U32 id);

#endif//__GAMEPLAYCOMMON_H__