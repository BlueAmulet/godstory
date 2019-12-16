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
		WEAPON     = 1,             // 武器
		EQUIPMENT  = 2,             // 装备
		ORNAMENT   = 3,             // 饰品 
		CHARM      = 4,             // 符咒
		ALCHEMY    = 5,             // 丹药
		FOOD       = 6,             // 食物
		ARTISAM    = 7,             // 工匠
		POISON     = 8,             // 蛊术
		

		WEAPON_FAQI                   = SUB(WEAPON, 1),        //法器
		WEAPON_SIGLESHORT             = SUB(WEAPON, 2),        //单短
		WEAPON_DOUBLESHORT            = SUB(WEAPON, 3),        //双短
		WEAPON_QIN                    = SUB(WEAPON, 4),        //琴
		WEAPON_BOW                    = SUB(WEAPON, 5),        //弓
		WEAPON_FLAGSTAFF              = SUB(WEAPON, 6),        //旗杖
		WEAPON_ARMS                   = SUB(WEAPON, 7),        //刀斧
		WEAPON_NEARMACHINING_LEVEL1   = SUB(WEAPON, 8),        //近程材料加工    
		WEAPON_NEARMACHINING_LEVEL2   = SUB(WEAPON, 9),
		WEAPON_NEARMACHINING_LEVEL3   = SUB(WEAPON, 10),
		WEAPON_NEARMACHINING_LEVEL4   = SUB(WEAPON, 11),
		WEAPON_NEARMACHINING_LEVEL5   = SUB(WEAPON, 12),
		WEAPON_FARMACHINING_LEVEL1    = SUB(WEAPON, 13),       //远程材料加工
		WEAPON_FARMACHINING_LEVEL2    = SUB(WEAPON, 14),
		WEAPON_FARMACHINING_LEVEL3    = SUB(WEAPON, 15),
		WEAPON_FARMACHINING_LEVEL4    = SUB(WEAPON, 16),
		WEAPON_FARMACHINING_LEVEL5    = SUB(WEAPON, 17),

		EQUIPMENT_HEAD                = SUB(EQUIPMENT, 1),	    //头部    
		EQUIPMENT_BODY                = SUB(EQUIPMENT, 2),		//身体    
		EQUIPMENT_BACK                = SUB(EQUIPMENT, 3),		//背部    
		EQUIPMENT_SHOULDER            = SUB(EQUIPMENT, 4),		//肩部    
		EQUIPMENT_HAND                = SUB(EQUIPMENT, 5),		//手部    
		EQUIPMENT_WAIST               = SUB(EQUIPMENT, 6),		//腰部    
		EQUIPMENT_FOOT                = SUB(EQUIPMENT, 7),		//脚部  
		EQUIPMENT_COMPOSE_LEVEL1      = SUB(EQUIPMENT, 8),      //合成兽皮
		EQUIPMENT_COMPOSE_LEVEL2      = SUB(EQUIPMENT, 9),
		EQUIPMENT_COMPOSE_LEVEL3      = SUB(EQUIPMENT, 10),
		EQUIPMENT_COMPOSE_LEVEL4      = SUB(EQUIPMENT, 11),
		EQUIPMENT_COMPOSE_LEVEL5      = SUB(EQUIPMENT, 12),
		EQUIPMENT_MACHINING_LEVEL1    = SUB(EQUIPMENT, 13),      //加工兽皮
		EQUIPMENT_MACHINING_LEVEL2    = SUB(EQUIPMENT, 14),
		EQUIPMENT_MACHINING_LEVEL3    = SUB(EQUIPMENT, 15),
		EQUIPMENT_MACHINING_LEVEL4    = SUB(EQUIPMENT, 16),
		EQUIPMENT_MACHINING_LEVEL5    = SUB(EQUIPMENT, 17),

		ORNAMENT_AMULET               = SUB(ORNAMENT, 1),       //护身符
		ORNAMENT_NECK                 = SUB(ORNAMENT, 2),       //颈部
		ORNAMENT_RING                 = SUB(ORNAMENT, 3),       //戒指
		ORNAMENT_MACHINING_LEVEL1     = SUB(ORNAMENT, 4),       //材料加工
		ORNAMENT_MACHINING_LEVEL2     = SUB(ORNAMENT, 5),
		ORNAMENT_MACHINING_LEVEL3     = SUB(ORNAMENT, 6),
		ORNAMENT_MACHINING_LEVEL4     = SUB(ORNAMENT, 7),
		ORNAMENT_MACHINING_LEVEL5     = SUB(ORNAMENT, 8),

		ALCHEMY_RENEW                 = SUB(ALCHEMY, 1),    //恢复类
		ALCHEMY_STATUS                = SUB(ALCHEMY, 2),    //状态类 
		ALCHEMY_SPECIAL               = SUB(ALCHEMY, 3),    //特殊类 
		ALCHEMY_MACHINING_LEVEL1      = SUB(ALCHEMY, 4),    //药材加工
		ALCHEMY_MACHINING_LEVEL2      = SUB(ALCHEMY, 5),
		ALCHEMY_MACHINING_LEVEL3      = SUB(ALCHEMY, 6),
		ALCHEMY_MACHINING_LEVEL4      = SUB(ALCHEMY, 7),
		ALCHEMY_MACHINING_LEVEL5      = SUB(ALCHEMY, 8),

		FOOD_RENEW                    = SUB(FOOD, 1),       //恢复类
		FOOD_BLOOD                    = SUB(FOOD, 2),       //特殊回血
		FOOD_BLUE                     = SUB(FOOD, 3),       //特殊回蓝
		FOOD_MACHINING_LEVEL1         = SUB(FOOD, 4),       //食材加工
		FOOD_MACHINING_LEVEL2         = SUB(FOOD, 5),
		FOOD_MACHINING_LEVEL3         = SUB(FOOD, 6),
		FOOD_MACHINING_LEVEL4         = SUB(FOOD, 7),
		FOOD_MACHINING_LEVEL5         = SUB(FOOD, 8),
	};
	enum 
	{
		MATERIAL_MAX = 6,        // 材料项数
	};

	struct stMaterial
	{
		U32     resID;     // 材料ID
		U8      resNum;    // 材料数量
	};

