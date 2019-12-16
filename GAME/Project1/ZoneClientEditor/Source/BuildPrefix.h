#ifndef __BUILDPREFIX_H__
#define __BUILDPREFIX_H__

#pragma warning(disable: 4018)	// 屏蔽有符号与无符号不匹配警告
#pragma warning(disable: 4100)	// 参数没引用
#pragma warning(disable: 4189)	// 变量定义了但没引用
#pragma warning(disable: 4127)	// 表达式常数
#pragma warning(disable: 4244)	// Argument type conversion warning.
#pragma warning(disable: 4245)	// 有符号/无符号不匹配
#pragma warning(disable: 4305)	// Variable initializing conversion warning.
#pragma warning(disable: 4311)	// 屏蔽类型转换” : 从“NTJu8 (*__w64)[3000]”到“unsigned int”的指针截断
#pragma warning(disable: 4312)	// 屏蔽类型转换” : 从“const NTJu32”转换到更大的“void *”
#pragma warning(disable: 4389)	// 有符号/无符号不匹配

#pragma warning(disable: 4511)	// 未能生成复制构造函数
#pragma warning(disable: 4512)	// 未能生成赋值运算符
#pragma warning(disable: 4267)	// 数据之间转换，数据丢失
#pragma warning(disable: 4800)	// 屏蔽强制转换为true或false的性能警告

#pragma warning(disable: 4505)	// 有变量定义了但是未使用
#pragma warning(error: 4819)

 

//#define WIN32					1
// Use ASM version blend
#define USEASSEMBLYTERRBLEND	1

// ====================================================================================
//  CONTENT BUILD
// ====================================================================================

#ifdef NTJ_INTEGRATED
	#define NTJ_CLIENT
	#define NTJ_CLIENT_GM
	#define NTJ_SERVER
	#define NTJ_CHAT_SERVER
#endif

// ====================================================================================
//  动态加载
// ====================================================================================
#ifdef NTJ_CLIENT
# ifdef DYNAMIC_LOAD
#	define NTJ_CLIENT__DYNAMIC_LOAD
# endif
#endif

// ====================================================================================
//  DEBUG BUILD
// ====================================================================================

#ifdef NTJ_DEBUG
//	#define USE_SIDE_ANIMATIONS		// Whether or not use side left/right animation.
	#define BUILD_SUFFIX			"_DEBUG"
	#define DEBUG					1
	#define ENABLE_ASSERTS
	#define ENABLE_TRACE			//是否使能时间追踪器，方便调试时找到低效的代码块
	#define PNG_NO_READ_TIME
	#define PNG_NO_WRITE_TIME
	#define NTJ_CHECKMEMORY
#endif


#ifdef NTJ_RELEASE
	#define _CRASH_CRT_
#endif

#ifdef NTJ_CHECKMEMORY
#define _CRTDBG_MAP_ALLOC
#endif

