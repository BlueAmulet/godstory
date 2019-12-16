#ifdef NTJ_CLIENT
#include "console/consoleTypes.h"
#include "console/console.h"
#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameObjects/PetTable.h"
#include "Gameplay/GameObjects/PetObject.h"
#include "Gameplay/GameObjects/PetOperationManager.h"

PetHelpList::PetHelpList()
{
	
}

PetHelpList::~PetHelpList()
{
	Clear();
}

void PetHelpList::Clear()
{
	for (S32 i = 0; i < mPetHelpList.size(); i++)
	{
		if (mPetHelpList[i])
		{
			delete mPetHelpList[i];
			mPetHelpList[i] = NULL;
		}
	}
	mPetHelpList.clear();
}

void PetHelpList::AppendSlot(PetShortcut *pSlot)
{
	if (pSlot)
		mPetHelpList.push_back(pSlot);
}

void PetHelpList::AppendSlot(S32 nPetSlotIndex)
{
	if (nPetSlotIndex >= 0 && nPetSlotIndex < PET_MAXSLOTS)
	{
		PetShortcut *pSlot = PetShortcut::CreatePetItem(nPetSlotIndex);
		if (pSlot)
			mPetHelpList.push_back(pSlot);
	}
}

PetShortcut *PetHelpList::GetSlot(S32 index)
{
	S32 size = mPetHelpList.size();
	if (index < 0 || index >= size)
		return NULL;
	return mPetHelpList[index];
}

void PetHelpList::Initialize(Opt_Type nOptType)
{
	mPlayer = g_ClientGameplayState->GetControlPlayer();
	for (S32 i = 0; i < PET_MAXSLOTS; i++)
	{
		stPetInfo *pPetInfo = (stPetInfo*)mPlayer->getPetTable().getPetInfo(i);
		if (!pPetInfo || pPetInfo->status != PetObject::PetStatus_Idle)
			continue;
		switch(nOptType)
		{
		case Opt_JianDing:
			{
				if (pPetInfo->chengZhangLv != 0)
					continue;
			}
			break;
		case Opt_HuiGeng:
			{
				if (pPetInfo->insight >= 10)
					continue;
			}
			break;
		case Opt_LianHua:
			{
				if (pPetInfo->level < 10)
					continue;
			}
			break;
		case Opt_HuanTong:
			{
				if (pPetInfo->level < 10 || (pPetInfo->style != 1 && pPetInfo->style != 2))
					continue;				
			}
			break;
		default:
			continue;
			break;
		}

		AppendSlot(i);
	}

	S32 size = mPetHelpList.size();
	Con::executef("RefreshPetHelpWnd", Con::getIntArg(size));
}

ConsoleFunction(PetHelpList_DeleteEntry, void, 2, 2, "PetHelpList_DeleteEntry(%nIndex);")
{
	S32 nIndex = atoi( argv[1] );
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	return;
}

ConsoleFunction(PetHelpList_Refresh, void, 2, 2, "PetHelpList_Refresh(%nOptType);")
{
	S32 iOptType = atoi(argv[1]);
	if (iOptType <= PetHelpList::Opt_None || iOptType >= PetHelpList::Opt_Max)
		return;
	PetHelpList::Opt_Type nOptType = (PetHelpList::Opt_Type)iOptType;
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	pPlayer->mPetHelpList.Initialize(nOptType);
}

ConsoleFunction(PetHelpList_Clear, void, 1, 1, "PetHelpList_Clear();")
{
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	GuiControl *pPetScroll_Floor = dynamic_cast<GuiControl*>(Sim::findObject("PetScroll_Floor"));
	if (!pPlayer || !pPetScroll_Floor)
		return;
	pPlayer->mPetHelpList.Clear();
	pPetScroll_Floor->clear();
}

ConsoleFunction(PetHelpList_SelectListID, void, 2, 2, "PetHelpWnd_SelectListID(%index);")
{
	S32 iIndex = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return;
	PetShortcut *pPetShortcut = pPlayer->mPetHelpList.GetSlot(iIndex);	
	if (!pPetShortcut)
		return;
	
	PetOperationManager::ClientSendPetSlotExchangeMsg(pPlayer, pPetShortcut->getSlotIndex(), SHORTCUTTYPE_TEMP, 0);
}

ConsoleFunction(PetHelpList_GetPetName, StringTableEntry, 2, 2, "PetHelpList_GetPetName(%index);")
{
	S32 iIndex = atoi(argv[1]);
	Player *pPlayer = g_ClientGameplayState->GetControlPlayer();
	if (!pPlayer)
		return NULL;
	PetShortcut *pPetShortcut = pPlayer->mPetHelpList.GetSlot(iIndex);	
	if (!pPetShortcut)
		return NULL;
	stPetInfo *pPetInfo = (stPetInfo *)pPlayer->getPetTable().getPetInfo(pPetShortcut->getSlotIndex());
	if (!pPetInfo)
		return NULL;
	
	return pPetInfo->name;
}
#endif