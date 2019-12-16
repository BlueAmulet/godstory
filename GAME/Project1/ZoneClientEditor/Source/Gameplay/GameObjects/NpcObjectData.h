//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/GameObjectData.h"
#include "Gameplay/GameplayCommon.h"

class IColumnData;
// ========================================================================================================================================
//	NpcObjectData
//	NpcObjectData ������Npc��ģ����Ϣ
// ========================================================================================================================================
struct NpcObjectData: public GameObjectData
{
	typedef GameObjectData Parent;

	enum Flags
	{
		Flags_Combative			= BIT(0),	// �Ƿ����ս��
		Flags_Tame				= BIT(1),	// �Ƿ�׽
		Flags_Active			= BIT(2),	// ����������
		Flags_ConstBuffId		= BIT(3),	// �̶�buffId������ȼ��仯��
		Flags_ConstScale		= BIT(4),	// ���������
	};

	enum enIntensity
	{
		Intensity_Normal		= 0,		// ��ͨ
		Intensity_Captain,					// ͷĿ
		Intensity_Elite,					// ��Ӣ
		Intensity_RareElite,				// ϡ�о�Ӣ
		Intensity_Boss,						// BOSS
	};

	StringTableEntry	npcName;			// ����
	StringTableEntry	npcTitle;			// �ƺ�
	StringTableEntry	icon;				// icon�ļ���
	StringTableEntry	topIcon;			// ͷ��ͼƬ�ļ���
	U32					flags;				// Npc���Ա�־
	enRace				race;				// ����
	enFamily			family;				// ����
	enInfluence			influence;			// ����(npc����һ������Influence_None)
	enIntensity			intensity;			// ǿ��
	U32					minLevel;			// ��С�ȼ�
	U32					maxLevel;			// ���ȼ�
	U32					buffId;				// ������ֵ
	U32					defaultSkillId;		// ��ͨ��������Id
	U32					experience;			// ����
	U32					teamMatchId;		// ����ƥ��

	U32					exBuffId_a;			// ����״̬1
	U32					exBuffId_b;			// ����״̬2

	F32					minScale;			// ��С���ŷ�Χ
	F32					maxScale;			// ������ŷ�Χ
	F32					visualField;		// ��Ұ��Χ
	F32					activeField;		// ���Χ
	F32					chaseField;			// ׷����Χ
	F32					combatField;		// ս����Χ
	F32					expField;			// ������������ҵ����淶Χ
	SimTime				scanInterval;		// ���м��

	SimTime				riseInterval;		// �������
	SimTime				corpseTime;			// ʬ��ɼ�ʱ��
	SimTime				fadeTime;			// ���뵭��ʱ��
	SimTime				dropTime;			// ���������ʱ��

	U32					eventID;			// �¼�ID
	U32					pathID;				// ·��ID

	U32					packageShapeId;		// ����ģ��ID

	StringTableEntry	attackSound_a;		// ������Ч
	StringTableEntry	attackSound_b;		// ������Ч
	U32					attackSoundOdds;	// ����

	StringTableEntry	interactionSound_a;	// ������Ч
	StringTableEntry	interactionSound_b;	// ������Ч
	StringTableEntry	deadthSound;		// ������Ч

	//
	DECLARE_CONOBJECT(NpcObjectData);
	NpcObjectData();
	~NpcObjectData();
	bool preload(bool server, char errorBuffer[256]);
	bool initDataBlock();
	inline bool isFlags(U32 val) { return (flags & val);}
};


// ========================================================================================================================================
//	NpcRepository
//	NpcRepository����������NpcObjectData����Ϣ
// ========================================================================================================================================
class NpcRepository
{
public:
	NpcRepository();
	~NpcRepository();
	typedef stdext::hash_map<U32, NpcObjectData*> NpcDataMap;
	NpcObjectData*			GetNpcData(U32 NpcID);
	U32						GetNpcIdByIndex(int index);
	S32						GetNpcDataCount();

	bool					Insert(NpcObjectData* pData);
	bool					Read();
	void					Clear();

	IColumnData*			mColumnData;
private:
	NpcDataMap				m_NpcDataMap;
};

extern NpcRepository g_NpcRepository;
