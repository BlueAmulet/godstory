//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _LIVINGSKILL_H_ 
#define _LIVINGSKILL_H_
#include <hash_map>
#include "Gameplay/GameObjects/GameObjectData.h"
#define SUB(a,b) a * 100 + b
#define GETSUBCATEGORY(skillId) (skillId/10000)%10000
#define LIVINGSKILL_ID_CATEGORYLIMIT   500000001
#define LIVINGSKILL_ID_LIMIT           501000000

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LivingSkillData
{
public:
	enum Constants
	{ 
		GATHER                  = 1,                 // �ɼ�
		PRODUCE_FOUNDRY         = 2,                 // ����
		PRODUCE_NEEDLEMAN       = 3,                 // �÷�
		PRODUCE_TECHNICS        = 4,                 // ����
		PRODUCE_CHARM           = 5,                 // ����
		PRODUCE_ELIXIR          = 6,                 // ����
		PRODUCE_COOKING         = 7,                 // ���
		PRODUCE_ARTISAM         = 8,                 // ����
		PRODUCE_POISON          = 9,                 // ����

		GATHER_MINING           = SUB(GATHER,1),     // �ɿ�
		GATHER_FELLING          = SUB(GATHER,2),     // ��ľ
		GATHER_FISHING          = SUB(GATHER,3),     // ����
		GATHER_PLANTING         = SUB(GATHER,4),     // ��ֲ
		GATHER_HERB             = SUB(GATHER,5),     // ��ҩ
		GATHER_SHIKAR           = SUB(GATHER,6),     // ����
	};

	enum LivingSkillType
	{
		LIVINGSKILL_NORM = 1,             // ��ͨ����
		LIVINGSKILL_PRO ,                 // ר������
	};

	enum CastMode
	{
		CAST_NOTHING = 0,                // ������
		CAST_NOCAST,                     // ������
		CAST_ITEMUSETIMES,               // ʹ�ô���
		
		
	};
	enum GuerdonType
	{
		GUERDON_RIPE,                   // ������
	};

	struct LearnLimit 
	{
		U32 prepSkill;			 // ǰ�ü���
		U32 level;				 // �ȼ�
		U32 money;				 // ��Ǯ
		U32 ripe;                // ������
		U32 exp;				 // ����
		U32 title;               // �ƺ�
		StringTableEntry scriptFunction;	// �ű��ж�
	};
	
	struct Cost//Cast
	{
		U8  mode;                // ���ķ�ʽ
		U16 itemSubCategory;     // ��������
		U16 itemUseTimes;        // ʹ�ô���
		U32 vigour;              // ����
		U32 insight;             // ����
	}; 

	struct Cast//Cost
	{
		SimTime cooldown;        // CDʱ��
		SimTime readyTime;       // ����ʱ��
		S32 cdGroup;			 // CD��
		GameObjectData::Animations readyBeginAction;      //����������ʼ
		GameObjectData::Animations readyLoopAction;       //��������ѭ��
		GameObjectData::Animations readyEndAction;        //������������
	};

	struct Guerdon
	{
		U8  type;                   // ��������
		U8  num;                    // ��������
	};

	friend class LivingSkillRepository;
	friend class LivingSkillTable;

protected:
	U32               mSkillID;             // ���
	U16               mlevel;               // �ȼ�
	U32               mRipe;                // ������
	U32               mRouteID;             // Ѱ��ID
	U16				  mCategory;			// �������
	U16				  mSubCategory;			// ���������
	U8                mType;                // ��������
	StringTableEntry  mName;                // ����
	StringTableEntry  mIconName;            // Icon
	StringTableEntry  mDesc;                // ����
	LearnLimit        mLearnLimit;          // ѧϰ����
	Cost              mCost;                // ʩ��
	Cast              mCast;                // ����
	Guerdon           mGuerdon;             // ����

public:
	LivingSkillData();

	inline U32                getID             ()          { return mSkillID; }
	inline U32                getRipe           ()          { return mRipe; }
	inline U16                getLevel          ()          { return mlevel; }
	inline U16                getCategory       ()          { return mCategory;}       
	inline U16                getSubCategory    ()          { return mSubCategory;}
	inline U8                 getType           ()          { return mType; }
	inline StringTableEntry   getName           ()          { return mName; }
	inline StringTableEntry   getIconName       ()          { return mIconName; }
	inline StringTableEntry   getDesc           ()          { return mDesc; }
	inline LearnLimit&        getLearnLimit     ()          { return mLearnLimit; }
	inline Cost&              getCost           ()          { return mCost; }
	inline Cast&              getCast           ()          { return mCast; }
	inline Guerdon&           getGuerdon        ()          { return mGuerdon; }
	StringTableEntry          getCategoryName   ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IColumnData;
class LivingSkillRepository
{
public:
	typedef stdext::hash_map<U32, LivingSkillData*> LivingSkillMap;
	LivingSkillMap mLivingSkillMap;
	IColumnData* mColumnData;
public:
	LivingSkillRepository();
	~LivingSkillRepository();

	bool	          read();
	void	          clear();
	bool	          insert(LivingSkillData* pData);
	LivingSkillData*  getLivingSkillData(U32 SkillID);
};
extern LivingSkillRepository g_LivingSkillRespository;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LivingSkill
{
	friend class LivingSkillTable;
protected:
	U32              mRipe;                // ������
	LivingSkillData* mLivingSkillData;
public:
	LivingSkill();
	~LivingSkill();
	inline U32              getRipe() { return mRipe; }
	inline void             setRipe(U32 Value) { mRipe = Value; }
	inline LivingSkillData* getData() { return mLivingSkillData; }
	void                    setData(LivingSkillData* pData) { mLivingSkillData = pData;}
private:
	static LivingSkill*     create(LivingSkillData* pData);
	
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Player;
struct stPlayerStruct;
class GameConnection;
class LivingSkillTable
{
public:
	enum
	{
		MAXTAB = 9,
	};
	typedef stdext::hash_map<U32, LivingSkill*> LivingSkillTableMap;
private:
	LivingSkillTableMap  mLivingSkillTable;    // ��ѧ���ܱ�
#ifdef NTJ_CLIENT
	Vector<U32> mLivingSkillView[MAXTAB];  // ����Ԥ����
#endif
public:
	LivingSkillTable();
	~LivingSkillTable();
	enWarnMessage	canLearn(Player* player, U32 skillID);
	bool            learnLivingSkill(Player* player, U32 skillID);
	bool            isLearn(U32 skillID);        //������Ƿ���ѧ

	LivingSkill*    addLivingSkill(U32 skillID) ;
	LivingSkill*    addLivingSkill(U32 skillID,U32 currRipe) ;
	LivingSkill*    addLivingSkill(LivingSkillData* pData,U32 currRipe=0);
	LivingSkill*    getLivingSkill(U32 skillID);    
	void            removeLivingSkill(U32 skillID);
	void            clear();

	inline LivingSkillTableMap& getLivingSkillTable(){ return mLivingSkillTable; }

#ifdef NTJ_CLIENT
	void            updateLiveingSkillView(U32 skillID);
	void            initLivingSkillPreview();
	bool            findViewByID(U8 iTab,U32 skillId);//����ͬ�༼��
	Vector<U32>&    getLivingSkillView(U8 index);
#endif

#ifdef NTJ_SERVER
	bool            saveData(stPlayerStruct* playerInfo);
	bool            updateToClient(GameConnection* conn,U32 skillID=0,bool bAddSkill=true);
#endif
};
#endif