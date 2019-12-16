#ifndef __BUILDPREFIX_H__
#define __BUILDPREFIX_H__

#pragma warning(disable: 4018)	// �����з������޷��Ų�ƥ�侯��
#pragma warning(disable: 4100)	// ����û����
#pragma warning(disable: 4189)	// ���������˵�û����
#pragma warning(disable: 4127)	// ���ʽ����
#pragma warning(disable: 4244)	// Argument type conversion warning.
#pragma warning(disable: 4245)	// �з���/�޷��Ų�ƥ��
#pragma warning(disable: 4305)	// Variable initializing conversion warning.
#pragma warning(disable: 4311)	// ��������ת���� : �ӡ�NTJu8 (*__w64)[3000]������unsigned int����ָ��ض�
#pragma warning(disable: 4312)	// ��������ת���� : �ӡ�const NTJu32��ת��������ġ�void *��
#pragma warning(disable: 4389)	// �з���/�޷��Ų�ƥ��

#pragma warning(disable: 4511)	// δ�����ɸ��ƹ��캯��
#pragma warning(disable: 4512)	// δ�����ɸ�ֵ�����
#pragma warning(disable: 4267)	// ����֮��ת�������ݶ�ʧ
#pragma warning(disable: 4800)	// ����ǿ��ת��Ϊtrue��false�����ܾ���

#pragma warning(disable: 4505)	// �б��������˵���δʹ��
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
//  ��̬����
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
	#define ENABLE_TRACE			//�Ƿ�ʹ��ʱ��׷�������������ʱ�ҵ���Ч�Ĵ����
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
//	��ϷĿ¼·���궨��
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
//	����ʱע��
// ========================================================================================================================================
#define ITOA(NUMBER)  #NUMBER
#define __TEXT_LINE__(LINE) ITOA(LINE)
#ifdef _DEBUG
#define HACK(MSG) __FILE__"("__TEXT_LINE__(__LINE__)") : ��HACK�� �� "#MSG
#else
#define HACK(MSG) __FILE__"("__TEXT_LINE__(__LINE__)") : HACK!! Error: "#MSG
#endif

//#define ENGINE(MSG) __FILE__"("__TEXT_LINE__(__LINE__)") : ��ENGINE�� �� "#MSG
#define ENGINE(MSG) ""

#undef  SAFE_DELETE
#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;

#undef  SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(a) if( (a) != NULL ) delete [] (a); (a) = NULL;

#undef  SAFE_FREE
#define SAFE_FREE(a)   if( (a) != NULL ) dFree (a); (a) = NULL;

typedef unsigned __int64 BIGINT;

// ========================================================================================================================================
//	��Ϸ������������
// �����ʹ��:		WarnMessageEvent::AddScreenMessage(Player* toplayer,enWarnMessage messagetype)
// �ͻ���ʹ��:		g_UIMouseGamePlay->AddScreenMessage(string message)
// �ͻ��˽ű�ʹ��:  AddScreenMessage(�ַ���)	
// ========================================================================================================================================
#ifndef WARN_MESSAGE
#define WARN_MESSAGE
//
// ע��ö��ֵ��Ҫ��sysString.ini�ļ��м���ƥ��
enum enWarnMessage
{
	MSG_NONE						= 0,		// ��ȷ,����Ҫ��ʾ

	// ================   1- 499	������ϵͳ��Ϣ===================================
	MSG_TRANSFER_NOMAP				= 1,		// ����û����Ч��Ŀ���ͼ
	MSG_TRANSFER_CANNOT				= 2,		// ��δ���ȼ��������ܴ���
	MSG_TRANSFER_OVERTIME			= 3,		// ���ͳ�ʱ������������һ��
	MSG_CANNTAIPATH					= 6,		// ��������Ѱ��
	MSG_SERVERBUSY					= 7,		// ��������æ�����Ժ�����

	// ================ 500-999		�����߼���Ϣ  ===================================
	MSG_PLAYER_JOB					= 500,		// ְҵ������
	MSG_PLAYER_SEX					= 501,		// �Ա𲻷���
	MSG_PLAYER_FAMILY				= 502,		// ���ڲ�����
	MSG_PLAYER_LEVEL				= 503,		// �ȼ�������
	MSG_PLAYER_RACE					= 504,		// ����������
	MSG_PLAYER_MONEYNOTENOUGH		= 505,		// ��Ǯ����
	MSG_PLAYER_GOLDNOTENOUGH		= 506,		// Ԫ������
	MSG_PLAYER_MATERIAL_NOTENOUGH	= 507,		// ���ϲ���
	MSG_PLAYER_EXP                  = 508,      // ���鲻��
	MSG_PLAYER_TITLE                = 509,      // �ƺŲ�����
	MSG_PLAYER_INSIGHT              = 510,      // ����̫��
	MSG_PLAYER_VIGOUR               = 511,      // ��������

	MSG_PLAYER_BUSY					= 520,		// �����æ���޷�����������
	MSG_PLAYER_TARGETBUSY			= 521,		// �Է������æ,�޷�����������
	MSG_PLAYER_RIDE					= 522,		// ���������״̬��
	MSG_PLAYER_CANNOTQUERY			= 523,		// ��ѯ��æ�����Ժ�����
	MSG_PLAYER_ISDEAD				= 524,		// ����Ѿ�����
	MSG_PLAYER_TOOFAR				= 525,		// ����Ŀ��̫Զ
	MSG_PLAYER_FIGHTING				= 526,		// �������ս����
	MSG_PLAYER_TARGET_FIGHTING		= 527,		// �Է��������ս����
	MSG_PLAYER_TARGET_INVALID		= 528,		// Ŀ����Ч
	MSG_PLAYER_TARGET_LOST			= 529,		// Ŀ�궪ʧ
	MSG_PLAYER_SPELLRUNNING			= 530,		// ����ʩ�ż��ܣ���ֹ��������
	MSG_PLAYER_BUFFDISABLED			= 531,		// �ܵ�״̬Ӱ�죬��ֹ��������
	MSG_PLAYER_TRANSFERING			= 532,		// ���ڴ����У���ֹ��������

	MSG_PLAYER_ADDMONEY				= 550,		// ������˽�Ǯ: %s
	MSG_PLAYER_REDUCEMONEY			= 551,		// �������˽�Ǯ: %s
	MSG_PLAYER_ADDBINDMONEY			= 552,		// ������˽�Ԫ: %s
	MSG_PLAYER_REDUCEBINDMONEY		= 553,		// �������˽�Ԫ: %s
	MSG_PLAYER_ADDEXP				= 554,		// ������˾���: %s
	MSG_PLAYER_ADDBANKMONEY			= 555,		// ����ֿ����: %s
	MSG_PLAYER_REDUCEBANKMONEY		= 556,		// ���Ӳֿ���ȡ: %s
	MSG_PLAYER_ADDVIGOR             = 557,      // �ָ�����%d��
    MSG_PLAYER_REDUCEVIGOR          = 558,      // ���Ļ���%d��
    MSG_PLAYER_ADDMAXVIGOR          = 559,      // ��������������%d�� 
	MSG_PLAYER_ADDLIVINGEXP         = 560,      // ����%d�������
	MSG_PLAYER_ADDLIVINGLEVEL       = 561,      // ����ȼ�������%d��

	// ================1000-1999	ս��ϵͳ��Ϣ  ==================================
	MSG_COMBAT_CANNOTATTACK			= 1000,		// �޷�����Ŀ��
	MSG_COMBAT_ERROR				= 1001,		// ����������ֹ
	MSG_COMBAT_CANCELOPERAT			= 1002,		// ���������(�����������)
	MSG_COMBAT_CANNOTOPERAT			= 1003,		// ��ǰ�����޷�����

	MSG_COMBAT_UNKNOWN_TARGET		= 1050,		// δ֪Ŀ��
	MSG_COMBAT_YOU					= 1051,		// ��
	MSG_COMBAT_MISS					= 1052,		// δ����
	MSG_COMBAT_DODGE				= 1053,		// ����
	MSG_COMBAT_IMMUNITY				= 1054,		// ����
	MSG_COMBAT_ABSORB				= 1055,		// ����

	MSG_SKILL_LEVEL					= 1100,		// ���ĵȼ�δ�����޷�ʹ�ô˼���
	MSG_SKILL_COODDOWN				= 1101,		// ������ȴ�У��Ժ�����
	MSG_SKILL_NOTREADY				= 1102,		// ��������ʹ�øü���
	MSG_SKILL_NOTSIT				= 1103,		// ֻ����վ����ʹ�ü���
	MSG_SKILL_DISABLESPELL			= 1104,		// ���ܱ���ֹʩ��
	MSG_SKILL_OUTRANGE				= 1105,		// ��������ʹ�÷�Χ
	MSG_SKILL_CASTFAILED			= 1106,		// ����ʩ��ʧ��
	MSG_SKILL_ALERADYLEARN			= 1107,		// ���Ѿ�ѧ��˼���
	MSG_SKILL_DONTLEARN				= 1108,		// ����δѧ��˼���
	MSG_SKILL_CANNOTLEARN			= 1109,		// ����δ�ﵽѧ��˼��ܵ�����
	MSG_SKILL_CANNOTDRAG			= 1110,		// �������ܲ����϶�
	MSG_SKILL_OUTOFHP				= 1111,		// ��û���㹻������
	MSG_SKILL_OUTOFMP				= 1112,		// ��û���㹻�ľ���
	MSG_SKILL_OUTOFPP				= 1113,		// ��û���㹻��Ԫ��
	MSG_SKILL_CANNOTDOTHAT			= 1114,		// ��������ô��
	MSG_SKILL_NEEDTARGET			= 1115,		// ��ҪĿ��
	MSG_SKILL_INVALIDTARGET			= 1116,		// ��Ч��Ŀ��

	MSG_COMBATLOG_SPELL				= 1200,		// ��ʩ����%s
	MSG_COMBATLOG_BUFF				= 1201,		// ������%s
	MSG_COMBATLOG_ITEM				= 1202,		// ��ʹ����%s
	MSG_COMBATLOG_DAMAGE			= 1203,		// %s��%s�����%d���˺�
	MSG_COMBATLOG_HEAL				= 1204,		// %sΪ%s�ָ���%d������
	MSG_COMBATLOG_MISS				= 1205,		// %sû�л���%s
	MSG_COMBATLOG_DODGE				= 1206,		// %s������%s�Ĺ���
	MSG_COMBATLOG_IMMUNITY			= 1207,		// %s������%s�Ĺ���
	MSG_COMBATLOG_KILL				= 1208,		// %sɱ����%s

	// ================2000-2999	����ϵͳ��Ϣ  ==================================
	MSG_TRADE_FAILED				= 2000,		// ����ʧ��
	MSG_TRADE_SUCCEED				= 2001,		// ���׳ɹ�
	MSG_TRADE_CANCEL				= 2002,		// ����ȡ��
	MSG_TRADE_TARGET_LEVEL			= 2003,		// �Է��ȼ������޷�����
	MSG_TRADE_SELF_LEVEL			= 2004,		// ���ȼ�̫���޷�����
	MSG_TRADE_TARGET_SWITCHCLOSE	= 2005,		// �Է�δ�������׿���
	MSG_TRADE_SELF_SWITCHCLOSE		= 2006,		// ��δ�������׿���
	MSG_TRADE_TARGET_TRADELOCK		= 2007,		// �Է����ڲƲ�����״̬
	MSG_TRADE_SELF_TRADELOCK		= 2008,		// �����ڲƲ�����״̬
	MSG_TRADE_TARGET_BUSY			= 2009,		// �Է���æ���޷�����
	MSG_TRADE_SELF_BUSY				= 2010,		// ����æ���޷�����
	MSG_TRADE_DISTANCE				= 2012,		// ����Է������Զ���޷�����
	MSG_TRADE_LOSTTARGET			= 2013,		// ����Ŀ�궪ʧ���������������Զ
	MSG_TRADE_SELF_BAGNOSPACE		= 2014,		// �������������޷�����
	MSG_TRADE_TARGET_BAGNOSPACE		= 2015,		// �Է������������޷�����
	MSG_TRADE_SELF_TRADING			= 2016,		// ��ȷ�����ף���ֹ�������
	MSG_TRADE_TARGET_TRADING		= 2017,		// �Է�ȷ��������,�޷��������
	MSG_TRADE_NOSPACE				= 2018,		// ������Ʒ������
	MSG_TRADE_TARGETITEMNOEXIST		= 2019,		// ��������Ʒ������
	MSG_TRADE_SELF_STATELOCK		= 2020,		// �������������޷�������Ʒ
	MSG_TRADE_NOENOUGHMONEY			= 2021,		// �����׽�Ǯ����

	MSG_TRADE_INVITE_FAILED			= 2100,		// ���뽻��ʧ��
	MSG_TRADE_INVITE_CANCEL			= 2101,		// ���뽻��ȡ��
	MSG_TRADE_INVITE_REJECT			= 2102,		// %s�ܾ����Ľ�������
	MSG_TRADE_INVITE_TIMEOUT		= 2103,		// �ظ����볬ʱ

	MSG_TRADE_NPCSHOP_NOGOODS		= 2200,		// �̵�û����Ʒ���޷�����
	MSG_TRADE_NOHAVE_ITEM			= 2201,		// ������Ʒ����
	MSG_TRADE_NOENOUGH_GOODS		= 2202,		// ���������Ʒ��治��

	MSG_TRADE_STALL_FAILD			= 2300,		// ��̯ʧ��
	MSG_TRADE_STALL_POSITION		= 2301,		// ��ǰλ�ý�ֹ��̯
	MSG_TRADE_STALL_NORENAME		= 2302,		// ��̯״̬�޷�������
	MSG_TRADE_STALL_NOREINFO		= 2303,		// ��̯״̬�޷��޸İ�̯��Ϣ
	MSG_TRADE_STALL_NOCHANGEPRICE   = 2304,		// ��̯״̬�޷��ļ�
	MSG_TRADE_STALL_ITEMNUM			= 2305,		// ��̯��Ʒ��������
	MSG_TRADE_STALL_STOP			= 2306,		// ����̯
	MSG_TRADE_NOSET_STALL			= 2307,     // �������޷���̯
	MSG_TRADE_STALL_UNABLEDOWN		= 2308,     // ��̯״̬�޷��¼�
	MSG_STALL_NOPOSITION			= 2309,     // �������ް�̯��
	MSG_TRADE_STALL_UNABLEUP		= 2310,		// ��̯״̬�޷��ϼ�


	MSG_TRADE_MAIL_SUCCESS			= 2400,		// �����ʼ��ɹ�
	MSG_TRADE_MAIL_FAILED			= 2401,		// �����ʼ�ʧ��

	MSG_BANK_LOCK					= 2500,		// �ֿⱻ����

	// ================3000-3999	��Ʒϵͳ��Ϣ  ==================================
	MSG_ITEM_NOSELECT				= 3000,		// ��ȷ����Ʒ��ѡ��
	MSG_ITEM_ITEMDATAERROR			= 3001,		// ��Ʒ���ݴ���
	MSG_ITEM_CANNOTDRAG				= 3002,		// ����Ʒ�޷��϶�
	MSG_ITEM_CANNOTMOUNT			= 3003,		// ����Ʒ�޷�װ��
	MSG_ITEM_CANNOTUSE				= 3004,		// ����Ʒ�޷�ʹ��
	MSG_ITEM_CANNOTDROP				= 3005,		// ����Ʒ�޷�����
	MSG_ITEM_CANNOTTRADE			= 3006,		// ����Ʒ�޷�����
	MSG_ITEM_CANNOTFIXED			= 3007,		// ����Ʒ�޷�����
	MSG_ITEM_CANNOTIDENT			= 3008,		// ����Ʒ�޷�����
	MSG_ITEM_CANNOTSALE				= 3009,		// ����Ʒ���ܳ���
	MSG_ITEM_CANNOTMULTI			= 3010,		// ����ƷΨһӵ��
	MSG_ITEM_CANNOTPICKUP			= 3011,		// ����Ʒ�޷�ʰȡ
	MSG_ITEM_CANNOOPERATE			= 3020,		// Ŀ�겻����Ʒ�����ܲ���
	MSG_ITEM_ITEMBEUSED				= 3021,		// ��Ʒ����ʹ���У��޷�����
	MSG_ITEM_OBJECTHASITEM			= 3022,		// Ŀ��λ���Ѿ�����Ʒ
	MSG_ITEM_CANNOTDRAGTOOBJECT		= 3023,		// ����Ʒ���ܷŵ�Ŀ��λ��
	MSG_ITEM_USEITEMNOOBJECT		= 3024,		// ʹ����Ʒ����Ŀ����Ч
	MSG_ITEM_ALREADYHASSHORTCUT		= 3025,		// ����Ʒ�Ѿ��ڿ������
	MSG_ITEM_ITEMNOSPACE			= 3026,		// ��������,�޷�����
	MSG_ITEM_NOFINDITEM				= 3027,		// û���ҵ�����Ʒ
	MSG_ITEM_NOENOUGHNUM			= 3028,		// ��������Ʒ��������
	MSG_ITEM_ITEMCOOLDOWN			= 3029,		// ��Ʒ��ʹ�û�����ȴ��
	MSG_ITEM_BAGNOEMPTY1			= 3030,		// ��1������������Ʒ�����ܸ���С�ı���
	MSG_ITEM_BAGNOEMPTY2			= 3031,		// ��2������������Ʒ�����ܸ���С�ı���
	MSG_ITEM_BAGLIMIT				= 3032,		// ���������Ѵ�����
	MSG_ITEM_MOUNTPOSITIONERROR		= 3040,		// װ����Ʒλ�ô���
	MSG_ITEM_EQUIPMENTSHATTER		= 3041,		// װ���Ѿ���
	MSG_ITEM_HAVENOTINDENTIFIED		= 3042,		// װ��δ����������
	MSG_ITEM_UPGRADE_LIMIT			= 3043,		// װ����������
	MSG_ITEM_HADINDENTIFIED			= 3044,		// �Ѿ�����
	MSG_ITEM_HADNOTINDENTIFIED		= 3045,		// װ��δͨ������
	MSG_ITEM_STRENGTHENSUCESS1		= 3046,		// ǿ���ɹ�
	MSG_ITEM_STRENGTHENSUCESS2		= 3047,		// ǿ�������ɹ�
	MSG_ITEM_STRENGTHENFAILURE1		= 3048,		// ǿ��ʧ��
	MSG_ITEM_STRENGTHENFAILURE2		= 3049,		// ǿ��ʧ��,�ܵ��ͷ�
	MSG_ITEM_WEAPONOREQUIPMENT		= 3050,		// ��Ʒ���������������
	MSG_ITEM_GEM					= 3051,		// ��Ʒ�����Ǳ�ʯ����
	MSG_ITEM_SLOT_EMBEDED			= 3052,		// �˲��Ѿ�����Ƕ
	MSG_ITEM_SLOT_NOTEMBEDED		= 3053,		// �˲�δ����Ƕ
	MSG_ITEM_SLOT_NOTOPENED			= 3054,		// �˲�û�б����
	MSG_ITEM_GEM_SLOT_COLOR			= 3055,		// ��ʯ����ɫ��׵���ɫ����ͬ
	MSG_ITEM_PUNCHHOLE_FULL			= 3056,		// ��װ���ϵĿ������������ٴ��
	MSG_ITEM_PUNCHHOLE_NONE			= 3057,		// ��װ�����ܴ��
	MSG_ITEM_PUNCHHOLE_FAIL			= 3058,		// ���ʧ��,��Ǯ���۳�
	MSG_ITEM_CANCEL_DRAGEMBED		= 3059,		// �û�ȡ���϶���ʯ����Ƕ��
	MSG_ITEM_GEM_LEVEL				= 3060,		// ��Ƕ���ñ�ʯ����Ϊ�ȼ���ʯ
	MSG_ITEM_BANK_NOSPACE			= 3061,     // �ֿ�����
	MSG_ITEM_BANK_SAVEERROR         = 3062,     // �ֿ�洢����
	MSG_ITEM_TOOLS_NOMOUNT          = 3063,     // δװ������
	MSG_ITEM_TOOLS_NOEQUAL          = 3064,     // ��װ����ȷ�Ĺ���
	MSG_ITEM_STRENGTHENMAXLEVEL		= 3065,		// ǿ���ȼ���������
	MSG_ITEM_STRENGTHENPROPERR		= 3066,		// ��ǿ������
	MSG_ITEM_GEMPARTLIMIT   		= 3067,		// ��ʯ��Ƕ��λ����
	MSG_STALL_LOOKUP_NOSPACE		= 3068,		// �չ�������
	MSG_ITEM_CANNOTSAVEBANK			= 3069,		// ��Ʒ���ܴ�������
	MSG_ITEM_CANNOTNEEDFIXED		= 3070,		// ����Ʒ��������
	MSG_ITEM_PANELLOCKED			= 3071,		// ���������
	MSG_ITEM_SELECTTARGET_ERROR		= 3072,		// Ŀ��ѡ�����

	// ================4000-4899	����ϵͳ��Ϣ  ==================================
	MSG_LIVINGSKILL_DATAERROR       = 4000,     // ��������ݳ���
	MSG_LIVINGSKILL_RIPE            = 4001,     // �����Ȳ���
	MSG_LIVINGSKILL_PREPERROR       = 4002,     // δ����ǰ�ü���ѧϰ����
	MSG_LIVINGSKILL_LEARNDONE       = 4003,     // �ü�����ѧϰ
	MSG_LIVINGSKILL_NOMINING        = 4004,     // ��ɿ���
	MSG_LIVINGSKILL_NOFELLING       = 4005,     // �跥ľ����
	MSG_LIVINGSKILL_FISHING         = 4006,     // ����㼼��
	MSG_LIVINGSKILL_PLANTING        = 4007,     // ����ֲ����
	MSG_LIVINGSKILL_HERB            = 4008,     // ���ҩ����
	MSG_LIVINGSKILL_SHIKAR          = 4009,     // �����Լ���
	MSG_LIVINGSKILL_NOMININGLOW     = 4010,     // �ɿ��ܵȼ�̫��
	MSG_LIVINGSKILL_NOFELLINGLOW    = 4011,     // ��ľ���ܵȼ�̫��
	MSG_LIVINGSKILL_FISHINGLOW      = 4012,     // ���㼼�ܵȼ�̫��
	MSG_LIVINGSKILL_PLANTINGLOW     = 4013,     // ��ֲ���ܵȼ�̫��
	MSG_LIVINGSKILL_HERBLOW         = 4014,     // ��ҩ���ܵȼ�̫��
	MSG_LIVINGSKILL_SHIKARLOW       = 4015,     // ���Լ��ܵȼ�̫��
	MSG_LIVINGSKILL_PRO             = 4016,     // ר������������
	MSG_LIVINGSKILL_USESKILL        = 4017,     // ʹ�ü��ܻ��������%d
	MSG_LIVINGSKILL_USEPRODUCE      = 4018,     // ������������%d

	MSG_PRESCRIPTION_DATAERROR      = 4100,     // �䷽���ݳ���
	MSG_PRESCRIPTION_MATERIALERROR  = 4101,     // �������ݳ���
	MSG_PRESCRIPTION_NOPRODUCE      = 4102,     // �˵ز��ܺϳ�
	MSG_PRESCRIPTION_ERROR          = 4103,     // �ϳ�ʧ��
	//=================4800-4899    ��Ʒ�ϳ���ֽ�==================================
	MSG_ITEMSPLIT_NOSHORTCUT        = 4800,     // ��ЧĿ���λ
	MSG_ITEMSPLIT_NOSPLIT           = 4801,     // ������ʰȡ����Ŀ���λ�Ĳ���
	MSG_ITEMSPLIT_ERROR             = 4802,     // �ֽ�ʧ��
	MSG_ITEMSPLIT_INDEXERROR        = 4803,     // ��Ч������λ
	MSG_ITEMSPLIT_ISLOCK            = 4804,     // �ֽ���Ʒ.��������
	MSG_ITEMCOMPOSE_LOCK            = 4820,     // �ϳ���Ʒ,��������
	MSG_ITEMCOMPOSE_NOPICKUP        = 4821,     // ������ʰȡ�ϳɲ���  
	MSG_ITEMCOMPOSE_ERROR           = 4822,     // �ϳ�ʧ��
	MSG_ITEMCOMPOSE_NOTHING         = 4823,     // ����δ�����κβ���

	// ================4900-4999	ʦͽϵͳ��Ϣ  ==================================
	MSG_MPMANAGER_LEVEL             = 4900,     // ���ʸ�������%s
	MSG_MPMANAGER_ADD_MASTEREXP     = 4901,     // ʦ������%d��
	MSG_MPMANAGER_DEC_MASTEREXP     = 4902,     // ʦ������%d��
	MSG_MPMANAGER_ADD_EXPPO0L       = 4903,     // �����ѹ�����%d�㾭��
	MSG_MPMANAGER_DEC_EXPPO0L       = 4904,     // ���Ի���%d�㾭��
	MSG_MPMANAGER_MASTEREXP_ERROR   = 4905,     // �ɶԻ����鲻��
	MSG_MPMANAGER_EXPPOOL_ERROR     = 4906,     // ʦ�²���
	MSG_MPMANAGER_PRENTICE_STATE    = 4907,     // ��ǰͽ��״̬
	MSG_MPMANAGER_PRENTICE_FULL     = 4908,     // ��ǰͽ������
	MSG_MPMANAGER_LEVEL_ZERO        = 4909,     // δ����ʸ�
	MSG_MPMANAGER_FRIEND_ERROR      = 4910,     // ��δ��Է����ѹ�ϵ
	MSG_MPMANAGER_LEAVEMASTER       = 4911,     // ���ѳ�ʦ������ӵ�а�ʦ�ʸ�
	MSG_MPMANAGER_NOTEAM            = 4912,     // ����δ���
	MSG_MPMANAGER_ERRORTEAM         = 4913,     // �������������
	MSG_MPMANAGER_DESTPLAEYR_ERROR  = 4914,     // ��ЧĿ�����
	MSG_MPMANAGER_DATAERROR         = 4915,     // ʦͽ���ݳ���
	MSG_MPMANAGER_DEST_LEVELZERO    = 4916,     // �Է�δ����ʸ�

	// ================5000-5999	����ϵͳ��Ϣ  ==================================
	MSG_PET_ITEM_NOT_GENGUDAN		= 5000,		// ���߲��Ǹ��ǵ�
	MSG_PET_STATUS_NOT_IDLE			= 5001,		// �����޲����ڴ���״̬
	MSG_PET_MAXIMUM_INSIGHT			= 5002,		// �����������Ѵﵽ���
	MSG_PET_GENGUDAN_LEVEL			= 5003,		// �������������ǵ��ȼ���ƥ��
	MSG_PET_LOW_HAPPINESS			= 5004,		// �����޵Ŀ��ֶȲ���
	MSG_PET_TALENT_ZERO				= 5005,		// �����޵ĸ���Ϊ��
	MSG_PET_NOT_EXIST				= 5006,		// ���޲�����
	MSG_NPC_CAN_NOT_TAME			= 5007,		// ��Npc���ܽ��в���
	MSG_PET_HIGH_LEVEL				= 5008,		// �����޵ĵȼ��ϸ�
	MSG_PET_LOW_HP					= 5009,		// �����޵������ϵ�
	MSG_PET_DATA_NOT_EXIST			= 5010,		// �������ݴ���
	MSG_PET_INFO_SLOT_FULL			= 5011,		// ���޲�����
	MSG_PET_ITEM_NOT_HUANTONG		= 5012,		// ���߲��ǻ�ͯ��
	MSG_PET_ITEM_NO_HUANTONGDAN		= 5013,		// û��ѡ��ͯ��
	MSG_PET_LOW_LEVEL				= 5014,		// �����޵ĵȼ�����
	MSG_PET_STATUS_NOT_COMBAT		= 5015,		// �����޴���ս��״̬
	MSG_PET_HAD_BEEN_IDENTIFIED		= 5016,		// �������ѱ�����
	MSG_PET_LIANHUA_LEVEL			= 5017,		// ���ޱ��뵽10����������
	MSG_MOUNT_NOT_EXIST				= 5101,		// ��費����
	MSG_MOUNT_DATA_NOT_EXIST		= 5102,		// ������ݴ���
	MSG_MOUNT_STATUS_NOT_IDLE		= 5103,		// ����費���ڴ���״̬
	MSG_MOUNT_INFO_SLOT_FULL		= 5104,		// ��������
	MSG_MOUNT_STATUS_IDLE			= 5105,		// ����账�ڴ���״̬
	MSG_PET_CANNOT_FEED				= 5106,		// �����޲���ιʳ
	MSG_PET_CANNOT_TAME				= 5107,		// �����޲���ѱ��
	MSG_PET_CANNOT_MEDICAL			= 5108,		// �����޲��ܲ���
	MSG_PET_FOUNDNO_FOOD			= 5109,		// û�к��ʵ�����ʳ��
	MSG_PET_FOUNDNO_MEDICAL			= 5110,		// û�к��ʵ����޵�ҩ
	MSG_PET_FOUNDNO_TOY				= 5111,		// û�к��ʵ��������
	MSG_PET_NAME_TOO_LONG			= 5112,		// �������ֵĳ���̫��
	MSG_PET_N0_GENGUDAN				= 5113,		// û�к��ʵĸ��ǵ�

	// ================6000-6999	Ԫ��ϵͳ��Ϣ  ==================================
	MSG_SPIRIT_SLOT_INVALID			= 6000,		// "��Ч��Ԫ����λ"
	MSG_SPIRIT_SLOT_OPENED			= 6001,		// "��Ԫ�����ѿ���"
	MSG_SPIRIT_SLOT_NOT_OPENED		= 6002,		// "��Ԫ����δ����"
	MSG_SPIRIT_FULL_SLOT			= 6003,		// "û�п��õ�Ԫ����λ"
	MSG_SPIRIT_NOT_EXIST			= 6004,		// "����û��Ԫ��"
	MSG_SPIRIT_BE_ACTIVE			= 6005,		// "��Ԫ���ڼ���״̬"
	MSG_SPIRIT_BE_INACTIVE			= 6006,		// "��Ԫ����δ����״̬"
	MSG_SPIRIT_ONLY_ONE				= 6007,		// "ֻʣһ��Ԫ��"
	MSG_SPIRIT_SKILL_NOT_OPENED		= 6008,		// �˾�����û�н��
	MSG_SPIRIT_SKILL_OPENED			= 6009,		// �˾������Ѿ����
	MSG_SPIRIT_SKILL_NOT_EXIST		= 6010,		// �˾�����û�о���
	MSG_SPIRIT_SKILL_ALREADY_EXIST	= 6011,		// �˾��������о���
	MSG_SPIRIT_SLOT_ACTIVE_NONE		= 6012,		// û��Ԫ�񱻼���
	MSG_SPIRIT_NONE					= 6013,		// û��Ԫ��
	MSG_SPIRIT_NO_STONE1			= 6014,		// û�з���Ԫ��ʯ
	MSG_SPIRIT_NO_STONE2			= 6015,		// û�з���Ԫ����
	MSG_SPIRIT_NO_SKILLBOOK			= 6016,		// û�з��뼼����
	MSG_SPIRIT_TALENTSLOT_INVALID	= 6017,		// ��Ч��Ԫ���츳λ��
	MSG_SPIRIT_TALENTLEVEL_LIMIT	= 6018,		// Ԫ���츳�ȼ��ѵ����
	MSG_SPIRIT_TALENT_LEFTPTS_ZERO	= 6019,		// �츳ʣ�����Ϊ0
	MSG_SPIRIT_BE_HUANHUA			= 6020,		// Ԫ���ڻû�״̬
	MSG_SPIRIT_BE_NOTHUANHUA		= 6021,		// Ԫ����δ�û�״̬

	// ================8000-9998	����ϵͳ��Ϣ  ==================================
	MSG_INPUTNUMBERERROR			= 8000,		// ��������ȷ����Ŀ
	MSG_INVALIDPARAM				= 8100,		// ��Ч�Ĳ���
	MSG_CALLBOARDDAILYSTART         = 8101,     // �ճ�����%s�ѿ���
	MSG_COLLECTION_COND_INVALID		= 8102,		// ������ɼ�����

	MSG_WAITING_CONFIRM				= 9998,		// �ȴ��ͻ���ȷ�ϲ���
	MSG_UNKOWNERROR					= 9999,		// δ֪����
};

#endif

#endif//__BUILDPREFIX_H__