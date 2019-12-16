#include "Gameplay/Item/Res.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/Item/DropItem.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "sim/netConnection.h"
#include "sim/netObject.h"
#include "ui/dGuiShortCut.h"
#include "gui/controls/guiMLTextCtrl.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "Gameplay/Item/Player_EquipIdentify.h"

void PickupList::Clear()
{
	S32 size = mSlots.size();

	for(U32 i = 0; i < size; i++)
	{
		if (mSlots[i] != NULL)
		{
			delete mSlots[i];			
		}
	}

	mSlots.clear();
	mCurrentPage = 0;
}

void PickupList::ClearList(S32 count, S32 mask)
{
	int deleteCount = 0;
	S32 size = mSlots.size();

	for (S32 i = 0; i < size; i++)
	{
		if (deleteCount >= count)
		{
			break;
		}
		if ( mask & (1 << i) )
		{
			DeleteSlot(i);
			deleteCount++;
		}
	}

	UpdateList();
}

void PickupList::DeleteSlot(U32 nIndex)
{
	delete mSlots[nIndex];
	mSlots[nIndex] = NULL;
}

void PickupList::UpdateList()
{
	Vector<ItemShortcut *>::iterator it = mSlots.begin();

	while(it != mSlots.end())
	{
		if (*it == NULL)
		{
			mSlots.erase(it);
		}
		else
		{
			it++;
		}		
	}
}

S32	PickupList::FindItemByID(U32 nItemID)
{
	S32 nIndex = 0;

	for (; nIndex < mSlots.size(); nIndex++)
	{
		if (mSlots[nIndex]->getRes()->getItemID() == nItemID)
		{
			return nIndex;
		}
	}

	return -1;
}

ShortcutObject* PickupList::GetSlot(S32 nIndex)
{
	if (nIndex < 0 || nIndex >= mSlots.size())
		return NULL;

	return mSlots[nIndex];
}

bool PickupList::EraseSlot(U32 nIndex)
{
	if (nIndex < 0 || nIndex >= mSlots.size())
		return false;

	if (mSlots[nIndex] != NULL)
	{
		delete mSlots[nIndex];
	}

	mSlots.erase(nIndex);
	return true;
}

bool PickupList::AddItem(Player *pPlayer, U32 nItemId)
{
	ItemShortcut *pItemShortcut = NULL;
	Vector<ItemShortcut *>::iterator it = mSlots.begin();

	while(it != mSlots.end())
	{
		if ((*it)->getRes()->getItemID() == nItemId)
		{
			break;
		}
		++it;
	}

	if (it == mSlots.end())
	{
		pItemShortcut = ItemShortcut::CreateItem(nItemId, 1);
		mSlots.push_back(pItemShortcut);
	}
	else
	{
		if ( !(*it)->getRes()->getBaseData()->canLapOver() )
		{
			pItemShortcut = ItemShortcut::CreateItem(nItemId, 1);
			mSlots.push_back(pItemShortcut);
		}
		else
		{
			//可以重叠的物品是不能强化的，此时pItemShortcut为NULL
			S32 quantity = (*it)->getRes()->getQuantity();
			(*it)->getRes()->setQuantity(quantity + 1);
		}
	}

	/////////////强化附加属性生成///////////////////
	if (pItemShortcut)
		pItemShortcut->RandStrengthenExtData();
#ifdef NTJ_SERVER
	if (pItemShortcut->getRes()->getIdentifyType() == Res::IDENTIFYTYPE_DROP && !pItemShortcut->getRes()->IsActivatePro(EAPF_ATTACH))
	{
		gIdentifyManager->setStatsID(pPlayer, pItemShortcut, 0);
	}
#endif

	return true;
}

S32 PickupList::GetTotalPage()
{
	S32 size = mSlots.size();
	return (size % NUM_ITEMS_PER_PAGE) ? size / NUM_ITEMS_PER_PAGE + 1 : size / NUM_ITEMS_PER_PAGE;
}

bool PickupList::UpdateToClient(GameConnection* conn, S32 index, U32 flag)
{
	return true;
}

bool PickupList::SetCurrentPage(S32 pageIndex)
{
	S32 nTotalItemNum = Size();
	S32 nTotalPage = (nTotalItemNum % NUM_ITEMS_PER_PAGE) ? (nTotalItemNum / NUM_ITEMS_PER_PAGE + 1) : (nTotalItemNum / NUM_ITEMS_PER_PAGE);

	if (pageIndex < nTotalPage && pageIndex >= 0)
	{
		mCurrentPage = pageIndex;
		return true;
	}

	return false;
}