// ========================================================================================================================================
//	游戏目录路径宏定义
// ========================================================================================================================================
#define GAME_CLIENTINI_FILE					"config/client.ini"
#define GAME_CONFIGINI_FILE					"config/Config.ini"
#define GAME_SYSSTRING_INI					"gameres/client/sysstring.ini"
#define GAME_ITEMDESC_INI					"gameres/data/repository/itemdesc.ini"
#define GAME_ACTIONDATA_FILE				"gameres/data/repository/actionrepository.dat"
#define GAME_BUFFDATA_FILE					"gameres/data/repository/buffrepository.dat"
#define GAME_DROPRULE_FILE					"gameres/data/repository/droprulerepository.dat"
#define GAME_EFFECTDATA_FILE				"gameres/data/repository/effectrepository.dat"
#define GAME_ITEMPACKAGE_FILE				"gameres/data/repository/itempackagerepository.dat"
#define GAME_MODEDATA_FILE					"gameres/data/repository/modelrepository.dat"
#define GAME_NPCDATA_FILE					"gameres/data/repository/npcrepository.dat"
#define GAME_PLAYDATA_FILE					"gameres/data/repository/playerrepository.dat"
#define GAME_ROUTEDATA_FILE					"gameres/data/repository/routerepository.dat"
#define GAME_SHOPLIST_FILE					"gameres/data/repository/shoplist.dat"
#define GAME_SKILLDATA_FILE					"gameres/data/repository/skillrepository.dat"
#define GAME_SOUNDDATA_FILE					"gameres/data/repository/soundrepository.dat"
#define GAME_TRIGGERDATA_FILE				"gameres/data/repository/triggerrepository.dat"
#define GAME_ITEMDATA_FILE					"gameres/data/repository/itemrepository.dat"
#define GAME_RANDPROPERTY_FILE				"gameres/data/repository/randproperty.dat"
#define LIVINGSKILLFILE                     "gameres/data/repository/LivingSkillRepository.dat"
#define PRESCRIPTIONFILE                    "gameres/data/repository/PrescriptionRepository.dat"
#define GAME_COLLECTIONDATA_FILE			"gameres/data/repository/CollectionRepository.dat"
#define GAME_CALLBOARDDAILYDATA_FILE        "gameres/data/repository/CallboardDailyData.dat"
#define GAME_CALLBOARDMISSIONDATA_FILE      "gameres/data/repository/CallboardMissionData.dat"
#define GAME_CALLBOARDEVENTDATA_FILE        "gameres/data/repository/CallboardEventData.dat"
#define GAME_AUTONPCSELL_FILE				"config/cache/%s/autosell.dat"
#define GAME_PANELFILE						"config/cache/%s/panelui.dat"
#define GAME_MODSETDATA_FILE				"gameres/data/repository/ModSetRepository.dat"
#define GAME_AICDDATA_FILE					"gameres/data/repository/AICdRepository.dat"
#define GAME_AIOPDATA_FILE					"gameres/data/repository/AIOpRepository.dat"
#define GAME_AISPELLDATA_FILE				"gameres/data/repository/AISpellRepository.dat"
#define GAME_AICHARDATA_FILE				"gameres/data/repository/AIRepository.dat"
#define GAME_MARKETDATA_FILE				"gameres/data/repository/SuperMarketRepository.dat"
#define GAME_PACKGOODS_FILE					"gameres/data/repository/PackGoods.dat"
#define GAME_VIEWMAPDATA_FILE	            "gameres/data/repository/ViewMapRepository.dat"

// ========================================================================================================================================
//	编译时注解
// ========================================================================================================================================
#define ITOA(NUMBER)  #NUMBER
#define __TEXT_LINE__(LINE) ITOA(LINE)
#ifdef _DEBUG
#define HACK(MSG) __FILE__"("__TEXT_LINE__(__LINE__)") : ★HACK★ → "#MSG
#else
#define HACK(MSG) __FILE__"("__TEXT_LINE__(__LINE__)") : HACK!! Error: "#MSG
#endif

//#define ENGINE(MSG) __FILE__"("__TEXT_LINE__(__LINE__)") : ●ENGINE● → "#MSG
#define ENGINE(MSG) ""

#undef  SAFE_DELETE
#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;

#undef  SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(a) if( (a) != NULL ) delete [] (a); (a) = NULL;

#undef  SAFE_FREE
#define SAFE_FREE(a)   if( (a) != NULL ) dFree (a); (a) = NULL;

typedef unsigned __int64 BIGINT;

// ========================================================================================================================================
//	游戏错误定义与描述
// 服务端使用:		WarnMessageEvent::AddScreenMessage(Player* toplayer,enWarnMessage messagetype)
// 客户端使用:		g_UIMouseGamePlay->AddScreenMessage(string message)
// 客户端脚本使用:  AddScreenMessage(字符串)	
// ========================================================================================================================================
#ifndef WARN_MESSAGE
#define WARN_MESSAGE
//
// 注：枚举值需要与sysString.ini文件中键项匹配
enum enWarnMessage
{
	MSG_NONE						= 0,		// 正确,不需要提示

	// ================   1- 499	服务器系统消息===================================
	MSG_TRANSFER_NOMAP				= 1,		// 传送没有有效的目标地图
	MSG_TRANSFER_CANNOT				= 2,		// 您未够等级，还不能传送
	MSG_TRANSFER_OVERTIME			= 3,		// 传送超时，请再重新试一次
	MSG_CANNTAIPATH					= 6,		// 该区域不能寻径
	MSG_SERVERBUSY					= 7,		// 服务器繁忙，请稍候再试

	// ================ 500-999		常规逻辑消息  ===================================
	MSG_PLAYER_JOB					= 500,		// 职业不符合
	MSG_PLAYER_SEX					= 501,		// 性别不符合
	MSG_PLAYER_FAMILY				= 502,		// 门宗不符合
	MSG_PLAYER_LEVEL				= 503,		// 等级不符合
	MSG_PLAYER_RACE					= 504,		// 声望不符合
	MSG_PLAYER_MONEYNOTENOUGH		= 505,		// 金钱不够
	MSG_PLAYER_GOLDNOTENOUGH		= 506,		// 元宝不够
	MSG_PLAYER_MATERIAL_NOTENOUGH	= 507,		// 材料不够
	MSG_PLAYER_EXP                  = 508,      // 经验不够
	MSG_PLAYER_TITLE                = 509,      // 称号不符合
	MSG_PLAYER_INSIGHT              = 510,      // 悟性太低
	MSG_PLAYER_VIGOUR               = 511,      // 活力不足

