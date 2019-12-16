//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/ai/AIData.h"
#include "Gameplay/Data/readDataFile.h"
#include "Util/ColumnData.h"
#include "console/console.h"


//-----------------------------------------------------------------------------
// 静态变量
//-----------------------------------------------------------------------------

AICondition::FunctionCommon AICondition::funcCom[] = {};
//AICondition::FunctionPlayer AICondition::funcPlayer[] = {};
AICondition::FunctionNpc AICondition::funcNpc[] = {};
AICondition::FunctionPet AICondition::funcPet[] = {};
//AICondition::FunctionTotem AICondition::funcTotem[] = {};

AIOperation::FunctionCommon AIOperation::funcCom[] = {};
//AIOperation::FunctionPlayer AIOperation::funcPlayer[] = {};
AIOperation::FunctionNpc AIOperation::funcNpc[] = {};
AIOperation::FunctionPet AIOperation::funcPet[] = {};
//AIOperation::FunctionTotem AIOperation::funcTotem[] = {};

//-----------------------------------------------------------------------------
// AI条件数据结构
//-----------------------------------------------------------------------------
void AICondition::initFunc()
{
	funcCom[Func_GetSex] = &AIBase::AICd_GetSex;
	funcCom[Func_GetRace] = &AIBase::AICd_GetRace;
	funcCom[Func_GetFamily] = &AIBase::AICd_GetFamily;
	funcCom[Func_GetLevel] = &AIBase::AICd_GetLevel;
	funcCom[Func_GetExp] = &AIBase::AICd_GetExp;
	funcCom[Func_IsCombative] = &AIBase::AICd_IsCombative;
	funcCom[Func_IsInCombat] = &AIBase::AICd_IsInCombat;
	funcCom[Func_CheckBuff] = &AIBase::AICd_CheckBuff;
	funcCom[Func_GetAIStateTimer] = &AIBase::AICd_GetAIStateTimer;
	funcCom[Func_IsPlayer] = &AIBase::AICd_IsPlayer;
	funcCom[Func_IsNpc] = &AIBase::AICd_IsNpc;
	funcCom[Func_IsPet] = &AIBase::AICd_IsPet;
	funcCom[Func_IsTotem] = &AIBase::AICd_IsTotem;

	funcCom[Func_GetHP] = &AIBase::AICd_GetHP;
	funcCom[Func_GetMP] = &AIBase::AICd_GetMP;
	funcCom[Func_GetPP] = &AIBase::AICd_GetPP;
	funcCom[Func_GetMaxHP] = &AIBase::AICd_GetMaxHP;
	funcCom[Func_GetMaxMP] = &AIBase::AICd_GetMaxMP;
	funcCom[Func_GetMaxPP] = &AIBase::AICd_GetMaxPP;
	funcCom[Func_GetHP_Pc] = &AIBase::AICd_GetHP_Pc;
	funcCom[Func_GetMP_Pc] = &AIBase::AICd_GetMP_Pc;
	funcCom[Func_GetPP_Pc] = &AIBase::AICd_GetPP_Pc;

	funcCom[Func_GetStamina] = &AIBase::AICd_GetStamina;
	funcCom[Func_GetMana] = &AIBase::AICd_GetMana;
	funcCom[Func_GetStrength] = &AIBase::AICd_GetStrength;
	funcCom[Func_GetIntellect] = &AIBase::AICd_GetIntellect;
	funcCom[Func_GetAgility] = &AIBase::AICd_GetAgility;
	funcCom[Func_GetPneuma] = &AIBase::AICd_GetPneuma;
	funcCom[Func_GetInsight] = &AIBase::AICd_GetInsight;
	funcCom[Func_GetLuck] = &AIBase::AICd_GetLuck;

	funcCom[Func_GetVelocity] = &AIBase::AICd_GetVelocity;
	funcCom[Func_GetAttackSpeed] = &AIBase::AICd_GetAttackSpeed;
	funcCom[Func_GetSpellSpeed] = &AIBase::AICd_GetSpellSpeed;
	funcCom[Func_GetCritical_Pc] = &AIBase::AICd_GetCritical_Pc;
	funcCom[Func_GetAntiCritical_Pc] = &AIBase::AICd_GetAntiCritical_Pc;
	funcCom[Func_GetFatal_Pc] = &AIBase::AICd_GetFatal_Pc;
	funcCom[Func_GetDodge_Pc] = &AIBase::AICd_GetDodge_Pc;
	funcCom[Func_GetHit_Pc] = &AIBase::AICd_GetHit_Pc;
	funcCom[Func_GetDamageHandle_Pc] = &AIBase::AICd_GetDamageHandle_Pc;

	funcCom[Func_GetPhyDamage] = &AIBase::AICd_GetPhyDamage;
	funcCom[Func_GetPhyDefence] = &AIBase::AICd_GetPhyDefence;
	funcCom[Func_GetMuDamage] = &AIBase::AICd_GetMuDamage;
	funcCom[Func_GetMuDefence] = &AIBase::AICd_GetMuDefence;
	funcCom[Func_GetHuoDamage] = &AIBase::AICd_GetHuoDamage;
	funcCom[Func_GetHuoDefence] = &AIBase::AICd_GetHuoDefence;
	funcCom[Func_GetTuDamage] = &AIBase::AICd_GetTuDamage;
	funcCom[Func_GetTuDefence] = &AIBase::AICd_GetTuDefence;
	funcCom[Func_GetJinDamage] = &AIBase::AICd_GetJinDamage;
	funcCom[Func_GetJinDefence] = &AIBase::AICd_GetJinDefence;
	funcCom[Func_GetShuiDamage] = &AIBase::AICd_GetShuiDamage;
	funcCom[Func_GetShuiDefence] = &AIBase::AICd_GetShuiDefence;
	funcCom[Func_GetPneumaDamage] = &AIBase::AICd_GetPneumaDamage;
	funcCom[Func_GetPneumaDefence] = &AIBase::AICd_GetPneumaDefence;
	funcCom[Func_GetHeal] = &AIBase::AICd_GetHeal;

	funcCom[Func_GetPhyScale] = &AIBase::AICd_GetPhyScale;
	funcCom[Func_GetSplScale] = &AIBase::AICd_GetSplScale;
	funcCom[Func_GetHealScale] = &AIBase::AICd_GetHealScale;
	funcCom[Func_GetHateScale] = &AIBase::AICd_GetHateScale;

	funcCom[Func_GetDeriveHP_Pc] = &AIBase::AICd_GetDeriveHP_Pc;
	funcCom[Func_GetDeriveMP_Pc] = &AIBase::AICd_GetDeriveMP_Pc;
	funcCom[Func_GetDerivePP_Pc] = &AIBase::AICd_GetDerivePP_Pc;

	funcCom[Func_GetMu] = &AIBase::AICd_GetMu;
	funcCom[Func_GetHuo] = &AIBase::AICd_GetHuo;
	funcCom[Func_GetTu] = &AIBase::AICd_GetTu;
	funcCom[Func_GetJin] = &AIBase::AICd_GetJin;
	funcCom[Func_GetShui] = &AIBase::AICd_GetShui;
	funcCom[Func_GetExitInterval] = &AIBase::AICd_GetExitInterval;
}