protected:
	U32                 mSerialID;                // 配方ID
	U16				    mCategory;			      // 配方类别 
	U16				    mSubCategory;		      // 配方子类别
	U8                  mLevel;                   // 配方等级
	U16                 mVigour;                  // 活力
	U32                 mInsight;                 // 悟性
	U32                 mLivingSkillID;           // 生活技能ID
	U32                 mMoney;                   // 所需金钱
	U16                 mSubCategoryTools;        // 消耗道具子类别
	U32                 mbaseItemID;              // 基础产出物品ID
	bool                mbScript;                 // 前置脚本判断
	U8                  mItemNum;                 // 一次产出个数
	StringTableEntry    mDesc;                    // 描述
	StringTableEntry    mName;                    // 配方名称
	stMaterial          mMaterial[MATERIAL_MAX];  // 所需材料  
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
		COMPOSE_NONMAX = 99,  // 无需要材料的最大合成数量
		RECORD_MAXSIZE = 150, // 单次发送最大记录数
		COMPOSE_MAX = 999,    // 最大合成数量
		
	};
	typedef stdext::hash_map<U32, PrescriptionData*> PrescriptionMap;
private:
	PrescriptionMap mPrescriptionTab;
public:
	Prescription();
	U16                      canCompose           (Player* player,U32 SerialID);
	U16                      getNeedResCount      (PrescriptionData* pData,U8 index);          // 获取所需材料数量
	StringTableEntry         getNeedResName       (PrescriptionData* pData,U8 index);          // 获取所需材料名
	U8                       getColResCount       (PrescriptionData* pData);                   // 获取材料项数
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