//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef __RES_H__
#define __RES_H__

#pragma once
#include "Gameplay/Item/ItemBaseData.h"
#include "core/stringTable.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/GameplayCommon.h"
#include "Util/LocalString.h"

#define SUB(a,b) a * 100 + b
#define GETSUB(a) a%100

struct stItemInfo;
class Player;

class Res
{
public:
	enum Constants
	{
		CATEGORY_WEAPON					= 1,			//1武器装备
		CATEGORY_EQUIPMENT				= 2,			//2防具装备
		CATEGORY_ORNAMENT				= 3,			//3饰品
		CATEGORY_PET					= 4,			//宠物
		CATEGORY_GOODS					= 5,			//物品
		CATEGORY_MATERIAL				= 6,			//材料
		CATEGORY_TOOLS					= 7,			//工具
		CATEGORY_MISSION				= 8,			//任务
		CATEGORY_DRAWING				= 9,			//配方
		CATEGORY_GEM					= 10,			//宝石升级
		CATEGORY_MAGIC					= 11,			//附魔升级
		CATEGORY_PACK					= 12,			//包裹
		CATEGORY_SCROLL					= 13,			//卷轴
		CATEGORY_OTHER					= 14,			//其他
		CATEGORY_PETEQUIP				= 15,			//宠物装备
		CATEGORY_ONADD					= 16,			//附加材料
		CATEGORY_TRUMP					= 17,			//法宝
		CATEGORY_PETEGG					= 18,			//宠物封印
		CATEGORY_MOUNTPETEGG			= 19,			//骑宠封印

		CATEGORY_FAQI					= SUB(CATEGORY_WEAPON, 1),		//法器
		CATEGORY_SIGLESHORT				= SUB(CATEGORY_WEAPON, 2),		//单短
		CATEGORY_DOUBLESHORT			= SUB(CATEGORY_WEAPON, 3),		//双短
		CATEGORY_QIN					= SUB(CATEGORY_WEAPON, 4),		//琴
		CATEGORY_BOW					= SUB(CATEGORY_WEAPON, 5),		//弓
		CATEGORY_FLAGSTAFF				= SUB(CATEGORY_WEAPON, 6),		//旗杖 
		CATEGORY_ARMS					= SUB(CATEGORY_WEAPON, 7),		//刀斧

		CATEGORY_HEAD					= SUB(CATEGORY_EQUIPMENT, 1),	//头部
		CATEGORY_BODY					= SUB(CATEGORY_EQUIPMENT, 2),	//身体
		CATEGORY_BACK					= SUB(CATEGORY_EQUIPMENT, 3),	//背部
		CATEGORY_SHOULDER				= SUB(CATEGORY_EQUIPMENT, 4),	//肩部
		CATEGORY_HAND					= SUB(CATEGORY_EQUIPMENT, 5),	//手部
		CATEGORY_WAIST					= SUB(CATEGORY_EQUIPMENT, 6),	//腰部
		CATEGORY_FOOT					= SUB(CATEGORY_EQUIPMENT, 7),	//脚部
		CATEGORY_FASHION_FAMILY			= SUB(CATEGORY_EQUIPMENT, 8),	//门派时装
		CATEGORY_FASHION_HEAD			= SUB(CATEGORY_EQUIPMENT, 9),	//头部时装
		CATEGORY_FASHION_BODY			= SUB(CATEGORY_EQUIPMENT, 10),	//身体时装
		CATEGORY_FASHION_BACK			= SUB(CATEGORY_EQUIPMENT, 11),	//背部时装

		CATEGORY_AMULET					= SUB(CATEGORY_ORNAMENT, 1),	//护身符
		CATEGORY_NECK					= SUB(CATEGORY_ORNAMENT, 2),	//颈部
		CATEGORY_RING					= SUB(CATEGORY_ORNAMENT, 3),	//戒指

		CATEGORY_RIDE					= SUB(CATEGORY_PET, 1),			//坐骑
		CATEGORY_SUB_PET				= SUB(CATEGORY_PET, 2),			//宠物
		CATEGORY_SUMMONER				= SUB(CATEGORY_PET, 3),			//召唤

