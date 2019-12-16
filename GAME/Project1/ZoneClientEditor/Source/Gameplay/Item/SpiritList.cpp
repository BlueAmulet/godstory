#include "Gameplay/Item/SpiritShortcut.h"
#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/Data/SpiritData.h"
#include "Gameplay/GameObjects/SpiritOperationManager.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/SkillData.h"
#include "Gameplay/GameObjects/Skill.h"
#include "Gameplay/GameObjects/SkillShortcut.h"
#include "Gameplay/Item/TalentShortcut.h"

#ifdef NTJ_CLIENT
#include "Gameplay/Data/SpiritData.h"
#endif

SpiritList::SpiritList() : BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_SPIRITLIST;
}

bool SpiritList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
	SpiritListEvent *ev = new SpiritListEvent(index, flag);
	if (ev && conn)
	{
		conn->postNetEvent(ev);
		return true;
	}
	return false;
}

void SpiritList::Initialize()
{
	for (S32 i = 0; i < MAXSLOTS; i++)
	{		
		SpiritShortcut *pShortcut = SpiritShortcut::CreateSpiritItem(i);
		SetSlot(i, pShortcut);
	}
}


/////////////////////////////////////////
IMPLEMENT_CO_CLIENTEVENT_V1(SpiritListEvent);
SpiritListEvent::SpiritListEvent()
{
	mIndex = 0;
	mFlag = 0;
}

SpiritListEvent::SpiritListEvent(U32 index, U32 flag)
{
	mIndex = index;
	mFlag = flag;
}

void SpiritListEvent::packData(NetConnection *_conn, BitStream * stream)
{
	/*
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* pPlayer = NULL;
	bool valid = (_conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);
	// 更新行为标志
	stream->writeInt(mFlag, Base::Bit8);

	ShortcutObject *pShortcut = pPlayer->mSpiritList.GetSlot(mIndex);
	SpiritShortcut *pSpiritShortcut = NULL;
	valid = (pShortcut && (pSpiritShortcut = dynamic_cast<SpiritShortcut *>(pShortcut)));
	if (!stream->writeFlag(valid))
		return;
	pSpiritShortcut->packUpdate(stream);
#endif
	*/
}

void SpiritListEvent::unpackData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;
	mIndex = stream->readInt(Base::Bit8);
	mFlag = stream->readInt(Base::Bit8);

	if(stream->readFlag())
	{
		// 确认服务端存在槽位对象
		ShortcutObject* pSlot = pPlayer->mSpiritList.GetSlot(mIndex);
		if (pSlot)
		{
			//确认客户端存在槽位对象
			pSlot->unpackUpdate(stream);
		}
		else
		{
			//确认客户端不存在槽位对象
			SpiritShortcut* pShortcut = SpiritShortcut::CreateEmptySpiritItem();
			pShortcut->unpackUpdate(stream);
			pPlayer->mSpiritList.SetSlot(mIndex, pShortcut);
		}
	}
	else
	{
		pPlayer->mSpiritList.SetSlot(mIndex, NULL);
	}
#endif
}

#ifdef NTJ_CLIENT
bool ItemManager::insertText(Player* pPlayer, stSpiritInfo* pSpiritInfo, char* buff, S32 mSlotType, S32 mSlotIndex)
{
	if(!pPlayer)
		return false;
	if(!pSpiritInfo)
		return false;
	char tempbuffer[1024] = "\n";

	if (pSpiritInfo->mDataId == 0)
		return false;

	SpiritData *pSpiritData = g_SpiritRepository.getSpiritData(pSpiritInfo->mDataId);

	if (pSpiritData->mName && pSpiritData->mName[0])
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t m='0' f='楷体_GB2312' n='16' c='0xffffffff'>%s</t>", pSpiritData->mName);
		strcat(buff, tempbuffer);
	}

	if (pSpiritInfo->mLevel)
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t h = '2' c='0xffffffff'>%s%d%s</t>", "等级:", pSpiritInfo->mLevel,"级");
		strcat(buff, tempbuffer);
	}

	dSprintf(tempbuffer, sizeof(tempbuffer), "<b/><b/><b/><b/><b/><b/>");
	strcat(buff, tempbuffer);

	return true;
}
#endif


