#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/BatchOperator.h"
#include "Gameplay/item/Player_Item.h"

//////////////////////////////////////////////////////////////////////////////////
//			���������������	BatchOperator									//
//////////////////////////////////////////////////////////////////////////////////

BatchOperator::BatchOperator(Player* source, Player* target)
{ 
	mIDCount	= 0;
	mSource		= source;
	mTarget		= target;
}

BatchOperator::~BatchOperator()
{
	for(S32 i = 0; i < mEventList.size(); ++i)
	{
		if(mEventList[i]) delete mEventList[i];
	}
	mEventList.clear();
}

// ----------------------------------------------------------------------------
// ��Ӳ����¼�
void BatchOperator::Add(BatchBase* ev)
{
	ev->mEventID = mIDCount++;
	ev->mSource = mSource;
	ev->mTarget = mTarget;
	mEventList.push_back(ev);
}

// ----------------------------------------------------------------------------
// ִ�����������
bool BatchOperator::Exec()
{
	S32 found = 0;
	for(S32 i = 0; i < mEventList.size(); ++i)
	{
		if(mEventList[i] && mEventList[i]->Commit() == false)
		{
			found = mEventList[i]->mEventID;
			break;
		}
	}

	if(found == 0)
		return true;

	for(S32 i = mEventList.size() - 1; i >= 0; --i)
	{
		if(mEventList[i] && mEventList[i]->mEventID <= found )
		{
			bool ret = mEventList[i]->Rollback();
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////
//			ʾ��:��Ǯ������	MoneyBatch											//
//////////////////////////////////////////////////////////////////////////////////
bool MoneyBatch::Commit()
{
	bool ret1,ret2;
	ret1 = ret2 = true;
	if(mSource)
		ret1 = mSource->reduceMoney(money);
	if(mTarget)
		ret2 = mTarget->addMoney(money);
	return ret1 && ret2;
}

bool MoneyBatch::Rollback()
{
	bool ret1,ret2;
	ret1 = ret2 = true;
	if(mSource)
		ret1 = mSource->addMoney(money);
	if(mTarget)
		ret2 = mTarget->reduceMoney(money);
	return ret1 && ret2;
}

//////////////////////////////////////////////////////////////////////////////////
//			ʾ��:��Ʒ��������Ʒ������	InventoryItemBatch						//
//////////////////////////////////////////////////////////////////////////////////
bool InventoryItemBatch::Commit()
{
#ifdef NTJ_SERVER
	bool ret1,ret2;
	ret1 = ret2 = true;
	S32 index;
	if(mSource)
		ret1 = g_ItemManager->addItemToInventory(mSource, pItem, index) == MSG_NONE;
	if(mTarget)
		ret2 = g_ItemManager->delItemFromInventory(mTarget, pItem) == MSG_NONE;
	return ret1 && ret2;
#endif//NTJ_SERVER
	return true;
}

bool InventoryItemBatch::Rollback()
{
#ifdef NTJ_SERVER
	bool ret1,ret2;
	ret1 = ret2 = true;
	S32 index;
	if(mSource)
		ret1 = g_ItemManager->delItemFromInventory(mSource, pItem) == MSG_NONE;
	if(mTarget)
		ret2 = g_ItemManager->addItemToInventory(mTarget, pItem, index) == MSG_NONE;
	return ret1 && ret2;
#endif//NTJ_SERVER
	return true;
}