	MSG_PLAYER_BUSY					= 520,		// 玩家正忙，无法做其它事情
	MSG_PLAYER_TARGETBUSY			= 521,		// 对方玩家正忙,无法做其它事情
	MSG_PLAYER_RIDE					= 522,		// 您正在骑乘状态中
	MSG_PLAYER_CANNOTQUERY			= 523,		// 查询正忙，请稍侯再试
	MSG_PLAYER_ISDEAD				= 524,		// 玩家已经死亡
	MSG_PLAYER_TOOFAR				= 525,		// 距离目标太远
	MSG_PLAYER_FIGHTING				= 526,		// 玩家正在战斗中
	MSG_PLAYER_TARGET_FIGHTING		= 527,		// 对方玩家正在战斗中
	MSG_PLAYER_TARGET_INVALID		= 528,		// 目标无效
	MSG_PLAYER_TARGET_LOST			= 529,		// 目标丢失
	MSG_PLAYER_SPELLRUNNING			= 530,		// 正在施放技能，禁止其它操作
	MSG_PLAYER_BUFFDISABLED			= 531,		// 受到状态影响，禁止其它操作
	MSG_PLAYER_TRANSFERING			= 532,		// 正在传送中，禁止其它操作

	MSG_PLAYER_ADDMONEY				= 550,		// 您获得了金钱: %s
	MSG_PLAYER_REDUCEMONEY			= 551,		// 您花费了金钱: %s
	MSG_PLAYER_ADDBINDMONEY			= 552,		// 您获得了金元: %s
	MSG_PLAYER_REDUCEBINDMONEY		= 553,		// 您花费了金元: %s
	MSG_PLAYER_ADDEXP				= 554,		// 您获得了经验: %s
	MSG_PLAYER_ADDBANKMONEY			= 555,		// 您向仓库存入: %s
	MSG_PLAYER_REDUCEBANKMONEY		= 556,		// 您从仓库提取: %s
	MSG_PLAYER_ADDVIGOR             = 557,      // 恢复活力%d点
    MSG_PLAYER_REDUCEVIGOR          = 558,      // 消耗活力%d点
    MSG_PLAYER_ADDMAXVIGOR          = 559,      // 活力上限提升到%d点 
	MSG_PLAYER_ADDLIVINGEXP         = 560,      // 增加%d点生活经验
	MSG_PLAYER_ADDLIVINGLEVEL       = 561,      // 生活等级提升到%d级

	// ================1000-1999	战斗系统消息  ==================================
	MSG_COMBAT_CANNOTATTACK			= 1000,		// 无法攻击目标
	MSG_COMBAT_ERROR				= 1001,		// 出错，动作终止
	MSG_COMBAT_CANCELOPERAT			= 1002,		// 动作被打断(打断吟唱动作)
	MSG_COMBAT_CANNOTOPERAT			= 1003,		// 当前操作无法进行

	MSG_COMBAT_UNKNOWN_TARGET		= 1050,		// 未知目标
	MSG_COMBAT_YOU					= 1051,		// 你
	MSG_COMBAT_MISS					= 1052,		// 未命中
	MSG_COMBAT_DODGE				= 1053,		// 闪避
	MSG_COMBAT_IMMUNITY				= 1054,		// 免疫
	MSG_COMBAT_ABSORB				= 1055,		// 吸收

	MSG_SKILL_LEVEL					= 1100,		// 您的等级未够，无法使用此技能
	MSG_SKILL_COODDOWN				= 1101,		// 技能冷却中，稍侯再试
	MSG_SKILL_NOTREADY				= 1102,		// 您还不能使用该技能
	MSG_SKILL_NOTSIT				= 1103,		// 只能在站立下使用技能
	MSG_SKILL_DISABLESPELL			= 1104,		// 技能被禁止施放
	MSG_SKILL_OUTRANGE				= 1105,		// 超出技能使用范围
	MSG_SKILL_CASTFAILED			= 1106,		// 技能施放失败
	MSG_SKILL_ALERADYLEARN			= 1107,		// 您已经学会此技能
	MSG_SKILL_DONTLEARN				= 1108,		// 您还未学会此技能
	MSG_SKILL_CANNOTLEARN			= 1109,		// 您还未达到学会此技能的条件
	MSG_SKILL_CANNOTDRAG			= 1110,		// 被动技能不能拖动
	MSG_SKILL_OUTOFHP				= 1111,		// 您没有足够的生命
	MSG_SKILL_OUTOFMP				= 1112,		// 您没有足够的精气
	MSG_SKILL_OUTOFPP				= 1113,		// 您没有足够的元气
	MSG_SKILL_CANNOTDOTHAT			= 1114,		// 您不能这么做
	MSG_SKILL_NEEDTARGET			= 1115,		// 需要目标
	MSG_SKILL_INVALIDTARGET			= 1116,		// 无效的目标