bool PickupList::IncCurrentPage()
{
	return SetCurrentPage(mCurrentPage+1);
}

bool PickupList::DecCurrentPage()
{
	return SetCurrentPage(mCurrentPage-1);
}

ItemShortcut* PickupList::GetSlotForGuiControl(S32 nIndex)
{
	S32 nAbsIndex = nIndex + mCurrentPage * NUM_ITEMS_PER_PAGE;

	if (nAbsIndex >= 0 && nAbsIndex < Size())
	{
		return mSlots[nAbsIndex];
	}

	return NULL;
}

StringTableEntry PickupList::GetSlotItemName(S32 nIndex)
{
	S32 nAbsIndex = nIndex + mCurrentPage * NUM_ITEMS_PER_PAGE;

	if (nAbsIndex >= 0 && nAbsIndex < Size())
	{
		return mSlots[nAbsIndex]->getRes()->getItemName();
	}

	return NULL;
}

bool PickupList::CanGetSlotInCurrentPage()
{
	S32 nAbsIndex = mCurrentPage * NUM_ITEMS_PER_PAGE;

	if (nAbsIndex >= 0 && nAbsIndex < Size())
	{
		return true;
	}

	return false;
}

#ifdef NTJ_CLIENT
/*
void ShowPickupItemIconAndText(S32 nStartIndex, S32 nCurrIndex, PickupList *pList)
{
	char iconCtrl[20]; 
	char textCtrl[20];

	dSprintf(iconCtrl, sizeof(iconCtrl), "%s_%d", "PickUpItemIcon", nCurrIndex+1);
	dSprintf(textCtrl, sizeof(textCtrl), "%s_%d", "PickUpItemText", nCurrIndex+1);

	dGuiShortCut *pShortcutCtrl = dynamic_cast<dGuiShortCut *>(Sim::findObject(iconCtrl));
	GuiMLTextCtrl *pTextCtrl = dynamic_cast<GuiMLTextCtrl *>(Sim::findObject(textCtrl));

	ItemShortcut *pItemShortcut = NULL;

	U32 nIndex = nStartIndex + nCurrIndex;

	if(pShortcutCtrl == NULL || pTextCtrl == NULL)
		return;

	if (nIndex >= 0 && nIndex < pList->Size())
	{
		pItemShortcut = dynamic_cast<ItemShortcut *>(pList->GetSlot(nIndex));
		pShortcutCtrl->setSlotCol(nIndex);
		const char *text = pItemShortcut->getRes()->getItemName();
		pTextCtrl->setText(text, dStrlen(text));
	}
	else
	{
		// 超出显示范围
		pTextCtrl->setText(" ", 1);
		pShortcutCtrl->setSlotCol(-1);
	}
}

// nPageIndex 从0开始
ConsoleFunction(ShowPickupList, void, 2, 2, "ShowPickupList(nPageIndex);")
{
	S32 nPageIndex = atoi(argv[1]);
	Player *player = dynamic_cast<Player *>(g_ClientGameplayState->GetControlObject());
	if (player == NULL)
		return;

	GuiTextCtrl *pTextCtrl = dynamic_cast<GuiTextCtrl *>(Sim::findObject("ItemPageNum_Display"));

	if (pTextCtrl == NULL)
		return;

	S32 nItemCount = player->m_PickupList.Size();
	S32 nPageNum = 0;	//UI界面上每页4个物品
	if (nItemCount % 4 == 0)
	{
		nPageNum = nItemCount / 4;
	}
	else
	{
		nPageNum = nItemCount / 4 + 1;
	}

	if(nPageIndex < 0 || nPageIndex > nPageNum)
	{
		// 刷新
		return;
	}

	// 显示当前页数和总页数
	char buf[20];

	dSprintf(buf, sizeof(buf), "%d/%d", nPageIndex+1, nPageNum);

	if (nPageNum != 0)
	{
		pTextCtrl->setText(buf);
	}

	// 显示列表
	PickupList *pList = &player->m_PickupList;
	S32 nStartIndex = nPageIndex * 4;

	for(int i = 0; i < 4; i++)
	{
		ShowPickupItemIconAndText(nStartIndex, i, pList);
	}

	GuiControl *pPickupWndGui = dynamic_cast<GuiControl *>(Sim::findObject("PickupItemWndGui"));
	pPickupWndGui->setVisible(true);
}


ConsoleFunction(GoNextPageInPickupList, void, 1, 1, "GoNextPageInPickupList()")
{
	char pageIndexBuf[10];
	Player *player = dynamic_cast<Player *>(g_ClientGameplayState->GetControlObject());
	if (player == NULL)
		return;

	S32 nTotalPage = 0;
	S32 nItemCount = player->m_PickupList.Size();
	nTotalPage = (nItemCount % 4 == 0) ? (nItemCount / 4) : (nItemCount / 4 + 1);
	S32 nCurrentPage = player->m_PickupList.GetCurrentPage();
	nCurrentPage++;

	if ( nCurrentPage < nTotalPage)
	{
		player->m_PickupList.SetCurrentPage(nCurrentPage);
		dSprintf(pageIndexBuf, sizeof(pageIndexBuf), "%d", nCurrentPage);		
		Con::executef("ShowPickupList", pageIndexBuf);
	}
}

ConsoleFunction(GoPrevPageInPickupList, void, 1, 1, "GoPrevPageInPickupList()")
{
	char pageIndexBuf[10];
	Player *player = dynamic_cast<Player *>(g_ClientGameplayState->GetControlObject());
	if (player == NULL)
		return;

	S32 nTotalPage = 0;
	S32 nItemCount = player->m_PickupList.Size();
	nTotalPage = (nItemCount % 4 == 0) ? (nItemCount / 4) : (nItemCount / 4 + 1);

	S32 nCurrentPage = player->m_PickupList.GetCurrentPage();
	nCurrentPage--;

	if ( nCurrentPage >= 0)
	{
		player->m_PickupList.SetCurrentPage(nCurrentPage);
		dSprintf(pageIndexBuf, sizeof(pageIndexBuf), "%d", nCurrentPage);
		Con::executef("ShowPickupList", pageIndexBuf);
	}
}
*/
ConsoleFunction(PickupList_GetCurrentPage, S32, 1, 1, "PickupList_GetCurrentPage();")
{
	Player *player = dynamic_cast<Player *>(g_ClientGameplayState->GetControlObject());
	if(player)
	{
		return player->pickupList.GetCurrentPage();
	}
	return 0;
}