		CATEGORY_FOOD					= SUB(CATEGORY_GOODS, 1),		//食物
		CATEGORY_MEDICA					= SUB(CATEGORY_GOODS, 2),		//药品
		CATEGORY_TONIC					= SUB(CATEGORY_GOODS, 3),		//补品
		CATEGORY_SWATHE					= SUB(CATEGORY_GOODS, 4),		//绷带
		CATEGORY_SKILLMATERIAL			= SUB(CATEGORY_GOODS, 5),		//技能材料
		CATEGORY_PLAYNATERIAL			= SUB(CATEGORY_GOODS, 6),		//施法材料
		CATEGORY_BOOK					= SUB(CATEGORY_GOODS, 7),		//书籍
		CATEGORY_SKILLBOOK				= SUB(CATEGORY_GOODS, 8),		//技能书
		CATEGORY_TREASURE				= SUB(CATEGORY_GOODS, 9),		//宝物
		CATEGORY_SUBGOODS				= SUB(CATEGORY_GOODS, 10),		//物品
		CATEGORY_PETFOOD				= SUB(CATEGORY_GOODS, 11),		//宠物食物
		CATEGORY_PETMEDICA				= SUB(CATEGORY_GOODS, 12),		//宠物丹药
		CATEGORY_PETTOY					= SUB(CATEGORY_GOODS, 13),		//宠物玩具
		CATEGORY_SPIRITSTONE1			= SUB(CATEGORY_GOODS, 14),		//元神石（用于开启元神绝招栏）
		CATEGORY_SPIRITSTONE2			= SUB(CATEGORY_GOODS, 15),		//元神玉（用于清除元神绝招）

		CATEGORY_HERBAL_MEDICINE		= SUB(CATEGORY_MATERIAL, 1),	//草药
		CATEGORY_MINERAL				= SUB(CATEGORY_MATERIAL, 2),	//矿物	
		CATEGORY_CROP					= SUB(CATEGORY_MATERIAL, 3),	//农作物
		CATEGORY_WOOD					= SUB(CATEGORY_MATERIAL, 4),	//木材
		CATEGORY_FISH					= SUB(CATEGORY_MATERIAL, 5),	//鱼类
		CATEGORY_MEAT					= SUB(CATEGORY_MATERIAL, 6),	//兽肉
		CATEGORY_SUNDRIES				= SUB(CATEGORY_MATERIAL, 7),	//杂物
		CATEGORY_SKIN					= SUB(CATEGORY_MATERIAL, 8),	//皮革
		CATEGORY_MAGICMATERIAL			= SUB(CATEGORY_MATERIAL, 9),	//魔法材料
		CATEGORY_PAPER					= SUB(CATEGORY_MATERIAL, 10),	//纸

		CATEGORY_GATHER_TOOL			= SUB(CATEGORY_TOOLS, 1),		//采集工具
		CATEGORY_MINERAL_TOOL			= SUB(CATEGORY_TOOLS, 2),		//挖矿工具
		CATEGORY_CULTURE_TOOL			= SUB(CATEGORY_TOOLS, 3),		//耕种工具
		CATEGORY_CUTTING_TOOL			= SUB(CATEGORY_TOOLS, 4),		//伐木工具
		CATEGORY_FISH_TOOL				= SUB(CATEGORY_TOOLS, 5),		//钓鱼工具
		CATEGORY_HUNT_TOOL				= SUB(CATEGORY_TOOLS, 6),		//狩猎工具
		CATEGORY_PET_TOOL				= SUB(CATEGORY_TOOLS, 7),		//抓宠工具
		CATEGORY_MAGIC_BOTTLE			= SUB(CATEGORY_TOOLS, 8),		//磨瓶
		CATEGORY_FOUND_TOOL				= SUB(CATEGORY_TOOLS, 9),		//铸造工具
		CATEGORY_SEWING_TOOL			= SUB(CATEGORY_TOOLS, 10),		//裁缝工具
		CATEGORY_ART_TOOL				= SUB(CATEGORY_TOOLS, 11),		//工艺工具
		CATEGORY_CHARM_TOOL				= SUB(CATEGORY_TOOLS, 12),		//符咒工具
		CATEGORY_SMELT_TOOL				= SUB(CATEGORY_TOOLS, 13),		//炼丹工具
		CATEGORY_COOK_TOOL				= SUB(CATEGORY_TOOLS, 14),		//烹饪工具
		CATEGORY_ARTISAN_TOOL			= SUB(CATEGORY_TOOLS, 15),		//工匠工具
		CATEGORY_DELUDE_TOOL			= SUB(CATEGORY_TOOLS, 16),		//蛊术工具

		CATEGORY_CANUSE					= SUB(CATEGORY_MISSION, 1),		//任务（触发）
		CATEGORY_CANOTUSE				= SUB(CATEGORY_MISSION, 2),		//任务物品