	MSG_COMBATLOG_SPELL				= 1200,		// 你施放了%s
	MSG_COMBATLOG_BUFF				= 1201,		// 你获得了%s
	MSG_COMBATLOG_ITEM				= 1202,		// 你使用了%s
	MSG_COMBATLOG_DAMAGE			= 1203,		// %s对%s造成了%d点伤害
	MSG_COMBATLOG_HEAL				= 1204,		// %s为%s恢复了%d点生命
	MSG_COMBATLOG_MISS				= 1205,		// %s没有击中%s
	MSG_COMBATLOG_DODGE				= 1206,		// %s闪避了%s的攻击
	MSG_COMBATLOG_IMMUNITY			= 1207,		// %s免疫了%s的攻击
	MSG_COMBATLOG_KILL				= 1208,		// %s杀死了%s

	// ================2000-2999	交易系统消息  ==================================
	MSG_TRADE_FAILED				= 2000,		// 交易失败
	MSG_TRADE_SUCCEED				= 2001,		// 交易成功
	MSG_TRADE_CANCEL				= 2002,		// 交易取消
	MSG_TRADE_TARGET_LEVEL			= 2003,		// 对方等级过低无法交易
	MSG_TRADE_SELF_LEVEL			= 2004,		// 您等级太低无法交易
	MSG_TRADE_TARGET_SWITCHCLOSE	= 2005,		// 对方未开启交易开关
	MSG_TRADE_SELF_SWITCHCLOSE		= 2006,		// 您未开启交易开关
	MSG_TRADE_TARGET_TRADELOCK		= 2007,		// 对方处于财产保护状态
	MSG_TRADE_SELF_TRADELOCK		= 2008,		// 您处于财产保护状态
	MSG_TRADE_TARGET_BUSY			= 2009,		// 对方正忙，无法交易
	MSG_TRADE_SELF_BUSY				= 2010,		// 您正忙，无法交易
	MSG_TRADE_DISTANCE				= 2012,		// 您与对方距离过远，无法交易
	MSG_TRADE_LOSTTARGET			= 2013,		// 交易目标丢失，可能与您距离过远
	MSG_TRADE_SELF_BAGNOSPACE		= 2014,		// 您包裹已满，无法交易
	MSG_TRADE_TARGET_BAGNOSPACE		= 2015,		// 对方包裹已满，无法交易
	MSG_TRADE_SELF_TRADING			= 2016,		// 您确定交易，禁止解除锁定
	MSG_TRADE_TARGET_TRADING		= 2017,		// 对方确定交易中,无法解除锁定
	MSG_TRADE_NOSPACE				= 2018,		// 交易物品栏已满
	MSG_TRADE_TARGETITEMNOEXIST		= 2019,		// 交易栏物品不存在
	MSG_TRADE_SELF_STATELOCK		= 2020,		// 您交易锁定，无法操作物品
	MSG_TRADE_NOENOUGHMONEY			= 2021,		// 您交易金钱不够

	MSG_TRADE_INVITE_FAILED			= 2100,		// 邀请交易失败
	MSG_TRADE_INVITE_CANCEL			= 2101,		// 邀请交易取消
	MSG_TRADE_INVITE_REJECT			= 2102,		// %s拒绝您的交易邀请
	MSG_TRADE_INVITE_TIMEOUT		= 2103,		// 回复邀请超时

	MSG_TRADE_NPCSHOP_NOGOODS		= 2200,		// 商店没有物品，无法交易
	MSG_TRADE_NOHAVE_ITEM			= 2201,		// 所需物品不够
	MSG_TRADE_NOENOUGH_GOODS		= 2202,		// 所购买的物品库存不够

