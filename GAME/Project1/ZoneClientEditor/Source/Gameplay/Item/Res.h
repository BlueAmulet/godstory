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
		CATEGORY_WEAPON					= 1,			//1����װ��
		CATEGORY_EQUIPMENT				= 2,			//2����װ��
		CATEGORY_ORNAMENT				= 3,			//3��Ʒ
		CATEGORY_PET					= 4,			//����
		CATEGORY_GOODS					= 5,			//��Ʒ
		CATEGORY_MATERIAL				= 6,			//����
		CATEGORY_TOOLS					= 7,			//����
		CATEGORY_MISSION				= 8,			//����
		CATEGORY_DRAWING				= 9,			//�䷽
		CATEGORY_GEM					= 10,			//��ʯ����
		CATEGORY_MAGIC					= 11,			//��ħ����
		CATEGORY_PACK					= 12,			//����
		CATEGORY_SCROLL					= 13,			//����
		CATEGORY_OTHER					= 14,			//����
		CATEGORY_PETEQUIP				= 15,			//����װ��
		CATEGORY_ONADD					= 16,			//���Ӳ���
		CATEGORY_TRUMP					= 17,			//����
		CATEGORY_PETEGG					= 18,			//�����ӡ
		CATEGORY_MOUNTPETEGG			= 19,			//����ӡ

		CATEGORY_FAQI					= SUB(CATEGORY_WEAPON, 1),		//����
		CATEGORY_SIGLESHORT				= SUB(CATEGORY_WEAPON, 2),		//����
		CATEGORY_DOUBLESHORT			= SUB(CATEGORY_WEAPON, 3),		//˫��
		CATEGORY_QIN					= SUB(CATEGORY_WEAPON, 4),		//��
		CATEGORY_BOW					= SUB(CATEGORY_WEAPON, 5),		//��
		CATEGORY_FLAGSTAFF				= SUB(CATEGORY_WEAPON, 6),		//���� 
		CATEGORY_ARMS					= SUB(CATEGORY_WEAPON, 7),		//����

		CATEGORY_HEAD					= SUB(CATEGORY_EQUIPMENT, 1),	//ͷ��
		CATEGORY_BODY					= SUB(CATEGORY_EQUIPMENT, 2),	//����
		CATEGORY_BACK					= SUB(CATEGORY_EQUIPMENT, 3),	//����
		CATEGORY_SHOULDER				= SUB(CATEGORY_EQUIPMENT, 4),	//�粿
		CATEGORY_HAND					= SUB(CATEGORY_EQUIPMENT, 5),	//�ֲ�
		CATEGORY_WAIST					= SUB(CATEGORY_EQUIPMENT, 6),	//����
		CATEGORY_FOOT					= SUB(CATEGORY_EQUIPMENT, 7),	//�Ų�
		CATEGORY_FASHION_FAMILY			= SUB(CATEGORY_EQUIPMENT, 8),	//����ʱװ
		CATEGORY_FASHION_HEAD			= SUB(CATEGORY_EQUIPMENT, 9),	//ͷ��ʱװ
		CATEGORY_FASHION_BODY			= SUB(CATEGORY_EQUIPMENT, 10),	//����ʱװ
		CATEGORY_FASHION_BACK			= SUB(CATEGORY_EQUIPMENT, 11),	//����ʱװ

		CATEGORY_AMULET					= SUB(CATEGORY_ORNAMENT, 1),	//�����
		CATEGORY_NECK					= SUB(CATEGORY_ORNAMENT, 2),	//����
		CATEGORY_RING					= SUB(CATEGORY_ORNAMENT, 3),	//��ָ

		CATEGORY_RIDE					= SUB(CATEGORY_PET, 1),			//����
		CATEGORY_SUB_PET				= SUB(CATEGORY_PET, 2),			//����
		CATEGORY_SUMMONER				= SUB(CATEGORY_PET, 3),			//�ٻ�

		CATEGORY_FOOD					= SUB(CATEGORY_GOODS, 1),		//ʳ��
		CATEGORY_MEDICA					= SUB(CATEGORY_GOODS, 2),		//ҩƷ
		CATEGORY_TONIC					= SUB(CATEGORY_GOODS, 3),		//��Ʒ
		CATEGORY_SWATHE					= SUB(CATEGORY_GOODS, 4),		//����
		CATEGORY_SKILLMATERIAL			= SUB(CATEGORY_GOODS, 5),		//���ܲ���
		CATEGORY_PLAYNATERIAL			= SUB(CATEGORY_GOODS, 6),		//ʩ������
		CATEGORY_BOOK					= SUB(CATEGORY_GOODS, 7),		//�鼮
		CATEGORY_SKILLBOOK				= SUB(CATEGORY_GOODS, 8),		//������
		CATEGORY_TREASURE				= SUB(CATEGORY_GOODS, 9),		//����
		CATEGORY_SUBGOODS				= SUB(CATEGORY_GOODS, 10),		//��Ʒ
		CATEGORY_PETFOOD				= SUB(CATEGORY_GOODS, 11),		//����ʳ��
		CATEGORY_PETMEDICA				= SUB(CATEGORY_GOODS, 12),		//���ﵤҩ
		CATEGORY_PETTOY					= SUB(CATEGORY_GOODS, 13),		//�������
		CATEGORY_SPIRITSTONE1			= SUB(CATEGORY_GOODS, 14),		//Ԫ��ʯ�����ڿ���Ԫ���������
		CATEGORY_SPIRITSTONE2			= SUB(CATEGORY_GOODS, 15),		//Ԫ�����������Ԫ����У�

		CATEGORY_HERBAL_MEDICINE		= SUB(CATEGORY_MATERIAL, 1),	//��ҩ
		CATEGORY_MINERAL				= SUB(CATEGORY_MATERIAL, 2),	//����	
		CATEGORY_CROP					= SUB(CATEGORY_MATERIAL, 3),	//ũ����
		CATEGORY_WOOD					= SUB(CATEGORY_MATERIAL, 4),	//ľ��
		CATEGORY_FISH					= SUB(CATEGORY_MATERIAL, 5),	//����
		CATEGORY_MEAT					= SUB(CATEGORY_MATERIAL, 6),	//����
		CATEGORY_SUNDRIES				= SUB(CATEGORY_MATERIAL, 7),	//����
		CATEGORY_SKIN					= SUB(CATEGORY_MATERIAL, 8),	//Ƥ��
		CATEGORY_MAGICMATERIAL			= SUB(CATEGORY_MATERIAL, 9),	//ħ������
		CATEGORY_PAPER					= SUB(CATEGORY_MATERIAL, 10),	//ֽ

		CATEGORY_GATHER_TOOL			= SUB(CATEGORY_TOOLS, 1),		//�ɼ�����
		CATEGORY_MINERAL_TOOL			= SUB(CATEGORY_TOOLS, 2),		//�ڿ󹤾�
		CATEGORY_CULTURE_TOOL			= SUB(CATEGORY_TOOLS, 3),		//���ֹ���
		CATEGORY_CUTTING_TOOL			= SUB(CATEGORY_TOOLS, 4),		//��ľ����
		CATEGORY_FISH_TOOL				= SUB(CATEGORY_TOOLS, 5),		//���㹤��
		CATEGORY_HUNT_TOOL				= SUB(CATEGORY_TOOLS, 6),		//���Թ���
		CATEGORY_PET_TOOL				= SUB(CATEGORY_TOOLS, 7),		//ץ�蹤��
		CATEGORY_MAGIC_BOTTLE			= SUB(CATEGORY_TOOLS, 8),		//ĥƿ
		CATEGORY_FOUND_TOOL				= SUB(CATEGORY_TOOLS, 9),		//���칤��
		CATEGORY_SEWING_TOOL			= SUB(CATEGORY_TOOLS, 10),		//�÷칤��
		CATEGORY_ART_TOOL				= SUB(CATEGORY_TOOLS, 11),		//���չ���
		CATEGORY_CHARM_TOOL				= SUB(CATEGORY_TOOLS, 12),		//���乤��
		CATEGORY_SMELT_TOOL				= SUB(CATEGORY_TOOLS, 13),		//��������
		CATEGORY_COOK_TOOL				= SUB(CATEGORY_TOOLS, 14),		//��⿹���
		CATEGORY_ARTISAN_TOOL			= SUB(CATEGORY_TOOLS, 15),		//��������
		CATEGORY_DELUDE_TOOL			= SUB(CATEGORY_TOOLS, 16),		//��������

		CATEGORY_CANUSE					= SUB(CATEGORY_MISSION, 1),		//���񣨴�����
		CATEGORY_CANOTUSE				= SUB(CATEGORY_MISSION, 2),		//������Ʒ

		CATEGORY_FOUND					= SUB(CATEGORY_DRAWING, 1),		//����
		CATEGORY_SEWING					= SUB(CATEGORY_DRAWING, 2),		//�÷�
		CATEGORY_ART					= SUB(CATEGORY_DRAWING, 3),		//����
		CATEGORY_CHARM					= SUB(CATEGORY_DRAWING, 4),		//����
		CATEGORY_SMELT					= SUB(CATEGORY_DRAWING, 5),		//����
		CATEGORY_COOKING				= SUB(CATEGORY_DRAWING, 6),		//���
		CATEGORY_ARTISAN				= SUB(CATEGORY_DRAWING, 7),		//����
		CATEGORY_DELUDE					= SUB(CATEGORY_DRAWING, 8),		//����
		CATEGORY_DRAWING_OTHER			= SUB(CATEGORY_DRAWING, 9),		//����		

		CATEGORY_GEM_COLOR1				= SUB(CATEGORY_GEM, 1),			//�챦ʯ
		CATEGORY_GEM_COLOR2				= SUB(CATEGORY_GEM, 2),			//�̱�ʯ
		CATEGORY_GEM_COLOR3				= SUB(CATEGORY_GEM, 3),			//����ʯ
		CATEGORY_GEM_COLOR4				= SUB(CATEGORY_GEM, 4),			//�Ʊ�ʯ
		CATEGORY_GEM_COLOR5				= SUB(CATEGORY_GEM, 5),			//�౦ʯ
		CATEGORY_GEM_COLOR6				= SUB(CATEGORY_GEM, 6),			//�ϱ�ʯ
		CATEGORY_GEM_COLOR7				= SUB(CATEGORY_GEM, 7),			//���ʯ
		CATEGORY_GEM_COLOR8				= SUB(CATEGORY_GEM, 8),			//�ƾ�ʯ
		CATEGORY_GEM_COLOR9				= SUB(CATEGORY_GEM, 9),			//�ྦྷʯ
		CATEGORY_GEM_COLOR10			= SUB(CATEGORY_GEM, 10),		//�Ͼ�ʯ
		CATEGORY_GEM_COLOR11			= SUB(CATEGORY_GEM, 11),		//
		CATEGORY_GEM_COLOR12			= SUB(CATEGORY_GEM, 12),		//
		CATEGORY_GEM_COLOR13			= SUB(CATEGORY_GEM, 13),		//
		CATEGORY_GEM_COLOR14			= SUB(CATEGORY_GEM, 14),		//
		CATEGORY_GEM_COLOR15			= SUB(CATEGORY_GEM, 15),		//

		CATEGORY_UP_MAGIC_WEAPON		= SUB(CATEGORY_MAGIC, 1),		//������ħ����
		CATEGORY_UP_MAGIC_DEFEND		= SUB(CATEGORY_MAGIC, 2),		//���߸�ħ����
		CATEGORY_UP_MAGIC_ACCOUNTER		= SUB(CATEGORY_MAGIC, 3),		//��Ʒ��ħ����
		CATEGORY_UP_MAGIC_ARMET			= SUB(CATEGORY_MAGIC, 4),		//ͷ����ħ����
		CATEGORY_UP_MAGIC_SHOULDER		= SUB(CATEGORY_MAGIC, 5),		//���ħ����
		CATEGORY_UP_MAGIC_MANTLE		= SUB(CATEGORY_MAGIC, 6),		//���總ħ����
		CATEGORY_UP_MAGIC_CLOTH			= SUB(CATEGORY_MAGIC, 7),		//�·���ħ����
		CATEGORY_UP_MAGIC_SHOSE			= SUB(CATEGORY_MAGIC, 8),		//Ь�Ӹ�ħ����
		CATEGORY_UP_MAGIC_ARMGUARD		= SUB(CATEGORY_MAGIC, 9),		//���ָ�ħ����
		CATEGORY_UP_MAGIC_CAESTUS		= SUB(CATEGORY_MAGIC, 10),		//������ħ����
		CATEGORY_UP_MAGIC_NECKLACE		= SUB(CATEGORY_MAGIC, 11),		//������ħ����
		CATEGORY_UP_MAGIC_RING			= SUB(CATEGORY_MAGIC, 12),		//��ָ��ħ����
		CATEGORY_UP_MAGIC_AMULET		= SUB(CATEGORY_MAGIC, 13),		//����ħ����
		CATEGORY_UP_MAGIC_ALL			= SUB(CATEGORY_MAGIC, 14),		//ȫ����ħ����
		CATEGORY_UP_MAGIC_TRUMP			= SUB(CATEGORY_MAGIC, 15),		//������ħ����

		CATEGORY_INVE_FOREVER_PACK		= SUB(CATEGORY_PACK, 1),		//��Ʒ�����ð���
		CATEGORY_BANK_FOREVER_PACK		= SUB(CATEGORY_PACK, 2),		//�ֿ������ð���
		CATEGORY_STAL_FOREVER_PACK		= SUB(CATEGORY_PACK, 3),		//��̯�����ð���
		CATEGORY_INVE_TIME_PACK			= SUB(CATEGORY_PACK, 4),		//��Ʒ��ʱЧ����
		CATEGORY_BANK_TIME_PACK			= SUB(CATEGORY_PACK, 5),		//�ֿ���ʱЧ����
		CATEGORY_STAL_TIME_PACK			= SUB(CATEGORY_PACK, 6),		//��̯��ʱЧ����
		CATEGORY_STAL_STYLE_PACK		= SUB(CATEGORY_PACK, 7),		//��̯����ʽ����

		CATEGORY_BACK_CITY				= SUB(CATEGORY_SCROLL, 1),		//�سǷ�
		CATEGORY_RECORD_SCROLL			= SUB(CATEGORY_SCROLL, 2),		//��¼����
		CATEGORY_PICTURE_CHECK			= SUB(CATEGORY_SCROLL, 3),		//ͼ��
		CATEGORY_SUB_SCROLL				= SUB(CATEGORY_SCROLL, 4),		//����
		CATEGORY_IDENTIFY				= SUB(CATEGORY_SCROLL, 5),		//������
		CATEGORY_BECOMEBABY				= SUB(CATEGORY_SCROLL, 6),		//��ͯ��

		CATEGORY_SMOKE_FLOVER			= SUB(CATEGORY_OTHER, 1),		//�̻�
		CATEGORY_DOWRY					= SUB(CATEGORY_OTHER, 2),		//��ױ
		CATEGORY_PHYSICS_EQUIPMET		= SUB(CATEGORY_OTHER, 3),		//Ԫ��װ��
		CATEGORY_CHIP					= SUB(CATEGORY_OTHER, 4),		//����
		CATEGORY_KEY					= SUB(CATEGORY_OTHER, 5),		//Կ��
		CATEGORY_CHANGED_CARD			= SUB(CATEGORY_OTHER, 6),		//����

		CATEGORY_PETEQUIP_HEAD			= SUB(CATEGORY_PETEQUIP, 1),	//����ͷ
		CATEGORY_PETEQUIP_BODY			= SUB(CATEGORY_PETEQUIP, 2),	//��������
		CATEGORY_PETEQUIP_BACK			= SUB(CATEGORY_PETEQUIP, 3),	//���ﱳ
		CATEGORY_PETEQUIP_WEAPON		= SUB(CATEGORY_PETEQUIP, 4),	//��������

		CATEGORY_STRENTHEN_BASE			= SUB(CATEGORY_ONADD, 1),		//ǿ����������	
		CATEGORY_STRENTHEN_ADD			= SUB(CATEGORY_ONADD, 2),		//ǿ�����Ӳ���
		CATEGORY_PUNCHHOLE				= SUB(CATEGORY_ONADD, 3),		//��ײ���
		CATEGORY_GET_TREASURE			= SUB(CATEGORY_ONADD, 4),		//ժȡ����
		CATEGORY_ITEM_IDENTIFY			= SUB(CATEGORY_ONADD, 5),		//��������
		CATEGORY_ITEM_SOULLINK			= SUB(CATEGORY_ONADD, 6),		//������Ӳ���
		CATEGORY_ITEM_GENGUDAN			= SUB(CATEGORY_ONADD, 7),		//���ǵ�
		GATEGORY_TRUMP_RECAST			= SUB(CATEGORY_ONADD, 8),		//������������
		GATEGORY_TRUMP_COMPOSE			= SUB(CATEGORY_ONADD, 9),		//�����ϳɲ���
		GATEGORY_TRUMP_STRENTHEN		= SUB(CATEGORY_ONADD, 10),		//����ǿ������

		CATEGORY_PETEGG_NORMAL			= SUB(CATEGORY_PETEGG, 1),		//���ޱ�����ӡ	
		CATEGORY_PETEGG_VARIATION		= SUB(CATEGORY_PETEGG, 2),		//�������ޱ�����ӡ
		CATEGORY_PETEGG_GROWN_NORMAL	= SUB(CATEGORY_PETEGG, 3),		//���޷�ӡ
		CATEGORY_PETEGG_GROWN_VARIATION	= SUB(CATEGORY_PETEGG, 4),		//�������޷�ӡ

		CATEGORY_MOUNTPETEGG_NORMAL		= SUB(CATEGORY_MOUNTPETEGG, 1),	//��ͨ����ӡ
	};

	// ������������������
	enum PropertyType
	{
		PROTYPE_RANDALL		= 1,		//������������
		PROTYPE_RANDNAME	= 2,		//����ǰ׺����
		PROTYPE_STRENGTHEN	= 3,		//װ��ǿ������
		PROTYPE_IMPRESS		= 4,		//��������
		PROTYPE_SOULBIND	= 5,		//��������
		PROTYPE_WUXING		= 6,		//��������
		PROTYPE_BESET		= 7,		//��ʯ��Ƕ����
		PROTYPE_GODHAND		= 8,		//��������
	};

	// �����Ա�����
	enum SexLimit
	{											
		SEXLIMIT_BOTH	= 0,			//��Ů����
		SEXLIMIT_MALE	= 1,			//����ʹ��
		SEXLIMIT_FEMALE	= 2,			//Ů��ʹ��
	};

	//��Ʒװ��ʱ�Ľ�����λ
	enum MountType
	{							
		MOUNTTYPE_NONE	= 1,			//��װ��
		MOUNTTYPE_LINK	= 2,			//����
		MOUNTTYPE_CHANGE= 3,			//�滻	
	};

	// װ���
	enum MountNode
	{
		MOUNTNODE_NONE	= 0,			//��
		MOUNTNODE_BODY	= 1,			//����
	};

	// ʱЧ���㷽ʽ
	enum TimeMode
	{
		TIMEMODE_NONE,					// ������ʱҪ��
		TIMEMODE_DOWNONLY,				// �����߿�ʱ
		TIMEMODE_ONLINEGET,				// ��ȡ�����߿�ʱ
		TIMEMODE_ONLINEUSE,				// ʹ�ú����߿�ʱ
		TIMEMODE_ALLGET,				// ��ȡ�������߶���ʱ
		TIMEMODE_ALL,					// ʹ�ú��������߶���ʱ
	};

	// ��Ʒ������
	enum BindMode
	{
		BINDMODE_PICKUP	= 1,					// ʰȡ��
		BINDMODE_EQUIP	= 2,					// װ����
		BINDMODE_NONE	= 3,					// ����
	};

	// ��������
	enum FamilyType
	{
		FAMILYTYPE_NONE		= BIT(Family_None),		// ������
		FAMLIYTYPE_SHENG	= BIT(Family_Sheng),	// ʥ
		FAMLIYTYPE_FO		= BIT(Family_Fo),		// ��
		FAMLIYTYPE_XIAN		= BIT(Family_Xian),		// ��
		FAMLIYTYPE_JING		= BIT(Family_Jing),		// ��
		FAMLIYTYPE_GUI		= BIT(Family_Gui),		// ��
		FAMILYTYPE_GUAI		= BIT(Family_Guai),		// ��
		FAMLIYTYPE_YAO		= BIT(Family_Yao),		// ��
		FAMLIYTYPE_MO		= BIT(Family_Mo),		// ħ

		FAMILYTYPE_COMMON	= FAMLIYTYPE_SHENG | FAMLIYTYPE_XIAN | FAMILYTYPE_GUAI |
		FAMLIYTYPE_GUI | FAMLIYTYPE_JING | 

		FAMLIYTYPE_YAO | FAMLIYTYPE_FO | FAMLIYTYPE_MO,
	};

	// �������Ʊ�־
	enum ItemFlag
	{
		ITEM_TRADE		=  1 << 0,		// ����ҽ���
		ITEM_SELLNPC	=  1 << 1,		// ���۸�NPC
		ITEM_SAVEBANK	=  1 << 2,		// ��ֿ�
		ITEM_FIX		=  1 << 3,		// ������
		ITEM_DROPEQUIP	=  1 << 4,		// жװ����ʧ(Ŀǰֻ�����������)
		ITEM_DOWNLINE	=  1 << 5,		// ������ʧ
		ITEM_CANUSE		=  1 << 6,		// ��ʹ��
		ITEM_SMELT		=  1 << 7,		// ����
		ITEM_DROP		=  1 << 8,		// ����
		ITEM_USEINFIGHT =  1 << 9,		// ս����ʹ��
		ITEM_USEDESTROY =  1 << 10,		// ʹ�ú���ʧ
		ITEM_MISSION	=  1 << 11,		// ����ʹ��
		ITEM_UPGRADE	=  1 << 12,		// ����
		ITEM_ENERGY		=  1 << 13,		// ����
		ITEM_PICKUPONLY =  1 << 14,		// Ψһʰȡ
		ITEM_SHENQI		=  1 << 15,		// ����
		ITEM_SOULLINK	=  1 << 16,		// �����������
		ITEM_SEARCH		=  1 << 17,		// ��������
		ITEM_USESCRIPT	=  1 << 18,		// ����ִ��ʹ�ýű�
		ITEM_MOUNTSCRIPT=  1 << 19,		// ����ִ����װ�ű�
		ITEM_UNMOUNTSCRIPT = 1 << 20,	// ����ִ��жװ�ű�
		ITEM_MONEY		=  1 << 21,		// Ԫ������
	};

	// Ч������(APF)
	enum ActivePropertyFlag
	{
		APF_BIND			= EAPF_BIND,		// ��Ч��
		APF_ATTACH			= EAPF_ATTACH,		// ����Ч��
		APF_WUXING			= EAPF_WUXING,		// ����Ч��
		APF_IMPRESS			= EAPF_IMPRESS,		// װ����������
		APF_EQUIPSKILLA		= EAPF_EQUIPSKILLA,	// װ������������
		APF_EQUIPSKILLB		= EAPF_EQUIPSKILLB,	// װ���󱻶�����		
	};

	// ����ʱ�����
	// ע��ֻ��װ������漰[װ������]����������漰[�������]
	enum IdentifyType
	{
		IDENTIFYTYPE_NONE	= 1,				// �������
		IDENTIFYTYPE_DROP	= 2,				// ����ʱ����
		IDENTIFYTYPE_EQUIP	= 3,				// װ��ʱ����
		IDENTIFYTYPE_NPC	= 4,				// NPC�ֶ�����
	};

	//��Ʒ�һ�����
	enum SaleType
	{
		SALETYPE_MONEY		= 1,				// ��Ԫ
		SALETYPE_GOLD		= 2,				// ��Ԫ
		SALETYPE_HONOR		= 3,				// ս��
		SALETYPE_BINDMONEY	= 4,				// �󶨽�Ǯ
		SALETYPE_CREDIT		= 5,				// ����
		SALETYPE_BINDGOLD	= 6,				// ��ʯ
	};


	enum EquipPlaceFlag
	{
		EQUIPPLACEFLAG_UNKNOWN		= 0,			// δ֪����

		EQUIPPLACEFLAG_HEAD			= 1,			// ͷ��
		EQUIPPLACEFLAG_NECK			= 2,			// ����
		EQUIPPLACEFLAG_BACK			= 3,			// ����
		EQUIPPLACEFLAG_SHOULDER		= 4,			// �粿
		EQUIPPLACEFLAG_BODY			= 5,			// ����
		EQUIPPLACEFLAG_HAND			= 6,			// �ֲ�
		EQUIPPLACEFLAG_WAIST		= 7,			// ����
		EQUIPPLACEFLAG_FOOT			= 8,			// �Ų�
		EQUIPPLACEFLAG_RING			= 9,			// ��ָ
		EQUIPPLACEFLAG_AMULET		= 10,			// �����
		EQUIPPLACEFLAG_WEAPON		= 11,			// ����
		EQUIPPLACEFLAG_TRUMP		= 12,			// ����������
		EQUIPPLACEFLAG_GATHER		= 13,           // �ɼ�����
		EQUIPPLACEFLAG_RIDE			= 14,			// �������(��UI������)
		EQUIPPLACEFLAG_PET			= 15,			// �������(��UI������)
		EQUIPPLACEFLAG_FASHIONHEAD  = 16,           // ʱװͷ
		EQUIPPLACEFLAG_FASHIONBODY  = 17,           // ʱװ����
		EQUIPPLACEFLAG_FASHIONBACK  = 18,           // ʱװ����
		EQUIPPLACEFLAG_PACK1		= 19,			// ��չ����1
		EQUIPPLACEFLAG_PACK2		= 20,			// ��չ����2
		EQUIPPLACEFLAG_BANKPACK		= 21,			// ������չ��
		EQUIPPLACEFLAG_STALLPACK1	= 22,			// ��̯��չ��1
		EQUIPPLACEFLAG_STALLPACK2	= 23,			// ��̯��չ��2
		EQUIPPLACEFLAG_STALLPACK3	= 24,			// ��̯��ʽ��
		EQUIPPLACEFLAG_FAMLIYFASHION= 25,			// ����ʱװ
	};

	// ��Ʒ��ɫ�ȼ�
	enum ColorLevel
	{
		COLORLEVEL_UNKNOWN  = 0,					// ��
		COLORLEVEL_GREY		= 1,					// ��ɫ
		COLORLEVEL_WHITE	= 2,					// ��ɫ
		COLORLEVEL_GREEN	= 3,					// ��ɫ
		COLORLEVEL_BLUE		= 4,					// ��ɫ
		COLORLEVEL_PURPLE	= 5,					// ��ɫ
		COLORLEVEL_ORANGE	= 6,					// ��ɫ
		COLORLEVEL_RED		= 7,					// ��ɫ
		COLORLEVEL_MAX		= 8,					// ��
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
		REPAIR_NORMAL		= 0,					// ��ͨ����
		REPAIR_RECOVER		= 1,					// �ָ�����
		REPAIR_DURABLE		= 2,					// ��������
	};

	enum ActiveFlagType
	{
		ACTIVEF_NONE		= 0,					// ��
		ACTIVEF_ISEQUIP		= 1 << 0,				// ��װ����Ʒ(���и�������)
		ACTIVEF_TRADE		= 1 << 1,				// ���׺󼤻�
		ACTIVEF_RANDPRO		= 1 << 2,				// �Ƿ����ǿ���ȼ���������
	};

	//��ʯ����
	enum StoneLimit
	{
		Stone_None			= 0,					//������

		Stone_Faqi			= 1 << 0,				//����
		Stone_Sigleshort	= 1 << 1,
		Stone_Doubleshort	= 1 << 2,
		Stone_Arms			= 1 << 3,
		Stone_Qin			= 1 << 4,		
		Stone_Bow			= 1 << 5,
		Stone_Flagstaff		= 1 << 6,

		Stone_Trump			= 1 << 7,				// ����

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
	// ����ģ�����ݷ���
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
	// ��չ���ݷ���
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

	//��Ƕ�������غ���
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
	// ��������
	// ============================================================================
	inline char* GetColorText(char *buff, U32 buffSize);		//ͨ����ɫ�ȼ������ʾ��ɫ�ַ���

public:
	static U32 sColorValue[COLORLEVEL_MAX + 1];
	static StringTableEntry getCategoryName(U32 cat,U32 subCat);
protected:
	ItemBaseData*	mBaseData;			// ����ģ������
	stItemInfo*		mExtData;			// ��չ����
};

#include "Gameplay/item/Res_inline.h"

#endif//__RES_H__