		CATEGORY_FOUND					= SUB(CATEGORY_DRAWING, 1),		//铸造
		CATEGORY_SEWING					= SUB(CATEGORY_DRAWING, 2),		//裁缝
		CATEGORY_ART					= SUB(CATEGORY_DRAWING, 3),		//工艺
		CATEGORY_CHARM					= SUB(CATEGORY_DRAWING, 4),		//符咒
		CATEGORY_SMELT					= SUB(CATEGORY_DRAWING, 5),		//炼丹
		CATEGORY_COOKING				= SUB(CATEGORY_DRAWING, 6),		//烹饪
		CATEGORY_ARTISAN				= SUB(CATEGORY_DRAWING, 7),		//工匠
		CATEGORY_DELUDE					= SUB(CATEGORY_DRAWING, 8),		//蛊术
		CATEGORY_DRAWING_OTHER			= SUB(CATEGORY_DRAWING, 9),		//其他		

		CATEGORY_GEM_COLOR1				= SUB(CATEGORY_GEM, 1),			//红宝石
		CATEGORY_GEM_COLOR2				= SUB(CATEGORY_GEM, 2),			//绿宝石
		CATEGORY_GEM_COLOR3				= SUB(CATEGORY_GEM, 3),			//蓝宝石
		CATEGORY_GEM_COLOR4				= SUB(CATEGORY_GEM, 4),			//黄宝石
		CATEGORY_GEM_COLOR5				= SUB(CATEGORY_GEM, 5),			//青宝石
		CATEGORY_GEM_COLOR6				= SUB(CATEGORY_GEM, 6),			//紫宝石
		CATEGORY_GEM_COLOR7				= SUB(CATEGORY_GEM, 7),			//五彩石
		CATEGORY_GEM_COLOR8				= SUB(CATEGORY_GEM, 8),			//黄晶石
		CATEGORY_GEM_COLOR9				= SUB(CATEGORY_GEM, 9),			//青晶石
		CATEGORY_GEM_COLOR10			= SUB(CATEGORY_GEM, 10),		//紫晶石
		CATEGORY_GEM_COLOR11			= SUB(CATEGORY_GEM, 11),		//
		CATEGORY_GEM_COLOR12			= SUB(CATEGORY_GEM, 12),		//
		CATEGORY_GEM_COLOR13			= SUB(CATEGORY_GEM, 13),		//
		CATEGORY_GEM_COLOR14			= SUB(CATEGORY_GEM, 14),		//
		CATEGORY_GEM_COLOR15			= SUB(CATEGORY_GEM, 15),		//

		CATEGORY_UP_MAGIC_WEAPON		= SUB(CATEGORY_MAGIC, 1),		//武器附魔升级
		CATEGORY_UP_MAGIC_DEFEND		= SUB(CATEGORY_MAGIC, 2),		//防具附魔升级
		CATEGORY_UP_MAGIC_ACCOUNTER		= SUB(CATEGORY_MAGIC, 3),		//饰品附魔升级
		CATEGORY_UP_MAGIC_ARMET			= SUB(CATEGORY_MAGIC, 4),		//头盔附魔升级
		CATEGORY_UP_MAGIC_SHOULDER		= SUB(CATEGORY_MAGIC, 5),		//肩膀附魔升级
		CATEGORY_UP_MAGIC_MANTLE		= SUB(CATEGORY_MAGIC, 6),		//披风附魔升级
		CATEGORY_UP_MAGIC_CLOTH			= SUB(CATEGORY_MAGIC, 7),		//衣服附魔升级
		CATEGORY_UP_MAGIC_SHOSE			= SUB(CATEGORY_MAGIC, 8),		//鞋子附魔升级
		CATEGORY_UP_MAGIC_ARMGUARD		= SUB(CATEGORY_MAGIC, 9),		//护手附魔升级
		CATEGORY_UP_MAGIC_CAESTUS		= SUB(CATEGORY_MAGIC, 10),		//腰带附魔升级
		CATEGORY_UP_MAGIC_NECKLACE		= SUB(CATEGORY_MAGIC, 11),		//项链附魔升级
		CATEGORY_UP_MAGIC_RING			= SUB(CATEGORY_MAGIC, 12),		//戒指附魔升级
		CATEGORY_UP_MAGIC_AMULET		= SUB(CATEGORY_MAGIC, 13),		//护身附魔升级
		CATEGORY_UP_MAGIC_ALL			= SUB(CATEGORY_MAGIC, 14),		//全部附魔升级
		CATEGORY_UP_MAGIC_TRUMP			= SUB(CATEGORY_MAGIC, 15),		//法宝附魔升级