	MSG_TRADE_STALL_FAILD			= 2300,		// 摆摊失败
	MSG_TRADE_STALL_POSITION		= 2301,		// 当前位置禁止摆摊
	MSG_TRADE_STALL_NORENAME		= 2302,		// 出摊状态无法重名名
	MSG_TRADE_STALL_NOREINFO		= 2303,		// 出摊状态无法修改摆摊信息
	MSG_TRADE_STALL_NOCHANGEPRICE   = 2304,		// 出摊状态无法改价
	MSG_TRADE_STALL_ITEMNUM			= 2305,		// 摆摊物品数量不足
	MSG_TRADE_STALL_STOP			= 2306,		// 已收摊
	MSG_TRADE_NOSET_STALL			= 2307,     // 该区域无法摆摊
	MSG_TRADE_STALL_UNABLEDOWN		= 2308,     // 出摊状态无法下架
	MSG_STALL_NOPOSITION			= 2309,     // 该区域无摆摊点
	MSG_TRADE_STALL_UNABLEUP		= 2310,		// 出摊状态无法上架


	MSG_TRADE_MAIL_SUCCESS			= 2400,		// 发送邮件成功
	MSG_TRADE_MAIL_FAILED			= 2401,		// 发送邮件失败

	MSG_BANK_LOCK					= 2500,		// 仓库被锁定

	// ================3000-3999	物品系统消息  ==================================
	MSG_ITEM_NOSELECT				= 3000,		// 请确认物品被选中
	MSG_ITEM_ITEMDATAERROR			= 3001,		// 物品数据错误
	MSG_ITEM_CANNOTDRAG				= 3002,		// 此物品无法拖动
	MSG_ITEM_CANNOTMOUNT			= 3003,		// 此物品无法装备
	MSG_ITEM_CANNOTUSE				= 3004,		// 此物品无法使用
	MSG_ITEM_CANNOTDROP				= 3005,		// 此物品无法丢弃
	MSG_ITEM_CANNOTTRADE			= 3006,		// 此物品无法交易
	MSG_ITEM_CANNOTFIXED			= 3007,		// 此物品无法修理
	MSG_ITEM_CANNOTIDENT			= 3008,		// 此物品无法鉴定
	MSG_ITEM_CANNOTSALE				= 3009,		// 此物品不能出售
	MSG_ITEM_CANNOTMULTI			= 3010,		// 此物品唯一拥有
	MSG_ITEM_CANNOTPICKUP			= 3011,		// 此物品无法拾取
	MSG_ITEM_CANNOOPERATE			= 3020,		// 目标不是物品，不能操作
	MSG_ITEM_ITEMBEUSED				= 3021,		// 物品正在使用中，无法操作
	MSG_ITEM_OBJECTHASITEM			= 3022,		// 目标位置已经有物品
	MSG_ITEM_CANNOTDRAGTOOBJECT		= 3023,		// 此物品不能放到目标位置
	MSG_ITEM_USEITEMNOOBJECT		= 3024,		// 使用物品错误，目标无效
	MSG_ITEM_ALREADYHASSHORTCUT		= 3025,		// 此物品已经在快捷栏中
	MSG_ITEM_ITEMNOSPACE			= 3026,		// 包裹已满,无法放入
	MSG_ITEM_NOFINDITEM				= 3027,		// 没有找到此物品
	MSG_ITEM_NOENOUGHNUM			= 3028,		// 包裹中物品数量不够
	MSG_ITEM_ITEMCOOLDOWN			= 3029,		// 物品的使用还在冷却中
	MSG_ITEM_BAGNOEMPTY1			= 3030,		// 第1个背包里有物品，不能更换小的背包
	MSG_ITEM_BAGNOEMPTY2			= 3031,		// 第2个背包里有物品，不能更换小的背包
	MSG_ITEM_BAGLIMIT				= 3032,		// 包裹容量已达上限
	MSG_ITEM_MOUNTPOSITIONERROR		= 3040,		// 装备物品位置错误
	MSG_ITEM_EQUIPMENTSHATTER		= 3041,		// 装备已经损坏
	MSG_ITEM_HAVENOTINDENTIFIED		= 3042,		// 装备未曾经过鉴定
	MSG_ITEM_UPGRADE_LIMIT			= 3043,		// 装备不能升级
	MSG_ITEM_HADINDENTIFIED			= 3044,		// 已经鉴定
	MSG_ITEM_HADNOTINDENTIFIED		= 3045,		// 装备未通过鉴定
	MSG_ITEM_STRENGTHENSUCESS1		= 3046,		// 强化成功
	MSG_ITEM_STRENGTHENSUCESS2		= 3047,		// 强化连升成功
	MSG_ITEM_STRENGTHENFAILURE1		= 3048,		// 强化失败
	MSG_ITEM_STRENGTHENFAILURE2		= 3049,		// 强化失败,受到惩罚
	MSG_ITEM_WEAPONOREQUIPMENT		= 3050,		// 物品必须是武器或防具
	MSG_ITEM_GEM					= 3051,		// 物品必须是宝石类型
	MSG_ITEM_SLOT_EMBEDED			= 3052,		// 此槽已经被镶嵌
	MSG_ITEM_SLOT_NOTEMBEDED		= 3053,		// 此槽未被镶嵌
	MSG_ITEM_SLOT_NOTOPENED			= 3054,		// 此槽没有被打孔
	MSG_ITEM_GEM_SLOT_COLOR			= 3055,		// 宝石的颜色与孔的颜色不相同
	MSG_ITEM_PUNCHHOLE_FULL			= 3056,		// 此装备上的孔已满，不能再打孔
	MSG_ITEM_PUNCHHOLE_NONE			= 3057,		// 此装备不能打孔
	MSG_ITEM_PUNCHHOLE_FAIL			= 3058,		// 打孔失败,金钱被扣除
	MSG_ITEM_CANCEL_DRAGEMBED		= 3059,		// 用户取消拖动宝石到镶嵌槽
	MSG_ITEM_GEM_LEVEL				= 3060,		// 镶嵌所用宝石必须为等级宝石
	MSG_ITEM_BANK_NOSPACE			= 3061,     // 仓库已满
	MSG_ITEM_BANK_SAVEERROR         = 3062,     // 仓库存储错误
	MSG_ITEM_TOOLS_NOMOUNT          = 3063,     // 未装备工具
	MSG_ITEM_TOOLS_NOEQUAL          = 3064,     // 请装备正确的工具
	MSG_ITEM_STRENGTHENMAXLEVEL		= 3065,		// 强化等级到达上限
	MSG_ITEM_STRENGTHENPROPERR		= 3066,		// 无强化属性
	MSG_ITEM_GEMPARTLIMIT   		= 3067,		// 宝石镶嵌部位限制
	MSG_STALL_LOOKUP_NOSPACE		= 3068,		// 收购栏已满
	MSG_ITEM_CANNOTSAVEBANK			= 3069,		// 物品不能存入银行
	MSG_ITEM_CANNOTNEEDFIXED		= 3070,		// 此物品不需修理
	MSG_ITEM_PANELLOCKED			= 3071,		// 快捷栏锁定
	MSG_ITEM_SELECTTARGET_ERROR		= 3072,		// 目标选择错误

