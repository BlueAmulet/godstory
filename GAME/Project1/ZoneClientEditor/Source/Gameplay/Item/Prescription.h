//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _Prescription_H_
#define _Prescription_H_
#include <hash_map>
#define SUB(a,b) a * 100 + b
#define GETSUB(a) a%100
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PrescriptionData
{
public:
	enum Constants
	{
		WEAPON     = 1,             // ����
		EQUIPMENT  = 2,             // װ��
		ORNAMENT   = 3,             // ��Ʒ 
		CHARM      = 4,             // ����
		ALCHEMY    = 5,             // ��ҩ
		FOOD       = 6,             // ʳ��
		ARTISAM    = 7,             // ����
		POISON     = 8,             // ����
		

		WEAPON_FAQI                   = SUB(WEAPON, 1),        //����
		WEAPON_SIGLESHORT             = SUB(WEAPON, 2),        //����
		WEAPON_DOUBLESHORT            = SUB(WEAPON, 3),        //˫��
		WEAPON_QIN                    = SUB(WEAPON, 4),        //��
		WEAPON_BOW                    = SUB(WEAPON, 5),        //��
		WEAPON_FLAGSTAFF              = SUB(WEAPON, 6),        //����
		WEAPON_ARMS                   = SUB(WEAPON, 7),        //����
		WEAPON_NEARMACHINING_LEVEL1   = SUB(WEAPON, 8),        //���̲��ϼӹ�    
		WEAPON_NEARMACHINING_LEVEL2   = SUB(WEAPON, 9),
		WEAPON_NEARMACHINING_LEVEL3   = SUB(WEAPON, 10),
		WEAPON_NEARMACHINING_LEVEL4   = SUB(WEAPON, 11),
		WEAPON_NEARMACHINING_LEVEL5   = SUB(WEAPON, 12),
		WEAPON_FARMACHINING_LEVEL1    = SUB(WEAPON, 13),       //Զ�̲��ϼӹ�
		WEAPON_FARMACHINING_LEVEL2    = SUB(WEAPON, 14),
		WEAPON_FARMACHINING_LEVEL3    = SUB(WEAPON, 15),
		WEAPON_FARMACHINING_LEVEL4    = SUB(WEAPON, 16),
		WEAPON_FARMACHINING_LEVEL5    = SUB(WEAPON, 17),

		EQUIPMENT_HEAD                = SUB(EQUIPMENT, 1),	    //ͷ��    
		EQUIPMENT_BODY                = SUB(EQUIPMENT, 2),		//����    
		EQUIPMENT_BACK                = SUB(EQUIPMENT, 3),		//����    
		EQUIPMENT_SHOULDER            = SUB(EQUIPMENT, 4),		//�粿    
		EQUIPMENT_HAND                = SUB(EQUIPMENT, 5),		//�ֲ�    
		EQUIPMENT_WAIST               = SUB(EQUIPMENT, 6),		//����    
		EQUIPMENT_FOOT                = SUB(EQUIPMENT, 7),		//�Ų�  
		EQUIPMENT_COMPOSE_LEVEL1      = SUB(EQUIPMENT, 8),      //�ϳ���Ƥ
		EQUIPMENT_COMPOSE_LEVEL2      = SUB(EQUIPMENT, 9),
		EQUIPMENT_COMPOSE_LEVEL3      = SUB(EQUIPMENT, 10),
		EQUIPMENT_COMPOSE_LEVEL4      = SUB(EQUIPMENT, 11),
		EQUIPMENT_COMPOSE_LEVEL5      = SUB(EQUIPMENT, 12),
		EQUIPMENT_MACHINING_LEVEL1    = SUB(EQUIPMENT, 13),      //�ӹ���Ƥ
		EQUIPMENT_MACHINING_LEVEL2    = SUB(EQUIPMENT, 14),
		EQUIPMENT_MACHINING_LEVEL3    = SUB(EQUIPMENT, 15),
		EQUIPMENT_MACHINING_LEVEL4    = SUB(EQUIPMENT, 16),
		EQUIPMENT_MACHINING_LEVEL5    = SUB(EQUIPMENT, 17),

		ORNAMENT_AMULET               = SUB(ORNAMENT, 1),       //�����
		ORNAMENT_NECK                 = SUB(ORNAMENT, 2),       //����
		ORNAMENT_RING                 = SUB(ORNAMENT, 3),       //��ָ
		ORNAMENT_MACHINING_LEVEL1     = SUB(ORNAMENT, 4),       //���ϼӹ�
		ORNAMENT_MACHINING_LEVEL2     = SUB(ORNAMENT, 5),
		ORNAMENT_MACHINING_LEVEL3     = SUB(ORNAMENT, 6),
		ORNAMENT_MACHINING_LEVEL4     = SUB(ORNAMENT, 7),
		ORNAMENT_MACHINING_LEVEL5     = SUB(ORNAMENT, 8),

		ALCHEMY_RENEW                 = SUB(ALCHEMY, 1),    //�ָ���
		ALCHEMY_STATUS                = SUB(ALCHEMY, 2),    //״̬�� 
		ALCHEMY_SPECIAL               = SUB(ALCHEMY, 3),    //������ 
		ALCHEMY_MACHINING_LEVEL1      = SUB(ALCHEMY, 4),    //ҩ�ļӹ�
		ALCHEMY_MACHINING_LEVEL2      = SUB(ALCHEMY, 5),
		ALCHEMY_MACHINING_LEVEL3      = SUB(ALCHEMY, 6),
		ALCHEMY_MACHINING_LEVEL4      = SUB(ALCHEMY, 7),
		ALCHEMY_MACHINING_LEVEL5      = SUB(ALCHEMY, 8),

		FOOD_RENEW                    = SUB(FOOD, 1),       //�ָ���
		FOOD_BLOOD                    = SUB(FOOD, 2),       //�����Ѫ
		FOOD_BLUE                     = SUB(FOOD, 3),       //�������
		FOOD_MACHINING_LEVEL1         = SUB(FOOD, 4),       //ʳ�ļӹ�
		FOOD_MACHINING_LEVEL2         = SUB(FOOD, 5),
		FOOD_MACHINING_LEVEL3         = SUB(FOOD, 6),
		FOOD_MACHINING_LEVEL4         = SUB(FOOD, 7),
		FOOD_MACHINING_LEVEL5         = SUB(FOOD, 8),
	};
	enum 
	{
		MATERIAL_MAX = 6,        // ��������
	};

	struct stMaterial
	{
		U32     resID;     // ����ID
		U8      resNum;    // ��������
	};

protected:
	U32                 mSerialID;                // �䷽ID
	U16				    mCategory;			      // �䷽��� 
	U16				    mSubCategory;		      // �䷽�����
	U8                  mLevel;                   // �䷽�ȼ�
	U16                 mVigour;                  // ����
	U32                 mInsight;                 // ����
	U32                 mLivingSkillID;           // �����ID
	U32                 mMoney;                   // �����Ǯ
	U16                 mSubCategoryTools;        // ���ĵ��������
	U32                 mbaseItemID;              // ����������ƷID
	bool                mbScript;                 // ǰ�ýű��ж�
	U8                  mItemNum;                 // һ�β�������
	StringTableEntry    mDesc;                    // ����
	StringTableEntry    mName;                    // �䷽����
	stMaterial          mMaterial[MATERIAL_MAX];  // �������  
public:
	PrescriptionData();

	inline   U32                 getSerialID         ()        { return mSerialID; }
	inline   U16                 getCategory         ()        { return mCategory; }
	inline   U16                 getSubCategory      ()        { return mSubCategory; }
	inline   U8                  getLevel            ()        { return mLevel; }
	inline   U16                 getVigour           ()        { return mVigour; }
	inline   U32                 getInsight          ()        { return mInsight; }
	inline   U32                 getLivingSkillID    ()        { return mLivingSkillID; }
	inline   U32                 getMoney            ()        { return mMoney; }
	inline   U16                 getToolsType        ()        { return mSubCategoryTools; }
	inline   bool                getScript           ()        { return mbScript; }
	inline   StringTableEntry    getDesc             ()        { return mDesc; }
	inline   stMaterial*         getMaterial         ()        { return mMaterial; }
	inline   U32                 getBaseItemID       ()        { return mbaseItemID; }  
	inline   U8                  getItemNum          ()        { return mItemNum; }
	StringTableEntry             getName             ();
	StringTableEntry             getSubCategoryName  ();

public:
	friend class Prescription;
	friend class PrescriptionRepository;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IColumnData;
class PrescriptionRepository
{
public:
	typedef stdext::hash_map<U32, PrescriptionData*> dataMap;
	dataMap mDataMap;
	IColumnData* mColumnData;
public:
	PrescriptionRepository();
	~PrescriptionRepository();
	void                   clear();
	bool                   read();
	bool                   insert(PrescriptionData* pData);
	PrescriptionData*      getPrescriptionData(U32 SerialID);
};
extern PrescriptionRepository g_PrescriptionRepository;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct stPlayerStruct;
class GameConnection;
class Player;
class Prescription
{
public:
	enum Constants
	{
		COMPOSE_NONMAX = 99,  // ����Ҫ���ϵ����ϳ�����
		RECORD_MAXSIZE = 150, // ���η�������¼��
		COMPOSE_MAX = 999,    // ���ϳ�����
		
	};
	typedef stdext::hash_map<U32, PrescriptionData*> PrescriptionMap;
private:
	PrescriptionMap mPrescriptionTab;
public:
	Prescription();
	U16                      canCompose           (Player* player,U32 SerialID);
	U16                      getNeedResCount      (PrescriptionData* pData,U8 index);          // ��ȡ�����������
	StringTableEntry         getNeedResName       (PrescriptionData* pData,U8 index);          // ��ȡ���������
	U8                       getColResCount       (PrescriptionData* pData);                   // ��ȡ��������
	bool                     addPrescription      (U32 SerialID);
	U16                      getPrescriptionCount ();
	inline PrescriptionMap&  getPrescriptionTab   () { return mPrescriptionTab; }
#ifdef NTJ_SERVER
	bool                  saveData             (stPlayerStruct* playerInfo);
	bool                  updateToClient       (GameConnection* conn,U32 mSerialID=0);
#endif
};
extern Prescription g_Prescription;

#endif