		CATEGORY_INVE_FOREVER_PACK		= SUB(CATEGORY_PACK, 1),		//物品栏永久包裹
		CATEGORY_BANK_FOREVER_PACK		= SUB(CATEGORY_PACK, 2),		//仓库栏永久包裹
		CATEGORY_STAL_FOREVER_PACK		= SUB(CATEGORY_PACK, 3),		//摆摊栏永久包裹
		CATEGORY_INVE_TIME_PACK			= SUB(CATEGORY_PACK, 4),		//物品栏时效包裹
		CATEGORY_BANK_TIME_PACK			= SUB(CATEGORY_PACK, 5),		//仓库栏时效包裹
		CATEGORY_STAL_TIME_PACK			= SUB(CATEGORY_PACK, 6),		//摆摊栏时效包裹
		CATEGORY_STAL_STYLE_PACK		= SUB(CATEGORY_PACK, 7),		//摆摊栏样式包裹

		CATEGORY_BACK_CITY				= SUB(CATEGORY_SCROLL, 1),		//回城符
		CATEGORY_RECORD_SCROLL			= SUB(CATEGORY_SCROLL, 2),		//记录卷轴
		CATEGORY_PICTURE_CHECK			= SUB(CATEGORY_SCROLL, 3),		//图鉴
		CATEGORY_SUB_SCROLL				= SUB(CATEGORY_SCROLL, 4),		//卷轴
		CATEGORY_IDENTIFY				= SUB(CATEGORY_SCROLL, 5),		//鉴定符
		CATEGORY_BECOMEBABY				= SUB(CATEGORY_SCROLL, 6),		//还童丹

		CATEGORY_SMOKE_FLOVER			= SUB(CATEGORY_OTHER, 1),		//烟花
		CATEGORY_DOWRY					= SUB(CATEGORY_OTHER, 2),		//嫁妆
		CATEGORY_PHYSICS_EQUIPMET		= SUB(CATEGORY_OTHER, 3),		//元神装备
		CATEGORY_CHIP					= SUB(CATEGORY_OTHER, 4),		//筹码
		CATEGORY_KEY					= SUB(CATEGORY_OTHER, 5),		//钥匙
		CATEGORY_CHANGED_CARD			= SUB(CATEGORY_OTHER, 6),		//变身卡

		CATEGORY_PETEQUIP_HEAD			= SUB(CATEGORY_PETEQUIP, 1),	//宠物头
		CATEGORY_PETEQUIP_BODY			= SUB(CATEGORY_PETEQUIP, 2),	//宠物身体
		CATEGORY_PETEQUIP_BACK			= SUB(CATEGORY_PETEQUIP, 3),	//宠物背
		CATEGORY_PETEQUIP_WEAPON		= SUB(CATEGORY_PETEQUIP, 4),	//宠物武器

		CATEGORY_STRENTHEN_BASE			= SUB(CATEGORY_ONADD, 1),		//强化基础材料	
		CATEGORY_STRENTHEN_ADD			= SUB(CATEGORY_ONADD, 2),		//强化附加材料
		CATEGORY_PUNCHHOLE				= SUB(CATEGORY_ONADD, 3),		//打孔材料
		CATEGORY_GET_TREASURE			= SUB(CATEGORY_ONADD, 4),		//摘取材料
		CATEGORY_ITEM_IDENTIFY			= SUB(CATEGORY_ONADD, 5),		//鉴定材料
		CATEGORY_ITEM_SOULLINK			= SUB(CATEGORY_ONADD, 6),		//灵魂链接材料
		CATEGORY_ITEM_GENGUDAN			= SUB(CATEGORY_ONADD, 7),		//根骨丹
		GATEGORY_TRUMP_RECAST			= SUB(CATEGORY_ONADD, 8),		//法宝重铸材料
		GATEGORY_TRUMP_COMPOSE			= SUB(CATEGORY_ONADD, 9),		//法宝合成材料
		GATEGORY_TRUMP_STRENTHEN		= SUB(CATEGORY_ONADD, 10),		//法宝强化材料

		CATEGORY_PETEGG_NORMAL			= SUB(CATEGORY_PETEGG, 1),		//灵兽宝宝封印	
		CATEGORY_PETEGG_VARIATION		= SUB(CATEGORY_PETEGG, 2),		//变异灵兽宝宝封印
		CATEGORY_PETEGG_GROWN_NORMAL	= SUB(CATEGORY_PETEGG, 3),		//灵兽封印
		CATEGORY_PETEGG_GROWN_VARIATION	= SUB(CATEGORY_PETEGG, 4),		//变异灵兽封印

		CATEGORY_MOUNTPETEGG_NORMAL		= SUB(CATEGORY_MOUNTPETEGG, 1),	//普通骑宠封印
	};

	// 道具随机附加属性类别
	enum PropertyType
	{
		PROTYPE_RANDALL		= 1,		//鉴定附加属性
		PROTYPE_RANDNAME	= 2,		//名称前缀属性
		PROTYPE_STRENGTHEN	= 3,		//装备强化属性
		PROTYPE_IMPRESS		= 4,		//刻铭属性
		PROTYPE_SOULBIND	= 5,		//灵魂绑定属性
		PROTYPE_WUXING		= 6,		//五行属性
		PROTYPE_BESET		= 7,		//宝石镶嵌属性
		PROTYPE_GODHAND		= 8,		//神器属性
	};