	// ================4000-4899	生活系统消息  ==================================
	MSG_LIVINGSKILL_DATAERROR       = 4000,     // 生活技能数据出错
	MSG_LIVINGSKILL_RIPE            = 4001,     // 熟练度不符
	MSG_LIVINGSKILL_PREPERROR       = 4002,     // 未满足前置技能学习条件
	MSG_LIVINGSKILL_LEARNDONE       = 4003,     // 该技能已学习
	MSG_LIVINGSKILL_NOMINING        = 4004,     // 需采矿技能
	MSG_LIVINGSKILL_NOFELLING       = 4005,     // 需伐木技能
	MSG_LIVINGSKILL_FISHING         = 4006,     // 需钓鱼技能
	MSG_LIVINGSKILL_PLANTING        = 4007,     // 需种植技能
	MSG_LIVINGSKILL_HERB            = 4008,     // 需采药技能
	MSG_LIVINGSKILL_SHIKAR          = 4009,     // 需狩猎技能
	MSG_LIVINGSKILL_NOMININGLOW     = 4010,     // 采矿技能等级太低
	MSG_LIVINGSKILL_NOFELLINGLOW    = 4011,     // 伐木技能等级太低
	MSG_LIVINGSKILL_FISHINGLOW      = 4012,     // 钓鱼技能等级太低
	MSG_LIVINGSKILL_PLANTINGLOW     = 4013,     // 种植技能等级太低
	MSG_LIVINGSKILL_HERBLOW         = 4014,     // 采药技能等级太低
	MSG_LIVINGSKILL_SHIKARLOW       = 4015,     // 狩猎技能等级太低
	MSG_LIVINGSKILL_PRO             = 4016,     // 专精技能修练中
	MSG_LIVINGSKILL_USESKILL        = 4017,     // 使用技能获得熟练度%d
	MSG_LIVINGSKILL_USEPRODUCE      = 4018,     // 制造获得熟练度%d

