#ifndef __BATCHOPERATOR_H__
#define __BATCHOPERATOR_H__
#include "core/tVector.h"
class Player;
class BatchOperator;

//////////////////////////////////////////////////////////////////////////////////
//			批处理操作事件类	BatchBase										//
//////////////////////////////////////////////////////////////////////////////////
class BatchBase
{
	friend class BatchOperator;
public:	
	// 成功操作确认方法
	virtual bool Commit() = 0;
	// 失败操作回退方法
	virtual bool Rollback() = 0;

protected:
	Player* mSource;		// 源玩家对象
	Player* mTarget;		// 目标玩家对象

private:
	void Set(Player* s, Player* t) { mSource = s; mTarget = t;}
	U32 mEventID;
};


//////////////////////////////////////////////////////////////////////////////////
//			批处理操作处理类	BatchOperator									//
//////////////////////////////////////////////////////////////////////////////////
class BatchOperator
{
public:
	BatchOperator(Player* source, Player* target);
	~BatchOperator();

	void Add(BatchBase* ev);
	bool Exec();

private:
	Vector<BatchBase*> mEventList;		// 操作事件列表
	U32 mIDCount;						// 操作事件ID计数器
	Player* mSource;					// 源玩家对象
	Player* mTarget;					// 目标玩家对象
};

//////////////////////////////////////////////////////////////////////////////////
//			示例:金钱操作类	MoneyBatch											//
//////////////////////////////////////////////////////////////////////////////////
class MoneyBatch : public BatchBase
{
public:
	bool Commit();
	bool Rollback();
public:
	S32  money;
};

//////////////////////////////////////////////////////////////////////////////////
//			示例:物品栏增减物品操作类	InventoryItemBatch						//
//////////////////////////////////////////////////////////////////////////////////
class InventoryItemBatch : public BatchBase
{
public:
	bool Commit();
	bool Rollback();
public:
	ItemShortcut* pItem;
};

//example:
//MoneyBatch* mba = new MoneyBatch;
//mba->money = 5000;
//InventoryItemBatch* iba = new InventoryItemBatch;
//iba->pItem = pTempItem;
//BatchOperator bo(player, NULL);
//bo.Add(mba);
//bo.Add(iba);
//bool ret = bo.Exec();
#endif//__BATCHOPERATOR_H__