	// 道具性别限制
	enum SexLimit
	{											
		SEXLIMIT_BOTH	= 0,			//男女适用
		SEXLIMIT_MALE	= 1,			//男性使用
		SEXLIMIT_FEMALE	= 2,			//女性使用
	};

	//物品装配时的交换部位
	enum MountType
	{							
		MOUNTTYPE_NONE	= 1,			//不装配
		MOUNTTYPE_LINK	= 2,			//链接
		MOUNTTYPE_CHANGE= 3,			//替换	
	};

	// 装配点
	enum MountNode
	{
		MOUNTNODE_NONE	= 0,			//空
		MOUNTNODE_BODY	= 1,			//身体
	};

	// 时效计算方式
	enum TimeMode
	{
		TIMEMODE_NONE,					// 不做扣时要求
		TIMEMODE_DOWNONLY,				// 仅下线扣时
		TIMEMODE_ONLINEGET,				// 获取后在线扣时
		TIMEMODE_ONLINEUSE,				// 使用后在线扣时
		TIMEMODE_ALLGET,				// 获取后上下线都扣时
		TIMEMODE_ALL,					// 使用后上线下线都扣时
	};

	// 物品绑定类型
	enum BindMode
	{
		BINDMODE_PICKUP	= 1,					// 拾取绑定
		BINDMODE_EQUIP	= 2,					// 装备绑定
		BINDMODE_NONE	= 3,					// 不绑定
	};

	// 门宗限制
	enum FamilyType
	{
		FAMILYTYPE_NONE		= BIT(Family_None),		// 无门宗
		FAMLIYTYPE_SHENG	= BIT(Family_Sheng),	// 圣
		FAMLIYTYPE_FO		= BIT(Family_Fo),		// 佛
		FAMLIYTYPE_XIAN		= BIT(Family_Xian),		// 仙
		FAMLIYTYPE_JING		= BIT(Family_Jing),		// 精
		FAMLIYTYPE_GUI		= BIT(Family_Gui),		// 鬼
		FAMILYTYPE_GUAI		= BIT(Family_Guai),		// 怪
		FAMLIYTYPE_YAO		= BIT(Family_Yao),		// 妖
		FAMLIYTYPE_MO		= BIT(Family_Mo),		// 魔

		FAMILYTYPE_COMMON	= FAMLIYTYPE_SHENG | FAMLIYTYPE_XIAN | FAMILYTYPE_GUAI |
		FAMLIYTYPE_GUI | FAMLIYTYPE_JING | 

		FAMLIYTYPE_YAO | FAMLIYTYPE_FO | FAMLIYTYPE_MO,
	};

	// 基本限制标志
	enum ItemFlag
	{
		ITEM_TRADE		=  1 << 0,		// 与玩家交易
		ITEM_SELLNPC	=  1 << 1,		// 出售给NPC
		ITEM_SAVEBANK	=  1 << 2,		// 存仓库
		ITEM_FIX		=  1 << 3,		// 可修理
		ITEM_DROPEQUIP	=  1 << 4,		// 卸装后消失(目前只用于坐骑道具)
		ITEM_DOWNLINE	=  1 << 5,		// 下线消失
		ITEM_CANUSE		=  1 << 6,		// 可使用
		ITEM_SMELT		=  1 << 7,		// 淬炼
		ITEM_DROP		=  1 << 8,		// 丢弃
		ITEM_USEINFIGHT =  1 << 9,		// 战斗中使用
		ITEM_USEDESTROY =  1 << 10,		// 使用后消失
		ITEM_MISSION	=  1 << 11,		// 任务使用
		ITEM_UPGRADE	=  1 << 12,		// 升级
		ITEM_ENERGY		=  1 << 13,		// 充能
		ITEM_PICKUPONLY =  1 << 14,		// 唯一拾取
		ITEM_SHENQI		=  1 << 15,		// 神器
		ITEM_SOULLINK	=  1 << 16,		// 允许灵魂链接
		ITEM_SEARCH		=  1 << 17,		// 允许被搜索
		ITEM_USESCRIPT	=  1 << 18,		// 允许执行使用脚本
		ITEM_MOUNTSCRIPT=  1 << 19,		// 允许执行上装脚本
		ITEM_UNMOUNTSCRIPT = 1 << 20,	// 允许执行卸装脚本
		ITEM_MONEY		=  1 << 21,		// 元宝道具
	};

