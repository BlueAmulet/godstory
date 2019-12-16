#ifndef __BATCHOPERATOR_H__
#define __BATCHOPERATOR_H__
#include "core/tVector.h"
class Player;
class BatchOperator;

//////////////////////////////////////////////////////////////////////////////////
//			����������¼���	BatchBase										//
//////////////////////////////////////////////////////////////////////////////////
class BatchBase
{
	friend class BatchOperator;
public:	
	// �ɹ�����ȷ�Ϸ���
	virtual bool Commit() = 0;
	// ʧ�ܲ������˷���
	virtual bool Rollback() = 0;

protected:
	Player* mSource;		// Դ��Ҷ���
	Player* mTarget;		// Ŀ����Ҷ���

private:
	void Set(Player* s, Player* t) { mSource = s; mTarget = t;}
	U32 mEventID;
};


//////////////////////////////////////////////////////////////////////////////////
//			���������������	BatchOperator									//
//////////////////////////////////////////////////////////////////////////////////
class BatchOperator
{
public:
	BatchOperator(Player* source, Player* target);
	~BatchOperator();

	void Add(BatchBase* ev);
	bool Exec();

private:
	Vector<BatchBase*> mEventList;		// �����¼��б�
	U32 mIDCount;						// �����¼�ID������
	Player* mSource;					// Դ��Ҷ���
	Player* mTarget;					// Ŀ����Ҷ���
};

//////////////////////////////////////////////////////////////////////////////////
//			ʾ��:��Ǯ������	MoneyBatch											//
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
//			ʾ��:��Ʒ��������Ʒ������	InventoryItemBatch						//
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