	MSG_PRESCRIPTION_DATAERROR      = 4100,     // 配方数据出错
	MSG_PRESCRIPTION_MATERIALERROR  = 4101,     // 材料数据出错
	MSG_PRESCRIPTION_NOPRODUCE      = 4102,     // 此地不能合成
	MSG_PRESCRIPTION_ERROR          = 4103,     // 合成失败
	//=================4800-4899    物品合成与分解==================================
	MSG_ITEMSPLIT_NOSHORTCUT        = 4800,     // 无效目标槽位
	MSG_ITEMSPLIT_NOSPLIT           = 4801,     // 请您先拾取所有目标槽位的材料
	MSG_ITEMSPLIT_ERROR             = 4802,     // 分解失败
	MSG_ITEMSPLIT_INDEXERROR        = 4803,     // 无效索引槽位
	MSG_ITEMSPLIT_ISLOCK            = 4804,     // 分解物品.锁定当中
	MSG_ITEMCOMPOSE_LOCK            = 4820,     // 合成物品,锁定当中
	MSG_ITEMCOMPOSE_NOPICKUP        = 4821,     // 请您先拾取合成产物  
	MSG_ITEMCOMPOSE_ERROR           = 4822,     // 合成失败
	MSG_ITEMCOMPOSE_NOTHING         = 4823,     // 您还未放入任何材料

	// ================4900-4999	师徒系统消息  ==================================
	MSG_MPMANAGER_LEVEL             = 4900,     // 您资格提升到%s
	MSG_MPMANAGER_ADD_MASTEREXP     = 4901,     // 师德增加%d点
	MSG_MPMANAGER_DEC_MASTEREXP     = 4902,     // 师德消耗%d点
	MSG_MPMANAGER_ADD_EXPPO0L       = 4903,     // 您队友贡献了%d点经验
	MSG_MPMANAGER_DEC_EXPPO0L       = 4904,     // 您对换了%d点经验
	MSG_MPMANAGER_MASTEREXP_ERROR   = 4905,     // 可对换经验不够
	MSG_MPMANAGER_EXPPOOL_ERROR     = 4906,     // 师德不足
	MSG_MPMANAGER_PRENTICE_STATE    = 4907,     // 当前徒弟状态
	MSG_MPMANAGER_PRENTICE_FULL     = 4908,     // 当前徒弟已满
	MSG_MPMANAGER_LEVEL_ZERO        = 4909,     // 未获得资格
	MSG_MPMANAGER_FRIEND_ERROR      = 4910,     // 您未与对方好友关系
	MSG_MPMANAGER_LEAVEMASTER       = 4911,     // 您已出师，不再拥有拜师资格
	MSG_MPMANAGER_NOTEAM            = 4912,     // 您还未组队
	MSG_MPMANAGER_ERRORTEAM         = 4913,     // 队伍必需是两人
	MSG_MPMANAGER_DESTPLAEYR_ERROR  = 4914,     // 无效目标玩家
	MSG_MPMANAGER_DATAERROR         = 4915,     // 师徒数据出错
	MSG_MPMANAGER_DEST_LEVELZERO    = 4916,     // 对方未获得资格

	// ================5000-5999	灵兽系统消息  ==================================
	MSG_PET_ITEM_NOT_GENGUDAN		= 5000,		// 道具不是根骨丹
	MSG_PET_STATUS_NOT_IDLE			= 5001,		// 该灵兽不处于待机状态
	MSG_PET_MAXIMUM_INSIGHT			= 5002,		// 该灵兽悟性已达到最大
	MSG_PET_GENGUDAN_LEVEL			= 5003,		// 该灵兽灵慧与根骨丹等级不匹配
	MSG_PET_LOW_HAPPINESS			= 5004,		// 该灵兽的快乐度不够
	MSG_PET_TALENT_ZERO				= 5005,		// 该灵兽的根骨为零
	MSG_PET_NOT_EXIST				= 5006,		// 灵兽不存在
	MSG_NPC_CAN_NOT_TAME			= 5007,		// 此Npc不能进行捕获
	MSG_PET_HIGH_LEVEL				= 5008,		// 该灵兽的等级较高
	MSG_PET_LOW_HP					= 5009,		// 该灵兽的生命较低
	MSG_PET_DATA_NOT_EXIST			= 5010,		// 灵兽数据错误
	MSG_PET_INFO_SLOT_FULL			= 5011,		// 灵兽槽已满
	MSG_PET_ITEM_NOT_HUANTONG		= 5012,		// 道具不是还童丹
	MSG_PET_ITEM_NO_HUANTONGDAN		= 5013,		// 没有选择还童丹
	MSG_PET_LOW_LEVEL				= 5014,		// 该灵兽的等级不够
	MSG_PET_STATUS_NOT_COMBAT		= 5015,		// 该灵兽处于战斗状态
	MSG_PET_HAD_BEEN_IDENTIFIED		= 5016,		// 该灵兽已被鉴定
	MSG_PET_LIANHUA_LEVEL			= 5017,		// 灵兽必须到10级才能炼化
	MSG_MOUNT_NOT_EXIST				= 5101,		// 骑宠不存在
	MSG_MOUNT_DATA_NOT_EXIST		= 5102,		// 骑宠数据错误
	MSG_MOUNT_STATUS_NOT_IDLE		= 5103,		// 该骑宠不处于待机状态
	MSG_MOUNT_INFO_SLOT_FULL		= 5104,		// 骑宠槽已满
	MSG_MOUNT_STATUS_IDLE			= 5105,		// 该骑宠处于待机状态
	MSG_PET_CANNOT_FEED				= 5106,		// 该灵兽不能喂食
	MSG_PET_CANNOT_TAME				= 5107,		// 该灵兽不能驯服
	MSG_PET_CANNOT_MEDICAL			= 5108,		// 该灵兽不能补气
	MSG_PET_FOUNDNO_FOOD			= 5109,		// 没有合适的灵兽食物
	MSG_PET_FOUNDNO_MEDICAL			= 5110,		// 没有合适的灵兽丹药
	MSG_PET_FOUNDNO_TOY				= 5111,		// 没有合适的灵兽玩具
	MSG_PET_NAME_TOO_LONG			= 5112,		// 灵兽名字的长度太大
	MSG_PET_N0_GENGUDAN				= 5113,		// 没有合适的根骨丹