	// 效果激活(APF)
	enum ActivePropertyFlag
	{
		APF_BIND			= EAPF_BIND,		// 绑定效果
		APF_ATTACH			= EAPF_ATTACH,		// 附加效果
		APF_WUXING			= EAPF_WUXING,		// 五行效果
		APF_IMPRESS			= EAPF_IMPRESS,		// 装备铭刻属性
		APF_EQUIPSKILLA		= EAPF_EQUIPSKILLA,	// 装备后主动技能
		APF_EQUIPSKILLB		= EAPF_EQUIPSKILLB,	// 装备后被动技能		
	};

	// 鉴定时间规则
	// 注：只有装备类才涉及[装备鉴定]，宠物类才涉及[宠物鉴定]
	enum IdentifyType
	{
		IDENTIFYTYPE_NONE	= 1,				// 无需鉴定
		IDENTIFYTYPE_DROP	= 2,				// 掉落时鉴定
		IDENTIFYTYPE_EQUIP	= 3,				// 装备时鉴定
		IDENTIFYTYPE_NPC	= 4,				// NPC手动鉴定
	};

	//物品兑换类型
	enum SaleType
	{
		SALETYPE_MONEY		= 1,				// 金元
		SALETYPE_GOLD		= 2,				// 灵元
		SALETYPE_HONOR		= 3,				// 战功
		SALETYPE_BINDMONEY	= 4,				// 绑定金钱
		SALETYPE_CREDIT		= 5,				// 声望
		SALETYPE_BINDGOLD	= 6,				// 仙石
	};


	enum EquipPlaceFlag
	{
		EQUIPPLACEFLAG_UNKNOWN		= 0,			// 未知类型

		EQUIPPLACEFLAG_HEAD			= 1,			// 头部
		EQUIPPLACEFLAG_NECK			= 2,			// 颈部
		EQUIPPLACEFLAG_BACK			= 3,			// 背部
		EQUIPPLACEFLAG_SHOULDER		= 4,			// 肩部
		EQUIPPLACEFLAG_BODY			= 5,			// 身体
		EQUIPPLACEFLAG_HAND			= 6,			// 手部
		EQUIPPLACEFLAG_WAIST		= 7,			// 腰部
		EQUIPPLACEFLAG_FOOT			= 8,			// 脚部
		EQUIPPLACEFLAG_RING			= 9,			// 戒指
		EQUIPPLACEFLAG_AMULET		= 10,			// 护身符
		EQUIPPLACEFLAG_WEAPON		= 11,			// 武器
		EQUIPPLACEFLAG_TRUMP		= 12,			// 法宝类武器
		EQUIPPLACEFLAG_GATHER		= 13,           // 采集道具
		EQUIPPLACEFLAG_RIDE			= 14,			// 坐骑道具(在UI上隐藏)
		EQUIPPLACEFLAG_PET			= 15,			// 宠物道具(在UI上隐藏)
		EQUIPPLACEFLAG_FASHIONHEAD  = 16,           // 时装头
		EQUIPPLACEFLAG_FASHIONBODY  = 17,           // 时装身体
		EQUIPPLACEFLAG_FASHIONBACK  = 18,           // 时装背部
		EQUIPPLACEFLAG_PACK1		= 19,			// 扩展背包1
		EQUIPPLACEFLAG_PACK2		= 20,			// 扩展背包2
		EQUIPPLACEFLAG_BANKPACK		= 21,			// 银行扩展格
		EQUIPPLACEFLAG_STALLPACK1	= 22,			// 摆摊扩展格1
		EQUIPPLACEFLAG_STALLPACK2	= 23,			// 摆摊扩展格2
		EQUIPPLACEFLAG_STALLPACK3	= 24,			// 摆摊样式格
		EQUIPPLACEFLAG_FAMLIYFASHION= 25,			// 门宗时装
	};

	// 物品颜色等级
	enum ColorLevel
	{
		COLORLEVEL_UNKNOWN  = 0,					// 无
		COLORLEVEL_GREY		= 1,					// 灰色
		COLORLEVEL_WHITE	= 2,					// 白色
		COLORLEVEL_GREEN	= 3,					// 绿色
		COLORLEVEL_BLUE		= 4,					// 蓝色
		COLORLEVEL_PURPLE	= 5,					// 紫色
		COLORLEVEL_ORANGE	= 6,					// 橙色
		COLORLEVEL_RED		= 7,					// 红色
		COLORLEVEL_MAX		= 8,					// 无
	};

	enum WuXing
	{
		WUXING_NONE		= 0,
		WUXING_GOOD		= 1,
		WUXING_WOOD		= 2,
		WUXING_WATER	= 3,
		WUXING_FIRE		= 4,
		WUXING_DUST		= 5,
	};