ConsoleFunction(PickupList_GetTotalPage, S32, 1, 1, "PickupList_GetTotalPage();")
{
	Player *player = dynamic_cast<Player *>(g_ClientGameplayState->GetControlObject());
	if(player)
	{
		return player->pickupList.GetTotalPage();
	}
	return 0;
}

ConsoleFunction(PickupList_IncPageIndex, bool, 1, 1, "PickupList_IncPageIndex();")
{
	Player *player = dynamic_cast<Player *>(g_ClientGameplayState->GetControlObject());

	if (player)
	{
		return player->pickupList.IncCurrentPage();
	}
	return false;
}

ConsoleFunction(PickupList_DecPageIndex, bool, 1, 1, "PickupList_DecPageIndex();")
{
	Player *player = dynamic_cast<Player *>(g_ClientGameplayState->GetControlObject());

	if (player)
	{
		return player->pickupList.DecCurrentPage();
	}

	return false;
}

ConsoleFunction(PickupList_GetItemName, StringTableEntry, 2, 2, "PickupList_GetItemName(nIndex);")
{
	S32 nIndex = dAtoi(argv[1]);
	Player *player = dynamic_cast<Player *>(g_ClientGameplayState->GetControlObject());

	if (player)
	{
		return player->pickupList.GetSlotItemName(nIndex);
	}

	return NULL;
}

ConsoleFunction(PickupList_GetAllItems, void, 1, 1, "PickupList_GetAllItems();")
{
	DropItemEvent::ClientSendPickupItem(0, true);
}

ConsoleFunction(PickupList_GetSingleItem, void, 2, 2, "PickupList_GetSingleItem(nIndex);")
{
	int nIndex = atoi(argv[1]);

	DropItemEvent::ClientSendPickupItem(nIndex, false);
}

ConsoleFunction(PickupList_CloseDropList, void, 1, 1, "PickupList_Close();")
{
	DropItemEvent::ClientSendCloseDropItemList();
	Con::executef("ClosePickupItemWnd");
}
#endif