//-----------------------------------------------------------------------------
// AI操作数据结构
//-----------------------------------------------------------------------------
void AIOperation::initFunc()
{
	funcCom[Func_AddBuff] = &AIBase::AIOp_AddBuff;
}

//-----------------------------------------------------------------------------
// AISpell
//-----------------------------------------------------------------------------

AISpell::~AISpell()
{
	for (S32 i=0; i<MaxSpells; ++i)
	{
		SAFE_DELETE(entry[i]);
	}
};

//-----------------------------------------------------------------------------
// AI性格数据结构
//-----------------------------------------------------------------------------

AICharacter::~AICharacter()
{
	for (S32 i=0; i<MaxAIStates; ++i)
	{
		for (S32 j=0; j<MaxAIStates; ++j)
		{
			SAFE_DELETE(aiGoto[i][j]);
		}
	}
};

//-----------------------------------------------------------------------------
// AI仓库
//-----------------------------------------------------------------------------

AIRepository g_AIRepository;

AIRepository::AIRepository()
{
	clear();
}

AIRepository::~AIRepository()
{
	clear();
}

void AIRepository::read()
{
	CDataFile file;
	RData tempData;
	char filename[1024] = {0};

	// 读取AI条件数据
	Platform::makeFullPathName(GAME_AICDDATA_FILE, filename, sizeof(filename));
	file.ReadDataInit();
	file.readDataFile(filename);

	IColumnData *columnData = new IColumnData(file.ColumNum, "AICdRepository.dat");
	columnData->setField(CalcOffset(id,						AICondition),	DType_U32,		"id");
	columnData->setField(CalcOffset(type,					AICondition),	DType_enum8,	"对象类型");
	columnData->setField(CalcOffset(object,					AICondition),	DType_enum8,	"条件对象");
	columnData->setField(CalcOffset(func,					AICondition),	DType_enum16,	"条件函数");
	columnData->setField(CalcOffset(param1,					AICondition),	DType_S32,		"参数1");
	columnData->setField(CalcOffset(param2,					AICondition),	DType_S32,		"参数2");
	columnData->setField(CalcOffset(cmp,					AICondition),	DType_enum8,	"操作符");
	columnData->setField(CalcOffset(val,					AICondition),	DType_S32,		"对比值");
	for(int k = 0; k < file.RecordNum; ++k)
	{
		AICondition* pData = new AICondition;
		for(int h = 0; h < file.ColumNum; ++h)
		{
			file.GetData(tempData);
			columnData->setData(pData, h, tempData);
		}
		mAICdMap.insert(AICdMap::value_type(pData->id, pData));
	}
	delete columnData;

	// 读取AI操作数据
	Platform::makeFullPathName(GAME_AIOPDATA_FILE, filename, sizeof(filename));
	file.ReadDataInit();
	file.readDataFile(filename);

	columnData = new IColumnData(file.ColumNum, "AIOpRepository.dat");
	columnData->setField(CalcOffset(id,						AIOperation),	DType_U32,		"id");
	columnData->setField(CalcOffset(type,					AIOperation),	DType_enum8,	"对象类型");
	columnData->setField(CalcOffset(object,					AIOperation),	DType_enum8,	"处理对象");
	columnData->setField(CalcOffset(func,					AICondition),	DType_enum16,	"处理函数");
	columnData->setField(CalcOffset(param1,					AICondition),	DType_S32,		"参数1");
	columnData->setField(CalcOffset(param2,					AICondition),	DType_S32,		"参数2");
	for(int k = 0; k < file.RecordNum; ++k)
	{
		AIOperation* pData = new AIOperation;
		for(int h = 0; h < file.ColumNum; ++h)
		{
			file.GetData(tempData);
			columnData->setData(pData, h, tempData);
		}
		mAIOpMap.insert(AIOpMap::value_type(pData->id, pData));
	}
	delete columnData;

	// 读取AI性格数据
	Platform::makeFullPathName(GAME_AICHARDATA_FILE, filename, sizeof(filename));
	file.ReadDataInit();
	file.readDataFile(filename);

	for(int k = 0 ; k < file.RecordNum ; ++k)
	{
		static U32 id;
		static StringTableEntry charName;
		static U32 type;
		static U32 uId;
		static AICdMap::iterator cd_itr;
		static AIOpMap::iterator op_itr;

		AIGoto* pData = new AIGoto;
		//id
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "AIRepository.dat::Read");
		id = tempData.m_U32;
		//charName
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "AIRepository.dat::Read");
		charName = StringTable->insert(tempData.m_string);
		//type
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_enum8, "AIRepository.dat::Read");
		type = tempData.m_Enum8;
		//src
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "AIRepository.dat::Read");
		pData->srcState = tempData.m_U8;
		//tgt
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "AIRepository.dat::Read");
		pData->tgtState = tempData.m_U8;
		//flagNOT
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "AIRepository.dat::Read");
		pData->flagNOT = tempData.m_U8;
		//flagOR
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "AIRepository.dat::Read");
		pData->flagOR = tempData.m_U8;

		for (S32 i=0, j=0; i<AIGoto::MaxCondition; ++i)
		{
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_U32, "AIRepository.dat::Read");
			uId = tempData.m_U32;
			if(uId)
			{
				cd_itr = mAICdMap.find(uId);
				if(cd_itr != mAICdMap.end() && cd_itr->second)
				{
					pData->condition[j] = cd_itr->second;
					++j;
				}
				else
					Con::errorf("can NOT find AICondition : %d", uId);
			}
		}
		for (S32 i=0, j=0; i<AIGoto::MaxOperation; ++i)
		{
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_U32, "AIRepository.dat::Read");
			uId = tempData.m_U32;
			if(uId)
			{
				op_itr = mAIOpMap.find(uId);
				if(op_itr != mAIOpMap.end() && op_itr->second)
				{
					pData->operation[j] = op_itr->second;
					++j;
				}
				else
					Con::errorf("can NOT find AIOperation : %d", uId);
			}
		}

		AICharMap::iterator itr = mAICharMap.find(id);
		if(itr == mAICharMap.end() || !itr->second)
		{
			AICharacter* pAIchar = new AICharacter;
			pAIchar->charId = id;
			pAIchar->charName = charName;
			pAIchar->type = (AI_ObjectType)type;
			itr = mAICharMap.insert(itr, AICharMap::value_type(id, pAIchar));
			AssertFatal(itr != mAICharMap.end() && itr->second, "");
		}
		AICharacter* pAIchar = (AICharacter*)itr->second;
		for (S32 i=0; i<AICharacter::MaxAIStates; ++i)
		{
			if(!pAIchar->aiGoto[pData->srcState][i])
			{
				pAIchar->aiGoto[pData->srcState][i] = pData;
				break;
			}
			AssertFatal(i != AICharacter::MaxAIStates - 1, "");
		}
	}

	// 读取AISpell数据
	Platform::makeFullPathName(GAME_AISPELLDATA_FILE, filename, sizeof(filename));
	file.ReadDataInit();
	file.readDataFile(filename);

	for(int k = 0 ; k < file.RecordNum ; ++k)
	{
		static U32 uId;
		static AICdMap::iterator cd_itr;
		AISpellEntry* pData = new AISpellEntry;
		//id
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "AISpellRepository.dat::Read");
		pData->id = tempData.m_U32;
		//skillId
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "AISpellRepository.dat::Read");
		pData->skillId = tempData.m_U32;
		//rate
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U16, "AISpellRepository.dat::Read");
		pData->rate = tempData.m_U16;
		//flagNOT
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "AISpellRepository.dat::Read");
		pData->flagNOT = tempData.m_U8;
		//flagOR
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U8, "AISpellRepository.dat::Read");
		pData->flagOR = tempData.m_U8;

		for (S32 i=0, j=0; i<AISpellEntry::MaxCondition; ++i)
		{
			file.GetData(tempData);
			AssertRelease(tempData.m_Type == DType_U32, "AISpellRepository.dat::Read");
			uId = tempData.m_U32;
			if(uId)
			{
				cd_itr = mAICdMap.find(uId);
				if(cd_itr != mAICdMap.end() && cd_itr->second)
				{
					pData->condition[j] = cd_itr->second;
					++j;
				}
				else
					Con::errorf("can NOT find AICondition : %d", uId);
			}
		}
		AISpellMap::iterator itr = mAISpellMap.find(pData->id);
		if(itr == mAISpellMap.end() || !itr->second)
		{
			AISpell* pAISpell = new AISpell;
			pAISpell->id = pData->id;
			itr = mAISpellMap.insert(itr, AISpellMap::value_type(pAISpell->id, pAISpell));
			AssertFatal(itr != mAISpellMap.end() && itr->second, "");
		}
		AISpell* pAISpell = (AISpell*)itr->second;
		for (S32 i=0; i<AISpell::MaxSpells; ++i)
		{
			if(!pAISpell->entry[i])
			{
				pAISpell->entry[i] = pData;
				break;
			}
			AssertFatal(i != AISpell::MaxSpells - 1, "");
		}
	}

	// 初始化函数
	AICondition::initFunc();
	AIOperation::initFunc();
	// 结束
	file.ReadDataClose();
}