	enum RepairType
	{
		REPAIR_NORMAL		= 0,					// 普通修理
		REPAIR_RECOVER		= 1,					// 恢复修理
		REPAIR_DURABLE		= 2,					// 经久修理
	};

	enum ActiveFlagType
	{
		ACTIVEF_NONE		= 0,					// 无
		ACTIVEF_ISEQUIP		= 1 << 0,				// 是装备物品(可有附加属性)
		ACTIVEF_TRADE		= 1 << 1,				// 交易后激活
		ACTIVEF_RANDPRO		= 1 << 2,				// 是否随机强化等级附加属性
	};

	//宝石限制
	enum StoneLimit
	{
		Stone_None			= 0,					//无限制

		Stone_Faqi			= 1 << 0,				//法器
		Stone_Sigleshort	= 1 << 1,
		Stone_Doubleshort	= 1 << 2,
		Stone_Arms			= 1 << 3,
		Stone_Qin			= 1 << 4,		
		Stone_Bow			= 1 << 5,
		Stone_Flagstaff		= 1 << 6,

		Stone_Trump			= 1 << 7,				// 法宝

		Stone_Head			= 1 << 8,
		Stone_Body			= 1 << 9,
		Stone_Back			= 1 << 10,
		Stone_Shouder		= 1 << 11,
		Stone_Hand			= 1 << 12,
		Stone_Waist			= 1 << 13,
		Stone_Foot			= 1 << 14,

		Stone_Amulet		= 1 << 15,
		Stone_Neck			= 1 << 16,
		Stone_Ring			= 1 << 17,
	};

public:
	Res();
	~Res();
	inline bool isSameBaseData(Res* res);
	inline bool isSameRes(Res* res);
	inline bool isSameExt(Res* res);
	// ============================================================================
	// 基本模板数据方法
	// ============================================================================
	inline bool setBaseData(U32 itemid);
	inline ItemBaseData* getBaseData();
	inline U32 getItemID();
	inline U32 getCategory();
	inline U32 getSubCategory();
	inline StringTableEntry getItemName();
	inline StringTableEntry getIconName();
	StringTableEntry	getCategoryName();
	inline U16 getPetInsight();
	inline U16 getPetTalent();
	inline StringTableEntry getDescription();
	inline StringTableEntry getHelp();
	inline StringTableEntry getPurpose();
	inline StringTableEntry getSoundID();
	inline U32 getMaxAdroit();
	inline U32 getBindMode();
	inline U32 getTimeMode();
	inline U32 getDuration();
	inline S32 getSaleType();
	inline S32 getSalePrice();
	inline U32 getMaxOverNum();
	inline U32 getMaxWear();
	inline U32 getResUsedTimes();
	inline U32 getDropRate();
	inline U32 getSuitID();
	inline U32 getMissionID();
	inline U32 getFamilyCredit();
	inline U32 getColorLevel();
	inline U32 getQualityLevel();
	inline U32 getCanEmbedSlots();
	inline U32 getCoolTimeType();
	inline U32 getCoolTime();
	inline U32 getVocalTime();
	inline U32 getRenascences();
	inline U32 getOpenedEmbedSlot();
	inline U32 getAllowEmbedSlot();
	inline U32 getEquipEffectID();
	inline U32 getEquipStrengthenEffectID();
	inline U32 getMaxEnhanceWear();
	inline U32 getShapesSetID(U32 sex);
	inline U32 getBindPro(U32 index);
	inline U32 getEmbedPro(U32 index);
	inline S32 getAdroitLv(U32 index);
	inline U32 getAdroitPro(U32 index);
	inline U32 getEquipUpgradePro(U32 index);
	inline U32 getIdentifyType();
	inline U32 getBaseAttribute();
	inline U32 getAppendAttribute(U32 index);
	inline U32 getAppendAttributeID(U32 index);
	inline U32 getUseStateID();
	inline U32 getLimitLevel();
	inline U32 getSexLimit();
	inline bool getMissionNeed(Vector<S32>& NeedMissionList);

