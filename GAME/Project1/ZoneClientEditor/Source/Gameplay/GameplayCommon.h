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
//  ö�� & �ṹ
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
	SEX_BOTH	= 0,			//��Ҫ��
	SEX_MALE,					//����
	SEX_FEMALE,					//Ů��
};

// ְҵ��ע���뼼������һ��
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

// ���мӳ�
enum enWuXingMaster
{
	_Mu,
	_Huo,
	_Tu,
	_Jin,
	_Shui,
	MaxWuXingMasters,
};

// ����
enum enFame
{
	Fame_Bottom,				// �������������޷��ﵽ
	Fame_Enmity,				// ���
	Fame_Rivalry,				// �ж�
	Fame_Neutrality,			// ����
	Fame_Amity,					// �Ѻ�
	Fame_Honor,					// ��
	Fame_Adoration,				// ���
	Fame_Top,					// �������������޷��ﵽ

	MaxFame,
};

enum InfoTypes
{
	INFO_NONE			= 0,					//δ֪���¼�����
	INFO_SET_TARGET,							//����Ŀ��
	INFO_SET_TARGET_TOCLIENT,					//ǿ������Ŀ�굽�ͻ���
	INFO_SET_AUTOCAST,							//�����Զ�ʩ��
	INFO_BREAKCAST,								//���ʩ��
	INFO_QUEST_TRIGGER_NPC,						//����NPC
	INFO_QUEST_TRIGGER_OTHER,					//������ʽ�Ĵ�������
	INFO_SHORTCUT_EXCHANGE,						//������λ����
	INFO_SHORTCUT_MOVEITEMOBJECT,				//�ƶ���λ����

	INFO_ITEM_DROP,								//������Ʒ
	INFO_ITEM_USE,								//ʹ����Ʒ
	INFO_ITEM_SPLIT,							//�����Ʒ
	INFO_ITEM_COMPACT,							//������Ʒ
	INFO_ITEM_ADD,								//���ӻ��߿۳���Ʒ
	INFO_ITEM_BUY,								//������Ʒ
	INFO_ITEM_BATCHBUY,							//��������
	INFO_ITEM_REPAIR,							//������Ʒ
	INFO_ITEM_IDENTIFY,							//����
	INFO_ITEM_IMPRESS,							//����
	INFO_ITEM_STRENGTHEN,						//ǿ��
	INFO_ITEM_PUNCHHOLE,						//װ�����
	INFO_ITEM_MOUNTGEM,							//��Ƕ��ʯ
	INFO_ITEM_CLOSE_MOUNTGEM,					//�ر���Ƕ����
	INFO_ITEM_UNMOUNTGEM,						//ժȡ��ʯ
	INFO_ITEM_SOULLINK,							//�������
	INFO_ITEM_WUXINGLINK,						//��������

	INFO_NPCSHOP,								//��NPC����
	INFO_TRADE,									//��Ҽ佻��
	INFO_STALL,									//��ͨ��̯
	INFO_BANK,									//���н���
	INFO_MAIL,									//�ʼ�����
	INFO_CHANGEMAXSLOTS,						//�ı���λ����

	INFO_SWITCH,								//���ÿ���״̬
	INFO_RISE_REQUEST,							//����

	INFO_MISSION,								//������

	INFO_LEVEL_UP,								//����
	INFO_TRANSPORT,                             //����
	INFO_PET_OPERATE,							//����
	INFO_MOUNT_PET_OPERATE,						//���
	INFO_SPIRIT_OPERATE,						//Ԫ��
	INFO_SKILL_OPERATE,                         //���ܲ���
	INFO_COOLDOWN,								//��ȴʱ��
	INFO_LIVINGSKILL_LEARN,                     //�����ѧϰ
	INFO_PRESCRIPTION_PRODUCE,                  //����
	INFO_CANCELVOCAL,                           //ȡ������
	INFO_FAME,									//����
	INFO_FLY,									//����
	INFO_TALENT,								//�츳

	INFO_COLLECTION_ITEM,						//�ɼ�������Ϣ
	INFO_TIMER_TRIGGER,							//ʱ�䴥������Ϣ
	INFO_TIMER_TRIGGER_PET_STUDY,				//��������ʱ�䴥������Ϣ