void AIRepository::clear()
{
	AICdMap::iterator cd_itr = mAICdMap.begin();
	for (; cd_itr != mAICdMap.end(); ++cd_itr)
		delete cd_itr->second;
	mAICdMap.clear();

	AIOpMap::iterator op_itr = mAIOpMap.begin();
	for (; op_itr != mAIOpMap.end(); ++op_itr)
		delete op_itr->second;
	mAIOpMap.clear();

	AICharMap::iterator char_itr = mAICharMap.begin();
	for (; char_itr != mAICharMap.end(); ++char_itr)
		delete char_itr->second;
	mAICharMap.clear();

	AISpellMap::iterator spell_itr = mAISpellMap.begin();
	for (; spell_itr != mAISpellMap.end(); ++spell_itr)
		delete spell_itr->second;
	mAISpellMap.clear();
}

const AICharacter* AIRepository::getAIChar(U32 id)
{
	if(!id)
		return NULL;
	AICharMap::iterator itr = mAICharMap.find(id);
	if(itr != mAICharMap.end())
		return itr->second;
	return NULL;
}

const AISpell* AIRepository::getAISpell(U32 id)
{
	if(!id)
		return NULL;
	AISpellMap::iterator itr = mAISpellMap.find(id);
	if(itr != mAISpellMap.end())
		return itr->second;
	return NULL;
}