	inline bool isWeapon();
	inline bool isRangeWeapon();
	inline bool isTrump();
	inline bool isEquipment();
	inline bool isOrnament();
	inline bool isBody();
	inline bool isEquip();
	inline bool isRide();
	inline bool isPet();
	inline bool isPetEquip();
	inline bool isPetEgg();
	inline bool isMountPetEgg();
	inline bool isGather();	
	inline bool isGem();
	inline bool isTaskItem();
	inline bool isOnlyOne();
	inline bool isSetCoolTime();
	inline bool isSkillBook();
	inline bool isSpiritStone1();
	inline bool isSpiritStone2();
	inline bool isBagItem();
	inline bool isInventoryBag();
	inline bool isBankBag();
	inline bool isStallBag();
	inline bool isStallStyleBag();
	inline bool isMaterial();
	inline bool isHuanTongDan();
	inline bool	isIdentify();
	inline bool canLapOver();
	inline bool canBaseLimit(U32 flag);
	inline bool canSexLimit(U32 sex);
	inline bool canFamilyLimit(U32 famliy);
	inline bool canLevelLimit(U32 level);
	inline bool canIdentify(U32 identify);
	inline bool hasIdentified();
	inline bool hasUsedScript();
	inline bool hasEqupScript();
	inline bool hasUnequpScript();
	inline bool	isGenGuDan();
	inline bool	getPetFoodType(S32 &nFoodType);
	inline bool isPetFood();
	inline bool isPetMedical();
	inline bool isPetToy();

	inline U32 getAppendStatsID(U32 index);
	inline U32 getRandomName();
	inline StringTableEntry getPrefixName();
	inline StringTableEntry getPostfixName();
	inline U32  getPostfixQuality();
	inline S32 getReserveValue();
	inline StringTableEntry getTrailEffectTexture();

	S32 getEquipPos();
	bool canPutEquip(S32 index);

	// ============================================================================
	// 扩展数据方法
	// ============================================================================
	bool				setExtData				();
	bool				setExtData				(stItemInfo* info);
	stItemInfo*			getExtData				();

	StringTableEntry	getProducer				();
	void				setProducer				(StringTableEntry name);
	U64					getUID					();
	void				setUID					(U64 uid);
	S32					getQuantity				();
	void				setQuantity				(S32 num, bool ignorecheck = false);
	U32					getBindPlayerID			();
	void				setBindPlayerID			(U32 playerid);
	U32					getBindFriendID			();
	void				setBindFriendID			(U32 playerid);
	U32					getActiveFlag			();
	void				setActiveFlag			(U32 flag, bool isSet = true);
	U32					getLapseTime			();
	void				setLapseTime			(U32 val);
	U32					getUsedTimes			();
	void				setUsedTimes			(S32 times);
	U32					getLockedLeftTime		();
	void				setLockedLeftTime		(S32 times);
	S32					getCurrentWear			();
	void				setCurrentWear			(S32 wear);
	S32					getCurrentMaxWear		();
	void				setCurrentMaxWear		(S32 max);
	S32					getCurAdroit			();
	void				setCurAdroit			(S32 adroit);
	S32					getCurAdroitLv			();
	void				setCurAdroitLv			(S32 adroitlv);
	bool				IsActivatePro			(U32 flag);
	void				setActivatePro			(U32 flag);
	U32					getIDEProNum			();
	U32					getIDEProValue			(S32 index);
	void				setIDEProValue			(S32 index, U32 val);
	bool				isRandStengthens		();
	U32					getEquipStrengthens		();
	void				getEquipStrengthenVal	(S32 index, U32& lv, U32& val);
	void				setEquipStrengthenVal	(S32 index, U32 lv, U32 val);
	U32					getImpressID			();
	void				setImpressID			(U32 statsid);
	U32					getSoulLinkStatsID		();
	void				setSoulLinkStatsID		(U32 statsid);
	
	U32					getSubPropertyID		(U32 statsid);
	inline S32			getColorValue			();	
	bool				canTrade				();
	U32					getStoneLimit			();

	//镶嵌、打孔相关函数
	bool				canMountGem				(U32 nGemLimit);
	S32					getOpenedHoles			();
	S32					getAllowedHoles			();
	S32					getLeftHoles			();

	S32					GetSlotColor			(S32 nIndex);
	bool				IsSlotOpened			(S32 nIndex);
	bool				IsSlotAllowEmbed		(S32 nIndex);
	bool				IsSlotEmbedable			(S32 nIndex);
	bool				IsSlotEmbeded			(S32 nIndex);
	S32					getActualPrice			();
	void				getItemName				(char* colorName, U32 size);
	bool				setRepairResult			(U32 type);
	U32					getRepairNeedMoney		(U8 level = 1);
	U32					getRepairNeedMoney1		();
	U32					getGemQualityValue		();
	U32					getItemQualityValue		();

	// ============================================================================
	// 公共方法
	// ============================================================================
	inline char* GetColorText(char *buff, U32 buffSize);		//通过颜色等级获得显示颜色字符串

public:
	static U32 sColorValue[COLORLEVEL_MAX + 1];
	static StringTableEntry getCategoryName(U32 cat,U32 subCat);
protected:
	ItemBaseData*	mBaseData;			// 基本模板数据
	stItemInfo*		mExtData;			// 扩展数据
};

#include "Gameplay/item/Res_inline.h"

#endif//__RES_H__