	INFO_ENTER_TRANSPORT,						// ���봫��
	INFO_UPDATEOTHERFLAG,						//

	INFO_GETENCOURAGE,							// ��Ҳ���

	INFO_INITIALIZCOMPLETE,						// ��ɫ��ʼ������PACK�������


	INFO_QUICKCHANGE,							// �л�ʱװ
	INFO_TEMPLIST,								// �ı�TempList����
	INFO_CANCELBUFF,							// ����ȡ��buff

	INFO_MPMANAGER,                             // ����ʦͽ��ϵ
	INFO_MPMCHANGETEXT,                         // ʦ��������Ϣ
	INFO_ITEMSPLIT,                             // ��Ʒ�ֽ�
	INFO_ITEMCOMPOSE,                           // ��Ʒ�ϳ�

	MAIL_REQUEST,								// �����ʼ�����
	MAIL_SEND,									// �����ʼ�
	MAIL_READ,									// ��ȡ�ʼ�
	MAIL_DELETE,								// ɾ���ʼ�
	MAIL_DELETEALL,								// ɾ�������ʼ�
	MAIL_GETITEMS,								// ��ȡ����
	MAIL_GETMONEY,								// ��ȡ��Ǯ
	MAIL_SEND_OK,								// �����ʼ��ɹ�

	ZONE_CHAT,									// ��Ҹ�������
	FRIEND_REQUEST,								// �����Ϊ����

	INFO_SUPERMARKET,							// �̳�

	INFO_HELPDIRECT,                            // �������ָ��
};

enum
{
	COLOR_WHITE			= 0,					// ��ɫ
	COLOR_BLACK			= 1,					// ��ɫ
	COLOR_RED			= 2, 					// ��ɫ
	COLOR_GREEN			= 3,					// ��ɫ
	COLOR_BLUE			= 4,					// ��ɫ
	COLOR_YELLOW		= 5,					// ��ɫ
	COLOR_PURPLE		= 6,					// ��ɫ
	COLOR_CYAN			= 7,					// ��ɫ
	COLOR_GREY			= 8,					// ��ɫ
	COLOR_DEEPRED		= 9,					// ����
	COLOR_DEEPGREEN		= 10,					// ����
	COLOR_DEEPBLUE		= 11,					// ����
	COLOR_DEEPYELLOW	= 12,					// ����
	COLOR_DEEPPURPLE	= 13,					// ����
	COLOR_DEEPCYAN		= 14,					// ����
	COLOR_FUCHSIN		= 15,					// Ʒ��ɫ
	COLOR_SHINEGREEN	= 16,					// ����ɫ
	COLOR_LAKEBLUE		= 17,					// ����ɫ
	COLOR_ORANGE		= 18,					// �Ȼ�ɫ
	COLOR_BROWNPURPLE	= 19,					// ����ɫ
	COLOR_NEWGREEN		= 20,					// ����ɫ
	COLOR_UNKNOWN		= 24,					// �û��Զ�����ɫ
	COLOR_NOTIFY		= COLOR_WHITE,			// ϵͳ��ʾ��ɫ����ɫ��
	COLOR_WARN			= COLOR_YELLOW,			// ϵͳ������ɫ����ɫ��
	COLOR_DANGER		= COLOR_RED,			// ϵͳ������ɫ����ɫ��

	FONT_SONG			= 0,					// ����
	FONT_HEI			= 1,					// ����
	FONT_KAI			= 2,					// ����
	FONT_LI				= 3,					// ����
	FONT_YH				= 4,					// ΢���ź�

	SHOWTYPE_NOTIFY		= 0,					// ϵͳ��ʾ
	SHOWTYPE_WARN		= 1,					// ϵͳ����
	SHOWTYPE_ERROR		= 2,					// ϵͳ����

	SHOWPOS_SCREEN		= 0,					// ��Ļ����
	SHOWPOS_CHAT		= 1,					// �����
	SHOWPOS_SCRANDCHAT	= 2,					// ��Ļ�����������

	SHOWLIMIT_TIME		= 0,					// ��ʱ
	SHOWLIMIT_TIMES		= 1,					// �޴�
};



// ========================================================================================================================================
//  ȫ�ֳ��� & ����
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
//  ȫ�ֺ���
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