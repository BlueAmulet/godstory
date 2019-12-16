//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef SUPERMARKET_MANAGER_H
#define SUPERMARKET_MANAGER_H

#define  SUB(a,b)  a * 100 + b
#define  GETSUB(a) a%100
//�̳ǹ�����
enum enMarketType
{
	MarketType_ImortalStone = 1,		//��ʯ�̳�
	MarketType_DeityStone,				//��ʯ�̳�
};

enum enMarketResType
{
	MARKETCATEGORY_HOTSELL		= 1,				//������
	MARKETCATEGORY_FASTION		= 2,				//ʱװ��
	MARKETCATEGORY_HORSE		= 3,				//�����
	MARKETCATEGORY_PET			= 4,				//������
	MARKETCATEGORY_MEDICINE		= 5,				//ҩ����
	MARKETCATEGORY_BOOK			= 6,				//�ؾ���
	MARKETCATEGORY_GEM			= 7,				//�鱦
	MARKETCATEGORY_MATERIAL		= 8,				//������
	MARKETCATEGORY_COSTFUL		= 9,				//�����

	MARKETCATEGORY_SPECIAL			= SUB(MARKETCATEGORY_HOTSELL, 1),	//�ؼ�
	MARKETCATEGORY_HOT				= SUB(MARKETCATEGORY_HOTSELL, 2),	//����

	MARKETCATEGORY_FAMILY			= SUB(MARKETCATEGORY_FASTION, 1),	//����ʱװ
	MARKETCATEGORY_COMMON			= SUB(MARKETCATEGORY_FASTION, 2),	//��ͨʱװ
	MARKETCATEGORY_FASTIONMATERIAL	= SUB(MARKETCATEGORY_FASTION, 3),	//ʱװ����
	MARKETCATEGORY_FATIONBOOK		= SUB(MARKETCATEGORY_FASTION, 4),	//ʱװ�䷽

	MARKETCATEGORY_HORSE_FAMILY		= SUB(MARKETCATEGORY_HORSE, 1),		//��������
	MARKETCATEGORY_HORSE_COMMON		= SUB(MARKETCATEGORY_HORSE, 2),		//��ͨ����
	MARKETCATEGORY_HORSE_BOOK		= SUB(MARKETCATEGORY_HORSE, 3),		//���＼����

	MARKETCATEGORY_PET_SEAL			= SUB(MARKETCATEGORY_PET, 1),		//���޷�ӡ
	MARKETCATEGORY_PET_SKILLBOOK	= SUB(MARKETCATEGORY_PET, 2),		//���޼�����
	MARKETCATEGORY_PET_FOOD			= SUB(MARKETCATEGORY_PET, 3),		//����ʳ��
	MARKETCATEGORY_PET_DRUG			= SUB(MARKETCATEGORY_PET, 4),		//����ҩƷ
	MARKETCATEGORY_PET_REBACK		= SUB(MARKETCATEGORY_PET, 5),		//����ϴ��

	MARKETCATEGORY_DRUG_STATE		= SUB(MARKETCATEGORY_MEDICINE, 1),	//״̬ҩ
	MARKETCATEGORY_DRUG_CAPACITY	= SUB(MARKETCATEGORY_MEDICINE, 2),	//����ҩ

	MARKETCATEGORY_BOOK_SKILL		= SUB(MARKETCATEGORY_BOOK, 1),		//������	
	MARKETCATEGORY_BOOK_FOUND		= SUB(MARKETCATEGORY_BOOK, 2),		//�����䷽
	MARKETCATEGORY_BOOK_SEWING		= SUB(MARKETCATEGORY_BOOK, 3),		//�÷��䷽
	MARKETCATEGORY_BOOK_ART			= SUB(MARKETCATEGORY_BOOK, 4),		//�����䷽

	MARKETCATEGORY_GEM_STONE		= SUB(MARKETCATEGORY_GEM, 1),		//��ʯ

	MARKETCATEGORY_MATERIAL_IDETIFY	= SUB(MARKETCATEGORY_MATERIAL, 1),	//��������
	MARKETCATEGORY_MATERIAL_STRONG	= SUB(MARKETCATEGORY_MATERIAL, 2),	//ǿ������
	MARKETCATEGORY_MATERIAL_BESET	= SUB(MARKETCATEGORY_MATERIAL, 3),	//��Ƕ����
	MARKETCATEGORY_MATERIAL_LIFE	= SUB(MARKETCATEGORY_MATERIAL, 4),	//�������

	MARKETCATEGORY_COSTFUL_EXP		= SUB(MARKETCATEGORY_COSTFUL, 1),	//���鵤
	MARKETCATEGORY_COSTFUL_BODY		= SUB(MARKETCATEGORY_COSTFUL, 2),	//����
	MARKETCATEGORY_COSTFUL_FIRE		= SUB(MARKETCATEGORY_COSTFUL, 3),	//�̻�
	MARKETCATEGORY_COSTFUL_FLOWER	= SUB(MARKETCATEGORY_COSTFUL, 4),	//�ʻ�
	MARKETCATEGORY_COSTFUL_OTHER	= SUB(MARKETCATEGORY_COSTFUL, 5),	//����

};

//��������ݽṹ
struct stPromotion
{
	U32 id;				//��ˮ��
	U32 begindate;		//��ʼ����ʱ��
	U32 enddate;		//��������ʱ��
	U32 mode;			//����ģʽ(0-�� 1-��ʯ 2-��ʯ)
	U32 money;			//��������
	U32 condition[2][2];//�ж�����
	U32 reward[4][2];	//������Ʒ
};

//�ۿ����ݽṹ
struct stDiscount
{
	U32 id;				//��ˮ��
	U32 goodid;			//��Ʒ���
	U32 type;			//0-�ۿ� 1-�ؼ�
	U32 value;			//�ۿ��ʻ��ؼ۶�ֵ
	U32 begindate;		//��ʼ����ʱ��
	U32 enddate;		//��������ʱ��
	S32	isoffsell;		//�Ƿ�������¼�
	S32 goodcount;		//�ϼ�����(-1Ϊ������)
	U32 reward[2][2];	//������Ʒ
};

//���׼�¼���ݽṹ
struct stMarketTrade
{
	U32 id;				//��ˮ��
	U32 tradeid;		//�������κ�
	U32 accountid;		//�ʺ�ID
	U32 playerid;		//��ɫID
	U8	playername[32];//��ɫ����
	U32 mode;			//����ģʽ(0-�� 1-��ʯ 2-��ʯ)
	U32 money;			//��ʯ��ֵ����ʯ��ֵ
	U32 goodid;			//��Ʒ���
	U32 goodnum;		//��Ʒ����
	U32 promotionid;	//�����������ˮ��
	U32 discountid;		//�ۿ�������ˮ��
	U32 tradedate;		//���׷���ʱ��
};

class SuperMarketRes;
class Player;
class SuperMarketManager
{
public:
	SuperMarketManager();
	~SuperMarketManager();
	//���̳ǽ���
	void openSuperMarket(Player* player);
	
#ifdef NTJ_CLIENT
	void filterMarketGoods();
	U32 getTempListSize() { return mTempGoodsList.size(); }
	U32 getMarketGoodID(S32 index);
	SuperMarketRes* getMarketRes(U32 id);
	S32 getSuperMarketData(Player* player, S32 page);
	void setMarkettype (U32 type) { mMarketType = type; }
	void showMarketItemInfo(Player* pPlayer);
	void setMarketBigType(U32 type) { mBigType = type; }
	void setMarketSmallType(U32 type) { mSmallType = type; }
	U32  getCurrentPage() { return mCurrentPage; }
	void setCurrentPage(U32 page) { mCurrentPage = page; }
	U32  getAllPage() { return mAllPage; }
	void setAllPage(U32 page) { mAllPage = page; }
	U32 getMaketType()	{ return mMarketType; }
	void findMarketItem(StringTableEntry name);
	void showFindMarketItem(U32 type);
	void addItemToBasketballList(U32 goodsId, U32 num);
	void showBasketballListInfo();	
	void setBasketListSlot(Player* pPlayer);
	void showBasketItemInfo(Player* pPlayer);
#endif

private:
	U32 mMarketType;		// �̳�����
	U32	mBigType;			// ��Ʒ����
	U32 mSmallType;			// ��ƷС��
	U32 mCurrentPage;
	S32 mAllPage;
public:
	Vector<U32> mTempGoodsList;			//������ʾ��Ʒ
	Vector<U32> mFindImortalStoneList;	//��ʯ����
	Vector<U32> mFindDeityStoneList;	//��ʯ����
	Vector<U32> mImortalBasketballList;	//��ʯ������
	Vector<U32> mDeityBasketballList;	//��ʯ������
	Vector<U32>	mImortalStoneNum;		//������ʯ����
	Vector<U32> mDeityStoneNum;		//������ʯ����
};

extern SuperMarketManager* g_MarketManager;
#endif