	// ================6000-6999	元神系统消息  ==================================
	MSG_SPIRIT_SLOT_INVALID			= 6000,		// "无效的元神栏位"
	MSG_SPIRIT_SLOT_OPENED			= 6001,		// "此元神栏已开启"
	MSG_SPIRIT_SLOT_NOT_OPENED		= 6002,		// "此元神栏未开启"
	MSG_SPIRIT_FULL_SLOT			= 6003,		// "没有可用的元神栏位"
	MSG_SPIRIT_NOT_EXIST			= 6004,		// "此栏没有元神"
	MSG_SPIRIT_BE_ACTIVE			= 6005,		// "此元神处于激活状态"
	MSG_SPIRIT_BE_INACTIVE			= 6006,		// "此元神处于未激活状态"
	MSG_SPIRIT_ONLY_ONE				= 6007,		// "只剩一个元神"
	MSG_SPIRIT_SKILL_NOT_OPENED		= 6008,		// 此绝招栏没有解封
	MSG_SPIRIT_SKILL_OPENED			= 6009,		// 此绝招栏已经解封
	MSG_SPIRIT_SKILL_NOT_EXIST		= 6010,		// 此绝招栏没有绝招
	MSG_SPIRIT_SKILL_ALREADY_EXIST	= 6011,		// 此绝招栏已有绝招
	MSG_SPIRIT_SLOT_ACTIVE_NONE		= 6012,		// 没有元神被激活
	MSG_SPIRIT_NONE					= 6013,		// 没有元神
	MSG_SPIRIT_NO_STONE1			= 6014,		// 没有放入元神石
	MSG_SPIRIT_NO_STONE2			= 6015,		// 没有放入元神玉
	MSG_SPIRIT_NO_SKILLBOOK			= 6016,		// 没有放入技能书
	MSG_SPIRIT_TALENTSLOT_INVALID	= 6017,		// 无效的元神天赋位置
	MSG_SPIRIT_TALENTLEVEL_LIMIT	= 6018,		// 元神天赋等级已到最大
	MSG_SPIRIT_TALENT_LEFTPTS_ZERO	= 6019,		// 天赋剩余点数为0
	MSG_SPIRIT_BE_HUANHUA			= 6020,		// 元神处于幻化状态
	MSG_SPIRIT_BE_NOTHUANHUA		= 6021,		// 元神处于未幻化状态

	// ================8000-9998	其它系统消息  ==================================
	MSG_INPUTNUMBERERROR			= 8000,		// 请输入正确的数目
	MSG_INVALIDPARAM				= 8100,		// 无效的参数
	MSG_CALLBOARDDAILYSTART         = 8101,     // 日常任务%s已开启
	MSG_COLLECTION_COND_INVALID		= 8102,		// 不满足采集条件

	MSG_WAITING_CONFIRM				= 9998,		// 等待客户端确认操作
	MSG_UNKOWNERROR					= 9999,		// 未知错误
};

#endif

#endif//__BUILDPREFIX_H__