#ifdef NTJ_CLIENT
//////////////////////////////////SkillList_Spirit   绝招栏//////////////////////////////////
SkillList_Spirit::SkillList_Spirit() : BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_SPIRITSKILLLIST;
}

bool SkillList_Spirit::AddSkillShortcut(Player *pPlayer, U32 nSeiresId, S32 nSlot)
{
	Skill* pSkill = pPlayer->GetSkillTable().GetSkill(nSeiresId);
	if(!pSkill && Macro_GetSkillSeriesId(pPlayer->GetSkillTable().GetDefaultSkillId()) == nSeiresId)
		pSkill = &pPlayer->GetSkillTable().GetDefaultSkill();
	if(pSkill && !IsAdd(pSkill))
	{
		SkillShortcut *pNewShortcut = SkillShortcut::CreateSkill(pSkill);
		SetSlot(nSlot, pNewShortcut);
		return true;
	}

	return false;
}

bool SkillList_Spirit::IsAdd(Skill *pSkill)
{
	for(int i = 0; i < mMaxSlots; ++i)
	{
		if(mSlots[i] && ((SkillShortcut*)mSlots[i])->getSkill() == pSkill)
			return true;
	}
	return false;
}
#endif


#ifdef NTJ_CLIENT
//////////////////////////////////TalentList 元神天赋栏//////////////////////////////////
TalentSkillList::TalentSkillList() : BaseItemList(MAXSLOTS)
{
	mType = SHORTCUTTYPE_SPIRITSKILLLIST;
}

void TalentSkillList::Initialize(Player *pPlayer, S32 nSpiritSlot, bool bIsSelf /*= true*/)
{
	if (!pPlayer)	return;
	SpiritTable &table = pPlayer->getSpiritTable();

	stSpiritInfo *pInfo = table.getSpiritInfo(nSpiritSlot);
	if (!pInfo)	return;			
	SpiritData *pSpiritData = g_SpiritRepository.getSpiritData(pInfo->mDataId);
	if (!pSpiritData) return;
	TalentSetData *pTalentSetData = g_TalentRepository.getTalentSet(pSpiritData->mTalentSetId);
	if (!pTalentSetData)	return;

	TalentShortcut::TARGET_TYPE type = bIsSelf ? TalentShortcut::TARGET_SELF : TalentShortcut::TARGET_OTHER;

	for (S32 i = 0; i < MAXSLOTS; i++)
	{		
		TalentShortcut *pShortcut = TalentShortcut::CreateTalentItem(nSpiritSlot, i, type);
		SetSlot(i, pShortcut);
	}
}

bool ItemManager::insertText(Player* pPlayer, TalentData* pTalentData, char* buff, S32 mSlotType, S32 mSlotIndex)
{
	if(!pPlayer || !pTalentData)
		return false;

	char tempbuffer[1024] = "\n";
	StringTableEntry sTalentName = pTalentData->getName();
	U32 sTalentLevel = pTalentData->getLevel();
	StringTableEntry sTalentText = pTalentData->getText();

	if (sTalentName && sTalentName[0])
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t m='0' f='楷体_GB2312' n='16' c='0xffffffff'>%s</t>", pTalentData->getName());
		strcat(buff, tempbuffer);
	}
	if (sTalentLevel)
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t h = '2' c='0xffffffff'>%s%d%s</t><b/>", "等级:", sTalentLevel,"级");
		strcat(buff, tempbuffer);
	}
	if (sTalentText && sTalentText[0])
	{
		dSprintf(tempbuffer, sizeof(tempbuffer), "<t h = '2' c='0xffffffff'>%s</t>", sTalentText);
		strcat(buff, tempbuffer);
	}
	dSprintf(tempbuffer, sizeof(tempbuffer), "<b/><b/><b/><b/><b/><b/>");
	strcat(buff, tempbuffer);

	return true;
}
#endif